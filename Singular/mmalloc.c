/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmalloc.c,v 1.13 1999-03-19 16:00:04 Singular Exp $ */

/*
* ABSTRACT:
*/


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
    void *res=(void*)malloc( size );
    if (res==NULL)
    {
      WerrorS("out of memory");
      m2_end(99);
    }
    mm_bytesMalloc += size;
    if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
    return res;
  }
  else
  {
    void* res;
    AllocHeap(res, &mm_theList[i]);
    return res;
  }
}

void mmFreeBlock(void* adr, size_t size)
{
  int i;
  if (adr == NULL ||  size == 0) return;
  
  i = mmGetIndex(size);
  
  if (i < 0)
  {
    mm_bytesMalloc -= size;
    if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
    free( adr );
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
    void *res=(void *)realloc( adr, newsize );
    if (res==NULL)
    {
      WerrorS("out of memory");
      m2_end(99);
    }
    mm_bytesMalloc += ( (int)newsize - (int)oldsize );
    if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
    return res;
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
  mmTrack(result->bt_stack);
#endif

  if (! mmCheckDBMCB(result, SizeFromRealSize(mmGetHeapBlockSize(heap)),
                     MM_FREEFLAG))
  {
    mmPrintFL( fname, lineno );
    return result;
  }
    
  mmMoveDBMCB( &mm_theDBfree, &mm_theDBused, result );
  mmFillDBMCB(result, size, heap, MM_USEDFLAG, fname, lineno);

  return (void*) &(result->data);
}

static void mmDBFreeHeapS(void* addr, memHeap heap, size_t size,
                          char* fname, int lineno)
{
  DBMCB *what = (DBMCB*) ((char*) addr - DebugOffsetFront);

  if ( ! mmCheckDBMCB(what, size, MM_USEDFLAG))
  {
    mmPrintFL( fname, lineno );
    return;
  }

  
#ifdef HEAP_DEBUG
  mmDebugFreeHeap(what, heap, fname, lineno);
#else
  assume(heap != NULL);
  mmFreeHeap(what, heap);
#endif
  
  mmMoveDBMCB( &mm_theDBused, &mm_theDBfree, what );
  mmFillDBMCB(what, SizeFromRealSize(mmGetHeapBlockSize(heap)),
              heap, MM_FREEFLAG, fname, lineno);
}

void * mmDBAllocHeap(memHeap heap, char* f, int l)
{
  return mmDBAllocHeapS(heap, SizeFromRealSize(mmGetHeapBlockSize(heap)),f,l);
}

                 
void   mmDBFreeHeap(void* addr, memHeap heap, char*f, int l)
{
  mmDBFreeHeapS(addr, heap, SizeFromRealSize(mmGetHeapBlockSize(heap)),f,l);
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
    DBMCB * result=NULL;
    int tmpsize=RealSizeFromSize(size);
    if ((result = (DBMCB*)malloc(tmpsize))!=NULL)
    {
      mm_bytesMalloc += tmpsize;
      if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();

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
      mmTrack(result->bt_stack);
      #endif
      
      return (void*) &(result->data);
    }
    else
    {
      WerrorS("out of memory");
      m2_end(99);
      return NULL;
    }
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

    if ( ! mmCheckDBMCB(what, size, MM_USEDFLAG))
    {
      mmPrintFL( fname, lineno );
      return;
    }
    mm_bytesMalloc -= tmpsize;
    mmTakeOutDBMCB(what );
    free( what );
    if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
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
#if SIZEOF_DOUBLE != SIZEOF_VOIDP 
    void *garbage = NULL, *good, *temp;

    while (1)
    {
#endif /* SIZEOF_DOUBLE != SIZEOF_VOIDP */

#ifdef MDEBUG
      good = mmDBAllocHeapS(&(mm_theList[i]), size, f, l);
#else
      AllocHeap(good, &(mm_theList[i]));
#endif
      assume(good != NULL);

#if SIZEOF_DOUBLE != SIZEOF_VOIDP 
    
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
#endif /* SIZEOF_DOUBLE != SIZEOF_VOIDP */

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
    


  
  
  

