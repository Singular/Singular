/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "memman.h"
#include "mmprivate.h"

typedef struct memBlock
{
    struct memBlock *next;
    char data[MAXDATA];
} memBlock;


static memBlock * mm_theBlocks = NULL;
static memBlock * mm_tmpBlocks = NULL;
static memBlock * mm_normBlocks = NULL;

static status_t mm_blockStatus = MM_NORMAL;

void
mmInitBlocks( void )
{
    mm_theBlocks = mm_tmpBlocks = mm_normBlocks = NULL;
    mm_blockStatus = MM_NORMAL;
}

void
mmMarkBlocks( void )
{
    mm_blockStatus = MM_TMP;
    mm_normBlocks = mm_theBlocks;
    mm_theBlocks = NULL;
}

void
mmSweepBlocks( void )
{
    memBlock * dummy;

    while ( mm_tmpBlocks != NULL ) {
	dummy = mm_tmpBlocks;
	mm_tmpBlocks = mm_tmpBlocks->next;
	free( dummy );
	mm_bytesReal -= sizeof( memBlock );
    }
    mmNewPrintMark();
    mm_tmpBlocks = NULL;
    mm_theBlocks = mm_normBlocks;
    mm_blockStatus = MM_NORMAL;
}

void
mmSwitchBlocks( void )
{
    if ( mm_blockStatus == MM_TMP ) {
	mm_blockStatus = MM_SWITCHED;
	mm_tmpBlocks = mm_theBlocks;
	mm_theBlocks = mm_normBlocks;
    }
    else {
	mm_blockStatus = MM_TMP;
	mm_normBlocks = mm_theBlocks;
	mm_theBlocks = mm_tmpBlocks;
    }
}

void *
mmGetBlock( void )
{
    memBlock * dummy = (memBlock*)malloc( sizeof( memBlock ) );

    if ( dummy == NULL ) {
	(void)fprintf( stderr, "\nSingular-Factory error: no more memory\n" );
	exit(14);
    }
    dummy->next = mm_theBlocks;
    mm_theBlocks = dummy;
    mm_bytesReal += sizeof( memBlock );
    return &dummy->data[0];
}
