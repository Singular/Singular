/*******************************************************************
 *  File:    omBinPage.c
 *  Purpose: implementation of routines for primitve BinPage managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include <limits.h>
#include "omalloc.h"

#ifdef HAVE_OMALLOC
#include "omDefaultConfig.h"

/*******************************************************************
 *
 * Local declarations
 *
 *******************************************************************/

/* define if you want to keep regions approximately in order */
#define OM_KEEP_REGIONS_ORDER

struct omBinPageRegion_s
{
  void* current;        /* linked list of free pages */
  omBinPageRegion next; /* nex/prev pointer in ring of regions */
  omBinPageRegion prev;
  char* init_addr;      /* pointer portion of inital chunk which is still free */
  char* addr;         /* addr returned by alloc */
  int   init_pages;   /* number of pages still available in init_chunk */
  int   used_pages;     /* number of used pages */
  int pages;          /* total size of region */
};

/* globale variable holding pointing int regions ring */
static omBinPageRegion om_CurrentBinPageRegion = NULL;
unsigned long om_MaxBinPageIndex = 0;
unsigned long om_MinBinPageIndex = ULONG_MAX;
unsigned long *om_BinPageIndicies = NULL;

/* declaration of local procs */
static void* omTakeOutConsecutivePages(omBinPageRegion region, int how_many);
static omBinPageRegion omAllocNewBinPagesRegion(int min_pages);
static void omFreeBinPagesRegion(omBinPageRegion region);

static void omBinPageIndexFault(unsigned long low_index, unsigned long high_index);
static void omRegisterBinPages(void* low_addr, int pages);
static void omUnregisterBinPages(void* low_addr, int pages);

OM_INLINE_LOCAL void omTakeOutRegion(omBinPageRegion region)
{
  omAssume(region != NULL);

  if (region->prev != NULL)
  {
    omAssume(region->prev != region && region->prev != region->next);
    region->prev->next = region->next;
  }

  if (region->next != NULL)
  {
    omAssume(region->next != region && region->prev != region->next);
    region->next->prev = region->prev;
  }
}

OM_INLINE_LOCAL void omInsertRegionAfter(omBinPageRegion insert, omBinPageRegion after)
{
  omAssume(insert != NULL && after != NULL && insert != after);
  insert->next = after->next;
  insert->prev = after;
  after->next = insert;
  if (insert->next != NULL)
  {
    omAssume(insert->next != insert && insert->next != after);
    insert->next->prev = insert;
  }
}

OM_INLINE_LOCAL void omInsertRegionBefore(omBinPageRegion insert, omBinPageRegion before)
{
  omAssume(insert != NULL && before != NULL && insert != before);
  insert->prev = before->prev;
  insert->next = before;
  before->prev = insert;
  if (insert->prev != NULL)
    insert->prev->next = insert;
}


/*******************************************************************
 *
 * Alloc/Free of BinPages
 *
 *******************************************************************/
#define NEXT_PAGE(page) *((void**) page)
#define OM_IS_EMPTY_REGION(region) ((region)->current == NULL && (region->init_addr == NULL))

omBinPage omAllocBinPage()
{
  omBinPage bin_page;

  if (om_CurrentBinPageRegion == NULL)
    om_CurrentBinPageRegion = omAllocNewBinPagesRegion(1);

  while (1)
  {
    if (om_CurrentBinPageRegion->current != NULL)
    {
      bin_page = om_CurrentBinPageRegion->current;
      om_CurrentBinPageRegion->current = NEXT_PAGE(bin_page);
      goto Found;
    }
    if (om_CurrentBinPageRegion->init_pages > 0)
    {
      bin_page = (omBinPage)om_CurrentBinPageRegion->init_addr;
      om_CurrentBinPageRegion->init_pages--;
      if (om_CurrentBinPageRegion->init_pages > 0)
        om_CurrentBinPageRegion->init_addr += SIZEOF_SYSTEM_PAGE;
      else
        om_CurrentBinPageRegion->init_addr = NULL;
      goto Found;
    }
    if (om_CurrentBinPageRegion->next != NULL)
    {
      om_CurrentBinPageRegion = om_CurrentBinPageRegion->next;
    }
    else
    {
      omBinPageRegion new_region = omAllocNewBinPagesRegion(1);
      new_region->prev = om_CurrentBinPageRegion;
      om_CurrentBinPageRegion->next = new_region;
      om_CurrentBinPageRegion = new_region;
    }
  }

  Found:
  bin_page->region = om_CurrentBinPageRegion;
  om_CurrentBinPageRegion->used_pages++;

  om_Info.UsedPages++;
  om_Info.AvailPages--;
  if (om_Info.UsedPages > om_Info.MaxPages)
    om_Info.MaxPages = om_Info.UsedPages;

  OM_ALLOC_BINPAGE_HOOK;
  return bin_page;
}

