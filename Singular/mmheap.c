/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.c,v 1.11 1999-10-14 14:27:19 obachman Exp $ */
#include <stdio.h>
#include "mod2.h"
#include "structs.h"
#include "mmpage.h"
#include "mmemory.h"
#include "mmprivate.h"



/*****************************************************************
 *
 * Implementation of basic routines
 *
 *****************************************************************/
void mmInitHeap(memHeap heap, size_t size)
{
  heap->current = NULL;
  heap->pages = NULL;
  heap->size = size;
  heap->last_gc= NULL;
}

memHeap mmCreateHeap(size_t size)
{
  memHeap heap = (memHeap) AllocSizeOf(ip_memHeap);
  mmInitHeap(heap, size);
  return heap;
}

#ifndef HEAP_DEBUG
void mmDestroyHeap(memHeap *heap)
#else
void mmDebugDestroyHeap(memHeap *heap, const char* file, int line)
#endif
{
#ifndef HEAP_DEBUG
  mmClearHeap(*heap);
#else
  mmDebugClearHeap(*heap, file, line);
#endif 
  FreeSizeOf(*heap, ip_memHeap);
  *heap = NULL;
}

#ifndef HEAP_DEBUG
void mmClearHeap(memHeap heap)
#else
void _mmClearHeap(memHeap heap)
#endif
{
  memHeapPage page, next_page;

  page = heap->pages;
  
  while(page != NULL)
  {
    next_page = page->next;
    mmFreePage((void*) page);
    page = next_page;
  }
  heap->current = NULL;
  heap->pages = NULL;
  heap->last_gc = NULL;
}

void mmAllocNewHeapPage(memHeap heap)
{
  if (heap->size > MAX_HEAP_CHUNK_SIZE)
  {
    heap->current = mmMallocFromSystem(heap->size);
    *((void**) heap->current) = NULL;
    heap->pages = (void*) ((int)heap->pages + 1);
  }
  else
  {
    memHeapPage newpage = (memHeapPage) mmGetPage();
    int i = 1, n = SIZE_OF_HEAP_PAGE / heap->size;
    void* tmp;
    assume(heap != NULL && newpage != NULL && heap->current == NULL);
    assume(mmIsAddrPageAligned((void*) newpage));

    newpage->next = heap->pages;
    heap->pages = newpage;
  
    heap->current = (void*) (((char*)newpage) + SIZE_OF_HEAP_PAGE_HEADER);
    tmp = heap->current;

    while (i < n)
    {
      *((void**)tmp) = (void*) (((char*)tmp) + heap->size);
      tmp = *((void**)tmp);
      i++;
    }
    *((void**)tmp) = NULL;
  }
#ifdef MDEBUG
    mmDBInitNewHeapPage(heap);
#endif
}

void mmMergeHeap(memHeap into, memHeap what)
{
  void* last;
  
  assume(into != NULL && what != NULL && into->size == what->size);

  if (into == what) return;
#ifdef MDEBUG
  mmDBSetHeapsOfBlocks(what, into);
#endif  

  if (into->size > MAX_HEAP_CHUNK_SIZE)
  {
    into->pages = (void*) ((int) what->pages + (int) into->pages);
  }
  else
  {
    if (what->pages != NULL)
    {
      last = mmListLast(what->pages);
      *((void**) last) = into->pages;
      into->pages = what->pages;
    }
  }

  if (what->current != NULL)
  {
    last = mmListLast(what->current);
    *((void**) last) = into->current;
    into->current = what->current;
  }
  
  what->pages = NULL;
  what->current = NULL;

  mmCheckHeap(into);
}

void mmRemoveFromCurrentHeap(memHeap heap, void* addr)
{
  heap->current = mmRemoveFromList((void*) heap->current, addr);
}

