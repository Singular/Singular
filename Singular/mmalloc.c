/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmalloc.c,v 1.21 1999-12-13 17:52:12 Singular Exp $ */

/*
* ABSTRACT: implementation of alloc/free routines
*/

#ifndef MM_ALLOC_C
#define MM_ALLOC_C

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mod2.h"
#ifdef HAVE_MALLOC_H
#include "malloc.h"
#endif
#include "structs.h"
#include "febase.h"
#include "tok.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmpage.h"
#ifdef MTRACK
#include "mmbt.h"
#endif

#undef HAVE_ASSUME
#define HAVE_ASSUME

#ifndef MDEBUG

/**********************************************************************
 *
 * Block Routines
 *
 **********************************************************************/

void* mmAllocBlock(size_t size)
{
  int i;
  assume(size > 0);
  
  i = mmGetIndex(size);
  
  if (i < 0)
  {
    return mmMallocFromSystem(size);
  }
  else
  {
    return AllocHeap(&mm_theList[i]);
  }
}

void mmFreeBlock(void* adr, size_t size)
{
  int i;
  if (adr == NULL ||  size == 0) return;
  
  i = mmGetIndex(size);
  
  if (i < 0)
  {
    mmFreeToSystem(adr, size);
  }
  else
  {
    FreeHeap(adr, &(mm_theList[i]));
  }
}

void * mmAllocBlock0( size_t size )
{
  void *result=mmAllocBlock(size);
  memset(result,0,size);
  return result;
}

void * mmReallocBlock( void* adr, size_t oldsize, size_t newsize )
{
  int i = (oldsize == 0 ? -1: mmGetIndex( oldsize ));
  int j = mmGetIndex( newsize );

  if ( ( i < 0 ) && ( j < 0 ) )
  {
    return mmReallocFromSystem(adr, newsize, oldsize);
  }
  else if ( i == j )
    return adr;
  else
  {
    /* since we know i and j this can be done better, this is the quick way */
    void *newadr = mmAllocBlock( newsize );
    memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
    mmFreeBlock( adr, oldsize );
    return newadr;
  }
}

void * mmReallocBlock0( void* adr, size_t oldsize, size_t newsize )
{
  void* result = mmReallocBlock(adr, oldsize, newsize);
  if (oldsize < newsize)
    memset(&(((char*)result)[oldsize]), 0, newsize-oldsize);

  return result;
}

/**********************************************************************
 *
 * Routines for chunks of memory
 *
 **********************************************************************/
    
void * mmAlloc( size_t size )
{
  size_t thesize = size + sizeof( ADDRESS );
  size_t * dummy = (size_t*)mmAllocBlock( thesize );
  *dummy = thesize;
  return (void*)(dummy+1);
}

void mmFree( void* adr )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    mmFreeBlock( adr, *(size_t*)adr );
  }
}

void * mmRealloc( void* adr, size_t newsize )
{
  size_t *aadr=(size_t*)(adr)-1;
  size_t oldsize = *aadr;
  void* newadr = mmAlloc( newsize );
  memcpy( newadr, adr, min(oldsize-sizeof(ADDRESS),newsize) );
  mmFreeBlock( aadr,oldsize );
  return newadr;
}

char * mmStrdup( const char * s)
{
  if (s==NULL) return NULL;
  {
    char * rc = (char*)mmAlloc( 1 + strlen( s ));
    strcpy( rc, s );
    return rc;
  }
}

#else /* MDEBUG */

/**********************************************************************
 *
 * Heap routines 
 *
 **********************************************************************/
static void * mmDBAllocHeapS(memHeap heap, size_t size,
                             char* fname, int lineno)
{
  DBMCB* result;

#ifdef HEAP_DEBUG
  result = mmDebugAllocHeap(heap, fname, lineno);
#else
  mmAllocHeap((void*) result, heap);
#endif  
    
#ifdef MTRACK
  mmTrack(result->bt_allocated_stack);
#endif

  mmMoveDBMCB( &mm_theDBfree, &mm_theDBused, result );
  mmFillDBMCB(result, size, heap, MM_USEDFLAG, fname, lineno);
  return (void*) &(result->data);
}

static void mmDBFreeHeapS(void* addr, memHeap heap, size_t size,
                          char* fname, int lineno)
{
  DBMCB *what = (DBMCB*) ((char*) addr - DebugOffsetFront);

#if MDEBUG >= 0
  if ( ! mmCheckDBMCB(what, size, MM_USEDFLAG))
  {
    mmPrintFL( fname, lineno );
    return;
  }
#endif

#ifdef HEAP_DEBUG
  mmDebugFreeHeap(what, heap, fname, lineno);
#else
  assume(heap != NULL);
  mmFreeHeap(what, heap);
#endif
  
  mmMoveDBMCB( &mm_theDBused, &mm_theDBfree, what );
  mmFillDBMCB(what, SizeFromRealSize(heap->size),
              heap, MM_FREEFLAG, fname, lineno);
  
#ifdef MTRACK_FREE
  mmTrack(what->bt_freed_stack);
#endif  
}

