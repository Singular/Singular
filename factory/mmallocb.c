/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "memman.h"
#include "mmprivate.h"

#ifndef MDEBUG

void *
mmAllocBlock( size_t size )
{
    int i = mmGetIndex( size );
    mcb result;

    if ( i < 0 ) {
#ifdef MM_COUNT
	mm_bytesUsed += size;
#endif
	mm_bytesReal += size;
	return (void*)malloc( size );
    }
    else {
	size = mmGetSize( i );
#ifdef MM_COUNT
	mm_bytesUsed += size; /*mmGetSize( i );*/
#endif
	if ( ( result = mm_theList[i] ) == NULL ) {
	    return (void*)mmGetMemory( size );
	}
	else {
	    mm_theList[i] = (mcb)*result;
	    return result;
	}
    }
}

void *
mmAllocBlock0( size_t size )
{
    int i = mmGetIndex( size );
    mcb result;

    if ( i < 0 ) {
#ifdef MM_COUNT
	mm_bytesUsed += size;
#endif
	mm_bytesReal += size;
	result=(mcb)malloc( size );
    }
    else {
	size = mmGetSize( i );
#ifdef MM_COUNT
	mm_bytesUsed += size; /*mmGetSize( i );*/
#endif
	if ( ( result = mm_theList[i] ) == NULL ) {
	    result=(mcb)mmGetMemory( size );
	}
	else {
	    mm_theList[i] = (mcb)*result;
	}
    }
    if (result!=NULL) memset(result,0,size);
    return result;
}

void
mmFreeBlock( void* adr, size_t size )
{

    int i;

    if ( ( adr == NULL ) || ( size == 0 ) )
	return;

    i = mmGetIndex( size );

    if ( i < 0 ) {
#ifdef MM_COUNT
	mm_bytesUsed -= size;
#endif
	mm_bytesReal -= size;
	mmNewPrintMark();
	free( adr );
    }
    else {
	*(mcb)adr = mm_theList[i];
	mm_theList[i] = (mcb)adr;
	size = mmGetSize( i );
#ifdef MM_COUNT
	mm_bytesUsed -= size;
#endif
    }
}

void *
mmReallocBlock( void* adr, size_t oldsize, size_t newsize )
{
    int i = mmGetIndex( oldsize );
    int j = mmGetIndex( newsize );
    void* newadr;

    if ( ( i < 0 ) && ( j < 0 ) ) {
#ifdef MM_COUNT
	mm_bytesUsed += ( (int)newsize - (int)oldsize );
#endif
	mm_bytesReal += ( (int)newsize - (int)oldsize );
	return realloc( adr, newsize );
    }
    else if ( i == j )
	return adr;
    else {
	/* since we know i and j this can be done better, this is the quick way */
	newadr = mmAllocBlock( newsize );
	memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
	mmFreeBlock( adr, oldsize );
	return newadr;
    }
}

#else /* MDEBUG */

void *
mmDBAllocBlock( size_t size, char * fname, int lineno )
{
    int i = mmGetIndex( size );
    DBMCB * result;
    char * patptr;

    if ( i < 0 ) {
	int tmpsize=RealSizeFromSize(size);
	if (result = (DBMCB*)malloc( tmpsize )) {
	    mm_bytesUsed += tmpsize;
	    mm_bytesReal += tmpsize;
	    mmMoveDBMCBInto( &mm_theDBused, result );
	}
	else
	    return NULL;
    }
    else {
	if ( mm_theList[i] != NULL ) {
	    result = (DBMCB*)((char*)(mm_theList[i]) - DebugOffsetFront);
	    mm_theList[i] = (mcb)*mm_theList[i];
#ifdef MM_TEST
	    if ( ! mmCheckDBMCB( result, MM_FREEFLAG, mmGetSize( i ) ) ) {
		fprintf( stderr, "alloc list damaged, file %s, line %d", fname, lineno );
		mmTestList();
		exit(12);
	    }
#endif
	    mmMoveDBMCB( &mm_theDBfree, &mm_theDBused, result );
	    mm_bytesUsed += mmGetSize( i );
	}
	else {
	    result = (DBMCB*)mmGetMemory( RealSizeFromSize( mmGetSize( i ) ) );
	    mmMoveDBMCBInto( &mm_theDBused, result );
	    mm_bytesUsed += mmGetSize( i );
	}
    }
    if ( mm_minAddr == NULL ) {
	mm_minAddr = (void*)result;
	mm_maxAddr = (void*)result;
    }
    else if ( (void*)result < mm_minAddr )
	mm_minAddr = (void*)result;
    else if ( (void*)result > mm_maxAddr )
	mm_maxAddr = (void*)result;
    result->size = size;
    result->lineno = lineno;
    result->fname = fname;
    result->pattern = PATTERN;
    if ( mm_status == MM_TMP )
	result->flags = MM_TMPFLAG | MM_USEDFLAG;
    else
	result->flags = MM_NORMFLAG | MM_USEDFLAG;
    /* pattern 2 has to be written at the end of the datablock */
    patptr = ((char*)&result->data) + size;
    for ( i = 4; i > 0; i-- ) *(patptr++) = PATTERN2;
#if MDEBUG != 0
    printf("Alloc %d bytes from %s, l: %d, adr: %p\n",size,fname,lineno,(void*)(&result->data));
#endif
    memset((void*)&result->data,250,size);
    return (void*)&result->data;
}

