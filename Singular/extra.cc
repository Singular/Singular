/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: extra.cc,v 1.42 1998-04-27 15:00:50 krueger Exp $ */
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mod2.h"

#ifndef macintosh
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
#endif

#include "tok.h"
#include "ipid.h"
#include "polys.h"
#include "kutil.h"
#include "cntrlc.h"
#include "stairc.h"
#include "ipshell.h"
#include "algmap.h"
#include "modulop.h"
#include "febase.h"
#include "matpol.h"
#include "longalg.h"
#include "ideals.h"
#include "kstd1.h"
#include "syz.h"
#include "polys.h"

#ifdef STDTRACE
//#include "comm.h"
#endif

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapsing.h"
#include "clapconv.h"
#include "kstdfac.h"
#include "fglmcomb.cc"
#include "fglm.h"
#endif

#include "silink.h"
#include "mpsr.h"

#ifdef HAVE_DYNAMIC_LOADING
#include <dlfcn.h>
#endif /* HAVE_DYNAMIC_LOADING */

// see clapsing.cc for a description of the `FACTORY_*' options

#ifdef FACTORY_GCD_STAT
#include "gcd_stat.h"
#endif

#ifdef FACTORY_GCD_TIMING
#define TIMING
#include "timing.h"
TIMING_DEFINE_PRINTPROTO( contentTimer );
TIMING_DEFINE_PRINTPROTO( algContentTimer );
TIMING_DEFINE_PRINTPROTO( algLcmTimer );
#endif

void piShowProcList();

//void emStart();
/*2
*  the "system" command
*/
BOOLEAN jjSYSTEM(leftv res, leftv h)
{
  if(h->Typ() == STRING_CMD)
  {
/*==================== lib ==================================*/
    if(strcmp((char*)(h->Data()),"LIB")==0)
    {
#ifdef HAVE_NAMESPACES
      idhdl hh=namespaceroot->get((char*)h->next->Data(),0);
#else /* HAVE_NAMESPACES */
      idhdl hh=idroot->get((char*)h->next->Data(),0);
#endif /* HAVE_NAMESPACES */
      if ((hh!=NULL)&&(IDTYP(hh)==PROC_CMD))
      {
        res->rtyp=STRING_CMD;
        res->data=mstrdup(iiGetLibName(IDPROC(hh)));
        if (res->data==NULL) res->data=mstrdup("");
        return FALSE;
      }
      else
        Warn("`%s` not found",(char*)h->next->Data());
    }
    else
#ifdef HAVE_NAMESPACES
/*==================== nspush ===================================*/
    if(strcmp((char*)(h->Data()),"nspush")==0)
    {
      idhdl hh=namespaceroot->get((char*)h->next->Data(),0, TRUE);
      if ((hh!=NULL)&&(IDTYP(hh)==PACKAGE_CMD))
      {
	namespaceroot = namespaceroot->push(IDPACKAGE(hh), IDID(hh));
	return FALSE;
      } else
        Warn("package `%s` not found",(char*)h->next->Data());
    }
    else
/*==================== nspop ====================================*/
    if(strcmp((char*)(h->Data()),"nspop")==0)
    {
      namespaceroot->pop();
      return FALSE;
    }
    else
/*==================== nsstack ===================================*/
    if(strcmp((char*)(h->Data()),"nsstack")==0)
    {
      namehdl nshdl = namespaceroot;
      for( ; nshdl->isroot != TRUE; nshdl = nshdl->next) {
	Print("NSstack: %s:%d\n", nshdl->name, nshdl->lev);
      }
      Print("NSstack: %s:%d\n", nshdl->name, nshdl->lev);
      return FALSE;
    }
    else
#endif /* HAVE_NAMESPACES */
/*==================== proclist =================================*/
    if(strcmp((char*)(h->Data()),"proclist")==0)
    {
      //res->rtyp=STRING_CMD;
      //res->data=(void *)mstrdup("");
      piShowProcList();
      return FALSE;
    }
    else
/*==================== version ==================================*/
    if(strcmp((char*)(h->Data()),"version")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)SINGULAR_VERSION;
      return FALSE;
    }
    else
#ifdef HAVE_DYNAMIC_LOADING
/*==================== load ==================================*/
    if(strcmp((char*)(h->Data()),"load")==0)
    {
      if ((h->next!=NULL) && (h->next->Typ()==STRING_CMD)) {
	int iiAddCproc(char *libname, char *procname, BOOLEAN pstatic,
		       BOOLEAN(*func)(leftv res, leftv v));
        int (*fktn)(int(*iiAddCproc)(char *libname, char *procname,
				     BOOLEAN pstatic,
				     BOOLEAN(*func)(leftv res, leftv v)));
        void *vp;
        res->rtyp=STRING_CMD;

        fprintf(stderr, "Loading %s\n", h->next->Data());
        res->data=(void *)mstrdup("");
        if((vp=dlopen(h->next->Data(),RTLD_LAZY))==(void *)NULL) {
          WerrorS("dlopen failed");
          Werror("%s not found", h->next->Data());
        } else {
          fktn = dlsym(vp, "mod_init");
          if( fktn!= NULL) (*fktn)(iiAddCproc);
          else Werror("mod_init: %s\n", dlerror());
          piShowProcList();
        }
        return FALSE;
      }
      else WerrorS("string expected");
    }
    else
#endif /* HAVE_DYNAMIC_LOADING */
/*==================== gen ==================================*/
    if(strcmp((char*)(h->Data()),"gen")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)npGen;
      return FALSE;
    }
    else
