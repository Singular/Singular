/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/extra.cc,v 1.1.1.1 1997-03-19 13:18:42 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/
/*
* ABSTRACT: general interface to internals of Singular ("system" command)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifndef macintosh
#include <sys/time.h>
#include <unistd.h>
#endif

#include "mod2.h"
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

#ifdef HAVE_LIBFACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapsing.h"
#include "clapconv.h"
#include "kstdfac.h"
#endif

#ifdef HAVE_FGLM
#include "fglm.h"
#endif

#ifdef HAVE_MPSR
extern BOOLEAN mpsr_GetDump(leftv h);
extern BOOLEAN mpsr_PutDump(leftv h);
#endif

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
      res->data=(void *)VERSION;
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
      if (h->next==NULL) system("/bin/sh");
      else system((char*)(h->next->Data()));
      #ifndef MSDOS
      #ifdef HAVE_FEREAD
      fe_temp_set();
      #endif
      #endif
      return FALSE;
    }
    else
    #endif
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
        #elif HAVE_LIBREADLINE
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
#ifdef HAVE_LIBFACTORY
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
#ifdef HAVE_LIBFACTORY
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
#ifdef HAVE_LIBFACTORY
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
/*===================== fglm ========================*/
#ifdef HAVE_FGLM
    if (strcmp((char*)(h->Data()),"fglm")==0)
    {
      if (h->next!=NULL)
      {
        if (h->next->Typ()==RING_CMD)
        {
          if (h->next->next != NULL)
          {
            res->rtyp = IDEAL_CMD;
            res->data= (void *)fglmProc(h->next, h->next->next);
          }
          else WerrorS("Needs two arguments (ring,ideal)\n");
        }
        else WerrorS("First argument has to be a ring\n");
      }
      else WerrorS("Needs two arguments (ring,ideal)\n");
      return FALSE;
    }
    else
#endif
/*========================= dump ===============================*/
#ifdef HAVE_MPSR
    if (strcmp((char*)(h->Data()),"dump")==0)
    {
      res->rtyp = INT_CMD;
      res->data = (void *) mpsr_PutDump(h->next);
      return FALSE;
    }
    else if (strcmp((char*)(h->Data()),"getdump")==0)
    {
      res->rtyp = INT_CMD;
      res->data = (void *) mpsr_GetDump(h->next);
      return FALSE;
    }
    else
#endif      
/*============================================================*/
      WerrorS("not implemented\n");
  }
  return TRUE;
}
