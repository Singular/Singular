/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.c,v 1.7 1998-12-02 13:57:33 obachman Exp $ */

#include "mod2.h"

#include <stdio.h>

#include "mmheap.h"
#include "mmpage.h"
#include "mmprivate.h"
#include "mmemory.h"



memHeap mmCreateHeap(size_t size)
{
  memHeap heap = (memHeap) Alloc(sizeof(struct sip_memHeap));
  mmInitHeap(heap, size);
  return heap;
}

void mmDestroyHeap(memHeap *heap)
{
  mmClearHeap(*heap);
  Free(*heap, sizeof(struct sip_memHeap));
  *heap = NULL;
}
  
void mmInitHeap(memHeap heap, size_t size)
{
  heap->current = NULL;
  heap->pages = NULL;
  heap->size = size;
}

void mmClearHeap(memHeap heap)
{
  void* page ;
  void* next_page;

  mmCheckHeap(heap);
  
  page = heap->pages;
  
  while(page != NULL)
  {
    next_page = mmGetPageOfAddr(*((void**) page));
    mmFreePage(page);
    page = next_page;
  }
  heap->current = NULL;
  heap->pages = NULL;
}

void mmAllocNewHeapPage(memHeap heap)
{
  void* newpage = mmGetPage();
  int i = 1, n = SIZE_OF_HEAP_PAGE / heap->size;
  assume(heap != NULL && heap->current == NULL);
  assume(mmIsAddrPageAligned(newpage));

  *((void**)newpage) = heap->pages;
  heap->pages = newpage;
  
  heap->current = (void*) (((char*)newpage) + SIZEOF_VOIDP);
  newpage = heap->current;

  while (i < n)
  {
    *((void**)newpage) = (void*) (((char*)newpage) + heap->size);
    newpage = *((void**)newpage);
    i++;
  }
  *((void**)newpage) = NULL;
#ifdef MDEBUG
  mmDBInitNewHeapPage(heap);
#endif
}

void mmMergeHeap(memHeap into, memHeap what)
{
  void* last;
  
  assume(into != NULL && what != NULL && into->size == what->size);

  if (into == what) return;

  if (what->pages != NULL)
  {
    last = mmListLast(what->pages);
    *((void**) last) = into->pages;
    into->pages = what->pages;

    if (what->current != NULL)
    {
      last = mmListLast(what->current);
      *((void**) last) = into->current;
      into->current = what->current;
    }
  }
  
  what->pages = NULL;
  what->current = NULL;

  mmCheckHeap(into);
}

#ifdef HEAP_DEBUG
int mm_HEAP_DEBUG = HEAP_DEBUG;

static int mmPrintHeapError(char* msg, void* addr, memHeap heap,
                            const char* fn, int l)
{
  fprintf(stderr, "\n%s: occured for addr:%p heap:%p of size %d in %s:%d\n",
          msg, addr, (void*) heap, heap->size, fn, l);
  assume(0);
  return 0;
}

static int mmDebugCheckSingleHeapAddr(void* addr, memHeap heap,
                                      const char* fn, int l)
{
  void* page;
  
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
  
#ifdef HAVE_PAGE_ALIGNMENT
  page = mmGetPageOfAddr(addr);

  if (! mmIsAddrOnList(page, heap->pages))
    return mmPrintHeapError("addr not on heap page", addr, heap, fn, l);

  if ( (((long) (((char*) addr) - ((char*) page + SIZE_OF_HEAP_PAGE_HEADER)))
        % heap->size) != 0)
    return mmPrintHeapError("addr unaligned within heap",
                            addr, heap, fn, l);
#endif
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
  
  if (mmListHasCycle(heap->pages))
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
    return 0;
  
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
