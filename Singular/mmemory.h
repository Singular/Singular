#ifndef MMEMORY_H
#define MMEMORY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmemory.h,v 1.25 1999-10-18 11:19:29 obachman Exp $ */
/*
* ABSTRACT
*/
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#else
#define inline static
#endif

#include "mod2.h"
#include "structs.h"

/**********************************************************************
 *
 * Memory allocation
 *
 **********************************************************************/
#ifndef MDEBUG
void * mmAllocBlock( size_t );
void * mmAllocBlock0( size_t );
void   mmFreeBlock( void*, size_t );
void * mmReallocBlock( void*, size_t, size_t );
void * mmReallocBlock0( void*, size_t, size_t );
void * mmAlloc( size_t );
void * mmRealloc( void*, size_t );
void   mmFree( void* );
char * mmStrdup( const char* );
#if SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP
void * mmAllocAlignedBlock( size_t );
void * mmAllocAlignedBlock0( size_t );
void   mmFreeAlignedBlock( void*, size_t );
#endif
#define FreeHeap                mmFreeHeap
#define Alloc                   mmAllocBlock
#define Alloc0                  mmAllocBlock0
#define Free                    mmFreeBlock
#define ReAlloc                 mmReallocBlock
#define ReAlloc0                mmReallocBlock0
#define FreeL                   mmFree
#define AllocL                  mmAlloc
#define mstrdup                 mmStrdup
#if SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP
#define AllocAligned0           mmAllocAlignedBlock0
#define AllocAligned            mmAllocAlignedBlock
#define FreeAligned             mmFreeAlignedBlock
#elif SIZEOF_DOUBLE == SIZEOF_VOIDP
#define AllocAligned0           mmAllocBlock0
#define AllocAligned            mmAllocBlock
#define FreeAligned             mmFreeBlock
#endif

#else /* MDEBUG */

void * mmDBAllocHeap(memHeap heap, char*, int );
void * mmDBAlloc0Heap(memHeap heap, char*, int );
void   mmDBFreeHeap(void* addr, memHeap heap, char*, int );
void * mmDBAllocBlock( size_t, char*, int );
void * mmDBAllocBlock0( size_t,  char*, int);
void   mmDBFreeBlock( void*, size_t, char*, int);
void * mmDBReallocBlock( void*, size_t, size_t, char*, int );
void * mmDBReallocBlock0( void*, size_t, size_t, char*, int );
void * mmDBAlloc( size_t, char*, int );
void * mmDBRealloc( void*, size_t, char*, int );
void   mmDBFree( void*, char*, int );
char * mmDBStrdup( const char * s, char *fname, int lineno);
#if SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP
void * mmDBAllocAlignedBlock( size_t, char*, int );
void * mmDBAllocAlignedBlock0( size_t,  char*, int);
void   mmDBFreeAlignedBlock( void*, size_t, char*, int );
#endif

#define AllocHeap(heap)         mmDBAllocHeap(heap, __FILE__, __LINE__)
#define Alloc0Heap(heap)        mmDBAlloc0Heap(heap, __FILE__, __LINE__)
#define FreeHeap(addr, heap)    mmDBFreeHeap(addr, heap,  __FILE__, __LINE__)
#define Alloc(s)                mmDBAllocBlock(s, __FILE__, __LINE__)
#define Alloc0(s)               mmDBAllocBlock0(s, __FILE__, __LINE__)
#define Free(a,s)               mmDBFreeBlock(a, s, __FILE__, __LINE__)
#define ReAlloc(a,o,n)          mmDBReallocBlock(a, o, n, __FILE__, __LINE__)
#define ReAlloc0(a,o,n)         mmDBReallocBlock0(a, o, n, __FILE__, __LINE__)
#define AllocL(s)               mmDBAlloc(s, __FILE__, __LINE__)
#define FreeL(a)                mmDBFree(a,__FILE__,__LINE__)
#define mstrdup(s)              mmDBStrdup(s, __FILE__, __LINE__)
#if SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP
#define AllocAligned(s)         mmDBAllocAlignedBlock(s, __FILE__, __LINE__)
#define AllocAligned0(s)        mmDBAllocAlignedBlock0(s, __FILE__, __LINE__)
#define FreeAligned(a,s)        mmDBFreeAlignedBlock(a, s, __FILE__, __LINE__)
#elif SIZEOF_DOUBLE == SIZEOF_VOIDP
#define AllocAligned(s)         mmDBAllocBlock(s, __FILE__, __LINE__)
#define AllocAligned0(s)        mmDBAllocBlock0(s, __FILE__, __LINE__)
#define FreeAligned(a,s)        mmDBFreeBlock(a, s, __FILE__, __LINE__)
#endif

