/*******************************************************************
 *  File:    omAlloc.c
 *  Purpose: implementation of main omalloc functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAlloc.c,v 1.3 1999-11-23 20:40:12 obachman Exp $
 *******************************************************************/
#ifndef OM_ALLOC_C
#define OM_ALLOC_C
#include <string.h>
#include <stdio.h>

#include "omConfig.h"
#include "omPrivate.h"
#include "omLocal.h"
#include "omList.h"


/*******************************************************************
 *  
 *  global variables
 *  
 *******************************************************************/

omBinPage_t om_ZeroPage[] = {{0, NULL, NULL, NULL, NULL}};
omBinPage_t om_CheckPage[] = {{0, NULL, NULL, NULL, NULL}};
omBinPage_t om_LargePage[] = {{0, NULL, NULL, NULL, NULL}};
omBin_t     om_LargeBin[] = {{om_LargePage, NULL, NULL, 0, 0, 0}};
omBin_t     om_CheckBin[] = {{om_CheckPage, NULL, NULL, 0, 0, 0}};
omSpecBin om_SpecBin = NULL;

/*******************************************************************
 *  
 *  Alloc routines which we do not define as macros
 *  
 *******************************************************************/
void* _omReallocBlock(void* old_addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  if (old_addr == NULL) __omTypeAllocBlock(void*, new_addr, new_size);
  else
  {
    if (new_size <= OM_MAX_BLOCK_SIZE && old_size <= OM_MAX_BLOCK_SIZE)
    {
      omBin __om_new_bin = omSmallSize2Bin(new_size);
      omBin __om_old_bin = omSmallSize2Bin(old_size);
      if (__om_new_bin != __om_old_bin)
      {
        __omTypeAllocBlock(void*, new_addr, new_size);
        omMemcpyW(new_addr, old_addr,
                  (__om_old_bin->sizeW > __om_new_bin->sizeW ?
                   __om_new_bin->sizeW : __om_old_bin->sizeW));
        __omFreeBin(old_addr);
      }
    }
    else if (new_size > OM_MAX_BLOCK_SIZE && old_size > OM_MAX_BLOCK_SIZE)
    {
      new_addr = (type) omLargeBlockRealloc(new_size, old_addr, old_size);
    }
    else
    {
      __omTypeAllocBlock(void*, new_addr, new_size);
      memcpy(new_addr, old_addr, (old_size > new_size ? new_size : old_size));
    }
  }
  return new_addr;
}

void* _omRealloc0Block(void* old_addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  int __om_fill = new_size - old_size;
  new_addr = _omReallocBlock(void*, new_addr, new_size, old_addr, old_size);
  if (_om_fill > 0) memset(((void*) new_addr) + __om_fill, 0, __om_fill);
  return new_addr;
}

#ifdef OM_ALIGNMENT_NEEDS_WORK      
void* _omReallocAlignedBlock(void* old_addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  if (old_addr == NULL) __omTypeAllocAlignedBlock(void*, new_addr, new_size);
  else
  {
    if (new_size <= OM_MAX_BLOCK_SIZE && old_size <= OM_MAX_BLOCK_SIZE)
    {
      omBin __om_new_bin = omSmallSize2AlignedBin(new_size);
      omBin __om_old_bin = omSmallSize2AlignedBin(old_size);
      if (__om_new_bin != __om_old_bin)
      {
        __omTypeAllocAlignedBlock(void*, new_addr, new_size);
        omMemcpyW(new_addr, old_addr,
                  (__om_old_bin->sizeW > __om_new_bin->sizeW ?
                   __om_new_bin->sizeW : __om_old_bin->sizeW));
        __omFreeBin(old_addr);
      }
    }
    else if (new_size > OM_MAX_BLOCK_SIZE && old_size > OM_MAX_BLOCK_SIZE)
    {
      new_addr = (type) omLargeBlockRealloc(new_size, old_addr, old_size);
    }
    else
    {
      __omTypeAllocAllignedBlock(void*, new_addr, new_size);
      memcpy(new_addr, old_addr, (old_size > new_size ? new_size : old_size));
    }
  }
}

