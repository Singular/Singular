#ifndef MMEMORY_H
#define MMEMORY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmemory.h,v 1.13 1998-12-16 18:43:40 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "structs.h"
#include "mmheap.h"

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

#define AllocHeap               mmAllocHeap
#define FreeHeap                mmFreeHeap
#define Alloc                   mmAllocBlock
#define Alloc0                  mmAllocBlock0
#define Free                    mmFreeBlock
#define ReAlloc                 mmReallocBlock
#define ReAlloc0                mmReallocBlock0
#define FreeL                   mmFree
#define AllocL                  mmAlloc
#define mstrdup                 mmStrdup


#else /* MDEBUG */

void * mmDBAllocHeap(memHeap heap, char*, int );
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

#define AllocHeap(res, heap)\
  ((void*)(res)) = mmDBAllocHeap(heap, __FILE__, __LINE__)
#define FreeHeap(addr, heap)\
  mmDBFreeHeap(addr, heap,  __FILE__, __LINE__)
#define Alloc(s)                mmDBAllocBlock(s, __FILE__, __LINE__)
#define Alloc0(s)               mmDBAllocBlock0(s, __FILE__, __LINE__)
#define Free(a,s)               mmDBFreeBlock(a, s, __FILE__, __LINE__)
#define ReAlloc(a,o,n)          mmDBReallocBlock(a, o, n, __FILE__, __LINE__)
#define ReAlloc0(a,o,n)         mmDBReallocBlock0(a, o, n, __FILE__, __LINE__)
#define AllocL(s)               mmDBAlloc(s, __FILE__, __LINE__)
#define FreeL(a)                mmDBFree(a,__FILE__,__LINE__)
#define mstrdup(s)              mmDBStrdup(s, __FILE__, __LINE__)

#endif /* MDEBUG */


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

#else


#define mmTestHeap(addr, heap) mmCheckHeapAddr(addr, heap)
#define mmTest(A,B) TRUE
#define mmTestL(A)  TRUE
#define mmTestP(A,B)
#define mmTestLP(A)
#define mmTestMemory 1
#define mmTestHeaps 1

#endif /* MDEBUG */


/**********************************************************************
 *
 * Misc stuff
 *
 **********************************************************************/

/* For handling of monomials */
size_t mmSpecializeBlock( size_t );
size_t mmGetSpecSize();
extern memHeap mm_specHeap;

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

/**********************************************************************
 *
 * some fast macros for basic memory operations
 *
 **********************************************************************/
#ifdef DO_DEEP_PROFILE
extern void _memcpyW(void* p1, void* p2, long l);
#define memcpy_nwEVEN(p1, p2, l)    _memcpyW((void*) p1, (void*) p2, (long) l)
#define memcpy_nwODD(p1, p2, l)     _memcpyW((void*) p1, (void*) p2, (long) l)
#define memcpyW(p1, p2, l)          _memcpyW((void*) p1, (void*) p2, (long) l)

extern void _memaddW(void* p1, void* p2, void* p3, long l);
#define memaddW(p1, p2, p3, l)          _memaddW(p1, p2, p3, l)
#define memadd_nwODD(p1, p2, p3, l)     _memaddW(p1, p2, p3, l)
#define memadd_nwEVEN(p1, p2, p3, l)    _memaddW(p1, p2, p3, l)
#define memadd_nwONE(p1, p2, p3)        _memaddW(p1, p2, p3, 1)
#define memadd_nwTWO(p1, p2, p3)        _memaddW(p1, p2, p3, 2)

extern void _memsetW(void* p1, long w, long l);
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
  long* _p1 = P1;                               \
  unsigned long _l = L;                         \
  long _w = W;                                  \
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
#endif


#endif
