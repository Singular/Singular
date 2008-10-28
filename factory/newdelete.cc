/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: newdelete.cc,v 1.4 2008-10-28 08:46:23 Singular Exp $ */

#include <config.h>
#include <new>

#include "assert.h"

#ifdef USE_OLD_MEMMAN
#include "memutil.h"
#else
#include "memman.h"
#endif

// The C++ standard has ratified a change to the new operator.
//
//  T *p = new T;
//
// Previously, if the call to new above failed, a null pointer would've been returned.
// Under the ISO C++ Standard, an exception of type std::bad_alloc is thrown.
// It is possible to suppress this behaviour in favour of the old style
// by using the nothrow version.
//
//  T *p = new (std::nothrow) T;
//
// So we have to overload this new also, just to be sure.
//
// A further interesting question is, if you don't have enough resources
// to allocate a request for memory,
// do you expect to have enough to be able to deal with it?
// Most operating systems will have slowed to be unusable
// long before the exception gets thrown.

#ifdef USE_OLD_MEMMAN

void * operator new ( size_t size )
{
    return getBlock( size );
}

void operator delete ( void * block )
{
    freeBlock( block, 0 );
}

void * operator new[] ( size_t size )
{
    return getBlock( size );
}

void operator delete[] ( void * block )
{
    freeBlock( block, 0 );
}

void * operator new(size_t size, std::nothrow_t) throw()
{
    return getBlock( size );
}
void * operator new[](size_t size, std::nothrow_t) throw()
{
    return getBlock( size );
}

#else

void * operator new ( size_t size )
{
    return mmAlloc( size );
}

void operator delete ( void * block )
{
    mmFree( block );
}

void * operator new[] ( size_t size )
{
    return mmAlloc( size );
}

void operator delete[] ( void * block )
{
    mmFree( block );
}

void * operator new(size_t size, const std::nothrow_t&) throw()
{
    return mmAlloc( size );
}
void * operator new[](size_t size, const std::nothrow_t&) throw()
{
    return mmAlloc( size );
}

#endif /* USE_OLD_MEMMAN */