void* _omRealloc0AlignedBlock(void* old_addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  int __om_fill = new_size - old_size;
  __omTypeReallocAlignedBlock(void*, new_addr, new_size, old_addr, old_size);
  if (_om_fill > 0) memset(((void*) new_addr) + __om_fill, 0, __om_fill);
  return new_addr;
}
#endif /* OM_ALIGNMENT_NEEDS_WORK */

void* _omReallocChunk(void* old_addr, size_t new_size)
{
  void* new_addr;
  if (old_addr == NULL) __omTypeAllocChunk(void*, new_addr, new_size);
  else
  {
    void* __old_addr = ((void*) (old_addr)) - SIZEOF_OM_ALIGNMENT;
    omBinPage __om_page = *((omBinPage*) __old_addr);
    size_t __new_size = new_size + SIZEOF_OM_ALIGNMENT;
    void* __new_addr;
    if (__new_size <= MAX_BLOCK_SIZE && __om_page != om_LargePage)
    {
      omBin __om_old_bin = omGetTopBinOfPage(__om_page);
      omBin __om_new_bin = omSmallSize2Bin(__new_size);
      if (__om_old_bin != __om_new_bin)
      {
        __omTypeAllocBin(void*, __new_addr, __new_size);
        omMemcpyW(__new_addr, __old_addr,                     
                  (__om_old_bin->sizeW > __om_new_bin->sizeW ?
                   __om_new_bin->sizeW : __om_old_bin->sizeW));
        *((void**) __new_addr) = (void*) __om_new_bin->current_page;
        __omFreeBin(__old_addr);
        new_addr = (type) (__new_addr + SIZEOF_OM_ALIGNMENT);
      }
      else
      {
        new_addr = (type) old_addr;
      }
    }
    else if (__new_size > MAX_BLOCK_SIZE && __om_page == om_LargePage)
    {
      __new_addr = omReallocLargeChunk(__old_addr, __new_size);
      *((void**) __new_addr) = (void*) om_LargePage;
      new_addr = (type) (__new_addr + SIZEOF_OM_ALIGNMENT);
    }
    else
    {
      __omTypeAllocChunk(void*, new_addr, new_size);
      if (__om_page != om_LargePage)
      {
        omBin old_bin = omGetTopBinOfPage(__om_page);
        omMemcpyW(new_addr, old_addr, old_bin->sizeW - 1);
      }
      else
      {
        memcpy(new_addr, old_addr, new_size);
      }
    }
  }
  return new_addr;
}

