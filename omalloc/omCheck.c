/*******************************************************************
 *  File:    omCheck.c
 *  Purpose: implementation of omCheck functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omCheck.c,v 1.2 2000-05-31 13:34:30 obachman Exp $
 *******************************************************************/
#include "omAlloc.h"

/*******************************************************************
 *  
 *   Static functions
 *  
 *******************************************************************/
static void* _omCheckAllocBin(omBin bin, const int zero, int level);
static void* _omCheckReallocBin(void* old_addr, omBin old_bin, omBin new_bin, int zero, int level);
static void* _omCheckAlloc(size_t size, const int zero_aligned, int level);
static void* _omCheckRealloc(void* old_addr, size_t old_size, size_t new_size, const int zero_aligned_size, int level);
static omError_t _omCheckAddr(void* addr, size_t size, int exact_size, int level);
static omError_t _omCheckLargeAddr(void* addr, size_t size, int exact_size, int level);
static omError_t _omCheckBinAddr(void* addr, omBin bin, size_t size, int exact, int level);
static omError_t omCheckBinPage(omBinPage page, int level);
static int omIsKnownTopBin(omBin bin);

/*******************************************************************
 *  
 *  Top Level: CheckAlloc/CheckFree
 *  
 *******************************************************************/
void* omCheckAllocBin(omBin bin, int level)
{
  return _omCheckAllocBin(bin, 0, level);
}
void* omCheckAlloc0Bin(omBin bin, int level)
{
  return _omCheckAllocBin(bin, 1, level);
}
void* omCheckReallocBin(void* addr, omBin old_bin, omBin new_bin, int level)
{
  return _omCheckReallocBin(addr, old_bin, new_bin, 0, level);
}
void* omCheckRealloc0Bin(void* addr, omBin old_bin, omBin new_bin, int level)
{
  return _omCheckReallocBin(addr, old_bin, new_bin, 1, level);
}

void* omCheckAlloc(size_t Size, int level)
{
  return _omCheckAlloc(Size, 0, level);
}
void* omCheckAlloc0(size_t Size, int level)
{
  return _omCheckAlloc(Size, 1, level);
}
void* omCheckAllocAligned(size_t Size, int level)
{
  return _omCheckAlloc(Size, 2, level);
}
void* omCheckAlloc0Aligned(size_t Size, int level)
{
  return _omCheckAlloc(Size, 3, level);
}

void* omCheckRealloc(void* addr, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, 0, new_Size, 0, level);
}
void* omCheckRealloc0(void* addr, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, 0, new_Size, 1, level);
}
void* omCheckReallocAligned(void* addr, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, 0, new_Size, 2, level);
}
void* omCheckRealloc0Aligned(void* addr, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, 0, new_Size, 3, level);
}
void* omCheckReallocSize(void* addr, size_t old_Size, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, old_Size, new_Size, 4, level);
}
void* omCheckRealloc0Size(void* addr, size_t old_Size, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, old_Size, new_Size, 5, level);
}
void* omCheckReallocAlignedSize(void* addr, size_t old_Size, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, old_Size, new_Size, 6, level);
}
void* omCheckRealloc0AlignedSize(void* addr, size_t old_Size, size_t new_Size, int level)
{
  return _omCheckRealloc(addr, old_Size, new_Size, 7, level);
}

void* omCheckAllocAlignedFunc(size_t size)
{
  if (size == 0) size = 1;
  return omCheckAllocAligned(size, 1);
}

void* omCheckReallocAlignedFunc(void* addr, size_t new_size)
{
  if (addr && new_size)
  {
    return omCheckReallocAligned(addr, new_size, 1);
  }
  else
  {
    omCheckFreeFunc(addr);
    return omCheckAllocAlignedFunc(new_size);
  }
}

void  omCheckFreeFunc(void* addr)
{
  if (addr != NULL) omCheckFree(addr, 1);
}