void *
mmDBAllocBlock0( size_t size, char * fname, int lineno )
{
    int i = mmGetIndex( size );
    DBMCB * result;
    char * patptr;

    if ( i < 0 ) {
	int tmpsize=RealSizeFromSize(size);
	if (result = (DBMCB*)malloc( tmpsize )) {
	    mm_bytesUsed += tmpsize;
	    mm_bytesReal += tmpsize;
	    mmMoveDBMCBInto( &mm_theDBused, result );
	}
	else
	    return NULL;
    }
    else {
	if ( mm_theList[i] != NULL ) {
	    result = (DBMCB*)((char*)(mm_theList[i]) - DebugOffsetFront);
	    mm_theList[i] = (mcb)*mm_theList[i];
#ifdef MM_TEST
	    if ( ! mmCheckDBMCB( result, MM_FREEFLAG, mmGetSize( i ) ) ) {
		fprintf( stderr, "alloc list damaged, file %s, line %d", fname, lineno );
		mmTestList();
		exit(12);
	    }
#endif
	    mmMoveDBMCB( &mm_theDBfree, &mm_theDBused, result );
	    mm_bytesUsed += mmGetSize( i );
	}
	else {
	    result = (DBMCB*)mmGetMemory( RealSizeFromSize( mmGetSize( i ) ) );
	    mmMoveDBMCBInto( &mm_theDBused, result );
	    mm_bytesUsed += mmGetSize( i );
	}
    }
    if ( mm_minAddr == NULL ) {
	mm_minAddr = (void*)result;
	mm_maxAddr = (void*)result;
    }
    else if ( (void*)result < mm_minAddr )
	mm_minAddr = (void*)result;
    else if ( (void*)result > mm_maxAddr )
	mm_maxAddr = (void*)result;
    result->size = size;
    result->lineno = lineno;
    result->fname = fname;
    result->pattern = PATTERN;
    if ( mm_status == MM_TMP )
	result->flags = MM_TMPFLAG | MM_USEDFLAG;
    else
	result->flags = MM_NORMFLAG | MM_USEDFLAG;
    /* pattern 2 has to be written at the end of the datablock */
    patptr = ((char*)&result->data) + size;
    for ( i = 4; i > 0; i-- ) *(patptr++) = PATTERN2;
#if MDEBUG != 0
    printf("Alloc %d bytes from %s, l: %d, adr: %p\n",size,fname,lineno,(void*)(&result->data));
#endif
    memset((void*)&result->data,0,size);
    return (void*)&result->data;
}

void
mmDBFreeBlock( void* adr, size_t size, char * fname, int lineno )
{
    int j, i = mmGetIndex( size );
    char * patptr;
    DBMCB * what;

    if ( ( adr == NULL ) || ( size == 0 ) )
	return;

#if MDEBUG != 0
    printf("Free %d bytes from %s, l: %d, adr: %p\n",size,fname,lineno,(void*)(adr));
#endif
    what = (DBMCB*)((char*)(adr) - DebugOffsetFront);
    if ( mm_status == MM_TMP ) {
	if ( ! mmCheckDBMCB( what, MM_TMPFLAG | MM_USEDFLAG, size ) ) {
	    mmPrintFL( fname, lineno );
	    return;
	}
	what->flags = MM_TMPFLAG | MM_FREEFLAG;
    }
    else {
	if ( ! mmCheckDBMCB( what, MM_NORMFLAG | MM_USEDFLAG, size ) ) {
	    mmPrintFL( fname, lineno );
	    return;
	}
	what->flags = MM_NORMFLAG | MM_FREEFLAG;
    }

    memset(adr,254,size);

    if ( i < 0 ) {
	int tmpsize=RealSizeFromSize(size);
	mm_bytesUsed -= tmpsize;
	mm_bytesReal -= tmpsize;
	mmNewPrintMark();
	mmTakeOutDBMCB( &mm_theDBused, what );
	free( what );
    }
    else {
	what->size = mmGetSize( i );
	what->fname = fname;
	what->lineno = lineno;
	patptr = ((char*)&what->data) + what->size;
	for ( j = 4; j > 0; j-- ) *(patptr++) = PATTERN2;
	what->data = mm_theList[i];
	mm_theList[i] = (mcb)&what->data;
	mmMoveDBMCB( &mm_theDBused, &mm_theDBfree, what );
	mm_bytesUsed -= mmGetSize( i );
    }
}

void *
mmDBReallocBlock( void* adr, size_t oldsize, size_t newsize, char * fname, int lineno )
{
    void* newadr;

    newadr = mmDBAllocBlock( newsize, fname, lineno );
    memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
    mmDBFreeBlock( adr, oldsize, fname, lineno );

    return newadr;
}

#endif /* MDEBUG */
