/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

#ifndef INCL_MMPRIVATE_H
#define INCL_MMPRIVATE_H

#include <config.h>

#define MAXDATA 4000
#ifdef __alpha__
#define MAXLIST 18
#else
#define MAXLIST 21
#endif

typedef void ** mcb;

#ifdef MDEBUG

typedef struct DBMCB
{
  struct DBMCB * next;
  struct DBMCB * prev;
  size_t size;
  int lineno;
  char * fname;
  int flags;
  int pattern;
  mcb data;
} DBMCB;

typedef DBMCB * pDBMCB;

extern DBMCB mm_theDBused;
extern DBMCB mm_theDBfree;
extern DBMCB mm_tmpDBused;
extern DBMCB mm_tmpDBfree;
extern DBMCB mm_normDBused;
extern DBMCB mm_normDBfree;

#endif /* MDEBUG */


extern /*const*/ size_t mm_mcbSizes [MAXLIST];

typedef enum { MM_NORMAL, MM_TMP, MM_SWITCHED } status_t;

extern mcb *mm_theList;
extern mcb *mm_tmpList;
extern mcb *mm_normList;

extern status_t mm_status;

extern int mm_bytesUsed;
extern int mm_bytesReal;
extern int mm_bytesTmp;
extern int mm_bytesNorm;

extern size_t mm_specSize;
extern int mm_specIndex;

extern int mm_printMark;

#define mmGetSpecSize() mm_specSize
#define mmGetSpecIndex() mm_specIndex

#define MM_TMPFLAG  1
#define MM_NORMFLAG 2
#define MM_FREEFLAG 4
#define MM_USEDFLAG 8

#define PATTERN -2
#define PATTERN2 'A'
#define PATTERN3 'Z'

/* stuff to handle index and sizes of free lists */

int mmGetIndex( size_t );

#define mmGetSize( index ) mm_mcbSizes[index]

#ifndef MDEBUG

#define DebugOffsetFront 0

#define DebugOffsetBack 0

#else /* MDEBUG */

#define DebugOffsetFront 28

#define DebugOffsetBack 4

#endif /* MDEBUG */

#define SizeFromRealSize( size ) (size - DebugOffsetFront - DebugOffsetBack)
#define RealSizeFromSize( size ) (size + DebugOffsetFront + DebugOffsetBack)

mcb mmNewMCBList( int i );

/* stuff for heap handling */

void mmInitHeap( void );

void mmMarkHeap( void );
void mmSweepHeap( void );
void mmSwitchHeap( void );

void mmCheckPrint( void );
void mmNewPrintMark( void );

void * mmGetMemory( size_t size );
int mmPutMemory( void *adr, size_t size );


/* stuff for block handling */

void mmInitBlocks( void );

void mmMarkBlocks( void );
void mmSweepBlocks( void );
void mmSwitchBlocks( void );

void * mmGetBlock( void );


/* since free functions get a (void*) that is in real life a (void**) */

#define r(adr) (*(void**)adr)

#ifdef MDEBUG

extern void * mm_maxAddr;
extern void * mm_minAddr;

void mmMoveDBMCB ( pDBMCB from, pDBMCB to, pDBMCB what );

void mmMoveDBMCBInto ( pDBMCB to, pDBMCB what );

void mmTakeOutDBMCB ( pDBMCB from, pDBMCB what );

int mmCheckDBMCB ( DBMCB * what, int flags, int size );

int mmPrintDBMCB ( DBMCB * what, char* msg , int given_size );

void mmPrintFL( const char* fname, const int lineno );

#endif /* MDEBUG */

#endif /* ! INCL_MMPRIVATE_H */
