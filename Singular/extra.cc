/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: extra.cc,v 1.12 1997-06-30 17:04:42 obachman Exp $ */
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

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapsing.h"
#include "clapconv.h"
#include "kstdfac.h"
#endif

#include "silink.h"
#include "mpsr.h"

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
      idhdl hh=idroot->get((char*)h->next->Data(),0);
      if ((hh!=NULL)&&(IDTYP(hh)==PROC_CMD))
      {
        res->rtyp=STRING_CMD;
        res->data=mstrdup(iiGetLibName(IDSTRING(hh)));
        if (res->data==NULL) res->data=mstrdup("");
        return FALSE;
      }
      else
        Warn("`%s` not found",(char*)h->next->Data());
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
        char with_str[]=""
        #ifdef DRING
          "DRING "
        #endif
        #ifdef HAVE_DBM
          "DBM "
        #endif
        #ifdef HAVE_DLD
          "DLD "
        #endif
        #ifdef HAVE_GMP
          "gmp "
        #endif
        #ifdef HAVE_FACTORY
          "factory "
        #endif
        #ifdef HAVE_LIBFAC_P
          "libfac "
        #endif
        #ifdef HAVE_MPSR
          "MP "
        #endif
        #ifdef HAVE_READLINE
          "readline "
        #endif
        #ifdef HAVE_TCL
          "tcl "
        #endif
        #ifdef SRING
          "SRING "
        #endif
        ;
        res->rtyp=STRING_CMD;
        char *s=mstrdup(with_str);
        s[strlen(s)-1]='\0';
        res->data=(void *)s;
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
    if (strcmp((char*)(h->Data()),"tty")==0)
    {
        #ifdef HAVE_FEREAD
        #ifdef HAVE_ATEXIT
        fe_reset_input_mode();
        #else
        fe_reset_input_mode(0,NULL);
        #endif
        if ((h->next!=NULL)&&(h->next->Typ()==INT_CMD))
        {
          fe_use_fgets=(int)h->next->Data();
          fe_set_input_mode();
        }
        #elif HAVE_READLINE
        system("stty sane");
        #endif
      return FALSE;
    }
    else
    #endif
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
/*==================== std =============================*/
#if 0
#ifdef HAVE_FACTORY
    if(strcmp((char*)(h->Data()),"std")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        res->rtyp=LIST_CMD;
        res->data=(void *)stdfac((ideal)h->next->Data(),NULL,testHomog,NULL);
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
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
/*==================== min_std =============================*/
#if 0
    if(strcmp((char*)(h->Data()),"min_std")==0)
    {
      if ((h->next!=NULL) &&(h->next->Typ()==IDEAL_CMD))
      {
        res->rtyp=LIST_CMD;
        res->data=(void *)min_std((ideal)h->next->Data(),currQuotient,testHomog,NULL);
        return FALSE;
      }
      else
        WerrorS("ideal expected");
    }
    else
#endif
/*==================== writemat ==================================*/
    if(strcmp((char*)(h->Data()),"writemat")==0)
    {
      if (h->next!=NULL)
      {
        leftv v=h->next;
        if (v->Typ() == STRING_CMD)
        {
          char *filename = (char *)v->Data();
          v = v->next;
          if (v->Typ()==MATRIX_CMD)
          {
            FILE *outfile = fopen(filename,"a");
            if (outfile==NULL)
            {
              Werror("cannot write to file %s",filename);
              return TRUE;
            }
            matrix m=(matrix)v->Data();
            fprintf(outfile,"%d\n%d\n",MATROWS(m),MATCOLS(m));
            char *s = iiStringMatrix(m,2);
            fprintf(outfile,"%s\n",s);
            FreeL((ADDRESS)s);
            fclose(outfile);
            return FALSE;
          }
          else
          {
            WerrorS("matrix expected");
          }
        }
        else
        {
          WerrorS("string expected");
        }
      }
      else
        WerrorS("matrix expected");
    }
    else
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
/*==================== indsetall =============================*/
    if(strcmp((char*)(h->Data()),"ftest")==0)
    {
      MP_Link_pt rlink = (MP_Link_pt) ((si_link) h->next->Data())->data;
//      MP_Link_pt wlink = (MP_Link_pt) ((si_link) h->next->next->Data())->data;
      MPT_Tree_pt tree1 = NULL, tree2;

      MPT_GetExternalData = MPT_DefaultGetExternalData;
      if (MP_InitMsg(rlink) != MP_Success)
      {
        WerrorS("Init failed\n");
        return TRUE;
      }
      
      if (MPT_GetTree(rlink, &tree1) != MPT_Success)
      {
        WerrorS("GetTree failed\n");
        return TRUE;
      }
        
//      MPT_CpyTree(&tree2, tree1);
//      MPT_DeleteTree(tree1);
//      tree2 = tree1;
//      tree2 = MPT_UntypespecTree(tree2);
//      MPT_PutTree(link, tree2);
      MP_Sint32_t ch;
      MPT_Tree_pt var_tree;
      MP_Common_t ordering;
      MPT_IsDDPTree(tree1, &ch, &var_tree, &ordering);
      MPT_CpyTree(&tree2, tree1);
      tree1 = MPT_DDP_2_ExpTree(tree1);
      MPT_DeleteTree(tree1);
      return FALSE;
    }
    else
/*============================================================*/
      WerrorS("not implemented\n");
  }
  return TRUE;
}