omBinPage omAllocBinPages(int how_many)
{
  omBinPage bin_page;
  omBinPageRegion region;

  if (om_CurrentBinPageRegion == NULL)
    om_CurrentBinPageRegion = omAllocNewBinPagesRegion(how_many);

  region = om_CurrentBinPageRegion;
  while (1)
  {
    if (region->init_pages >= how_many)
    {
      bin_page = (omBinPage)region->init_addr;
      region->init_pages -= how_many;
      if (region->init_pages)
        region->init_addr += how_many*SIZEOF_SYSTEM_PAGE;
      else
        region->init_addr = NULL;
      goto Found;
    }
    if ((bin_page = omTakeOutConsecutivePages(region, how_many)) != NULL)
    {
      goto Found;
    }
    if (region->next != NULL)
    {
      region = region->next;
    }
    else
    {
      omBinPageRegion new_region = omAllocNewBinPagesRegion(how_many);
      region->next = new_region;
      new_region->prev = region;
      region = new_region;
    }
  }
  /*while (1) */

  Found:
  bin_page->region = region;
  region->used_pages += how_many;

  if (region != om_CurrentBinPageRegion && OM_IS_EMPTY_REGION(region))
  {
    omTakeOutRegion(region);
    omInsertRegionBefore(region, om_CurrentBinPageRegion);
  }
  om_Info.UsedPages += how_many;
  om_Info.AvailPages -= how_many;
  if (om_Info.UsedPages > om_Info.MaxPages)
    om_Info.MaxPages = om_Info.UsedPages;

  OM_ALLOC_BINPAGE_HOOK;
  return bin_page;
}

void omFreeBinPages(omBinPage bin_page, int how_many)
{
  omBinPageRegion region = bin_page->region;

  region->used_pages -= how_many;
  if (region->used_pages == 0)
  {
    if (region == om_CurrentBinPageRegion)
    {
      if (region->next != NULL)
        om_CurrentBinPageRegion = region->next;
      else
        om_CurrentBinPageRegion = region->prev;
    }
    omTakeOutRegion(region);
    omFreeBinPagesRegion(region);
  }
  else
  {
    if (region != om_CurrentBinPageRegion && OM_IS_EMPTY_REGION(region))
    {
      omTakeOutRegion(region);
      omInsertRegionAfter(region, om_CurrentBinPageRegion);
    }
    if (how_many > 1)
    {
      int i = how_many;
      char* page = (char *)bin_page;

      while (i > 1)
      {
        NEXT_PAGE(page) = page + SIZEOF_SYSTEM_PAGE;
        page = NEXT_PAGE(page);
        i--;
      }
      NEXT_PAGE(page) = region->current;
    }
    else
    {
      NEXT_PAGE(bin_page) = region->current;
    }
    region->current = (void*) bin_page;
  }
  om_Info.AvailPages += how_many;
  om_Info.UsedPages -= how_many;
  OM_FREE_BINPAGE_HOOK;
}

static void* omTakeOutConsecutivePages(omBinPageRegion region, int pages)
{
  void* current;
  char* iter;
  void* prev = NULL;
  void* bin_page;
  int found;
  current = region->current;
  while (current != NULL)
  {
    found = 1;
    iter = current;
    while (NEXT_PAGE(iter) == (iter + SIZEOF_SYSTEM_PAGE))
    {
      iter = NEXT_PAGE(iter);
      /* handle pathological case that iter + SIZEOF_SYSTEM_PAGE == 0 */
      if (iter == NULL) return NULL;
      found++;
      if (found == pages)
      {
        bin_page = current;
        if (current == region->current)
        {
          region->current = NEXT_PAGE(iter);
        }
        else
        {
          omAssume(prev != NULL);
          NEXT_PAGE(prev) = NEXT_PAGE(iter);
        }
        return bin_page;
      }
    }
    prev = iter;
    current = NEXT_PAGE(iter);
  }
  return NULL;
}

