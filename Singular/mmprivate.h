#ifndef MEMPRIVAT_H
#define MEMPRIVAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmprivate.h,v 1.8 1999-03-19 17:42:31 obachman Exp $ */
/*
* ABSTRACT
*/
#include "structs.h"
#include "mmheap.h"

#define MAX_BLOCK_SIZE  (((SIZE_OF_HEAP_PAGE) / 16)*4)

#define INDEX_ENTRY_T   char
extern const INDEX_ENTRY_T mm_IndexTable[];
extern const size_t mm_mcbSizes[];
extern struct sip_memHeap mm_theList[];
#define mmGetSize( index ) mm_mcbSizes[index]

#ifndef GENERATE_INDEX_TABLE
#define mmGetIndex(size)                                        \
 (size <= MAX_BLOCK_SIZE ? mm_IndexTable[(size -1) >> 2] : -1)
#else /* GENERATE_INDEX_TABLE */
int mmGetIndex( size_t );
#endif /* ! GENERATE_INDEX_TABLE */

#if defined(MDEBUG) || defined(GENERATE_INDEX_TABLE)

#define MM_NUMBER_OF_FRONT_PATTERNS   4
typedef struct DBMCB
{
  void* _nnext;
  struct DBMCB * next;
  struct DBMCB * prev;
  char * fname;
  unsigned long bt_stack[BT_MAXSTACK];
  int init;
  size_t size;
  int lineno;
  int flags;
  memHeap heap;
  char front_pattern[MM_NUMBER_OF_FRONT_PATTERNS];
  void* data;
} DBMCB;

typedef DBMCB * pDBMCB;

#define DebugOffsetFront (sizeof(DBMCB) - sizeof(void*))
#define DebugOffsetBack 4

#define SizeFromRealSize( size ) (size - DebugOffsetFront - DebugOffsetBack)
#define RealSizeFromSize( size ) (size + DebugOffsetFront + DebugOffsetBack)


extern DBMCB mm_theDBused;
extern DBMCB mm_theDBfree;

extern void * mm_maxAddr;
extern void * mm_minAddr;

#define MM_FREEFLAG 4
#define MM_USEDFLAG 8

void mmPrintFL( const char* fname, const int lineno );
void mmDBInitNewHeapPage(memHeap heap);
int mmCheckDBMCB ( DBMCB * what, int size , int flags);
void mmFillDBMCB(DBMCB* what, size_t size, memHeap heap,
                 int flags, char* fname, int lineno);
void mmDBSetHeapsOfBlocks(memHeap fromheap, memHeap toheap);
void mmTakeOutDBMCB (DBMCB* what );
void mmMoveDBMCB ( pDBMCB from, pDBMCB to, DBMCB * what );
void mmMoveDBMCBInto ( pDBMCB to, pDBMCB what );

#endif /* MDEBUG */

extern int mm_bytesMalloc;
#ifdef HAVE_SBRK
extern unsigned long mm_SbrkInit;
#endif

void mmCheckPrint( void );

#define MM_PRINT_ALL_ADDR 1
#define MM_PRINT_ALL_STACK 2


#endif /* MEMPRIVATE_H */

