/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mminit.cc,v 1.26 2002-12-19 11:22:17 Singular Exp $ */
/*
* ABSTRACT: init of memory management
*/

#include <stdio.h>
#include <stdlib.h>

#include "mod2.h"
#include "mmalloc.h"
#include "structs.h"
// this prevents the definition of malloc/free and macros
// by omalloc
#define OMALLOC_C
#include "omalloc.h"

#include <si_gmp.h>

int mmIsInitialized=mmInit();

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
#if defined(OMALLOC_USES_MALLOC) || defined(X_OMALLOC)
    mp_set_memory_functions(omMallocFunc,omReallocSizeFunc,omFreeSizeFunc);
#else
    mp_set_memory_functions(malloc,reallocSize,freeSize);
#endif
    om_Opts.OutOfMemoryFunc = omSingOutOfMemoryFunc;
#ifndef OM_NDEBUG
    om_Opts.ErrorHook = dErrorBreak;
#endif
    omInitInfo();
#ifdef OM_SING_KEEP
    om_Opts.Keep = OM_SING_KEEP;
#endif
  }
  mmIsInitialized=1;
  return 1;
}