void * mmDBAllocHeap(memHeap heap, char* f, int l)
{
  return mmDBAllocHeapS(heap, SizeFromRealSize(heap->size),f,l);
}

void * mmDBAlloc0Heap(memHeap heap, char* f, int l)
{
  void* ptr = mmDBAllocHeapS(heap, SizeFromRealSize(heap->size),f,l);
  memsetW(ptr, 0, (SizeFromRealSize(heap->size)) >> LOG_SIZEOF_LONG);
  return ptr;
}

                 
void   mmDBFreeHeap(void* addr, memHeap heap, char*f, int l)
{
  mmDBFreeHeapS(addr, heap, SizeFromRealSize(heap->size),f,l);
}


/**********************************************************************
 *
 * Block Routines
 *
 **********************************************************************/
  
void * mmDBAllocBlock( size_t size,  char * fname, int lineno)
{
  int i;

  if (size==0)
  {
    fprintf(stderr,"alloc(0) in %s:%d\n",fname,lineno);
    size = 1;
  }

  i = mmGetIndex( size );
  if ( i < 0 )
  {
    int tmpsize=RealSizeFromSize(size);
    DBMCB * result = (DBMCB *) mmMallocFromSystem(tmpsize);
    memset(result, 0, sizeof(DBMCB));
    
    mmMoveDBMCBInto( &mm_theDBused, result );
      
    if ( mm_minAddr == NULL )
    {
      mm_minAddr = (void*)result;
      mm_maxAddr = (void*)result;
    }
    else if ( (void*)result < mm_minAddr )
      mm_minAddr = (void*)result;
    else if ( (void*)result > mm_maxAddr )
      mm_maxAddr = (void*)result;

    mmFillDBMCB(result, size, NULL, MM_USEDFLAG, fname, lineno);
  
#ifdef MTRACK
    mmTrack(result->bt_allocated_stack);
#endif
    return (void*) &(result->data);
  }
  else
  {
    return mmDBAllocHeapS(&(mm_theList[i]), size, fname, lineno);
  }
}

void mmDBFreeBlock(void* adr, size_t size, char * fname, int lineno)
{
  int i;
  
  if ( ( adr == NULL ) || ( size == 0 ) ) return;

  i = mmGetIndex( size );
  if ( i < 0 )
  {
    DBMCB * what = (DBMCB*) ((char*) adr - DebugOffsetFront);
    int tmpsize=RealSizeFromSize(size);

#if MDEBUG >= 0
    if ( ! mmCheckDBMCB(what, size, MM_USEDFLAG))
    {
      mmPrintFL( fname, lineno );
      return;
    }
#endif

    mmTakeOutDBMCB(what);
    mmFreeToSystem(what, tmpsize);
  }
  else
  {
    mmDBFreeHeapS(adr, &(mm_theList[i]), size, fname, lineno);
  }
}

void * mmDBAllocBlock0( size_t size, char * fname, int lineno )
{
  void *result=mmDBAllocBlock(size, fname,lineno);
  memset(result,0,size);
  return result;
}

void * mmDBReallocBlock( void* adr, size_t oldsize, size_t newsize, 
                         char * fname, int lineno )
{
  void* newadr;
  mmTest(adr, oldsize);
  newadr = mmDBAllocBlock( newsize, fname, lineno);
  memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
  mmDBFreeBlock( adr, oldsize, fname, lineno);

  return newadr;
}

void * mmDBReallocBlock0( void* adr, size_t oldsize, size_t newsize, 
                          char * fname, int lineno )
{
  void* newadr;

  newadr = mmDBAllocBlock0( newsize, fname, lineno);
  memcpy( newadr, adr, (oldsize < newsize) ? oldsize : newsize );
  mmDBFreeBlock( adr, oldsize, fname, lineno);

  return newadr;
}


/**********************************************************************
 *
 * Routines for chunks of memory
 *
 **********************************************************************/

void * mmDBAlloc( size_t size, char* fname, int lineno )
{
  size_t thesize = size + sizeof( ADDRESS );
  size_t * dummy = (size_t*)mmDBAllocBlock( thesize, fname, lineno);
  *dummy = thesize;
  return (void*)(dummy+1);
}