#endif /* MDEBUG */

/**********************************************************************
 *
 * ASO stuff -- enable/disable in mod2.h
 *
 **********************************************************************/
#if defined(HAVE_ASO) && HAVE_ASO == 1

/* definitions of ALLOC_SIZE_OF_## are given in *.aso */

#define AllocSizeOf(x)  ALLOC_SIZE_OF_##x
#define Alloc0SizeOf(x) ALLOC0_SIZE_OF_##x
#define FreeSizeOf(x,y) FREE_SIZE_OF_##y(x)

#if defined(ASO_DEBUG) || defined(MDEBUG)
void* mmDBAllocHeapSizeOf(memHeap heap, size_t size, char* file, int line);
void* mmDBAlloc0HeapSizeOf(memHeap heap, size_t size, char* file, int line);
void  mmDBFreeHeapSizeOf(void* addr, memHeap heap, size_t size, 
                         char* file, int line);
#define AllocHeapSizeOf(h, s) mmDBAllocHeapSizeOf(h, s, __FILE__, __LINE__)
#define Alloc0HeapSizeOf(h, s) mmDBAlloc0HeapSizeOf(h, s, __FILE__, __LINE__)
#define FreeHeapSizeOf(x, h, s) mmDBFreeHeapSizeOf(x, h, s, __FILE__, __LINE__)
#else /* ! (ASO_DEBUG || MDEBUG) */
#define AllocHeapSizeOf(h, s)   AllocHeap(h)
#define Alloc0HeapSizeOf(h, s)  Alloc0Heap(h)
#define FreeHeapSizeOf(x, h, s) FreeHeap(x, h)
#endif /* ASO_DEBUG || MDEBUG */

#else
/* defaults for AllocSizeOf stuff -- should be redefined by *.aso files */
#define AllocSizeOf(x)    Alloc(sizeof(x))
#define Alloc0SizeOf(x)   Alloc0(sizeof(x))
#define FreeSizeOf(x, y)  FreeSizeOf(x, sizeof(y))
#endif /* HAVE_ASO */


/**********************************************************************
 *
 * Tests of memory (for MDEBUG, only)
 *
 **********************************************************************/

#ifdef MDEBUG

/* use this variables to control level of MDEBUG at run-time
   (see mod2.h for details) */
extern int mm_MDEBUG;

BOOLEAN mmDBTestHeapBlock(const void* adr, const memHeap heap,
                          const char * fname, const int lineno );
BOOLEAN mmDBTestBlock(const void* adr, const size_t size,
                      const char * fname, const int lineno );
BOOLEAN mmDBTest(const void* adr, const char * fname, const int lineno);

#define mmTestHeap(a, h)\
  mmDBTestHeapBlock(a, h, __FILE__, __LINE__)
#define mmTest(A,B)     mmDBTestBlock(A,B,__FILE__,__LINE__)
#define mmTestL(A)      mmDBTest(A,__FILE__,__LINE__)
#define mmTestP(A,B)    mmDBTestBlock(A,B,__FILE__,__LINE__)
#define mmTestLP(A)     mmDBTest(A,__FILE__,__LINE__)

int mmTestMemory();
int mmTestHeaps();

void mmPrintUsedList();
void mmMarkInitDBMCB();
void mmMarkCurrentUsageState();
void mmMarkCurrentUsageStart();
void mmMarkCurrentUsageStop();
void mmPrintUnMarkedBlocks();
void mmStartReferenceWatch();
void mmStopReferenceWatch();
void mmTestList (int all);

#else


#define mmTestHeap(addr, heap) mmCheckHeapAddr(addr, heap)
#define mmTest(A,B) TRUE
#define mmTestL(A)  TRUE
#define mmTestP(A,B)
#define mmTestLP(A)
#define mmTestMemory 1
#define mmTestHeaps 1
#define mmMarkInitDBMCB()
#define mmTestList(a) 

#endif /* MDEBUG */

/**********************************************************************
 *
 * Public Heap Routines
 *
 **********************************************************************/

/**********************************
 *
 * Creation/Destruction/Garbage Collection
 *
 **********************************/
/* creates and initializes a temporary heap */
extern memHeap mmGetTempHeap(size_t size);
#ifndef HEAP_DEBUG
/* UNCONDITIONALLY clears and destroys temporary heap */
extern void mmUnGetTempHeap(memHeap *heap_p);
#else
#define mmUnGetTempHeap(h) mmDebugDestroyTempHeap(h)
extern void mmUnGetTempHeap(memHeap *heap);
#endif /* HEAP_DEBUG */

