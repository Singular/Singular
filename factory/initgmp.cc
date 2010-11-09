/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include "cf_gmp.h"

#include "assert.h"

#include "cf_defs.h"
#ifdef USE_MEMUTIL
#ifdef USE_OLD_MEMMAN
#include "memutil.h"
#else
#include "memman.h"
#endif
#endif

#ifdef USE_MEMUTIL
#ifndef USE_OLD_MEMMAN
#ifdef MDEBUG
void * mgAllocBlock( size_t t)
{
  return mmDBAllocBlock(t,"gmp",0);
}
void mgFreeBlock( void* a, size_t t)
{
  mmDBFreeBlock(a,t,"gmp",0);
}
void * mgReallocBlock( void* a, size_t t1, size_t t2)
{
  return mmDBReallocBlock(a,t1,t2,"gmp",0);
}
#endif
#endif
#endif

int initializeGMP()
{
#ifdef USE_MEMUTIL
  static int initialized = 0;

  if ( ! initialized )
  {
    initialized = 1;
#ifdef USE_OLD_MEMMAN
    mp_set_memory_functions( getBlock, reallocBlock, freeBlock );
#else
#ifdef MDEBUG
    mp_set_memory_functions( mgAllocBlock, mgReallocBlock, mgFreeBlock );
#else
    // do not initialize this within Singular:
    mp_set_memory_functions( mmAllocBlock, mmReallocBlock, mmFreeBlock );
#endif
#endif
  }
#endif
  return 1;
}
