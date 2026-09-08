/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
* jjSYSTEM: official commands, must be documented in the manual,
*           #defines must be local to each command
* jjEXTENDED_SYSTEM: tests, temporary comands etc.
*/

#define HAVE_WALK 1

#include <errno.h>
#include "kernel/mod2.h"
#include "misc/sirandom.h"
#include "resources/omFindExec.h"

#ifdef HAVE_CCLUSTER
#undef CC
#include "ccluster/ccluster.h"
#endif

#include "factory/factory.h"

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

#include "misc/options.h"

// #include "coeffs/ffields.h"
#include "coeffs/coeffs.h"
#include "coeffs/mpr_complex.h"


#include "resources/feResource.h"
#include "polys/monomials/ring.h"
#include "kernel/polys.h"

#include "polys/monomials/maps.h"
#include "polys/matpol.h"
#include "polys/pCoeff.h"

#include "polys/weight.h"

#ifdef HAVE_SHIFTBBA
#include "polys/shiftop.h"
#endif

#include "coeffs/bigintmat.h"
#include "kernel/fast_mult.h"
#include "kernel/digitech.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/syz.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kverify.h"

#include "kernel/linear_algebra/linearAlgebra.h"

#include "kernel/combinatorics/hutil.h"

// for tests of t-rep-GB
#include "kernel/GBEngine/tgb.h"

#include "kernel/linear_algebra/minpoly.h"

#include "numeric/mpr_base.h"

#include "tok.h"
#include "ipid.h"
#include "lists.h"
#include "cntrlc.h"
#include "ipshell.h"
#include "sdb.h"
#include "feOpt.h"
#include "fehelp.h"
#include "misc/distrib.h"

#include "misc_ip.h"

#include "attrib.h"

#include "links/silink.h"
#include "links/ssiLink.h"
#include "walk.h"
#include "Singular/newstruct.h"
#include "Singular/blackbox.h"
#include "Singular/pyobject_setup.h"


#include "kernel/GBEngine/ringgb.h"

#ifdef HAVE_WALK
#include "walk.h"
#endif

#ifdef HAVE_SPECTRUM
#include "kernel/spectrum/spectrum.h"
#endif

#ifdef HAVE_PLURAL
#include "polys/nc/nc.h"
#include "polys/nc/ncSAMult.h" // for CMultiplier etc classes
#include "polys/nc/sca.h"
#include "kernel/GBEngine/nc.h"
#include "ipconv.h"
#ifdef HAVE_RATGRING
#include "kernel/GBEngine/ratgring.h"
#endif
#endif

#ifdef __CYGWIN__ /* only for the DLLTest */
/* #include "WinDllTest.h" */
#ifdef HAVE_DL
#include "polys/mod_raw.h"
#endif
#endif

// Define to enable many more system commands
//#undef MAKE_DISTRIBUTION
#ifndef MAKE_DISTRIBUTION
#define HAVE_EXTENDED_SYSTEM 1
#endif

#include "polys/flintconv.h"
#include "polys/clapconv.h"
#include "kernel/GBEngine/kstdfac.h"

#include "polys/clapsing.h"

#ifdef HAVE_EIGENVAL
#include "eigenval_ip.h"
#endif

#ifdef HAVE_GMS
#include "gms.h"
#endif

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif

#ifdef HAVE_PCV
#include "pcv.h"
#endif

#ifndef MAKE_DISTRIBUTION
static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h);
#endif
static BOOLEAN jjEXPERIMENTAL_SYSTEM(leftv res, leftv h,
                                     BOOLEAN *handled);
EXTERN_VAR BOOLEAN FE_OPT_NO_SHELL_FLAG;

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
/*==================== alarm ==================================*/
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
/*==================== content ==================================*/
    if(strcmp(sys_cmd,"content")==0)
    {
      if ((h!=NULL) && ((h->Typ()==POLY_CMD)||(h->Typ()==VECTOR_CMD)))
      {
        int t=h->Typ();
        poly p=(poly)h->CopyD();
        if (p!=NULL)
        {
           if (!nCoeff_is_Ring(currRing->cf)) p_Cleardenom(p,currRing);
          p_Content(p,currRing);
        }
        res->data=(void *)p;
        res->rtyp=t;
        return FALSE;
      }
      return TRUE;
    }
    else
/*==================== cpu ==================================*/
    if(strcmp(sys_cmd,"cpu")==0)
    {
      #if 0
      long cpu=1;
      #ifdef _SC_NPROCESSORS_ONLN
      cpu=sysconf(_SC_NPROCESSORS_ONLN);
      #elif defined(_SC_NPROCESSORS_CONF)
      cpu=sysconf(_SC_NPROCESSORS_CONF);
      #endif
      res->data=(void *)cpu;
      #else
      res->data=(void *)feOptValue(FE_OPT_CPUS);
      #endif
      res->rtyp=INT_CMD;
      return FALSE;
    }
    else
/*==================== executable ==================================*/
    if(strcmp(sys_cmd,"executable")==0)
    {
      if ((h!=NULL) && (h->Typ()==STRING_CMD))
      {
        char tbuf[MAXPATHLEN];
        char *s=omFindExec((char*)h->Data(),tbuf);
        if(s==NULL) s=(char*)"";
        res->data=(void *)omStrDup(s);
        res->rtyp=STRING_CMD;
        return FALSE;
      }
      return TRUE;
    }
    else
  /*==================== flatten =============================*/
    if(strcmp(sys_cmd,"flatten")==0)
    {
      if ((h!=NULL) &&(h->Typ()==SMATRIX_CMD))
      {
        res->data=(char*)sm_Flatten((ideal)h->Data(),currRing);
        res->rtyp=SMATRIX_CMD;
        return FALSE;
      }
      else
        WerrorS("smatrix expected");
    }
    else
  /*==================== unflatten =============================*/
    if(strcmp(sys_cmd,"unflatten")==0)
    {
      const short t1[]={2,SMATRIX_CMD,INT_CMD};
      if (iiCheckTypes(h,t1,1))
      {
        res->data=(char*)sm_UnFlatten((ideal)h->Data(),(int)(long)h->next->Data(),currRing);
        res->rtyp=SMATRIX_CMD;
        return res->data==NULL;
      }
      else return TRUE;
    }
    else
  /*==================== neworder =============================*/
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
/*===== nc_hilb ===============================================*/
   // Hilbert series of non-commutative monomial algebras
    if(strcmp(sys_cmd,"nc_hilb") == 0)
    {
      ideal i; int lV;
      bool ig = FALSE;
      bool mgrad = FALSE;
      bool autop = FALSE;
      int trunDegHs=0;
      if((h != NULL)&&(h->Typ() == IDEAL_CMD))
        i = (ideal)h->Data();
      else
      {
        WerrorS("nc_Hilb:ideal expected");
        return TRUE;
      }
      h = h->next;
      if((h != NULL)&&(h->Typ() == INT_CMD))
        lV = (int)(long)h->Data();
      else
      {
        WerrorS("nc_Hilb:int expected");
        return TRUE;
      }
      h = h->next;
      while(h != NULL)
      {
        if((int)(long)h->Data() == 1)
          ig = TRUE;
        else if((int)(long)h->Data() == 2)
          mgrad = TRUE;
        else if(h->Typ()==STRING_CMD)
           autop = TRUE;
        else if(h->Typ() == INT_CMD)
          trunDegHs = (int)(long)h->Data();
        h = h->next;
      }
      if(h != NULL)
      {
        WerrorS("nc_Hilb:int 1,2, total degree for the truncation, and a string                  for printing the details are expected");
        return TRUE;
      }

      HilbertSeries_OrbitData(i, lV, ig, mgrad, autop, trunDegHs);
      return(FALSE);
    }
    else
/* ====== verify ============================*/
    if(strcmp(sys_cmd,"verifyGB")==0)
    {
      if (rIsNCRing(currRing))
      {
        WerrorS("system(\"verifyGB\",<ideal>,..) expects a commutative ring");
        return TRUE;
      }
      if (((h->Typ()!=IDEAL_CMD)&&(h->Typ()!=MODUL_CMD))
      || (h->next!=NULL))
      {
        Werror("expected system(\"verifyGB\",<ideal/module>), found <%s>",Tok2Cmdname(h->Typ()));
        return TRUE;
      }
      ideal F=(ideal)h->Data();
      res->data=(char*)(long) kVerify(F,currRing->qideal);
      res->rtyp=INT_CMD;
      return FALSE;
    }
    else
/*===== rcolon ===============================================*/
  if(strcmp(sys_cmd,"rcolon") == 0)
  {
    const short t1[]={3,IDEAL_CMD,POLY_CMD,INT_CMD};
    if (iiCheckTypes(h,t1,1))
    {
      ideal i = (ideal)h->Data();
      h = h->next;
      poly w=(poly)h->Data();
      h = h->next;
      int lV = (int)(long)h->Data();
      res->rtyp = IDEAL_CMD;
      res->data = RightColonOperation(i, w, lV);
      return(FALSE);
    }
    else
      return TRUE;
  }
  else

/*==================== sh ==================================*/
    if(strcmp(sys_cmd,"sh")==0)
    {
      if (FE_OPT_NO_SHELL_FLAG)
      {
        WerrorS("shell execution is disallowed in restricted mode");
        return TRUE;
      }
      res->rtyp=INT_CMD;
      if (h==NULL) res->data = (void *)(long) system("sh");
      else if (h->Typ()==STRING_CMD)
        res->data = (void*)(long) system((char*)(h->Data()));
      else
        WerrorS("string expected");
      if (errno==ECHILD) res->data=NULL;
      return FALSE;
    }
    else
