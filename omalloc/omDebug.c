/*******************************************************************
 *  File:    omDebug.c
 *  Purpose: implementation of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omDebug.c,v 1.2 1999-11-22 18:12:58 obachman Exp $
 *******************************************************************/
#include "omConfig.h"
#include "omPrivate.h"
#include "omLocal.h"
#include "omList.h"
#include "omDebug.h"

static int omdCheckBinAddrSize(void* addr, size_t size, int level);
static int omdPrintBinError(const char* msg, void* addr, size_t size,
                            omBin bin);
static const char* omdTestBinAddrSize(void* addr, size_t size, int level, 
                                      omBin *bin);
static const char* omdTestBin(omBin bin, int level);
static const char* omdTestBinPage(omBinPage page, int level);
static int   omdIsKnownTopBin(omBin bin);

/*******************************************************************
 *  
 *   Checking addresses 
 *  
 *******************************************************************/
int omdCheckBinAddr(void* addr, int level)
{
  return omdCheckBinAddrSize(addr, 0, level);
}

int omdCheckBlockAddr(void* addr, size_t size, int level)
{
  if (size == 0)
  {
    omError("0 size");
    return 0;
  }
  if (size <= OM_MAX_BLOCK_SIZE)
    return omdCheckBinAddrSize(addr, size, level);
  else
    return 1;
}

int omdCheckChunkAddr(void* addr, int level)
{
  omBinPage page;
  addr = addr - SIZEOF_OM_ALIGNMENT;
  page = *((omBinPage*) addr);
  if (page != om_LargePage)
    return omdCheckBinAddrSize(addr, -1, level);
  else
    return 1;
}
    
/*******************************************************************
 *  
 * Checking Bins
 *  
 *******************************************************************/
static int omdIsKnownTopBin(omBin bin)
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

int omdCheckBin(omBin bin, int level)
{
  const char* msg = omdTestBin(bin, level);
  if (msg != NULL) 
  {
    omdPrintBinError(msg, NULL, 0, bin);
    return 0;
  }
  return 1;
}

int omdCheckBins(int level)
{
  if (level <= 0)
  {
    return 1;
  }
  else
  {
    
    int i = 0;
    omSpecBin s_bin;
  
    for (i=0; i<= OM_MAX_BIN_INDEX; i++)
    {
      if (! omdCheckBin(&om_StaticBin[i], level)) return 0;
    }
    s_bin = om_SpecBin;
    while (s_bin != NULL)
    {
      if (! omdCheckBin(s_bin->bin, level)) return 0;
      s_bin = s_bin->next;
    }
    return 1;
  }
}

/*******************************************************************
 *  
 *  CheckAlloc CheckFree
 *  
 *******************************************************************/

void* omdCheckAllocBin(omBin bin, const int zero, int level)
{
  void* res;
  
  omdCheckBins(level-2);

  if (zero)
    __omTypeAlloc0Bin(void*, res, bin);
  else
    __omTypeAllocBin(void*, res, bin);

  omdCheckBinAddr(res, level-1);
  omdCheckBins(level-2);

  return res;
}
  
void  omdCheckFreeBin(void* addr, int level)
{
  omdCheckBins(level-2);

  if (omdCheckBinAddr(addr, level))
    __omFreeBin(addr);

  omdCheckBins(level - 2);
}

void* omdCheckAllocBlock(size_t size, const int zero, int level)
{
  void* res;
  
  if (level > 0 && size <= 0) 
  {
    omdPrintBinError("requested AllocBlock size <= 0", 
                     NULL, size, NULL);
    size = 1;
  }

  omdCheckBins(level-2);

  if (zero)
    __omTypeAlloc0Block(void*, res, size);
  else
    __omTypeAllocBlock(void*, res, size);
  
  omdCheckBlockAddr(res, size, level - 1);
  omdCheckBins(level - 2);

  return res;
}

void omdCheckFreeBlock(void* addr, size_t size, int level)
{
  omdCheckBins(level - 2);
  if (! omdCheckBlockAddr(addr, size, level)) return;
  
  __omFreeBlock(addr, size);
  
  omdCheckBins(level - 2);
}