/*******************************************************************
 *  
 *   Low-Level: CheckAlloc/CheckFree
 *  
 *******************************************************************/

static void* _omCheckAllocBin(omBin bin, const int zero, int level)
{
  void* res;
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  (void) (omCheckBin(bin, level-1) || omCheckBins(level-2));

  if (zero)
    __omTypeAlloc0Bin(void*, res, bin);
  else
    __omTypeAllocBin(void*, res, bin);

  om_ReportedError = omError_InternalBug;
  (void) (om_ErrorStatus || omCheckBinAddrBin(res, bin, level) || omCheckBins(level-2));
  om_ReportedError = 0;
  return res;
}
  
void  omCheckFreeBin(void* addr, omBin bin, int level)
{
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  if (!omCheckBin(bin, level-1) && !omCheckBins(level - 2))
  {
    om_ReportedError = 0;
    if (! omCheckBinAddrBin(addr, bin, level))
    {
      __omFreeBin(addr, bin);

      om_ReportedError = omError_InternalBug;
      (void) (omCheckBin(bin, level-1) || omCheckBins(level - 2));
    }
  }
  om_ReportedError = 0;
}

static void* _omCheckReallocBin(void* old_addr, omBin old_bin, omBin new_bin, int zero, int level)
{
  void* new_addr;
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  if (!omCheckBin(old_bin, level-1) && !omCheckBin(new_bin, level - 1) && !omCheckBins(level - 2))
  {
    om_ReportedError = 0;
    if (!omCheckBinAddrBin(old_addr, old_bin, level))
    {
      if (zero)
        __omTypeRealloc0Bin(old_addr, old_bin, void*, new_addr, new_bin);
      else
        __omTypeReallocBin(old_addr, old_bin, void*, new_addr, new_bin);
      om_ReportedError = omError_InternalBug;
      (void) (omCheckBinAddrBin(new_addr, new_bin, level) || 
              omCheckBin(old_bin, level-1) || 
              omCheckBin(new_bin, level-1) || 
              omCheckBins(level - 2));
      om_ReportedError = 0;
      return new_addr;
    }
  }
  return _omCheckAllocBin(new_bin, zero, 0);
}


static void* _omCheckAlloc(size_t size, const int zero_aligned, int level)
{
  void* res;
  
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;
  if (size <= 0) 
  {
    omReportError(omError_AllocZeroSize, NULL);
    size = 1;
  }

  om_ReportedError = omError_MemoryCorrupted;
  omCheckBins(level-2);

  if (zero_aligned & 1)
  {
    if (zero_aligned & 2)
    {
      __omTypeAlloc0Aligned(void*, res, size);
    }
    else
    {
      __omTypeAlloc0(void*, res, size);
    }
  }
  else
  {
    if (zero_aligned & 2)
    {
      __omTypeAllocAligned(void*, res, size);
    }
    else
    {
      __omTypeAlloc(void*, res, size);
    }
  }
  om_ReportedError = omError_InternalBug;
  (void) (omCheckAddrSize(res, size, level) || omCheckBins(level - 2));

  om_ReportedError = 0;
  return res;
}

void omCheckFreeSize(void* addr, size_t size, int level)
{
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  if (! omCheckBins(level - 2))
  {
    om_ReportedError = 0;
    if (!omCheckAddrSize(addr, size, level))
    {
      __omFreeSize(addr, size);
      om_ReportedError = omError_InternalBug;
      omCheckBins(level - 2);
    }
  }
  om_ReportedError = 0;
}

void omCheckFree(void* addr, int level)
{
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  if (! omCheckBins(level - 2))
  {
    om_ReportedError = 0;
    if (! omCheckAddr(addr, level))
    {
      __omFree(addr);
      om_ReportedError = omError_InternalBug;
      omCheckBins(level - 2);
    }
  }
  om_ReportedError = 0;
}

