/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.c,v 1.16 1999-11-15 17:20:24 obachman Exp $ */
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
#ifndef HEAP_DEBUG
void mmDestroyHeap(memHeap *heap)
#else
static int mmPrintHeapError(char* msg, void* addr, memHeap heap,
                            const char* fn, int l);
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

memHeapPage mmAllocNewHeapPage(memHeap heap)
{
#ifndef HAVE_AUTOMATIC_GC
  if (heap->size > MAX_HEAP_CHUNK_SIZE)
  {
    heap->current = mmMallocFromSystem(heap->size);
    *((void**) heap->current) = NULL;
    heap->pages = (void*) ((int)heap->pages + 1);
#ifdef MDEBUG
    mmDBInitNewHeapPage(heap, NULL);
#endif
    return NULL;
  }
  else
#endif /* ! HAVE_AUTOMATIC_GC */
  {
    memHeapPage newpage;
    int i = 1, n = SIZE_OF_HEAP_PAGE / heap->size;
    void* tmp;
    if (n > 0) newpage = mmGetPage();
    else newpage = mmVallocFromSystem(heap->size + SIZE_OF_HEAP_PAGE_HEADER);
    
    assume(mmIsAddrPageAligned((void*) newpage));

#ifndef HAVE_AUTOMATIC_GC
    newpage->next = heap->pages;
    heap->pages = newpage;
    heap->current = (void*) (((char*)newpage) + SIZE_OF_HEAP_PAGE_HEADER);
    tmp = heap->current;
#else
    newpage->used_blocks = 0;
    newpage->current = (void*) (((char*)newpage) + SIZE_OF_HEAP_PAGE_HEADER);
    tmp = newpage->current;
#endif   /* ! HAVE_AUTOMATIC_GC */ 

    while (i < n)
    {
      *((void**)tmp) = (void*) (((char*)tmp) + heap->size);
      tmp = *((void**)tmp);
      i++;
    }
    *((void**)tmp) = NULL;
#ifdef MDEBUG
    mmDBInitNewHeapPage(heap, newpage);
#endif
    return newpage;
  }
}

#ifndef HAVE_AUTOMATIC_GC 
memHeap mmCreateHeap(size_t size)
{
  memHeap heap = (memHeap) AllocSizeOf(ip_memHeap);
  heap->current = NULL;
  heap->pages = NULL;
  heap->size = size;
  heap->last_gc= NULL;
  return heap;
}

#ifndef HEAP_DEBUG
void mmClearHeap(memHeap heap)
{
  memHeapPage page, next_page;

  page = heap->pages;

#if 0  
  while(page != NULL)
  {
    next_page = page->next;
    mmFreePage((void*) page);
    page = next_page;
  }
#endif
  heap->current = NULL;
  heap->pages = NULL;
  heap->last_gc = NULL;
}
#else
void mmDebugClearHeap(memHeap heap, const char* file, int line)
{
  mmCheckHeap(heap);
  mmGarbageCollectHeap(heap, 1);
  if (heap->current != NULL)
  {
    mmPrintHeapError("ClearHeap: Heap not empty", 0, heap, file, line);
  }
}
#endif

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

static void mmRemoveHeapBlocksOfPage(memHeap heap, memHeapPage hpage)
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
    return;
  }

  if (! strict && heap->current == heap->last_gc)
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
int mmNumberOfAllocatedHeapPages(memHeap heap)
{
  return mmListLength(heap->pages);
}

int mmNumberOfFreeHeapBlocks(memHeap heap)
{
  return mmListLength(heap->current);
}

#else /* HAVE_AUTOMATIC_GC */

int mmNumberOfAllocatedHeapPages(memHeap heap)
{
  return mmGListLength(heap->last_page, 
                       (void*) &(heap->last_page->prev) - (void*)heap->last_page);
}

int mmNumberOfFreeHeapBlocks(memHeap heap)
{
  int n = 0;
  memHeapPage page;
  
  if (heap->current_page == mmZeroPage) return 0;
  
  page = heap->last_page;
  do
  {
    n += mmListLength(page->current);
    if (page == heap->current_page) break;
    page = page->prev;
  }
  while (page != NULL);
  
  return n;
}
                       