/* Alloc a new region and insert into regions ring, set current to new region */
static omBinPageRegion omAllocNewBinPagesRegion(int min_pages)
{
  omBinPageRegion region = omAllocFromSystem(sizeof(omBinPageRegion_t));
  om_Info.InternalUsedBytesMalloc+=sizeof(omBinPageRegion_t);
  void* addr;
  int pages = (min_pages>om_Opts.PagesPerRegion ? min_pages : om_Opts.PagesPerRegion);
  size_t size = ((size_t)pages)*SIZEOF_SYSTEM_PAGE;

  addr = _omVallocFromSystem(size, 1);
  if (addr == NULL)
  {
    pages = min_pages;
    size = min_pages*SIZEOF_SYSTEM_PAGE;
    addr = omVallocFromSystem(size);
  }

  omRegisterBinPages(addr, pages);
  region->addr = addr;
  region->pages = pages;
  region->used_pages = 0;
  region->init_addr = addr;
  region->init_pages = pages;
  region->current = NULL;
  region->next = NULL;
  region->prev = NULL;

  om_Info.AvailPages += pages;

  om_Info.CurrentRegionsAlloc++;
  if (om_Info.CurrentRegionsAlloc > om_Info.MaxRegionsAlloc)
    om_Info.MaxRegionsAlloc = om_Info.CurrentRegionsAlloc;

  return region;
}

/* Free region */
static void omFreeBinPagesRegion(omBinPageRegion region)
{
  omAssume(region != NULL && region->used_pages == 0);

  om_Info.AvailPages -= region->pages;
  om_Info.CurrentRegionsAlloc--;

  omUnregisterBinPages(region->addr, region->pages);
  omVfreeToSystem(region->addr, region->pages*SIZEOF_SYSTEM_PAGE);
  omFreeSizeToSystem(region, sizeof(omBinPageRegion_t));
  om_Info.InternalUsedBytesMalloc-=sizeof(omBinPageRegion_t);
}

/*******************************************************************
 *
 * BinPage registration
 *
 *******************************************************************/

static void omBinPageIndexFault(unsigned long low_index, unsigned long high_index)
{
  unsigned long index_diff = high_index - low_index;
  omAssume(low_index <= high_index &&
           (high_index > om_MaxBinPageIndex || low_index < om_MinBinPageIndex));

  if (om_BinPageIndicies == NULL)
  {
    unsigned long i;
    om_BinPageIndicies = (unsigned long*) omAllocFromSystem((index_diff + 1)*SIZEOF_LONG);
    om_Info.InternalUsedBytesMalloc+=(index_diff + 1)*SIZEOF_LONG;
    om_MaxBinPageIndex = high_index;
    om_MinBinPageIndex = low_index;
    for (i=0; i<=index_diff; i++) om_BinPageIndicies[i] = 0;
  }
  else
  {
    unsigned long old_length = om_MaxBinPageIndex - om_MinBinPageIndex + 1;
    unsigned long new_length = (low_index < om_MinBinPageIndex ?
                                om_MaxBinPageIndex - low_index :
                                high_index - om_MinBinPageIndex) + 1;
    om_BinPageIndicies  = (unsigned long*) omReallocSizeFromSystem(om_BinPageIndicies, old_length*SIZEOF_LONG,
                                                                   new_length*SIZEOF_LONG);
    om_Info.InternalUsedBytesMalloc+= (new_length-old_length)*SIZEOF_LONG;
    if (low_index < om_MinBinPageIndex)
    {
      long i;
      unsigned long offset = new_length - old_length;
      for (i=old_length - 1; i >= 0; i--) om_BinPageIndicies[i+offset] = om_BinPageIndicies[i];
      for (i=offset-1; i>=0; i--)  om_BinPageIndicies[i] = 0;
      om_MinBinPageIndex = low_index;
    }
    else
    {
      unsigned long i;
      for (i=old_length; i<new_length; i++) om_BinPageIndicies[i] = 0;
      om_MaxBinPageIndex = high_index;
    }
  }
}

