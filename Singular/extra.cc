/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
*/

#define HAVE_WALK 1

#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include <kernel/mod2.h>
#include <misc/auxiliary.h>

#include <factory/factory.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#ifdef TIME_WITH_SYS_TIME
# include <time.h>
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# endif
#else
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# else
#   include <time.h>
# endif
#endif
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif

#include <unistd.h>

#include <misc/options.h>

// #include <coeffs/ffields.h>
#include <coeffs/coeffs.h>
#include <coeffs/mpr_complex.h>
#include "coeffs/AE.h"
#include "coeffs/OPAE.h"
#include "coeffs/AEp.h"
#include "coeffs/OPAEp.h"
#include "coeffs/AEQ.h"
#include "coeffs/OPAEQ.h"


#include <polys/monomials/ring.h>
#include <kernel/polys.h>

#include <polys/monomials/maps.h>
#include <polys/matpol.h>

// #include <kernel/longalg.h>
#include <polys/prCopy.h>
#include <polys/weight.h>


#include <kernel/fast_mult.h>
#include <kernel/digitech.h>
#include <kernel/stairc.h>
#include <kernel/febase.h>
#include <kernel/ideals.h>
#include <kernel/kstd1.h>
#include <kernel/syz.h>
#include <kernel/kutil.h>

#include <kernel/shiftgb.h>
#include <kernel/linearAlgebra.h>

#include <kernel/hutil.h>

// for tests of t-rep-GB
#include <kernel/tgb.h>

#include <kernel/minpoly.h>

#include "tok.h"
#include "ipid.h"
#include "lists.h"
#include "cntrlc.h"
#include "ipshell.h"
#include "sdb.h"
#include "feOpt.h"
#include "fehelp.h"
#include "distrib.h"

#include "misc_ip.h"

#include "attrib.h"

#include "links/silink.h"
#include "walk.h"
#include <Singular/newstruct.h>
#include <Singular/blackbox.h>
#include <Singular/pyobject_setup.h>


#ifdef HAVE_RINGS
#include <kernel/ringgb.h>
#endif

#ifdef HAVE_F5
#include <kernel/f5gb.h>
#endif

#ifdef HAVE_WALK
#include "walk.h"
#endif


#ifdef HAVE_SPECTRUM
#include <kernel/spectrum.h>
#endif

#ifdef HAVE_PLURAL
#include <polys/nc/nc.h>
#include <polys/nc/ncSAMult.h> // for CMultiplier etc classes
#include <polys/nc/sca.h>
#include <kernel/nc.h>
#include "ipconv.h"
#ifdef HAVE_RATGRING
#include <kernel/ratgring.h>
#endif
#endif

#ifdef ix86_Win /* only for the DLLTest */
/* #include "WinDllTest.h" */
#ifdef HAVE_DL
#include <polys/mod_raw.h>
#endif
#endif


// Define to enable many more system commands
#undef MAKE_DISTRIBUTION
#ifndef MAKE_DISTRIBUTION
#define HAVE_EXTENDED_SYSTEM 1
#endif

#include <polys/clapconv.h>
#include <kernel/kstdfac.h>

#include <polys/clapsing.h>

#ifdef HAVE_EIGENVAL
#include "eigenval_ip.h"
#endif

#ifdef HAVE_GMS
#include "gms.h"
#endif

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif

/*
 *   New function/system-calls that will be included as dynamic module
 * should be inserted here.
 * - without HAVE_DYNAMIC_LOADING: these functions comes as system("....");
 * - with    HAVE_DYNAMIC_LOADING: these functions are loaded as module.
 */
//#ifndef HAVE_DYNAMIC_LOADING

#ifdef HAVE_PCV
#include "pcv.h"
#endif

//#endif /* not HAVE_DYNAMIC_LOADING */

#ifdef ix86_Win
//#include <Python.h>
//#include <python_wrapper.h>
#endif

#ifndef MAKE_DISTRIBUTION
static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h);
#endif

#ifdef ix86_Win  /* PySingular initialized? */
static int PyInitialized = 0;
#endif

/* expects a SINGULAR square matrix with number entries
   where currRing is expected to be over some field F_p;
   returns a long** matrix with the "same", i.e.,
   appropriately mapped entries;
   leaves singularMatrix unmodified */
unsigned long** singularMatrixToLongMatrix(matrix singularMatrix)
{
  int n = singularMatrix->rows();
  assume(n == singularMatrix->cols());
  unsigned long **longMatrix = 0;
  longMatrix = new unsigned long *[n] ;
  for (int i = 0 ; i < n; i++)
    longMatrix[i] = new unsigned long [n];
  number entry;
  for (int r = 0; r < n; r++)
    for (int c = 0; c < n; c++)
    {
      poly p=MATELEM(singularMatrix, r + 1, c + 1);
      int entryAsInt;
      if (p!=NULL)
      {
        entry = p_GetCoeff(p, currRing);
        entryAsInt = n_Int(entry, currRing->cf);
        if (entryAsInt < 0) entryAsInt += n_GetChar(currRing->cf);
      }
      else
        entryAsInt=0;
      longMatrix[r][c] = (unsigned long)entryAsInt;
    }
  return longMatrix;
}

/* expects an array of unsigned longs with valid indices 0..degree;
   returns the following poly, where x denotes the first ring variable
   of currRing, and d = degree:
      polyCoeffs[d] * x^d + polyCoeffs[d-1] * x^(d-1) + ... + polyCoeffs[0]
   leaves polyCoeffs unmodified */
poly longCoeffsToSingularPoly(unsigned long *polyCoeffs, const int degree)
{
  poly result = NULL;
  for (int i = 0; i <= degree; i++)
  {
    if ((int)polyCoeffs[i] != 0)
    {
      poly term = p_ISet((int)polyCoeffs[i], currRing);
      if (i > 0)
      {
        p_SetExp(term, 1, i, currRing);
        p_Setm(term, currRing);
      }
      result = p_Add_q(result, term, currRing);
    }
  }
  return result;
}

