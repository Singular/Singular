/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdlib.h>

#include "memman.h"
#include "mmprivate.h"

static char * mm_theHeapPtr = NULL;
static char * mm_tmpHeapPtr = NULL;
static char * mm_normHeapPtr = NULL;

static int mm_theBytesLeft = 0;
static int mm_tmpBytesLeft = 0;
static int mm_normBytesLeft = 0;

static status_t mm_heapStatus = MM_NORMAL;

static void mmDistributeRestOfHeap( void );

void
mmInitHeap( void )
{
    mm_theHeapPtr = mm_tmpHeapPtr = mm_normHeapPtr = NULL;
    mm_theBytesLeft = mm_tmpBytesLeft = mm_normBytesLeft = 0;
    mm_heapStatus = MM_NORMAL;
}

void
mmMarkHeap( void )
{
    mm_normHeapPtr = mm_theHeapPtr; mm_normBytesLeft = mm_theBytesLeft;
    mm_theHeapPtr = NULL; mm_theBytesLeft = 0;
    mm_heapStatus = MM_TMP;
}

void
mmSweepHeap( void )
{
    mm_theHeapPtr = mm_normHeapPtr; mm_theBytesLeft = mm_normBytesLeft;
    mm_heapStatus = MM_NORMAL;
}

void
mmSwitchHeap( void )
{
    if ( mm_heapStatus == MM_TMP ) {
	mm_heapStatus = MM_SWITCHED;
	mm_tmpHeapPtr = mm_theHeapPtr; mm_tmpBytesLeft = mm_theBytesLeft;
	mm_theHeapPtr = mm_normHeapPtr; mm_theBytesLeft = mm_normBytesLeft;
    }
    else {
	mm_heapStatus = MM_TMP;
	mm_normHeapPtr = mm_theHeapPtr; mm_normBytesLeft = mm_theBytesLeft;
	mm_theHeapPtr = mm_tmpHeapPtr; mm_theBytesLeft = mm_tmpBytesLeft;
    }
}

void *
mmGetMemory( size_t size )
{
    void * dummy;

    if ( mm_theBytesLeft < size ) {
	mmDistributeRestOfHeap();
	mm_theHeapPtr = (char*)mmGetBlock();
	mm_theBytesLeft = MAXDATA;
    }
    dummy = (void*)mm_theHeapPtr;
    mm_theBytesLeft -= size;
    mm_theHeapPtr += size;
    return dummy;
}

int
mmPutMemory( void *adr, size_t size )
{
    void * dummy=(void *)((char *)adr+size);

    if ( dummy == mm_theHeapPtr) {
	mm_theHeapPtr=(char *)adr;
	mm_theBytesLeft += size;
	return 1; /*TRUE*/
    }
    return 0; /*FALSE*/
}

#ifndef MDEBUG

void
mmDistributeRestOfHeap( void )
{
/*
  char* dummy;
  int j;

  while ( mm_theBytesLeft > RealSizeFromSize( mmGetSize( 0 ) ) ) {
  j = mmGetIndex( SizeFromRealSize( mm_theBytesLeft ) );
  if ( RealSizeFromSize( mmGetSize( j ) ) > mm_theBytesLeft ) j--;
  dummy = mm_theHeapPtr;
  mm_theHeapPtr += RealSizeFromSize( mmGetSize( j ) );
  mm_theBytesLeft -= RealSizeFromSize( mmGetSize( j ) );
  mmFreeBlock( &(dummy[DebugOffsetFront]), mmGetSize( j ) );
  }
  */
}

#else /* MDEBUG */

void
mmDistributeRestOfHeap( void )
{
/*
  void* dummy;
  int j;

  while ( mm_theBytesLeft > RealSizeFromSize( mmGetSize( 0 ) ) ) {
  j = mmGetIndex( SizeFromRealSize( mm_theBytesLeft ) );
  if ( RealSizeFromSize( mmGetSize( j ) ) > mm_theBytesLeft ) j--;
  dummy = mmAllocBlock( mmGetSize( j ) );
  mmFreeBlock( &dummy, mmGetSize( j ) );
  }
  */
}

#endif /* MDEBUG */