static void* _omCheckRealloc(void* old_addr, size_t old_size, size_t new_size, 
                             const int zero_aligned_size, int level)
{
  void* new_addr;
  if (om_Opts.CheckLevel > level) level = om_Opts.CheckLevel;

  om_ReportedError = omError_MemoryCorrupted;
  if (! omCheckBins(level - 2))
  {
    om_ReportedError = 0;
    if ( ((zero_aligned_size & 4) && ! omCheckAddrSize(old_addr, old_size, level)) ||
         ( !(zero_aligned_size & 4) && ! omCheckAddr(old_addr, level)))
    {
      if (zero_aligned_size & 4)
      {
        if (zero_aligned_size & 1)
        {
          if (zero_aligned_size & 2)
          {
            __omTypeRealloc0AlignedSize(old_addr, old_size, void*, new_addr, new_size);
          }
          else
          {
            __omTypeRealloc0Size(old_addr, old_size, void*, new_addr, new_size);
          }
        }
        else
        {
          if (zero_aligned_size & 2)
          {
            __omTypeReallocAlignedSize(old_addr, old_size, void*, new_addr, new_size);
          }
          else
          {
            __omTypeReallocSize(old_addr, old_size, void*, new_addr, new_size);
          }
        }
      }
      else
      {
        if (zero_aligned_size & 1)
        {
          if (zero_aligned_size & 2)
          {
            __omTypeRealloc0Aligned(old_addr, void*, new_addr, new_size);
          }
          else
          {
            __omTypeRealloc0(old_addr, void*, new_addr, new_size);
          }
        }
        else
        {
          if (zero_aligned_size & 2)
          {
            __omTypeReallocAligned(old_addr, void*, new_addr, new_size);
          }
          else
          {
            __omTypeRealloc(old_addr, void*, new_addr, new_size);
          }
        }
      }
      om_ReportedError = omError_InternalBug;
      (void) (omCheckAddrSize(new_addr, new_size, level) || omCheckBins(level - 2));
      om_ReportedError = 0;
      return new_addr;
    }
  }
  om_ReportedError = 0;
  return _omCheckAlloc(new_size, zero_aligned_size, 0);
}

/*******************************************************************
 *  
 *   Top-Level: Checking addresses 
 *  
 *******************************************************************/
omError_t omCheckBinAddr(void* addr, int level)
{
  return _omCheckBinAddr(addr, NULL, 0, 0, level);
}

omError_t omCheckBinAddrBin(void* addr, omBin bin, int level)
{
  return _omCheckBinAddr(addr, bin, 0, 2, level);
}

omError_t omCheckAddr(void* addr, int level)
{
  return _omCheckAddr(addr, 0, 0, level);
}

omError_t omCheckAddrSize(void* addr, size_t size, int level)
{
  return _omCheckAddr(addr, size, 1, level);
}

omError_t omCheckAlignedBinAddr(void* addr, int level)
{
  return _omCheckBinAddr(addr, NULL, 0, 4, level);
}

omError_t omCheckAlignedBinAddrBin(void* addr, omBin bin, int level)
{
  return _omCheckBinAddr(addr, bin, 0, 6, level);
}

omError_t omCheckAlignedAddr(void* addr, int level)
{
  return _omCheckAddr(addr, 0, 4, level);
}

omError_t omCheckAlignedAddrSize(void* addr, size_t size, int level)
{
  return _omCheckAddr(addr, size, 5, level);
}

/*******************************************************************
 *  
 *   Low-Level: Checking addresses 
 *  
 *******************************************************************/
#define omIsAddrInValidRange(addr) 1

static omError_t _omCheckAddr(void* addr, size_t size, int exact_size, int level)
{
  if (level <= 0) return omError_NoError;

  if (addr == NULL) return omReportError(omError_NullAddr, NULL);

  if (((long) addr  & (SIZEOF_OM_ALIGNMENT - 1)))
    return omReportError(omError_UnalignedAddr, NULL);

  if (! omIsAddrInValidRange(addr))
    return omReportError(omError_InvalidRange, NULL);
  
  if (omIsBinPageAddr(addr))
  {
    return _omCheckBinAddr(addr, NULL, size, exact_size, level);
  }
  else
  {
    return _omCheckLargeAddr(addr, size, exact_size, level);
  }
}