#ifdef OM_ALIGNMENT_NEEDS_WORK
void* _omReallocAlignedChunk(void* old_addr, size_t new_size)
{
  void* new_addr;
  if (old_addr == NULL) __omTypeAllocAlignedChunk(void*, new_addr, new_size);
  else
  {
    void* __old_addr = ((void*) (old_addr)) - SIZEOF_OM_CHUNK_ALIGNMENT;
    omBinPage __om_page = *((omBinPage*) __old_addr);
    size_t __new_size = new_size + SIZEOF_OM_CHUNK_ALIGNMENT;
    void* __new_addr;
    if (__new_size <= MAX_BLOCK_SIZE && __om_page != om_LargePage)
    {
      omBin __om_old_bin = omGetTopBinOfPage(__om_page);
      omBin __om_new_bin = omSmallSize2AlignedBin(__new_size);
      if (__om_old_bin != __om_new_bin)
      {
        __omTypeAllocBin(void*, __new_addr, __new_size);
        omMemcpyW(__new_addr, __old_addr,                     
                  (__om_old_bin->sizeW > __om_new_bin->sizeW ?
                   __om_new_bin->sizeW : __om_old_bin->sizeW));
        *((void**) __new_addr) = (void*) __om_new_bin->current_page;
        __omFreeBin(__old_addr);
        new_addr = (type) (__new_addr + SIZEOF_OM_ALIGNMENT);
      }
      else
      {
        new_addr = (type) old_addr;
      }
    }
    else if (__new_size > MAX_BLOCK_SIZE && __om_page == om_LargePage)
    {
      __new_addr = omReallocLargeChunk(__old_addr, __new_size);
      *((void**) __new_addr) = (void*) om_LargePage;
      new_addr = (type) (__new_addr + SIZEOF_OM_CHUNK_ALIGNMENT);
    }
    else
    {
      __omTypeAllocChunk(void*, new_addr, new_size);
      if (__om_page != om_LargePage)
      {
        omBin old_bin = omGetTopBinOfPage(__om_page);
        memcpy(new_addr, old_addr, 
               old_bin->sizeW*SIZEOF_LONG - SIZEOF_OM_CHUNK_ALIGNMENT);
      }
      else
      {
        memcpy(new_addr, old_addr, new_size);
      }
      __omFreeAlignedChunk(old_addr);
    }
  }
  return new_addr;
}
#endif /* OM_ALIGNMENT_NEEDS_WORK */


/*******************************************************************
 *  
 *  Local stuff
 *  
 *******************************************************************/
/* this should go away */
#define AllocSizeOf(x) _omAllocBlock(sizeof(x))
#define FreeSizeOf(addr, x) _omFreeBin(addr)

/* Get new page and initialize */
static omBinPage omAllocNewBinPage(omBin bin)
{
  omBinPage newpage;
  void* tmp;
  int i = 1;

  if (bin->max_blocks > 0) newpage = omGetPage();
  else newpage = omVallocFromSystem(bin->sizeW * SIZEOF_LONG);
    
  omAssume(omIsAddrPageAligned((void*) newpage));

  omSetTopBinAndStickyOfPage(newpage, bin, bin->sticky);
  newpage->used_blocks = -1;
  newpage->current = (void*) (((void*)newpage) + SIZEOF_OM_BIN_PAGE_HEADER);
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

OM_INLINE void omFreeBinPage(omBinPage page, omBin bin)
{
  if (bin->max_blocks > 0)
    omFreePage(page);
  else
    omVfreeToSystem(page, bin->sizeW * SIZEOF_LONG);
}

    
/* primitives for handling of list of pages */
OM_INLINE void omTakeOutBinPage(omBinPage page, omBin bin)
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
    page->next->prev = page->prev;
  }
  if (page->prev != NULL) page->prev->next = page->next;
}

OM_INLINE void omInsertBinPage(omBinPage after, omBinPage page, omBin bin)
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
    bin->current_page->used_blocks = 0;
  }

  if (bin->current_page->next != NULL)
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
  __omTypeAllocFromPage(void*, addr, newpage);
  return addr;
}


/* page->used_blocks == 0, so, either free page or reallocate to 
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
  omAssume(page->used_blocks == 0);

  if (page == om_LargePage)
  {
    omFreeToSystem(addr);
  }
  else
  {
    bin = omGetBinOfPage(page);
    if (page->current != NULL || bin->max_blocks <= 1)
    {
      // all blocks of page are now collected
      omTakeOutBinPage(page, bin);
      // page can be freed
      omFreeBinPage(page, bin);
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
#elsif defined(PAGE_AFTER_CURRENT)
      omInsertBinPage(bin->current_page, page, bin);
#else
      omInsertBinPage(bin->last_page, page, bin);
#endif    
    }
  }
}

/*****************************************************************
 *
 * SpecBin business
 *
 *****************************************************************/