void mmRemoveHeapBlocksOfPage(memHeap heap, memHeapPage hpage)
{
  void *prev = NULL, *new_current = NULL, *current = heap->current;
  int nblocks = SIZE_OF_HEAP_PAGE / heap->size;
  
#ifdef HAVE_ASSUME
  int l = mmListLength(current);
#endif
  assume(nblocks > 0);

  while (mmIsAddrOnPage(current, hpage))
  {
#ifdef MDEBUG
    mmTakeOutDBMCB((DBMCB*) current);
#endif    
    if (--(nblocks) == 0) 
    {
      new_current = *((void**) current);
      goto Finish;
    }
    current = *((void**) current);
  }
  
  new_current = current;
  
  while (1)
  {
    assume(mmListLength(new_current) == 
           l - (SIZE_OF_HEAP_PAGE / heap->size) + nblocks);
    prev = current;
    current = *((void**) current);
    
    while (! mmIsAddrOnPage(current, hpage))
    {
      prev = current;
      current = *((void**) current);
    }
    
#ifdef MDEBUG
    mmTakeOutDBMCB((DBMCB*) current);
#endif    
    if (--(nblocks) == 0) goto Finish;

    current = *((void**) current);
    while (mmIsAddrOnPage(current, hpage))
    {
#ifdef MDEBUG
      mmTakeOutDBMCB((DBMCB*) current);
#endif    
      if (--(nblocks) == 0) goto Finish;
      current = *((void**) current);
    }
    *((void**) prev) = current;
  }

  Finish:
  heap->current = new_current;
  if (prev != NULL)
    *((void**) prev) = *((void**) current);
  heap->pages = mmRemoveFromList(heap->pages, hpage);
  mmFreePage(hpage);
  assume(mmListLength(heap->current) == l -  (SIZE_OF_HEAP_PAGE / heap->size));
  mmCheckHeap(heap);
}

void mmGarbageCollectHeap(memHeap heap, int strict)
{
  if (heap->size > MAX_HEAP_CHUNK_SIZE)
  {
    void* current = heap->current;
    void* next;
    while (current != NULL)
    {
      next = *((void**) current);
#ifdef MDEBUG
      mmTakeOutDBMCB((DBMCB*) current);
#endif      
      mmFreeToSystem(current, heap->size);
      current = next;
      heap->pages = (void*) ((int) heap->pages - 1);
    }
    heap->current = NULL;
    heap->last_gc = NULL;
  }
  else if (! strict && heap->current == heap->last_gc)
  {
    return;
  }
  else
  {
    memHeapPage hpage = heap->pages;
    void* current = heap->current;
    int nblocks = SIZE_OF_HEAP_PAGE / (heap->size);
  
    assume(heap != NULL);
    mmCheckHeap(heap);

    while (hpage != NULL)
    {
      hpage->counter = nblocks;
      hpage = hpage->next;
    }

    while (current != NULL)
    {
      hpage = mmGetHeapPageOfAddr(current);
      current = *((void**) current);
      if (--(hpage->counter) == 0)
        mmRemoveHeapBlocksOfPage(heap, hpage);
    }
    heap->last_gc = heap->current;
  }
}

#ifdef HEAP_DEBUG
int mm_HEAP_DEBUG = HEAP_DEBUG;

static int mmPrintHeapError(char* msg, void* addr, memHeap heap,
                            const char* fn, int l)
{
  fprintf(stderr, "\n%s: occured for addr:%p heap:%p of size %ld in %s:%d\n",
          msg, addr, (void*) heap, heap->size, fn, l);
  assume(0);
  return 0;
}

void mmDebugClearHeap(memHeap heap, const char* file, int line)
{
  mmCheckHeap(heap);
  mmGarbageCollectHeap(heap, 1);
  if (heap->current != NULL)
  {
    mmPrintHeapError("ClearHeap: Heap not empty", 0, heap, file, line);
  }
}

