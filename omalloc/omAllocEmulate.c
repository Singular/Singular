/*******************************************************************
 *  File:    omAllocEmulate.c
 *  Purpose: implementation of emulated omalloc routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#include <stdlib.h>
#include <string.h>

#include <omalloc/omMalloc.h>

void* omEmulateAlloc0(size_t size)
{
  void* addr = OM_MALLOC_MALLOC(size);
  memset(addr, 0, size);
  return addr;
}

void* omEmulateRealloc0Size(void* o_addr, size_t o_size, size_t n_size)
{
  void* addr = OM_MALLOC_REALLOC(o_addr, n_size);

  if (n_size > o_size)
    memset((char *)addr + o_size, 0, n_size - o_size);

  return addr;
}

void* omEmulateRealloc0(void* o_addr, size_t n_size)
{
#ifdef OM_MALLOC_SIZEOF_ADDR
  size_t o_size = OM_MALLOC_SIZEOF_ADDR(o_addr);
#endif
  void* addr = OM_MALLOC_REALLOC(o_addr, n_size);
#ifdef OM_MALLOC_SIZEOF_ADDR
  if (n_size > o_size)
    memset((char *)addr + o_size, 0, n_size - o_size);
#endif
  return addr;
}

#if defined(OM_EMULATE_OMALLOC) && defined(OM_PROVIDE_MALLOC)

#undef calloc
#undef malloc
#undef realloc
#undef free

void* calloc(size_t n, size_t s)
{
  return omEmulateAlloc0(n*s);
}

void* malloc(size_t size)
{
  return OM_MALLOC_MALLOC(size);
}

void* realloc(void* o_addr, size_t n_size);
{
  return OM_MALLOC_REALLOC(size);
}

void free(void* addr)
{
  OM_MALLOC_FREE(addr);
}

#endif /* defined(OM_EMULATE_OMALLOC) && defined(OM_PROVIDE_MALLOC) */
