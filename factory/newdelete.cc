// emacs edit mode for this file is -*- C++ -*-
// $Id: newdelete.cc,v 1.0 1996-05-17 12:21:32 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "conf.h"
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
