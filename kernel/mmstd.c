/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmstd.c,v 1.7 2009-03-18 18:53:28 Singular Exp $ */
/*
* ABSTRACT: standard version of C-memory management alloc func 
* i.e. (malloc/realloc/free)
*/

#include "mod2.h"

#define OM_NO_MALLOC_MACROS
#include "omalloc.h"
#include "../Singular/static.h"

// we provide these functions, so that the settings of OM_CHECK
// and OM_TRACK are used, but only provide them if omalloc is not based
// on them
// already provided in libomalloc
#if defined(HAVE_STATIC)
void* si_malloc(size_t size)
{
  void* addr;
  if (size == 0) size = 1;

  omTypeAllocAligned(void*, addr, size);
  return addr;
}

void freeSize(void* addr, size_t size)
{
  if (addr) omFreeSize(addr, size);
}

void* reallocSize(void* old_addr, size_t old_size, size_t new_size)
{
  if (old_addr && new_size)
  {
   void* new_addr;
    omTypeReallocAlignedSize(old_addr, old_size, void*, new_addr, new_size);
    return new_addr;
  }
  else
  {
    freeSize(old_addr, old_size);
    return si_malloc(new_size);
  }
}
#else
#if !defined(OMALLOC_USES_MALLOC) && !defined(X_OMALLOC)
    /* in mmstd.c, for some architectures freeSize() unconditionally uses the *system* free() */
    /* sage ticket 5344: http://trac.sagemath.org/sage_trac/ticket/5344 */
    /* solution: correctly check OMALLOC_USES_MALLOC from omalloc.h, */
    /* do not rely on the default in Singular as libsingular may be different */

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
#endif

