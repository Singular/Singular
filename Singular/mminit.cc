/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mminit.cc,v 1.20 2000-08-14 12:56:39 obachman Exp $ */
/*
* ABSTRACT: init of memory management
*/

#include <stdio.h>
#include <stdlib.h>

#include "mod2.h"
#include "mmalloc.h"
// this prevents the definition of malloc/free ad macros
// by omalloc
#define OMALLOC_C
#include <omalloc.h>

extern "C"
{
#include <gmp.h>
}

static int mmIsInitialized=mmInit();

extern "C"
{
  void omSingOutOfMemoryFunc()
  {
    fprintf(stderr, "\nerror: no more memory\n");
    omPrintStats(stderr);
    m2_end(14);
    /* should never get here */
    exit(1);
  }
}

int mmInit( void )
{
  if(mmIsInitialized==0)
  {
    mp_set_memory_functions(malloc,reallocSize,freeSize);
    om_Opts.OutOfMemoryFunc = omSingOutOfMemoryFunc;
    omInitInfo();
#ifdef OM_SING_KEEP
    om_Opts.Keep = OM_SING_KEEP;
#endif
  }
  mmIsInitialized=1;
  return 1;
}


  



