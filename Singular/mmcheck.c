/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmcheck.c,v 1.4 1999-03-18 17:00:15 Singular Exp $ */

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod2.h"

#ifdef MDEBUG

#include "mmemory.h"
#include "mmprivate.h"
#include "mmpage.h"
#include "mmheap.h"



DBMCB mm_theDBused;
DBMCB mm_theDBfree;

void * mm_maxAddr = NULL;
void * mm_minAddr = NULL;

/* int mm_MDEBUG = MDEBUG; */
int mm_MDEBUG = 0;

/**********************************************************************
 *
 * Auxillary routines
 *
 **********************************************************************/

void mmMoveDBMCB ( pDBMCB from, pDBMCB to, DBMCB * what )
{
  what->prev->next = what->next;
  if ( what->next != NULL )
    what->next->prev = what->prev;
  what->prev = to;
  what->next = to->next;
  if (to->next!=NULL)
    to->next->prev=what;
  to->next = what;
}

void mmMoveDBMCBInto ( pDBMCB to, pDBMCB what )
{
  if (to->next !=NULL)
  {
    what->next = to->next;
    what->next->prev = what;
  }
  to->next = what;
  what->prev = to;
}

void mmTakeOutDBMCB ( pDBMCB what )
{
  what->prev->next = what->next;
  if ( what->next != NULL )
    what->next->prev = what->prev;
}

void mmDBSetHeapsOfBlocks(memHeap fromheap, memHeap toheap)
{
  memHeapPage pages = fromheap->pages;
  int nblocks = SIZE_OF_HEAP_PAGE / toheap->size;
  int i;

  assume(fromheap->size == toheap->size);
  while (pages != NULL)
  {
    DBMCB *what = (DBMCB*) (((char*) pages) + SIZE_OF_HEAP_PAGE_HEADER);
    
    for (i=0; i<nblocks; i++)
    {
      what->heap = toheap;
      what = (DBMCB*) (((char*) what)  + toheap->size);
    }
    pages = pages->next;
  }
}

void mmDBInitNewHeapPage(memHeap heap)
{
  DBMCB* what = (DBMCB*) heap->current;
  DBMCB* prev = NULL;
  size_t size = SizeFromRealSize(heap->size);
  
  if (mm_minAddr == 0 || mm_minAddr > (void*) what)
    mm_minAddr = (void*) what;

  while (what != NULL)
  {
    mmFillDBMCB(what, size, heap, MM_FREEFLAG, __FILE__, __LINE__);    
    mmMoveDBMCBInto(&mm_theDBfree, what);
    prev = what;
    what = *((void**) what);
  }
  
  if (mm_maxAddr == 0 || mm_maxAddr < (void*) prev)
    mm_maxAddr = (void*) prev;
}

static int mmPrintDBMCB ( DBMCB * what, char* msg , int given_size)
{
  (void)fprintf( stderr, "warning: %s\n", msg );
  (void)fprintf( stderr, "block %x allocated in: %s:%d\n",
     (int)&(what->data), what->fname, what->lineno );
  if (strcmp(msg,"size")==0)
    (void)fprintf( stderr, "size is: %d, but check said %d \n",
      (int)what->size, given_size );
  else
    (void)fprintf( stderr, "size is: %d\n", (int)what->size );
  return 0;
}

void mmPrintUsedList( )
{
  DBMCB * what;
  what=mm_theDBused.next;
  fprintf(stderr,"list of used blocks:\n");
  while (what!=NULL)
  {
    (void)fprintf( stderr, "%d bytes at %p in: %s:%d\n",
      (int)what->size, what, what->fname, what->lineno);
    what=what->next;
  }
}
    
void mmPrintFL( const char* fname, const int lineno )
{
  (void)fprintf( stderr, "occured in %s:%d\n", fname, lineno );
}

/**********************************************************************
 *
 * Init routines
 *
 **********************************************************************/

#define MM_USED_PATTERN 255
#define MM_FREE_PATTERN 251
#define MM_FRONT_PATTERN 247
#define MM_BACK_PATTERN  249

void mmFillDBMCB(DBMCB* what, size_t size, memHeap heap, 
                 int flags, char* fname, int lineno)
{
  void* addr = &(what->data);

  what->heap = heap;
  what->size = size;
  what->fname = fname;
  what->lineno = lineno;
  what->flags = flags;
  
  if (flags & MM_FREEFLAG)
    memset(addr, MM_FREE_PATTERN, size);

  memset(what->front_pattern, MM_FRONT_PATTERN, MM_NUMBER_OF_FRONT_PATTERNS);
  memset((char*) addr + size, MM_BACK_PATTERN, DebugOffsetBack);
  #ifdef MTRACK
  mmTrack(what->bt_stack);
  #endif
}

/**********************************************************************
 *
 * Check routines
 *
 **********************************************************************/