/* removes chunks in freelist which fill one page */
/* if strict & 1, does it even if free ptr  has not changed w.r.t. last gc */
/* if strict & 2, also releases free pages */
/* if strict & 4, gc also of temp heaps */
extern void mmGarbageCollectHeaps(int strict);
extern void mmGarbageCollectHeap(memHeap heap, int strict);

/* Returns a heap of the given size */
extern memHeap mmGetSpecHeap( size_t );
/* use this to "unget" (free) a heap once allocated with mmGetSpecHeap */
extern void mmUnGetSpecHeap(memHeap *heap);
  
/* Merges what is free in Heap "what" into free list of heap "into" */
extern void mmMergeHeap(memHeap into, memHeap what);
/* Removes addr from freelist of heap, provided it finds it there */
extern void mmRemoveFromCurrentHeap(memHeap heap, void* addr);

/**********************************
 *
 * Allocate, Free from Heaps
 *
 **********************************/
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
#endif /* HEAP_DEBUG */

/**********************************
 *
 * Low-level heap stuff 
 *
 **********************************/
/* Need to define it here, has to be known to macros */
struct sip_memHeap 
{
  void*         current; /* Freelist pointer */
  memHeapPage   pages;   /* Pointer to linked list of pages */
  void*         last_gc; /* current pointer after last gc */
  long          size;    /* Size of heap chunks */
};

/* array of static heaps */
extern struct sip_memHeap mm_theList[];

