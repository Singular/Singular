// emacs edit mode for this file is -*- C++ -*-
// $Id: clapmem.cc,v 1.1.1.1 1997-03-19 13:18:44 obachman Exp $

#include "mod2.h"
#ifdef HAVE_LIBFACTORY
#include "mmemory.h"

void *
operator new[] ( size_t size )
{
#ifdef MDEBUG
  return mmDBAlloc( size, "new[]",0 );
#else
  return AllocL( size );
#endif
}

void
operator delete[] ( void * block )
{
#ifdef MDEBUG
  mmDBFree( block,"delete[]",0 );
#else
  FreeL( block );
#endif
}
#endif