static omError_t _omCheckLargeAddr(void* addr, size_t size, int exact_size, int level)
{
  if (level <= 0) return omError_NoError;

  if (addr == NULL) return ! omReportError(omError_NullAddr, NULL);

  if (((long) addr  & (SIZEOF_OM_ALIGNMENT - 1)))
    return omReportError(omError_UnalignedAddr, "for large addr %p", addr);

  if (!omIsAddrInValidRange(addr))
    return omReportError(omError_InvalidRange, "for large addr %p", addr);

  if (omIsBinPageAddr(addr) || (level > 1 && omCheckIsBinPageAddr(addr)))
    return omReportError(omError_InternalBug, "for large addr %p", addr);

  if ((exact_size & 4) && ((long) addr & (SIZEOF_STRICT_ALIGNMENT - 1)))
    return omReportError(omError_UnalignedAddr, "for addr %p mod %d == %d",
                         addr, SIZEOF_STRICT_ALIGNMENT, ((long) addr & (SIZEOF_STRICT_ALIGNMENT - 1)));
    
  if (level > 1 && omIsAddrFromBinPage(addr))
    return omReportError(omError_FreedAddrOrMemoryCorrupted, 
                         "omIsLargeAddr == 1 but omIsAddrFromBinPage == 1 for addr %p", addr);

  if (omSizeOfLargeAddr(addr) <= OM_MAX_BLOCK_SIZE)
    return omReportError(omError_FalseAddrOrMemoryCorrupted,
                         "large addr %p has size %u not larger than %d",  addr,
                         omSizeOfLargeAddr(addr), OM_MAX_BLOCK_SIZE);
  
  if ((exact_size & 1) && omSizeOfLargeAddr(addr) < OM_ALIGN_SIZE(size))
    return ! omReportError(omError_WrongSize,
                           "large addr %p has specified size %u but should have size %u",
                     addr, OM_ALIGN_SIZE(size), omSizeOfLargeAddr(addr));
  return om_ErrorStatus = omError_NoError;
}

#define omReportBinAddrError(error, fmt) \
  omReportError(error, "%s for BinAddr %p: bin %p, size %p", fmt, addr, bin, size) 

