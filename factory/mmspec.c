/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "memman.h"
#include "mmprivate.h"

void
mmMark( void )
{
    if ( mm_status != MM_NORMAL )
	fprintf( stderr, "can only mark memory in status normal" );
    else {
	mm_status = MM_TMP;
	mm_normList = mm_theList;
	mm_theList = mm_tmpList;
	mm_bytesNorm = mm_bytesUsed;
	mm_bytesUsed = 0;
#ifdef MDEBUG
	mm_normDBused = mm_theDBused;
	mm_normDBfree = mm_theDBfree;
	memset(&mm_theDBused,0,sizeof(mm_theDBused));
	memset(&mm_theDBfree,0,sizeof(mm_theDBfree));
#endif /* MDEBUG */
	mmMarkHeap(); mmMarkBlocks();
    }
}

void
mmSweep( void )
{
    int i;

    if ( mm_status != MM_TMP )
	fprintf( stderr, "can only sweep memory in status tmp" );
    else {
	mm_status = MM_NORMAL;
	mm_theList = mm_normList;
	mm_bytesUsed = mm_bytesNorm;
	for ( i = 0; i < MAXLIST; i++ )
	    mm_tmpList[i] = NULL;
#ifdef MDEBUG
	mm_theDBused = mm_normDBused;
	mm_theDBfree = mm_normDBfree;
#endif /* MDEBUG */
	mmSweepHeap(); mmSweepBlocks();
    }
}

void
mmSwitch( void )
{
    if ( mm_status == MM_NORMAL )
	fprintf( stderr, "can not switch from state normal" );
    else {
	if ( mm_status == MM_SWITCHED ) {
	    mm_normList = mm_theList;
	    mm_theList = mm_tmpList;
	    mm_status = MM_TMP;
	    mm_bytesNorm = mm_bytesUsed;
	    mm_bytesUsed = mm_bytesTmp;
#ifdef MDEBUG
	    mm_normDBfree = mm_theDBfree;
	    mm_normDBused = mm_theDBused;
	    mm_theDBfree = mm_tmpDBfree;
	    mm_theDBused = mm_tmpDBused;
#endif /* MDEBUG */
	}
	else {
	    mm_tmpList = mm_theList;
	    mm_theList = mm_normList;
	    mm_status = MM_SWITCHED;
	    mm_bytesTmp = mm_bytesUsed;
	    mm_bytesUsed = mm_bytesNorm;
#ifdef MDEBUG
	    mm_tmpDBfree = mm_theDBfree;
	    mm_tmpDBused = mm_theDBused;
	    mm_theDBfree = mm_normDBfree;
	    mm_theDBused = mm_normDBused;
#endif /* MDEBUG */
	}
	mmSwitchHeap(); mmSwitchBlocks();
    }
}

#ifndef MDEBUG
char *
mmStrdup( char * s )
{
    char * rc;
    if (s==NULL) return NULL;
    rc = (char*)mmAlloc( 1 + strlen( s ) );
    strcpy( rc, s );
    return rc;
}
#else
char *
mmDBStrdup( char * s, char *fname, int lineno)
{
    char * rc;
    if (s==NULL) return NULL;
    rc = (char*)mmDBAlloc( 1 + strlen( s ),fname,lineno );
    strcpy( rc, s );
    return rc;
}
#endif

int
mmMemReal( void )
{
    return mm_bytesReal;
}

int
mmMemUsed( void )
{
    if ( mm_status == MM_TMP )
	return mm_bytesUsed + mm_bytesNorm;
    else if ( mm_status == MM_SWITCHED )
	return mm_bytesUsed + mm_bytesTmp;
    else
	return mm_bytesUsed;
}

int mmOLdPrintMark=0;

void
mmCheckPrint( void )
{
    if ( mm_bytesReal > mm_printMark ) {
	int i=(mm_bytesReal+1023)/1024;
	if (i!=mmOLdPrintMark) {
	    fprintf( stdout, "[%dk]", i );
	    fflush( stdout );
	    mmOLdPrintMark=i;
	}
	mmNewPrintMark();
    }
}

void
mmNewPrintMark( void )
{
    mm_printMark = (mm_bytesReal / (100*1024) + 1) * 1024 * 100;
}
