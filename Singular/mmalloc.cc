/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmalloc.cc,v 1.1 2000-08-14 13:11:26 obachman Exp $ */
/*
* ABSTRACT: standard version of memory management alloc func
*/

#include "mod2.h"

/* We define those, so that our values of 
   OM_TRACK and OM_CHECK are used  */

extern "C"
{
// define this so that all addr allocated there are marked 
// as static, i.e. not metioned by omPrintUsedAddr
#define OM_MALLOC_MARK_AS_STATIC
#include <omalloc.c>
}

void* operator new ( size_t size )
{
  void* addr;
  if (!size) size = 1;
  omTypeAlloc(void*, addr, size);
  return addr;
}

void operator delete ( void* block )
{
  omfree( block );
}

void* operator new[] ( size_t size )
{
  void* addr;
  if (! size) size = 1;
  omTypeAlloc(void*, addr, size);
  return addr;
}

void operator delete[] ( void* block )
{
  omfree( block );
}



