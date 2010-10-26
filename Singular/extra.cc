/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
*/

#define HAVE_WALK 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <kernel/mod2.h>
#include <misc_ip.h>

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

#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipid.h>
#include <kernel/polys.h>
#include <Singular/lists.h>
#include <kernel/kutil.h>
#include <Singular/cntrlc.h>
#include <kernel/stairc.h>
#include <Singular/ipshell.h>
#include <kernel/modulop.h>
#include <kernel/febase.h>
#include <kernel/matpol.h>
#include <kernel/longalg.h>
#include <kernel/ideals.h>
#include <kernel/kstd1.h>
#include <kernel/syz.h>
#include <Singular/sdb.h>
#include <Singular/feOpt.h>
#include <Singular/distrib.h>
#include <kernel/prCopy.h>
#include <kernel/mpr_complex.h>
#include <kernel/ffields.h>

#ifdef HAVE_RINGS
#include <kernel/ringgb.h>
#endif

#ifdef HAVE_GFAN
#include <kernel/gfan.h>
#endif

#ifdef HAVE_F5
#include <Singular/f5gb.h>
#endif

#ifdef HAVE_F5C
#include <Singular/f5c.h>
#endif

#ifdef HAVE_WALK
#include <Singular/walk.h>
#endif

#include <kernel/weight.h>
#include <kernel/fast_mult.h>
#include <kernel/digitech.h>

#ifdef HAVE_SPECTRUM
#include <kernel/spectrum.h>
#endif

#ifdef HAVE_BIFAC
#include <bifac.h>
#endif

#include <Singular/attrib.h>

#if defined(HPUX_10) || defined(HPUX_9)
extern "C" int setenv(const char *name, const char *value, int overwrite);
#endif

#include <kernel/sca.h>
#ifdef HAVE_PLURAL
#include <kernel/ring.h>
#include <kernel/ncSAMult.h> // for CMultiplier etc classes
#include <Singular/ipconv.h>
#include <kernel/ring.h>
#ifdef HAVE_RATGRING
#include <kernel/ratgring.h>
#endif
#endif

#ifdef ix86_Win /* only for the DLLTest */
/* #include "WinDllTest.h" */
#ifdef HAVE_DL
#include <Singular/mod_raw.h>
#endif
#endif

// for tests of t-rep-GB
#include <kernel/tgb.h>

// Define to enable many more system commands
#undef MAKE_DISTRIBUTION
#ifndef MAKE_DISTRIBUTION
#define HAVE_EXTENDED_SYSTEM 1
#endif

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <kernel/clapconv.h>
#include <kernel/kstdfac.h>
#include <factor.h>
#endif
#include <kernel/clapsing.h>

#include <Singular/silink.h>
#include <Singular/walk.h>

#include <kernel/maps.h>

#include <kernel/shiftgb.h>

#ifdef HAVE_EIGENVAL
#include <Singular/eigenval_ip.h>
#endif

#ifdef HAVE_GMS
#include <Singular/gms.h>
#endif

/*
 *   New function/system-calls that will be included as dynamic module
 * should be inserted here.
 * - without HAVE_DYNAMIC_LOADING: these functions comes as system("....");
 * - with    HAVE_DYNAMIC_LOADING: these functions are loaded as module.
 */
//#ifndef HAVE_DYNAMIC_LOADING

#ifdef HAVE_PCV
#include <Singular/pcv.h>
#endif

//#endif /* not HAVE_DYNAMIC_LOADING */

#ifdef ix86_Win
//#include <Python.h>
//#include <python_wrapper.h>
#endif

void piShowProcList();
#ifndef MAKE_DISTRIBUTION
static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h);
#endif

extern BOOLEAN jjJanetBasis(leftv res, leftv v);

#ifdef ix86_Win  /* PySingular initialized? */
static int PyInitialized = 0;
#endif

int singular_homog_flag=1;

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
      res->rtyp=INT_CMD;
      #ifdef _SC_NPROCESSORS_ONLN
      res->data=(void *)sysconf(_SC_NPROCESSORS_ONLN);
      #elif defined(_SC_NPROCESSORS_CONF)
      res->data=(void *)sysconf(_SC_NPROCESSORS_CONF);
      #else
      // dummy, if not defined:
      res->data=(void *)1;
      #endif
      return FALSE;
    }
    else




/*==================== gen ==================================*/
    if(strcmp(sys_cmd,"gen")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)(long)npGen;
      return FALSE;
    }
    else