extern void mmAllocNewHeapPage(memHeap heap);
/* Allocates memory block from a heap */
#define _mmAllocHeap(what, heap)                            \
do                                                          \
{                                                           \
  register memHeap _heap = heap;                            \
  if ((_heap)->current == NULL) mmAllocNewHeapPage(_heap);  \
  what = (void *)((_heap)->current);              \
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
/* use this for unknown heaps */
#define MM_UNKNOWN_HEAP ((memHeap) 1)


/**********************************************************************
 *
 * Misc stuff
 *
 **********************************************************************/
/* for handling of memory statistics */
int mmMemAlloc( void );
int mmMemUsed( void );
#ifdef HAVE_SBRK
int mmMemPhysical( void );
#endif
void mmPrintStat();

size_t mmSizeL( void* );

/**********************************************************************
 *
 * Some operations on linked lists of memory
 *
 **********************************************************************/
/* The following routines assume that Next(list) == *((void**) list) */
/* Removes element from list, if contained in it and returns list */
void* mmRemoveFromList(void* list, void* element);
/* Returns the length of a memory list; assumes list has no cycles */
int mmListLength(void* list);
/* Returns last non-NULL element of list; assumes list has no cycles */
void* mmListLast(void* list);
/* returns 1, if addr is contained in memory list
 * 0, otherwise */
int mmIsAddrOnList(void* addr, void* list);
/* Checks whether memory list has cycles: If yes, returns address of
 * first element of list which is contained at least twice in memory
 * list. If no, NULL is returned */
void* mmListHasCycle(void* list);

/* The following routines assume that Next(list) == *((void**) list + next) */

/* Returns the length of a memory list; assumes list has no cycles */
int mmGListLength(void* list, int next);
/* Returns last non-NULL element of list; assumes list has no cycles */
void* mmGListLast(void* list, int next);
/* returns 1, if addr is contained in memory list
 * 0, otherwise */
int mmIsAddrOnGList(void* addr, void* list, int next);
/* Checks whether memory list has cycles: If yes, returns address of
 * first element of list which is contained at least twice in memory
 * list. If no, NULL is returned */
void* mmGListHasCycle(void* list, int next);
/* Removes element from list, if contained in it and returns list */
void* mmRemoveFromGList(void* list, int next, void* element);

/* The following cast (list + int_field) to a pointer to int 
   and assume list is sorted in ascending order w.r.t. *(list + int_field) 
 */
/* Inserts element at the right place */
void* mmSortedInsertInGList(void* list, int next, int int_field, void* element);
/* Finds element */
void* mmFindInSortedGList(void* list, int next, int int_field, int what);
  

  
/**********************************************************************
 *
 * some fast macros for basic memory operations
 *
 **********************************************************************/
#ifdef DO_DEEP_PROFILE
extern void _memcpyW(long* p1, long* p2, long l);
#define memcpy_nwEVEN(p1, p2, l)    _memcpyW((long*) p1, (long*) p2, (long) l)
#define memcpy_nwODD(p1, p2, l)     _memcpyW((long*) p1, (long*) p2, (long) l)
#define memcpyW(p1, p2, l)          _memcpyW((long*) p1, (long*) p2, (long) l)

extern void _memaddW(long* p1, long* p2, long* p3, long l);
#define memaddW(p1, p2, p3, l)          _memaddW(p1, p2, p3, l)
#define memadd_nwODD(p1, p2, p3, l)     _memaddW(p1, p2, p3, l)
#define memadd_nwEVEN(p1, p2, p3, l)    _memaddW(p1, p2, p3, l)
#define memadd_nwONE(p1, p2, p3)        _memaddW(p1, p2, p3, 1)
#define memadd_nwTWO(p1, p2, p3)        _memaddW(p1, p2, p3, 2)

extern void _memsetW(long* p1, long w, long l);
#define memsetW(p1, w, l) _memsetW(p1, w, l)

#else /* ! DO_DEEP_PROFILE */

#define memcpyW(p1, p2, l)                      \
do                                              \
{                                               \
  long _i = l;                                  \
  long* _s1 = (long*) p1;                       \
  const long* _s2 = (long*) p2;                 \
                                                \
  for (;;)                                      \
  {                                             \
    *_s1 = *_s2;                                \
    _i--;                                       \
    if (_i == 0) break;                         \
    _s1++;                                      \
    _s2++;                                      \
  }                                             \
}                                               \
while(0)

#define memcpy_nwODD(p1, p2, l)                 \
do                                              \
{                                               \
  long _i = l - 1;                              \
  long* _s1 = (long*) p1;                       \
  const long* _s2 = (long*) p2;                 \
                                                \
  *_s1++ = *_s2++;                              \
  for (;;)                                      \
  {                                             \
    *_s1++ = *_s2++;                            \
    *_s1++ = *_s2++;                            \
    _i -= 2;                                    \
    if (_i == 0) break;                         \
  }                                             \
}                                               \
while(0)

#define memcpy_nwEVEN(p1, p2, l)                \
do                                              \
{                                               \
  long _i = l;                                  \
  long* _s1 = (long*) p1;                       \
  const long* _s2 = (long*) p2;                 \
                                                \
  for (;;)                                      \
  {                                             \
    *_s1++ = *_s2++;                            \
    *_s1++ = *_s2++;                            \
    _i -= 2;                                    \
    if (_i == 0) break;                         \
  }                                             \
}                                               \
while(0)

#define memaddW(P1, P2, P3, L)                  \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
  do                                            \
  {                                             \
    *_p1++ = *_p2++ + *_p3++;                   \
    l--;                                        \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define memadd_nwODD(P1, P2, P3, L)             \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
 *_p1++ = *_p2++ + *_p3++;                      \
  l--;                                          \
                                                \
  do                                            \
  {                                             \
     *_p1++ = *_p2++ + *_p3++;                  \
     *_p1++ = *_p2++ + *_p3++;                  \
     l -=2;                                     \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define memadd_nwEVEN(P1, P2, P3, L)            \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
  do                                            \
  {                                             \
     *_p1++ = *_p2++ + *_p3++;                  \
     *_p1++ = *_p2++ + *_p3++;                  \
     l -=2;                                     \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define memadd_nwONE(P1, P2, P3)                \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
                                                \
 *_p1 = *_p2 + *_p3;                            \
}                                               \
while(0)

#define memadd_nwTWO(P1, P2, P3)                \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
                                                \
 *_p1++ = *_p2++ + *_p3++;                      \
 *_p1 = *_p2 + *_p3;                            \
}                                               \
while(0)

#define memsetW(P1, W, L)                       \
do                                              \
{                                               \
  long* _p1 = (long*) P1;                               \
  unsigned long _l = L;                         \
  long _w = (long) W;                                  \
                                                \
  while(_l)                                     \
  {                                             \
    *_p1++ = W;                                 \
    _l--;                                       \
  }                                             \
}                                               \
while(0)

#endif /* DO_DEEP_PROFILE */

#ifdef __cplusplus
}
int mmInit();
#endif

#if ! defined(MDEBUG)
inline void* AllocHeap(memHeap heap)
{
  void* ptr;
  mmAllocHeap(ptr, heap);
  return ptr;
}
inline void* Alloc0Heap(memHeap heap)
{
  void* ptr;
  mmAllocHeap(ptr, heap);
  memsetW(ptr, 0, (heap->size) >> LOG_SIZEOF_LONG);
  return ptr;
}
#endif


#endif