/*==================== sh ==================================*/
    #ifndef macintosh
    if(strcmp((char*)(h->Data()),"sh")==0)
    {
      #ifndef MSDOS
      #ifdef HAVE_FEREAD
      fe_temp_reset();
      #endif
      #endif
      res->rtyp=INT_CMD;
      if (h->next==NULL) res->data = (void *)system("/bin/sh");
      else res->data = (void*) system((char*)(h->next->Data()));
      #ifndef MSDOS
      #ifdef HAVE_FEREAD
      fe_temp_set();
      #endif
      #endif
      return FALSE;
    }
    else
    #endif
/*==================== with ==================================*/
    if(strcmp((char*)(h->Data()),"with")==0)
    {
      if (h->next==NULL)
      {
        res->rtyp=STRING_CMD;
        res->data=(void *)mstrdup(versionString());
        return FALSE;
      }
      else if (h->next->Typ()==STRING_CMD)
      {
        #define TEST_FOR(A) if(strcmp(s,A)==0) res->data=(void *)1; else
        char *s=(char *)h->next->Data();
        res->rtyp=INT_CMD;
        #ifdef DRING
          TEST_FOR("DRING")
        #endif
        #ifdef HAVE_DBM
          TEST_FOR("DBM")
        #endif
        #ifdef HAVE_DLD
          TEST_FOR("DLD")
        #endif
        #ifdef HAVE_GMP
          TEST_FOR("gmp")
        #endif
        #ifdef HAVE_FACTORY
          TEST_FOR("factory")
        #endif
        #ifdef HAVE_LIBFAC_P
          TEST_FOR("libfac")
        #endif
        #ifdef HAVE_MPSR
          TEST_FOR("MP")
        #endif
        #ifdef HAVE_READLINE
          TEST_FOR("readline")
        #endif
        #ifdef HAVE_TCL
          TEST_FOR("tcl")
        #endif
        #ifdef SRING
          TEST_FOR("SRING")
        #endif
        #ifdef TEST_MAC_ORDER
          TEST_FOR("MAC_ORDER");
        #endif
          ;
        return FALSE;
        #undef TEST_FOR
      }
      return TRUE;
    }
    else
/*==================== pid ==================================*/
    #ifndef MSDOS
    #ifndef macintosh
    if (strcmp((char*)(h->Data()),"pid")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)getpid();
      return FALSE;
    }
    else
    #endif
    #endif
/*==================== getenv ==================================*/
    if (strcmp((char*)(h->Data()),"getenv")==0)
    {
      if ((h->next!=NULL) && (h->next->Typ()==STRING_CMD))
      {
        res->rtyp=STRING_CMD;
        char *r=getenv((char *)h->next->Data());
        if (r==NULL) r="";
        res->data=(void *)mstrdup(r);
        return FALSE;
      }
      else
      {
        WerrorS("string expected");
      }
    }
    else
