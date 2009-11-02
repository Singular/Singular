/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "memman.h"
#include "mmprivate.h"

mcb *mm_theList;
mcb *mm_normList;
mcb *mm_tmpList;

#ifdef MDEBUG

DBMCB mm_theDBused;
DBMCB mm_theDBfree;
DBMCB mm_tmpDBused;
DBMCB mm_tmpDBfree;
DBMCB mm_normDBused;
DBMCB mm_normDBfree;
void * mm_maxAddr;
void * mm_minAddr;

#endif /* MDEBUG */

status_t mm_status;

int mm_bytesUsed;
int mm_bytesReal;
int mm_bytesTmp;
int mm_bytesNorm;

int mm_specIndex;
size_t mm_specSize;

int mm_printMark;

#if 0
void* operator new ( size_t size )
{
#ifdef MDEBUG
    return mmDBAlloc( size, "new operator",0);
#else
    return mmAlloc( size );
#endif
}

void operator delete ( void* block )
{
#ifdef MDEBUG
    mmDBFree( block, "delete operator",0);
#else
    mmFree( block );
#endif
}
#endif /* 0 */

int mmIsInitialized=0;

int
mmInit( void )
{
    int i;

    if ( ! mmIsInitialized ) {
	mm_bytesUsed = 0;
	mm_bytesReal = 0;
	mm_bytesTmp = 0;
	mm_bytesNorm = 0;
	mmNewPrintMark();
	mmInitBlocks();
	mmInitHeap();
	mm_tmpList = (mcb*)malloc( MAXLIST * sizeof( mcb ) );
	mm_normList = (mcb*)malloc( MAXLIST * sizeof( mcb ) );
	for ( i = 0; i < MAXLIST; i++ )
	    mm_tmpList[i] = mm_normList[i] = NULL;
	mm_theList = mm_normList;
	mm_status = MM_NORMAL;
	mm_specIndex = 0;
	mm_specSize = mm_mcbSizes[0];
#ifdef MDEBUG
	memset(&mm_theDBused,0,sizeof(mm_theDBused));
	memset(&mm_theDBfree,0,sizeof(mm_theDBfree));
	memset(&mm_tmpDBused,0,sizeof(mm_tmpDBused));
	memset(&mm_tmpDBfree,0,sizeof(mm_tmpDBfree));
	memset(&mm_normDBused,0,sizeof(mm_normDBused));
	memset(&mm_normDBfree,0,sizeof(mm_normDBfree));
	mm_maxAddr = NULL;
	mm_minAddr = NULL;
#endif /* MDEBUG */
    }
    return 1;
}
