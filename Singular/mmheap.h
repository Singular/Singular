#ifndef MEMHEAP_H
#define MEMHEAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.h,v 1.3 1998-12-02 14:04:03 obachman Exp $ */
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
struct sip_memHeap
{
  void* current;
  void* pages;
  size_t size;
};

/*****************************************************************
 *
 * Basic routines
 *
 *****************************************************************/
  
/* Initializes Heap, assumes size < SIZE_OF_HEAP_PAGE */
extern void mmInitHeap(memHeap heap, size_t size);
/* UNCONDITIONALLY frees all pages of heap */
extern void mmClearHeap(memHeap heap);
/* Tries to free as many unused pages of heap as possible */
extern void mmCleanHeap(memHeap heap);
/* creates and initializes heap */
extern memHeap mmCreateHeap(size_t size);
/* Clears and destroys heap */
void mmDestroyHeap(memHeap *heap);
/* Merges what is free in Heap "what" into free list of heap "into" */
void mmMergeHeap(memHeap into, memHeap what);

#define MM_HEAP_ADDR_UNKNOWN_FLAG 0  
#define MM_HEAP_ADDR_USED_FLAG   1
#define MM_HEAP_ADDR_FREE_FLAG   2

#ifndef HEAP_DEBUG

#define mmAllocHeap               _mmAllocHeap
#define mmFreeHeap                _mmFreeHeap

#define mmCheckHeap(heap)           1
#define mmCheckHeapAddr(addr, heap) 1
  
#else

/* use this variables to control level of HEAP_DEBUG at run-time
   (see mod2.h for details) */
extern int mm_HEAP_DEBUG;
  
#define mmAllocHeap(res, heap)\
  ((void*)(res)) = mmDebugAllocHeap(heap, __FILE__, __LINE__)
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
  
/* Allocates memory block from a heap */
#define _mmAllocHeap(what, heap)                            \
do                                                          \
{                                                           \
  if ((heap)->current == NULL) mmAllocNewHeapPage(heap);    \
  ((void*) (what)) = (heap)->current;                       \
  (heap)->current =  *((void**)(heap)->current);            \
}                                                           \
while (0)

/* Frees addr into heap, assumes  addr was previously allocated from heap */ 
#define _mmFreeHeap(addr, heap)                  \
do                                              \
{                                               \
  *((void**) addr) = (heap)->current;           \
  (heap)->current = (void*) addr;               \
}                                               \
while (0)

/*****************************************************************
 *
 * Auxillary routines
 *
 *****************************************************************/
void mmAllocNewHeapPage(memHeap heap);

#if 0
#define mmGetNumberOfAllocatedHeapPages(heap) ((heap)->pnumber)
#define mmGetNumberOfAllocatedHeapMemory(heap) ((heap)->pnumber * SIZE_OF_PAGE)
#endif
#define mmGetHeapBlockSize(heap) ((heap)->size)

#define SIZE_OF_HEAP_PAGE (SIZE_OF_PAGE - SIZEOF_VOIDP)
#define SIZE_OF_HEAP_PAGE_HEADER (SIZEOF_VOIDP)

#ifdef __cplusplus
}
#endif

#endif /* MEMHEAP_H */

