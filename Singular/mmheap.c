/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/mmheap.c,v 1.1.1.1 1997-03-19 13:18:58 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <stdlib.h>
#include "mod2.h"
#include "febase.h"
#include "mmemory.h"
#include "mmprivat.h"

static char * mm_theHeapPtr = NULL;
static char * mm_tmpHeapPtr = NULL;
static char * mm_normHeapPtr = NULL;

static int mm_theBytesLeft = 0;
static int mm_tmpBytesLeft = 0;
static int mm_normBytesLeft = 0;

static status_t mm_heapStatus = MM_NORMAL;

static void mmDistributeRestOfHeap( void );

void* mmGetMemory( size_t size )
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

BOOLEAN mmPutMemory( void *adr, size_t size )
{
  void * dummy=(void *)((char *)adr+size);

  if ( dummy == mm_theHeapPtr)
  {
    mm_theHeapPtr=(char *)adr;
    mm_theBytesLeft += size;
    return TRUE;
  }
  return FALSE;
}

#ifndef MDEBUG

static void mmDistributeRestOfHeap( void )
{
/*
*  char* dummy;
*  int j;
*
*  while ( mm_theBytesLeft > RealSizeFromSize( mmGetSize( 0 ) ) ) {
*    j = mmGetIndex( SizeFromRealSize( mm_theBytesLeft ) );
*    if ( RealSizeFromSize( mmGetSize( j ) ) > mm_theBytesLeft ) j--;
*    dummy = mm_theHeapPtr;
*    mm_theHeapPtr += RealSizeFromSize( mmGetSize( j ) );
*    mm_theBytesLeft -= RealSizeFromSize( mmGetSize( j ) );
*    mmFreeBlock( &(dummy[DebugOffsetFront]), mmGetSize( j ) );
*  }
*/
}

#else /* MDEBUG */

static void mmDistributeRestOfHeap( void )
{
/*
*  void* dummy;
*  int j;
*
*  while ( mm_theBytesLeft > RealSizeFromSize( mmGetSize( 0 ) ) ) {
*    j = mmGetIndex( SizeFromRealSize( mm_theBytesLeft ) );
*    if ( RealSizeFromSize( mmGetSize( j ) ) > mm_theBytesLeft ) j--;
*    dummy = mmAllocBlock( mmGetSize( j ) );
*    mmFreeBlock( &dummy, mmGetSize( j ) );
*  }
*/
}

#endif /* MDEBUG */
