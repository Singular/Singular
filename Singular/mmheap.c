/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.c,v 1.14 1999-10-25 08:32:16 obachman Exp $ */
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

#else /* HAVE_AUTOMATIC_GC */

memHeapPage mmGetNewCurrentPage(memHeap heap)
{
  if (heap->current_page == mmZeroPage)
  {
    memHeapPage new_page = mmAllocNewHeapPage(heap);
    new_page->next = mmZeroPage;
    new_page->prev = NULL;
    heap->current_page = new_page;
    return new_page;
  }
  else 
  {
    heap->current_page->used_blocks = 1;
    heap->current_page->current = (void*) heap;
    if (heap->current_page->next->current != NULL)
    {
      heap->current_page = heap->current_page->next;
      return heap->current_page;
    }
    else
    {
      memHeapPage new_page = mmAllocNewHeapPage(heap);
      new_page->next = mmZeroPage;
      heap->current_page->next = new_page;
      new_page->previous = heap->current_page;
      heap->current_page = new_page;
      return new_page;
    }
  }
}


static void mmTakeOutHeapPage(memHeapPage page, memHeap heap)
{
  if (heap->current_page == page) heap->current_page = page->next;
  if (page->prev != NULL) page->prev->next = page->next;
  if (page->next != mmZeroPage) page->next->prev = page->prev;
}

static void mmInsertHeapPage(memHeapPage after, memHeapPage page, memHeap heap)
{
  if (after == mmZeroPage)
  {
    page->next = mmZeroPage;
    page->prev = NULL;
    heap->current_page = page;
  }
  else
  {
    page->next = after->next;
    page->prev = after;
    if (after->next != mmZeroPage) after->next->prev = page;
    after->next page;
  }
}

void mmRearrangeHeapPages(memHeapPage page, void* addr)
{
  memHeap heap;
  if (mmGetPageOfAddr(page->current) == page)
  {
    // get heap
    heap = (memHeap) *(page->current + 1);
     mmTakeOutHeapPage(page, heap);
    // page can be freed
    mmFreePage(page);
  }
  else
  {
    // page was full
    heap = (memHeap) page->current;
    page->current = addr;
    page->used_blocks = (SIZE_OF_HEAP_PAGE / heap->block) - 1;
    if (page->used_blocks <= 0) page->used_blocks = 1;
    *(page->current + 1) = heap;
    mmTakeOutHeapPage(page, heap);
#if defined(PAGE_BEFORE_CURRENT)
    if (heap->current_page->prev != NULL)
    {
      mmInsertHeapPage(heap->current_page->prev, page);
    }
    else
    {
      mmInsertHeapPage(heap->current, page, heap);
    }
    heap->current = page;
#elsif defined(PAGE_AFTER_CURRENT)
    mmInsertHeapPage(heap->current, page, heap);
#else
    {
      memPage lastpage = heap->current_page;
      if (lastpage != mmZeroPage) 
      {
        memPage lastpage = heap->current_page;
        while (lastpage->next != mmZeroPage)
        {
          lastpage = lastpage->next;
        }
      }
      mmInsertHeapPage(lastpage, page, heap);
    }
#endif    
  }
}

  
    
    
    

  if (page->used_blocks == 0)
  {
    if (heap->current_page == page) 
      heap->current_page = heap->current_page->next;
    page->prev->next = page->next;
    page->next->prev = page->prev;
    if (SIZE_OF_HEAP_PAGE / heap->size > 0)
      mmFreePage(page);
    else
      mmVfreeToSystem(page, heap->size + SIZE_OF_HEAP_PAGE_HEADER);
  }
  else
  {
    memHeapPage next_page = page->next;

    if (heap->current_page->current == NULL)
      heap->current_page = page;
    else if (heap->current_page == page && next_page->current != NULL)
      heap->current_page = next_page;

    while (page->used_blocks < next_page->used_blocks) 
      next_page = next_page->next;

    if (next_page != page->next)
    {
      page->prev->next = page->next;
      page->next->prev = page->prev;
      page->prev = next_page;
      page->next = next_page->next;
      next_page->next->prev = page;
      next_page->next = page;
    }
  }
  mmCheckHeap(heap);
}

