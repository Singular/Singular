/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memman.h"
#include "mmprivate.h"

#ifdef __alpha__
const size_t mm_mcbSizes [MAXLIST] = {   8,  16,  24,  32,
					 40,  48,  56,  64,
					 80,  96, 112, 128,
					 160, 192, 224, 256,
					 512, 1024 };
#else
/*const*/ size_t mm_mcbSizes [MAXLIST] = {   8,  12,  16,  20,  24,  28,  32,
					     40,  48,  56,  64,
					     80,  96, 112, 128,
					     160, 192, 224, 256,
					     512, 1024 };
#endif

int
mmGetIndex( size_t size )
{
    int i;

    if ( size > mm_mcbSizes[MAXLIST-1] )
	return -1;
    else {
	for ( i = 0; i < MAXLIST; i++ )
	    if ( size <= mm_mcbSizes[i] )
		return i;
    }
    return MAXLIST-1;
}

mcb
mmNewMCBList( int i )
{
    char* blk = (char*)mmGetBlock();
    char* top;
    mcb dummy, p;
    size_t offset = mm_mcbSizes[i];

    dummy = (mcb)blk;
    p = dummy;
    top = blk + MAXDATA - offset;
    while ( (char*)p < top ) {
	*p = (void*)((char*)p + offset);
	p = (mcb)*p;
    }
    *p = NULL;

    return dummy;
}

#ifdef MDEBUG

void
mmMoveDBMCB ( pDBMCB from, pDBMCB to, DBMCB * what )
{
    what->prev->next = what->next;
    if ( what->next != NULL )
	what->next->prev = what->prev;
    what->prev = to;
    what->next = to->next;
    if (to->next!=NULL)
	to->next->prev=what;
    to->next = what;
}

void
mmMoveDBMCBInto ( pDBMCB to, pDBMCB what )
{
    if (to->next !=NULL) {
	what->next = to->next;
	what->next->prev = what;
    }
    to->next = what;
    what->prev = to;
}

void
mmTakeOutDBMCB ( pDBMCB from, pDBMCB what )
{
    what->prev->next = what->next;
    if ( what->next != NULL )
	what->next->prev = what->prev;
}

int
mmPrintDBMCB ( DBMCB * what, char* msg , int given_size)
{
    DBMCB * before=NULL;
    (void)fprintf( stderr, "warning: %s\n", msg );
    (void)fprintf( stderr, "block %x allocated in: %s, line no %d\n",
		   (int)what, what->fname, what->lineno );
    if (strcmp(msg,"size")==0)
	(void)fprintf( stderr, "size is: %d, but free call said %d \n",
		       (int)what->size, given_size );
    else
	(void)fprintf( stderr, "size is: %d\n", (int)what->size );
#ifdef MM_BEFORE
    {
	char * queue="used";
	DBMCB *x=&mm_theDBused;
	while (x!=NULL) {
	    if (((unsigned)x<(unsigned)what) && ((unsigned)before<(unsigned)x))
		before=x;
	    x=x->next;
	}
	x=&mm_theDBfree;
	while (x!=NULL) {
	    if (((unsigned)x<(unsigned)what) && ((unsigned)before<(unsigned)x)) {
		before=x;
		queue="free";
	    }
	    x=x->next;
	}
	x=before;
	fprintf( stderr, "previous (%s queue) at %x to %x, alloc: %s, l: %d, size:%d\n",
		 queue,x,(unsigned)x+RealSizeFromSize(x->size),x->fname,x->lineno,x->size);
    }
#endif
    return 0;
}

void
mmTestList ( )
{
    DBMCB * what=mm_theDBused.next;
    fprintf(stderr,"list of used blocks:\n");
    while (what!=NULL) {
	(void)fprintf( stderr, "%d bytes at %p in: %s, line no %d\n",
		       (int)what->size, what, what->fname, what->lineno);
	what=what->next;
    }
/*
  //what=mm_theDBfree.next;
  //fprintf(stderr,"list of free blocks:\n");
  //while (what!=NULL)
  //{
  //  (void)fprintf( stderr, "%d bytes at %p in: %s, line no %d\n",
  //     what->size, what, what->fname, what->lineno );
  //  what=what->next;
  // }
  */
}

void
mmPrintFL( const char* fname, const int lineno )
{
    (void)fprintf( stderr, "occured in %s line %d\n", fname, lineno );
}

int
mmCheckDBMCB ( DBMCB * what, int flags, int size )
{
    char * patptr;
    int i;

    if ( ((int)what % 4 ) != 0 ) {
	(void)fprintf( stderr, "warning: odd address" );
	return 0;
    }
    if ( (void*)what > mm_maxAddr ) {
	(void)fprintf( stderr, "warning: address too high" );
	return 0;
    }
    if ( (void*)what < mm_minAddr ) {
	(void)fprintf( stderr, "warning: address too low" );
	return 0;
    }
    if ( ( what->flags & flags ) != flags )
	return mmPrintDBMCB( what, "flags", 0 );
    if ( what->size != size )
	return mmPrintDBMCB( what, "size", size );
    if ( what->pattern != PATTERN )
	return mmPrintDBMCB( what, "front pattern", 0 );
    patptr = ((char*)&what->data) + size;
    for ( i = 4; i > 0; i-- )
	if ( *(patptr++) != PATTERN2 ) return mmPrintDBMCB( what, "back pattern", 0 );
    if ( ( what->next != NULL ) && ( what->next->prev != what ) )
	return mmPrintDBMCB( what, "chain:n", 0 );
    if ( ( what->prev != NULL ) && ( what->prev->next != what ) ) {
	mmPrintDBMCB( what->prev, "prev", 0 );
	return mmPrintDBMCB( what, "chain:p", 0 );
    }
    return 1;
}

int
mmDBTestBlock( const void* adr, const size_t size, const char * fname, const int lineno )
{
    DBMCB * what;

    if ( ( adr == NULL ) || ( size == 0 ) )
	return 1; /*TRUE*/

    what = (DBMCB*)((char*)(adr) - DebugOffsetFront);
    if ( mm_status == MM_TMP ) {
	if ( ! mmCheckDBMCB( what, MM_TMPFLAG | MM_USEDFLAG, size ) ) {
	    mmPrintFL( fname, lineno );
	    return 0; /*FALSE*/
	}
    }
    else {
	if ( ! mmCheckDBMCB( what, MM_NORMFLAG | MM_USEDFLAG, size ) ) {
	    mmPrintFL( fname, lineno );
	    return 0; /*FALSE*/
	}
    }
    return 1; /*TRUE*/
}

int
mmDBTest( const void* adr, const char * fname, const int lineno )
{
    if (adr!=NULL) {
	size_t l;
	adr = (size_t*)adr-1;
	l= (adr<=mm_maxAddr) ? (*(size_t*)((int)adr&(~3))) :0;
	return mmDBTestBlock( adr,l, fname, lineno );
    }
    return 1; /*TRUE*/
}

#endif /* MDEBUG */
