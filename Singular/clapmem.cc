// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapmem.cc,v 1.3 1997-04-25 15:03:52 obachman Exp $
/*
* ABSTRACT: map new[]/delete[] to Singular memeory management
*/


#include "mod2.h"
#ifdef HAVE_FACTORY
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
