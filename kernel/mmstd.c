/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmstd.c,v 1.2 2004-07-16 08:43:00 Singular Exp $ */
/*
* ABSTRACT: standard version of C-memory management alloc func 
* i.e. (malloc/realloc/free)
*/

#include "mod2.h"

#define OM_NO_MALLOC_MACROS
#ifdef ix86_Win
#define OMALLOC_USES_MALLOC
#endif
#ifdef ppcMac_darwin
#define OMALLOC_USES_MALLOC
#endif
#include "omalloc.h"

// we provide these functions, so that the settings of OM_CHECK
// and OM_TRACK are used, but only provide them if omalloc is not based
// on them
// already provided in libomalloc
#if !defined(OMALLOC_USES_MALLOC) && !defined(X_OMALLOC)

// define this so that all addr allocated there are marked 
// as static, i.e. not metioned by omPrintUsedAddr
#define OM_MALLOC_MARK_AS_STATIC
#define strdup_ strdup__
#include <omalloc.c>
#else
#include <stdlib.h>
void freeSize(void* addr, size_t size)
{
  if (addr) free(addr);
}

void* reallocSize(void* old_addr, size_t old_size, size_t new_size)
{
  if (old_addr && new_size)
  {
   return realloc(old_addr, new_size);
  }
  else 
  {
    freeSize(old_addr, old_size);
    return malloc(new_size);
  }
}
#endif