//void emStart();
/*2
*  the "system" command
*/
BOOLEAN jjSYSTEM(leftv res, leftv args)
{
  if(args->Typ() == STRING_CMD)
  {
    const char *sys_cmd=(char *)(args->Data());
    leftv h=args->next;
// ONLY documented system calls go here
// Undocumented system calls go down into jjEXTENDED_SYSTEM (#ifdef HAVE_EXTENDED_SYSTEM)
/*==================== nblocks ==================================*/
    if (strcmp(sys_cmd, "nblocks") == 0)
    {
      ring r;
      if (h == NULL)
      {
        if (currRingHdl != NULL)
        {
          r = IDRING(currRingHdl);
        }
        else
        {
          WerrorS("no ring active");
          return TRUE;
        }
      }
      else
      {
        if (h->Typ() != RING_CMD)
        {
          WerrorS("ring expected");
          return TRUE;
        }
        r = (ring) h->Data();
      }
      res->rtyp = INT_CMD;
      res->data = (void*) (long)(rBlocks(r) - 1);
      return FALSE;
    }
/*==================== version ==================================*/
    if(strcmp(sys_cmd,"version")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)SINGULAR_VERSION;
      return FALSE;
    }
    else
/*==================== cpu ==================================*/
    if(strcmp(sys_cmd,"cpu")==0)
    {
      long cpu=1; //feOptValue(FE_OPT_CPUS);
      #ifdef _SC_NPROCESSORS_ONLN
      cpu=sysconf(_SC_NPROCESSORS_ONLN);
      #elif defined(_SC_NPROCESSORS_CONF)
      cpu=sysconf(_SC_NPROCESSORS_CONF);
      #endif
      res->data=(void *)cpu;
      res->rtyp=INT_CMD;
      return FALSE;
    }
    else




/*==================== gen ==================================*/
// // This seems to be obsolette...?!
// // TODO: cleanup doc/reference.doc:6998 to system("gen")
//     if(strcmp(sys_cmd,"gen")==0)
//     {
//       res->rtyp=INT_CMD;
//       res->data=(void *)(long)npGen;
//       return FALSE;
//     }
//     else
/*==================== sh ==================================*/
    if(strcmp(sys_cmd,"sh")==0)
    {
      if (feOptValue(FE_OPT_NO_SHELL)) {
       WerrorS("shell execution is disallowed in restricted mode");
       return TRUE;
       }
      res->rtyp=INT_CMD;
      if (h==NULL) res->data = (void *)(long) system("sh");
      else if (h->Typ()==STRING_CMD)
        res->data = (void*)(long) system((char*)(h->Data()));
      else
        WerrorS("string expected");
      return FALSE;
    }
    else
    #if 0
    if(strcmp(sys_cmd,"power1")==0)
    {
      res->rtyp=POLY_CMD;
      poly f=(poly)h->CopyD();
      poly g=pPower(f,2000);
      res->data=(void *)g;
      return FALSE;
    }
    else
    if(strcmp(sys_cmd,"power2")==0)
    {
      res->rtyp=POLY_CMD;
      poly f=(poly)h->Data();
      poly g=pOne();
      for(int i=0;i<2000;i++)
        g=pMult(g,pCopy(f));
      res->data=(void *)g;
      return FALSE;
    }
    if(strcmp(sys_cmd,"power3")==0)
    {
      res->rtyp=POLY_CMD;
      poly f=(poly)h->Data();
      poly p2=pMult(pCopy(f),pCopy(f));
      poly p4=pMult(pCopy(p2),pCopy(p2));
      poly p8=pMult(pCopy(p4),pCopy(p4));
      poly p16=pMult(pCopy(p8),pCopy(p8));
      poly p32=pMult(pCopy(p16),pCopy(p16));
      poly p64=pMult(pCopy(p32),pCopy(p32));
      poly p128=pMult(pCopy(p64),pCopy(p64));
      poly p256=pMult(pCopy(p128),pCopy(p128));
      poly p512=pMult(pCopy(p256),pCopy(p256));
      poly p1024=pMult(pCopy(p512),pCopy(p512));
      poly p1536=pMult(p1024,p512);
      poly p1792=pMult(p1536,p256);
      poly p1920=pMult(p1792,p128);
      poly p1984=pMult(p1920,p64);
      poly p2000=pMult(p1984,p16);
      res->data=(void *)p2000;
      pDelete(&p2);
      pDelete(&p4);
      pDelete(&p8);
      //pDelete(&p16);
      pDelete(&p32);
      //pDelete(&p64);
      //pDelete(&p128);
      //pDelete(&p256);
      //pDelete(&p512);
      //pDelete(&p1024);
      //pDelete(&p1536);
      //pDelete(&p1792);
      //pDelete(&p1920);
      //pDelete(&p1984);
      return FALSE;
    }
    else
    #endif
/*==================== uname ==================================*/
    if(strcmp(sys_cmd,"uname")==0)
    {
      res->rtyp=STRING_CMD;
      res->data = omStrDup(S_UNAME);
      return FALSE;
    }
    else
/*==================== with ==================================*/
    if(strcmp(sys_cmd,"with")==0)
    {
      if (h==NULL)
      {
        res->rtyp=STRING_CMD;
        res->data=(void *)versionString();
        return FALSE;
      }
      else if (h->Typ()==STRING_CMD)
      {
          #define TEST_FOR(A) if(strcmp(s,A)==0) res->data=(void *)1; else
          char *s=(char *)h->Data();
          res->rtyp=INT_CMD;
          #ifdef HAVE_DBM
            TEST_FOR("DBM")
          #endif
          #ifdef HAVE_DLD
            TEST_FOR("DLD")
          #endif
            //TEST_FOR("factory")
            //TEST_FOR("libfac")
          #ifdef HAVE_READLINE
            TEST_FOR("readline")
          #endif
          #ifdef TEST_MAC_ORDER
            TEST_FOR("MAC_ORDER")
          #endif
          // unconditional since 3-1-0-6
            TEST_FOR("Namespaces")
          #ifdef HAVE_DYNAMIC_LOADING
            TEST_FOR("DynamicLoading")
          #endif
          #ifdef HAVE_EIGENVAL
            TEST_FOR("eigenval")
          #endif
          #ifdef HAVE_GMS
            TEST_FOR("gms")
          #endif
          #ifdef OM_NDEBUG
            TEST_FOR("om_ndebug")
          #endif
          #ifdef SING_NDEBUG
            TEST_FOR("ndebug")
          #endif
            {};
          return FALSE;
          #undef TEST_FOR
        }
        return TRUE;
      }
      else
  /*==================== browsers ==================================*/
      if (strcmp(sys_cmd,"browsers")==0)
      {
        res->rtyp = STRING_CMD;
        StringSetS("");
        feStringAppendBrowsers(0);
        res->data = StringEndS();
        return FALSE;
      }
      else
  /*==================== pid ==================================*/
      if (strcmp(sys_cmd,"pid")==0)
      {
        res->rtyp=INT_CMD;
        res->data=(void *)(long) getpid();
        return FALSE;
      }
      else
  /*==================== getenv ==================================*/
      if (strcmp(sys_cmd,"getenv")==0)
      {
        if ((h!=NULL) && (h->Typ()==STRING_CMD))
        {
          res->rtyp=STRING_CMD;
          const char *r=getenv((char *)h->Data());
          if (r==NULL) r="";
          res->data=(void *)omStrDup(r);
          return FALSE;
        }
        else
        {
          WerrorS("string expected");
          return TRUE;
        }
      }
      else
  /*==================== setenv ==================================*/
      if (strcmp(sys_cmd,"setenv")==0)
      {
  #ifdef HAVE_SETENV
        if (h!=NULL && h->Typ()==STRING_CMD && h->Data() != NULL &&
            h->next != NULL && h->next->Typ() == STRING_CMD
            && h->next->Data() != NULL)
        {
          res->rtyp=STRING_CMD;
          setenv((char *)h->Data(), (char *)h->next->Data(), 1);
          res->data=(void *)omStrDup((char *)h->next->Data());
          feReInitResources();
          return FALSE;
        }
        else
        {
          WerrorS("two strings expected");
          return TRUE;
        }
  #else
        WerrorS("setenv not supported on this platform");
        return TRUE;
  #endif
      }
      else
  /*==================== Singular ==================================*/
      if (strcmp(sys_cmd, "Singular") == 0)
      {
        res->rtyp=STRING_CMD;
        const char *r=feResource("Singular");
        if (r == NULL) r="";
        res->data = (void*) omStrDup( r );
        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "SingularLib") == 0)
      {
        res->rtyp=STRING_CMD;
        const char *r=feResource("SearchPath");
        if (r == NULL) r="";
        res->data = (void*) omStrDup( r );
        return FALSE;
      }
      else
  /*==================== options ==================================*/
      if (strstr(sys_cmd, "--") == sys_cmd)
      {
        if (strcmp(sys_cmd, "--") == 0)
        {
          fePrintOptValues();
          return FALSE;
        }

        feOptIndex opt = feGetOptIndex(&sys_cmd[2]);
        if (opt == FE_OPT_UNDEF)
        {
          Werror("Unknown option %s", sys_cmd);
          Werror("Use 'system(\"--\");' for listing of available options");
          return TRUE;
        }

        // for Untyped Options (help version),
        // setting it just triggers action
        if (feOptSpec[opt].type == feOptUntyped)
        {
          feSetOptValue(opt,0);
          return FALSE;
        }

        if (h == NULL)
        {
          if (feOptSpec[opt].type == feOptString)
          {
            res->rtyp = STRING_CMD;
            const char *r=(const char*)feOptSpec[opt].value;
            if (r == NULL) r="";
            res->data = omStrDup(r);
          }
          else
          {
            res->rtyp = INT_CMD;
            res->data = feOptSpec[opt].value;
          }
          return FALSE;
        }

        if (h->Typ() != STRING_CMD &&
            h->Typ() != INT_CMD)
        {
          Werror("Need string or int argument to set option value");
          return TRUE;
        }
        const char* errormsg;
        if (h->Typ() == INT_CMD)
        {
          if (feOptSpec[opt].type == feOptString)
          {
            Werror("Need string argument to set value of option %s", sys_cmd);
            return TRUE;
          }
          errormsg = feSetOptValue(opt, (int)((long) h->Data()));
          if (errormsg != NULL)
            Werror("Option '--%s=%d' %s", sys_cmd, (int) ((long)h->Data()), errormsg);
        }
        else
        {
          errormsg = feSetOptValue(opt, (char*) h->Data());
          if (errormsg != NULL)
            Werror("Option '--%s=%s' %s", sys_cmd, (char*) h->Data(), errormsg);
        }
        if (errormsg != NULL) return TRUE;
        return FALSE;
      }
      else
  /*==================== HC ==================================*/
      if (strcmp(sys_cmd,"HC")==0)
      {
        res->rtyp=INT_CMD;
        res->data=(void *)(long) HCord;
        return FALSE;
      }
      else
  /*==================== random ==================================*/
      if(strcmp(sys_cmd,"random")==0)
      {
        if ((h!=NULL) &&(h->Typ()==INT_CMD))
        {
          siRandomStart=(int)((long)h->Data());
          siSeed=siRandomStart;
          factoryseed(siRandomStart);
          return FALSE;
        }
        else if (h != NULL)
        {
          WerrorS("int expected");
          return TRUE;
        }
        res->rtyp=INT_CMD;
        res->data=(void*)(long) siRandomStart;
        return FALSE;
      }
  /*==================== complexNearZero ======================*/
      if(strcmp(sys_cmd,"complexNearZero")==0)
      {
        if (h->Typ()==NUMBER_CMD )
        {
          if ( h->next!=NULL && h->next->Typ()==INT_CMD )
          {
            if ( !rField_is_long_C(currRing) )
              {
                Werror( "unsupported ground field!");
                return TRUE;
              }
            else
              {
                res->rtyp=INT_CMD;
                res->data=(void*)complexNearZero((gmp_complex*)h->Data(),
                               (int)((long)(h->next->Data())));
                return FALSE;
              }
          }
          else
          {
            Werror( "expected <int> as third parameter!");
            return TRUE;
          }
        }
        else
        {
          Werror( "expected <number> as second parameter!");
          return TRUE;
        }
      }
  /*==================== getPrecDigits ======================*/
      if(strcmp(sys_cmd,"getPrecDigits")==0)
      {
        if ( !rField_is_long_C(currRing) && !rField_is_long_R(currRing) )
        {
          Werror( "unsupported ground field!");
          return TRUE;
        }
        res->rtyp=INT_CMD;
        res->data=(void*)getGMPFloatDigits();
        return FALSE;
      }
  /*==================== mpz_t loader ======================*/
      if(strcmp(sys_cmd, "GNUmpLoad")==0)
      {
        if ((h != NULL) && (h->Typ() == STRING_CMD))
        {
          char* filename = (char*)h->Data();
          FILE* f = fopen(filename, "r");
          if (f == NULL)
          {
            Werror( "invalid file name (in paths use '/')");
            return FALSE;
          }
          mpz_t m; mpz_init(m);
          mpz_inp_str(m, f, 10);
          fclose(f);
          number n = n_InitMPZ(m, coeffs_BIGINT);
          res->rtyp = BIGINT_CMD;
          res->data = (void*)n;
          return FALSE;
        }
        else
        {
          Werror( "expected valid file name as a string");
          return TRUE;
        }
      }
  /*==================== intvec matching ======================*/
      /* Given two non-empty intvecs, the call
            'system("intvecMatchingSegments", ivec, jvec);'
         computes all occurences of jvec in ivec, i.e., it returns
         a list of int indices k such that ivec[k..size(jvec)+k-1] = jvec.
         If no such k exists (e.g. when ivec is shorter than jvec), an
         intvec with the single entry 0 is being returned. */
      if(strcmp(sys_cmd, "intvecMatchingSegments")==0)
      {
        if ((h       != NULL) && (h->Typ()       == INTVEC_CMD) &&
            (h->next != NULL) && (h->next->Typ() == INTVEC_CMD) &&
            (h->next->next == NULL))
        {
          intvec* ivec = (intvec*)h->Data();
          intvec* jvec = (intvec*)h->next->Data();
          intvec* r = new intvec(1); (*r)[0] = 0;
          int validEntries = 0;
          for (int k = 0; k <= ivec->rows() - jvec->rows(); k++)
          {
            if (memcmp(&(*ivec)[k], &(*jvec)[0],
                       sizeof(int) * jvec->rows()) == 0)
            {
              if (validEntries == 0)
                (*r)[0] = k + 1;
              else
              {
                r->resize(validEntries + 1);
                (*r)[validEntries] = k + 1;
              }
              validEntries++;
            }
          }
          res->rtyp = INTVEC_CMD;
          res->data = (void*)r;
          return FALSE;
        }
        else
        {
          Werror("expected two non-empty intvecs as arguments");
          return TRUE;
        }
      }
      /* Given two non-empty intvecs, the call
            'system("intvecOverlap", ivec, jvec);'
         computes the longest intvec kvec such that ivec ends with kvec
         and jvec starts with kvec. The length of this overlap is being
         returned. If there is no overlap at all, then 0 is being returned. */
      if(strcmp(sys_cmd, "intvecOverlap")==0)
      {
        if ((h       != NULL) && (h->Typ()       == INTVEC_CMD) &&
            (h->next != NULL) && (h->next->Typ() == INTVEC_CMD) &&
            (h->next->next == NULL))
        {
          intvec* ivec = (intvec*)h->Data();
          intvec* jvec = (intvec*)h->next->Data();
          int ir = ivec->rows(); int jr = jvec->rows();
          int r = jr; if (ir < jr) r = ir;   /* r = min{ir, jr} */
          while ((r >= 1) && (memcmp(&(*ivec)[ir - r], &(*jvec)[0],
                                     sizeof(int) * r) != 0))
            r--;
          res->rtyp = INT_CMD;
          res->data = (void*)(long)r;
          return FALSE;
        }
        else
        {
          Werror("expected two non-empty intvecs as arguments");
          return TRUE;
        }
      }
  /*==================== Hensel's lemma ======================*/
      if(strcmp(sys_cmd, "henselfactors")==0)
      {
        if ((h != NULL) && (h->Typ() == INT_CMD) &&
            (h->next != NULL) && (h->next->Typ() == INT_CMD) &&
            (h->next->next != NULL) && (h->next->next->Typ() == POLY_CMD) &&
            (h->next->next->next != NULL) &&
               (h->next->next->next->Typ() == POLY_CMD) &&
            (h->next->next->next->next != NULL) &&
               (h->next->next->next->next->Typ() == POLY_CMD) &&
            (h->next->next->next->next->next != NULL) &&
               (h->next->next->next->next->next->Typ() == INT_CMD) &&
            (h->next->next->next->next->next->next == NULL))
        {
          int xIndex = (int)(long)h->Data();
          int yIndex = (int)(long)h->next->Data();
          poly hh    = (poly)h->next->next->Data();
          poly f0    = (poly)h->next->next->next->Data();
          poly g0    = (poly)h->next->next->next->next->Data();
          int d      = (int)(long)h->next->next->next->next->next->Data();
          poly f; poly g;
          henselFactors(xIndex, yIndex, hh, f0, g0, d, f, g);
          lists L = (lists)omAllocBin(slists_bin);
          L->Init(2);
          L->m[0].rtyp = POLY_CMD; L->m[0].data=(void*)f;
          L->m[1].rtyp = POLY_CMD; L->m[1].data=(void*)g;
          res->rtyp = LIST_CMD;
          res->data = (char *)L;
          return FALSE;
        }
        else
        {
          Werror( "expected argument list (int, int, poly, poly, poly, int)");
          return TRUE;
        }
      }
  /*==================== lduDecomp ======================*/
      if(strcmp(sys_cmd, "lduDecomp")==0)
      {
        if ((h != NULL) && (h->Typ() == MATRIX_CMD) && (h->next == NULL))
        {
          matrix aMat = (matrix)h->Data();
          matrix pMat; matrix lMat; matrix dMat; matrix uMat;
          poly l; poly u; poly prodLU;
          lduDecomp(aMat, pMat, lMat, dMat, uMat, l, u, prodLU);
          lists L = (lists)omAllocBin(slists_bin);
          L->Init(7);
          L->m[0].rtyp = MATRIX_CMD; L->m[0].data=(void*)pMat;
          L->m[1].rtyp = MATRIX_CMD; L->m[1].data=(void*)lMat;
          L->m[2].rtyp = MATRIX_CMD; L->m[2].data=(void*)dMat;
          L->m[3].rtyp = MATRIX_CMD; L->m[3].data=(void*)uMat;
          L->m[4].rtyp = POLY_CMD; L->m[4].data=(void*)l;
          L->m[5].rtyp = POLY_CMD; L->m[5].data=(void*)u;
          L->m[6].rtyp = POLY_CMD; L->m[6].data=(void*)prodLU;
          res->rtyp = LIST_CMD;
          res->data = (char *)L;
          return FALSE;
        }
        else
        {
          Werror( "expected argument list (int, int, poly, poly, poly, int)");
          return TRUE;
        }
      }
  /*==================== lduSolve ======================*/
      if(strcmp(sys_cmd, "lduSolve")==0)
      {
        /* for solving a linear equation system A * x = b, via the
           given LDU-decomposition of the matrix A;
           There is one valid parametrisation:
           1) exactly eight arguments P, L, D, U, l, u, lTimesU, b;
              P, L, D, and U realise the LDU-decomposition of A, that is,
              P * A = L * D^(-1) * U, and P, L, D, and U satisfy the
              properties decribed in method 'luSolveViaLDUDecomp' in
              linearAlgebra.h; see there;
              l, u, and lTimesU are as described in the same location;
              b is the right-hand side vector of the linear equation system;
           The method will return a list of either 1 entry or three entries:
           1) [0] if there is no solution to the system;
           2) [1, x, H] if there is at least one solution;
              x is any solution of the given linear system,
              H is the matrix with column vectors spanning the homogeneous
              solution space.
           The method produces an error if matrix and vector sizes do not
           fit. */
        if ((h == NULL) || (h->Typ() != MATRIX_CMD) ||
            (h->next == NULL) || (h->next->Typ() != MATRIX_CMD) ||
            (h->next->next == NULL) || (h->next->next->Typ() != MATRIX_CMD) ||
            (h->next->next->next == NULL) ||
              (h->next->next->next->Typ() != MATRIX_CMD) ||
            (h->next->next->next->next == NULL) ||
              (h->next->next->next->next->Typ() != POLY_CMD) ||
            (h->next->next->next->next->next == NULL) ||
              (h->next->next->next->next->next->Typ() != POLY_CMD) ||
            (h->next->next->next->next->next->next == NULL) ||
              (h->next->next->next->next->next->next->Typ() != POLY_CMD) ||
            (h->next->next->next->next->next->next->next == NULL) ||
              (h->next->next->next->next->next->next->next->Typ()
                != MATRIX_CMD) ||
            (h->next->next->next->next->next->next->next->next != NULL))
        {
          Werror("expected input (matrix, matrix, matrix, matrix, %s",
                                 "poly, poly, poly, matrix)");
          return TRUE;
        }
        matrix pMat  = (matrix)h->Data();
        matrix lMat  = (matrix)h->next->Data();
        matrix dMat  = (matrix)h->next->next->Data();
        matrix uMat  = (matrix)h->next->next->next->Data();
        poly l       = (poly)  h->next->next->next->next->Data();
        poly u       = (poly)  h->next->next->next->next->next->Data();
        poly lTimesU = (poly)  h->next->next->next->next->next->next
                                                              ->Data();
        matrix bVec  = (matrix)h->next->next->next->next->next->next
                                                        ->next->Data();
        matrix xVec; int solvable; matrix homogSolSpace;
        if (pMat->rows() != pMat->cols())
        {
          Werror("first matrix (%d x %d) is not quadratic",
                 pMat->rows(), pMat->cols());
          return TRUE;
        }
        if (lMat->rows() != lMat->cols())
        {
          Werror("second matrix (%d x %d) is not quadratic",
                 lMat->rows(), lMat->cols());
          return TRUE;
        }
        if (dMat->rows() != dMat->cols())
        {
          Werror("third matrix (%d x %d) is not quadratic",
                 dMat->rows(), dMat->cols());
          return TRUE;
        }
        if (dMat->cols() != uMat->rows())
        {
          Werror("third matrix (%d x %d) and fourth matrix (%d x %d) %s",
                 dMat->rows(), dMat->cols(), uMat->rows(), uMat->cols(),
                 "do not t");
          return TRUE;
        }
        if (uMat->rows() != bVec->rows())
        {
          Werror("fourth matrix (%d x %d) and vector (%d x 1) do not fit",
                 uMat->rows(), uMat->cols(), bVec->rows());
          return TRUE;
        }
        solvable = luSolveViaLDUDecomp(pMat, lMat, dMat, uMat, l, u, lTimesU,
                                       bVec, xVec, homogSolSpace);

        /* build the return structure; a list with either one or
           three entries */
        lists ll = (lists)omAllocBin(slists_bin);
        if (solvable)
        {
          ll->Init(3);
          ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)solvable;
          ll->m[1].rtyp=MATRIX_CMD; ll->m[1].data=(void *)xVec;
          ll->m[2].rtyp=MATRIX_CMD; ll->m[2].data=(void *)homogSolSpace;
        }
        else
        {
          ll->Init(1);
          ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)solvable;
        }
        res->rtyp = LIST_CMD;
        res->data=(char*)ll;
        return FALSE;
      }
  /*==================== forking experiments ======================*/
      if(strcmp(sys_cmd, "waitforssilinks")==0)
      {
        if ((h != NULL) && (h->Typ() == LIST_CMD) &&
            (h->next != NULL) && (h->next->Typ() == INT_CMD))
        {
          lists L = (lists)h->Data();
          int timeMillisec = (int)(long)h->next->Data();
          int n = slStatusSsiL(L, timeMillisec * 1000);
          res->rtyp = INT_CMD;
          res->data = (void*)(long)n;
          return FALSE;
        }
        else
        {
          Werror( "expected list of open ssi links and timeout");
          return TRUE;
        }
      }
  /*==================== neworder =============================*/
  // should go below
      if(strcmp(sys_cmd,"neworder")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          res->rtyp=STRING_CMD;
          res->data=(void *)singclap_neworder((ideal)h->Data(), currRing);
          return FALSE;
        }
        else
          WerrorS("ideal expected");
      }
      else
  //#ifndef HAVE_DYNAMIC_LOADING
  /*==================== pcv ==================================*/
  #ifdef HAVE_PCV
      if(strcmp(sys_cmd,"pcvLAddL")==0)
      {
        return pcvLAddL(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvPMulL")==0)
      {
        return pcvPMulL(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvMinDeg")==0)
      {
        return pcvMinDeg(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvP2CV")==0)
      {
        return pcvP2CV(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvCV2P")==0)
      {
        return pcvCV2P(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvDim")==0)
      {
        return pcvDim(res,h);
      }
      else
      if(strcmp(sys_cmd,"pcvBasis")==0)
      {
        return pcvBasis(res,h);
      }
      else
  #endif
  /*==================== eigenvalues ==================================*/
  #ifdef HAVE_EIGENVAL
      if(strcmp(sys_cmd,"hessenberg")==0)
      {
        return evHessenberg(res,h);
      }
      else
      if(strcmp(sys_cmd,"eigenvals")==0)
      {
        return evEigenvals(res,h);
      }
      else
  #endif
  /*==================== Gauss-Manin system ==================================*/
  #ifdef HAVE_GMS
      if(strcmp(sys_cmd,"gmsnf")==0)
      {
        return gmsNF(res,h);
      }
      else
  #endif
  //#endif /* HAVE_DYNAMIC_LOADING */
  /*==================== contributors =============================*/
     if(strcmp(sys_cmd,"contributors") == 0)
     {
       res->rtyp=STRING_CMD;
       res->data=(void *)omStrDup(
         "Olaf Bachmann, Michael Brickenstein, Hubert Grassmann, Kai Krueger, Victor Levandovskyy, Wolfgang Neumann, Thomas Nuessler, Wilfred Pohl, Jens Schmidt, Mathias Schulze, Thomas Siebert, Ruediger Stobbe, Moritz Wenk, Tim Wichmann");
       return FALSE;
     }
     else
  /*==================== spectrum =============================*/
     #ifdef HAVE_SPECTRUM
     if(strcmp(sys_cmd,"spectrum") == 0)
     {
       if (h->Typ()!=POLY_CMD)
       {
         WerrorS("poly expected");
         return TRUE;
       }
       if (h->next==NULL)
         return spectrumProc(res,h);
       if (h->next->Typ()!=INT_CMD)
       {
         WerrorS("poly,int expected");
         return TRUE;
       }
       if(((long)h->next->Data())==1L)
         return spectrumfProc(res,h);
       return spectrumProc(res,h);
     }
     else
  /*==================== semic =============================*/
     if(strcmp(sys_cmd,"semic") == 0)
     {
       if ((h->next!=NULL)
       && (h->Typ()==LIST_CMD)
       && (h->next->Typ()==LIST_CMD))
       {
         if (h->next->next==NULL)
           return semicProc(res,h,h->next);
         else if (h->next->next->Typ()==INT_CMD)
           return semicProc3(res,h,h->next,h->next->next);
       }
       return TRUE;
     }
     else
  /*==================== spadd =============================*/
     if(strcmp(sys_cmd,"spadd") == 0)
     {
       if ((h->next!=NULL)
       && (h->Typ()==LIST_CMD)
       && (h->next->Typ()==LIST_CMD))
       {
         if (h->next->next==NULL)
           return spaddProc(res,h,h->next);
       }
       return TRUE;
     }
     else
  /*==================== spmul =============================*/
     if(strcmp(sys_cmd,"spmul") == 0)
     {
       if ((h->next!=NULL)
       && (h->Typ()==LIST_CMD)
       && (h->next->Typ()==INT_CMD))
       {
         if (h->next->next==NULL)
           return spmulProc(res,h,h->next);
       }
       return TRUE;
     }
     else
  #endif

  #define HAVE_SHEAFCOH_TRICKS 1

  #ifdef HAVE_SHEAFCOH_TRICKS
      if(strcmp(sys_cmd,"tensorModuleMult")==0)
      {
  //      WarnS("tensorModuleMult!");
        if (h!=NULL && h->Typ()==INT_CMD && h->Data() != NULL &&
            h->next != NULL && h->next->Typ() == MODUL_CMD
            && h->next->Data() != NULL)
        {
          int m = (int)( (long)h->Data() );
          ideal M = (ideal)h->next->Data();

          res->rtyp=MODUL_CMD;
          res->data=(void *)id_TensorModuleMult(m, M, currRing);
          return FALSE;
        }
        WerrorS("system(\"tensorModuleMult\", int, module) expected");
        return TRUE;
      } else
  #endif

  ////////////////////////////////////////////////////////////////////////
  /// Additional interface functions to non-commutative subsystem (PLURAL)
  ///


  #ifdef HAVE_PLURAL
  /*==================== Approx_Step  =================*/
       if (strcmp(sys_cmd, "astep") == 0)
       {
         ideal I;
         if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
         {
           I=(ideal)h->CopyD();
           res->rtyp=IDEAL_CMD;
           if (rIsPluralRing(currRing)) res->data=Approx_Step(I);
           else res->data=I;
           setFlag(res,FLAG_STD);
         }
         else return TRUE;
         return FALSE;
       }
  /*==================== PrintMat  =================*/
      if (strcmp(sys_cmd, "PrintMat") == 0)
      {
          int a;
          int b;
          ring r;
          int metric;
          if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            a=(int)((long)(h->Data()));
            h=h->next;
          }
          else if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            b=(int)((long)(h->Data()));
            h=h->next;
          }
          else if ((h!=NULL) && (h->Typ()==RING_CMD))
          {
            r=(ring)h->Data();
            h=h->next;
          }
          else
            return TRUE;
          if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            metric=(int)((long)(h->Data()));
          }
          res->rtyp=MATRIX_CMD;
          if (rIsPluralRing(r)) res->data=nc_PrintMat(a,b,r,metric);
          else res->data=NULL;
          return FALSE;
        }
  /*==================== twostd  =================*/
        if (strcmp(sys_cmd, "twostd") == 0)
        {
          ideal I;
          if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
          {
            I=(ideal)h->CopyD();
            res->rtyp=IDEAL_CMD;
            if (rIsPluralRing(currRing)) res->data=twostd(I);
            else res->data=I;
            setFlag(res,FLAG_TWOSTD);
            setFlag(res,FLAG_STD);
          }
          else return TRUE;
          return FALSE;
        }
  /*==================== lie bracket =================*/
      if (strcmp(sys_cmd, "bracket") == 0)
      {
        poly p;
        poly q;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          q=(poly)h->Data();
        }
        else return TRUE;
        res->rtyp=POLY_CMD;
        if (rIsPluralRing(currRing))  res->data=nc_p_Bracket_qq(p,q, currRing);
        else res->data=NULL;
        return FALSE;
      }
      if(strcmp(sys_cmd,"NCUseExtensions")==0)
      {

        if ((h!=NULL) && (h->Typ()==INT_CMD))
          res->data=(void *)(long)setNCExtensions( (int)((long)(h->Data())) );
        else
          res->data=(void *)(long)getNCExtensions();

        res->rtyp=INT_CMD;
        return FALSE;
      }


      if(strcmp(sys_cmd,"NCGetType")==0)
      {
        res->rtyp=INT_CMD;

        if( rIsPluralRing(currRing) )
          res->data=(void *)(long)ncRingType(currRing);
        else
          res->data=(void *)(-1L);

        return FALSE;
      }


      if(strcmp(sys_cmd,"ForceSCA")==0)
      {
        if( !rIsPluralRing(currRing) )
          return TRUE;

        int b, e;

        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          b = (int)((long)(h->Data()));
          h=h->next;
        }
        else return TRUE;

        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          e = (int)((long)(h->Data()));
        }
        else return TRUE;


        if( !sca_Force(currRing, b, e) )
          return TRUE;

        return FALSE;
      }

      if(strcmp(sys_cmd,"ForceNewNCMultiplication")==0)
      {
        if( !rIsPluralRing(currRing) )
          return TRUE;

        if( !ncInitSpecialPairMultiplication(currRing) ) // No Plural!
          return TRUE;

        return FALSE;
      }

      if(strcmp(sys_cmd,"ForceNewOldNCMultiplication")==0)
      {
        if( !rIsPluralRing(currRing) )
          return TRUE;

        if( !ncInitSpecialPowersMultiplication(currRing) ) // Enable Formula for Plural (depends on swiches)!
          return TRUE;

        return FALSE;
      }




      /*==================== PLURAL =================*/
  /*==================== opp ==================================*/
      if (strcmp(sys_cmd, "opp")==0)
      {
        if ((h!=NULL) && (h->Typ()==RING_CMD))
        {
          ring r=(ring)h->Data();
          res->data=rOpposite(r);
          res->rtyp=RING_CMD;
          return FALSE;
        }
        else
        {
          WerrorS("`system(\"opp\",<ring>)` expected");
          return TRUE;
        }
      }
      else
  /*==================== env ==================================*/
      if (strcmp(sys_cmd, "env")==0)
      {
        if ((h!=NULL) && (h->Typ()==RING_CMD))
        {
          ring r = (ring)h->Data();
          res->data = rEnvelope(r);
          res->rtyp = RING_CMD;
          return FALSE;
        }
        else
        {
          WerrorS("`system(\"env\",<ring>)` expected");
          return TRUE;
        }
      }
      else
  /*==================== oppose ==================================*/
      if (strcmp(sys_cmd, "oppose")==0)
      {
        if ((h!=NULL) && (h->Typ()==RING_CMD)
        && (h->next!= NULL))
        {
          ring Rop = (ring)h->Data();
          h   = h->next;
          idhdl w;
          if ((w=Rop->idroot->get(h->Name(),myynest))!=NULL)
          {
            poly p = (poly)IDDATA(w);
            res->data = pOppose(Rop, p, currRing); // into CurrRing?
            res->rtyp = POLY_CMD;
            return FALSE;
          }
        }
        else
        {
          WerrorS("`system(\"oppose\",<ring>,<poly>)` expected");
          return TRUE;
        }
      }
      else
  /*==================== freeGB, twosided GB in free algebra =================*/
  #ifdef HAVE_SHIFTBBA
      if (strcmp(sys_cmd, "freegb") == 0)
      {
        ideal I;
        int uptodeg, lVblock;
        if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
        {
          I=(ideal)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          uptodeg=(int)((long)(h->Data()));
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          lVblock=(int)((long)(h->Data()));
          res->data = freegb(I,uptodeg,lVblock);
          if (res->data == NULL)
          {
            /* that is there were input errors */
            res->data = I;
          }
          res->rtyp = IDEAL_CMD;
        }
        else return TRUE;
        return FALSE;
      }
      else
  #endif /*SHIFTBBA*/
  #endif /*PLURAL*/
  /*==================== walk stuff =================*/
  #ifdef HAVE_WALK
  #ifdef OWNW
      if (strcmp(sys_cmd, "walkNextWeight") == 0)
      {
        if (h == NULL || h->Typ() != INTVEC_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != IDEAL_CMD)
        {
          Werror("system(\"walkNextWeight\", intvec, intvec, ideal) expected");
          return TRUE;
        }

        if (((intvec*) h->Data())->length() != currRing->N ||
            ((intvec*) h->next->Data())->length() != currRing->N)
        {
          Werror("system(\"walkNextWeight\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        res->data = (void*) walkNextWeight(((intvec*) h->Data()),
                                           ((intvec*) h->next->Data()),
                                           (ideal) h->next->next->Data());
        if (res->data == NULL || res->data == (void*) 1L)
        {
          res->rtyp = INT_CMD;
        }
        else
        {
          res->rtyp = INTVEC_CMD;
        }
        return FALSE;
      }
      else if (strcmp(sys_cmd, "walkInitials") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD)
        {
          WerrorS("system(\"walkInitials\", ideal) expected");
          return TRUE;
        }

        res->data = (void*) walkInitials((ideal) h->Data());
        res->rtyp = IDEAL_CMD;
        return FALSE;
      }
      else
  #endif
  #ifdef WAIV
      if (strcmp(sys_cmd, "walkAddIntVec") == 0)
      {
        if (h == NULL || h->Typ() != INTVEC_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD)
        {
          WerrorS("system(\"walkAddIntVec\", intvec, intvec) expected");
          return TRUE;
        }
        intvec* arg1 = (intvec*) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();


        res->data = (intvec*) walkAddIntVec(arg1, arg2);
        res->rtyp = INTVEC_CMD;
        return FALSE;
      }
      else
  #endif
  #ifdef MwaklNextWeight
      if (strcmp(sys_cmd, "MwalkNextWeight") == 0)
      {
        if (h == NULL || h->Typ() != INTVEC_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != IDEAL_CMD)
        {
          Werror("system(\"MwalkNextWeight\", intvec, intvec, ideal) expected");
          return TRUE;
        }

        if (((intvec*) h->Data())->length() != currRing->N ||
            ((intvec*) h->next->Data())->length() != currRing->N)
        {
          Werror("system(\"MwalkNextWeight\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        intvec* arg1 = (intvec*) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        ideal arg3   =   (ideal) h->next->next->Data();

        intvec* result = (intvec*) MwalkNextWeight(arg1, arg2, arg3);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #endif //MWalkNextWeight
      if(strcmp(sys_cmd, "Mivdp") == 0)
      {
        if (h == NULL || h->Typ() != INT_CMD)
        {
          Werror("system(\"Mivdp\", int) expected");
          return TRUE;
        }
        if ((int) ((long)(h->Data())) != currRing->N)
        {
          Werror("system(\"Mivdp\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        int arg1 = (int) ((long)(h->Data()));

        intvec* result = (intvec*) Mivdp(arg1);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }

      else if(strcmp(sys_cmd, "Mivlp") == 0)
      {
        if (h == NULL || h->Typ() != INT_CMD)
        {
          Werror("system(\"Mivlp\", int) expected");
          return TRUE;
        }
        if ((int) ((long)(h->Data())) != currRing->N)
        {
          Werror("system(\"Mivlp\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        int arg1 = (int) ((long)(h->Data()));

        intvec* result = (intvec*) Mivlp(arg1);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }
     else
  #ifdef MpDiv
        if(strcmp(sys_cmd, "MpDiv") == 0)
        {
          if(h==NULL || h->Typ() != POLY_CMD ||
             h->next == NULL || h->next->Typ() != POLY_CMD)
          {
            Werror("system(\"MpDiv\",poly, poly) expected");
            return TRUE;
          }
          poly arg1 = (poly) h->Data();
          poly arg2 = (poly) h->next->Data();

          poly result = MpDiv(arg1, arg2);

          res->rtyp = POLY_CMD;
          res->data = result;
          return FALSE;
        }
      else
  #endif
  #ifdef MpMult
        if(strcmp(sys_cmd, "MpMult") == 0)
        {
          if(h==NULL || h->Typ() != POLY_CMD ||
             h->next == NULL || h->next->Typ() != POLY_CMD)
          {
            Werror("system(\"MpMult\",poly, poly) expected");
            return TRUE;
          }
          poly arg1 = (poly) h->Data();
          poly arg2 = (poly) h->next->Data();

          poly result = MpMult(arg1, arg2);
          res->rtyp = POLY_CMD;
          res->data = result;
          return FALSE;
        }
    else
  #endif
     if (strcmp(sys_cmd, "MivSame") == 0)
      {
        if(h == NULL || h->Typ() != INTVEC_CMD ||
           h->next == NULL || h->next->Typ() != INTVEC_CMD )
        {
          Werror("system(\"MivSame\", intvec, intvec) expected");
          return TRUE;
        }
        /*
        if (((intvec*) h->Data())->length() != currRing->N ||
            ((intvec*) h->next->Data())->length() != currRing->N)
        {
          Werror("system(\"MivSame\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        */
        intvec* arg1 = (intvec*) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        /*
        poly result = (poly) MivSame(arg1, arg2);

        res->rtyp = POLY_CMD;
        res->data =  (poly) result;
        */
        res->rtyp = INT_CMD;
        res->data = (void*)(long) MivSame(arg1, arg2);
        return FALSE;
      }
    else
     if (strcmp(sys_cmd, "M3ivSame") == 0)
      {
        if(h == NULL || h->Typ() != INTVEC_CMD ||
           h->next == NULL || h->next->Typ() != INTVEC_CMD ||
           h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD  )
        {
          Werror("system(\"M3ivSame\", intvec, intvec, intvec) expected");
          return TRUE;
        }
        /*
        if (((intvec*) h->Data())->length() != currRing->N ||
            ((intvec*) h->next->Data())->length() != currRing->N ||
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"M3ivSame\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        */
        intvec* arg1 = (intvec*) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3 = (intvec*) h->next->next->Data();
        /*
        poly result = (poly) M3ivSame(arg1, arg2, arg3);

        res->rtyp = POLY_CMD;
        res->data =  (poly) result;
        */
        res->rtyp = INT_CMD;
        res->data = (void*)(long) M3ivSame(arg1, arg2, arg3);
        return FALSE;
      }
    else
        if(strcmp(sys_cmd, "MwalkInitialForm") == 0)
        {
          if(h == NULL || h->Typ() != IDEAL_CMD ||
             h->next == NULL || h->next->Typ() != INTVEC_CMD)
          {
            Werror("system(\"MwalkInitialForm\", ideal, intvec) expected");
            return TRUE;
          }
          if(((intvec*) h->next->Data())->length() != currRing->N)
          {
            Werror("system \"MwalkInitialForm\"...) intvec not of length %d\n",
                   currRing->N);
            return TRUE;
          }
          ideal id      = (ideal) h->Data();
          intvec* int_w = (intvec*) h->next->Data();
          ideal result  = (ideal) MwalkInitialForm(id, int_w);

          res->rtyp = IDEAL_CMD;
          res->data = result;
          return FALSE;
        }
    else
      /************** Perturbation walk **********/
       if(strcmp(sys_cmd, "MivMatrixOrder") == 0)
        {
          if(h==NULL || h->Typ() != INTVEC_CMD)
          {
            Werror("system(\"MivMatrixOrder\",intvec) expected");
            return TRUE;
          }
          intvec* arg1 = (intvec*) h->Data();

          intvec* result = MivMatrixOrder(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
       if(strcmp(sys_cmd, "MivMatrixOrderdp") == 0)
        {
          if(h==NULL || h->Typ() != INT_CMD)
          {
            Werror("system(\"MivMatrixOrderdp\",intvec) expected");
            return TRUE;
          }
          int arg1 = (int) ((long)(h->Data()));

          intvec* result = (intvec*) MivMatrixOrderdp(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
      if(strcmp(sys_cmd, "MPertVectors") == 0)
        {

          if(h==NULL || h->Typ() != IDEAL_CMD ||
             h->next == NULL || h->next->Typ() != INTVEC_CMD ||
             h->next->next == NULL || h->next->next->Typ() != INT_CMD)
          {
            Werror("system(\"MPertVectors\",ideal, intvec, int) expected");
            return TRUE;
          }

          ideal arg1 = (ideal) h->Data();
          intvec* arg2 = (intvec*) h->next->Data();
          int arg3 = (int) ((long)(h->next->next->Data()));

          intvec* result = (intvec*) MPertVectors(arg1, arg2, arg3);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
      if(strcmp(sys_cmd, "MPertVectorslp") == 0)
        {

          if(h==NULL || h->Typ() != IDEAL_CMD ||
             h->next == NULL || h->next->Typ() != INTVEC_CMD ||
             h->next->next == NULL || h->next->next->Typ() != INT_CMD)
          {
            Werror("system(\"MPertVectorslp\",ideal, intvec, int) expected");
            return TRUE;
          }

          ideal arg1 = (ideal) h->Data();
          intvec* arg2 = (intvec*) h->next->Data();
          int arg3 = (int) ((long)(h->next->next->Data()));

          intvec* result = (intvec*) MPertVectorslp(arg1, arg2, arg3);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
          /************** fractal walk **********/
      else
        if(strcmp(sys_cmd, "Mfpertvector") == 0)
        {
          if(h==NULL || h->Typ() != IDEAL_CMD ||
            h->next==NULL || h->next->Typ() != INTVEC_CMD  )
          {
            Werror("system(\"Mfpertvector\", ideal,intvec) expected");
            return TRUE;
          }
          ideal arg1 = (ideal) h->Data();
          intvec* arg2 = (intvec*) h->next->Data();
          intvec* result = Mfpertvector(arg1, arg2);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
       if(strcmp(sys_cmd, "MivUnit") == 0)
        {
          int arg1 = (int) ((long)(h->Data()));

          intvec* result = (intvec*) MivUnit(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
       else
         if(strcmp(sys_cmd, "MivWeightOrderlp") == 0)
         {
          if(h==NULL || h->Typ() != INTVEC_CMD)
          {
            Werror("system(\"MivWeightOrderlp\",intvec) expected");
            return TRUE;
          }
          intvec* arg1 = (intvec*) h->Data();
          intvec* result = MivWeightOrderlp(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
       else
      if(strcmp(sys_cmd, "MivWeightOrderdp") == 0)
        {
          if(h==NULL || h->Typ() != INTVEC_CMD)
          {
            Werror("system(\"MivWeightOrderdp\",intvec) expected");
            return TRUE;
          }
          intvec* arg1 = (intvec*) h->Data();
          //int arg2 = (int) h->next->Data();

          intvec* result = MivWeightOrderdp(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
       if(strcmp(sys_cmd, "MivMatrixOrderlp") == 0)
        {
          if(h==NULL || h->Typ() != INT_CMD)
          {
            Werror("system(\"MivMatrixOrderlp\",int) expected");
            return TRUE;
          }
          int arg1 = (int) ((long)(h->Data()));

          intvec* result = (intvec*) MivMatrixOrderlp(arg1);

          res->rtyp = INTVEC_CMD;
          res->data =  result;
          return FALSE;
        }
      else
      if (strcmp(sys_cmd, "MkInterRedNextWeight") == 0)
      {
        if (h == NULL || h->Typ() != INTVEC_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != IDEAL_CMD)
        {
          Werror("system(\"MkInterRedNextWeight\", intvec, intvec, ideal) expected");
          return TRUE;
        }

        if (((intvec*) h->Data())->length() != currRing->N ||
            ((intvec*) h->next->Data())->length() != currRing->N)
        {
          Werror("system(\"MkInterRedNextWeight\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        intvec* arg1 = (intvec*) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        ideal arg3   =   (ideal) h->next->next->Data();

        intvec* result = (intvec*) MkInterRedNextWeight(arg1, arg2, arg3);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #ifdef MPertNextWeight
      if (strcmp(sys_cmd, "MPertNextWeight") == 0)
      {
        if (h == NULL || h->Typ() != INTVEC_CMD ||
            h->next == NULL || h->next->Typ() != IDEAL_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"MPertNextWeight\", intvec, ideal, int) expected");
          return TRUE;
        }

        if (((intvec*) h->Data())->length() != currRing->N)
        {
          Werror("system(\"MPertNextWeight\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        intvec* arg1 = (intvec*) h->Data();
        ideal arg2 = (ideal) h->next->Data();
        int arg3   =   (int) h->next->next->Data();

        intvec* result = (intvec*) MPertNextWeight(arg1, arg2, arg3);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #endif //MPertNextWeight
  #ifdef Mivperttarget
    if (strcmp(sys_cmd, "Mivperttarget") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INT_CMD )
        {
          Werror("system(\"Mivperttarget\", ideal, int) expected");
          return TRUE;
        }

        ideal arg1 = (ideal) h->Data();
        int arg2 = (int) h->next->Data();

        intvec* result = (intvec*) Mivperttarget(arg1, arg2);

        res->rtyp = INTVEC_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #endif //Mivperttarget
      if (strcmp(sys_cmd, "Mwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"Mwalk\", ideal, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"Mwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();


        ideal result = (ideal) Mwalk(arg1, arg2, arg3);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #ifdef MPWALK_ORIG
      if (strcmp(sys_cmd, "Mpwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INT_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INT_CMD ||
            h->next->next->next == NULL ||
              h->next->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next->next == NULL ||
              h->next->next->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"Mpwalk\", ideal, int, int, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->next->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->next->next->Data())->length()!=currRing->N)
        {
          Werror("system(\"Mpwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        int arg2 = (int) h->next->Data();
        int arg3 = (int) h->next->next->Data();
        intvec* arg4 = (intvec*) h->next->next->next->Data();
        intvec* arg5   =  (intvec*) h->next->next->next->next->Data();


        ideal result = (ideal) Mpwalk(arg1, arg2, arg3, arg4, arg5);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #endif
      if (strcmp(sys_cmd, "Mpwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INT_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INT_CMD ||
            h->next->next->next == NULL ||
              h->next->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next->next == NULL ||
              h->next->next->next->next->Typ() != INTVEC_CMD||
            h->next->next->next->next->next == NULL ||
              h->next->next->next->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"Mpwalk\", ideal, int, int, intvec, intvec, int) expected");
          return TRUE;
        }

        if (((intvec*) h->next->next->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->next->next->Data())->length()!=currRing->N)
        {
          Werror("system(\"Mpwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        int arg2 = (int) ((long)(h->next->Data()));
        int arg3 = (int) ((long)(h->next->next->Data()));
        intvec* arg4 = (intvec*) h->next->next->next->Data();
        intvec* arg5   =  (intvec*) h->next->next->next->next->Data();
        int arg6   =  (int) ((long)(h->next->next->next->next->next->Data()));


        ideal result = (ideal) Mpwalk(arg1, arg2, arg3, arg4, arg5, arg6);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "Mrwalk") == 0)
      { // Random Walk
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next == NULL || h->next->next->next->Typ() != INT_CMD ||
            h->next->next->next->next == NULL || h->next->next->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"Mrwalk\", ideal, intvec, intvec, int, int) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"Mrwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3 =  (intvec*) h->next->next->Data();
        int arg4 = (int)(long) h->next->next->next->Data();
        int arg5 = (int)(long) h->next->next->next->next->Data();


        ideal result = (ideal) Mrwalk(arg1, arg2, arg3, arg4, arg5);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "MAltwalk1") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INT_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INT_CMD ||
            h->next->next->next == NULL ||
              h->next->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next->next == NULL ||
              h->next->next->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"MAltwalk1\", ideal, int, int, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->next->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->next->next->Data())->length()!=currRing->N)
        {
          Werror("system(\"MAltwalk1\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        int arg2 = (int) ((long)(h->next->Data()));
        int arg3 = (int) ((long)(h->next->next->Data()));
        intvec* arg4 = (intvec*) h->next->next->next->Data();
        intvec* arg5   =  (intvec*) h->next->next->next->next->Data();


        ideal result = (ideal) MAltwalk1(arg1, arg2, arg3, arg4, arg5);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
  #ifdef MFWALK_ALT
      else
      if (strcmp(sys_cmd, "Mfwalk_alt") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next == NULL || h->next->next->next->Typ() !=INT_CMD)
        {
          Werror("system(\"Mfwalk\", ideal, intvec, intvec,int) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"Mfwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();
        int arg4 = (int) h->next->next->next->Data();

        ideal result = (ideal) Mfwalk_alt(arg1, arg2, arg3, arg4);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
  #endif
      else
      if (strcmp(sys_cmd, "Mfwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"Mfwalk\", ideal, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"Mfwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();

        ideal result = (ideal) Mfwalk(arg1, arg2, arg3);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "Mfrwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next == NULL || h->next->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"Mfrwalk\", ideal, intvec, intvec, int) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"Mfrwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3 = (intvec*) h->next->next->Data();
        int arg4 = (int)(long) h->next->next->next->Data();

        ideal result = (ideal) Mfrwalk(arg1, arg2, arg3, arg4);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else

  #ifdef TRAN_Orig
      if (strcmp(sys_cmd, "TranMImprovwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"TranMImprovwalk\", ideal, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"TranMImprovwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();


        ideal result = (ideal) TranMImprovwalk(arg1, arg2, arg3);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
  #endif
      if (strcmp(sys_cmd, "MAltwalk2") == 0)
        {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD)
        {
          Werror("system(\"MAltwalk2\", ideal, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"MAltwalk2\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();


        ideal result = (ideal) MAltwalk2(arg1, arg2, arg3);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "TranMImprovwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD||
            h->next->next->next == NULL || h->next->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"TranMImprovwalk\", ideal, intvec, intvec, int) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"TranMImprovwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3   =  (intvec*) h->next->next->Data();
        int arg4   =  (int) ((long)(h->next->next->next->Data()));

        ideal result = (ideal) TranMImprovwalk(arg1, arg2, arg3, arg4);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else
      if (strcmp(sys_cmd, "TranMrImprovwalk") == 0)
      {
        if (h == NULL || h->Typ() != IDEAL_CMD ||
            h->next == NULL || h->next->Typ() != INTVEC_CMD ||
            h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD ||
            h->next->next->next == NULL || h->next->next->next->Typ() != INT_CMD ||
            h->next->next->next == NULL || h->next->next->next->next->Typ() != INT_CMD ||
            h->next->next->next == NULL || h->next->next->next->next->next->Typ() != INT_CMD)
        {
          Werror("system(\"TranMrImprovwalk\", ideal, intvec, intvec) expected");
          return TRUE;
        }

        if (((intvec*) h->next->Data())->length() != currRing->N &&
            ((intvec*) h->next->next->Data())->length() != currRing->N )
        {
          Werror("system(\"TranMrImprovwalk\" ...) intvecs not of length %d\n", currRing->N);
          return TRUE;
        }
        ideal arg1 = (ideal) h->Data();
        intvec* arg2 = (intvec*) h->next->Data();
        intvec* arg3 = (intvec*) h->next->next->Data();
        int arg4 = (int)(long) h->next->next->next->Data();
        int arg5 = (int)(long) h->next->next->next->next->Data();
        int arg6 = (int)(long) h->next->next->next->next->next->Data();

        ideal result = (ideal) TranMrImprovwalk(arg1, arg2, arg3, arg4, arg5, arg6);

        res->rtyp = IDEAL_CMD;
        res->data =  result;

        return FALSE;
      }
      else

  #endif
  /*================= Extended system call ========================*/
     {
       #ifndef MAKE_DISTRIBUTION
       return(jjEXTENDED_SYSTEM(res, args));
       #else
       Werror( "system(\"%s\",...) %s", sys_cmd, feNotImplemented );
       #endif
     }
    } /* typ==string */
    return TRUE;
  }


#ifdef HAVE_EXTENDED_SYSTEM
  // You can put your own system calls here
#  include <kernel/fglm/fglmcomb.cc>
#  include <kernel/fglm/fglm.h>
#  ifdef HAVE_NEWTON
#    include <hc_newton.h>
#  endif
#  include <polys/mod_raw.h>
#  include <polys/monomials/ring.h>
#  include <kernel/shiftgb.h>

static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h)
{
    if(h->Typ() == STRING_CMD)
    {
      char *sys_cmd=(char *)(h->Data());
      h=h->next;
  /*==================== test syz strat =================*/
      if (strcmp(sys_cmd, "syz") == 0)
      {
         int posInT_EcartFDegpLength(const TSet set,const int length,LObject &p);
         int posInT_FDegpLength(const TSet set,const int length,LObject &p);
         int posInT_pLength(const TSet set,const int length,LObject &p);
         int posInT0(const TSet set,const int length,LObject &p);
         int posInT1(const TSet set,const int length,LObject &p);
         int posInT2(const TSet set,const int length,LObject &p);
         int posInT11(const TSet set,const int length,LObject &p);
         int posInT110(const TSet set,const int length,LObject &p);
         int posInT13(const TSet set,const int length,LObject &p);
         int posInT15(const TSet set,const int length,LObject &p);
         int posInT17(const TSet set,const int length,LObject &p);
         int posInT17_c(const TSet set,const int length,LObject &p);
         int posInT19(const TSet set,const int length,LObject &p);
         if ((h!=NULL) && (h->Typ()==STRING_CMD))
         {
           const char *s=(const char *)h->Data();
           if (strcmp(s,"posInT_EcartFDegpLength")==0)
             test_PosInT=posInT_EcartFDegpLength;
           else if (strcmp(s,"posInT_FDegpLength")==0)
             test_PosInT=posInT_FDegpLength;
           else if (strcmp(s,"posInT_pLength")==0)
             test_PosInT=posInT_pLength;
           else if (strcmp(s,"posInT0")==0)
             test_PosInT=posInT0;
           else if (strcmp(s,"posInT1")==0)
             test_PosInT=posInT1;
           else if (strcmp(s,"posInT2")==0)
             test_PosInT=posInT2;
           else if (strcmp(s,"posInT11")==0)
             test_PosInT=posInT11;
           else if (strcmp(s,"posInT110")==0)
             test_PosInT=posInT110;
           else if (strcmp(s,"posInT13")==0)
             test_PosInT=posInT13;
           else if (strcmp(s,"posInT15")==0)
             test_PosInT=posInT15;
           else if (strcmp(s,"posInT17")==0)
             test_PosInT=posInT17;
           else if (strcmp(s,"posInT17_c")==0)
             test_PosInT=posInT17_c;
           else if (strcmp(s,"posInT19")==0)
             test_PosInT=posInT19;
           else Print("valid posInT:0,1,2,11,110,13,15,17,17_c,19,_EcartFDegpLength,_FDegpLength,_pLength,_EcartpLength\n");
         }
         else
         {
           test_PosInT=NULL;
           test_PosInL=NULL;
         }
         si_opt_2|=Sy_bit(23);
         return FALSE;
      }
      else
  /*==================== locNF ======================================*/
      if(strcmp(sys_cmd,"locNF")==0)
      {
        if (h != NULL && h->Typ() == VECTOR_CMD)
        {
          poly f=(poly)h->Data();
          h=h->next;
          if (h != NULL && h->Typ() == MODUL_CMD)
          {
            ideal m=(ideal)h->Data();
            assumeStdFlag(h);
            h=h->next;
            if (h != NULL && h->Typ() == INT_CMD)
            {
              int n=(int)((long)h->Data());
              h=h->next;
              if (h != NULL && h->Typ() == INTVEC_CMD)
              {
                intvec *v=(intvec *)h->Data();

                /* == now the work starts == */

                short * iv=iv2array(v, currRing);
                poly r=0;
                poly hp=ppJetW(f,n,iv);
                int s=MATCOLS(m);
                int j=0;
                matrix T=mp_InitI(s,1,0, currRing);

                while (hp != NULL)
                {
                  if (pDivisibleBy(m->m[j],hp))
                    {
                      if (MATELEM(T,j+1,1)==0)
                      {
                        MATELEM(T,j+1,1)=pDivideM(pHead(hp),pHead(m->m[j]));
                      }
                      else
                      {
                        pAdd(MATELEM(T,j+1,1),pDivideM(pHead(hp),pHead(m->m[j])));
                      }
                      hp=ppJetW(ksOldSpolyRed(m->m[j],hp,0),n,iv);
                      j=0;
                    }
                  else
                  {
                    if (j==s-1)
                    {
                      r=pAdd(r,pHead(hp));
                      hp=pLmDeleteAndNext(hp); /* hp=pSub(hp,pHead(hp));*/
                      j=0;
                    }
                    else
                    {
                      j++;
                    }
                  }
                }

                matrix Temp=mp_Transp((matrix) id_Vec2Ideal(r, currRing), currRing);
                matrix R=mpNew(MATCOLS((matrix) id_Vec2Ideal(f, currRing)),1);
                for (int k=1;k<=MATROWS(Temp);k++)
                {
                  MATELEM(R,k,1)=MATELEM(Temp,k,1);
                }

                lists L=(lists)omAllocBin(slists_bin);
                L->Init(2);
                L->m[0].rtyp=MATRIX_CMD;   L->m[0].data=(void *)R;
                L->m[1].rtyp=MATRIX_CMD;   L->m[1].data=(void *)T;
                res->data=L;
                res->rtyp=LIST_CMD;
                // iv aufraeumen
                omFree(iv);
              }
              else
              {
                Warn ("4th argument: must be an intvec!");
              }
            }
            else
            {
              Warn("3rd argument must be an int!!");
            }
          }
          else
          {
            Warn("2nd argument must be a module!");
          }
        }
        else
        {
          Warn("1st argument must be a vector!");
        }
        return FALSE;
      }
      else
  /*==================== poly debug ==================================*/
        if(strcmp(sys_cmd,"p")==0)
        {
#  ifdef RDEBUG
          p_DebugPrint((poly)h->Data(), currRing);
#  else
          Warn("Sorry: not available for release build!");
#  endif
          return FALSE;
        }
        else
  /*==================== setsyzcomp ==================================*/
      if(strcmp(sys_cmd,"setsyzcomp")==0)
      {
      
      if ((h!=NULL) && (h->Typ()==INT_CMD))
         {
           int k = (int)(long)h->Data();
           if ( currRing->order[0] == ringorder_s )
           {
                rSetSyzComp(k, currRing);
           }
          }
      
      }
  /*==================== ring debug ==================================*/
        if(strcmp(sys_cmd,"r")==0)
        {
#  ifdef RDEBUG
          rDebugPrint((ring)h->Data());
#  else
          Warn("Sorry: not available for release build!");
#  endif
          return FALSE;
        }
        else
  /*==================== changeRing ========================*/
        /* The following code changes the names of the variables in the
           current ring to "x1", "x2", ..., "xN", where N is the number
           of variables in the current ring.
           The purpose of this rewriting is to eliminate indexed variables,
           as they may cause problems when generating scripts for Magma,
           Maple, or Macaulay2. */
        if(strcmp(sys_cmd,"changeRing")==0)
        {
          int varN = currRing->N;
          char h[10];
          for (int i = 1; i <= varN; i++)
          {
            omFree(currRing->names[i - 1]);
            sprintf(h, "x%d", i);
            currRing->names[i - 1] = omStrDup(h);
          }
          rComplete(currRing);
          res->rtyp = INT_CMD;
          res->data = (void*)0L;
          return FALSE;
        }
        else
  /*==================== mtrack ==================================*/
      if(strcmp(sys_cmd,"mtrack")==0)
      {
  #ifdef OM_TRACK
        om_Opts.MarkAsStatic = 1;
        FILE *fd = NULL;
        int max = 5;
        while (h != NULL)
        {
          omMarkAsStaticAddr(h);
          if (fd == NULL && h->Typ()==STRING_CMD)
          {
            fd = fopen((char*) h->Data(), "w");
            if (fd == NULL)
              Warn("Can not open %s for writing og mtrack. Using stdout"); // %s  ???
          }
          if (h->Typ() == INT_CMD)
          {
            max = (int)(long)h->Data();
          }
          h = h->Next();
        }
        omPrintUsedTrackAddrs((fd == NULL ? stdout : fd), max);
        if (fd != NULL) fclose(fd);
        om_Opts.MarkAsStatic = 0;
        return FALSE;
  #endif
      }
  /*==================== mtrack_all ==================================*/
      if(strcmp(sys_cmd,"mtrack_all")==0)
      {
  #ifdef OM_TRACK
        om_Opts.MarkAsStatic = 1;
        FILE *fd = NULL;
        if ((h!=NULL) &&(h->Typ()==STRING_CMD))
        {
          fd = fopen((char*) h->Data(), "w");
          if (fd == NULL)
            Warn("Can not open %s for writing og mtrack. Using stdout");
          omMarkAsStaticAddr(h);
        }
        // OB: TBC print to fd
        omPrintUsedAddrs((fd == NULL ? stdout : fd), 5);
        if (fd != NULL) fclose(fd);
        om_Opts.MarkAsStatic = 0;
        return FALSE;
  #endif
      }
      else
  /*==================== backtrace ==================================*/
  #ifndef OM_NDEBUG
      if(strcmp(sys_cmd,"backtrace")==0)
      {
        omPrintCurrentBackTrace(stdout);
        return FALSE;
      }
      else
  #endif

#if !defined(OM_NDEBUG)
  /*==================== omMemoryTest ==================================*/
      if (strcmp(sys_cmd,"omMemoryTest")==0)
      {

#ifdef OM_STATS_H
        PrintS("\n[om_Info]: \n");
        omUpdateInfo();
#define OM_PRINT(name) Print(" %-22s : %10ld \n", #name, om_Info . name)
        OM_PRINT(MaxBytesSystem);
        OM_PRINT(CurrentBytesSystem);
        OM_PRINT(MaxBytesSbrk);
        OM_PRINT(CurrentBytesSbrk);
        OM_PRINT(MaxBytesMmap);
        OM_PRINT(CurrentBytesMmap);
        OM_PRINT(UsedBytes);
        OM_PRINT(AvailBytes);
        OM_PRINT(UsedBytesMalloc);
        OM_PRINT(AvailBytesMalloc);
        OM_PRINT(MaxBytesFromMalloc);
        OM_PRINT(CurrentBytesFromMalloc);
        OM_PRINT(MaxBytesFromValloc);
        OM_PRINT(CurrentBytesFromValloc);
        OM_PRINT(UsedBytesFromValloc);
        OM_PRINT(AvailBytesFromValloc);
        OM_PRINT(MaxPages);
        OM_PRINT(UsedPages);
        OM_PRINT(AvailPages);
        OM_PRINT(MaxRegionsAlloc);
        OM_PRINT(CurrentRegionsAlloc);
#undef OM_PRINT
#endif

#ifdef OM_OPTS_H
        PrintS("\n[om_Opts]: \n");
#define OM_PRINT(format, name) Print(" %-22s : %10" format"\n", #name, om_Opts . name)
        OM_PRINT("d", MinTrack);
        OM_PRINT("d", MinCheck);
        OM_PRINT("d", MaxTrack);
        OM_PRINT("d", MaxCheck);
        OM_PRINT("d", Keep);
        OM_PRINT("d", HowToReportErrors);
        OM_PRINT("d", MarkAsStatic);
        OM_PRINT("u", PagesPerRegion);
        OM_PRINT("p", OutOfMemoryFunc);
        OM_PRINT("p", MemoryLowFunc);
        OM_PRINT("p", ErrorHook);
#undef OM_PRINT
#endif

#ifdef OM_ERROR_H
        Print("\n\n[om_ErrorStatus]        : '%s' (%s)\n",
                omError2String(om_ErrorStatus),
                omError2Serror(om_ErrorStatus));
        Print("[om_InternalErrorStatus]: '%s' (%s)\n",
                omError2String(om_InternalErrorStatus),
                omError2Serror(om_InternalErrorStatus));

#endif

//        omTestMemory(1);
//        omtTestErrors();
        return FALSE;
      }
      else
#endif
  /*==================== naIdeal ==================================*/
//       // This seems to be obsolette with the new Frank's alg.ext field...
//       if(strcmp(sys_cmd,"naIdeal")==0)
//       {
//         if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
//         {
//           naSetIdeal((ideal)h->Data());
//           return FALSE;
//         }
//         else
//            WerrorS("ideal expected");
//       }
//       else
  /*==================== pDivStat =============================*/
  #if defined(PDEBUG) || defined(PDIV_DEBUG)
      if(strcmp(sys_cmd,"pDivStat")==0)
      {
        extern void pPrintDivisbleByStat();
        pPrintDivisbleByStat();
        return FALSE;
      }
      else
  #endif
  /*==================== alarm ==================================*/
  #ifdef unix
      if(strcmp(sys_cmd,"alarm")==0)
      {
        if ((h!=NULL) &&(h->Typ()==INT_CMD))
        {
          // standard variant -> SIGALARM (standard: abort)
          //alarm((unsigned)h->next->Data());
          // process time (user +system): SIGVTALARM
          struct itimerval t,o;
          memset(&t,0,sizeof(t));
          t.it_value.tv_sec     =(unsigned)((unsigned long)h->Data());
          setitimer(ITIMER_VIRTUAL,&t,&o);
          return FALSE;
        }
        else
          WerrorS("int expected");
      }
      else
  #endif
  /*==================== red =============================*/
  #if 0
      if(strcmp(sys_cmd,"red")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          res->rtyp=IDEAL_CMD;
          res->data=(void *)kStdred((ideal)h->Data(),NULL,testHomog,NULL);
          setFlag(res,FLAG_STD);
          return FALSE;
        }
        else
          WerrorS("ideal expected");
      }
      else
  #endif
  /*==================== fastcomb =============================*/
      if(strcmp(sys_cmd,"fastcomb")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          if (h->next!=NULL)
          {
            if (h->next->Typ()!=POLY_CMD)
            {
              Warn("Wrong types for poly= comb(ideal,poly)");
            }
          }
          res->rtyp=POLY_CMD;
          res->data=(void *) fglmLinearCombination(
                             (ideal)h->Data(),(poly)h->next->Data());
          return FALSE;
        }
        else
          WerrorS("ideal expected");
      }
      else
  /*==================== comb =============================*/
      if(strcmp(sys_cmd,"comb")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          if (h->next!=NULL)
          {
            if (h->next->Typ()!=POLY_CMD)
            {
                Warn("Wrong types for poly= comb(ideal,poly)");
            }
          }
          res->rtyp=POLY_CMD;
          res->data=(void *)fglmNewLinearCombination(
                              (ideal)h->Data(),(poly)h->next->Data());
          return FALSE;
        }
        else
          WerrorS("ideal expected");
      }
      else
  #if 0 /* debug only */
  /*==================== listall ===================================*/
      if(strcmp(sys_cmd,"listall")==0)
      {
        void listall(int showproc);
        int showproc=0;
        if ((h!=NULL) && (h->Typ()==INT_CMD)) showproc=(int)((long)h->Data());
        listall(showproc);
        return FALSE;
      }
      else
  #endif
  #if 0 /* debug only */
  /*==================== proclist =================================*/
      if(strcmp(sys_cmd,"proclist")==0)
      {
        void piShowProcList();
        piShowProcList();
        return FALSE;
      }
      else
  #endif
  /* ==================== newton ================================*/
  #ifdef HAVE_NEWTON
      if(strcmp(sys_cmd,"newton")==0)
      {
        if ((h->Typ()!=POLY_CMD)
        || (h->next->Typ()!=INT_CMD)
        || (h->next->next->Typ()!=INT_CMD))
        {
          WerrorS("system(\"newton\",<poly>,<int>,<int>) expected");
          return TRUE;
        }
        poly  p=(poly)(h->Data());
        int l=pLength(p);
        short *points=(short *)omAlloc(currRing->N*l*sizeof(short));
        int i,j,k;
        k=0;
        poly pp=p;
        for (i=0;pp!=NULL;i++)
        {
          for(j=1;j<=currRing->N;j++)
          {
            points[k]=pGetExp(pp,j);
            k++;
          }
          pIter(pp);
        }
        hc_ERG r=hc_KOENIG(currRing->N,      // dimension
                  l,      // number of points
                  (short*) points,   // points: x_1, y_1,z_1, x_2,y_2,z2,...
                  currRing->OrdSgn==-1,
                  (int) (h->next->Data()),      // 1: Milnor, 0: Newton
                  (int) (h->next->next->Data()) // debug
                 );
        //----<>---Output-----------------------


  //  PrintS("Bin jetzt in extra.cc bei der Auswertung.\n"); // **********


        lists L=(lists)omAllocBin(slists_bin);
        L->Init(6);
        L->m[0].rtyp=STRING_CMD;               // newtonnumber;
        L->m[0].data=(void *)omStrDup(r.nZahl);
        L->m[1].rtyp=INT_CMD;
        L->m[1].data=(void *)(long)r.achse;          // flag for unoccupied axes
        L->m[2].rtyp=INT_CMD;
        L->m[2].data=(void *)(long)r.deg;            // #degenerations
        if ( r.deg != 0)              // only if degenerations exist
        {
          L->m[3].rtyp=INT_CMD;
          L->m[3].data=(void *)(long)r.anz_punkte;     // #points
          //---<>--number of points------
          int anz = r.anz_punkte;    // number of points
          int dim = (currRing->N);     // dimension
          intvec* v = new intvec( anz*dim );
          for (i=0; i<anz*dim; i++)    // copy points
            (*v)[i] = r.pu[i];
          L->m[4].rtyp=INTVEC_CMD;
          L->m[4].data=(void *)v;
          //---<>--degenerations---------
          int deg = r.deg;    // number of points
          intvec* w = new intvec( r.speicher );  // necessary memeory
          i=0;               // start copying
          do
          {
            (*w)[i] = r.deg_tab[i];
            i++;
          }
          while (r.deg_tab[i-1] != -2);   // mark for end of list
          L->m[5].rtyp=INTVEC_CMD;
          L->m[5].data=(void *)w;
        }
        else
        {
          L->m[3].rtyp=INT_CMD; L->m[3].data=(char *)0;
          L->m[4].rtyp=DEF_CMD;
          L->m[5].rtyp=DEF_CMD;
        }

        res->data=(void *)L;
        res->rtyp=LIST_CMD;
        // free all pointer in r:
        delete[] r.nZahl;
        delete[] r.pu;
        delete[] r.deg_tab;      // Ist das ein Problem??

        omFreeSize((ADDRESS)points,currRing->N*l*sizeof(short));
        return FALSE;
      }
      else
  #endif
  /*==== connection to Sebastian Jambor's code ======*/
  /* This code connects Sebastian Jambor's code for
     computing the minimal polynomial of an (n x n) matrix
     with entries in F_p to SINGULAR. Two conversion methods
     are needed; see further up in this file:
        (1) conversion of a matrix with long entries to
            a SINGULAR matrix with number entries, where
            the numbers are coefficients in currRing;
        (2) conversion of an array of longs (encoding the
            coefficients of the minimal polynomial) to a
            SINGULAR poly living in currRing. */
      if (strcmp(sys_cmd, "minpoly") == 0)
      {
        if ((h == NULL) || (h->Typ() != MATRIX_CMD) || h->next != NULL)
        {
          Werror("expected exactly one argument: %s",
                 "a square matrix with number entries");
          return TRUE;
        }
        else
        {
          matrix m = (matrix)h->Data();
          int n = m->rows();
          unsigned long p = (unsigned long)n_GetChar(currRing->cf);
          if (n != m->cols())
          {
            Werror("expected exactly one argument: %s",
                   "a square matrix with number entries");
            return TRUE;
          }
          unsigned long** ml = singularMatrixToLongMatrix(m);
          unsigned long* polyCoeffs = computeMinimalPolynomial(ml, n, p);
          poly theMinPoly = longCoeffsToSingularPoly(polyCoeffs, n);
          res->rtyp = POLY_CMD;
          res->data = (void *)theMinPoly;
          for (int i = 0; i < n; i++) delete[] ml[i];
          delete[] ml;
          delete[] polyCoeffs;
          return FALSE;
        }
      }
      else
  /*==================== sdb_flags =================*/
  #ifdef HAVE_SDB
      if (strcmp(sys_cmd, "sdb_flags") == 0)
      {
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          sdb_flags=(int)((long)h->Data());
        }
        else
        {
          WerrorS("system(\"sdb_flags\",`int`) expected");
          return TRUE;
        }
        return FALSE;
      }
      else
  #endif
  /*==================== sdb_edit =================*/
  #ifdef HAVE_SDB
      if (strcmp(sys_cmd, "sdb_edit") == 0)
      {
        if ((h!=NULL) && (h->Typ()==PROC_CMD))
        {
          procinfov p=(procinfov)h->Data();
          sdb_edit(p);
        }
        else
        {
          WerrorS("system(\"sdb_edit\",`proc`) expected");
          return TRUE;
        }
        return FALSE;
      }
      else
  #endif
  /*==================== GF =================*/
  #if 0 // for testing only
      if (strcmp(sys_cmd, "GF") == 0)
      {
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          int c=rChar(currRing);
          setCharacteristic( c,nfMinPoly[0], currRing->parameter[0][0] );
          CanonicalForm F( convSingGFFactoryGF( (poly)h->Data(), currRing ) );
          res->rtyp=POLY_CMD;
          res->data=convFactoryGFSingGF( F, currRing );
          return FALSE;
        }
        else { Werror("wrong typ"); return TRUE;}
      }
      else
  #endif
  /*==================== stdX =================*/
      if (strcmp(sys_cmd, "std") == 0)
      {
        ideal i1;
        int i2;
        if ((h!=NULL) && (h->Typ()==MODUL_CMD))
        {
          i1=(ideal)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          i2=(int)((long)h->Data());
        }
        else return TRUE;
        res->rtyp=MODUL_CMD;
        res->data=idXXX(i1,i2);
        return FALSE;
      }
      else
  /*==================== SVD =================*/
  #ifdef HAVE_SVD
       if (strcmp(sys_cmd, "svd") == 0)
       {
            extern lists testsvd(matrix M);
              res->rtyp=LIST_CMD;
            res->data=(char*)(testsvd((matrix)h->Data()));
            return FALSE;
       }
       else
  #endif

  /*==== countedref: reference and shared ====*/
       if (strcmp(sys_cmd, "shared") == 0)
       {
       #ifndef SI_COUNTEDREF_AUTOLOAD
         void countedref_shared_load();
         countedref_shared_load();
       #endif
         res->rtyp = NONE;
         return FALSE;
       }
       else if (strcmp(sys_cmd, "reference") == 0)
       {
       #ifndef SI_COUNTEDREF_AUTOLOAD
         void countedref_reference_load();
         countedref_reference_load();
       #endif
         res->rtyp = NONE;
         return FALSE;
       }
       else

  /*==================== DLL =================*/
  #ifdef ix86_Win
  #ifdef HAVE_DL
  /* testing the DLL functionality under Win32 */
        if (strcmp(sys_cmd, "DLL") == 0)
        {
          typedef void  (*Void_Func)();
          typedef int  (*Int_Func)(int);
          void *hh=dynl_open("WinDllTest.dll");
          if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            int (*f)(int);
            if (hh!=NULL)
            {
              int (*f)(int);
              f=(Int_Func)dynl_sym(hh,"PlusDll");
              int i=10;
              if (f!=NULL) printf("%d\n",f(i));
              else PrintS("cannot find PlusDll\n");
            }
          }
          else
          {
            void (*f)();
            f= (Void_Func)dynl_sym(hh,"TestDll");
            if (f!=NULL) f();
            else PrintS("cannot find TestDll\n");
          }
          return FALSE;
        }
        else
  #endif
  #endif
  /*==================== eigenvalues ==================================*/
  #ifdef HAVE_EIGENVAL
      if(strcmp(sys_cmd,"eigenvals")==0)
      {
        return evEigenvals(res,h);
      }
      else
  #endif
  /*==================== Gauss-Manin system ==================================*/
  #ifdef HAVE_GMS
      if(strcmp(sys_cmd,"gmsnf")==0)
      {
        return gmsNF(res,h);
      }
      else
  #endif
  /*==================== facstd_debug ==================================*/
  #if !defined(SING_NDEBUG)
      if(strcmp(sys_cmd,"facstd")==0)
      {
        extern int strat_nr;
        extern int strat_fac_debug;
        strat_fac_debug=(int)(long)h->Data();
        strat_nr=0;
        return FALSE;
      }
      else
  #endif
  #ifdef HAVE_RING2TOM
  /*==================== ring-GB ==================================*/
      if (strcmp(sys_cmd, "findZeroPoly")==0)
      {
        ring r = currRing;
        poly f = (poly) h->Data();
        res->rtyp=POLY_CMD;
        res->data=(poly) kFindZeroPoly(f, r, r);
        return(FALSE);
      }
      else
  /*==================== Creating zero polynomials =================*/
  #ifdef HAVE_VANIDEAL
      if (strcmp(sys_cmd, "createG0")==0)
      {
        /* long exp[50];
        int N = 0;
        while (h != NULL)
        {
          N += 1;
          exp[N] = (long) h->Data();
          // if (exp[i] % 2 != 0) exp[i] -= 1;
          h = h->next;
        }
        for (int k = 1; N + k <= currRing->N; k++) exp[k] = 0;

        poly t_p;
        res->rtyp=POLY_CMD;
        res->data= (poly) kCreateZeroPoly(exp, -1, &t_p, currRing, currRing);
        return(FALSE); */

        res->rtyp = IDEAL_CMD;
        res->data = (ideal) createG0();
        return(FALSE);
      }
      else
  #endif
  /*==================== redNF_ring =================*/
      if (strcmp(sys_cmd, "redNF_ring")==0)
      {
        ring r = currRing;
        poly f = (poly) h->Data();
        h = h->next;
        ideal G = (ideal) h->Data();
        res->rtyp=POLY_CMD;
        res->data=(poly) ringRedNF(f, G, r);
        return(FALSE);
      }
      else
  #endif
  /*==================== Roune Hilb  =================*/
       if (strcmp(sys_cmd, "hilbroune") == 0)
       {
         ideal I;
         if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
         {
           I=(ideal)h->CopyD();
           slicehilb(I);
         }
         else return TRUE;
         return FALSE;
       }
  /*==================== minor =================*/
      if (strcmp(sys_cmd, "minor")==0)
      {
        matrix a = (matrix) h->Data();
        h = h->next;
        int ar = (int)(long) h->Data();
        h = h->next;
        int which = (int)(long) h->Data();
        h = h->next;
        ideal R = NULL;
        if (h != NULL)
        {
          R = (ideal) h->Data();
        }
        res->data=(poly) idMinor(a, ar, (unsigned long) which, R);
        if (res->data == (poly) 1)
        {
          res->rtyp=INT_CMD;
          res->data = 0;
        }
        else
        {
          res->rtyp=POLY_CMD;
        }
        return(FALSE);
      }
      else
  /*==================== F5 Implementation =================*/
  #ifdef HAVE_F5
      if (strcmp(sys_cmd, "f5")==0)
      {
        if (h->Typ()!=IDEAL_CMD)
        {
          WerrorS("ideal expected");
          return TRUE;
        }

        ring r = currRing;
        ideal G = (ideal) h->Data();
        h = h->next;
        int opt;
        if(h != NULL) {
          opt = (int) (long) h->Data();
        }
        else {
          opt = 2;
        }
        h = h->next;
        int plus;
        if(h != NULL) {
          plus = (int) (long) h->Data();
        }
        else {
          plus = 0;
        }
        h = h->next;
        int termination;
        if(h != NULL) {
          termination = (int) (long) h->Data();
        }
        else {
          termination = 0;
        }
        res->rtyp=IDEAL_CMD;
        res->data=(ideal) F5main(G,r,opt,plus,termination);
        return FALSE;
      }
      else
  #endif
  /*==================== Testing groebner basis =================*/
  #ifdef HAVE_RINGS
      if (strcmp(sys_cmd, "NF_ring")==0)
      {
        ring r = currRing;
        poly f = (poly) h->Data();
        h = h->next;
        ideal G = (ideal) h->Data();
        res->rtyp=POLY_CMD;
        res->data=(poly) ringNF(f, G, r);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "spoly")==0)
      {
        poly f = pCopy((poly) h->Data());
        h = h->next;
        poly g = pCopy((poly) h->Data());

        res->rtyp=POLY_CMD;
        res->data=(poly) plain_spoly(f,g);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "testGB")==0)
      {
        ideal I = (ideal) h->Data();
        h = h->next;
        ideal GI = (ideal) h->Data();
        res->rtyp = INT_CMD;
        res->data = (void *)(long) testGB(I, GI);
        return(FALSE);
      }
      else
  #endif
  /*==================== sca?AltVar ==================================*/
  #ifdef HAVE_PLURAL
      if ( (strcmp(sys_cmd, "AltVarStart") == 0) || (strcmp(sys_cmd, "AltVarEnd") == 0) )
      {
        ring r = currRing;

        if((h!=NULL) && (h->Typ()==RING_CMD)) r = (ring)h->Data(); else
        {
          WerrorS("`system(\"AltVarStart/End\"[,<ring>])` expected");
          return TRUE;
        }

        res->rtyp=INT_CMD;

        if (rIsSCA(r))
        {
          if(strcmp(sys_cmd, "AltVarStart") == 0)
            res->data = (void*)(long)scaFirstAltVar(r);
          else
            res->data = (void*)(long)scaLastAltVar(r);
          return FALSE;
        }

        WerrorS("`system(\"AltVarStart/End\",<ring>) requires a SCA ring");
        return TRUE;
      }
      else
  #endif
  /*==================== RatNF, noncomm rational coeffs =================*/
  #ifdef HAVE_PLURAL
  #ifdef HAVE_RATGRING
      if (strcmp(sys_cmd, "intratNF") == 0)
      {
        poly p;
        poly *q;
        ideal I;
        int is, k, id;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
          //        Print("poly is done\n");
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
        {
          I=(ideal)h->CopyD();
          q = I->m;
          h=h->next;
          //        Print("ideal is done\n");
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          is=(int)((long)(h->Data()));
          //        res->rtyp=INT_CMD;
          //        Print("int is done\n");
          //        res->rtyp=IDEAL_CMD;
          if (rIsPluralRing(currRing))
          {
            id = IDELEMS(I);
                   int *pl=(int*)omAlloc0(IDELEMS(I)*sizeof(int));
            for(k=0; k < id; k++)
            {
              pl[k] = pLength(I->m[k]);
            }
            Print("starting redRat\n");
            //res->data = (char *)
            redRat(&p, q, pl, (int)IDELEMS(I),is,currRing);
            res->data=p;
            res->rtyp=POLY_CMD;
            //        res->data = ncGCD(p,q,currRing);
          }
          else
          {
            res->rtyp=POLY_CMD;
            res->data=p;
          }
        }
        else return TRUE;
        return FALSE;
      }
      else
  /*==================== RatNF, noncomm rational coeffs =================*/
      if (strcmp(sys_cmd, "ratNF") == 0)
      {
        poly p,q;
        int is, htype;
        if ((h!=NULL) && ( (h->Typ()==POLY_CMD) || (h->Typ()==VECTOR_CMD) ) )
        {
          p=(poly)h->CopyD();
          h=h->next;
          htype = h->Typ();
        }
        else return TRUE;
        if ((h!=NULL) && ( (h->Typ()==POLY_CMD) || (h->Typ()==VECTOR_CMD) ) )
        {
          q=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          is=(int)((long)(h->Data()));
          res->rtyp=htype;
          //        res->rtyp=IDEAL_CMD;
          if (rIsPluralRing(currRing))
          {
            res->data = nc_rat_ReduceSpolyNew(q,p,is, currRing);
            //        res->data = ncGCD(p,q,currRing);
          }
          else res->data=p;
        }
        else return TRUE;
        return FALSE;
      }
      else
  /*==================== RatSpoly, noncomm rational coeffs =================*/
      if (strcmp(sys_cmd, "ratSpoly") == 0)
      {
        poly p,q;
        int is;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          q=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          is=(int)((long)(h->Data()));
          res->rtyp=POLY_CMD;
          //        res->rtyp=IDEAL_CMD;
          if (rIsPluralRing(currRing))
          {
            res->data = nc_rat_CreateSpoly(p,q,is,currRing);
            //        res->data = ncGCD(p,q,currRing);
          }
          else res->data=p;
        }
        else return TRUE;
        return FALSE;
      }
      else
  #endif // HAVE_RATGRING
  /*==================== Rat def =================*/
      if (strcmp(sys_cmd, "ratVar") == 0)
      {
        int start,end;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          start=pIsPurePower((poly)h->Data());
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          end=pIsPurePower((poly)h->Data());
          h=h->next;
        }
        else return TRUE;
        currRing->real_var_start=start;
        currRing->real_var_end=end;
        return (start==0)||(end==0)||(start>end);
      }
      else
  /*==================== shift-test for freeGB  =================*/
  #ifdef HAVE_SHIFTBBA
      if (strcmp(sys_cmd, "stest") == 0)
      {
        poly p;
        int sh,uptodeg, lVblock;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          sh=(int)((long)(h->Data()));
          h=h->next;
        }
        else return TRUE;

        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          uptodeg=(int)((long)(h->Data()));
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          lVblock=(int)((long)(h->Data()));
          res->data = pLPshift(p,sh,uptodeg,lVblock);
          res->rtyp = POLY_CMD;
        }
        else return TRUE;
        return FALSE;
      }
      else
  #endif
  /*==================== block-test for freeGB  =================*/
  #ifdef HAVE_SHIFTBBA
      if (strcmp(sys_cmd, "btest") == 0)
      {
        poly p;
        int lV;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          lV=(int)((long)(h->Data()));
          res->rtyp = INT_CMD;
          res->data = (void*)(long)pLastVblock(p, lV);
        }
        else return TRUE;
        return FALSE;
      }
      else
  /*==================== shrink-test for freeGB  =================*/
      if (strcmp(sys_cmd, "shrinktest") == 0)
      {
        poly p;
        int lV;
        if ((h!=NULL) && (h->Typ()==POLY_CMD))
        {
          p=(poly)h->CopyD();
          h=h->next;
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          lV=(int)((long)(h->Data()));
          res->rtyp = POLY_CMD;
          //        res->data = p_mShrink(p, lV, currRing);
          //        kStrategy strat=new skStrategy;
          //        strat->tailRing = currRing;
          res->data = p_Shrink(p, lV, currRing);
        }
        else return TRUE;
        return FALSE;
      }
      else
  #endif
  #endif
  /*==================== t-rep-GB ==================================*/
      if (strcmp(sys_cmd, "unifastmult")==0)
      {
        poly f = (poly)h->Data();
        h=h->next;
        poly g=(poly)h->Data();
        res->rtyp=POLY_CMD;
        res->data=unifastmult(f,g,currRing);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "multifastmult")==0)
      {
        poly f = (poly)h->Data();
        h=h->next;
        poly g=(poly)h->Data();
        res->rtyp=POLY_CMD;
        res->data=multifastmult(f,g,currRing);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "mults")==0)
      {
        res->rtyp=INT_CMD ;
        res->data=(void*)(long) Mults();
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "fastpower")==0)
      {
        ring r = currRing;
        poly f = (poly)h->Data();
        h=h->next;
        int n=(int)((long)h->Data());
        res->rtyp=POLY_CMD ;
        res->data=(void*) pFastPower(f,n,r);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "normalpower")==0)
      {
        poly f = (poly)h->Data();
        h=h->next;
        int n=(int)((long)h->Data());
        res->rtyp=POLY_CMD ;
        res->data=(void*) pPower(pCopy(f),n);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "MCpower")==0)
      {
        ring r = currRing;
        poly f = (poly)h->Data();
        h=h->next;
        int n=(int)((long)h->Data());
        res->rtyp=POLY_CMD ;
        res->data=(void*) pFastPowerMC(f,n,r);
        return(FALSE);
      }
      else
      if (strcmp(sys_cmd, "bit_subst")==0)
      {
        ring r = currRing;
        poly outer = (poly)h->Data();
        h=h->next;
        poly inner=(poly)h->Data();
        res->rtyp=POLY_CMD ;
        res->data=(void*) uni_subst_bits(outer, inner,r);
        return(FALSE);
      }
      else
  /*==================== gcd-varianten =================*/
      if (strcmp(sys_cmd, "gcd") == 0)
      {
        if (h==NULL)
        {
#ifdef HAVE_PLURAL
          Print("EZGCD:%d (use EZGCD for gcd of polynomials in char 0)\n",isOn(SW_USE_EZGCD));
          Print("EZGCD_P:%d (use EZGCD_P for gcd of polynomials in char p)\n",isOn(SW_USE_EZGCD_P));
          Print("CRGCD:%d (use chinese Remainder for gcd of polynomials in char 0)\n",isOn(SW_USE_CHINREM_GCD));
          Print("QGCD:%d (use QGCD for gcd of polynomials in alg. ext.)\n",isOn(SW_USE_QGCD));
#endif
          Print("homog:%d (use homog. test for factorization of polynomials)\n",singular_homog_flag);
          return FALSE;
        }
        else
        if ((h!=NULL) && (h->Typ()==STRING_CMD)
        && (h->next!=NULL) && (h->next->Typ()==INT_CMD))
        {
          int d=(int)(long)h->next->Data();
          char *s=(char *)h->Data();
#ifdef HAVE_PLURAL
          if (strcmp(s,"EZGCD")==0) { if (d) On(SW_USE_EZGCD); else Off(SW_USE_EZGCD); } else
          if (strcmp(s,"EZGCD_P")==0) { if (d) On(SW_USE_EZGCD_P); else Off(SW_USE_EZGCD_P); } else
          if (strcmp(s,"CRGCD")==0) { if (d) On(SW_USE_CHINREM_GCD); else Off(SW_USE_CHINREM_GCD); } else
          if (strcmp(s,"QGCD")==0) { if (d) On(SW_USE_QGCD); else Off(SW_USE_QGCD); } else
#endif
          if (strcmp(s,"homog")==0) { if (d) singular_homog_flag=1; else singular_homog_flag=0; } else
          return TRUE;
          return FALSE;
        }
        else return TRUE;
      }
      else
  /*==================== subring =================*/
      if (strcmp(sys_cmd, "subring") == 0)
      {
        if (h!=NULL)
        {
          extern ring rSubring(ring r,leftv v); /* ipshell.cc*/
          res->data=(char *)rSubring(currRing,h);
          res->rtyp=RING_CMD;
          return res->data==NULL;
        }
        else return TRUE;
      }
      else
  /*==================== HNF =================*/
  #ifdef HAVE_NTL
      if (strcmp(sys_cmd, "HNF") == 0)
      {
        if (h!=NULL)
        {
          res->rtyp=h->Typ();
          if (h->Typ()==MATRIX_CMD)
          {
            res->data=(char *)singntl_HNF((matrix)h->Data(), currRing);
            return FALSE;
          }
          else if (h->Typ()==INTMAT_CMD)
          {
            res->data=(char *)singntl_HNF((intvec*)h->Data(), currRing);
            return FALSE;
          }
          else return TRUE;
        }
        else return TRUE;
      }
      else
      if (strcmp(sys_cmd, "LLL") == 0)
      {
        if (h!=NULL)
        {
          res->rtyp=h->Typ();
          if (h->Typ()==MATRIX_CMD)
          {
            res->data=(char *)singntl_LLL((matrix)h->Data(), currRing);
            return FALSE;
          }
          else if (h->Typ()==INTMAT_CMD)
          {
            res->data=(char *)singntl_LLL((intvec*)h->Data(), currRing);
            return FALSE;
          }
          else return TRUE;
        }
        else return TRUE;
      }
      else
  /*================= absBiFact ======================*/
      if (strcmp(sys_cmd, "absFact") == 0)
      {
        if (h!=NULL)
        {
          res->rtyp=LIST_CMD;
          if (h->Typ()==POLY_CMD)
          {
            intvec *v=NULL;
            ideal mipos= NULL;
            int n= 0;
            ideal f=singclap_absFactorize((poly)(h->Data()), mipos, &v, n, currRing);
            if (f==NULL) return TRUE;
            ivTest(v);
            lists l=(lists)omAllocBin(slists_bin);
            l->Init(4);
            l->m[0].rtyp=IDEAL_CMD;
            l->m[0].data=(void *)f;
            l->m[1].rtyp=INTVEC_CMD;
            l->m[1].data=(void *)v;
            l->m[2].rtyp=IDEAL_CMD;
            l->m[2].data=(void*) mipos;
            l->m[3].rtyp=INT_CMD;
            l->m[3].data=(void*) (long) n;
            res->data=(void *)l;
            return FALSE;
          }
          else return TRUE;
        }
        else return TRUE;
      }
      else
  /*================= probIrredTest ======================*/
      if (strcmp (sys_cmd, "probIrredTest") == 0)
      {
        if (h!=NULL && (h->Typ()== POLY_CMD) && ((h->next != NULL) && h->next->Typ() == STRING_CMD))
        {
          CanonicalForm F= convSingPFactoryP((poly)(h->Data()), currRing);
          char *s=(char *)h->next->Data();
          double error= atof (s);
          int irred= probIrredTest (F, error);
          res->rtyp= INT_CMD;
          res->data= (void*)(long)irred;
          return FALSE;
        }
        else return TRUE;
      }
      else
  #endif
  #ifdef ix86_Win
  /*==================== Python Singular =================*/
      if (strcmp(sys_cmd, "python") == 0)
      {
        const char* c;
        if ((h!=NULL) && (h->Typ()==STRING_CMD))
        {
          c=(const char*)h->Data();
          if (!PyInitialized) {
            PyInitialized = 1;
  //          Py_Initialize();
  //          initPySingular();
          }
  //      PyRun_SimpleString(c);
          return FALSE;
        }
        else return TRUE;
      }
      else
  /*==================== Python Singular =================
      if (strcmp(sys_cmd, "ipython") == 0)
      {
        const char* c;
        {
          if (!PyInitialized)
          {
            PyInitialized = 1;
            Py_Initialize();
            initPySingular();
          }
    PyRun_SimpleString(
  "try:                                                                                       \n\
      __IPYTHON__                                                                             \n\
  except NameError:                                                                           \n\
      argv = ['']                                                                             \n\
      banner = exit_msg = ''                                                                  \n\
  else:                                                                                       \n\
      # Command-line options for IPython (a list like sys.argv)                               \n\
      argv = ['-pi1','In <\\#>:','-pi2','   .\\D.:','-po','Out<\\#>:']                        \n\
      banner = '*** Nested interpreter ***'                                                   \n\
      exit_msg = '*** Back in main IPython ***'                                               \n\
                            \n\
  # First import the embeddable shell class                                                   \n\
  from IPython.Shell import IPShellEmbed                                                      \n\
  # Now create the IPython shell instance. Put ipshell() anywhere in your code                \n\
  # where you want it to open.                                                                \n\
  ipshell = IPShellEmbed(argv,banner=banner,exit_msg=exit_msg)                                \n\
  ipshell()");
          return FALSE;
        }
      }
      else
                */

  #endif
/*==================== semaphore =================*/
#ifdef HAVE_SIMPLEIPC
    if (strcmp(sys_cmd,"semaphore")==0)
    {
      if((h!=NULL) && (h->Typ()==STRING_CMD) && (h->next!=NULL) && (h->next->Typ()==INT_CMD))
      {
        int v=1;
        if ((h->next->next!=NULL)&& (h->next->next->Typ()==INT_CMD))
          v=(int)(long)h->next->next->Data();
        res->data=(char *)(long)simpleipc_cmd((char *)h->Data(),(int)(long)h->next->Data(),v);
        res->rtyp=INT_CMD;
        return FALSE;
      }
      else
      {
        WerrorS("Usage: system(\"semaphore\",<cmd>,int)");
        return TRUE;
      }
    }
    else
#endif
/*======================= demon_list =====================*/
  if (strcmp(sys_cmd,"denom_list")==0)
  {
    res->rtyp=LIST_CMD;
    extern lists get_denom_list();
    res->data=(lists)get_denom_list();
    return FALSE;
  }
  else
/*==================== install newstruct =================*/
  if (strcmp(sys_cmd,"install")==0)
  {
    if ((h!=NULL) && (h->Typ()==STRING_CMD)
    && (h->next!=NULL) && (h->next->Typ()==STRING_CMD)
    && (h->next->next!=NULL) && (h->next->next->Typ()==PROC_CMD)
    && (h->next->next->next!=NULL) && (h->next->next->next->Typ()==INT_CMD))
    {
      return newstruct_set_proc((char*)h->Data(),(char*)h->next->Data(),
                                (int)(long)h->next->next->next->Data(),
                                (procinfov)h->next->next->Data());
    }
    return TRUE;
  }
  else
  if (strcmp(sys_cmd,"newstruct")==0)
  {
    if ((h!=NULL) && (h->Typ()==STRING_CMD))
    {
      int id=0;
      blackboxIsCmd((char*)h->Data(),id);
      if (id>0)
      {
        blackbox *bb=getBlackboxStuff(id);
	if (BB_LIKE_LIST(bb))
	{
          newstruct_desc desc=(newstruct_desc)bb->data;
          newstructShow(desc);
          return FALSE;
	}
      }
    }
    return TRUE;
  }
  else
  if (strcmp(sys_cmd,"blackbox")==0)
  {
    printBlackboxTypes();
    return FALSE;
  }
  else
/*==================== reserved port =================*/
  if (strcmp(sys_cmd,"reserve")==0)
  {
    int ssiReservePort(int clients);
    if ((h!=NULL) && (h->Typ()==INT_CMD))
    {
      res->rtyp=INT_CMD;
      int p=ssiReservePort((int)(long)h->Data());
      res->data=(void*)(long)p;
      return (p==0);
    }
    else
    {
      WerrorS("system(\"reserve\",<int>)");
    }
    return TRUE;
  }
  else
  if (strcmp(sys_cmd,"reservedLink")==0)
  {
    extern si_link ssiCommandLink();
    res->rtyp=LINK_CMD;
    si_link p=ssiCommandLink();
    res->data=(void*)p;
    return (p==NULL);
  }
  else
  /*==================== Test Boos Epure ==================================*/
  if (strcmp(sys_cmd, "Hallo")==0)
  {
    n_coeffType nae=nRegister(n_unknown,n_AEInitChar);
    coeffs AE=nInitChar(nae,NULL);
    ring r=currRing;
    rUnComplete(r);
    r->cf=AE;
    rComplete(r,TRUE);
    /*
    // Ab hier wird gespielt
    int_poly* f=new int_poly;
    f->poly_insert();
    int_poly* g=new int_poly;
    g->poly_insert();
    // Ab hier gerechnet
    number a=reinterpret_cast<number> (f);
    number b=reinterpret_cast<number> (g);
    number erg=n_Gcd(a,b,AE);
    int_poly* h= reinterpret_cast<int_poly*> (erg);
    h->poly_print();
*/
    return FALSE;
  }
  else
  /*==================== Test Boos Epure 2 ==================================*/
  if (strcmp(sys_cmd, "Hallo2")==0)
  {
    n_coeffType naeq=nRegister(n_unknown,n_QAEInitChar);
    coeffs AEQ=nInitChar(naeq,NULL);
    ring r=currRing;
    rUnComplete(r);
    r->cf=AEQ;
    rComplete(r,TRUE);

    return FALSE;
  }
  else
  /*==================== Test Boos Epure 3==================================*/
  if (strcmp(sys_cmd, "Hallo3")==0)
  {
    n_coeffType naep=nRegister(n_unknown,n_pAEInitChar);
    coeffs AEp=nInitChar(naep,NULL);
    ring r=currRing;
    rUnComplete(r);
    r->cf=AEp;
    rComplete(r,TRUE);
    //JETZT WOLLEN WIR DOCH MAL SPIELEN

    // Ab hier wird gespielt
    p_poly* f=new p_poly;
    f->p_poly_insert();

    p_poly* g=new p_poly;
    g->p_poly_insert();
    // Ab hier gerechnet
    number a=reinterpret_cast<number> (f);
    number b=reinterpret_cast<number> (g);
    number erg=n_Add(a,b,AEp);
    p_poly* h= reinterpret_cast<p_poly*> (erg);
    h->p_poly_print();

    return FALSE;
  }
  else
/*==================== Error =================*/
      Werror( "(extended) system(\"%s\",...) %s", sys_cmd, feNotImplemented );
  }
  return TRUE;
}

#endif // HAVE_EXTENDED_SYSTEM