/*========reduce procedure like the global one but with jet bounds=======*/
    if(strcmp(sys_cmd,"reduce_bound")==0)
    {
      poly p=NULL;
      ideal pid=NULL;
      const short t1[]={3,POLY_CMD,IDEAL_CMD,INT_CMD};
      const short t2[]={3,IDEAL_CMD,IDEAL_CMD,INT_CMD};
      const short t3[]={3,VECTOR_CMD,MODUL_CMD,INT_CMD};
      const short t4[]={3,MODUL_CMD,MODUL_CMD,INT_CMD};
      if ((iiCheckTypes(h,t1,0))||((iiCheckTypes(h,t3,0))))
      {
        p = (poly)h->CopyD();
      }
      else if  ((iiCheckTypes(h,t2,0))||(iiCheckTypes(h,t4,1)))
      {
        pid = (ideal)h->CopyD();
      }
      else return TRUE;
      //int htype;
      res->rtyp= h->Typ(); /*htype*/
      ideal q = (ideal)h->next->CopyD();
      int bound = (int)(long)h->next->next->Data();
      if (pid==NULL) /*(htype == POLY_CMD || htype == VECTOR_CMD)*/
        res->data = (char *)kNFBound(q,currRing->qideal,p,bound);
      else /*(htype == IDEAL_CMD || htype == MODUL_CMD)*/
        res->data = (char *)kNFBound(q,currRing->qideal,pid,bound);
      return FALSE;
    }
    else
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
      const short t[]={2,STRING_CMD,STRING_CMD};
      if (iiCheckTypes(h,t,1))
      {
        res->rtyp=STRING_CMD;
        setenv((char *)h->Data(), (char *)h->next->Data(), 1);
        res->data=(void *)omStrDup((char *)h->next->Data());
        feReInitResources();
        return FALSE;
      }
      else
      {
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
    if (strcmp(sys_cmd, "SingularBin") == 0)
    {
      res->rtyp=STRING_CMD;
      const char *r=feResource('r');
      if (r == NULL) r="/usr/local";
      int l=strlen(r);
      /* where to find Singular's programs: */
      #define SINGULAR_PROCS_DIR "/libexec/singular/MOD"
      int ll=si_max((int)strlen(SINGULAR_PROCS_DIR),(int)strlen(LIBEXEC_DIR));
      char *s=(char*)omAlloc(l+ll+2);
      if ((strstr(r,".libs/..")==NULL)   /*not installed Singular (libtool)*/
      &&(strstr(r,"Singular/..")==NULL)) /*not installed Singular (static)*/
      {
        strcpy(s,r);
        strcat(s,SINGULAR_PROCS_DIR);
        if (access(s,X_OK)==0)
        {
          strcat(s,"/");
        }
        else
        {
          /*second try: LIBEXEC_DIR*/
          strcpy(s,LIBEXEC_DIR);
          if (access(s,X_OK)==0)
          {
            strcat(s,"/");
          }
          else
          {
            s[0]='\0';
          }
        }
      }
      else
      {
        const char *r=feResource('b');
        if (r == NULL)
        {
          s[0]='\0';
        }
        else
        {
          strcpy(s,r);
          strcat(s,"/");
        }
      }
      res->data = (void*)s;
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
        WerrorS("Use 'system(\"--\");' for listing of available options");
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
      const char* errormsg=NULL;
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
      else if (h->Typ()==STRING_CMD)
      {
        errormsg = feSetOptValue(opt, (char*) h->Data());
        if (errormsg != NULL)
          Werror("Option '--%s=%s' %s", sys_cmd, (char*) h->Data(), errormsg);
      }
      else
      {
        WerrorS("Need string or int argument to set option value");
        return TRUE;
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
      const short t[]={1,INT_CMD};
      if (h!=NULL)
      {
        if (iiCheckTypes(h,t,1))
        {
          siRandomStart=(int)((long)h->Data());
          siSeed=siRandomStart;
          factoryseed(siRandomStart);
          return FALSE;
        }
        else
        {
          return TRUE;
        }
      }
      res->rtyp=INT_CMD;
      res->data=(void*)(long) siSeed;
      return FALSE;
    }
    else
  /*======================= demon_list =====================*/
    if (strcmp(sys_cmd,"denom_list")==0)
    {
      res->rtyp=LIST_CMD;
      extern lists get_denom_list();
      res->data=(lists)get_denom_list();
      return FALSE;
    }
    else
    /*==================== complexNearZero ======================*/
    if(strcmp(sys_cmd,"complexNearZero")==0)
    {
      const short t[]={2,NUMBER_CMD,INT_CMD};
      if (iiCheckTypes(h,t,1))
      {
        if ( !rField_is_long_C(currRing) )
        {
          WerrorS( "unsupported ground field!");
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
        return TRUE;
      }
    }
    else
  /*==================== getPrecDigits ======================*/
    if(strcmp(sys_cmd,"getPrecDigits")==0)
    {
      if ( (currRing==NULL)
      ||  (!rField_is_long_C(currRing) && !rField_is_long_R(currRing)))
      {
        WerrorS( "unsupported ground field!");
        return TRUE;
      }
      res->rtyp=INT_CMD;
      res->data=(void*)(long)gmp_output_digits;
      //if (gmp_output_digits!=getGMPFloatDigits())
      //{ Print("%d, %d\n",getGMPFloatDigits(),gmp_output_digits);}
      return FALSE;
    }
    else
  /*==================== lduDecomp ======================*/
    if(strcmp(sys_cmd, "lduDecomp")==0)
    {
      const short t[]={1,MATRIX_CMD};
      if (iiCheckTypes(h,t,1))
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
        return TRUE;
      }
    }
    else
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
      const short t[]={7,MATRIX_CMD,MATRIX_CMD,MATRIX_CMD,MATRIX_CMD,POLY_CMD,POLY_CMD,MATRIX_CMD};
      if (!iiCheckTypes(h,t,1))
      {
        return TRUE;
      }
      if (rField_is_Ring(currRing))
      {
        WerrorS("field required");
        return TRUE;
      }
      matrix pMat  = (matrix)h->Data();
      matrix lMat  = (matrix)h->next->Data();
      matrix dMat  = (matrix)h->next->next->Data();
      matrix uMat  = (matrix)h->next->next->next->Data();
      poly l       = (poly)  h->next->next->next->next->Data();
      poly u       = (poly)  h->next->next->next->next->next->Data();
      poly lTimesU = (poly)  h->next->next->next->next->next->next->Data();
      matrix bVec  = (matrix)h->next->next->next->next->next->next->next->Data();
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
    else
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
/*==================== reserved port =================*/
    if (strcmp(sys_cmd,"reserve")==0)
    {
      const short t[]={1,INT_CMD};
      if (iiCheckTypes(h,t,1))
      {
        res->rtyp=INT_CMD;
        int p=ssiReservePort((int)(long)h->Data());
        res->data=(void*)(long)p;
        return (p==0);
      }
      return TRUE;
    }
    else
/*==================== reserved link =================*/
    if (strcmp(sys_cmd,"reservedLink")==0)
    {
      res->rtyp=LINK_CMD;
      si_link p=ssiCommandLink();
      res->data=(void*)p;
      return (p==NULL);
    }
    else
/*==================== install newstruct =================*/
    if (strcmp(sys_cmd,"install")==0)
    {
      const short t[]={4,STRING_CMD,STRING_CMD,PROC_CMD,INT_CMD};
      if (iiCheckTypes(h,t,1))
      {
        return newstruct_set_proc((char*)h->Data(),(char*)h->next->Data(),
                                (int)(long)h->next->next->next->Data(),
                                (procinfov)h->next->next->Data());
      }
      return TRUE;
    }
    else
/*==================== newstruct =================*/
    if (strcmp(sys_cmd,"newstruct")==0)
    {
      const short t[]={1,STRING_CMD};
      if (iiCheckTypes(h,t,1))
      {
        int id=0;
        char *n=(char*)h->Data();
        blackboxIsCmd(n,id);
        if (id>0)
        {
          blackbox *bb=getBlackboxStuff(id);
          if (BB_LIKE_LIST(bb))
          {
            newstruct_desc desc=(newstruct_desc)bb->data;
            newstructShow(desc);
            return FALSE;
          }
          else Werror("'%s' is not a newstruct",n);
        }
        else Werror("'%s' is not a blackbox object",n);
      }
      return TRUE;
    }
    else
/*==================== blackbox =================*/
    if (strcmp(sys_cmd,"blackbox")==0)
    {
      printBlackboxTypes();
      return FALSE;
    }
    else
  /*================= absBiFact ======================*/
    #if defined(HAVE_FLINT) || defined(HAVE_NTL)
    if (strcmp(sys_cmd, "absFact") == 0)
    {
      const short t[]={1,POLY_CMD};
      if (iiCheckTypes(h,t,1)
      && (currRing!=NULL)
      && (getCoeffType(currRing->cf)==n_transExt))
      {
        res->rtyp=LIST_CMD;
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
    else
    #endif
  /* =================== LLL via NTL ==============================*/
  #ifdef HAVE_NTL
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
          res->data=(char *)singntl_LLL((intvec*)h->Data());
          return FALSE;
        }
        else return TRUE;
      }
      else return TRUE;
    }
    else
  #endif
  /* =================== LLL via Flint ==============================*/
  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20500
    if (strcmp(sys_cmd, "LLL_Flint") == 0)
    {
      if (h!=NULL)
      {
        if(h->next == NULL)
        {
            res->rtyp=h->Typ();
            if (h->Typ()==BIGINTMAT_CMD)
            {
              res->data=(char *)singflint_LLL((bigintmat*)h->Data(), NULL);
              return FALSE;
            }
            else if (h->Typ()==INTMAT_CMD)
            {
              res->data=(char *)singflint_LLL((intvec*)h->Data(), NULL);
              return FALSE;
            }
            else return TRUE;
        }
        if(h->next->Typ()!= INT_CMD)
        {
            WerrorS("matrix,int or bigint,int expected");
            return TRUE;
        }
        if(h->next->Typ()== INT_CMD)
        {
            if(((int)((long)(h->next->Data())) != 0) && (int)((long)(h->next->Data()) != 1))
            {
                WerrorS("int is different from 0, 1");
                return TRUE;
            }
            res->rtyp=h->Typ();
            if((long)(h->next->Data()) == 0)
            {
                if (h->Typ()==BIGINTMAT_CMD)
                {
                  res->data=(char *)singflint_LLL((bigintmat*)h->Data(), NULL);
                  return FALSE;
                }
                else if (h->Typ()==INTMAT_CMD)
                {
                  res->data=(char *)singflint_LLL((intvec*)h->Data(), NULL);
                  return FALSE;
                }
                else return TRUE;
            }
            // This will give also the transformation matrix U s.t. res = U * m
            if((long)(h->next->Data()) == 1)
            {
                if (h->Typ()==BIGINTMAT_CMD)
                {
                  bigintmat* m = (bigintmat*)h->Data();
                  bigintmat* T = new bigintmat(m->rows(),m->rows(),m->basecoeffs());
                  for(int i = 1; i<=m->rows(); i++)
                  {
                    n_Delete(&(BIMATELEM(*T,i,i)),T->basecoeffs());
                    BIMATELEM(*T,i,i)=n_Init(1, T->basecoeffs());
                  }
                  m = singflint_LLL(m,T);
                  lists L = (lists)omAllocBin(slists_bin);
                  L->Init(2);
                  L->m[0].rtyp = BIGINTMAT_CMD;  L->m[0].data = (void*)m;
                  L->m[1].rtyp = BIGINTMAT_CMD;  L->m[1].data = (void*)T;
                  res->data=L;
                  res->rtyp=LIST_CMD;
                  return FALSE;
                }
                else if (h->Typ()==INTMAT_CMD)
                {
                  intvec* m = (intvec*)h->Data();
                  intvec* T = new intvec(m->rows(),m->rows(),(int)0);
                  for(int i = 1; i<=m->rows(); i++)
                    IMATELEM(*T,i,i)=1;
                  m = singflint_LLL(m,T);
                  lists L = (lists)omAllocBin(slists_bin);
                  L->Init(2);
                  L->m[0].rtyp = INTMAT_CMD;  L->m[0].data = (void*)m;
                  L->m[1].rtyp = INTMAT_CMD;  L->m[1].data = (void*)T;
                  res->data=L;
                  res->rtyp=LIST_CMD;
                  return FALSE;
                }
                else return TRUE;
            }
        }

      }
      else return TRUE;
    }
    else
  #endif
  #endif
/* ====== rref ============================*/
  #if defined(HAVE_FLINT) || defined(HAVE_NTL)
  if(strcmp(sys_cmd,"rref")==0)
  {
    const short t1[]={1,MATRIX_CMD};
    const short t2[]={1,SMATRIX_CMD};
    if (iiCheckTypes(h,t1,0))
    {
      matrix M=(matrix)h->Data();
      #if defined(HAVE_FLINT)
      res->data=(void*)singflint_rref(M,currRing);
      #elif defined(HAVE_NTL)
      res->data=(void*)singntl_rref(M,currRing);
      #endif
      res->rtyp=MATRIX_CMD;
      return FALSE;
    }
    else if (iiCheckTypes(h,t2,1))
    {
      ideal M=(ideal)h->Data();
      #if defined(HAVE_FLINT)
      res->data=(void*)singflint_rref(M,currRing);
      #elif defined(HAVE_NTL)
      res->data=(void*)singntl_rref(M,currRing);
      #endif
      res->rtyp=SMATRIX_CMD;
      return FALSE;
    }
    else
    {
      WerrorS("expected system(\"rref\",<matrix>/<smatrix>)");
      return TRUE;
    }
  }
  else
  #endif
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
  /*==================== hessenberg/eigenvalues ==================================*/
  #ifdef HAVE_EIGENVAL
    if(strcmp(sys_cmd,"hessenberg")==0)
    {
      return evHessenberg(res,h);
    }
    else
  #endif
  /*==================== eigenvalues ==================================*/
  #ifdef HAVE_EIGENVAL
    if(strcmp(sys_cmd,"eigenvals")==0)
    {
      return evEigenvals(res,h);
    }
    else
  #endif
  /*==================== rowelim ==================================*/
  #ifdef HAVE_EIGENVAL
    if(strcmp(sys_cmd,"rowelim")==0)
    {
      return evRowElim(res,h);
    }
    else
  #endif
  /*==================== rowcolswap ==================================*/
  #ifdef HAVE_EIGENVAL
    if(strcmp(sys_cmd,"rowcolswap")==0)
    {
      return evSwap(res,h);
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
      if ((h==NULL) || (h->Typ()!=POLY_CMD))
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
      const short t[]={2,LIST_CMD,LIST_CMD};
      if (iiCheckTypes(h,t,1))
      {
        return spaddProc(res,h,h->next);
      }
      return TRUE;
    }
    else
  /*==================== spmul =============================*/
    if(strcmp(sys_cmd,"spmul") == 0)
    {
      const short t[]={2,LIST_CMD,INT_CMD};
      if (iiCheckTypes(h,t,1))
      {
        return spmulProc(res,h,h->next);
      }
      return TRUE;
    }
    else
  #endif
/*==================== tensorModuleMult ========================= */
  #define HAVE_SHEAFCOH_TRICKS 1

  #ifdef HAVE_SHEAFCOH_TRICKS
    if(strcmp(sys_cmd,"tensorModuleMult")==0)
    {
      const short t[]={2,INT_CMD,MODUL_CMD};
  //      WarnS("tensorModuleMult!");
      if (iiCheckTypes(h,t,1))
      {
        int m = (int)( (long)h->Data() );
        ideal M = (ideal)h->next->Data();
        res->rtyp=MODUL_CMD;
        res->data=(void *)id_TensorModuleMult(m, M, currRing);
        return FALSE;
      }
      return TRUE;
    }
    else
  #endif
  /*==================== twostd  =================*/
  #ifdef HAVE_PLURAL
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
    else
  #endif
  /*==================== lie bracket =================*/
  #ifdef HAVE_PLURAL
    if (strcmp(sys_cmd, "bracket") == 0)
    {
      const short t[]={2,POLY_CMD,POLY_CMD};
      if (iiCheckTypes(h,t,1))
      {
        poly p=(poly)h->CopyD();
        h=h->next;
        poly q=(poly)h->Data();
        res->rtyp=POLY_CMD;
        if (rIsPluralRing(currRing))  res->data=nc_p_Bracket_qq(p,q, currRing);
        return FALSE;
      }
      return TRUE;
    }
    else
  #endif
  /*==================== env ==================================*/
  #ifdef HAVE_PLURAL
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
  #endif
/* ============ opp ======================== */
  #ifdef HAVE_PLURAL
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
  #endif
  /*==================== oppose ==================================*/
  #ifdef HAVE_PLURAL
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
  #endif
/*==================== sat =================*/
    if(strcmp(sys_cmd,"sat")==0)
    {
      ideal I= (ideal)h->Data();
      ideal J=(ideal)h->next->Data();
      int k;
      ideal S=idSaturate_intern(I,J,k,h->Typ()==IDEAL_CMD,hasFlag(h,FLAG_STD));
      res->rtyp=h->Typ();
      res->data=(void*)S;
      setFlag(res,FLAG_STD);
      return FALSE;
    }
    else
  /*==================== walk stuff =================*/
  /*==================== walkNextWeight =================*/
  #ifdef HAVE_WALK
  #ifdef OWNW
    if (strcmp(sys_cmd, "walkNextWeight") == 0)
    {
      const short t[]={3,INTVEC_CMD,INTVEC_CMD,IDEAL_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
    else
  #endif
  #endif
  /*==================== walkNextWeight =================*/
  #ifdef HAVE_WALK
  #ifdef OWNW
    if (strcmp(sys_cmd, "walkInitials") == 0)
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
  #endif
  /*==================== walkAddIntVec =================*/
  #ifdef HAVE_WALK
  #ifdef WAIV
    if (strcmp(sys_cmd, "walkAddIntVec") == 0)
    {
      const short t[]={2,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      intvec* arg1 = (intvec*) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      res->data = (intvec*) walkAddIntVec(arg1, arg2);
      res->rtyp = INTVEC_CMD;
      return FALSE;
    }
    else
  #endif
  #endif
  /*==================== MwalkNextWeight =================*/
  #ifdef HAVE_WALK
  #ifdef MwaklNextWeight
    if (strcmp(sys_cmd, "MwalkNextWeight") == 0)
    {
      const short t[]={3,INTVEC_CMD,INTVEC_CMD,IDEAL_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== Mivdp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "Mivdp") == 0)
    {
      if (h == NULL || h->Typ() != INT_CMD)
      {
        WerrorS("system(\"Mivdp\", int) expected");
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
    else
  #endif
  /*==================== Mivlp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "Mivlp") == 0)
    {
      if (h == NULL || h->Typ() != INT_CMD)
      {
        WerrorS("system(\"Mivlp\", int) expected");
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
  #endif
  /*==================== MpDiv =================*/
  #ifdef HAVE_WALK
  #ifdef MpDiv
    if(strcmp(sys_cmd, "MpDiv") == 0)
    {
      const short t[]={2,POLY_CMD,POLY_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      poly arg1 = (poly) h->Data();
      poly arg2 = (poly) h->next->Data();
      poly result = MpDiv(arg1, arg2);
      res->rtyp = POLY_CMD;
      res->data = result;
      return FALSE;
    }
    else
  #endif
  #endif
  /*==================== MpMult =================*/
  #ifdef HAVE_WALK
  #ifdef MpMult
    if(strcmp(sys_cmd, "MpMult") == 0)
    {
      const short t[]={2,POLY_CMD,POLY_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      poly arg1 = (poly) h->Data();
      poly arg2 = (poly) h->next->Data();
      poly result = MpMult(arg1, arg2);
      res->rtyp = POLY_CMD;
      res->data = result;
      return FALSE;
    }
    else
  #endif
  #endif
  /*==================== MivSame =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "MivSame") == 0)
    {
      const short t[]={2,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== M3ivSame =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "M3ivSame") == 0)
    {
      const short t[]={3,INTVEC_CMD,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== MwalkInitialForm =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MwalkInitialForm") == 0)
    {
      const short t[]={2,IDEAL_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== MivMatrixOrder =================*/
  #ifdef HAVE_WALK
    /************** Perturbation walk **********/
    if(strcmp(sys_cmd, "MivMatrixOrder") == 0)
    {
      if(h==NULL || h->Typ() != INTVEC_CMD)
      {
        WerrorS("system(\"MivMatrixOrder\",intvec) expected");
        return TRUE;
      }
      intvec* arg1 = (intvec*) h->Data();
      intvec* result = MivMatrixOrder(arg1);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MivMatrixOrderdp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MivMatrixOrderdp") == 0)
    {
      if(h==NULL || h->Typ() != INT_CMD)
      {
        WerrorS("system(\"MivMatrixOrderdp\",intvec) expected");
        return TRUE;
      }
      int arg1 = (int) ((long)(h->Data()));
      intvec* result = (intvec*) MivMatrixOrderdp(arg1);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MPertVectors =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MPertVectors") == 0)
    {
      const short t[]={3,IDEAL_CMD,INTVEC_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      int arg3 = (int) ((long)(h->next->next->Data()));
      intvec* result = (intvec*) MPertVectors(arg1, arg2, arg3);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MPertVectorslp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MPertVectorslp") == 0)
    {
      const short t[]={3,IDEAL_CMD,INTVEC_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      int arg3 = (int) ((long)(h->next->next->Data()));
      intvec* result = (intvec*) MPertVectorslp(arg1, arg2, arg3);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
    /************** fractal walk **********/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "Mfpertvector") == 0)
    {
      const short t[]={2,IDEAL_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* result = Mfpertvector(arg1, arg2);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MivUnit =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MivUnit") == 0)
    {
      const short t[]={1,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      int arg1 = (int) ((long)(h->Data()));
      intvec* result = (intvec*) MivUnit(arg1);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MivWeightOrderlp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MivWeightOrderlp") == 0)
    {
      const short t[]={1,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      intvec* arg1 = (intvec*) h->Data();
      intvec* result = MivWeightOrderlp(arg1);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MivWeightOrderdp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MivWeightOrderdp") == 0)
    {
      if(h==NULL || h->Typ() != INTVEC_CMD)
      {
        WerrorS("system(\"MivWeightOrderdp\",intvec) expected");
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
  #endif
  /*==================== MivMatrixOrderlp =================*/
  #ifdef HAVE_WALK
    if(strcmp(sys_cmd, "MivMatrixOrderlp") == 0)
    {
      if(h==NULL || h->Typ() != INT_CMD)
      {
        WerrorS("system(\"MivMatrixOrderlp\",int) expected");
        return TRUE;
      }
      int arg1 = (int) ((long)(h->Data()));
      intvec* result = (intvec*) MivMatrixOrderlp(arg1);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MkInterRedNextWeight =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "MkInterRedNextWeight") == 0)
    {
      const short t[]={3,INTVEC_CMD,INTVEC_CMD,IDEAL_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== MPertNextWeight =================*/
  #ifdef HAVE_WALK
  #ifdef MPertNextWeight
    if (strcmp(sys_cmd, "MPertNextWeight") == 0)
    {
      const short t[]={3,INTVEC_CMD,IDEAL_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== Mivperttarget =================*/
  #ifdef HAVE_WALK
  #ifdef Mivperttarget
    if (strcmp(sys_cmd, "Mivperttarget") == 0)
    {
      const short t[]={2,IDEAL_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      ideal arg1 = (ideal) h->Data();
      int arg2 = (int) h->next->Data();
      intvec* result = (intvec*) Mivperttarget(arg1, arg2);
      res->rtyp = INTVEC_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif //Mivperttarget
  #endif
  /*==================== Mwalk =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "Mwalk") == 0)
    {
      const short t[]={6,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,RING_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if (((intvec*) h->next->Data())->length() != currRing->N &&
        ((intvec*) h->next->next->Data())->length() != currRing->N )
      {
        Werror("system(\"Mwalk\" ...) intvecs not of length %d\n",
           currRing->N);
        return TRUE;
      }
      ideal arg1 = (ideal) h->CopyD();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3 = (intvec*) h->next->next->Data();
      ring arg4 = (ring) h->next->next->next->Data();
      int arg5 = (int) (long) h->next->next->next->next->Data();
      int arg6 = (int) (long) h->next->next->next->next->next->Data();
      ideal result = (ideal) Mwalk(arg1, arg2, arg3, arg4, arg5, arg6);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== Mpwalk =================*/
  #ifdef HAVE_WALK
  #ifdef MPWALK_ORIG
    if (strcmp(sys_cmd, "Mwalk") == 0)
    {
      const short t[]={4,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,RING_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if ((((intvec*) h->next->Data())->length() != currRing->N &&
          ((intvec*) h->next->next->Data())->length() != currRing->N ) &&
          (((intvec*) h->next->Data())->length() != (currRing->N)*(currRing->N) &&
          ((intvec*) h->next->next->Data())->length() != (currRing->N)*(currRing->N)))
      {
        Werror("system(\"Mwalk\" ...) intvecs not of length %d or %d\n",
               currRing->N,(currRing->N)*(currRing->N));
        return TRUE;
      }
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3   =  (intvec*) h->next->next->Data();
      ring arg4 = (ring) h->next->next->next->Data();
      ideal result = (ideal) Mwalk(arg1, arg2, arg3,arg4);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #else
    if (strcmp(sys_cmd, "Mpwalk") == 0)
    {
      const short t[]={8,IDEAL_CMD,INT_CMD,INT_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if(((intvec*) h->next->next->next->Data())->length() != currRing->N &&
         ((intvec*) h->next->next->next->next->Data())->length()!=currRing->N)
      {
        Werror("system(\"Mpwalk\" ...) intvecs not of length %d\n",currRing->N);
        return TRUE;
      }
      ideal arg1 = (ideal) h->Data();
      int arg2 = (int) (long) h->next->Data();
      int arg3 = (int) (long) h->next->next->Data();
      intvec* arg4 = (intvec*) h->next->next->next->Data();
      intvec* arg5 = (intvec*) h->next->next->next->next->Data();
      int arg6 = (int) (long) h->next->next->next->next->next->Data();
      int arg7 = (int) (long) h->next->next->next->next->next->next->Data();
      int arg8 = (int) (long) h->next->next->next->next->next->next->next->Data();
      ideal result = (ideal) Mpwalk(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
    #endif
  #endif
  /*==================== Mrwalk =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "Mrwalk") == 0)
    {
      const short t[]={7,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD,INT_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if(((intvec*) h->next->Data())->length() != currRing->N &&
         ((intvec*) h->next->Data())->length() != (currRing->N)*(currRing->N) &&
         ((intvec*) h->next->next->Data())->length() != currRing->N &&
         ((intvec*) h->next->next->Data())->length() != (currRing->N)*(currRing->N) )
      {
        Werror("system(\"Mrwalk\" ...) intvecs not of length %d or %d\n",
               currRing->N,(currRing->N)*(currRing->N));
        return TRUE;
      }
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3 =  (intvec*) h->next->next->Data();
      int arg4 = (int)(long) h->next->next->next->Data();
      int arg5 = (int)(long) h->next->next->next->next->Data();
      int arg6 = (int)(long) h->next->next->next->next->next->Data();
      int arg7 = (int)(long) h->next->next->next->next->next->next->Data();
      ideal result = (ideal) Mrwalk(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== MAltwalk1 =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "MAltwalk1") == 0)
    {
      const short t[]={5,IDEAL_CMD,INT_CMD,INT_CMD,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
    else
  #endif
  /*==================== MAltwalk1 =================*/
  #ifdef HAVE_WALK
  #ifdef MFWALK_ALT
    if (strcmp(sys_cmd, "Mfwalk_alt") == 0)
    {
      const short t[]={4,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
    else
  #endif
  #endif
  /*==================== Mfwalk =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "Mfwalk") == 0)
    {
      const short t[]={5,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if (((intvec*) h->next->Data())->length() != currRing->N &&
        ((intvec*) h->next->next->Data())->length() != currRing->N )
      {
        Werror("system(\"Mfwalk\" ...) intvecs not of length %d\n",
                 currRing->N);
        return TRUE;
      }
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3 = (intvec*) h->next->next->Data();
      int arg4 = (int)(long) h->next->next->next->Data();
      int arg5 = (int)(long) h->next->next->next->next->Data();
      ideal result = (ideal) Mfwalk(arg1, arg2, arg3, arg4, arg5);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== Mfrwalk =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "Mfrwalk") == 0)
    {
      const short t[]={6,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
/*
      if (((intvec*) h->next->Data())->length() != currRing->N &&
          ((intvec*) h->next->next->Data())->length() != currRing->N)
      {
        Werror("system(\"Mfrwalk\" ...) intvecs not of length %d\n",currRing->N);
        return TRUE;
      }
*/
      if((((intvec*) h->next->Data())->length() != currRing->N &&
         ((intvec*) h->next->next->Data())->length() != currRing->N ) &&
         (((intvec*) h->next->Data())->length() != (currRing->N)*(currRing->N) &&
         ((intvec*) h->next->next->Data())->length() != (currRing->N)*(currRing->N) ))
      {
        Werror("system(\"Mfrwalk\" ...) intvecs not of length %d or %d\n",
               currRing->N,(currRing->N)*(currRing->N));
        return TRUE;
      }

      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3 = (intvec*) h->next->next->Data();
      int arg4 = (int)(long) h->next->next->next->Data();
      int arg5 = (int)(long) h->next->next->next->next->Data();
      int arg6 = (int)(long) h->next->next->next->next->next->Data();
      ideal result = (ideal) Mfrwalk(arg1, arg2, arg3, arg4, arg5, arg6);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  /*==================== Mprwalk =================*/
    if (strcmp(sys_cmd, "Mprwalk") == 0)
    {
      const short t[]={9,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD,INT_CMD,INT_CMD,INT_CMD,INT_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
      if((((intvec*) h->next->Data())->length() != currRing->N &&
         ((intvec*) h->next->next->Data())->length() != currRing->N ) &&
         (((intvec*) h->next->Data())->length() != (currRing->N)*(currRing->N) &&
         ((intvec*) h->next->next->Data())->length() != (currRing->N)*(currRing->N) ))
      {
        Werror("system(\"Mrwalk\" ...) intvecs not of length %d or %d\n",
               currRing->N,(currRing->N)*(currRing->N));
        return TRUE;
      }
      ideal arg1 = (ideal) h->Data();
      intvec* arg2 = (intvec*) h->next->Data();
      intvec* arg3 =  (intvec*) h->next->next->Data();
      int arg4 = (int)(long) h->next->next->next->Data();
      int arg5 = (int)(long) h->next->next->next->next->Data();
      int arg6 = (int)(long) h->next->next->next->next->next->Data();
      int arg7 = (int)(long) h->next->next->next->next->next->next->Data();
      int arg8 = (int)(long) h->next->next->next->next->next->next->next->Data();
      int arg9 = (int)(long) h->next->next->next->next->next->next->next->next->Data();
      ideal result = (ideal) Mprwalk(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
      res->rtyp = IDEAL_CMD;
      res->data =  result;
      return FALSE;
    }
    else
  #endif
  /*==================== TranMImprovwalk =================*/
  #ifdef HAVE_WALK
  #ifdef TRAN_Orig
    if (strcmp(sys_cmd, "TranMImprovwalk") == 0)
    {
      const short t[]={3,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== MAltwalk2 =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "MAltwalk2") == 0)
    {
      const short t[]={3,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  #endif
  /*==================== MAltwalk2 =================*/
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "TranMImprovwalk") == 0)
    {
      const short t[]={4,IDEAL_CMD,INTVEC_CMD,INTVEC_CMD,INT_CMD};
      if (!iiCheckTypes(h,t,1)) return TRUE;
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
  /*==================== TranMrImprovwalk =================*/
  #if 0
  #ifdef HAVE_WALK
    if (strcmp(sys_cmd, "TranMrImprovwalk") == 0)
    {
      if (h == NULL || h->Typ() != IDEAL_CMD ||
        h->next == NULL || h->next->Typ() != INTVEC_CMD ||
        h->next->next == NULL || h->next->next->Typ() != INTVEC_CMD ||
        h->next->next->next == NULL || h->next->next->next->Typ() != INT_CMD ||
        h->next->next->next == NULL || h->next->next->next->next->Typ() != INT_CMD ||
        h->next->next->next == NULL || h->next->next->next->next->next->Typ() != INT_CMD)
      {
        WerrorS("system(\"TranMrImprovwalk\", ideal, intvec, intvec) expected");
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
  #endif
  /*================= Extended system call ========================*/
    {
       BOOLEAN handled;
       const BOOLEAN error =
         jjEXPERIMENTAL_SYSTEM(res, args, &handled);
       if (handled) return error;
       #ifndef MAKE_DISTRIBUTION
       return(jjEXTENDED_SYSTEM(res, args));
       #else
       Werror( "system(\"%s\",...) %s", sys_cmd, feNotImplemented );
       return TRUE;
       #endif
    }
  } /* typ==string */
  return TRUE;
}


#ifdef HAVE_EXTENDED_SYSTEM
  // You can put your own system calls here
#  include "kernel/fglm/fglm.h"
#  ifdef HAVE_NEWTON
#    include "hc_newton.h"
#  endif

static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h)
{
    if(h->Typ() == STRING_CMD)
    {
      char *sys_cmd=(char *)(h->Data());
      h=h->next;
  /*==================== locNF ======================================*/
      if(strcmp(sys_cmd,"locNF")==0)
      {
        const short t[]={4,VECTOR_CMD,MODUL_CMD,INT_CMD,INTVEC_CMD};
        if (iiCheckTypes(h,t,1))
        {
          poly f=(poly)h->Data();
          h=h->next;
          ideal m=(ideal)h->Data();
          assumeStdFlag(h);
          h=h->next;
          int n=(int)((long)h->Data());
          h=h->next;
          intvec *v=(intvec *)h->Data();

          /* == now the work starts == */

          int * iv=iv2array(v, currRing);
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
          return FALSE;
        }
        else
          return TRUE;
      }
      else
  /*==================== poly debug ==================================*/
        if(strcmp(sys_cmd,"p")==0)
        {
#  ifdef RDEBUG
          p_DebugPrint((poly)h->Data(), currRing);
#  else
          WarnS("Sorry: not available for release build!");
#  endif
          return FALSE;
        }
        else
  /* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
  /*==================== setsyzcomp ==================================*/
//      if(strcmp(sys_cmd,"setsyzcomp")==0)
//      {
//        if ((h!=NULL) && (h->Typ()==INT_CMD))
//        {
//          int k = (int)(long)h->Data();
//          if ( currRing->order[0] == ringorder_s )
//          {
//            rSetSyzComp(k, currRing);
//            return FALSE;
//          }
//        }
//      }
  /*==================== ring debug ==================================*/
        if(strcmp(sys_cmd,"r")==0)
        {
#  ifdef RDEBUG
          rDebugPrint((ring)h->Data());
#  else
          WarnS("Sorry: not available for release build!");
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
            snprintf(h,10, "x%d", i);
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
            char *fn=(char*) h->Data();
            fd = fopen(fn, "w");
            if (fd == NULL)
              Warn("Can not open %s for writing og mtrack. Using stdout",fn);
          }
          else if (h->Typ() == INT_CMD)
          {
            max = (int)(long)h->Data();
          }
          h = h->Next();
        }
        omPrintUsedTrackAddrs((fd == NULL ? stdout : fd), max);
        if (fd != NULL) fclose(fd);
        om_Opts.MarkAsStatic = 0;
        return FALSE;
  #else
        WerrorS("system(\"mtrack\",..) is not implemented in this version");
        return TRUE;
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
              WarnS("Wrong types for poly= comb(ideal,poly)");
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
                WarnS("Wrong types for poly= comb(ideal,poly)");
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
          intvec* w = new intvec( r.speicher );  // necessary memory
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
            WerrorS("expected exactly one argument: "
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
        if (FE_OPT_NO_SHELL_FLAG)
        {
          WerrorS("shell execution is disallowed in restricted mode");
          return TRUE;
        }
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
        else { WerrorS("wrong typ"); return TRUE;}
      }
      else
  #endif
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
  /*==================== Testing groebner basis =================*/
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
    /*==================== sca:AltVar ==================================*/
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
          //        PrintS("poly is done\n");
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==IDEAL_CMD))
        {
          I=(ideal)h->CopyD();
          q = I->m;
          h=h->next;
          //        PrintS("ideal is done\n");
        }
        else return TRUE;
        if ((h!=NULL) && (h->Typ()==INT_CMD))
        {
          is=(int)((long)(h->Data()));
          //        res->rtyp=INT_CMD;
          //        PrintS("int is done\n");
          //        res->rtyp=IDEAL_CMD;
          if (rIsPluralRing(currRing))
          {
            id = IDELEMS(I);
                   int *pl=(int*)omAlloc0(IDELEMS(I)*sizeof(int));
            for(k=0; k < id; k++)
            {
              pl[k] = pLength(I->m[k]);
            }
            PrintS("starting redRat\n");
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
  /* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
//      if (strcmp(sys_cmd, "mults")==0)
//      {
//        res->rtyp=INT_CMD ;
//        res->data=(void*)(long) Mults();
//        return(FALSE);
//      }
//      else
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
  /* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
  /*==================== gcd-varianten =================*/
//      if (strcmp(sys_cmd, "gcd") == 0)
//      {
//        if (h==NULL)
//        {
//        #if 0
//          Print("FLINT_P:%d (use Flints gcd for polynomials in char p)\n",isOn(SW_USE_FL_GCD_P));
//          Print("FLINT_0:%d (use Flints gcd for polynomials in char 0)\n",isOn(SW_USE_FL_GCD_0));
//        #endif
//          Print("EZGCD:%d (use EZGCD for gcd of polynomials in char 0)\n",isOn(SW_USE_EZGCD));
//          Print("EZGCD_P:%d (use EZGCD_P for gcd of polynomials in char p)\n",isOn(SW_USE_EZGCD_P));
//          Print("CRGCD:%d (use chinese Remainder for gcd of polynomials in char 0)\n",isOn(SW_USE_CHINREM_GCD));
//          #ifndef __CYGWIN__
//          Print("homog:%d (use homog. test for factorization of polynomials)\n",singular_homog_flag);
//          #endif
//          return FALSE;
//        }
//        else
//        if ((h!=NULL) && (h->Typ()==STRING_CMD)
//        && (h->next!=NULL) && (h->next->Typ()==INT_CMD))
//        {
//          int d=(int)(long)h->next->Data();
//          char *s=(char *)h->Data();
//        #if 0
//          if (strcmp(s,"FLINT_P")==0) { if (d) On(SW_USE_FL_GCD_P); else Off(SW_USE_FL_GCD_P); } else
//          if (strcmp(s,"FLINT_0")==0) { if (d) On(SW_USE_FL_GCD_0); else Off(SW_USE_FL_GCD_0); } else
//        #endif
//          if (strcmp(s,"EZGCD")==0) { if (d) On(SW_USE_EZGCD); else Off(SW_USE_EZGCD); } else
//          if (strcmp(s,"EZGCD_P")==0) { if (d) On(SW_USE_EZGCD_P); else Off(SW_USE_EZGCD_P); } else
//          if (strcmp(s,"CRGCD")==0) { if (d) On(SW_USE_CHINREM_GCD); else Off(SW_USE_CHINREM_GCD); } else
//          #ifndef __CYGWIN__
//          if (strcmp(s,"homog")==0) { if (d) singular_homog_flag=1; else singular_homog_flag=0; } else
//          #endif
//          return TRUE;
//          return FALSE;
//        }
//        else return TRUE;
//      }
//      else
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
            res->data=(char *)singntl_HNF((intvec*)h->Data());
            return FALSE;
          }
          else if (h->Typ()==INTMAT_CMD)
          {
            res->data=(char *)singntl_HNF((intvec*)h->Data());
            return FALSE;
          }
          else
          {
            WerrorS("expected `system(\"HNF\",<matrix|intmat|bigintmat>)`");
            return TRUE;
          }
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
  /*==================== mpz_t loader ======================*/
    if(strcmp(sys_cmd, "GNUmpLoad")==0)
    {
      if ((h != NULL) && (h->Typ() == STRING_CMD))
      {
        char* filename = (char*)h->Data();
        FILE* f = fopen(filename, "r");
        if (f == NULL)
        {
          WerrorS( "invalid file name (in paths use '/')");
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
        WerrorS( "expected valid file name as a string");
        return TRUE;
      }
    }
    else
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
        WerrorS("expected two non-empty intvecs as arguments");
        return TRUE;
      }
    }
    else
  /* ================== intvecOverlap ======================= */
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
        WerrorS("expected two non-empty intvecs as arguments");
        return TRUE;
      }
    }
    else
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
        WerrorS( "expected argument list (int, int, poly, poly, poly, int)");
        return TRUE;
      }
    }
    else
  /*==================== Approx_Step  =================*/
  #ifdef HAVE_PLURAL
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
    else
  #endif
  /*==================== PrintMat  =================*/
  #ifdef HAVE_PLURAL
    if (strcmp(sys_cmd, "PrintMat") == 0)
    {
      int a=0;
      int b=0;
      ring r=NULL;
      int metric=0;
      if (h!=NULL)
      {
        if (h->Typ()==INT_CMD)
        {
          a=(int)((long)(h->Data()));
          h=h->next;
        }
        else if (h->Typ()==INT_CMD)
        {
          b=(int)((long)(h->Data()));
          h=h->next;
        }
        else if (h->Typ()==RING_CMD)
        {
          r=(ring)h->Data();
          h=h->next;
        }
        else
          return TRUE;
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
    else
  #endif
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
/* ============ NCUseExtensions ======================== */
//  #ifdef HAVE_PLURAL
//    if(strcmp(sys_cmd,"NCUseExtensions")==0)
//    {
//      if ((h!=NULL) && (h->Typ()==INT_CMD))
//        res->data=(void *)(long)setNCExtensions( (int)((long)(h->Data())) );
//      else
//        res->data=(void *)(long)getNCExtensions();
//      res->rtyp=INT_CMD;
//      return FALSE;
//    }
//    else
//  #endif
/* ============ NCGetType ======================== */
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
//  #ifdef HAVE_PLURAL
//    if(strcmp(sys_cmd,"NCGetType")==0)
//    {
//      res->rtyp=INT_CMD;
//      if( rIsPluralRing(currRing) )
//        res->data=(void *)(long)ncRingType(currRing);
//      else
//        res->data=(void *)(-1L);
//      return FALSE;
//    }
//    else
//  #endif
/* ============ ForceSCA ======================== */
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
//  #ifdef HAVE_PLURAL
//    if(strcmp(sys_cmd,"ForceSCA")==0)
//    {
//      if( !rIsPluralRing(currRing) )
//        return TRUE;
//      int b, e;
//      if ((h!=NULL) && (h->Typ()==INT_CMD))
//      {
//        b = (int)((long)(h->Data()));
//        h=h->next;
//      }
//      else return TRUE;
//      if ((h!=NULL) && (h->Typ()==INT_CMD))
//      {
//        e = (int)((long)(h->Data()));
//      }
//      else return TRUE;
//      if( !sca_Force(currRing, b, e) )
//        return TRUE;
//      return FALSE;
//    }
//    else
//  #endif
/* ============ ForceNewNCMultiplication ======================== */
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
//  #ifdef HAVE_PLURAL
//    if(strcmp(sys_cmd,"ForceNewNCMultiplication")==0)
//    {
//      if( !rIsPluralRing(currRing) )
//        return TRUE;
//      if( ncInitSpecialPairMultiplication(currRing) ) // No Plural!
//        return TRUE;
//      return FALSE;
//    }
//    else
//  #endif
/* ============ ForceNewOldNCMultiplication ======================== */
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
//  #ifdef HAVE_PLURAL
//    if(strcmp(sys_cmd,"ForceNewOldNCMultiplication")==0)
//    {
//      if( !rIsPluralRing(currRing) )
//        return TRUE;
//      if( !ncInitSpecialPowersMultiplication(currRing) ) // Enable Formula for Plural (depends on swiches)!
//        return TRUE;
//      return FALSE;
//    }
//    else
//  #endif
/*==================== test64 =================*/
  #if 0
    if(strcmp(sys_cmd,"test64")==0)
    {
      long l=8;int i;
      for(i=1;i<62;i++)
      {
        l=l<<1;
        number n=n_Init(l,coeffs_BIGINT);
        Print("%ld= ",l);n_Print(n,coeffs_BIGINT);
        CanonicalForm nn=n_convSingNFactoryN(n,TRUE,coeffs_BIGINT);
        n_Delete(&n,coeffs_BIGINT);
        n=n_convFactoryNSingN(nn,coeffs_BIGINT);
        PrintS(" F:");
        n_Print(n,coeffs_BIGINT);
        PrintLn();
        n_Delete(&n,coeffs_BIGINT);
      }
      Print("SIZEOF_LONG=%d\n",SIZEOF_LONG);
      return FALSE;
    }
    else
   #endif
/* Handled by jjEXPERIMENTAL_SYSTEM in all builds. */
/*==================== n_SwitchChinRem =================*/
//    if(strcmp(sys_cmd,"cache_chinrem")==0)
//    {
//      EXTERN_VAR int n_SwitchChinRem;
//      Print("caching inverse in chines remainder:%d\n",n_SwitchChinRem);
//      if ((h!=NULL)&&(h->Typ()==INT_CMD))
//        n_SwitchChinRem=(int)(long)h->Data();
//      return FALSE;
//    }
//    else
/*==================== LU for bigintmat =================*/
#ifdef SINGULAR_4_2
    if(strcmp(sys_cmd,"LU")==0)
    {
      if ((h!=NULL) && (h->Typ()==CMATRIX_CMD))
      {
        // get the argument:
        bigintmat *b=(bigintmat *)h->Data();
        // just for tests: simply transpose
        bigintmat *bb=b->transpose();
        // return the result:
        res->rtyp=CMATRIX_CMD;
        res->data=(char*)bb;
        return FALSE;
      }
      else
      {
        WerrorS("system(\"LU\",<cmatrix>) expected");
        return TRUE;
      }
    }
    else
#endif
/*==================== sort =================*/
    if(strcmp(sys_cmd,"sort")==0)
    {
      extern BOOLEAN jjSORTLIST(leftv,leftv);
      if (h->Typ()==LIST_CMD)
        return jjSORTLIST(res,h);
      else
        return TRUE;
    }
    else
/*==================== uniq =================*/
    if(strcmp(sys_cmd,"uniq")==0)
    {
      extern BOOLEAN jjUNIQLIST(leftv, leftv);
      if (h->Typ()==LIST_CMD)
        return jjUNIQLIST(res,h);
      else
        return TRUE;
    }
    else
/*==================== GF(p,n) ==================================*/
    if(strcmp(sys_cmd,"GF")==0)
    {
      const short t[]={3,INT_CMD,INT_CMD,STRING_CMD};
      if (iiCheckTypes(h,t,1))
      {
        int p=(int)(long)h->Data();
        int n=(int)(long)h->next->Data();
        char *v=(char*)h->next->next->CopyD();
        GFInfo param;
        param.GFChar = p;
        param.GFDegree = n;
        param.GFPar_name = v;
        coeffs cf= nInitChar(n_GF, &param);
        res->rtyp=CRING_CMD;
        res->data=cf;
        return FALSE;
      }
      else
        return TRUE;
    }
    else
/*==================== power* ==================================*/
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
/* ccluster --------------------------------------------------------------*/
#if defined(HAVE_CCLUSTER) && defined(HAVE_FLINT)
    if(strcmp(sys_cmd,"ccluster")==0)
    {
      if ((currRing!=NULL)
      && (rField_is_Q(currRing) || rField_is_R(currRing) || rField_is_long_R(currRing)))
      {
        const short t[]={5,POLY_CMD,NUMBER_CMD,NUMBER_CMD,NUMBER_CMD,NUMBER_CMD};
        const short t2[]={6,POLY_CMD,POLY_CMD,NUMBER_CMD,NUMBER_CMD,NUMBER_CMD,NUMBER_CMD};

//         printf("test t : %d\n", h->Typ()==POLY_CMD);
//         printf("test t : %d\n", h->next->Typ()==POLY_CMD);
        int pol_with_complex_coeffs=0;
        if (h->next->Typ()==POLY_CMD)
            pol_with_complex_coeffs=1;

        if ( (pol_with_complex_coeffs==0 && iiCheckTypes(h,t,1))
       ||(pol_with_complex_coeffs==1 && iiCheckTypes(h,t2,1)) )
        {
          // convert first arg. to fmpq_poly_t
          fmpq_poly_t fre, fim;
          convSingPFlintP(fre,(poly)h->Data(),currRing); h=h->next;
          if (pol_with_complex_coeffs==1)
          { // convert second arg. to fmpq_poly_t
            convSingPFlintP(fim,(poly)h->Data(),currRing); h=h->next;
          }
          // convert box-center(re,im), box-size, epsilon
          fmpq_t center_re,center_im,boxsize,eps;
          convSingNFlintN(center_re,(number)h->Data(),currRing->cf); h=h->next;
          convSingNFlintN(center_im,(number)h->Data(),currRing->cf); h=h->next;
          convSingNFlintN(boxsize,(number)h->Data(),currRing->cf); h=h->next;
          convSingNFlintN(eps,(number)h->Data(),currRing->cf); h=h->next;
          // alloc arrays
          int n=fmpq_poly_length(fre);
          fmpq_t* re_part=(fmpq_t*)omAlloc(n*sizeof(fmpq_t));
          fmpq_t* im_part=(fmpq_t*)omAlloc(n*sizeof(fmpq_t));
          int *mult      =(int*)   omAlloc(n*sizeof(int));
          for(int i=0; i<n;i++)
          { fmpq_init(re_part[i]); fmpq_init(im_part[i]); }
          // call cccluster, adjust n
          int verbosity =0; //nothing is printed
          int strategy = 23; //default strategy
          int nn=0;
          long nb_threads = (long) feOptValue(FE_OPT_CPUS);
          strategy = strategy+(nb_threads<<6);
//       printf("nb threads: %ld\n", nb_threads);
//       printf("strategy: %ld\n", strategy);
          if (pol_with_complex_coeffs==0)
            nn=ccluster_interface_poly_real(re_part,im_part,mult,fre,center_re,center_im,boxsize,eps,strategy,verbosity);
          else
            nn=ccluster_interface_poly_real_imag(re_part,im_part,mult,fre,fim,center_re,center_im,boxsize,eps,strategy,verbosity);
          // convert to list
          lists l=(lists)omAlloc0Bin(slists_bin);
          l->Init(nn);
          for(int i=0; i<nn;i++)
          {
            lists ll=(lists)omAlloc0Bin(slists_bin);
            l->m[i].rtyp=LIST_CMD;
            l->m[i].data=ll;
            ll->Init(3);
            ll->m[0].rtyp=NUMBER_CMD;
            ll->m[1].rtyp=NUMBER_CMD;
            ll->m[2].rtyp=INT_CMD;
            ll->m[0].data=convFlintNSingN(re_part[i],currRing->cf);
            ll->m[1].data=convFlintNSingN(im_part[i],currRing->cf);
            ll->m[2].data=(void *)(long)mult[i];
          }
          //clear re, im, mults, fre, fim
          for(int i=n-1;i>=0;i--) { fmpq_clear(re_part[i]); fmpq_clear(im_part[i]); }
          omFree(re_part);
          omFree(im_part);
          omFree(mult);
          fmpq_clear(center_re); fmpq_clear(center_im); fmpq_clear(boxsize); fmpq_clear(eps);
          fmpq_poly_clear(fre);
          if (pol_with_complex_coeffs==1) fmpq_poly_clear(fim);
          // result
          res->rtyp=LIST_CMD;
          res->data=l;
          return FALSE;
        }
      }
      return TRUE;
    }
    else
#endif
/* ====== maEvalAt ============================*/
    if(strcmp(sys_cmd,"evaluate")==0)
    {
      extern number maEvalAt(const poly p,const number* pt, const ring r);
      if (h->Typ()!=POLY_CMD)
      {
        WerrorS("expected system(\"evaluate\",<poly>,..)");
        return TRUE;
      }
      poly p=(poly)h->Data();
      number *pt=(number*)omAlloc(sizeof(number)*currRing->N);
      for(int i=0;i<currRing->N;i++)
      {
        h=h->next;
        if ((h==NULL)||(h->Typ()!=NUMBER_CMD))
        {
          WerrorS("system(\"evaluate\",<poly>,<number>..) - expect number");
          return TRUE;
        }
        pt[i]=(number)h->Data();
      }
      res->data=maEvalAt(p,pt,currRing);
      res->rtyp=NUMBER_CMD;
      return FALSE;
    }
    else
/* ====== DivRem ============================*/
    if(strcmp(sys_cmd,"DivRem")==0)
    {
      const short t1[]={2,POLY_CMD,POLY_CMD};
      if (iiCheckTypes(h,t1,1))
      {
        poly p=(poly)h->CopyD();
        poly q=(poly)h->next->CopyD();
        poly rest;
        res->data=p_DivRem(p,q,rest,currRing);
        res->rtyp=POLY_CMD;
        PrintS("rest:");pWrite(rest);
        return FALSE;
      }
      else
      {
        WerrorS("expected system(\"DivRem\",<poly>,<poly>)");
        return TRUE;
      }
    }
    else
/* ====== DivRemId ============================*/
    if(strcmp(sys_cmd,"DivRemIdU")==0)
    {
      const short t1[]={2,IDEAL_CMD,IDEAL_CMD};
      const short t2[]={2,MODUL_CMD,MODUL_CMD};
      if (iiCheckTypes(h,t1,0)
      || iiCheckTypes(h,t2,0))
      {
        ideal p=(ideal)h->CopyD();
        ideal q=(ideal)h->next->CopyD();
        ideal factors;
        ideal unit;
        ideal rest=idDivRem(p,q,factors,&unit,0);
        //matrix T = id_Module2Matrix(factors,currRing);
        //matrix U = id_Module2Matrix(unit,currRing);
        lists L=(lists)omAllocBin(slists_bin);
        L->Init(3);
        //L->m[0].rtyp=h->Typ();   L->m[0].data=(void *)rest;
        L->m[0].rtyp=MODUL_CMD;   L->m[0].data=(void *)rest;
        L->m[1].rtyp=MODUL_CMD;  L->m[1].data=(void *)factors;
        L->m[2].rtyp=MODUL_CMD;  L->m[2].data=(void *)unit;
        res->rtyp=LIST_CMD;
        res->data=L;
        return FALSE;
      }
      else
      {
        WerrorS("expected system(\"DivRemId\",<ideal>,<ideal>)");
        return TRUE;
      }
    }
    else
    if(strcmp(sys_cmd,"DivRemId")==0)
    {
      const short t1[]={2,IDEAL_CMD,IDEAL_CMD};
      const short t2[]={2,MODUL_CMD,MODUL_CMD};
      if (iiCheckTypes(h,t1,0)
      || iiCheckTypes(h,t2,0))
      {
        ideal p=(ideal)h->CopyD();
        ideal q=(ideal)h->next->CopyD();
        ideal rest;
        ideal quot=idDivRem(p,q,rest,NULL,0);
        matrix T = id_Module2Matrix(rest,currRing);
        lists L=(lists)omAllocBin(slists_bin);
        L->Init(2);
        L->m[0].rtyp=IDEAL_CMD;   L->m[0].data=(void *)quot;
        L->m[1].rtyp=MATRIX_CMD;  L->m[1].data=(void *)T;
        res->rtyp=LIST_CMD;
        res->data=L;
        return FALSE;
      }
      else
      {
        WerrorS("expected system(\"DivRemId\",<ideal>,<ideal>)");
        return TRUE;
      }
    }
    else
/* ====== CoeffTerm ============================*/
    if(strcmp(sys_cmd,"CoeffTerm")==0)
    {
      const short t1[]={2,POLY_CMD,POLY_CMD};
      const short t2[]={2,VECTOR_CMD,VECTOR_CMD};
      const short t3[]={2,IDEAL_CMD,POLY_CMD};
      const short t4[]={2,MODUL_CMD,VECTOR_CMD};
      const short t5[]={2,VECTOR_CMD,POLY_CMD};
      const short t6[]={2,MODUL_CMD,POLY_CMD};
      const short t7[]={2,VECTOR_CMD,IDEAL_CMD};
      const short t8[]={2,VECTOR_CMD,MODUL_CMD};
      if (iiCheckTypes(h,t1,0)
      || iiCheckTypes(h,t2,0))
      {
        poly p=(poly)h->Data();
        poly q=(poly)h->next->Data();
        res->data=p_CoeffTerm(p,q,currRing);
        res->rtyp=NUMBER_CMD;
        return FALSE;
      }
      else if (iiCheckTypes(h,t3,0)
      || iiCheckTypes(h,t4,0))
      {
        ideal p=(ideal)h->Data();
        poly q=(poly)h->next->Data();
        res->data=id_CoeffTerm(p,q,currRing);
        res->rtyp=h->Typ();
        return FALSE;
      }
      else if (iiCheckTypes(h,t5,0))
      {
        poly p=(poly)h->Data();
        poly q=(poly)h->next->Data();
        res->data=p_CoeffTermV(p,q,currRing);
        res->rtyp=VECTOR_CMD;
        return FALSE;
      }
      else if (iiCheckTypes(h,t6,0))
      {
        ideal p=(ideal)h->Data();
        poly q=(poly)h->next->Data();
        res->data=id_CoeffTermV(p,q,currRing);
        res->rtyp=MODUL_CMD;
        return FALSE;
      }
      else if (iiCheckTypes(h,t7,0)) /* vector,ideal*/
      {
        poly p=(poly)h->Data();
        ideal q=(ideal)h->next->Data();
        res->data=p_CoeffTermId(p,q,p_MaxComp(p,currRing),currRing);
        res->rtyp=VECTOR_CMD;
        return FALSE;
      }
      else if (iiCheckTypes(h,t8,0)) /* vector,module*/
      {
        poly p=(poly)h->Data();
        ideal q=(ideal)h->next->Data();
        res->data=p_CoeffTermMo(p,q,currRing);
        res->rtyp=VECTOR_CMD;
        return FALSE;
      }
      else
      {
        WerrorS("expected system(\"CoeffTerm\",<poly>/<vector>,<poly>/<vector>)"        "\n or                          <ideal>/<module>,<poly>/<vector>");
        return TRUE;
      }
    }
    else
/*==================== sat1 =================*/
    if(strcmp(sys_cmd,"sat1")==0)
    {
      ideal I= (ideal)h->Data();
      ideal J=(ideal)h->next->Data();
      res->rtyp=IDEAL_CMD;
      res->data=(void*)id_Sat_principal(I,J,currRing);
      return FALSE;
    }
    else
/*==================== minres_with_map =================*/
    if(strcmp(sys_cmd,"minres_with_map")==0)
    {
      syStrategy r= syCopy((syStrategy)h->Data());
      ideal trans;
      res->rtyp=RESOLUTION_CMD;
      syMinimize_with_map(r,trans);
      res->data=(void*)r;
      res->next=(leftv)omAlloc0Bin(sleftv_bin);
      res->next->data=(void*)trans;
      res->next->rtyp=MODUL_CMD;
      return FALSE;
    }
    else
/*==================== sat =================*/
#if 0
    if(strcmp(sys_cmd,"sat_with_exp")==0)
    {
      ideal I= (ideal)h->Data();
      ideal J=(ideal)h->next->Data();
      int k;
      ideal S=idSaturate_intern(I,J,k,h->Typ()==IDEAL_CMD,hasFlag(h,FLAG_STD));
      lists L = (lists)omAllocBin(slists_bin);
      L->Init(2);
      L->m[0].rtyp = h->Typ(); L->m[0].data=(void*)S; // ideal or module
      setFlag(&(L->m[0]),FLAG_STD);
      L->m[1].rtyp = INT_CMD; L->m[1].data=(void*)(long)k;
      res->rtyp=LIST_CMD;
      res->data=(void*)L;
      return FALSE;
    }
    else
#endif
/*==================== ssi =================*/
    if(strcmp(sys_cmd,"ssi")==0)
    {
      res->rtyp=STRING_CMD;
      StringSetS("");
      ssiWrite_S(h, currRing);
      res->data=(void*)StringEndS();
      return res->data==NULL;
    }
    else
    if(strcmp(sys_cmd,"ssi-r")==0)
    {
      if (h->Typ()==STRING_CMD)
      {
        char *s=(char*)h->Data();
        leftv rr=ssiRead1_S(&s,currRing);
        memcpy(res,rr,sizeof(*rr));
        omFreeBin(rr,sleftv_bin);
        return FALSE;
      }
    }
    else
/*==================== += =================*/
    if(strcmp(sys_cmd,"+=")==0)
    {
      const short t1[]={2,BUCKET_CMD,POLY_CMD};
      if (iiCheckTypes(h,t1,1))
      {
        if (h->rtyp==IDHDL)
	{
	  sBucket_pt b=(sBucket_pt)h->Data();
	  poly p=(poly)h->next->CopyD();
	  sBucket_Add_p(b,p,pLength(p));
	}
      }
      res->rtyp=NONE;
      return FALSE;
    }
    else
/*==================== Error =================*/
      Werror( "(extended) system(\"%s\",...) %s", sys_cmd, feNotImplemented );
  }
  return TRUE;
}

#endif // HAVE_EXTENDED_SYSTEM

/* Release-visible interfaces for selected algorithms which used to be
 * reachable only through jjEXTENDED_SYSTEM.  Keep this section independent
 * of that dispatcher so distribution and development builds use the same
 * public implementation. */
#include "kernel/fglm/fglm.h"
#include "kernel/maps/gen_maps.h"

static BOOLEAN jjExtraRequireRing(const char *name)
{
  if (currRing != NULL) return FALSE;
  Werror("%s requires a basering", name);
  return TRUE;
}

static BOOLEAN jjExtraIsPoly(leftv value)
{
  if (value == NULL) return FALSE;
  if (value->Typ() == POLY_CMD) return TRUE;
  if (value->Typ() != BUCKET_CMD) return FALSE;
  return p_MaxComp(sBucketPeek((sBucket_pt)value->Data()), currRing) == 0;
}

static poly jjExtraPoly(leftv value)
{
  if (value->Typ() == BUCKET_CMD)
    return sBucketPeek((sBucket_pt)value->Data());
  return (poly)value->Data();
}

static BOOLEAN jjExtraIsVector(leftv value)
{
  if (value == NULL) return FALSE;
  if (value->Typ() == VECTOR_CMD) return TRUE;
  if (value->Typ() != BUCKET_CMD) return FALSE;
  return p_MaxComp(sBucketPeek((sBucket_pt)value->Data()), currRing) > 0;
}

static BOOLEAN jjExtraIsUnivariateInFirst(poly p)
{
  while (p != NULL)
  {
    if (p_GetComp(p, currRing) != 0) return FALSE;
    for (int i = 2; i <= currRing->N; i++)
      if (p_GetExp(p, i, currRing) != 0) return FALSE;
    pIter(p);
  }
  return TRUE;
}

static BOOLEAN jjExtraLocNF(leftv res, leftv h)
{
  const short t[] = {4, VECTOR_CMD, MODUL_CMD, INT_CMD, INTVEC_CMD};
  if (jjExtraRequireRing("locNF") || !iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `locNF(vector,module,int,intvec)`");
    return TRUE;
  }

  poly f = (poly)h->Data();
  ideal m = (ideal)h->next->Data();
  const int degree = (int)(long)h->next->next->Data();
  intvec *weights = (intvec*)h->next->next->next->Data();
  const int generators = IDELEMS(m);
  if ((degree < 0) || (weights->length() != currRing->N))
  {
    Werror("locNF requires a non-negative degree and %d weights",
           currRing->N);
    return TRUE;
  }
  if (generators == 0)
  {
    WerrorS("locNF requires a non-empty module");
    return TRUE;
  }
  assumeStdFlag(h->next);

  int *weightArray = iv2array(weights, currRing);
  poly remainder = NULL;
  poly work = ppJetW(f, degree, weightArray);
  int j = 0;
  matrix transformation = mp_InitI(generators, 1, 0, currRing);

  while (work != NULL)
  {
    if ((m->m[j] != NULL) && pDivisibleBy(m->m[j], work))
    {
      poly factor = pDivideM(pHead(work), pHead(m->m[j]));
      MATELEM(transformation, j + 1, 1) =
        pAdd(MATELEM(transformation, j + 1, 1), factor);
      work = ppJetW(ksOldSpolyRed(m->m[j], work, 0), degree, weightArray);
      j = 0;
    }
    else if (j == generators - 1)
    {
      remainder = pAdd(remainder, pHead(work));
      work = pLmDeleteAndNext(work);
      j = 0;
    }
    else
      j++;
  }

  ideal remainderIdeal = id_Vec2Ideal(remainder, currRing);
  matrix temp = mp_Transp((matrix)remainderIdeal, currRing);
  id_Delete(&remainderIdeal, currRing);
  ideal inputIdeal = id_Vec2Ideal(f, currRing);
  matrix normalForm = mpNew(IDELEMS(inputIdeal), 1);
  id_Delete(&inputIdeal, currRing);
  for (int k = 1; k <= MATROWS(temp); k++)
  {
    MATELEM(normalForm, k, 1) = MATELEM(temp, k, 1);
    MATELEM(temp, k, 1) = NULL;
  }
  id_Delete((ideal*)&temp, currRing);
  p_Delete(&remainder, currRing);
  omFree(weightArray);

  lists result = (lists)omAllocBin(slists_bin);
  result->Init(2);
  result->m[0].rtyp = MATRIX_CMD;
  result->m[0].data = (void*)normalForm;
  result->m[1].rtyp = MATRIX_CMD;
  result->m[1].data = (void*)transformation;
  res->rtyp = LIST_CMD;
  res->data = result;
  return FALSE;
}

static BOOLEAN jjExtraFglmCombination(leftv res, leftv h)
{
  if (jjExtraRequireRing("fglmCombination") || (h == NULL) ||
      (h->Typ() != IDEAL_CMD) || !jjExtraIsPoly(h->next) ||
      (h->next->next != NULL))
  {
    WerrorS("expected `fglmCombination(ideal,poly)`");
    return TRUE;
  }
  if (jjExtraPoly(h->next) == NULL)
  {
    WerrorS("fglmCombination requires a non-zero polynomial");
    return TRUE;
  }

  ideal source = (ideal)h->Data();
  poly monomials = jjExtraPoly(h->next);
  assumeStdFlag(h);
  res->rtyp = POLY_CMD;
  res->data = (void*)fglmLinearCombination(source, monomials);
  return FALSE;
}

static BOOLEAN jjExtraMatrixMinpoly(leftv res, leftv h)
{
  const short t[] = {1, MATRIX_CMD};
  if (jjExtraRequireRing("matrixMinpoly") || !iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `matrixMinpoly(matrix)`");
    return TRUE;
  }
  if (!rField_is_Zp(currRing))
  {
    WerrorS("matrixMinpoly requires a prime field as coefficient field");
    return TRUE;
  }

  matrix m = (matrix)h->Data();
  const int n = MATROWS(m);
  if ((n == 0) || (n != MATCOLS(m)))
  {
    WerrorS("matrixMinpoly requires a non-empty square matrix");
    return TRUE;
  }
  for (int i = n * n - 1; i >= 0; i--)
    if (!p_IsConstant(m->m[i], currRing))
    {
      WerrorS("matrixMinpoly requires a matrix with constant entries");
      return TRUE;
    }

  const unsigned long p = (unsigned long)n_GetChar(currRing->cf);
  unsigned long **longMatrix = singularMatrixToLongMatrix(m);
  unsigned long *coefficients = computeMinimalPolynomial(longMatrix, n, p);
  res->rtyp = POLY_CMD;
  res->data = (void*)longCoeffsToSingularPoly(coefficients, n);
  for (int i = 0; i < n; i++) delete[] longMatrix[i];
  delete[] longMatrix;
  delete[] coefficients;
  return FALSE;
}

static BOOLEAN jjExtraFastMult(leftv res, leftv h)
{
  if (jjExtraRequireRing("fastMult") || !jjExtraIsPoly(h) ||
      !jjExtraIsPoly(h->next) || (h->next->next == NULL) ||
      (h->next->next->Typ() != STRING_CMD) ||
      (h->next->next->next != NULL))
  {
    WerrorS("expected `fastMult(poly,poly,string)`");
    return TRUE;
  }

  poly f = jjExtraPoly(h);
  poly g = jjExtraPoly(h->next);
  const char *algorithm = (const char*)h->next->next->Data();
  res->rtyp = POLY_CMD;
  if (strcmp(algorithm, "univariate") == 0)
  {
    if (!jjExtraIsUnivariateInFirst(f) || !jjExtraIsUnivariateInFirst(g))
    {
      WerrorS("fastMult with `univariate` requires polynomials in the first variable");
      return TRUE;
    }
    res->data = (void*)unifastmult(f, g, currRing);
  }
  else
  {
    WerrorS("fastMult: algorithm must be `univariate`");
    return TRUE;
  }
  return FALSE;
}

static BOOLEAN jjExtraPower(leftv res, leftv h)
{
  if (jjExtraRequireRing("powerWithAlgorithm") || !jjExtraIsPoly(h) ||
      (h->next == NULL) || (h->next->Typ() != INT_CMD) ||
      (h->next->next == NULL) ||
      (h->next->next->Typ() != STRING_CMD) ||
      (h->next->next->next != NULL))
  {
    WerrorS("expected `powerWithAlgorithm(poly,int,string)`");
    return TRUE;
  }

  poly f = jjExtraPoly(h);
  const int exponent = (int)(long)h->next->Data();
  const char *algorithm = (const char*)h->next->next->Data();
  if (exponent < 0)
  {
    WerrorS("powerWithAlgorithm requires a non-negative exponent");
    return TRUE;
  }
  if ((f != NULL) && (exponent != 0) &&
      ((long)pTotaldegree(f) > (signed long)currRing->bitmask /
       (signed long)exponent / 2))
  {
    Werror("OVERFLOW in power(d=%ld, e=%d, max=%ld)",
           pTotaldegree(f), exponent, currRing->bitmask / 2);
    return TRUE;
  }

  res->rtyp = POLY_CMD;
  if (strcmp(algorithm, "fast") == 0)
  {
    if (exponent == 1)
      res->data = (void*)p_Copy(f, currRing);
    else
      res->data = (void*)pFastPower(f, exponent, currRing);
  }
  else if (strcmp(algorithm, "standard") == 0)
    res->data = (void*)pPower(p_Copy(f, currRing), exponent);
  else if (strcmp(algorithm, "multinomial") == 0)
  {
    if ((rChar(currRing) != 0) || (exponent <= 1) || (pLength(f) <= 1))
    {
      WerrorS("the multinomial algorithm requires characteristic 0, exponent at least 2, and at least two terms");
      return TRUE;
    }
    res->data = (void*)pFastPowerMC(f, exponent, currRing);
  }
  else
  {
    WerrorS("powerWithAlgorithm: algorithm must be `fast`, `standard`, or `multinomial`");
    return TRUE;
  }
  return errorreported;
}

static BOOLEAN jjExtraBitSubst(leftv res, leftv h)
{
  if (jjExtraRequireRing("bitSubst") || !jjExtraIsPoly(h) ||
      !jjExtraIsPoly(h->next) || (h->next->next != NULL))
  {
    WerrorS("expected `bitSubst(poly,poly)`");
    return TRUE;
  }
  poly outer = jjExtraPoly(h);
  if ((outer != NULL) &&
      (!rHasGlobalOrdering(currRing) || !jjExtraIsUnivariateInFirst(outer)))
  {
    WerrorS("bitSubst requires a global ordering and an outer polynomial in the first variable");
    return TRUE;
  }
  res->rtyp = POLY_CMD;
  res->data = (outer == NULL)
    ? NULL : (void*)uni_subst_bits(outer, jjExtraPoly(h->next), currRing);
  return FALSE;
}

static BOOLEAN jjExtraSubring(leftv res, leftv h)
{
  if (jjExtraRequireRing("subring") || (h == NULL))
  {
    WerrorS("expected `subring(variable,...)`");
    return TRUE;
  }
  extern ring rSubring(ring r, leftv v);
  res->rtyp = RING_CMD;
  res->data = (void*)rSubring(currRing, h);
  return res->data == NULL;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
static BOOLEAN jjExtraHNF(leftv res, leftv h)
{
  if ((h == NULL) || (h->next != NULL))
  {
    WerrorS("expected `HNF(matrix|intmat|bigintmat)`");
    return TRUE;
  }
  res->rtyp = h->Typ();
  if (h->Typ() == MATRIX_CMD)
  {
    if (jjExtraRequireRing("HNF") || !rField_is_Q(currRing))
    {
      WerrorS("HNF of a matrix requires coefficient field QQ");
      return TRUE;
    }
    res->data = (void*)singntl_HNF((matrix)h->Data(), currRing);
  }
  else if (h->Typ() == INTMAT_CMD)
    res->data = (void*)singntl_HNF((intvec*)h->Data());
  else if (h->Typ() == BIGINTMAT_CMD)
    res->data = (void*)singntl_HNF((bigintmat*)h->Data());
  else
  {
    WerrorS("expected `HNF(matrix|intmat|bigintmat)`");
    return TRUE;
  }
  return res->data == NULL;
}
#endif

static BOOLEAN jjExtraProbIrredTest(leftv res, leftv h)
{
  if (jjExtraRequireRing("probIrredTest") || !jjExtraIsPoly(h) ||
      (h->next == NULL) || (h->next->Typ() != STRING_CMD) ||
      (h->next->next != NULL))
  {
    WerrorS("expected `probIrredTest(poly,string)`");
    return TRUE;
  }
  if (!rField_is_Zp(currRing))
  {
    WerrorS("probIrredTest requires a prime field as coefficient field");
    return TRUE;
  }

  const char *errorString = (const char*)h->next->Data();
  char *end = NULL;
  errno = 0;
  const double error = strtod(errorString, &end);
  if ((errno == ERANGE) || (end == errorString) || (*end != '\0') ||
      (error <= 0.0) || (error >= 1.0))
  {
    WerrorS("probIrredTest requires an error probability between 0 and 1");
    return TRUE;
  }

  CanonicalForm f = convSingPFactoryP(jjExtraPoly(h), currRing);
  res->rtyp = INT_CMD;
  res->data = (void*)(long)probIrredTest(f, error);
  return FALSE;
}

static BOOLEAN jjExtraLoadBigint(leftv res, leftv h)
{
  const short t[] = {1, STRING_CMD};
  if (!iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `loadBigint(string)`");
    return TRUE;
  }
  FILE *file = fopen((const char*)h->Data(), "r");
  if (file == NULL)
  {
    Werror("cannot open `%s`", (const char*)h->Data());
    return TRUE;
  }

  mpz_t value;
  mpz_init(value);
  const size_t read = mpz_inp_str(value, file, 10);
  fclose(file);
  if (read == 0)
  {
    mpz_clear(value);
    WerrorS("loadBigint could not read a base-10 integer");
    return TRUE;
  }
  res->rtyp = BIGINT_CMD;
  res->data = (void*)n_InitMPZ(value, coeffs_BIGINT);
  mpz_clear(value);
  return FALSE;
}

static BOOLEAN jjExtraIntvecMatchingSegments(leftv res, leftv h)
{
  const short t[] = {2, INTVEC_CMD, INTVEC_CMD};
  if (!iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `intvecMatchingSegments(intvec,intvec)`");
    return TRUE;
  }
  intvec *first = (intvec*)h->Data();
  intvec *second = (intvec*)h->next->Data();
  if ((first->length() == 0) || (second->length() == 0))
  {
    WerrorS("intvecMatchingSegments requires two non-empty intvecs");
    return TRUE;
  }

  intvec *result = new intvec(1);
  (*result)[0] = 0;
  int matches = 0;
  for (int k = 0; k <= first->length() - second->length(); k++)
  {
    if (memcmp(&(*first)[k], &(*second)[0],
               sizeof(int) * second->length()) == 0)
    {
      if (matches == 0)
        (*result)[0] = k + 1;
      else
      {
        result->resize(matches + 1);
        (*result)[matches] = k + 1;
      }
      matches++;
    }
  }
  res->rtyp = INTVEC_CMD;
  res->data = (void*)result;
  return FALSE;
}

static BOOLEAN jjExtraIntvecOverlap(leftv res, leftv h)
{
  const short t[] = {2, INTVEC_CMD, INTVEC_CMD};
  if (!iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `intvecOverlap(intvec,intvec)`");
    return TRUE;
  }
  intvec *first = (intvec*)h->Data();
  intvec *second = (intvec*)h->next->Data();
  if ((first->length() == 0) || (second->length() == 0))
  {
    WerrorS("intvecOverlap requires two non-empty intvecs");
    return TRUE;
  }

  const int firstLength = first->length();
  int overlap = si_min(firstLength, second->length());
  while ((overlap >= 1) &&
         (memcmp(&(*first)[firstLength - overlap], &(*second)[0],
                 sizeof(int) * overlap) != 0))
    overlap--;
  res->rtyp = INT_CMD;
  res->data = (void*)(long)overlap;
  return FALSE;
}

#if defined(HAVE_CCLUSTER) && defined(HAVE_FLINT)
static BOOLEAN jjExtraCcluster(leftv res, leftv h)
{
  if (jjExtraRequireRing("ccluster") || !jjExtraIsPoly(h))
  {
    WerrorS("expected `ccluster(poly[,poly],number,number,number,number)`");
    return TRUE;
  }
  if (!rField_is_Q(currRing) && !rField_is_R(currRing) &&
      !rField_is_long_R(currRing))
  {
    WerrorS("ccluster requires rational or real coefficients");
    return TRUE;
  }

  const BOOLEAN complexCoefficients =
    jjExtraIsPoly(h->next);
  leftv values = complexCoefficients ? h->next->next : h->next;
  leftv check = values;
  int numberArguments = 0;
  while ((check != NULL) && (check->Typ() == NUMBER_CMD))
  {
    numberArguments++;
    check = check->next;
  }
  if ((numberArguments != 4) || (check != NULL))
  {
    WerrorS("expected `ccluster(poly[,poly],number,number,number,number)`");
    return TRUE;
  }
  if ((jjExtraPoly(h) == NULL) ||
      !jjExtraIsUnivariateInFirst(jjExtraPoly(h)) ||
      (complexCoefficients &&
       !jjExtraIsUnivariateInFirst(jjExtraPoly(h->next))))
  {
    WerrorS("ccluster requires non-zero univariate polynomial data");
    return TRUE;
  }

  fmpq_poly_t realPart, imaginaryPart;
  convSingPFlintP(realPart, jjExtraPoly(h), currRing);
  h = h->next;
  if (complexCoefficients)
  {
    convSingPFlintP(imaginaryPart, jjExtraPoly(h), currRing);
    h = h->next;
  }

  fmpq_t centerReal, centerImaginary, boxSize, epsilon;
  convSingNFlintN(centerReal, (number)h->Data(), currRing->cf);
  h = h->next;
  convSingNFlintN(centerImaginary, (number)h->Data(), currRing->cf);
  h = h->next;
  convSingNFlintN(boxSize, (number)h->Data(), currRing->cf);
  h = h->next;
  convSingNFlintN(epsilon, (number)h->Data(), currRing->cf);

  const int degreeBound = complexCoefficients
    ? si_max(fmpq_poly_length(realPart), fmpq_poly_length(imaginaryPart))
    : fmpq_poly_length(realPart);
  fmpq_t *realCenters =
    (fmpq_t*)omAlloc(degreeBound * sizeof(fmpq_t));
  fmpq_t *imaginaryCenters =
    (fmpq_t*)omAlloc(degreeBound * sizeof(fmpq_t));
  int *multiplicities = (int*)omAlloc(degreeBound * sizeof(int));
  for (int i = 0; i < degreeBound; i++)
  {
    fmpq_init(realCenters[i]);
    fmpq_init(imaginaryCenters[i]);
  }

  const int verbosity = 0;
  long threads = (long)feOptValue(FE_OPT_CPUS);
  if (threads < 1) threads = 1;
  const int strategy = 23 + (threads << 6);
  int count;
  if (complexCoefficients)
    count = ccluster_interface_poly_real_imag(
      realCenters, imaginaryCenters, multiplicities, realPart, imaginaryPart,
      centerReal, centerImaginary, boxSize, epsilon, strategy, verbosity);
  else
    count = ccluster_interface_poly_real(
      realCenters, imaginaryCenters, multiplicities, realPart,
      centerReal, centerImaginary, boxSize, epsilon, strategy, verbosity);

  lists result = (lists)omAlloc0Bin(slists_bin);
  result->Init(count);
  for (int i = 0; i < count; i++)
  {
    lists cluster = (lists)omAlloc0Bin(slists_bin);
    cluster->Init(3);
    cluster->m[0].rtyp = NUMBER_CMD;
    cluster->m[0].data = convFlintNSingN(realCenters[i], currRing->cf);
    cluster->m[1].rtyp = NUMBER_CMD;
    cluster->m[1].data = convFlintNSingN(imaginaryCenters[i], currRing->cf);
    cluster->m[2].rtyp = INT_CMD;
    cluster->m[2].data = (void*)(long)multiplicities[i];
    result->m[i].rtyp = LIST_CMD;
    result->m[i].data = cluster;
  }

  for (int i = degreeBound - 1; i >= 0; i--)
  {
    fmpq_clear(realCenters[i]);
    fmpq_clear(imaginaryCenters[i]);
  }
  omFree(realCenters);
  omFree(imaginaryCenters);
  omFree(multiplicities);
  fmpq_clear(centerReal);
  fmpq_clear(centerImaginary);
  fmpq_clear(boxSize);
  fmpq_clear(epsilon);
  fmpq_poly_clear(realPart);
  if (complexCoefficients) fmpq_poly_clear(imaginaryPart);

  res->rtyp = LIST_CMD;
  res->data = result;
  return FALSE;
}
#endif

static BOOLEAN jjExtraEvaluateAt(leftv res, leftv h)
{
  if (jjExtraRequireRing("evaluateAt") || !jjExtraIsPoly(h))
  {
    WerrorS("expected `evaluateAt(poly,number,...)`");
    return TRUE;
  }
  poly p = jjExtraPoly(h);
  h = h->next;
  number *point = (number*)omAlloc0(sizeof(number) * currRing->N);
  int initialized = 0;
  for (int i = 0; i < currRing->N; i++)
  {
    if ((h == NULL) ||
        ((h->Typ() != NUMBER_CMD) && (h->Typ() != INT_CMD)))
    {
      for (int j = 0; j < initialized; j++)
        n_Delete(&point[j], currRing->cf);
      omFree(point);
      Werror("evaluateAt requires exactly %d coordinates", currRing->N);
      return TRUE;
    }
    if (h->Typ() == NUMBER_CMD)
      point[i] = n_Copy((number)h->Data(), currRing->cf);
    else
      point[i] = n_Init((long)h->Data(), currRing->cf);
    initialized++;
    h = h->next;
  }
  if (h != NULL)
  {
    for (int j = 0; j < initialized; j++)
      n_Delete(&point[j], currRing->cf);
    omFree(point);
    Werror("evaluateAt requires exactly %d coordinates", currRing->N);
    return TRUE;
  }

  res->rtyp = NUMBER_CMD;
  res->data = (void*)maEvalAt(p, point, currRing);
  for (int i = 0; i < initialized; i++)
    n_Delete(&point[i], currRing->cf);
  omFree(point);
  return FALSE;
}

static BOOLEAN jjExtraDivrem(leftv res, leftv h)
{
  if (jjExtraRequireRing("divrem") || !jjExtraIsPoly(h) ||
      !jjExtraIsPoly(h->next) || (h->next->next != NULL))
  {
    WerrorS("expected `divrem(poly,poly)`");
    return TRUE;
  }
  if (jjExtraPoly(h->next) == NULL)
  {
    WerrorS("division by zero");
    return TRUE;
  }

  poly remainder = NULL;
  poly dividend = jjExtraPoly(h);
  poly divisor = jjExtraPoly(h->next);
  poly quotient = p_DivRem(p_Copy(dividend, currRing),
                           p_Copy(divisor, currRing),
                           remainder, currRing);
  if (errorreported)
  {
    p_Delete(&quotient, currRing);
    p_Delete(&remainder, currRing);
    return TRUE;
  }

  // Some p_DivRem backends do not preserve their remainder input.  Derive
  // the public result from the defining identity without changing them.
  p_Delete(&remainder, currRing);
  remainder = p_Sub(p_Copy(dividend, currRing),
                    pp_Mult_qq(divisor, quotient, currRing), currRing);

  lists result = (lists)omAllocBin(slists_bin);
  result->Init(2);
  result->m[0].rtyp = POLY_CMD;
  result->m[0].data = (void*)quotient;
  result->m[1].rtyp = POLY_CMD;
  result->m[1].data = (void*)remainder;
  res->rtyp = LIST_CMD;
  res->data = result;
  return FALSE;
}

static BOOLEAN jjExtraIdealDivrem(leftv res, leftv h)
{
  const short ideals[] = {2, IDEAL_CMD, IDEAL_CMD};
  const BOOLEAN withUnit =
    (h != NULL) && (h->next != NULL) && (h->next->next != NULL);
  if (jjExtraRequireRing("idealDivRem") ||
      (!withUnit && !iiCheckTypes(h, ideals, 0)))
  {
    WerrorS("expected `idealDivRem(ideal,ideal[,\"withUnit\"])`");
    return TRUE;
  }
  if (withUnit)
  {
    const short idealsWithUnit[] =
      {3, IDEAL_CMD, IDEAL_CMD, STRING_CMD};
    if (!iiCheckTypes(h, idealsWithUnit, 0) ||
        (strcmp((const char*)h->next->next->Data(), "withUnit") != 0))
    {
      WerrorS("expected `idealDivRem(ideal,ideal[,\"withUnit\"])`");
      return TRUE;
    }
  }

  ideal factors = NULL;
  ideal unit = NULL;
  ideal remainder = idDivRem((ideal)h->Data(), (ideal)h->next->Data(),
                             factors, withUnit ? &unit : NULL, 0);
  if (errorreported)
  {
    id_Delete(&remainder, currRing);
    id_Delete(&factors, currRing);
    id_Delete(&unit, currRing);
    return TRUE;
  }

  lists result = (lists)omAllocBin(slists_bin);
  if (withUnit)
  {
    result->Init(3);
    result->m[0].rtyp = MODUL_CMD;
    result->m[0].data = (void*)remainder;
    result->m[1].rtyp = MODUL_CMD;
    result->m[1].data = (void*)factors;
    result->m[2].rtyp = MODUL_CMD;
    result->m[2].data = (void*)unit;
  }
  else
  {
    result->Init(2);
    result->m[0].rtyp = h->Typ();
    result->m[0].data = (void*)remainder;
    result->m[1].rtyp = MATRIX_CMD;
    result->m[1].data = (void*)id_Module2Matrix(factors, currRing);
  }
  res->rtyp = LIST_CMD;
  res->data = result;
  return FALSE;
}

static BOOLEAN jjExtraCoeffTerm(leftv res, leftv h)
{
  if (jjExtraRequireRing("coeffTerm")) return TRUE;
  if ((h == NULL) || (h->next == NULL) || (h->next->next != NULL))
  {
    WerrorS("expected `coeffTerm(poly|vector|ideal|module,poly|vector|ideal|module)`");
    return TRUE;
  }
  leftv first = h;
  leftv second = h->next;

  if ((jjExtraIsPoly(first) && jjExtraIsPoly(second)) ||
      (jjExtraIsVector(first) && jjExtraIsVector(second)))
  {
    res->rtyp = NUMBER_CMD;
    res->data = (void*)p_CoeffTerm(jjExtraPoly(first),
                                   jjExtraPoly(second), currRing);
  }
  else if (((first->Typ() == IDEAL_CMD) && jjExtraIsPoly(second)) ||
           ((first->Typ() == MODUL_CMD) && jjExtraIsVector(second)))
  {
    res->rtyp = first->Typ();
    res->data = (void*)id_CoeffTerm((ideal)first->Data(),
                                    jjExtraPoly(second), currRing);
  }
  else if (jjExtraIsVector(first) && jjExtraIsPoly(second))
  {
    poly monomial = p_Copy(jjExtraPoly(second), currRing);
    res->rtyp = VECTOR_CMD;
    res->data = (void*)p_CoeffTermV(jjExtraPoly(first), monomial, currRing);
    p_Delete(&monomial, currRing);
  }
  else if ((first->Typ() == MODUL_CMD) && jjExtraIsPoly(second))
  {
    poly monomial = p_Copy(jjExtraPoly(second), currRing);
    res->rtyp = MODUL_CMD;
    res->data = (void*)id_CoeffTermV((ideal)first->Data(), monomial, currRing);
    p_Delete(&monomial, currRing);
  }
  else if (jjExtraIsVector(first) && (second->Typ() == IDEAL_CMD))
  {
    poly vector = jjExtraPoly(first);
    res->rtyp = VECTOR_CMD;
    res->data = (void*)p_CoeffTermId(vector, (ideal)second->Data(),
                                     p_MaxComp(vector, currRing), currRing);
  }
  else if (jjExtraIsVector(first) && (second->Typ() == MODUL_CMD))
  {
    res->rtyp = VECTOR_CMD;
    res->data = (void*)p_CoeffTermMo(jjExtraPoly(first),
                                     (ideal)second->Data(), currRing);
  }
  else
  {
    WerrorS("expected `coeffTerm(poly|vector|ideal|module,poly|vector|ideal|module)`");
    return TRUE;
  }
  return FALSE;
}

static BOOLEAN jjExtraSatPrincipal(leftv res, leftv h)
{
  const short t[] = {2, IDEAL_CMD, IDEAL_CMD};
  if (jjExtraRequireRing("sat") || !iiCheckTypes(h, t, 0))
  {
    WerrorS("expected two ideals");
    return TRUE;
  }
  ideal divisor = (ideal)h->next->Data();
  if ((IDELEMS(divisor) != 1) || (divisor->m[0] == NULL))
  {
    WerrorS("the principal saturation algorithm requires exactly one non-zero generator");
    return TRUE;
  }
  res->rtyp = IDEAL_CMD;
  res->data = (void*)id_Sat_principal((ideal)h->Data(), divisor, currRing);
  return errorreported;
}

static BOOLEAN jjExtraMinresWithMap(leftv res, leftv h)
{
  const short t[] = {1, RESOLUTION_CMD};
  if (jjExtraRequireRing("minresWithMap") || !iiCheckTypes(h, t, 0))
  {
    WerrorS("expected `minresWithMap(resolution)`");
    return TRUE;
  }
  syStrategy resolution = syCopy((syStrategy)h->Data());
  ideal transformation = NULL;
  syMinimize_with_map(resolution, transformation);
  if (errorreported)
  {
    syKillComputation(resolution, currRing);
    id_Delete(&transformation, currRing);
    return TRUE;
  }
  res->rtyp = RESOLUTION_CMD;
  res->data = (void*)resolution;
  res->next = (leftv)omAlloc0Bin(sleftv_bin);
  res->next->rtyp = MODUL_CMD;
  res->next->data = (void*)transformation;
  return FALSE;
}

static BOOLEAN jjExtraSsiWriteToString(leftv res, leftv h)
{
  if ((h == NULL) || (h->next != NULL))
  {
    WerrorS("expected one argument");
    return TRUE;
  }
  StringSetS("");
  ssiWrite_S(h, currRing);
  res->rtyp = STRING_CMD;
  res->data = (void*)StringEndS();
  return (res->data == NULL) || errorreported;
}

static BOOLEAN jjExtraSsiReadFromString(leftv res, leftv h)
{
  const short t[] = {1, STRING_CMD};
  if (!iiCheckTypes(h, t, 0))
  {
    WerrorS("expected one string argument");
    return TRUE;
  }
  char *input = (char*)h->Data();
  leftv value = ssiRead1_S(&input, currRing);
  if (value == NULL) return TRUE;
  memcpy(res, value, sizeof(*value));
  omFreeBin(value, sleftv_bin);
  return FALSE;
}

static BOOLEAN jjEXPERIMENTAL_SYSTEM(leftv res, leftv h,
                                     BOOLEAN *handled)
{
  *handled = TRUE;
  if ((h == NULL) || (h->Typ() != STRING_CMD))
  {
    *handled = FALSE;
    return TRUE;
  }
  char *sys_cmd=(char *)(h->Data());
  h=h->next;

  if(strcmp(sys_cmd,"setsyzcomp")==0)
  {
    if ((h!=NULL) && (h->Typ()==INT_CMD))
    {
      int k = (int)(long)h->Data();
      if ( currRing->order[0] == ringorder_s )
      {
        rSetSyzComp(k, currRing);
        return FALSE;
      }
    }
  }

  if (strcmp(sys_cmd, "mults")==0)
  {
    res->rtyp=INT_CMD ;
    res->data=(void*)(long) Mults();
    return(FALSE);
  }
  else

  if (strcmp(sys_cmd, "gcd") == 0)
  {
    if (h==NULL)
    {
    #if 0
      Print("FLINT_P:%d (use Flints gcd for polynomials in char p)\n",isOn(SW_USE_FL_GCD_P));
      Print("FLINT_0:%d (use Flints gcd for polynomials in char 0)\n",isOn(SW_USE_FL_GCD_0));
    #endif
      Print("EZGCD:%d (use EZGCD for gcd of polynomials in char 0)\n",isOn(SW_USE_EZGCD));
      Print("EZGCD_P:%d (use EZGCD_P for gcd of polynomials in char p)\n",isOn(SW_USE_EZGCD_P));
      Print("CRGCD:%d (use chinese Remainder for gcd of polynomials in char 0)\n",isOn(SW_USE_CHINREM_GCD));
      #ifndef __CYGWIN__
      Print("homog:%d (use homog. test for factorization of polynomials)\n",singular_homog_flag);
      #endif
      return FALSE;
    }
    else
    if ((h!=NULL) && (h->Typ()==STRING_CMD)
    && (h->next!=NULL) && (h->next->Typ()==INT_CMD))
    {
      int d=(int)(long)h->next->Data();
      char *s=(char *)h->Data();
    #if 0
      if (strcmp(s,"FLINT_P")==0) { if (d) On(SW_USE_FL_GCD_P); else Off(SW_USE_FL_GCD_P); } else
      if (strcmp(s,"FLINT_0")==0) { if (d) On(SW_USE_FL_GCD_0); else Off(SW_USE_FL_GCD_0); } else
    #endif
      if (strcmp(s,"EZGCD")==0) { if (d) On(SW_USE_EZGCD); else Off(SW_USE_EZGCD); } else
      if (strcmp(s,"EZGCD_P")==0) { if (d) On(SW_USE_EZGCD_P); else Off(SW_USE_EZGCD_P); } else
      if (strcmp(s,"CRGCD")==0) { if (d) On(SW_USE_CHINREM_GCD); else Off(SW_USE_CHINREM_GCD); } else
      #ifndef __CYGWIN__
      if (strcmp(s,"homog")==0) { if (d) singular_homog_flag=1; else singular_homog_flag=0; } else
      #endif
      return TRUE;
      return FALSE;
    }
    else return TRUE;
  }
  else

/* ============ NCUseExtensions ======================== */
  #ifdef HAVE_PLURAL
  if(strcmp(sys_cmd,"NCUseExtensions")==0)
  {
    if ((h!=NULL) && (h->Typ()==INT_CMD))
      res->data=(void *)(long)setNCExtensions( (int)((long)(h->Data())) );
    else
      res->data=(void *)(long)getNCExtensions();
    res->rtyp=INT_CMD;
    return FALSE;
  }
  else
  #endif
/* ============ NCGetType ======================== */
  #ifdef HAVE_PLURAL
  if(strcmp(sys_cmd,"NCGetType")==0)
  {
    res->rtyp=INT_CMD;
    if( rIsPluralRing(currRing) )
      res->data=(void *)(long)ncRingType(currRing);
    else
      res->data=(void *)(-1L);
    return FALSE;
  }
  else
  #endif
/* ============ ForceSCA ======================== */
  #ifdef HAVE_PLURAL
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
  else
  #endif
/* ============ ForceNewNCMultiplication ======================== */
  #ifdef HAVE_PLURAL
  if(strcmp(sys_cmd,"ForceNewNCMultiplication")==0)
  {
    if( !rIsPluralRing(currRing) )
      return TRUE;
    if( ncInitSpecialPairMultiplication(currRing) ) // No Plural!
      return TRUE;
    return FALSE;
  }
  else
  #endif
/* ============ ForceNewOldNCMultiplication ======================== */
  #ifdef HAVE_PLURAL
  if(strcmp(sys_cmd,"ForceNewOldNCMultiplication")==0)
  {
    if( !rIsPluralRing(currRing) )
      return TRUE;
    if( !ncInitSpecialPowersMultiplication(currRing) ) // Enable Formula for Plural (depends on swiches)!
      return TRUE;
    return FALSE;
  }
  else
  #endif

  if(strcmp(sys_cmd,"cache_chinrem")==0)
  {
    EXTERN_VAR int n_SwitchChinRem;
    Print("caching inverse in chines remainder:%d\n",n_SwitchChinRem);
    if ((h!=NULL)&&(h->Typ()==INT_CMD))
      n_SwitchChinRem=(int)(long)h->Data();
    return FALSE;
  }
  else
  {
    *handled = FALSE;
    return TRUE;
  }
}

void iiInitExtraCprocs()
{
  iiAddCproc("kernel", "locNF", FALSE, jjExtraLocNF);
  iiAddCproc("kernel", "fglmCombination", FALSE, jjExtraFglmCombination);
  iiAddCproc("kernel", "matrixMinpoly", FALSE, jjExtraMatrixMinpoly);
  iiAddCproc("kernel", "fastMult", FALSE, jjExtraFastMult);
  iiAddCproc("kernel", "powerWithAlgorithm", FALSE, jjExtraPower);
  iiAddCproc("kernel", "bitSubst", FALSE, jjExtraBitSubst);
  iiAddCproc("kernel", "subring", FALSE, jjExtraSubring);
#if defined(HAVE_NTL) || defined(HAVE_FLINT)
  iiAddCproc("kernel", "HNF", FALSE, jjExtraHNF);
#endif
  iiAddCproc("kernel", "probIrredTest", FALSE, jjExtraProbIrredTest);
  iiAddCproc("kernel", "loadBigint", FALSE, jjExtraLoadBigint);
  iiAddCproc("kernel", "intvecMatchingSegments", FALSE,
             jjExtraIntvecMatchingSegments);
  iiAddCproc("kernel", "intvecOverlap", FALSE, jjExtraIntvecOverlap);
#if defined(HAVE_CCLUSTER) && defined(HAVE_FLINT)
  iiAddCproc("kernel", "ccluster", FALSE, jjExtraCcluster);
#endif
  iiAddCproc("kernel", "evaluateAt", FALSE, jjExtraEvaluateAt);
  iiAddCproc("kernel", "divrem", FALSE, jjExtraDivrem);
  iiAddCproc("kernel", "idealDivRem", FALSE, jjExtraIdealDivrem);
  iiAddCproc("kernel", "coeffTerm", FALSE, jjExtraCoeffTerm);
  iiAddCproc("kernel", "minresWithMap", FALSE, jjExtraMinresWithMap);
  iiAddCproc("kernel", "satPrincipalInternal", FALSE,
             jjExtraSatPrincipal);
  iiAddCproc("kernel", "ssiWriteToStringInternal", FALSE,
             jjExtraSsiWriteToString);
  iiAddCproc("kernel", "ssiReadFromStringInternal", FALSE,
             jjExtraSsiReadFromString);
}
