/*******************************************************************
 *  File:    omAllocEmulate.c
 *  Purpose: implementation of emulated omalloc routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include <stdlib.h>
#include <string.h>

#include "omMalloc.h"

#ifdef OM_EMULATE_OMALLOC

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
#endif