static void omRegisterBinPages(void* low_addr, int pages)
{
  unsigned long low_index = omGetPageIndexOfAddr(low_addr);
  char* high_addr = (char *)low_addr + (pages-1)*SIZEOF_SYSTEM_PAGE;
  unsigned long high_index = omGetPageIndexOfAddr(high_addr);
  unsigned long shift;

  if (low_index < om_MinBinPageIndex || high_index > om_MaxBinPageIndex)
    omBinPageIndexFault(low_index, high_index);

  shift = omGetPageShiftOfAddr(low_addr);
  if (low_index < high_index)
  {
    if (shift == 0)
    {
      om_BinPageIndicies[low_index-om_MinBinPageIndex] = ULONG_MAX;
    }
    else
    {
      om_BinPageIndicies[low_index-om_MinBinPageIndex] |= ~ ((((unsigned long) 1) << shift) - 1);
    }
    for (shift = low_index+1; shift < high_index; shift++)
    {
      om_BinPageIndicies[shift-om_MinBinPageIndex] = ULONG_MAX;
    }
    shift = omGetPageShiftOfAddr(high_addr);
    if (shift == BIT_SIZEOF_LONG - 1)
    {
      om_BinPageIndicies[high_index - om_MinBinPageIndex] = ULONG_MAX;
    }
    else
    {
      om_BinPageIndicies[high_index-om_MinBinPageIndex] |= ((((unsigned long) 1) << (shift + 1)) - 1);
    }
  }
  else
  {
    high_index = omGetPageShiftOfAddr(high_addr);
    while (high_index > shift)
    {
      om_BinPageIndicies[low_index-om_MinBinPageIndex] |= (((unsigned long) 1) << high_index);
      high_index--;
    }
    om_BinPageIndicies[low_index-om_MinBinPageIndex] |= (((unsigned long) 1) << shift);
  }
}

static void omUnregisterBinPages(void* low_addr, int pages)
{
  unsigned long low_index = omGetPageIndexOfAddr(low_addr);
  char* high_addr = (char *)low_addr + (pages-1)*SIZEOF_SYSTEM_PAGE;
  unsigned long high_index = omGetPageIndexOfAddr(high_addr);
  unsigned long shift;

  shift = omGetPageShiftOfAddr(low_addr);
  if (low_index < high_index)
  {
    if (shift == 0)
    {
      om_BinPageIndicies[low_index-om_MinBinPageIndex] = 0;
    }
    else
    {
      om_BinPageIndicies[low_index-om_MinBinPageIndex] &= ((((unsigned long) 1) << shift) - 1);
    }
    for (shift = low_index+1; shift < high_index; shift++)
    {
      om_BinPageIndicies[shift-om_MinBinPageIndex] = 0;
    }
    shift = omGetPageShiftOfAddr(high_addr);
    if (shift == BIT_SIZEOF_LONG - 1)
    {
      om_BinPageIndicies[high_index - om_MinBinPageIndex] = 0;
    }
    else
    {
      om_BinPageIndicies[high_index-om_MinBinPageIndex] &= ~ ((((unsigned long) 1) << (shift + 1)) - 1);
    }
  }
  else
  {
     high_index = omGetPageShiftOfAddr(high_addr);
     while (high_index > shift)
     {
       om_BinPageIndicies[low_index-om_MinBinPageIndex] &= ~(((unsigned long) 1) << high_index);
       high_index--;
     }
     om_BinPageIndicies[low_index-om_MinBinPageIndex] &= ~(((unsigned long) 1) << shift);
  }
}

/***********************************************************************
 *
 * checking routines
 *
 *******************************************************************/
#ifndef OM_NDEBUG
#include "omDebug.h"

int omIsKnownMemoryRegion(omBinPageRegion region)
{
  omBinPageRegion iter = om_CurrentBinPageRegion;

  if (region == NULL || iter == NULL) return 0;
  iter = omGListLast(om_CurrentBinPageRegion, prev);
  do
  {
    if (region == iter) return 1;
    iter = iter->next;
  }
  while (iter != NULL);
  return 0;
}


