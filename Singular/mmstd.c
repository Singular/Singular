/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: standard version of C-memory management alloc func
* i.e. (malloc/realloc/free)
*/




#include "kernel/mod2.h"

#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

/* we provide these functions, so that the settings of OM_CHECK
* and OM_TRACK are used, but only provide them if omalloc is not based
* on them
* already provided in libomalloc */
#if !defined(OMALLOC_USES_MALLOC) && !defined(X_OMALLOC)

/* in mmstd.c, for some architectures freeSize() unconditionally uses the *system* free() */
/* sage ticket 5344: http://trac.sagemath.org/sage_trac/ticket/5344 */
/* solution: correctly check OMALLOC_USES_MALLOC from omalloc.h, */
/* do not rely on the default in Singular as libsingular may be different */

/* define this so that all addr allocated there are marked
* as static, i.e. not metioned by omPrintUsedAddr*/
#define OM_MALLOC_MARK_AS_STATIC
#define strdup_ strdup__
#include "omalloc/omalloc.c" /// UGLY!!!!!!!!!!!!!!!!

#else
#include "Singular/mmalloc.h"

void freeSize(void* addr, size_t size)
{
  (void) size;
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

