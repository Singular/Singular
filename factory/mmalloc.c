/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <string.h>
#include <stdlib.h>

#include "memman.h"
#include "mmprivate.h"

size_t
mmSizeL( void* adr )
{
    if (adr!=NULL) {

	adr = (size_t*)adr-1;
	return *(size_t*)adr;
    }
    return 0;
}

#ifndef MDEBUG

void *
mmAlloc( size_t size )
{
    size_t thesize = size + sizeof( void * );
    size_t * dummy = (size_t*)mmAllocBlock( thesize );
    *dummy = thesize;
    return (void*)(dummy+1);
}

void
mmFree( void* adr )
{
    if (adr!=NULL) {
	adr = (size_t*)adr-1;
	mmFreeBlock( adr, *(size_t*)adr );
    }
}

void *
mmRealloc( void* adr, size_t newsize )
{
    size_t oldsize = *((size_t*)(adr)-1);
    void* newadr = mmAlloc( newsize );
    memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
    mmFree( adr );
    return newadr;
}

#else /* MDEBUG */

void *
mmDBAlloc( size_t size, char* fname, int lineno )
{
    size_t thesize = size + sizeof( void * );
    size_t * dummy = (size_t*)mmDBAllocBlock( thesize, fname, lineno );
    *dummy = thesize;
    return (void*)(dummy+1);
}

void
mmDBFree( void* adr, char* fname, int lineno )
{
    if (adr!=NULL) {
	size_t l;
	adr = (size_t*)adr-1;
#ifdef __alpha__
	l= (*(size_t*)((int)adr&(~7)));
#else
	l= (*(size_t*)((int)adr&(~3)));
#endif
	mmDBFreeBlock( adr,l, fname, lineno );
    }
}

void *
mmDBRealloc( void* adr, size_t newsize, char* fname, int lineno )
{
    size_t oldsize = *((size_t*)(adr)-1);
    void* newadr = mmDBAlloc( newsize, fname, lineno );
    memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
    mmDBFree( adr, fname, lineno );
    return newadr;
}

#endif /* MDEBUG */
