/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: extra.cc,v 1.57 1998-06-13 13:24:26 obachman Exp $ */
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mod2.h"

#ifndef __MWERKS__
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

// Define to enable many more system commands
// #define HAVE_EXTENDED_SYSTEM

#ifdef STDTRACE
//#include "comm.h"
#endif

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapsing.h"
#include "clapconv.h"
#include "kstdfac.h"
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
static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h);


//void emStart();
/*2
*  the "system" command
*/
BOOLEAN jjSYSTEM(leftv res, leftv h)
{
  if(h->Typ() == STRING_CMD)
  {
// ONLY documented system calls go here
// Undocumented system calls go down into #ifdef HAVE_EXTENDED_SYSTEM
/*==================== nblocks ==================================*/
    if (strcmp((char*)(h->Data()), "nblocks") == 0)
    {
      ring r;
      if (h->next == NULL)
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
        if (h->next->Typ() != RING_CMD)
        {
          WerrorS("ring expected");
          return TRUE;
        }
        r = (ring) h->next->Data();
      }
      res->rtyp = INT_CMD;
      res->data = (void*) rBlocks(r) - 1;
      return FALSE;
    }
/*==================== version ==================================*/
    if(strcmp((char*)(h->Data()),"version")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)SINGULAR_VERSION;
      return FALSE;
    }
    else
/*==================== gen ==================================*/
    if(strcmp((char*)(h->Data()),"gen")==0)
    {
      res->rtyp=INT_CMD;
      res->data=(void *)npGen;
      return FALSE;
    }
    else
/*==================== sh ==================================*/
    #ifndef __MWERKS__
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
        #ifdef HAVE_NAMESPACES
          TEST_FOR("Namespaces");
        #endif
        #ifdef HAVE_DYNAMIC_LOADING
          TEST_FOR("DynamicLoading");
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
    #ifndef __MWERKS__
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
    #ifndef __MWERKS__
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
/*==================== Singular ==================================*/
    if (strcmp((char*)(h->data), "Singular") == 0)
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
      else if (h->next != NULL)
      {
        WerrorS("int expected");
        return TRUE;
      }
      res->rtyp=INT_CMD;
      res->data=(void*) siRandomStart;
      return FALSE;
    }
/*==================== neworder =============================*/
// should go below
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
/*==================== contributors =============================*/
   if(strcmp((char*)(h->Data()),"contributors") == 0)
   {
     res->rtyp=STRING_CMD;
     res->data=(void *)
       "Olaf Bachmann, Hubert Grassmann, Kai Krueger, Wolfgang Neumann, Thomas Nuessler, Wilfred Pohl, Thomas Siebert, Ruediger Stobbe, Tim Wichmann";
     return FALSE;
   }
   else
   {
/*================= Extended system call ========================*/
#ifdef HAVE_EXTENDED_SYSTEM
     return(jjEXTENDED_SYSTEM(res, h));
#else
     WerrorS( feNotImplemented );
#endif
   }
  } /* typ==string */
  return TRUE;
}



#ifdef HAVE_EXTENDED_SYSTEM
// You can put your own system calls here
#include "fglmcomb.cc"
#include "fglm.h"
#ifdef HAVE_NEWTON
#include <hc_newton.h>
#endif
static BOOLEAN jjEXTENDED_SYSTEM(leftv res, leftv h)
{
  if(h->Typ() == STRING_CMD)
  {
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
/*==================== alarm ==================================*/
#ifndef __MWERKS__
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
        res->data=(void *)kStdred((ideal)h->next->Data(),NULL,testHomog,NULL);
        setFlag(res,FLAG_STD);
        return FALSE;
      }
      else
        WerrorS("ideal expected");
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
    /*==================== trace =============================*/
#ifdef STDTRACE
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
      }
      else
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
/* ==================== newton ================================*/
#ifdef HAVE_NEWTON
    if(strcmp((char*)(h->Data()),"newton")==0)
    {
      if ((h->next->Typ()!=POLY_CMD)
      || (h->next->next->Typ()!=INT_CMD)
      || (h->next->next->next->Typ()!=INT_CMD))
      {
        WerrorS("system(\"newton\",<poly>,<int>,<int>) expected");
        return TRUE;
      }
      poly  p=(poly)(h->next->Data());
      int l=pLength(p);
      short *points=(short *)Alloc(currRing->N*l*sizeof(short));
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
                (int) (h->next->next->Data()),      // 1: Milnor, 0: Newton
                (int) (h->next->next->next->Data()) // debug
               );
      //----<>---Output-----------------------


//  PrintS("Bin jetzt in extra.cc bei der Auswertung.\n"); // **********
 

      lists L=(lists)Alloc(sizeof(slists));
      L->Init(6);
      L->m[0].rtyp=STRING_CMD;               // newtonnumber;
      L->m[0].data=(void *)mstrdup(r.nZahl);
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

      Free((ADDRESS)points,currRing->N*l*sizeof(short));
      return FALSE;
    }
    else
#endif
/*==================== print all option values =================*/
#ifndef NDEBUG
    if (strcmp((char*)(h->data), "options") == 0)
    {
      void mainOptionValues();
      mainOptionValues();
      return FALSE;
    }
    else
#endif
      WerrorS( feNotImplemented );
  }
  return TRUE;
}
#endif // HAVE_EXTENDED_SYSTEM
/*============================================================*/