memHeap mmCreateHeap(size_t size)
{
  memHeap heap = (memHeap) AllocSizeOf(ip_memHeap);
  heap->current_page = mmZeroPage;
  heap->size = size;
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
  if (heap->current_page != mmZeroPage &&  
      (heap->current_page->used_blocks != 0 || 
       heap->current_page->next != heap->current_page ||
       heap->current_page->prev != heap->current_page))
    mmPrintHeapError("ClearHeap: Heap not empty", NULL, heap, file, line);
#endif
  if (heap->current_page != mmZeroPage)
  {
    if (SIZE_OF_HEAP_PAGE / heap->size > 0)
      mmFreePage(heap->current_page);
    else
      mmVfreeToSystem(heap->current_page, 
                      heap->size + SIZE_OF_HEAP_PAGE_HEADER);
  }
  heap->current_page = mmZeroPage;
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

  if (! mmIsNotAddrOnFreePage(addr))
    return mmPrintHeapError("Addr on freed page", addr, heap, fn, l);

#if 0  
  page = mmGetPageOfAddr(addr);
  
  if (! mmIsAddrOnList(page, heap->pages))
    return mmPrintHeapError("addr not on heap page", addr, heap, fn, l);

  if ( (((long) (((char*) addr) - ((char*) page + SIZE_OF_HEAP_PAGE_HEADER)))
        % heap->size) != 0)
    return mmPrintHeapError("addr unaligned within heap", addr, heap, fn, l);
  hpage = (memHeapPage) page;
#endif
  return 1;
}


static int mmDebugCheckHeapAddrContainment(void* addr, memHeap heap, int flag,
                                           const char* fn, int l)
{
  if (flag == MM_HEAP_ADDR_FREE_FLAG)
  {
    if (! mmIsAddrOnList(addr, heap->current_page->current))
      return mmPrintHeapError("addr not on heap free list",
                              addr, heap, fn, l);
  }
  else if (flag == MM_HEAP_ADDR_USED_FLAG)
  {
    if (mmIsAddrOnList(addr, heap->current_page->current))
      return mmPrintHeapError("used addr on heap free list",
                              addr, heap, fn, l);
  }
  return 1;
}

int mmDebugCheckHeap(memHeap heap, const char* fn, int l)
{
  void* p;
  memHeapPage page;
  long prev_used_blocks;
  
  long has_changed = FALSE;
  long max_blocks = SIZE_OF_HEAP_PAGE / heap->size;
  if (max_blocks == 0) max_blocks = 1;
  
  if (heap->current_page == mmZeroPage) return 1;

  // check used_blocks
  page = heap->current_page;
  prev_used_blocks = heap->current_page->prev->used_blocks;
  do
  {
    if (page->prev->next != page || page->next->prev != page)
      return mmPrintHeapError("page list damaged",  NULL, heap, fn, l);
    if (page->used_blocks == 0) 
      return mmPrintHeapError("No used blocks",  NULL, heap, fn, l);
    if (page->used_blocks < 0 || page->used_blocks > max_blocks)
      return mmPrintHeapError("used_blocks out of bounds", NULL, heap, fn, l);
    if (max_blocks - mmListLength(page->current) != page->used_blocks)
    {
      fprintf(stderr, "used_blocks wrong: should be %ld, is %ld\n", 
              max_blocks - mmListLength(page->current), page->used_blocks);
      return mmPrintHeapError("", NULL, heap, fn, l);
    }
    if (page->used_blocks > heap->current_page->used_blocks)
    {
      fprintf(stderr, "used_blocks %ld larger than %ld", page->used_blocks,
              heap->current_page->used_blocks);
      return mmPrintHeapError("", NULL, heap, fn, l);
    }
    if (prev_used_blocks < page->used_blocks)
    {
      if (has_changed)
      {
        fprintf(stderr, "pages not decr order with change at %ld and %ld\n",
                has_changed, page->used_blocks);
        return mmPrintHeapError("", NULL, heap, fn, l);
      }
      else
      {
        has_changed = page->used_blocks;
      }

      p = page->current;
      while (p != NULL)
      {
        if (mmGetPageOfAddr(p) != page)
          return mmPrintHeapError("addr in freelist not on page",
                                  p, heap, fn, l);
        if (!mmDebugCheckSingleHeapAddr(p, heap, fn, l)  ||
            !mmDebugCheckHeapAddrContainment(p, heap, MM_HEAP_ADDR_FREE_FLAG,
                                             fn, l))
          return 0;
        p = *((void**) p);
      }
    }
    prev_used_blocks = page->used_blocks;
    page = page->next;
  }
  while(page != heap->current_page);

  return 1;
}
#endif /* ! HAVE_AUTOMATIC_GC */


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
