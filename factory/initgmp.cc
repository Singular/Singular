/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include "config.h"

#include "cf_assert.h"

#include "cf_gmp.h"

#include "cf_defs.h"

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