static omError_t _omCheckBinAddr(void* addr, omBin bin, size_t size, int exact, int level)
{
  omBinPage page;
  omBin h_bin;

  if (level <= 0) return omError_NoError;

  if (addr == NULL) return ! omReportBinAddrError(omError_NullAddr, "");

  if (((long) addr  & (SIZEOF_OM_ALIGNMENT - 1)))
    return omReportBinAddrError(omError_UnalignedAddr, "");

  if (! omIsBinPageAddr(addr))
    return omReportBinAddrError(omError_NotBinAddr, "");

  if ((exact & 4) && ((long) addr & (SIZEOF_STRICT_ALIGNMENT - 1)))
    return omReportError(omError_UnalignedAddr, "for addr %p mod %d == %d",
                         addr, SIZEOF_STRICT_ALIGNMENT, ((long) addr & (SIZEOF_STRICT_ALIGNMENT - 1)));

  if (level > 1 && omIsAddrOnFreeBinPage(addr))
    return omReportBinAddrError(omError_FreedAddr, "");

  if (level > 1 && ! omCheckIsBinPageAddr(addr))
    return omReportError(omError_InternalBug, "omIsBinPageAddr != omCheckIsBinPageAddr");

  if (level > 1 && !omIsAddrFromBinPage(addr))
    return omReportBinAddrError(omError_FreedAddrOrMemoryCorrupted, 
                                "omIsBinPageAddr == 1 but omIsAddrFromBinPage == 1");

  if (exact & 2)
  {
    if (bin != omGetBinOfAddr(addr))
      return omReportBinAddrError(omError_WrongBin, "");
  }
  else
  {
    bin = omGetBinOfAddr(addr);
  }

  if (exact & 1)
  {
    int size_error = 0;
    h_bin = omGetTopBinOfAddr(addr);
    if (size > OM_MAX_BLOCK_SIZE || ! omIsStaticBin(h_bin))
    {
      if (exact & 4) h_bin = omGetAlignedSpecBin(size);
      else h_bin = omGetSpecBin(size);
      if (h_bin->sizeW != bin->sizeW)
      {
#ifdef OM_ALIGNMENT_NEEDS_WORK
        if ( (exact & 4) || (((long) addr & (SIZEOF_STRICT_ALIGNMENT -1))) )
        {
          size_error = 1;
        }
        else
        {
          omUnGetSpecBin(&h_bin);
          h_bin = omGetAlignedSpecBin(size);
          if (h_bin->sizeW != bin->sizeW)
          {
            size_error = 1;
          }
        }
#else
        size_error = 1;
#endif
      }
      if (size_error)
      {
        omReportError(omError_WrongSize, 
                        "for SpecBin : specified %u, aligned %u should be %u", 
                        size, bin->sizeW*SIZEOF_LONG, h_bin->sizeW*SIZEOF_LONG);
        omUnGetSpecBin(&h_bin);
        return omError_WrongSize;
      }
      else
        omUnGetSpecBin(&h_bin);
    }
    else
    {
      if (exact & 4) h_bin = omSmallSize2AlignedBin(size);
      else h_bin = omSmallSize2Bin(size);
      if (h_bin->sizeW != bin->sizeW)
      {
#ifdef OM_ALIGNMENT_NEEDS_WORK
        h_bin = omSmallSize2AlignedBin(size);
        if ( (exact & 4) || (((long) addr & (SIZEOF_STRICT_ALIGNMENT -1))) ||
             (h_bin->sizeW != bin->sizeW))
#endif
        return omReportError(omError_WrongSize, 
                             "for StaticBin: specified %u, aligned %u should be %u", 
                             size, bin->sizeW*SIZEOF_LONG, h_bin->sizeW*SIZEOF_LONG);
      }
    }
  }

  h_bin = omGetTopBinOfAddr(addr);
  // check bin and page
  if (! omIsKnownTopBin(h_bin)) 
    return omReportBinAddrError(omError_UnknownBin, "Addr not from Bin (Bin unknown)");
  if (omCheckBin(bin, level - 1)) return om_ErrorStatus;

  // check page 
  page = omGetPageOfAddr(addr);
  if (omCheckBinPage(page, level - 1)) return om_ErrorStatus;
  if (omCheckBinPageRegionAddr(addr, page->region, level -1)) return om_ErrorStatus;

  // look that page is in queue of pages of this Bin
  if (level > 1)
  {
    h_bin = omGetBinOfPage(page);
    if ( ! omIsOnGList(h_bin->last_page, prev, page))
      return omReportBinAddrError(omError_FreedAddr, "page of addr not from this bin");
  }

  // check that addr is aligned within bin
  if (bin->max_blocks >= 1)
  {
    if ( ( ( (unsigned long) addr) 
           - ((unsigned long) page) 
           - SIZEOF_OM_BIN_PAGE_HEADER) 
         % (bin->sizeW * SIZEOF_VOIDP)
         != 0)
      return omReportBinAddrError(omError_FalseAddr, "addr unaligned within page");
  }
  
  // check that addr is not on current list of page
  if (level > 1 && omIsOnList(page->current, addr))
    return ! omReportError(omError_FreedAddr, "used addr on free list of page");
  
  // ok
  return om_ErrorStatus = omError_NoError;
}

/*******************************************************************
 *  
 * Top Level: Checking Bins
 *  
 *******************************************************************/