memHeapPage mmGetNewCurrentPage(memHeap heap)
{
  assume(heap->current_page->current == NULL);
  if (heap->current_page == mmZeroPage)
  {
    memHeapPage new_page = mmAllocNewHeapPage(heap);
    new_page->next = mmZeroPage;
    new_page->prev = heap->last_page;
    if (heap->last_page != NULL)
      heap->last_page->next = new_page;
    heap->current_page = new_page;
    heap->last_page = new_page;
  }
  else 
  {
    heap->current_page->used_blocks = 1;
    if (heap->current_page != heap->last_page)
    {
      assume(heap->current_page->next->current != NULL);
      heap->current_page = heap->current_page->next;
    }
    else
    {
      memHeapPage new_page = mmAllocNewHeapPage(heap);
      new_page->next = mmZeroPage;
      new_page->prev = heap->last_page;
      new_page->prev->next = new_page;
      heap->current_page = new_page;
      heap->last_page = new_page;
    }
  }
#ifdef HEAP_DEBUG
  if (mm_HEAP_DEBUG > 1) mmDebugCheckHeap(heap, __FILE__, __LINE__);
#endif
  return heap->current_page;
}


static void mmTakeOutHeapPage(memHeapPage page, memHeap heap)
{
  if (heap->current_page == page) heap->current_page = page->next;
  if (heap->last_page == page) heap->last_page = page->prev;
  if (page->prev != NULL) page->prev->next = page->next;
  if (page->next != mmZeroPage) page->next->prev = page->prev;
}

static void mmInsertHeapPage(memHeapPage after, memHeapPage page, memHeap heap)
{
  if (after == NULL)
  {
    assume(heap->current_page == mmZeroPage);
    page->next = mmZeroPage;
    page->prev = NULL;
    heap->current_page = page;
    heap->last_page = page;
  }
  else
  {
    if (after == heap->last_page) heap->last_page = page;
    page->prev = after;
    page->next = after->next;
    if (after->next != mmZeroPage) after->next->prev = page;
    after->next = page;
    if (heap->current_page == mmZeroPage && page->current != NULL)
      heap->current_page = page;
  }
}

void mmRearrangeHeapPages(memHeapPage page, void* addr, memHeap heap)
{
  assume(page->used_blocks == 0);
  if (page->current != NULL)
  {
    // all blocks of page are now collected
    // get heap
    mmTakeOutHeapPage(page, heap);
    // page can be freed
    mmFreePage(page);
  }
  else
  {
    // page was full
    if (heap->max_blocks == 1) 
    {
      mmTakeOutHeapPage(page, heap);
      mmFreePage(page);
    }
    else
    {
      page->current = addr;
      page->used_blocks = heap->max_blocks - 1;
      *((void**)addr) = NULL;
      mmTakeOutHeapPage(page, heap);
#if defined(PAGE_BEFORE_CURRENT)
      if (heap->current_page->prev != NULL)
        mmInsertHeapPage(heap->current_page->prev, page);
      else
        mmInsertHeapPage(heap->current_page, page, heap);
      heap->current_page = page;
#elsif defined(PAGE_AFTER_CURRENT)
      mmInsertHeapPage(heap->current_page, page, heap);
#else
      mmInsertHeapPage(heap->last_page, page, heap);
#endif    
    }
  }
#ifdef HEAP_DEBUG
  if (mm_HEAP_DEBUG > 1) mmDebugCheckHeap(heap, __FILE__, __LINE__);
#endif
}


memHeap mmCreateHeap(size_t size)
{
  memHeap heap = (memHeap) AllocSizeOf(ip_memHeap);
  heap->current_page = mmZeroPage;
  heap->last_page = NULL;
  heap->size = size;
  heap->max_blocks = SIZE_OF_HEAP_PAGE / size;
  if (heap->max_blocks == 0) heap->max_blocks = 1;
  return heap;
}