omError_t omCheckBinPageRegion(omBinPageRegion region, int level, omError_t report, OM_FLR_DECL)
{
  if (level <= 0) return omError_NoError;

  omCheckReturn(omCheckPtr(region, report, OM_FLR_VAL));
  omCheckReturnCorrupted(! omIsKnownMemoryRegion(region));
  omCheckReturnCorrupted(! omIsAddrPageAligned(region->addr) || ! omIsAddrPageAligned(region->current));
  omCheckReturnCorrupted(region->used_pages < 0);
  omCheckReturnCorrupted(region->init_pages < 0 || region->init_pages > region->pages);

  if (region->init_pages)
  {
    omCheckReturnCorrupted(! omIsAddrPageAligned(region->init_addr));
    omCheckReturnCorrupted(! (region->init_addr >= region->addr
                              && region->init_addr <= region->addr + (region->pages -1)*SIZEOF_SYSTEM_PAGE));
    omCheckReturnCorrupted(region->init_addr !=
                           region->addr + (region->pages - region->init_pages)*SIZEOF_SYSTEM_PAGE);
  }

  omCheckReturn(omCheckList(region->current, level, report, OM_FLR_VAL));
  omCheckReturnCorrupted(region->current == NULL && region->used_pages + region->init_pages != region->pages);
  omCheckReturnCorrupted(level > 1 &&
                         omListLength(region->current)+region->used_pages+region->init_pages != region->pages);
  return omError_NoError;
}

omError_t omCheckBinPageRegions(int level, omError_t report, OM_FLR_DECL)
{
  omBinPageRegion iter = om_CurrentBinPageRegion;

  if (level <= 0) return omError_NoError;
  if (iter == NULL) return omError_NoError;

  omCheckReturnError(om_CurrentBinPageRegion->next != NULL && OM_IS_EMPTY_REGION(om_CurrentBinPageRegion->next),
                     omError_InternalBug);
  omCheckReturnError(om_CurrentBinPageRegion->prev != NULL && ! OM_IS_EMPTY_REGION(om_CurrentBinPageRegion->prev),
                     omError_InternalBug);


  if (level > 1)
  {
    omBinPageRegion prev_last = (omBinPageRegion) omGListLast(om_CurrentBinPageRegion, prev);
    omBinPageRegion next_last = (omBinPageRegion) omGListLast(om_CurrentBinPageRegion, next);

    omCheckReturn(omCheckGList(iter, next, level, report, OM_FLR_VAL));
    omCheckReturn(omCheckGList(iter, prev, level, report, OM_FLR_VAL));

    omCheckReturnCorrupted(omGListLength(prev_last, next)
                          !=
                          omGListLength(next_last, prev));

    omCheckReturn(omCheckBinPageRegion(om_CurrentBinPageRegion, level - 1, report, OM_FLR_VAL));

    iter = om_CurrentBinPageRegion->next;
    while (iter)
    {
      omCheckReturnError(OM_IS_EMPTY_REGION(iter), omError_InternalBug);

      omCheckReturn(omCheckBinPageRegion(iter, level - 1, report, OM_FLR_VAL));
      iter = iter->next;
    }

    iter = om_CurrentBinPageRegion->prev;
    while (iter)
    {
      omCheckReturnError( !OM_IS_EMPTY_REGION(iter), omError_InternalBug);
      omCheckReturn(omCheckBinPageRegion(iter, level - 1, report, OM_FLR_VAL));
      iter = iter->prev;
    }
  }
  return omError_NoError;
}

omBinPageRegion omFindRegionOfAddr(void* addr)
{
  omBinPageRegion region = om_CurrentBinPageRegion;

  if (region == NULL) return 0;
  region = omGListLast(region, prev);
  do
  {
    if ((char *)addr >= region->addr
    && (char *)addr < region->addr + (region->pages)*SIZEOF_SYSTEM_PAGE)
      return region;
    region = region->next;
  }
  while (region != NULL);
  return NULL;
}

int omIsAddrOnFreeBinPage(void* addr)
{
  char *c_addr=(char *)addr;
  omBinPageRegion region = om_CurrentBinPageRegion;

  if (region == NULL) return 0;
  do
  {
    if (c_addr > region->addr && c_addr < region->addr + (region->pages)*SIZEOF_SYSTEM_PAGE)
    {
      if (omIsOnList(region->current, omGetPageOfAddr(addr))) return 1;
      return 0;
    }
    region = region->next;
  }
  while (region != NULL);
  return 0;
}

#endif /* ! OM_NDEBUG */
#endif /* HAVE_OMALLOC*/