omError_t omCheckBins(int level)
{
  if (level <= 0)
  {
    return omError_NoError;
  }
  else
  {
    int i = 0;
    omSpecBin s_bin;
  
    if (! omCheckBinPageRegions(level)) return om_ErrorStatus;
    
    for (i=0; i<= OM_MAX_BIN_INDEX; i++)
    {
      if (omCheckBin(&om_StaticBin[i], level)) return om_ErrorStatus;
    }
    s_bin = om_SpecBin;
    while (s_bin != NULL)
    {
      if (omCheckBin(s_bin->bin, level)) return om_ErrorStatus;
      s_bin = s_bin->next;
    }
    return om_ErrorStatus = omError_NoError;
  }
}


omError_t omCheckBin(omBin bin, int level)
{
  int where;
  omBinPage page;
  omBin top_bin = bin;
  
  if (level <= 0) return omError_NoError;
  
  if (bin == NULL) return omReportError(omError_NullAddr, "NULL Bin");

  if (! omIsKnownTopBin(bin)) return omReportError(omError_UnknownBin, "TopBin unknown");
  
  if (level > 2 && omGListHasCycle(bin, next)) 
    return omReportError(omError_MemoryCorrupted, "bin->next list has cycle");

  do
  {
    if (bin->last_page == NULL || bin->current_page == om_ZeroPage)
    {
      if (! (bin->current_page == om_ZeroPage && bin->last_page == NULL))
        return omReportError(omError_MemoryCorrupted, "current_page out of sync with last_page");
      continue;
    }

    if (! omCheckBinPage(bin->current_page, level-1)) return om_ErrorStatus;

    if (bin->current_page != bin->last_page  && 
        ! omCheckBinPage(bin->last_page, level- 1)) return om_ErrorStatus;
    
    if (bin->last_page->next != NULL) 
      return omReportError(omError_MemoryCorrupted, "last_page->next != NULL");

    if (level == 0) continue;
    
    /* check page list for cycles */
    if (level > 2)
    {
      if (omGListHasCycle(bin->last_page, prev))
        return omReportError(omError_MemoryCorrupted, "prev chain of bin has cycles");
      page = omGListLast(bin->last_page, prev);
      if (omGListHasCycle(page, next))
        return omReportError(omError_MemoryCorrupted, "next chain of bin has cycles");
    }

    /* check that current_page is on list of pages */
    if (! omIsOnGList(bin->last_page, prev, bin->current_page))
      return omReportError(omError_MemoryCorrupted, "current_page not in page list");

    /* now check single pages */
    page = bin->last_page;
    where = 1;
    while (page != NULL)
    {
      if (page != bin->last_page && page != bin->current_page &&
          ! omCheckBinPage(page, level - 1))
        return om_ErrorStatus;

      if (page != bin->last_page && 
          (page->next == NULL || page->next->prev != page))
        return omReportError(omError_MemoryCorrupted, "page->next wrong");
      if (page->prev != NULL && page->prev->next != page)
        return omReportError(omError_MemoryCorrupted, "page->prev wrong");

      if (omGetTopBinOfPage(page) != top_bin)
        return omReportError(omError_MemoryCorrupted, "TopBin of page wrong");

      if (omGetStickyOfPage(page) != bin->sticky)
        return omReportError(omError_MemoryCorrupted, "Sticky of page wrong");

      if (omGetBinOfPage(page) != bin)
        return omReportError(omError_MemoryCorrupted, "Bin of Page wrong");
       
      if (where == -1)
      {
        if (page->used_blocks != 0 || page->current != NULL)
          return omReportError(omError_MemoryCorrupted, "used_blocks and/or current of full page out of sync");
      }
      else
      {
        if (page == bin->current_page)
        {
          where = -1;
        }
        else
        {
          if (page->current == NULL ||
              page->used_blocks < 0 ||
              page->used_blocks == bin->max_blocks - 1)
            return omReportError(omError_MemoryCorrupted, "used_blocks and current of not full page out of sync");
        }
      }
      page = page->prev;
    }
  }
  while ((bin = bin->next) != NULL);
  
  return om_ErrorStatus = omError_NoError;
}