/*==================== tty ==================================*/
    #ifndef macintosh
    #ifndef MSDOS
    #if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
    if (strcmp((char*)(h->Data()),"tty")==0)
    {
      #ifdef HAVE_FEREAD
      #ifdef HAVE_ATEXIT
      fe_reset_input_mode();
      #else
      fe_reset_input_mode(0,NULL);
      #endif
      #elif defined(HAVE_READLINE)
      system("stty sane");
      #endif
      if ((h->next!=NULL)&&(h->next->Typ()==INT_CMD))
      {
        fe_use_fgets=(int)h->next->Data();
        fe_set_input_mode();
      }
      return FALSE;
    }
    else
    #endif
    #endif
    #endif
/*==================== whoami ==================================*/
    if (strcmp((char*)(h->data), "whoami") == 0)
    {
      res->rtyp=STRING_CMD;
      res->data=(void*)feGetExpandedExecutable();
      if (res->data != NULL)
        res->data = (void*) mstrdup((char*) res->data);
      else
        res->data = (void*) mstrdup("");
      return FALSE;
    }
    else
/*==================== options ==================================*/
    if (strstr((char*)(h->data), "--") == (char*)(h->data))
    {
      BOOLEAN mainGetSingOptionValue(const char* name, char** result);
      char* val;
      
      if (mainGetSingOptionValue(&((char*)(h->data))[2], &val))
      {
        res->data = (void*) val;
        if ((unsigned int) val > 1)
        {
          res->rtyp=STRING_CMD;
          res->data = (void*) mstrdup((char*) res->data);
        }
        else
          res->rtyp=INT_CMD;
        return FALSE;
      }
      else
      {
        Werror("Unknown option %s\n", (char*)(h->data));
        return TRUE;
      }
    }
    else
/*==================== print all option values =================*/
#ifndef NDEBUG
    if (strcmp((char*)(h->data), "OptionValues") == 0)
    {
      void mainOptionValues();
      mainOptionValues();
      return FALSE;
    }
    else
#endif
/*==================== HC ==================================*/
    if (strcmp((char*)(h->data),"HC")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)HCord;
      return FALSE;
    }
    else
/*==================== random ==================================*/
    if(strcmp((char*)(h->Data()),"random")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==INT_CMD))
      {
        siRandomStart=(int)h->next->Data();
#ifdef buildin_rand
        siSeed=siRandomStart;
#else
        srand((unsigned int)siRandomStart);
#endif
        return FALSE;
      }
      else
        WerrorS("int expected");
    }
    else
/*==================== LaScala ==================================*/
    if(strcmp((char*)(h->Data()),"LaScala")==0)
    {
      if ((h->next!=NULL)
      &&((h->next->Typ()==IDEAL_CMD)||(h->next->Typ()==MODUL_CMD)))
      {
        int dummy;
        res->data=(void *)syLaScala3((ideal)h->next->Data(),&dummy);
        mmTest(res->data,sizeof(ssyStrategy));
        syStrategy s=(syStrategy)res->data;
        for (int i=s->length;i>=0;i--)
        {
          if (s->res[i]!=NULL)
            idTest(s->res[i]);
        }
        res->rtyp=RESOLUTION_CMD;
        return FALSE;
      }
      else
         WerrorS("ideal/module expected");
    }
    else
/*==================== naIdeal ==================================*/
    if(strcmp((char*)(h->Data()),"naIdeal")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        naSetIdeal((ideal)h->next->Data());
        return FALSE;
      }
      else
         WerrorS("ideal expected");
    }
    else
/*==================== isSqrFree =============================*/
#ifdef HAVE_FACTORY
    if(strcmp((char*)(h->Data()),"isSqrFree")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==POLY_CMD))
      {
        res->rtyp=INT_CMD;
        res->data=(void *)singclap_isSqrFree((poly)h->next->Data());
        return FALSE;
      }
      else
        WerrorS("poly expected");
    }
    else
#endif
/*==================== neworder =============================*/
#ifdef HAVE_LIBFAC_P
    if(strcmp((char*)(h->Data()),"neworder")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        res->rtyp=STRING_CMD;
        res->data=(void *)singclap_neworder((ideal)h->next->Data());
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
#endif
/*==================== alarm ==================================*/
#ifndef macintosh
#ifndef MSDOS
#ifndef atarist
#ifdef unix
    if(strcmp((char*)(h->Data()),"alarm")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==INT_CMD))
      {
        // standard variant -> SIGALARM (standard: abort)
        //alarm((unsigned)h->next->Data());
        // process time (user +system): SIGVTALARM
        struct itimerval t,o;
        memset(&t,0,sizeof(t));
        t.it_value.tv_sec     =(unsigned)h->next->Data();
        setitimer(ITIMER_VIRTUAL,&t,&o);
        return FALSE;
      }
      else
        WerrorS("int expected");
    }
    else