omBin omGetSpecBin(size_t size)
{
  omBin om_new_specBin;
  long max_blocks;
  long sizeW;


  size = OM_ALIGN_SIZE(size);
  
  if (size > SIZEOF_OM_BIN_PAGE)
  {
    /* need page header */
    max_blocks = - (long) 
      ((size+(SIZEOF_SYSTEM_PAGE-SIZEOF_OM_BIN_PAGE))+SIZEOF_SYSTEM_PAGE-1)
      / SIZEOF_SYSTEM_PAGE;
    sizeW = ((-max_blocks*SIZEOF_SYSTEM_PAGE) - 
             (SIZEOF_SYSTEM_PAGE - SIZEOF_OM_BIN_PAGE)) / SIZEOF_LONG;
    om_new_specBin = om_LargeBin;
  }
  else
  {
    max_blocks = SIZEOF_OM_BIN_PAGE / size;
    sizeW = (SIZEOF_OM_BIN_PAGE / max_blocks) / SIZEOF_LONG;
    om_new_specBin = omSize2Bin( size );
  }

  if (om_new_specBin == om_LargeBin ||
      om_new_specBin->max_blocks < max_blocks)
  {
    omSpecBin s_bin 
      = omFindInSortedGList(om_SpecBin, next, max_blocks, max_blocks);

    if (s_bin != NULL) 
    {
      (s_bin->ref)++;
      omAssume(s_bin->bin != NULL && 
             s_bin->bin->max_blocks == s_bin->max_blocks &&
             s_bin->bin->sizeW == sizeW);
      return s_bin->bin;
    }
    s_bin = (omSpecBin) AllocSizeOf(omSpecBin_t);
    s_bin->ref = 1;
    s_bin->next = NULL;
    s_bin->max_blocks = max_blocks;
    s_bin->bin = (omBin) AllocSizeOf(omBin_t);
    s_bin->bin->current_page = om_ZeroPage;
    s_bin->bin->last_page = NULL;
    s_bin->bin->next = NULL;
    s_bin->bin->sizeW = sizeW;
    s_bin->bin->max_blocks = max_blocks;
    s_bin->bin->sticky = 0;
    om_SpecBin = omInsertInSortedGList(om_SpecBin, next, max_blocks, s_bin);
    return s_bin->bin;
  }
  else
  {
    return om_new_specBin;
  }
}

int omIsStaticBin(omBin bin)
{
  return ((unsigned long) bin >= ((unsigned long) &om_StaticBin[0]) &&
          (unsigned long) bin <= ((unsigned long) &om_StaticBin[OM_MAX_BIN_INDEX]));
}
  
void omUnGetSpecBin(omBin *bin_p)
{
  omBin bin = *bin_p;
  if (! omIsStaticBin(bin))
  {
    omSpecBin s_bin 
      = omFindInSortedGList(om_SpecBin, next, max_blocks, bin->max_blocks);
    omAssume(s_bin != NULL);
    if (s_bin != NULL)
    {
      (s_bin->ref)--;
      if (s_bin->ref == 0)
      {
        if(s_bin->bin->last_page == NULL)
        {
          om_SpecBin = omRemoveFromSortedGList(om_SpecBin, next, max_blocks,
                                               s_bin);
          FreeSizeOf(s_bin->bin, omBin_t);
          FreeSizeOf(s_bin, omSpecBin_t);
        }
      }
    }
  }
  *bin_p = NULL;
}

/*****************************************************************
 *
 * Statistics
 *
 *****************************************************************/
static void omGetBinStat(omBin bin, int *pages_p, int *used_blocks_p, 
                          int *free_blocks_p)
{
  int pages = 0, used_blocks = 0, free_blocks = 0;
  int where = 1;
  
  omBinPage page = bin->last_page;
  while (page != NULL)
  {
    pages++;
    if (where == 1) 
    {
      used_blocks += page->used_blocks + 1;
      if (bin->max_blocks > 0)
        free_blocks += bin->max_blocks - page->used_blocks -1;
    }
    else 
    {
      if (bin->max_blocks > 1)
        used_blocks += bin->max_blocks;
      else
        used_blocks++;
    }
    if (page == bin->current_page) where = -1;
    page = page->prev;
  }
  *pages_p = pages;
  *used_blocks_p = used_blocks;
  *free_blocks_p = free_blocks;
}

