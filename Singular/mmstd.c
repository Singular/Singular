/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmstd.c,v 1.2 2001-01-24 16:54:04 levandov Exp $ */
/*
* ABSTRACT: standard version of C-memory management alloc func
*/

#include "mod2.h"

/* We define those, so that our values of 
   OM_TRACK and OM_CHECK are used  */

// define this so that all addr allocated there are marked 
// as static, i.e. not metioned by omPrintUsedAddr
#ifndef ix86_Win
#define OM_MALLOC_MARK_AS_STATIC
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
