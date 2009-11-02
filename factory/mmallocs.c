/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdlib.h>
#include <stdio.h>

#include "memman.h"
#include "mmprivate.h"

#ifndef MDEBUG

void *
mmAllocSpecialized( void )
{
    mcb result;

    if ( mm_theList[mm_specIndex] == NULL )
	return mmAllocBlock( mmGetSize( mm_specIndex ) );
    result = mm_theList[mm_specIndex];
    mm_theList[mm_specIndex] = (mcb)*result;
#ifdef MM_COUNT
    mm_bytesUsed += mmGetSize( mm_specIndex );
#endif
    return (void*)result;
}

void
mmFreeSpecialized( void* adr )
{
    *((mcb)adr) = mm_theList[mm_specIndex];
    mm_theList[mm_specIndex] = (mcb)adr;
#ifdef MM_COUNT
    mm_bytesUsed -= mmGetSize( mm_specIndex );
#endif
}

#else /* MDEBUG */

void *
mmDBAllocSpecialized( char* fname, int lineno )
{
    return mmDBAllocBlock( mm_specSize, fname, lineno );
}

void
mmDBFreeSpecialized( void* adr, char* fname, int lineno )
{
    mmDBFreeBlock( adr, mm_specSize, fname, lineno );
}

#endif /* MDEBUG */


void
mmSpecializeBlock( size_t size )
{
    mm_specIndex = mmGetIndex( size );
    if (mm_specIndex<0) {
	fprintf(stderr,"too many ring variables\nleaving Singular...");
	exit(11);
    }
    mm_specSize = size;
}