#ifndef HEAP_DEBUG
void mmClearHeap(memHeap heap)
#else
void mmDebugClearHeap(memHeap heap, const char* file, int line)
#endif
{
#ifdef HEAP_DEBUG  
  mmCheckHeap(heap);
  if (heap->current_page != mmZeroPage)
  {
    mmPrintHeapError("ClearHeap: Heap not empty", NULL, heap, file, line);
  }
#endif
}

void  mmMergeHeap(memHeap into, memHeap what)
{/* TBC */}

#endif /* HAVE_AUTOMATIC_GC */
  
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

#ifndef HAVE_AUTOMATIC_GC
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


#else /* HAVE_AUTOMATIC_GC */


static int mmDebugCheckSingleHeapAddr(void* addr, memHeap heap,
                                      const char* fn, int l)
{
  memHeapPage page;
  
  if (heap == NULL)
  {
    fprintf(stderr, "NULL heap in %s:%d\n", fn, l);
    assume(0);
    return 0;
  }

  if (addr == NULL)
    return mmPrintHeapError("NULL addr", addr, heap, fn, l);

  if (! mmIsNotAddrOnFreePage(addr))
    return mmPrintHeapError("Addr on freed page", addr, heap, fn, l);

  page = mmGetPageOfAddr(addr);
  
  if (! mmIsAddrOnGList(page, heap->last_page, (void*) &page->prev - (void*) page))
    return mmPrintHeapError("addr not on page of this heap", 
                            addr, heap, fn, l);

  if ( (((long) (((char*) addr) - ((char*) page + SIZE_OF_HEAP_PAGE_HEADER)))
        % heap->size) != 0)
    return mmPrintHeapError("addr unaligned within heap", addr, heap, fn, l);
  return 1;
}


static int mmDebugCheckHeapAddrContainment(void* addr, memHeap heap, int flag,
                                           const char* fn, int l)
{
  memHeapPage page = heap->last_page;
  int where;
  if (heap->current_page != mmZeroPage) where = 1;
  else where = -1;
  
  
  while (page != NULL && mmGetPageOfAddr(addr) != page) 
  {
    if (page == heap->current_page) where = -1;
    page = page->prev;
  }

  if (page == NULL) mmPrintHeapError("addr not from heap", addr, heap, fn, l);
  
  if (flag == MM_HEAP_ADDR_FREE_FLAG)
  {
    if (where == -1 || ! mmIsAddrOnList(addr, heap->current_page->current))
      return mmPrintHeapError("free addr not on a page free list",
                              addr, heap, fn, l);
  }
  else if (flag == MM_HEAP_ADDR_USED_FLAG)
  {
    if (where == 1 &&  mmIsAddrOnList(addr, heap->current_page->current))
      return mmPrintHeapError("used addr on a page free list",
                              addr, heap, fn, l);
  }
  return 1;
}