void* omdCheckAllocChunk(size_t size, const int zero, int level)
{
  void* res;
  
  if (level > 0 && size <= 0) 
  {
    omdPrintBinError("requested AllocBlock size <= 0", 
                     NULL, size, NULL);
    size = 1;
  }
  omdCheckBins(level - 2);

  if (zero)
    __omTypeAlloc0Chunk(void*, res, size);
  else
    __omTypeAllocChunk(void*, res, size);
  
  omdCheckChunkAddr(res, level - 1);
  omdCheckBins(level - 2);

  return res;
}

void omdCheckFreeChunk(void* addr, int level)
{
  if (! omdCheckChunkAddr(addr, level)) return;
  
  __omFreeChunk(addr);
  
  omdCheckBins(level - 2);
}


/*******************************************************************
 *  
 *  Checking a bin address
 *  
 *******************************************************************/
static int omdPrintBinError(const char* msg, void* addr, size_t size,omBin bin)
{
  fprintf(stderr, 
          "for addr:%p (%d) bin:%p (%ld:%ld)\n",
          addr, size, (void*) bin, 
          (bin != NULL ? bin->max_blocks : 0), 
          (bin != NULL ? bin->sizeW : 0));
  fflush(stderr);
  return 0;
}

static int omdCheckBinAddrSize(void* addr, size_t size, int level)
{
  omBin bin = NULL;
  const char* msg = omdTestBinAddrSize(addr, size, level, &bin);
  if (msg != NULL)
    return omdPrintBinError(msg, addr, size, bin);
  else
    return 1;
}


/* Check that addr is activ (used) adr of bin */
static const char* 
omdTestBinAddrSize(void* addr, size_t size, int level, omBin* r_bin)
{
  omBin bin;
  omBinPage page;
  omBin h_bin;
  const char* msg;
  
  if (level <= 0) return NULL;
  
  if (addr == NULL) return omError("NULL addr");

  if (size > OM_MAX_BLOCK_SIZE) return NULL;

  bin = omGetTopBinOfAddr(addr);
  
  if (bin == NULL) return omError("NULL Bin");

  if (! omdIsKnownTopBin(bin))
    return omError("Addr not from Bin (Bin unknown)");
  *r_bin = bin;
  
  if ((msg = omdTestBin(bin, level - 1)) != NULL) return msg;

  // check for right bin
  if (size > 0)
  {
    if (omIsStaticBin(bin))
      h_bin = omSize2Bin(size);
    else
      h_bin = omGetSpecBin(size);
    if (bin != h_bin) return omError("size is wrong");
    if (! omIsStaticBin(bin)) omUnGetSpecBin(&h_bin);
  }

  // check page 
  page = omGetPageOfAddr(addr);
  if ((msg = omdTestBinPage(page, level - 1)) != NULL) return msg;
  
  // look that page is in queue of pages of this Bin
  h_bin = omGetBinOfPage(page);
  *r_bin = h_bin;
  if ( ! omIsOnGList(h_bin->last_page, prev, page))
    return omError("page of addr not from this Bin");

  // check that addr is aligned within bin
  if (bin->max_blocks >= 1)
  {
    if ( ( ( (unsigned long) addr) 
           - ((unsigned long) page) 
           - SIZEOF_OM_BIN_PAGE_HEADER) 
         % (bin->sizeW * SIZEOF_VOIDP)
         != 0)
      return omError("addr unaligned within page");
  }
  
  // check that addr is not on current list of page
  if (level > 0 && omIsOnList(page->current, addr))
    return omError("used addr on free list of page");
  
  return NULL;
}