static void omGetTotalBinStat(omBin bin, int *pages_p, int *used_blocks_p, 
                               int *free_blocks_p)
{
  int t_pages = 0, t_used_blocks = 0, t_free_blocks = 0;
  int pages = 0, used_blocks = 0, free_blocks = 0;
 
  while (bin != NULL)
  {
    omGetBinStat(bin, &pages, &used_blocks, &free_blocks);
    t_pages += pages;
    t_used_blocks += used_blocks;
    t_free_blocks += free_blocks;
    bin = bin->next;
  }
  *pages_p = t_pages;
  *used_blocks_p = t_used_blocks;
  *free_blocks_p = t_free_blocks;
}

void omGetBinUsage()
{
  int pages = 0, used_blocks = 0, free_blocks = 0;
  size_t bytes;
  int i;
  omSpecBin s_bin = om_SpecBin;
  
  for(i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    omGetTotalBinStat(&om_StaticBin[i], &pages, &used_blocks, &free_blocks);
    bytes += used_blocks*om_StaticBin[i].sizeW*SIZEOF_LONG;
  }
  while(s_bin != NULL)
  {
    omGetTotalBinStat(s_bin->bin, &pages, &used_blocks, &free_blocks);
    bytes +=  s_bin->bin->sizeW*used_blocks*SIZEOF_LONG;
    s_bin = s_bin->next;
  }
}

void omPrintBinStat(FILE * fd, omBin bin)
{
  int pages = 0, used_blocks = 0, free_blocks = 0;
  fprintf(fd, "%ld\t%ld\t", bin->sizeW, bin->max_blocks);
  omGetTotalBinStat(bin, &pages, &used_blocks, &free_blocks);
  fprintf(fd, "%d\t%d\t%d\n", pages, free_blocks, used_blocks);
  if (bin->next != NULL)
  {
    while (bin != NULL)
    {
      omGetBinStat(bin, &pages, &used_blocks, &free_blocks);
      fprintf(fd, " \t \t%d\t%d\t%d\t%d\n", pages, free_blocks, used_blocks, 
              (int) bin->sticky);
      bin = bin->next;
    }
  }
}
  
void omPrintBinStats(FILE* fd)
{
  int i;
  omSpecBin s_bin = om_SpecBin;
  
  fprintf(fd, "SizeW\tMBlocks\tUPages\tFBlocks\tUBlocks\tSticky\n");
  fflush(fd);
  for(i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    omPrintBinStat(fd, &om_StaticBin[i]);
  }
  while(s_bin != NULL)
  {
    omPrintBinStat(fd, s_bin->bin);
    s_bin = s_bin->next;
  }
}
 
/*****************************************************************
 *
 * Sticky business
 *
 *****************************************************************/
#define omGetStickyBin(bin, sticky_tag) \
   omFindInGList(bin, next, sticky, sticky_tag)

static omBin omCreateStickyBin(omBin bin, unsigned long sticky)
{
  omBin s_bin = (omBin) AllocSizeOf(omBin_t);
  s_bin->sticky = sticky;
  s_bin->current_page = om_ZeroPage;
  s_bin->last_page = NULL;
  s_bin->max_blocks = bin->max_blocks;
  s_bin->sizeW = bin->sizeW;
  s_bin->next = bin->next;
  bin->next = s_bin;
  return s_bin;
}

unsigned long omGetMaxStickyBinTag(omBin bin)
{
  unsigned long sticky = 0;
  do
  {
    if (bin->sticky > sticky) sticky = bin->sticky;
    bin = bin->next;
  }
  while (bin != NULL);
  return sticky;
}

