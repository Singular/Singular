#ifndef MEMHEAP_H
#define MEMHEAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.h,v 1.7 1999-06-08 07:34:38 Singular Exp $ */
#include <stdlib.h>
#include "mod2.h"
#include "structs.h"
#include "mmpage.h"


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 *
 * Declaration and Configuration
 *
 *****************************************************************/
/* define to enable automatic "garbage" collection of heaps */
/* #define HAVE_AUTOMATIC_HEAP_COLLECTION */

/*****************************************************************
 *
 * Basic routines
 *
 *****************************************************************/
  
/* Initializes Heap, assumes size < SIZE_OF_HEAP_PAGE */
extern void mmInitHeap(memHeap heap, size_t size);
/* creates and initializes heap */
extern memHeap mmCreateHeap(size_t size);

/* UNCONDITIONALLY frees all pages of heap */
extern void mmClearHeap(memHeap heap);
/* UNCONDITIONALLY Clears and destroys heap */
void mmDestroyHeap(memHeap *heap);

/* Tries to free as many unused pages of heap as possible */
#if defined(HAVE_AUTOMATIC_HEAP_COLLECTION) && defined(HAVE_PAGE_ALIGNMENT)
/* CleanUpHeap does not do anything when automatic heap collection is on */
#define mmCleanHeap(heap) 
extern void _mmCleanHeap(memHeap heap);
#else
extern void mmCleanHeap(memHeap heap);
#endif

/* Merges what is free in Heap "what" into free list of heap "into" */
void mmMergeHeap(memHeap into, memHeap what);

/*****************************************************************
 *
 * Alloc, Free and Check for Heaps
 *
 *****************************************************************/

#ifndef HEAP_DEBUG

#define mmAllocHeap(res, heap)  _mmAllocHeap(res, heap)
#define mmFreeHeap(addr, heap) _mmFreeHeap(addr, heap)

#define mmCheckHeap(heap)           1
#define mmCheckHeapAddr(addr, heap) 1

#else

/* use this variables to control level of HEAP_DEBUG at run-time
   (see mod2.h for details) */
extern int mm_HEAP_DEBUG;
  
#define mmAllocHeap(res, heap)\
  (res) = (void *)mmDebugAllocHeap(heap, __FILE__, __LINE__)
void * mmDebugAllocHeap(memHeap heap, const char*, int );

#define mmFreeHeap(addr, heap)\
  mmDebugFreeHeap(addr, heap, __FILE__, __LINE__)
void   mmDebugFreeHeap(void* addr, memHeap heap, const char*, int );

#define mmCheckHeap(heap)\
  mmDebugCheckHeap(heap, __FILE__, __LINE__)
int mmDebugCheckHeap(memHeap heap, const char* fn, int line);

#define mmCheckHeapAddr(addr, heap) \
  mmDebugCheckHeapAdr(addr, heap, MM_HEAP_ADDR_USED_FLAG, __FILE__, __LINE__)  
int mmDebugCheckHeapAddr(void* addr, memHeap heap, int flag,
                         const char* fn, int l);
  
#endif

/*****************************************************************
 *
 * Low-level allocation routines
 *
 *****************************************************************/

struct sip_memHeapPage;
typedef struct sip_memHeapPage * memHeapPage;

struct sip_memHeapPage 
{
  memHeapPage next;
  long counter;
};

struct sip_memHeap
{
  void*         current;
  memHeapPage   pages;
  size_t        size;
};



#define SIZE_OF_HEAP_PAGE_HEADER (SIZEOF_VOIDP + SIZEOF_LONG) 

#ifdef HAVE_PAGE_ALIGNMENT

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
#endif /* HAVE_PAGE_ALIGNMENT */


#define SIZE_OF_HEAP_PAGE (SIZE_OF_PAGE - SIZE_OF_HEAP_PAGE_HEADER)
#define mmGetHeapBlockSize(heap) ((heap)->size)

extern void mmAllocNewHeapPage(memHeap heap);

#if defined (HAVE_PAGE_ALIGNMENT) && defined(HAVE_AUTOMATIC_HEAP_COLLECTION)
/* Allocates memory block from a heap */
#define _mmAllocHeap(what, heap)                            \
do                                                          \
{                                                           \
  register memHeap _heap = heap;                            \
  if ((_heap)->current == NULL) mmAllocNewHeapPage(_heap);  \
  mmIncrHeapPageCounterOfAddr((_heap)->current);            \
  (what) = (void *)(_heap)->current;                        \
  (_heap)->current =  *((void**)(_heap)->current);          \
}                                                           \
while (0)

/* Frees addr into heap, assumes  addr was previously allocated from heap */ 
#define _mmFreeHeap(addr, heap)                 \
do                                              \
{                                               \
  register memHeap _heap = heap;                \
  *((void**) addr) = (_heap)->current;          \
  (_heap)->current = (void*) addr;              \
  mmDecrCurrentHeapPageCounter(_heap);          \
}                                               \
while (0)

#else 

/* Allocates memory block from a heap */
#define _mmAllocHeap(what, heap)                            \
do                                                          \
{                                                           \
  register memHeap _heap = heap;                            \
  if ((_heap)->current == NULL) mmAllocNewHeapPage(_heap);  \
  ((void*) (what)) = (_heap)->current;                      \
  (_heap)->current =  *((void**)(_heap)->current);          \
}                                                           \
while (0)

/* Frees addr into heap, assumes  addr was previously allocated from heap */ 
#define _mmFreeHeap(addr, heap)                \
do                                              \
{                                               \
  register memHeap _heap = heap;                \
  *((void**) addr) = (_heap)->current;          \
  (_heap)->current = (void*) addr;              \
}                                               \
while (0)

#endif /* defined(HAVE_PAGE_ALIGNMENT) && ... */

#define MM_HEAP_ADDR_UNKNOWN_FLAG 0  
#define MM_HEAP_ADDR_USED_FLAG   1
#define MM_HEAP_ADDR_FREE_FLAG   2

#ifdef __cplusplus
}
#endif

#endif /* MEMHEAP_H */

