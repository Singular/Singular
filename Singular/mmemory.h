#ifndef MMEMORY_H
#define MMEMORY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmemory.h,v 1.8 1998-04-28 08:39:40 obachman Exp $ */
/*
* ABSTRACT
*/
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mod2.h"
  
#ifdef DO_DEEP_PROFILE
extern void _memcpyW(void* p1, void* p2, long l);
#define  memcpyW(p1, p2, l) _memcpyW((void*) p1, (void*) p2, (long) l)  
#else                                                                   
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
#endif
  

void mmSpecializeBlock( size_t );

int mmMemReal( void );
int mmMemUsed( void );
#ifdef HAVE_SBRK
int mmMemPhysical( void );
#endif  

#define mstrdup mmStrdup
/* char * mstrdup(char *s); */

size_t mmSizeL( void* );
#define mmGetSpecSize() mm_specSize

extern size_t mm_specSize;

#ifdef MM_STAT
void mmStat(int slot);
#endif
#ifndef MDEBUG

void * mmAllocBlock( size_t );
void * mmAllocBlock0( size_t );

void mmFreeBlock( void*, size_t );

void * mmReallocBlock( void*, size_t, size_t );

void * mmAllocSpecialized( void );

void mmFreeSpecialized( void* );

void * mmAlloc( size_t );

void * mmRealloc( void*, size_t );

void mmFree( void* );

char * mmStrdup( const char* );

#define mmTest(A,B) TRUE
#define mmTestL(A) TRUE
#define mmTestP(A,B)
#define mmTestLP(A)
#define Free mmFreeBlock
#define Alloc mmAllocBlock
#define Alloc0 mmAllocBlock0
#define ReAlloc mmReallocBlock
#define FreeL mmFree
#define AllocL mmAlloc

#else /* MDEBUG */

void * mmDBAllocBlock( size_t, char*, int );
void * mmDBAllocBlock0( size_t, char*, int );

void mmDBFreeBlock( void*, size_t, char*, int );

void * mmDBReallocBlock( void*, size_t, size_t, char*, int );

void * mmDBAllocSpecialized( char*, int );

void mmDBFreeSpecialized( void*, char*, int );

void * mmDBAlloc( size_t, char*, int );

void * mmDBRealloc( void*, size_t, char*, int );

void mmDBFree( void*, char*, int );

char * mmDBStrdup( const char * s, char *fname, int lineno);

BOOLEAN mmDBTestBlock( const void* adr, const size_t size, const char * fname, const int lineno );

BOOLEAN mmDBTest( const void* adr, const char * fname, const int lineno );

#define mmAllocBlock(size) mmDBAllocBlock(size,__FILE__,__LINE__)

#define mmFreeBlock(adr,size) mmDBFreeBlock(adr,size,__FILE__,__LINE__)

#define mmAllocSpecialized() mmDBAllocSpecialized(__FILE__,__LINE__)

#define mmFreeSpecialized(adr) mmDBFreeSpecialized(adr,__FILE__,__LINE__)

#define mmAlloc(size) mmDBAlloc(size,__FILE__,__LINE__)

#define mmFree(adr) mmDBFree(adr,__FILE__,__LINE__)

#define mmReallocBlock(adr,oldsize,newsize) mmDBReallocBlock(adr,oldsize,newsize,__FILE__,__LINE__)

#define mmStrdup(A) mmDBStrdup(A,__FILE__,__LINE__)

#define mmTest(A,B) mmDBTestBlock(A,B,__FILE__,__LINE__)
#define mmTestL(A) mmDBTest(A,__FILE__,__LINE__)
#define mmTestP(A,B) mmDBTestBlock(A,B,__FILE__,__LINE__)
#define mmTestLP(A) mmDBTest(A,__FILE__,__LINE__)
#define Alloc(size) mmDBAllocBlock(size,__FILE__,__LINE__)
#define Alloc0(size) mmDBAllocBlock0(size,__FILE__,__LINE__)
#define Free(adr,size) mmDBFreeBlock(adr,size,__FILE__,__LINE__)
#define mmAllocSpecialized() mmDBAllocSpecialized(__FILE__,__LINE__)
#define mmFreeSpecialized(adr) mmDBFreeSpecialized(adr,__FILE__,__LINE__)
#define AllocL(size) mmDBAlloc(size,__FILE__,__LINE__)
#define FreeL(adr) mmDBFree(adr,__FILE__,__LINE__)
#define ReAlloc(adr,oldsize,newsize) mmDBReallocBlock(adr,oldsize,newsize,__FILE__,__LINE__)

void mmTestList();

#endif /* MDEBUG */
#ifdef __cplusplus
}
#endif

#endif