#endif
#endif
#endif
#endif
/*==================== red =============================*/
#if 0
    if(strcmp((char*)(h->Data()),"red")==0)
    {
     if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        res->rtyp=IDEAL_CMD;
        res->data=(void *)stdred((ideal)h->next->Data(),NULL,testHomog,NULL);
        setFlag(res,FLAG_STD);
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
#endif
#ifdef HAVE_FACTORY
/*==================== pdivide ====================*/
    if (strcmp((char*)(h->Data()),"pdivide")==0)
    {
      if (h->next!=NULL && h->next->next!=NULL &&
           h->next->Typ()==POLY_CMD && h->next->next->Typ()==POLY_CMD)
      {
        res->rtyp=POLY_CMD;
        res->data=(void*)(singclap_pdivide((poly)(h->next->Data()),
                                         (poly)(h->next->next->Data())));
        return FALSE;
      }
      else
        WerrorS("poly expected");
    }
    else
#endif
/*==================== algfetch =====================*/
    if (strcmp((char*)(h->Data()),"algfetch")==0)
    {
      int k;
      idhdl w;
      ideal i0, i1;
      leftv v=h->next;
      ring r0=(ring)v->Data();
      v = v->next;
      w = r0->idroot->get(v->Name(),myynest);
      i0 = IDIDEAL(w);
      i1 = idInit(IDELEMS(i0),i0->rank);
      for (k=0; k<IDELEMS(i1); k++)
      {
        i1->m[k] = maAlgpolyFetch(r0, i0->m[k]);
      }
      res->rtyp = IDEAL_CMD;
      res->data = (void*)i1;
      return FALSE;
    }
    else
/*==================== algmap =======================*/
    if (strcmp((char*)(h->Data()),"algmap")==0)
    {
      int k;
      idhdl w;
      ideal i0, i1, i, j;
      leftv v=h->next;
      ring r0=(ring)v->Data();
      v = v->next;
      w = r0->idroot->get(v->Name(),myynest);
      i0 = IDIDEAL(w);
      v = v->next;
      i = (ideal)v->Data();
      v = v->next;
      j = (ideal)v->Data();
      i1 = idInit(IDELEMS(i0),i0->rank);
      for (k=0; k<IDELEMS(i1); k++)
      {
        i1->m[k] = maAlgpolyMap(r0, i0->m[k], i, j);
      }
      res->rtyp = IDEAL_CMD;
      res->data = (void*)i1;
      return FALSE;
    }
    else
/*==================== indsetall =============================*/
    if(strcmp((char*)(h->Data()),"indsetall")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        int i=0;
        if (h->next->next!=NULL)
        {
          if (h->next->next->Typ()==INT_CMD)
            i=(int)h->next->next->Data();
          else
          {
            Warn("int expected");
          }
        }
        res->rtyp=LIST_CMD;
        res->data=(void *)scIndIndset((ideal)h->next->Data(),i,currQuotient);
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
#ifdef STDTRACE
    /*==================== trace =============================*/
    /* Parameter : Ideal, Liste mit Links. */
    if(strcmp((char*)(h->Data()),"stdtrace")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        leftv root  = NULL,
              ptr   = NULL,
              lv    = NULL;
        lists l     = NULL;
        ideal I     = (ideal)(h->next->Data());
        lists links = (lists)(h->next->next->Data());
        tHomog hom  = testHomog;
        int rw      = (int)(h->next->next->next->Data());

        if(I==NULL)
          PrintS("I==NULL\n");
        for(int i=0; i <= links->nr ; i++)
        {
          lv = (leftv)Alloc0(sizeof(sleftv));
          lv->Copy(&(links->m[i]));
          if(root==NULL)
          root=lv;
          if(ptr==NULL)
          {
            ptr=lv;
            ptr->next=NULL;
          }
          else
          {
            ptr->next=lv;
            ptr=lv;
          }
        }
        ptr->next=NULL;
        l=TraceStd(root,rw,I,currQuotient,testHomog,NULL);
        idSkipZeroes(((ideal)l->m[0].Data()));
        res->rtyp=LIST_CMD;
        res->data=(void *) l;
        res->next=NULL;
        root->CleanUp();
        Free(root,sizeof(sleftv));
        return FALSE;
      }
      else
         WerrorS("ideal expected");
    }
    else
#endif
#ifdef HAVE_FACTORY
/*==================== fastcomb =============================*/
    if(strcmp((char*)(h->Data()),"fastcomb")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        int i=0;
        if (h->next->next!=NULL)
        {
          if (h->next->next->Typ()!=POLY_CMD)
          {
	      Warn("Wrong types for poly= comb(ideal,poly)");
          }
        }
        res->rtyp=POLY_CMD;
        res->data=(void *)fglmLinearCombination((ideal)h->next->Data(),(poly)h->next->next->Data());
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
/*==================== comb =============================*/
    if(strcmp((char*)(h->Data()),"comb")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        int i=0;
        if (h->next->next!=NULL)
        {
          if (h->next->next->Typ()!=POLY_CMD)
          {
	      Warn("Wrong types for poly= comb(ideal,poly)");
          }
        }
        res->rtyp=POLY_CMD;
        res->data=(void *)fglmNewLinearCombination((ideal)h->next->Data(),(poly)h->next->next->Data());
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
#endif
/*==================== divcount & mcount =============================*/
#ifdef MONOM_COUNT
    if (strcmp((char*)(h->Data()),"mcount")==0)
    {
      extern void OutputMonomCount();
      OutputMonomCount();
      return FALSE;
    }
    else if (strcmp((char*)(h->Data()),"mreset")==0)
    {
      extern void ResetMonomCount();
      ResetMonomCount();
      return FALSE;
    }
    else
#endif
#ifdef DIV_COUNT
    if (strcmp((char*)(h->Data()),"dcount")==0)
    {
      extern void OutputDivCount();
      OutputDivCount();
      return FALSE;
    }
    else if (strcmp((char*)(h->Data()),"dreset")==0)
    {
      extern void ResetDivCount();
      ResetDivCount();
      return FALSE;
    }
    else
#endif
/*==================== barstep =============================*/
    if(strcmp((char*)(h->Data()),"barstep")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==MATRIX_CMD))
      {
        if (h->next->next!=NULL)
        {
          if (h->next->next->Typ()!=POLY_CMD)
          {
            Warn("Wrong types for barstep(matrix,poly)");
          }
        }
        int r,c;
        poly div=(poly)h->next->next->Data();
        res->rtyp=MATRIX_CMD;
        res->data=(void *)mpOneStepBareiss((matrix)h->next->Data(),
                                           &div,&r,&c);
        Print("div: ");pWrite(div);
        Print("rows: %d, cols: %d\n",r,c);
        pDelete(&div);
        return FALSE;
      }
      else
        WerrorS("matrix expected");
    }
    else
#ifdef FACTORY_GCD_TEST
/*=======================gcd Testerei ================================*/
    if ( ! strcmp( (char*)(h->Data()), "setgcd" ) ) {
	if ( (h->next != NULL) && (h->next->Typ() == INT_CMD) ) {
	    CFPrimitiveGcdUtil::setAlgorithm( (int)h->next->Data() );
	    return FALSE;
	} else
	    WerrorS("int expected");
    }
    else
#endif

#ifdef FACTORY_GCD_TIMING
    if ( ! strcmp( (char*)(h->Data()), "gcdtime" ) ) {
	TIMING_PRINT( contentTimer, "time used for content: " );
	TIMING_PRINT( algContentTimer, "time used for algContent: " );
	TIMING_PRINT( algLcmTimer, "time used for algLcm: " );
	TIMING_RESET( contentTimer );
	TIMING_RESET( algContentTimer );
	TIMING_RESET( algLcmTimer );
	return FALSE;
    }
    else
#endif
      
#ifdef FACTORY_GCD_STAT
    if ( ! strcmp( (char*)(h->Data()), "gcdstat" ) ) {
	printGcdTotal();
	printContTotal();
	resetGcdTotal();
	resetContTotal();
	return FALSE;
    }
    else
#endif
      
/*============================================================*/
      WerrorS( feNotImplemented );
  }
  return TRUE;
}
