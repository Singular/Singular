/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: sing_dld.cc,v 1.1.1.1 2003-10-06 12:16:03 Singular Exp $ */

/*
* ABSTRACT: interface to dynamic loading
*/

#include "mod2.h"
#include "febase.h"
#include "structs.h"
#include "ipshell.h"


#ifdef HAVE_DLD
#ifdef linux
  #ifdef __ELF__
  #include <dlfcn.h>
  #else
  #include <dld.h>
  #endif
#elif defined(hpux)
#include <dl.h>
#endif

#ifdef linux
#ifndef __ELF__
static int dlInitError=0;
void dlInit(const char *thisfile)
{
  int i=dld_init (thisfile);
  if (i!=0)
  {
    //Print("dld_int:%d, errno:%d\n",i,dld_errno);
    i=dld_init (dld_find_executable(thisfile));
    //Print("dld_int(dld_find_executable):%d, errno:%d\n",i,dld_errno);
    dlInitError=i;
  }
}
#endif
#endif

BOOLEAN dlLoad(const char * fn, const char *pn)
{
  typedef void (*PROC)(void);
  PROC p1;
  #ifdef linux
  #ifdef __ELF__
  /*======================linux elf dl============================*/
  void *h=dlopen(fn,RTD_GLOBAL|RTD_NOW);
  if (h!=NULL)
  {
    p1=(PROC)dlsym(h,pn);
    char *s;
    if((s=dlerror())==NULL)
    {
      p1();
      return FALSE;
    }
    else
    {
      Werror("cannot find %s in %s(%s)",pn,fn,s);
      dlclose(h);
    }
  }
  else
  {
    Werror("cannot link %s (%s)",fn,dlerror());
  }
  #else
  /*======================linux dld============================*/
  if (dlInitError)
  {
    Werror("dld init error %d",dlInitError);
  }
  else
  {
    dld_errno=0;
    int i= dld_link (fn);
    if (dld_function_executable_p(pn))
    {
      p1 = (PROC) dld_get_func (pn);
      p1();
      return FALSE;
    }
    else
    {
      Werror("cannot find %s in %s",pn,fn);
      dld_unlink_by_file (fn,0);
    }
  }
  #endif
  /*======================hpux dl============================*/
  #elif defined(hpux)
  shl_t hdl=shl_load(fn,
          BIND_IMMEDIATE|BIND_NOSTART|BIND_NONFATAL|BIND_VERBOSE,0L);
  int i=shl_findsym(&hdl,pn,TYPE_PROCEDURE,&p1);
  if (i==0)
  {
    p1();
    return FALSE;
  }
  else
  {
    Werror("cannot find %s in %s",pn,fn);
    shl_unload(hdl);
  }
  /*=================systems without dynamic loading================*/
  #else
  WerrorS("dynamic loading not implemented");
  #endif
  return TRUE;
}
sleftv * iiMake_binary(idhdl pn, sleftv* sl)
{
  return NULL;
}
#endif