static int mmDebugCheckSingleHeapAddr(void* addr, memHeap heap,
                                      const char* fn, int l)
{
  void* page;
  memHeapPage hpage;
  
  if (heap == NULL)
  {
    fprintf(stderr, "NULL heap in %s:%d\n", fn, l);
    assume(0);
    return 0;
  }

  if (addr == NULL)
    return mmPrintHeapError("NULL addr", addr, heap, fn, l);

  if (heap->size > MAX_HEAP_CHUNK_SIZE) return 1;

  if (! mmIsNotAddrOnFreePage(addr))
    return mmPrintHeapError("Addr on freed page", addr, heap, fn, l);
  
  page = mmGetPageOfAddr(addr);
  
  if (! mmIsAddrOnList(page, heap->pages))
    return mmPrintHeapError("addr not on heap page", addr, heap, fn, l);

  if ( (((long) (((char*) addr) - ((char*) page + SIZE_OF_HEAP_PAGE_HEADER)))
        % heap->size) != 0)
    return mmPrintHeapError("addr unaligned within heap", addr, heap, fn, l);
  hpage = (memHeapPage) page;
  return 1;
}


static int mmDebugCheckHeapAddrContainment(void* addr, memHeap heap, int flag,
                                           const char* fn, int l)
{
  if (flag == MM_HEAP_ADDR_FREE_FLAG)
  {
    if (! mmIsAddrOnList(addr, heap->current))
      return mmPrintHeapError("addr not on heap free list",
                              addr, heap, fn, l);
  }
  else if (flag == MM_HEAP_ADDR_USED_FLAG)
  {
    if (mmIsAddrOnList(addr, heap->current))
      return mmPrintHeapError("used addr on heap free list",
                              addr, heap, fn, l);
  }
  return 1;
}

int mmDebugCheckHeap(memHeap heap, const char* fn, int l)
{
  void* p;
  
  if (heap->size <= MAX_HEAP_CHUNK_SIZE && mmListHasCycle(heap->pages))
    return mmPrintHeapError("heap pages list has cycles",
                            NULL, heap, fn, l);

  if (mmListHasCycle(heap->current))
    return mmPrintHeapError("heap current list has cycles",
                            NULL, heap, fn, l);
    
  p = heap->current;
  while (p != NULL)
  {
    if (!mmDebugCheckSingleHeapAddr(p, heap, fn, l)  ||
        !mmDebugCheckHeapAddrContainment(p, heap, MM_HEAP_ADDR_FREE_FLAG,
                                         fn, l))
      return 0;
    p = *((void**) p);
  }
  return 1;
}

int mmDebugCheckHeapAddr(void* addr, memHeap heap, int flag,
                         const char* fn, int l)
{
  if (! mmDebugCheckSingleHeapAddr(addr, heap, fn, l)) return 0;

  if (mm_HEAP_DEBUG > 1)
  {
    if (! mmDebugCheckHeap(heap, fn, l)) return 0;
  }

  if (mm_HEAP_DEBUG > 0)
    return mmDebugCheckHeapAddrContainment(addr, heap, flag, fn, l);
  else
    return 1;
}
  
void * mmDebugAllocHeap(memHeap heap, const char* fn, int l)
{
  void* res;

  if (mm_HEAP_DEBUG > 1 && ! mmDebugCheckHeap(heap, fn, l))
    return NULL;
  _mmAllocHeap(res, heap);

  mmDebugCheckSingleHeapAddr(res, heap, fn, l);
  
  if (mm_HEAP_DEBUG > 0)
    mmDebugCheckHeapAddrContainment(res, heap, MM_HEAP_ADDR_USED_FLAG,
                                    fn, l);
  return res;
}
  
void  mmDebugFreeHeap(void* addr, memHeap heap, const char* fn, int l)
{
  mmDebugCheckHeapAddr(addr, heap, MM_HEAP_ADDR_USED_FLAG, fn, l);

  _mmFreeHeap(addr, heap);
}

#endif
