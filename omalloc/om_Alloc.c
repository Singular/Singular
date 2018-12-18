/*******************************************************************
 *  File:    omAlloc.c
 *  Purpose: implementation of main omalloc functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_ALLOC_C
#define OM_ALLOC_C

#include "omalloc.h"

#ifdef HAVE_OMALLOC
/*******************************************************************
 *
 *  global variables
 *
 *******************************************************************/

omBinPage_t om_ZeroPage[] = {{0, NULL, NULL, NULL, NULL}};
omSpecBin om_SpecBin = NULL;

#include "omalloc/omTables.inc"


/*******************************************************************
 *
 *  Local stuff
 *
 *******************************************************************/

/* Get new page and initialize */
static omBinPage omAllocNewBinPage(omBin bin)
{
  omBinPage newpage;
  void* tmp;
  int i = 1;

  if (bin->max_blocks > 0) newpage = omAllocBinPage();
  else newpage = omAllocBinPages(-bin->max_blocks);

  omAssume(omIsAddrPageAligned((void*) newpage));

  omSetTopBinAndStickyOfPage(newpage, bin, bin->sticky);
  newpage->used_blocks = -1;
  newpage->current = (void*) (((char*)newpage) + SIZEOF_OM_BIN_PAGE_HEADER);
  tmp = newpage->current;
  while (i < bin->max_blocks)
  {
    tmp = *((void**)tmp) = ((void**) tmp) + bin->sizeW;
    i++;
  }
  *((void**)tmp) = NULL;
  omAssume(omListLength(newpage->current) ==
           (bin->max_blocks > 1 ? bin->max_blocks : 1));
  return newpage;
}

/* primitives for handling of list of pages */
OM_INLINE_LOCAL void omTakeOutBinPage(omBinPage page, omBin bin)
{
  if (bin->current_page == page)
  {
    if (page->next == NULL)
    {
      if (page->prev == NULL)
      {
        omAssume(bin->last_page == page);
        bin->last_page = NULL;
        bin->current_page = om_ZeroPage;
        return;
      }
      bin->current_page = page->prev;
    }
    else
      bin->current_page = page->next;
  }
  if (bin->last_page == page)
  {
    omAssume(page->prev != NULL && page->next == NULL);
    bin->last_page = page->prev;
  }
  else
  {
    omAssume(page->next != NULL);
    page->next->prev = page->prev;
  }
  if (page->prev != NULL) page->prev->next = page->next;
}

OM_INLINE_LOCAL void omInsertBinPage(omBinPage after, omBinPage page, omBin bin)
{
  if (bin->current_page == om_ZeroPage)
  {
    omAssume(bin->last_page == NULL);
    page->next = NULL;
    page->prev = NULL;
    bin->current_page = page;
    bin->last_page = page;
  }
  else
  {
    omAssume(after != NULL && bin->last_page != NULL);
    if (after == bin->last_page)
    {
      bin->last_page = page;
    }
    else
    {
      omAssume(after->next != NULL);
      after->next->prev = page;
    }
    page->next = after->next;
    after->next = page;
    page->prev = after;
  }
}

/* bin->current_page is empty, get new bin->current_page, return addr*/
void* omAllocBinFromFullPage(omBin bin)
{
  void* addr;
  omBinPage newpage;
  omAssume(bin->current_page->current == NULL);

  if (bin->current_page != om_ZeroPage)
  {
    omAssume(bin->last_page != NULL);
    /* Set this to zero, but preserve the first bit,
       so that tracking works */
#ifdef OM_HAVE_TRACK
    bin->current_page->used_blocks &= (((unsigned long) 1) << (BIT_SIZEOF_LONG -1));
#else
    bin->current_page->used_blocks = 0;
#endif
  }

  if (!bin->sticky && bin->current_page->next != NULL)
  {
    omAssume(bin->current_page->next->current != NULL);
    newpage = bin->current_page->next;
  }
  else
  {
    // need to Allocate new page
    newpage = omAllocNewBinPage(bin);
    omInsertBinPage(bin->current_page, newpage, bin);
  }

  bin->current_page = newpage;
  omAssume(newpage != NULL && newpage != om_ZeroPage &&
           newpage->current != NULL);
  __omTypeAllocFromNonEmptyPage(void*, addr, newpage);
  return addr;
}


/* page->used_blocks <= 0, so, either free page or reallocate to
   the right of current_page */
/*
 * Now: there are three different strategies here, on what to do with
 * pages which were full and now have a free block:
 * 1.) Insert at the end (default)
 * 2.) Insert after current_page  => #define PAGE_AFTER_CURRENT
 * 3.) Let it be new current_page => #define PAGE_BEFORE_CURRENT
 * Still need to try out which is best
 */
void  omFreeToPageFault(omBinPage page, void* addr)
{
  omBin bin;
  omAssume(page->used_blocks <= 0L);

#ifdef OM_HAVE_TRACK
  if (page->used_blocks < 0L)
  {
    omFreeTrackAddr(addr);
    return;
  }
#endif

  bin = omGetBinOfPage(page);
  if ((page->current != NULL) || (bin->max_blocks <= 1))
  {
    // all blocks of page are now collected
    omTakeOutBinPage(page, bin);
    // page can be freed
    if (bin->max_blocks > 0)
      omFreeBinPage(page);
    else
      omFreeBinPages(page, - bin->max_blocks);
#ifdef OM_HAVE_TRACK
    om_JustFreedPage = page;
#endif
  }
  else
  {
    // page was full
    page->current = addr;
    page->used_blocks = bin->max_blocks - 2;
    *((void**)addr) = NULL;

    omTakeOutBinPage(page, bin);
#if defined(PAGE_BEFORE_CURRENT)
    if (bin->current_page->prev != NULL)
      omInsertBinPage(bin->current_page->prev, page);
    else
      omInsertBinPage(bin->current_page, page, bin);
    bin->current_page = page;
#else
#  if defined(PAGE_AFTER_CURRENT)
      omInsertBinPage(bin->current_page, page, bin);
#  else
      omInsertBinPage(bin->last_page, page, bin);
#  endif
#endif
  }
}

/*******************************************************************
 *
 *  DoRealloc
 *
 *******************************************************************/
#ifdef OM_ALIGNMNET_NEEDS_WORK
#define DO_ZERO(flag) (flag & 1)
#else
#define DO_ZERO(flag)    flag
#endif

void* omDoRealloc(void* old_addr, size_t new_size, int flag)
{
  void* new_addr;

  if (!omIsBinPageAddr(old_addr) && new_size > OM_MAX_BLOCK_SIZE)
  {
    if (DO_ZERO(flag))
      return omRealloc0Large(old_addr, new_size);
    else
      return omReallocLarge(old_addr, new_size);
  }
  else
  {
    size_t old_size = omSizeOfAddr(old_addr);
    size_t min_size;

    omAssume(OM_IS_ALIGNED(old_addr));

#ifdef OM_ALIGNMENT_NEEDS_WORK
    if (flag & 2)
      __omTypeAllocAligned(void*, new_addr, new_size);
    else
#endif
      __omTypeAlloc(void*, new_addr, new_size);

    new_size = omSizeOfAddr(new_addr);
    min_size = (old_size < new_size ? old_size : new_size);
    omMemcpyW(new_addr, old_addr, min_size >> LOG_SIZEOF_LONG);

    if (DO_ZERO(flag) && (new_size > old_size))
      omMemsetW((char*) new_addr + min_size, 0, (new_size - old_size) >> LOG_SIZEOF_LONG);

    __omFreeSize(old_addr, old_size);

    return new_addr;
  }
}
#endif
#endif /* OM_ALLOC_C */
