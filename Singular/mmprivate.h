#ifndef MM_PRIVAT_H
#define MM_PRIVAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmprivate.h,v 1.13 1999-10-14 14:27:21 obachman Exp $ */
/*
* ABSTRACT
*/
#include <stdlib.h>
#include "mod2.h"
#include "structs.h"

#if defined(HAVE_ASO) && HAVE_ASO == 1
#include "mmprivate.aso"
#endif

extern const char mm_IndexTable[];
extern const size_t mm_mcbSizes[];
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
  char * allocated_fname;
  char * freed_fname;
#ifdef MTRACK
  unsigned long bt_allocated_stack[BT_MAXSTACK];
#ifdef MTRACK_FREE
  unsigned long bt_freed_stack[BT_MAXSTACK];
#endif
#endif
  size_t size;
  int allocated_lineno;
  int freed_lineno;
  int flags;
  memHeap heap;
  char front_pattern[MM_NUMBER_OF_FRONT_PATTERNS];
  void* data;
} DBMCB;

typedef DBMCB * pDBMCB;

#define DebugOffsetFront (sizeof(DBMCB) - sizeof(void*))
#define DebugOffsetBack SIZEOF_VOIDP

#define SizeFromRealSize( size ) (size - DebugOffsetFront - DebugOffsetBack)
#define RealSizeFromSize( size ) (size + DebugOffsetFront + DebugOffsetBack)


extern DBMCB mm_theDBused;
extern DBMCB mm_theDBfree;

extern void * mm_maxAddr;
extern void * mm_minAddr;

#define MM_INITFLAG 2
#define MM_FREEFLAG 4
#define MM_USEDFLAG 8
#define MM_CURRENTLY_USEDFLAG 16
#define MM_REFERENCEFLAG 32

void mmPrintFL( const char* fname, const int lineno );
void mmDBInitNewHeapPage(memHeap heap);
int mmCheckDBMCB ( DBMCB * what, int size , int flags);
void mmFillDBMCB(DBMCB* what, size_t size, memHeap heap,
                 int flags, char* fname, int lineno);
void mmDBSetHeapsOfBlocks(memHeap fromheap, memHeap toheap);
void mmTakeOutDBMCB (DBMCB* what );
void mmMoveDBMCB ( pDBMCB from, pDBMCB to, DBMCB * what );
void mmMoveDBMCBInto ( pDBMCB to, pDBMCB what );

#else /* ! MDEBUG */

#define SizeFromRealSize( size ) (size)
#define RealSizeFromSize( size ) (size)

#endif /* MDEBUG */

/* Malloc and Free from/to system */
void* mmMallocFromSystem(size_t size);
void* mmReallocFromSystem(void* addr, size_t new_size, size_t old_size);
void  mmFreeToSystem(void* addr, size_t size);

void* mmAllocPageFromSystem();
void  mmFreePageToSystem(void* page);

extern int mm_bytesMalloc;
extern int mm_bytesValloc;
#ifdef HAVE_SBRK
extern unsigned long mm_SbrkInit;
#endif

void mmCheckPrint( void );

#define MM_PRINT_ALL_ADDR 1
#define MM_PRINT_ALL_STACK 2

/**********************************************************************
 *
 * Low-level heap routines
 *
 **********************************************************************/
struct sip_memHeapPage 
{
  memHeapPage next;
  long counter;
};

/* Change this appropriately, if you change sip_memHeapPage           */
/* However, make sure that sizeof(sip_memHeapPage) is a multiple of 8 */
#define SIZE_OF_HEAP_PAGE_HEADER (SIZEOF_VOIDP + SIZEOF_LONG) 
#define SIZE_OF_HEAP_PAGE (SIZE_OF_PAGE - SIZE_OF_HEAP_PAGE_HEADER)

/* size of max cunk of memory which is treated by "normal" (static) heaps */
#define MAX_BLOCK_SIZE  (((SIZE_OF_HEAP_PAGE) / 16)*4)

/* For Heap whose chunk size is greater than SIZE_OF_MAX_HEAP_CHUNK,
   AllocHeap defaults to Alloc and heap->page is counter of allocted chunks */
#ifndef MDEBUG
#define MAX_HEAP_CHUNK_SIZE MAX_BLOCK_SIZE
#else
#define MAX_HEAP_CHUNK_SIZE RealSizeFromSize(MAX_BLOCK_SIZE)
#endif

#define mmGetHeapPageOfAddr(addr) (memHeapPage) mmGetPageOfAddr(addr)
#define mmIncrHeapPageCounterOfAddr(addr)       \
do                                              \
{                                               \
  register memHeapPage page = mmGetHeapPageOfAddr(addr); \
  (page->counter)++;                            \
}                                               \
while (0)

extern void mmRemoveHeapBlocksOfPage(memHeap heap, memHeapPage hpage);
#define mmDecrCurrentHeapPageCounter(heap)                          \
do                                                                  \
{                                                                   \
  register memHeapPage page = mmGetHeapPageOfAddr((heap)->current); \
  if (--(page->counter) == 0) mmRemoveHeapBlocksOfPage(heap, page); \
}                                                                   \
while (0)

/* Initializes Heap */
extern void mmInitHeap(memHeap heap, size_t size);
/* creates and initializes heap */
extern memHeap mmCreateHeap(size_t size);

#ifndef HEAP_DEBUG
/* UNCONDITIONALLY frees all pages of heap */
extern void mmClearHeap(memHeap heap);
/* UNCONDITIONALLY Clears and destroys heap */
extern void mmDestroyHeap(memHeap *heap);
#else
#define mmClearHeap(h)   mmDebugClearHeap(h, __FILE__, __LINE__)
#define mmDestroyHeap(h) mmDebugDestroyHeap(h, __FILE__, __LINE__)
void mmDebugClearHeap(memHeap heap, const char* fn, int line);
void mmDebugDestroyHeap(memHeap *heap, const char* fn, int line);
#endif

/**********************************************************************
 *
 * Heap Management
 *
 **********************************************************************/
struct sip_memSpecHeap;
typedef struct sip_memSpecHeap ip_memSpecHeap;
typedef struct sip_memSpecHeap* memSpecHeap;

struct sip_memSpecHeap
{
  memSpecHeap       next; /* pointer to next heap */
  memHeap           heap;  /* pointer to heap itself */
  size_t            size;  /* size of heap */
  int               ref;   /* ref count */
};

  
#endif /* MM_PRIVATE_H */

