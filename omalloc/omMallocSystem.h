/*******************************************************************
 *  File:    omMallocSystem.h
 *  Purpose: declaration of macros for malloc to be used from the system
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_MALLOC_SYSTEM_H
#define OM_MALLOC_SYSTEM_H

/* this is the minimal set of OM_MALLOC_* macros which must be defined */
#define OM_MALLOC_MALLOC   malloc
#define OM_MALLOC_REALLOC  realloc
#define OM_MALLOC_FREE     free
#define OM_MALLOC_VALLOC   valloc
#define OM_MALLOC_VFREE(addr, size) OM_MALLOC_FREE(addr)
#ifdef HAVE_MALLOC_SIZE
/* the following will work under Mac OS X */
  #ifdef HAVE_MALLOC_H
  #include <malloc.h>
  #elif defined(HAVE_MALLOC_MALLOC_H)
  #include <malloc/malloc.h>
  #endif
  #define OM_MALLOC_SIZEOF_ADDR(addr)  (malloc_size(addr))
#elif (defined(HAVE_MALLOC_USABLE_SIZE))
/* and this will work under Linux */
#include <stdlib.h>
#define OM_MALLOC_SIZEOF_ADDR(addr) (malloc_usable_size(addr))
#else
static inline size_t OM_MALLOC_SIZEOF_ADDR(void *d) { long*dd=(long*)d; return *(dd-1); }
#endif

#endif /* OM_MALLOC_SYSTEM_H */
