/* emacs edit mode for this file is -*- C -*- */
/* $Id: memman.h,v 1.3 2004-09-23 16:51:48 Singular Exp $ */

#ifndef INCL_MEMMAN_H
#define INCL_MEMMAN_H

#include <config.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int  mmInit( void );

void mmSpecializeBlock( size_t );

void mmMark( void );

void mmSweep( void );

void mmSwitch( void );

int mmMemReal( void );

int mmMemUsed( void );

#define mstrdup mmStrdup
/* char * mstrdup(char *s); */

size_t mmSizeL( void* );

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

char * mmStrdup( char* );

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

char * mmDBStrdup( char * s, char *fname, int lineno);

int mmDBTestBlock( const void* adr, const size_t size, const char * fname, const int lineno );

int mmDBTest( const void* adr, const char * fname, const int lineno );

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
struct memman_new_delete
{
  inline void* operator new( size_t size ) { return mmAlloc( size ); }
  inline void* operator new []( size_t size ) { return mmAlloc( size ); }
  inline void operator delete( void* block ) {  mmFree( block ); }
  inline void operator delete []( void* block ) {  mmFree( block ); }
};
}
#endif

#endif /* ! INCL_MEMMAN_H */