void mmDBFree( void* adr, char* fname, int lineno )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    mmDBFreeBlock( adr,*(size_t *)adr, fname, lineno);
  }
}

void * mmDBRealloc( void* adr, size_t newsize, char* fname, int lineno )
{
  size_t *aadr=(size_t*)(adr)-1;
  size_t oldsize = *aadr;
  void* newadr = mmDBAlloc( newsize, fname, lineno );
  memcpy( newadr, adr, min(oldsize-sizeof(ADDRESS), newsize) );
  mmDBFreeBlock( aadr, oldsize, fname, lineno);
  return newadr;
}

char * mmDBStrdup( const char * s, char *fname, int lineno)
{
  if (s==NULL) return NULL;
  {
    char * rc = (char*)mmDBAlloc( 1 + strlen( s ),fname,lineno );
    strcpy( rc, s );
    return rc;
  }
}

#endif /* MDEBUG */

/**********************************************************************
 *
 * Routines to debug ASO
 *
 **********************************************************************/
#if defined(ASO_DEBUG) || defined(MDEBUG)
void* mmDBAllocHeapSizeOf(memHeap heap, size_t size, char* file, int line)
{
  if (&(mm_theList[mmGetIndex(size)]) != heap)
  {
    fprintf(stderr, "ASO Error: Got heap %d:%ld but should be from heap %d:%d occured in %s:%d\n", 
            mmGetIndex(SizeFromRealSize(heap->size)), SizeFromRealSize(heap->size), 
            mmGetIndex(size), size, file, line);
    fflush(stderr);
    heap = &(mm_theList[mmGetIndex(size)]);
  }

#ifdef MDEBUG
  return mmDBAllocHeapS(heap, size, file, line);
#else
  return AllocHeap(heap);
#endif
}

void* mmDBAlloc0HeapSizeOf(memHeap heap, size_t size, char* file, int line)
{
  void* res;
  if (&(mm_theList[mmGetIndex(size)]) != heap)
  {
    fprintf(stderr, "ASO Error: Got heap %d:%ld but should be from heap %d:%d occured in %s:%d\n", 
            mmGetIndex(SizeFromRealSize(heap->size)), SizeFromRealSize(heap->size), 
            mmGetIndex(size), size, file, line);
    fflush(stderr);
    heap = &(mm_theList[mmGetIndex(size)]);
  }
#ifdef MDEBUG
  res = mmDBAllocHeapS(heap, size, file, line);
  memset(res, 0, size);
  return res;
#else
  return Alloc0Heap(heap);
#endif
}

void  mmDBFreeHeapSizeOf(void* addr, memHeap heap, size_t size, 
                         char* file, int line)
{
  if (&(mm_theList[mmGetIndex(size)]) != heap)
  {
    fprintf(stderr, "ASO Error: Got heap %d:%ld but should be from heap %d:%d occured in %s:%d\n", 
            mmGetIndex(SizeFromRealSize(heap->size)), SizeFromRealSize(heap->size), 
            mmGetIndex(size), size, file, line);
    fflush(stderr);
    heap = &(mm_theList[mmGetIndex(size)]);
  }
#ifdef MDEBUG 
  mmDBFreeHeapS(addr, heap, size, file, line);
#else
  mmFreeHeap(addr, heap);
#endif
}

#endif /* ASO_DEBUG || MDEBUG */

/**********************************************************************
 *
 * Routines for aligned memory allocation
 *
 **********************************************************************/
    
#if SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP

#ifdef MDEBUG
void * mmDBAllocAlignedBlock( size_t size, char* f, int l)    
#else
void * mmAllocAlignedBlock( size_t size)
#endif
{
  int i = mmGetIndex(size);
  
  if (i < 0)
  {
#ifdef MDEBUG
    unsigned long ret = 
      (unsigned long) mmDBAllocBlock(size + SIZEOF_DOUBLE + 1, f, l);
#else
    unsigned long ret = 
      (unsigned long) mmAllocBlock(size+SIZEOF_DOUBLE+1);
#endif
    unsigned char shift = (ret + 1) & (SIZEOF_DOUBLE - 1);
    *((unsigned char*) ret) = (unsigned char) shift;

    assume(ret != 0);
    ret = ((ret + 1) & ~(SIZEOF_DOUBLE - 1));

    assume(ret % SIZEOF_DOUBLE == 0);

    return (void*) ret;
  }
  else
  {
    void *garbage = NULL, *good, *temp;

    while (1)
    {
#ifdef MDEBUG
      good = mmDBAllocHeapS(&(mm_theList[i]), size, f, l);
#else
      good = AllocHeap(&(mm_theList[i]));
#endif
      assume(good != NULL);

      if (((unsigned long) good) & (SIZEOF_DOUBLE - 1))
      {
        *((void**) good) = garbage;
        garbage = good;
      }
      else
      {
        break;
      }
    }
  
    while (garbage != NULL)
    {
      temp = garbage;
      garbage = *((void**) garbage);
#ifdef MDEBUG
      mmDBFreeHeapS(temp, &(mm_theList[i]), size, f, l);
#else
      FreeHeap(temp, &(mm_theList[i]));
#endif
    }
    assume(((unsigned long) good) % SIZEOF_DOUBLE == 0);
    
    return good;
  }
}


