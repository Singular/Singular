#ifndef MM_HEAP_H
#define MM_HEAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.h,v 1.12 1999-10-14 14:27:20 obachman Exp $ */

#include "mod2.h"
#include "structs.h"
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 *
 * Basic routines, see the beginning of mmheap.c for customizations
 *
 *****************************************************************/
/* Initializes Heap */
extern void mmInitHeap(memHeap heap, size_t size);
/* creates and initializes heap */
extern memHeap mmCreateHeap(size_t size);

#ifndef HEAP_DEBUG
/* UNCONDITIONALLY frees all pages of heap */
extern void mmClearHeap(memHeap heap);
/* UNCONDITIONALLY Clears and destroys heap */
extern void mmDestroyHeap(memHeap *heap);
#endif

/* if all cunks of one page are in free list, then removes these
   chunks from freelist 
   if strict == 0, does it only if current free pointer is different
   from what it was the last time we did a GC
*/
extern void mmGarbageCollectHeap(memHeap heap, int strict);

/* Merges what is free in Heap "what" into free list of heap "into" */
void mmMergeHeap(memHeap into, memHeap what);

/* Removes addr from freelist of heap, provided it finds it there */
void mmRemoveFromCurrentHeap(memHeap heap, void* addr);
  

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
/* 
 * define HEAP_DEBUG  and/or set mm_HEAP_DEBUG to 
 * 0 to enable basic heap addr checks (at least on each alloc/free)
 * 1 for addtl. containment checks in free/alloc list of heap
 * 2 for addtl. check of entire  heap at each heap addr check
 * NOTE: For HEAP_DEBUG > 1 on, it gets very slow
 */
extern int mm_HEAP_DEBUG;
  
#define mmAllocHeap(res, heap)\
  (res) = mmDebugAllocHeap(heap, __FILE__, __LINE__)
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
  

#define mmClearHeap(h)   mmDebugClearHeap(h, __FILE__, __LINE__)
#define mmDestroyHeap(h) mmDebugDestroyHeap(h, __FILE__, __LINE__)
void mmDebugClearHeap(memHeap heap, const char* fn, int line);
void mmDebugDestroyHeap(memHeap *heap, const char* fn, int line);
#endif

/* use this for unknown heaps */
#define MM_UNKNOWN_HEAP ((memHeap) 1)

/*****************************************************************
 *
 * Low-level allocation/ routines and declarations
 *
 *****************************************************************/

struct sip_memHeapPage 
{
  memHeapPage next;
  long counter;
};

/* Change this appropriately, if you change sip_memHeapPage           */
/* However, make sure that sizeof(sip_memHeapPage) is a multiple of 8 */
#define SIZE_OF_HEAP_PAGE_HEADER (SIZEOF_VOIDP + SIZEOF_LONG) 
#define SIZE_OF_HEAP_PAGE (SIZE_OF_PAGE - SIZE_OF_HEAP_PAGE_HEADER)

struct sip_memHeap
{
  void*         current; /* Freelist pointer */
  memHeapPage   pages;   /* Pointer to linked list of pages */
  void*         last_gc; /* current pointer after last gc */
  int           size;    /* Size of heap chunks */
};


extern void mmAllocNewHeapPage(memHeap heap);

/* Allocates memory block from a heap */
#define _mmAllocHeap(what, heap)                            \
do                                                          \
{                                                           \
  register memHeap _heap = heap;                            \
  if ((_heap)->current == NULL) mmAllocNewHeapPage(_heap);  \
  (void*) (what) = (void *)((_heap)->current);              \
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

#define MM_HEAP_ADDR_UNKNOWN_FLAG 0  
#define MM_HEAP_ADDR_USED_FLAG   1
#define MM_HEAP_ADDR_FREE_FLAG   2

#ifdef __cplusplus
}
#endif

#endif /* MM_HEAP_H */

