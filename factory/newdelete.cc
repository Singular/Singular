// emacs edit mode for this file is -*- C++ -*-
// $Id: newdelete.cc,v 1.1 1997-04-15 09:33:49 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 12:21:32  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#ifdef USE_OLD_MEMMAN
#include "memutil.h"
#else
#include "memman.h"
#endif

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

#endif /* USE_OLD_MEMMAN */