#ifdef MDEBUG
void * mmDBAllocAlignedBlock0( size_t size, char* f, int l)    
#else
void * mmAllocAlignedBlock0( size_t size)
#endif
{
  void* good;
#ifdef MDEBUG
  good = mmDBAllocAlignedBlock(size, f, l);
#else
  good = mmAllocAlignedBlock(size);
#endif
  
  memset(good, 0, size);
  
  return good;
}

#ifdef MDEBUG
void  mmDBFreeAlignedBlock( void* addr, size_t size, char* f, int l)    
#else
void  mmFreeAlignedBlock( void* addr, size_t size)
#endif
{
  int i = mmGetIndex(size);

  assume((unsigned long) addr % SIZEOF_DOUBLE == 0);
  
  if (i < 0)
  {
    unsigned char* adj_addr = ((unsigned char*) addr) - 1;
    unsigned char shift = *adj_addr;

#ifdef MDEBUG    
    mmDBFreeBlock(adj_addr - shift, size + SIZEOF_DOUBLE + 1, f, l);
#else
    mmFreeBlock(adj_addr - shift, size + SIZEOF_DOUBLE + 1);
#endif
  }
  else
  {
#ifdef MDEBUG
    mmDBFreeHeapS(addr, &(mm_theList[i]), size, f, l);
#else
    FreeHeap(addr, &(mm_theList[i]));
#endif
  }
}

#endif /* SIZEOF_DOUBLE == SIZEOF_VOIDP + SIZEOF_VOIDP */
    

/**********************************************************************
 *
 * malloc/free routine from/to system
 *
 **********************************************************************/
void* mmMallocFromSystem(size_t size)
{
  void* ptr;
#ifdef ALIGN_8
  if (size % 8 != 0) size = size + 8 - (size % 8);
#endif
  ptr = malloc(size);
  if (ptr == NULL)
  {
    mmGarbageCollectHeaps(4);
    ptr = malloc(size);
    if (ptr == NULL)
    {
      WerrorS("out of memory");
      m2_end(99);
      /* should never get here */
      exit(1);
    }
  }
  mm_bytesMalloc += size;
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
  return ptr;
}

void* mmReallocFromSystem(void* addr, size_t newsize, size_t oldsize)
{
  void* res;
#ifdef ALIGN_8
  if (newsize % 8 != 0) newsize = newsize + 8 - (newsize % 8);
  if (oldsize % 8 != 0) oldsize = oldsize + 8 - (oldsize % 8);
#endif
  
  res = realloc(addr, newsize);
  if (res == NULL)
  {
    mmGarbageCollectHeaps(4);
    /* Can do a realloc again: manpage reads:
       "If realloc() fails the original block is left untouched - 
       it is not freed or moved." */
    res = realloc(addr, newsize); 
    if (res == NULL)
    {
      WerrorS("out of memory");
      m2_end(99);
      /* should never get here */
      exit(1);
    }
  }
  mm_bytesMalloc += ( (int)newsize - (int)oldsize );
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
  return res;
}
  
void mmFreeToSystem(void* addr, size_t size)
{
#ifdef ALIGN_8
  if (size % 8 != 0) size = size + 8 - (size % 8);
#endif
  free( addr );
  mm_bytesMalloc -= size;
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
}

void* mmVallocFromSystem(size_t size)
{
  void* page = PALLOC(SIZE_OF_PAGE);
  if (page == NULL)
  {
    mmGarbageCollectHeaps(4);
    page = PALLOC(SIZE_OF_PAGE);
    if (page == NULL)
    {
      (void)fprintf( stderr, "\nerror: no more memory\n" );
      m2_end(14);
      /* should never get here */
      exit(1);
    }
  }
  mm_bytesValloc += SIZE_OF_PAGE;
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
  return page;
}

void mmVfreeToSystem(void* page, size_t size)
{
  PFREE(page);
  mm_bytesValloc -= size;
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
}
  

#endif /* MM_ALLOC_C */

      