/*******************************************************************
 *  
 * Misc things
 *  
 *******************************************************************/

static omError_t omCheckBinPage(omBinPage page, int level)
{
  omBin bin;
  
  if (level <= 0) return omError_NoError;
  
  if (page == NULL) return omReportError(omError_InternalBug, "NULL page");
  if (page != omGetPageOfAddr(page)) return omReportError(omError_InternalBug, "page %p unaligned", page);

  if (! omCheckBinPageRegion(page->region, level)) return om_ErrorStatus;
  
  bin = omGetTopBinOfPage(page);
  if (bin->max_blocks > 1)
  {
    if (page->used_blocks > bin->max_blocks - 1)
      return omReportError(omError_MemoryCorrupted, "used_blocks of page > max_blocks - 1");
  
    if (page->used_blocks == bin->max_blocks - 1 && 
        page->current != NULL)
      return omReportError(omError_MemoryCorrupted, "used_blocks and current out of sync");

    if (page->used_blocks < 0)
      return omReportError(omError_MemoryCorrupted, "used_blocks of page < 0");
  }
  else
  {
    if (page->used_blocks != 0)
      return omReportError(omError_MemoryCorrupted, "used_blocks != 0 of with max_blocks <= 1");
  }
  
  if (page->current == NULL &&
      ( ! (page->used_blocks == 0 || 
           page->used_blocks == bin->max_blocks - 1 ||
           bin->max_blocks < 1)))
    return omReportError(omError_MemoryCorrupted, "used_blocks and NULL current out of sync");
  
  if (level > 2 && omListHasCycle(page->current))
    return omReportError(omError_MemoryCorrupted, "current list has cycle");
  
  if (level > 1)
  {
    void* current = page->current;
    int i = 1;
    if (current != NULL &&
        omListLength(current) != bin->max_blocks - page->used_blocks - 1)
      return omReportError(omError_MemoryCorrupted, "used_blocks and length of current out of sync");

    while (current != NULL)
    {
      if (omGetPageOfAddr(current) != page) 
        return omReportError(omError_MemoryCorrupted, "current has address not from page");
      
      if ( ( ( (unsigned long) current) 
             - ((unsigned long) page) 
             - SIZEOF_OM_BIN_PAGE_HEADER) 
           % (bin->sizeW * SIZEOF_LONG)
           != 0)
        return omReportError(omError_MemoryCorrupted, "current has unaligned adress");
      current = *((void**) current);
      i++;
    }
  }
  return om_ErrorStatus = omError_NoError;
}

static int omIsKnownTopBin(omBin bin)
{
  int i = 0;
  omSpecBin s_bin;
  
  for (i=0; i<= OM_MAX_BIN_INDEX; i++)
  {
    if (bin == &om_StaticBin[i]) return 1;
  }
  
  s_bin = om_SpecBin;
  while (s_bin != NULL)
  {
    if (bin == s_bin->bin) return 1;
    s_bin = s_bin->next;
  }
  
  return 0;
}

int omIsAddrFromBinPage(void* addr)
{
  int i = 0;
  omSpecBin s_bin;
  omBin bin;
  void* page = omGetPageOfAddr(addr);

  for (i=0; i<= OM_MAX_BIN_INDEX; i++)
  {
    bin = &om_StaticBin[i];
    while (bin != NULL)
    {
      if (omIsOnGList(bin->last_page, prev, page))
        return 1;
      bin = bin->next;
    }
  }

  s_bin = om_SpecBin;
  while (s_bin != NULL)
  {
    bin = s_bin->bin;
    while (bin != NULL)
    {
      if (omIsOnGList(bin->last_page, prev, page))
        return 1;
      bin = bin->next;
    }
    s_bin = s_bin->next;
  }
  return 0;
}