/*==================== sh ==================================*/
    if(strcmp(sys_cmd,"sh")==0)
    {
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
        res->data=(void *)omStrDup(versionString());
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
          #ifdef HAVE_FACTORY
            TEST_FOR("factory")
            //TEST_FOR("libfac")
          #endif
          #ifdef HAVE_MPSR
            TEST_FOR("MP")
          #endif
          #ifdef HAVE_READLINE
            TEST_FOR("readline")
          #endif
          #ifdef TEST_MAC_ORDER
            TEST_FOR("MAC_ORDER");
          #endif
          // unconditional since 3-1-0-6
            TEST_FOR("Namespaces");
          #ifdef HAVE_DYNAMIC_LOADING
            TEST_FOR("DynamicLoading");
          #endif
          #ifdef HAVE_EIGENVAL
            TEST_FOR("eigenval");
          #endif
          #ifdef HAVE_GMS
            TEST_FOR("gms");
          #endif
            ;
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
        char* b = StringSetS("");
        feStringAppendBrowsers(0);
        res->data = omStrDup(b);
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
  #ifdef HAVE_FACTORY
          factoryseed(siRandomStart);
  #endif
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
            if ( !rField_is_long_C() )
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
        if ( !rField_is_long_C() && !rField_is_long_R() )
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
          number n = mpz2number(m);
          res->rtyp = BIGINT_CMD;
          res->data = (void*)n;
          return FALSE;
        }
        else
        {
          Werror( "expected valid file name as a string");
          return FALSE;
        }
      }
  /*==================== neworder =============================*/
  // should go below
  #ifdef HAVE_FACTORY
      if(strcmp(sys_cmd,"neworder")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          res->rtyp=STRING_CMD;
          res->data=(void *)singclap_neworder((ideal)h->Data());
          return FALSE;
        }
        else
          WerrorS("ideal expected");
      }
      else
  #endif
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


  #if 0
      /// Returns old SyzCompLimit, can set new limit
      if(strcmp(sys_cmd,"SetSyzComp")==0)
      {
        res->rtyp = INT_CMD;
        res->data = (void *)rGetCurrSyzLimit(currRing);
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          const int iSyzComp = (int)((long)(h->Data()));
          assume( iSyzComp > 0 );
          rSetSyzComp( iSyzComp );
        }

        return FALSE;
      }
      /// Endowe the current ring with additional (leading) Syz-component ordering
      if(strcmp(sys_cmd,"MakeSyzCompOrdering")==0)
      {
        extern ring rAssure_SyzComp(const ring r, BOOLEAN complete);

  //    res->data = rCurrRingAssure_SyzComp(); // changes current ring! :(
        res->data = (void *)rAssure_SyzComp(currRing, TRUE);
        res->rtyp = RING_CMD; // return new ring!

        return FALSE;
      }
  #endif

      /// Same for Induced Schreyer ordering (ordering on components is defined by sign!)
      if(strcmp(sys_cmd,"MakeInducedSchreyerOrdering")==0)
      {
        extern ring rAssure_InducedSchreyerOrdering(const ring r, BOOLEAN complete, int sign);
        int sign = 1;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          sign = (int)((long)(h->Data()));
          assume( sign == 1 || sign == -1 );
        }
        res->data = (void *)rAssure_InducedSchreyerOrdering(currRing, TRUE, sign);
        res->rtyp = RING_CMD; // return new ring!
        return FALSE;
      }

      /// Returns old SyzCompLimit, can set new limit
      if(strcmp(sys_cmd,"SetInducedReferrence")==0)
      {
        extern void rSetISReference(const ideal F, const int rank, const int p, const intvec * componentWeights, const ring r);

        if ((h!=NULL) && ( (h->Typ()==IDEAL_CMD) || (h->Typ()==MODUL_CMD)))
        {
          intvec * componentWeights = (intvec *)atGet(h,"isHomog",INTVEC_CMD); // No copy?!

          const ideal F = (ideal)h->Data(); ; // No copy!
          h=h->next;

          int rank = idRankFreeModule(F, currRing, currRing); // Starting syz-comp (1st: i+1)

          if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            rank = (int)((long)(h->Data())); h=h->next;
          }

          int p = 0; // which IS-block? p^th!

          if ((h!=NULL) && (h->Typ()==INT_CMD))
          {
            p = (int)((long)(h->Data())); h=h->next;
          }

          // F & componentWeights belong to that ordering block of currRing now:
          rSetISReference(F, rank, p, componentWeights, currRing); // F and componentWeights will be copied!
        }
        else
        {
          WerrorS("`system(\"SetISReferrence\",<ideal/module>, [int, int])` expected");
          return TRUE;
        }
        return FALSE;
      }


  //    F = system("ISUpdateComponents", F, V, MIN );
  //    // replace gen(i) -> gen(MIN + V[i-MIN]) for all i > MIN in all terms from F!
  //    extern void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r);
      if(strcmp(sys_cmd,"ISUpdateComponents")==0)
      {

        PrintS("ISUpdateComponents:.... \n");

        if ((h!=NULL) && (h->Typ()==MODUL_CMD))
        {
          ideal F = (ideal)h->Data(); ; // No copy!
          h=h->next;

          if ((h!=NULL) && (h->Typ()==INTVEC_CMD))
          {
            const intvec* const V = (const intvec* const) h->Data();
            h=h->next;

            if ((h!=NULL) && (h->Typ()==INT_CMD))
            {
              const int MIN = (int)((long)(h->Data()));

              extern void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r);
              pISUpdateComponents(F, V, MIN, currRing);
              return FALSE;
            }
          }
        }

        WerrorS("`system(\"ISUpdateComponents\",<module>, intvec, int)` expected");
        return TRUE;
      }

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
        if (rIsPluralRing(currRing))  res->data=nc_p_Bracket_qq(p,q);
        else res->data=NULL;
        return FALSE;
      }
      if(strcmp(sys_cmd,"NCUseExtensions")==0)
      {

        if ((h!=NULL) && (h->Typ()==INT_CMD))
          res->data=(void *)setNCExtensions( (int)((long)(h->Data())) );
        else
          res->data=(void *)getNCExtensions();

        res->rtyp=INT_CMD;
        return FALSE;
      }


      if(strcmp(sys_cmd,"NCGetType")==0)
      {
        res->rtyp=INT_CMD;

        if( rIsPluralRing(currRing) )
          res->data=(void *)ncRingType(currRing);
        else
          res->data=(void *)(-1);

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
            res->data = pOppose(Rop,p);
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
        if (res->data == (void*) 0 || res->data == (void*) 1)
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
#  include <kernel/fglmcomb.cc>
#  include <kernel/fglm.h>
#  ifdef HAVE_NEWTON
#    include <hc_newton.h>
#  endif
#  include <mpsr.h>
#  include <kernel/mod_raw.h>
#  include <kernel/ring.h>
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
         verbose|=Sy_bit(23);
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

                short * iv=iv2array(v);
                poly r=0;
                poly hp=ppJetW(f,n,iv);
                int s=MATCOLS(m);
                int j=0;
                matrix T=mpInitI(s,1,0);

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

                matrix Temp=mpTransp((matrix) idVec2Ideal(r));
                matrix R=mpNew(MATCOLS((matrix) idVec2Ideal(f)),1);
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
  #ifdef RDEBUG
        if(strcmp(sys_cmd,"p")==0)
        {
          pDebugPrint((poly)h->Data());
          return FALSE;
        }
        else
  #endif
  /*==================== ring debug ==================================*/
  #ifdef RDEBUG
        if(strcmp(sys_cmd,"r")==0)
        {
          rDebugPrint((ring)h->Data());
          return FALSE;
        }
        else
  #endif
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
          res->data = 0;
          return FALSE;
        }
        else
  /*==================== generic debug ==================================*/
        if(strcmp(sys_cmd,"DetailedPrint")==0)
        {
  #ifndef NDEBUG
          if( h == NULL )
          {
            WarnS("DetailedPrint needs arguments...");
            return TRUE;
          }

          if( h->Typ() == RING_CMD)
          {
            const ring r = (const ring)h->Data();
            rWrite(r);
            PrintLn();
#ifdef RDEBUG
            rDebugPrint(r);
#endif
          }
#ifdef PDEBUG
          else
          if( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD)
          {
            const int nTerms = 3;
            const poly p = (const poly)h->Data();
            p_DebugPrint(p, currRing, currRing, nTerms);
          }
          else
          if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
          {
            const ideal id = (const ideal)h->Data();

            h = h->Next();

            int nTerms = 3;

            if( h!= NULL && h->Typ() == INT_CMD )
            {
              int n = (int)(long)(h->Data());
              if( n < 0 )
              {
                Warn("Negative int argument: %d", n);
              }
              else
                nTerms = n;
            }

            idShow(id, currRing, currRing, nTerms);
          }
#endif
#endif
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
      if(strcmp(sys_cmd,"naIdeal")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          naSetIdeal((ideal)h->Data());
          return FALSE;
        }
        else
           WerrorS("ideal expected");
      }
      else
  /*==================== isSqrFree =============================*/
  #ifdef HAVE_FACTORY
      if(strcmp(sys_cmd,"isSqrFree")==0)
      {
        if ((h!=NULL) &&(h->Typ()==POLY_CMD))
        {
          res->rtyp=INT_CMD;
          res->data=(void *)(long) singclap_isSqrFree((poly)h->Data());
          return FALSE;
        }
        else
          WerrorS("poly expected");
      }
      else
  #endif
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
  #ifdef HAVE_FACTORY
  /*==================== fastcomb =============================*/
      if(strcmp(sys_cmd,"fastcomb")==0)
      {
        if ((h!=NULL) &&(h->Typ()==IDEAL_CMD))
        {
          int i=0;
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
          int i=0;
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
  #endif
  /*==================== listall ===================================*/
      if(strcmp(sys_cmd,"listall")==0)
      {
        int showproc=0;
        if ((h!=NULL) && (h->Typ()==INT_CMD)) showproc=(int)((long)h->Data());
        listall(showproc);
        return FALSE;
      }
      else
  /*==================== proclist =================================*/
      if(strcmp(sys_cmd,"proclist")==0)
      {
        piShowProcList();
        return FALSE;
      }
      else
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
        L->m[1].data=(void *)r.achse;          // flag for unoccupied axes
        L->m[2].rtyp=INT_CMD;
        L->m[2].data=(void *)r.deg;            // #degenerations
        if ( r.deg != 0)              // only if degenerations exist
        {
          L->m[3].rtyp=INT_CMD;
          L->m[3].data=(void *)r.anz_punkte;     // #points
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
          CanonicalForm F( convSingGFFactoryGF( (poly)h->Data() ) );
          res->rtyp=POLY_CMD;
          res->data=convFactoryGFSingGF( F );
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
  #if !defined(NDEBUG)
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
  /*==================== minor =================*/
      if (strcmp(sys_cmd, "minor")==0)
      {
        ring r = currRing;
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
  /*==================== F5C Implementation =================*/
  #ifdef HAVE_F5C
      if (strcmp(sys_cmd, "f5c")==0)
      {
        if (h->Typ()!=IDEAL_CMD)
        {
          WerrorS("ideal expected");
          return TRUE;
        }

        ring r = currRing;
        ideal G = (ideal) h->Data();
        res->rtyp=IDEAL_CMD;
        res->data=(ideal) f5cMain(G,r);
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
        res->data = (void *) testGB(I, GI);
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
            res->data = (void*)scaFirstAltVar(r);
          else
            res->data = (void*)scaLastAltVar(r);
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
        int is;
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
          res->data = (void*)pLastVblock(p, lV);
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
        ring r = currRing;
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
        ring r = currRing;
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
        ring r = currRing;
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
  /*==================== bifac =================*/
  #ifdef HAVE_BIFAC
      if (strcmp(sys_cmd, "bifac")==0)
      {
        if (h->Typ()!=POLY_CMD)
        {
          WerrorS("`system(\"bifac\",<poly>) expected");
          return TRUE;
        }
        if (!rField_is_Q())
        {
          WerrorS("coeff field must be Q");
          return TRUE;
        }
        BIFAC B;
        CFFList C;
        int sw_rat=isOn(SW_RATIONAL);
        On(SW_RATIONAL);
        CanonicalForm F( convSingPClapP((poly)(h->Data())));
        B.bifac(F, 1);
        CFFList L=B.getFactors();
        // construct the ring ==============================================
        int i;
        int lev=ExtensionLevel();
        char **names=(char**)omAlloc0(lev*sizeof(char_ptr));
        for(i=1;i<=lev; i++)
        {
          StringSetS("");
          names[i-1]=omStrDup(StringAppend("a(%d)",i));
        }
        ring alg_ring=rDefault(0,lev,names);
        ring new_ring=rCopy0(currRing); // all variable names, ordering etc.
        new_ring->P=lev;
        new_ring->parameter=names;
        new_ring->algring=alg_ring;
        new_ring->ch=1;
        rComplete(new_ring,TRUE);
        // set the mipo ===============================================
        ring save_currRing=currRing; idhdl save_currRingHdl=currRingHdl;
        rChangeCurrRing(alg_ring);
        ideal mipo_id=idInit(lev,1);
        for (i=lev; i>0;i--)
        {
          CanonicalForm Mipo=getMipo(Variable(-i),Variable(i));
          mipo_id->m[i-1]=convClapPSingP(Mipo);
        }
        idShow(mipo_id);
        alg_ring->qideal=mipo_id;
        rChangeCurrRing(new_ring);
        for (i=lev-1; i>=0;i--)
        {
          poly p=pOne();
          lnumber n=(lnumber)pGetCoeff(p);
          // no need to delete nac 1
          n->z=(napoly)mipo_id->m[i];
          mipo_id->m[i]=p;
        }
        new_ring->minideal=id_Copy(alg_ring->qideal,new_ring);
        // convert factors =============================================
        ideal fac_id=idInit(L.length(),1);
        CFFListIterator J=L;
        i=0;
        intvec *v = new intvec( L.length() );
        for ( ; J.hasItem(); J++,i++ )
        {
          fac_id->m[i]=convClapAPSingAP( J.getItem().factor() );
          (*v)[i]=J.getItem().exp();
        }
        idhdl hh=enterid("factors",0,LIST_CMD,&(currRing->idroot),FALSE);
        lists LL=(lists)omAllocBin( slists_bin);
        LL->Init(2);
        LL->m[0].rtyp=IDEAL_CMD;
        LL->m[0].data=(char *)fac_id;
        LL->m[1].rtyp=INTVEC_CMD;
        LL->m[1].data=(char *)v;
        IDDATA(hh)=(char *)LL;

        rChangeCurrRing(save_currRing);
        currRingHdl=save_currRingHdl;
        if (!sw_rat) Off(SW_RATIONAL);

        res->data=new_ring;
        res->rtyp=RING_CMD;
        return FALSE;
      }
      else
  #endif
  /*==================== gcd-varianten =================*/
  #ifdef HAVE_FACTORY
      if (strcmp(sys_cmd, "gcd") == 0)
      {
        if (h==NULL)
        {
#ifdef HAVE_PLURAL
          Print("NTL_0:%d (use NTL for gcd of polynomials in char 0)\n",isOn(SW_USE_NTL_GCD_0));
          Print("NTL_p:%d (use NTL for gcd of polynomials in char p)\n",isOn(SW_USE_NTL_GCD_P));
          Print("EZGCD:%d (use EZGCD for gcd of polynomials in char 0)\n",isOn(SW_USE_EZGCD));
          Print("EZGCD_P:%d (use EZGCD_P for gcd of polynomials in char p)\n",isOn(SW_USE_EZGCD_P));
          Print("CRGCD:%d (use chinese Remainder for gcd of polynomials in char 0)\n",isOn(SW_USE_CHINREM_GCD));
          Print("SPARSEMOD:%d (use SPARSEMOD for gcd of polynomials in char 0)\n",isOn(SW_USE_SPARSEMOD));
          Print("QGCD:%d (use QGCD for gcd of polynomials in alg. ext.)\n",isOn(SW_USE_QGCD));
          Print("FGCD:%d (use fieldGCD for gcd of polynomials in Z/p)\n",isOn(SW_USE_fieldGCD));
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
          if (strcmp(s,"NTL_0")==0) { if (d) On(SW_USE_NTL_GCD_0); else Off(SW_USE_NTL_GCD_0); } else
          if (strcmp(s,"NTL_p")==0) { if (d) On(SW_USE_NTL_GCD_P); else Off(SW_USE_NTL_GCD_P); } else
          if (strcmp(s,"EZGCD")==0) { if (d) On(SW_USE_EZGCD); else Off(SW_USE_EZGCD); } else
          if (strcmp(s,"EZGCD_P")==0) { if (d) On(SW_USE_EZGCD_P); else Off(SW_USE_EZGCD_P); } else
          if (strcmp(s,"CRGCD")==0) { if (d) On(SW_USE_CHINREM_GCD); else Off(SW_USE_CHINREM_GCD); } else
          if (strcmp(s,"SPARSEMOD")==0) { if (d) On(SW_USE_SPARSEMOD); else Off(SW_USE_SPARSEMOD); } else
          if (strcmp(s,"QGCD")==0) { if (d) On(SW_USE_QGCD); else Off(SW_USE_QGCD); } else
          if (strcmp(s,"FGCD")==0) { if (d) On(SW_USE_fieldGCD); else Off(SW_USE_fieldGCD); } else
#endif
          if (strcmp(s,"homog")==0) { if (d) singular_homog_flag=1; else singular_homog_flag=0; } else
          return TRUE;
          return FALSE;
        }
        else return TRUE;
      }
      else
  #endif
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
  #ifdef HAVE_FACTORY
      if (strcmp(sys_cmd, "HNF") == 0)
      {
        if (h!=NULL)
        {
          res->rtyp=h->Typ();
          if (h->Typ()==MATRIX_CMD)
          {
            res->data=(char *)singntl_HNF((matrix)h->Data());
            return FALSE;
          }
          else if (h->Typ()==INTMAT_CMD)
          {
            res->data=(char *)singntl_HNF((intvec*)h->Data());
            return FALSE;
          }
          else return TRUE;
        }
        else return TRUE;
      }
      else
  /*================= factoras =========================*/
      if (strcmp (sys_cmd, "factoras") == 0)
      {
        if (h!=NULL && (h->Typ()== POLY_CMD) && (h->next->Typ() == IDEAL_CMD))
        {
          CanonicalForm F( convSingTrPFactoryP((poly)(h->Data())));
          h= h->next;
          ideal I= ((ideal) h->Data());
          int i= IDELEMS (I);
          CFList as;
          for (int j= 0; j < i; j++)
            as.append (convSingTrPFactoryP (I->m[j]));
          int success= 0;
          CFFList libfacResult= newfactoras (F, as, success);
          if (success >= 0)
          {
            //convert factors
            ideal factors= idInit(libfacResult.length(),1);
            CFFListIterator j= libfacResult;
            i= 0;
            intvec *mult= new intvec (libfacResult.length());
            for ( ; j.hasItem(); j++,i++ )
            {
              factors->m[i]= convFactoryPSingTrP (j.getItem().factor());
              (*mult)[i]= j.getItem().exp();
            }
            lists l= (lists)omAllocBin( slists_bin);
            l->Init(2);
            l->m[0].rtyp= IDEAL_CMD;
            l->m[0].data= (char *) factors;
            l->m[1].rtyp= INTVEC_CMD;
            l->m[1].data= (char *) mult;
            res->data= l;
            res->rtyp= LIST_CMD;
            if (success == 0)
              WerrorS ("factorization maybe incomplete");
            return FALSE;
          }
          else
          {
            WerrorS("problem in libfac");
            return TRUE;
          }
        }
        else
        {
          WerrorS("`system(\"factoras\",<poly>,<ideal>) expected");
          return TRUE;
        }
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

  // TODO: What about a dynamic module instead? Only Linux? NO:
  // ONLY: ELF systems and HPUX
  #ifdef HAVE_SINGULAR_PLUS_PLUS
    if (strcmp(sys_cmd,"Singular++")==0)
    {
  //    using namespace SINGULAR_NS;
      extern BOOLEAN Main(leftv res, leftv h); // FALSE = Ok, TRUE = Error!
      return Main(res, h);
    }
    else
  #endif // HAVE_SINGULAR_PLUS_PLUS

#ifdef HAVE_GFAN
  /*======== GFAN ==============*/
  /*
   WILL HAVE TO CHANGE RETURN TYPE TO LIST_CMD
  */
  if (strcmp(sys_cmd,"grfan")==0)
  {
    /*
    heuristic:
    0 = keep all Gröbner bases in memory
    1 = write all Gröbner bases to disk and read whenever necessary
    2 = use a mixed heuristic, based on length of Gröbner bases
    */
    if( h!=NULL && h->Typ()==IDEAL_CMD && h->next!=NULL && h->next->Typ()==INT_CMD)
    {
      int heuristic;
      heuristic=(int)(long)h->next->Data();
      ideal I=((ideal)h->Data());
      res->rtyp=LIST_CMD;
      res->data=(lists) gfan(I,heuristic);
      return FALSE;
    }
    else
    {
      WerrorS("Usage: system(\"grfan\",I,int)");
      return TRUE;
    }
  }
  else
#endif
/*==================== Error =================*/
      Werror( "(extended) system(\"%s\",...) %s", sys_cmd, feNotImplemented );
  }
  return TRUE;
}

#endif // HAVE_EXTENDED_SYSTEM