unsigned long omGetNewStickyBinTag(omBin bin)
{
  unsigned long sticky = omGetMaxStickyBinTag(bin);
  if (sticky < BIT_SIZEOF_LONG - 2)
  {
    sticky++;
    omCreateStickyBin(bin, sticky);
    return sticky;
  }
  else
  {
    omAssume(sticky == BIT_SIZEOF_LONG - 1);
  }
  return sticky;
}

void omSetStickyBinTag(omBin bin, unsigned long sticky_tag)
{
  omBin s_bin;
  s_bin = omGetStickyBin(bin, sticky_tag);
  
  if (s_bin != bin)
  {
    omBinPage tc, tl;
    unsigned long ts;
    
    if (s_bin == NULL) s_bin = omCreateStickyBin(bin, sticky_tag);
    ts = bin->sticky;
    tl = bin->last_page;
    tc = bin->current_page;
    bin->sticky = s_bin->sticky;
    bin->current_page = s_bin->current_page;
    bin->last_page = s_bin->last_page;
    s_bin->sticky = ts;
    s_bin->last_page = tl;
    s_bin->current_page = tc;
  }
}

void omUnSetStickyBinTag(omBin bin, unsigned long sticky)
{
  omAssume(omGetStickyBin(bin, 0) != NULL);
  if (bin->sticky == sticky)
    omSetStickyBinTag(bin, 0);
}

static void omMergeStickyPages(omBin to_bin, omBin from_bin)
{
#ifdef HAVE_OM_ASSUME
  int length = omGListLength(to_bin->last_page, prev) +
    omGListLength(from_bin->last_page, prev);
#endif  
  
  omBinPage page = from_bin->last_page;
  omAssume(to_bin->sticky == 0);
  omAssume(to_bin->sizeW == from_bin->sizeW);
  omAssume(to_bin != from_bin);
  omAssume(omIsOnGList(to_bin->next, next, from_bin) 
           || 
           omIsOnGList(from_bin->next, next, to_bin));

  if (page == NULL) return;
  do
  {
    omSetStickyOfPage(page, 0);
    if (page->prev == NULL) break;
    page = page->prev;
  }
  while(1);

  if (to_bin->last_page == NULL)
  {
    omAssume(to_bin->current_page == om_ZeroPage);
    to_bin->last_page = from_bin->last_page;
    to_bin->current_page = from_bin->current_page;
    return;
  }
  
  omAssume(to_bin->current_page != om_ZeroPage && 
           to_bin->current_page != NULL);

  if (to_bin->current_page->current != NULL)
  {
    if (to_bin->current_page->prev == NULL)
    {
      from_bin->last_page->next = to_bin->current_page;
      to_bin->current_page->prev = from_bin->last_page;
      to_bin->current_page = from_bin->current_page;
      return;
    }
    to_bin->current_page = to_bin->current_page->prev;
  }
  else
  {
    /* need to reset this here, since new current_page is going to be
       from_bin->current_page, and only for current_page may we have 
       used_blocks != 0 && current == NULL */
    to_bin->current_page->used_blocks = 0;
  }
  

  omAssume(to_bin->current_page != NULL && 
           to_bin->current_page->current == NULL &&
           to_bin->current_page->used_blocks == 0);

  from_bin->last_page->next = to_bin->current_page->next;
  if (to_bin->current_page->next != NULL) 
    to_bin->current_page->next->prev =  from_bin->last_page;
  else
  {
    omAssume(to_bin->current_page == to_bin->last_page);
    to_bin->last_page = from_bin->last_page;
  }
  to_bin->current_page->next = page;
  page->prev = to_bin->current_page;
  to_bin->current_page = from_bin->current_page;
  
#ifdef HAVE_OM_ASSUME
  omAssume(omGListLength(to_bin->last_page, prev) == length);
#endif
}