int mmDebugCheckHeap(memHeap heap, const char* fn, int l)
{
  void* p;
  int where;
  memHeapPage page;
  
  if (heap->last_page == NULL)
  {
    if (heap->current_page != mmZeroPage)
      return mmPrintHeapError("last_page out of sunc", NULL, heap, fn, l);
    return 1;
  }
  
  if (heap->last_page != mmGetPageOfAddr(heap->last_page))
    return mmPrintHeapError("last_page not page aligned", heap->last_page,
                            heap, fn, l);

  if (heap->current_page != mmZeroPage && 
      mmGetPageOfAddr(heap->current_page) != heap->current_page)
    return mmPrintHeapError("current_page not page aligned", 
                            heap->current_page,
                            heap, fn, l);
    

  if (heap->last_page->next != mmZeroPage) 
    return mmPrintHeapError("last_page out of sunc", NULL, heap, fn, l);

  /* check page list for cycles */
  if (mmGListHasCycle(heap->last_page, (void*) &page->prev - (void*) page))
    return mmPrintHeapError("prev chain of heap has cycles", NULL, heap, fn,l);
  page = heap->last_page;
  while (page->prev != NULL) page = page->prev;
  if (mmGListHasCycle(heap->last_page, (void*) &page->next - (void*) page))
    return mmPrintHeapError("next chain of heap has cycles", NULL, heap, fn,l);

  /* check prev/next relations of pages */
  page = heap->last_page;
  do
  {
    if (page != mmGetPageOfAddr(page))
      return mmPrintHeapError("page not aligned", page, heap, fn, l);
    if (page->prev != NULL && page->prev->next != page)
      return mmPrintHeapError("page list damaged (prev)", page, heap, fn, l);
    if (page->next != mmZeroPage && page->next->prev != page)
      return mmPrintHeapError("page list damaged (next)", page, heap, fn, l);
    page = page->prev;
  }
  while (page != NULL);

  /* check that current_page is on list of pages */
  if (heap->current_page != mmZeroPage && 
      ! mmIsAddrOnGList(heap->current_page, heap->last_page, 
                        (void*) &page->prev - (void*) page))
    return mmPrintHeapError("current_page not in page list", 
                            heap->current_page, heap, fn, l);

  /* now check single pages */
  page = heap->last_page;
  if (heap->current_page != mmZeroPage) where = 1;
  else where = -1;
  while (page != NULL)
  {
    if (page == heap->current_page) where = 0;
    if (where > 0 && page->used_blocks <= 0) 
      return mmPrintHeapError("used_blocks of page <= 0", page, heap, fn, l);

    if (where >= 0)
    {
      if (mmListHasCycle(page->current))
        return mmPrintHeapError("current of page has cycle", page, heap, fn, l);

      if (heap->max_blocks - page->used_blocks != mmListLength(page->current))
      return mmPrintHeapError("used_blocks of fpage out of sync", 
                              page, heap, fn, l);
    }
    else
    {
      if (page->used_blocks != 1)
        return mmPrintHeapError("used_blocks of upage not 1", 
                                page, heap, fn, l);  
    }
    
    if (mm_HEAP_DEBUG > 2 && where >= 0)
    {
      p = page->current;
      if (mmListHasCycle(p))
        return mmPrintHeapError("current list of fpage has cycle", page,
                                heap, fn, l);
      while (p != NULL)
      {
        if (mmGetPageOfAddr(p) != page)
          return mmPrintHeapError("addr not on heap page", p, heap, fn, l);
        p = *((void**) p);
      }
    }
    if (where == 0) where = -1;
    page = page->prev;
  }
  return 1;
}
#endif /* ! HAVE_AUTOMATIC_GC */


int mmDebugCheckHeapAddr(void* addr, memHeap heap, int flag,
                         const char* fn, int l)
{
  if (mm_HEAP_DEBUG > 2 && ! mmDebugCheckHeap(heap, fn, l)) return 0;

  if (! mmDebugCheckSingleHeapAddr(addr, heap, fn, l)) return 0;

  return mmDebugCheckHeapAddrContainment(addr, heap, flag, fn, l);
}
  
void * mmDebugAllocHeap(memHeap heap, const char* fn, int l)
{
  void* res;
  
  if (mm_HEAP_DEBUG > 1 && ! mmDebugCheckHeap(heap, fn, l))
    return NULL;

  _mmAllocHeap(res, heap, void*);

  if (mm_HEAP_DEBUG > 2)
    mmDebugCheckHeapAddr(res, heap, MM_HEAP_ADDR_USED_FLAG, fn, l);

  return res;
}
  
void  mmDebugFreeHeap(void* addr, memHeap heap, const char* fn, int l)
{
  if (mm_HEAP_DEBUG > 0)
    mmDebugCheckHeapAddr(addr, heap, MM_HEAP_ADDR_USED_FLAG, fn, l);
  
  _mmFreeHeap(addr, heap);

  if (mm_HEAP_DEBUG > 2)
    mmDebugCheckHeapAddr(addr, heap, MM_HEAP_ADDR_FREE_FLAG, fn, l);
}

#endif /* HEAP_DEBUG */
