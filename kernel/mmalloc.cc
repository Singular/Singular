/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmalloc.cc,v 1.5 2006-07-27 09:09:37 Singular Exp $ */
/*
* ABSTRACT: standard version of C++-memory management alloc func
*/
#include <stdlib.h>

#include "mod2.h"

#include <omalloc.h>

/* We define those, so that our values of
   OM_TRACK and OM_CHECK are used  */
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