void omDeleteStickyBinTag(omBin bin, unsigned long sticky)
{
  omBin no_sticky_bin = NULL;
  omBin sticky_bin = NULL;

  if (sticky == 0)
  {
    omAssume(0);
    return;
  }
  
  sticky_bin = omGetStickyBin(bin, sticky);
  if (sticky_bin != NULL)
  {
    no_sticky_bin = omGetStickyBin(bin, 0);
    omAssume(no_sticky_bin != NULL && sticky_bin != no_sticky_bin);

    omMergeStickyPages(no_sticky_bin, sticky_bin);

    if (bin == sticky_bin) 
    {
      sticky_bin = no_sticky_bin;
      omSetStickyBinTag(bin, 0);
    }
    bin->next = omRemoveFromGList(bin->next, next, sticky_bin);
    FreeSizeOf(sticky_bin, omBin_t);
  }
}


 
/*****************************************************************
*
* AllBin business
*
*****************************************************************/
unsigned long omGetNewStickyAllBinTag()
{
  unsigned long sticky = 0, new_sticky;
  int i;
  omSpecBin s_bin;
  // first, find new sticky tag
  for (i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    new_sticky = omGetMaxStickyBinTag(&(om_StaticBin[i]));
    if (new_sticky > sticky) sticky = new_sticky;
  }
  s_bin = om_SpecBin;
  while (s_bin != NULL) 
  {
    new_sticky = omGetMaxStickyBinTag(s_bin->bin);
    if (new_sticky > sticky) sticky = new_sticky;
    s_bin = s_bin->next;
  }
  if (sticky < BIT_SIZEOF_LONG - 2) 
  {
    sticky++;
    for (i=0; i<=OM_MAX_BIN_INDEX; i++)
    {
      omCreateStickyBin(&(om_StaticBin[i]), sticky);
    }
    s_bin = om_SpecBin;
    while (s_bin != NULL) 
    {
      omCreateStickyBin(s_bin->bin, sticky);
      s_bin = s_bin->next;
    }
    return sticky;
  }
  else
  {
    omBin bin;
    omAssume(sticky == BIT_SIZEOF_LONG - 1);
    for (i=0; i<=OM_MAX_BIN_INDEX; i++)
    {
      bin = &om_StaticBin[i];
      if (omGetStickyBin(bin, BIT_SIZEOF_LONG -1) == NULL)
        omCreateStickyBin(bin, BIT_SIZEOF_LONG -1);
    }
    s_bin = om_SpecBin;
    while (s_bin != NULL) 
    {
      if (omGetStickyBin(s_bin->bin, BIT_SIZEOF_LONG -1) == NULL)
        omCreateStickyBin(s_bin->bin, BIT_SIZEOF_LONG -1);
      s_bin = s_bin->next;
    }
    return BIT_SIZEOF_LONG - 1;
  }
}

void omSetStickyAllBinTag(unsigned long sticky)
{
  omSpecBin s_bin = om_SpecBin;
  int i;
  for (i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    omSetStickyBinTag(&(om_StaticBin[i]), sticky);
  }
  while (s_bin != NULL) 
  {
    omSetStickyBinTag(s_bin->bin, sticky);
    s_bin = s_bin->next;
  }
}

void omUnSetStickyAllBinTag(unsigned long sticky)
{
  omSpecBin s_bin = om_SpecBin;
  int i;
  for (i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    omUnSetStickyBinTag(&(om_StaticBin[i]), sticky);
  }
  while (s_bin != NULL) 
  {
    omUnSetStickyBinTag(s_bin->bin, sticky);
    s_bin = s_bin->next;
  }
}

void omDeleteStickyAllBinTag(unsigned long sticky)
{
  omSpecBin s_bin = om_SpecBin;
  int i;
  for (i=0; i<=OM_MAX_BIN_INDEX; i++)
  {
    omDeleteStickyBinTag(&(om_StaticBin[i]), sticky);
  }
  while (s_bin != NULL) 
  {
    omDeleteStickyBinTag(s_bin->bin, sticky);
    s_bin = s_bin->next;
  }
}

#endif /* OM_ALLOC_C */