static const char* omdTestBinPage(omBinPage page, int level)
{
  omBin bin;
  
  if (level <= 0) return NULL;
  
  if (page == NULL) return omError("NULL page");
  if (page != omGetPageOfAddr(page)) return omError("page unaligned");

  bin = omGetTopBinOfPage(page);
  if (bin->max_blocks > 1)
  {
    if (page->used_blocks > bin->max_blocks - 1)
      return omError("used_blocks of page > max_blocks - 1");
  
    if (page->used_blocks == bin->max_blocks - 1 && 
        page->current != NULL)
      return omError("used_blocks and current out of sync");

    if (page->used_blocks < 0)
      return omError("used_blocks of page < 0");
  }
  else
  {
    if (page->used_blocks != 0)
      return omError("used_blocks != 0 of with max_blocks <= 1");
  }
  
  if (page->current == NULL &&
      ( ! (page->used_blocks == 0 || 
            page->used_blocks == bin->max_blocks - 1 ||
           bin->max_blocks < 1)))
    return omError("used_blocks and current out of sync");
  
  if (level > 2 && omListHasCycle(page->current))
    return omError("current list has cycle");
  
  if (level > 1)
  {
    void* current = page->current;
    int i = 1;
    if (current != NULL &&
        omListLength(current) != bin->max_blocks - page->used_blocks - 1)
      return omError("used_blocks and current out of sync");

    while (current != NULL)
    {
      if (omGetPageOfAddr(current) != page) 
        return omError("current has address not from page");
      
      if ( ( ( (unsigned long) current) 
             - ((unsigned long) page) 
             - SIZEOF_OM_BIN_PAGE_HEADER) 
           % (bin->sizeW * SIZEOF_LONG)
           != 0)
        return omError("current has unaligned adress");
      current = *((void**) current);
      i++;
    }
  }
  return NULL;
}

static const char* omdTestBin(omBin bin, int level)
{
  int where;
  omBinPage page;
  omBin top_bin = bin;
  const char* msg;
  
  if (level <= 0) return NULL;
  
  if (bin == NULL) return omError("NULL Bin");

  if (! omdIsKnownTopBin(bin)) return omError("TopBin unknown");
  
  if (level > 2 && 
      omGListHasCycle(bin, next)) 
    return omError("bin->next list has cycle");

  do
  {
    if (bin->last_page == NULL || bin->current_page == om_ZeroPage)
    {
      if (! (bin->current_page == om_ZeroPage && bin->last_page == NULL))
        return omError("current_page out of sync with last_page");
      continue;
    }

    if ((msg = omdTestBinPage(bin->current_page, level-1)) != NULL)
      return msg;

    if ((msg = omdTestBinPage(bin->last_page, level- 1)) != NULL)
      return msg;
    
    if (bin->last_page->next != NULL) 
      return omError("last_page->next != NULL");

    if (level == 0) continue;
    
    /* check page list for cycles */
    if (level > 2)
    {
      if (omGListHasCycle(bin->last_page, prev))
        return omError("prev chain of bin has cycles");
      page = omGListLast(bin->last_page, prev);
      if (omGListHasCycle(page, next))
        return omError("next chain of bin has cycles");
    }

    /* check that current_page is on list of pages */
    if (! omIsOnGList(bin->last_page, prev, bin->current_page))
      return"current_page not in page list";

    /* now check single pages */
    page = bin->last_page;
    where = 1;
    while (page != NULL)
    {
      if (page != bin->last_page && page != bin->current_page &&
          ((msg = omdTestBinPage(page, level - 1)) != NULL))
        return msg;

      if (page != bin->last_page && 
          (page->next == NULL || page->next->prev != page))
        return omError("page->next wrong");
      if (page->prev != NULL && page->prev->next != page)
        return omError("page->prev wrong");

      if (omGetTopBinOfPage(page) != top_bin)
        return omError("TopBin of page wrong");

      if (omGetStickyOfPage(page) != bin->sticky)
        return omError("Sticky of page wrong");

      if (omGetBinOfPage(page) != bin)
        return omError("Bin of Page wrong");
       
      if (where == -1)
      {
        if (page->used_blocks != 0 || page->current != NULL)
          return omError("used_blocks and current of fpage out of sync");
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
            return omError("used_blocks and current of upage out of sync");
        }
      }
      page = page->prev;
    }
  }
  while ((bin = bin->next) != NULL);
  
  return NULL;
}


    



