/*******************************************************************
 *  File:    omalloc.c
 *  Purpose: implementation of ANSI-C conforming malloc functions
 *           -- the real version
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifndef OMALLOC_C
#define OMALLOC_C

#include "omalloc.h"

#ifdef OM_MALLOC_MARK_AS_STATIC
#define OM_MARK_AS_STATIC(addr) omMarkAsStaticAddr(addr)
#else
#define OM_MARK_AS_STATIC(addr) do {} while (0)
#endif

#if OM_PROVIDE_MALLOC > 0

void* calloc(size_t nmemb, size_t size)
{
  void* addr;
  if (size == 0) size = 1;
  if (nmemb == 0) nmemb = 1;

  size = size*nmemb;
  omTypeAlloc0Aligned(void*, addr, size);
  OM_MARK_AS_STATIC(addr);
  return addr;
}

void free(void* addr)
{
  omfree(addr);
}

void* valloc(size_t size)
{
  fputs("omalloc Warning: valloc not yet implemented\n",stderr);
  fflush(NULL);
  return NULL;
}

void* realloc(void* old_addr, size_t new_size)
{
  if (old_addr && new_size)
  {
    void* new_addr;
    omTypeReallocAligned(old_addr, void*, new_addr, new_size);
    OM_MARK_AS_STATIC(new_addr);
    return new_addr;
  }
  else
  {
    free(old_addr);
    return malloc(new_size);
  }
}

/* on some systems strdup is a macro -- replace it unless OMALLOC_FUNC
   is defined */
#ifndef OMALLOC_USES_MALLOC
#if !defined(OMALLOC_FUNC)
#undef strdup
#endif
char* strdup_(const char* addr)
{
  char* n_s;
  if (addr)
  {
    n_s = omStrDup(addr);
    OM_MARK_AS_STATIC(n_s);
    return n_s;
  }
  return NULL;
}
#endif
#endif

void* malloc(size_t size)
{
  void* addr;
  if (size == 0) size = 1;

  omTypeAllocAligned(void*, addr, size);
  OM_MARK_AS_STATIC(addr);
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
    OM_MARK_AS_STATIC(new_addr);
    return new_addr;
  }
  else
  {
    freeSize(old_addr, old_size);
    return malloc(new_size);
  }
}
#endif