static int mmCheckPattern(char* ptr, char pattern, size_t size)
{
  int i;
  
  for (i = 0; i<size; i++)
    if (ptr[i] != pattern) return 0;
  return 1;
}

#ifdef unix
extern int _end;
#endif
static int mmCheckSingleDBMCB ( DBMCB * what, int size , int flags)
{
  char * patptr;
  int i;
  int ok=1;

  if ( ((int)what % 4 ) != 0 )
  {
    fprintf( stderr, "warning: odd address\n" );
    assume(0);
    return 0;
  }
  if ( (void*)what > mm_maxAddr )
  {
    fprintf( stderr, "warning: address too high\n" );
    assume(0);
    return 0;
  }
  if ( (void*)what < mm_minAddr )
  {
    fprintf( stderr, "warning: address too low\n" );
    assume(0);
    return 0;
  }
  if (! mmIsNotAddrOnFreePage(what))
  {
    fprintf(stderr, "warning: address on freed page\n");
    assume(0);
    return 0;
  }
  
  if ( ((long)what->fname<1000)
  #ifdef unix
       ||((long)what->fname>(int)&(_end))
  #endif
  )
  { /* fname should be in the text segment */
    fprintf(stderr, "warning: fname (%lx) out of range\n", (long)what->fname );
    assume(0);
    what->fname="???";
    ok=0;
  }
  if ( (what->lineno< -9999) ||(what->lineno>9999) )
  {
    fprintf( stderr, "warning: lineno %d out of range\n",what->lineno );
    assume(0);
    what->lineno=0;
    ok=0;
  }
  #ifdef unix
  if ( (what->next!=NULL)
       && (((long)what->next>(long)mm_maxAddr)
           || ((long)what->next<(long)mm_minAddr) )
       && (((long)what->next>(long)&_end)
           || ((long)what->next<1000))
  )
  {
    fprintf( stderr, "warning: next (%lx) out of range\n", (long)what->next );
    assume(0);
    what->next=NULL;
    ok=0;
  }
  if ( (what->prev!=NULL)
       && (((long)what->prev>(long)mm_maxAddr)
           || ((long)what->prev<(long)mm_minAddr) )
       && (((long)what->prev>(long)&_end)
           || ((long)what->prev<1000)))
  {
    fprintf( stderr, "warning: prev (%lx) out of range\n", (long)what->prev );
    assume(0);
    what->prev=NULL;
    ok=0;
  }
  #endif

  if ( ( what->flags & flags ) != flags )
    return mmPrintDBMCB( what, "flags", 0 );


  if ( what->size != size )
    return mmPrintDBMCB( what, "size", size );

  i = mmGetIndex(size);

  if (! mmCheckPattern(what->front_pattern, MM_FRONT_PATTERN,
                       MM_NUMBER_OF_FRONT_PATTERNS))
    return mmPrintDBMCB(what , "front pattern", 0);
  if ((what->flags & MM_FREEFLAG)  && 
      ! mmCheckPattern((char*)&(what->data), MM_FREE_PATTERN, 0))
    return mmPrintDBMCB(what, "free pattern", 0);
  if (! mmCheckPattern((char*) &(what->data) + size, 
                       MM_BACK_PATTERN, DebugOffsetBack))
    return mmPrintDBMCB(what, "back pattern", 0);

  if ( ( what->next != NULL ) && ( what->next->prev != what ) )
  {
    mmPrintDBMCB( what->next, "next", 0 );
    what->next->prev=what;
    return mmPrintDBMCB( what, "chain:n", 0 );
  }
  if ( ( what->prev != NULL ) && ( what->prev->next != what ) )
  {
    mmPrintDBMCB( what->prev, "prev", 0 );
    what->prev->next=what;
    return mmPrintDBMCB( what, "chain:p", 0 );
  }
  if(ok==0)
  {
    return mmPrintDBMCB( what, "see above", 0 );
  }

  return 1;
}

static int mmCheckDBMCBList(DBMCB * list, int flags)
{
  assume(flags == MM_FREEFLAG || flags == MM_USEDFLAG);

  if (mmGListHasCycle(list,
                      (void*) &(mm_theDBfree.next) -
                      (void*) &(mm_theDBfree)))
  {
    fprintf(stderr, "%s list contains cycles\n",
            (flags == MM_FREEFLAG ? "Free" : "Used"));
    assume(0);
    return 0;
  }

  while (list != NULL)
  {
    if (! mmCheckSingleDBMCB(list, list->size, flags))
    {
      fprintf(stderr, "while check of %s list\n",
            (flags == MM_FREEFLAG ? "free" : "used"));
      return 0;
    }
    list = list->next;
  }

  return 1;
}

int mmTestMemory()
{
  if (mmCheckDBMCBList(mm_theDBused.next, MM_USEDFLAG))
    return mmCheckDBMCBList(mm_theDBfree.next, MM_FREEFLAG);
  return 0;
}

int mmTestHeaps()
{
  int i = -1;

  do
  {
    i++;
    if (! mmCheckHeap(&mm_theList[i])) return 0;
  }
  while (mmGetSize(i) < MAX_BLOCK_SIZE);

  return 1;
}
 
static int   mmIsAddrOnDList(DBMCB *what, DBMCB* list, int s)
{
  while (list != NULL)
  {
    if (list == what) return 1;
    list = list->next;
  }
  return 0;
}

static int mmCheckListContainment(DBMCB * what, int flags)
{
  if ( mmIsAddrOnDList((void*) what, mm_theDBused.next,
                       (void*) &(mm_theDBused.next) -
                       (void*) &(mm_theDBused)))
  {
    if (flags == MM_FREEFLAG)
      return mmPrintDBMCB(what, "Free flag and in used list", 0);
  }
  else
  {
    if (flags == MM_USEDFLAG)
      return mmPrintDBMCB(what, "Used flag but not in used list", 0);
  }
  
  if ( mmIsAddrOnDList((void*) what, mm_theDBfree.next,
                       (void*) &(mm_theDBfree.next) -
                       (void*) &(mm_theDBfree)))
  {
    if (flags == MM_USEDFLAG)
      return mmPrintDBMCB(what, "Used flag and in free list", 0);
  }
  else
  {
    if (flags == MM_FREEFLAG)
      return mmPrintDBMCB(what,"Free flag but not in free list", 0);
  }
  return 1;
}
  
int mmCheckDBMCB ( DBMCB * what, int size , int flags)
{

  assume(flags == MM_FREEFLAG || flags == MM_USEDFLAG);
  if (! mmCheckSingleDBMCB(what, size, flags))
    return 0;

  if (mm_MDEBUG > 1 && ! mmTestMemory()) return 0;
  
  if (mm_MDEBUG > 0 && ! mmCheckListContainment(what, flags)) return 0;

  return 1;
}

/**********************************************************************
 *
 * Test routines
 *
 **********************************************************************/
static BOOLEAN mmDBTestHeapBlockS(const void* adr, const memHeap heap,
                                  const size_t size,
                                  const char * fname, const int lineno )
{
  DBMCB * what;

  if ( adr == NULL || size == 0) 
    return TRUE;

  if (mm_MDEBUG > 2 && ! mmTestHeaps())
  {
    mmPrintFL( fname, lineno );
    return FALSE;
  }
    
  what = (DBMCB*)((char*)(adr) - DebugOffsetFront);

  if ( ! mmCheckDBMCB( what, size, MM_USEDFLAG) )
  {
    mmPrintFL( fname, lineno );
    return FALSE;
  }

  if (what->heap != heap)
  {
    mmPrintDBMCB(what, "Not from specified heap", 0);
    mmPrintFL( fname, lineno );
    return FALSE;
  }

#ifdef HEAP_DEBUG
  return
    mmDebugCheckHeapAddr(what, heap, MM_HEAP_ADDR_USED_FLAG, fname, lineno);
#else
  return TRUE;
#endif
}

BOOLEAN mmDBTestHeapBlock(const void* adr, const memHeap heap,
                          const char * fname, const int lineno )
{
  return mmDBTestHeapBlockS(adr, heap,
                            SizeFromRealSize(mmGetHeapBlockSize(heap)),
                            fname, lineno);
}
                            
BOOLEAN mmDBTestBlock(const void* adr, const size_t size, 
                      const char * fname, const int lineno)
{
  int i;
  DBMCB * what;
  
  if ( ( adr == NULL ) || ( size == 0 ) ) return TRUE;

  i = mmGetIndex(size);
  if (i < 0)
  {
    what = (DBMCB*)((char*)(adr) - DebugOffsetFront);
    if ( ! mmCheckDBMCB( what, size, MM_USEDFLAG) ) 
    {
      mmPrintFL( fname, lineno );
      return FALSE;
    }
    return TRUE;
  }
  else
  {
    return mmDBTestHeapBlockS(adr, &(mm_theList[i]), size, fname, lineno);
  }
}


BOOLEAN mmDBTest( const void* adr, const char * fname, const int lineno )
{
  if (adr!=NULL)
  {
    size_t l;
    adr = (size_t*)adr-1;
#ifdef ALIGN_8
    l= (adr<=mm_maxAddr) ? (*(size_t*)((int)adr&(~7))) :0;
#else
    l= (adr<=mm_maxAddr) ? (*(size_t*)((int)adr&(~3))) :0;
#endif
    return mmDBTestBlock( adr,l, fname, lineno );
  }
  return TRUE;
}

#endif /* MDEBUG */



