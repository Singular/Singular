/*******************************************************************
 *  File:    omBin.c
 *  Purpose: definitions of routines for working with bins
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

#include "omalloc.h"
/* this should go away */

#ifdef HAVE_OMALLOC

#ifdef OM_INTERNAL_DEBUG
size_t omSizeOfBinAddr(void* addr)
{
  return _omSizeOfBinAddr(addr);
}
#endif

/*****************************************************************
 *
 * SpecBin business
 *
 *****************************************************************/
#define om_LargeBin ((omBin) 1)
omBin _omGetSpecBin(size_t size, int align, int track)

{
  omBin om_new_specBin;
  long max_blocks;
  long sizeW;


  size = OM_ALIGN_SIZE(size);
#ifdef OM_ALIGNMENT_NEEDS_WORK
  if (align || size >= OM_SIZEOF_UNIQUE_MAX_BLOCK_THRESHOLD)
  {
    align = 1;
    size = OM_STRICT_ALIGN_SIZE(size);
  }
#else
  align = 0;
#endif

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
    /* SIZEOF_OM_BIN_PAGE == max_blocks*size + r1; r1 < size */
    /* r1 = max_blocks*(size_padding) + r2; r2 < max_blocks */
    max_blocks = SIZEOF_OM_BIN_PAGE / size;
    sizeW = (SIZEOF_OM_BIN_PAGE % size) / max_blocks;
#ifdef OM_ALIGNMENT_NEEDS_WORK
    if (align)
      sizeW = ((size + sizeW) & ~ (SIZEOF_STRICT_ALIGNMENT - 1));
    else
#endif
      sizeW = ((size + sizeW) & ~ (SIZEOF_OM_ALIGNMENT - 1));

    omAssume(sizeW >= size);
    omAssume(max_blocks*sizeW <= SIZEOF_OM_BIN_PAGE);
    omAssume((max_blocks+1)*sizeW > SIZEOF_OM_BIN_PAGE ||
             max_blocks*(sizeW + SIZEOF_STRICT_ALIGNMENT) > SIZEOF_OM_BIN_PAGE);

    sizeW = sizeW >> LOG_SIZEOF_LONG;
    if (size > OM_MAX_BLOCK_SIZE)
    {
      om_new_specBin = om_LargeBin;
    }
#ifdef OM_ALIGNMENT_NEEDS_WORK
    else if (align)
    {
      om_new_specBin = omSmallSize2AlignedBin( size );
    }
#endif
#ifdef OM_HAVE_TRACK
    else if (track)
    {
      om_new_specBin = omSmallSize2TrackBin( size );
    }
#endif
    else
    {
      om_new_specBin = omSmallSize2Bin( size );
    }
  }

  if (om_new_specBin == om_LargeBin ||
      om_new_specBin->max_blocks < max_blocks)
  {
    omSpecBin s_bin;
#ifdef OM_HAVE_TRACK
    if (track)
      s_bin = omFindInSortedGList(om_SpecTrackBin, next, max_blocks, max_blocks);
    else
#endif
      s_bin = omFindInSortedGList(om_SpecBin, next, max_blocks, max_blocks);

    if (s_bin != NULL)
    {
      (s_bin->ref)++;
      omAssume(s_bin->bin != NULL &&
             s_bin->bin->max_blocks == s_bin->max_blocks &&
             s_bin->bin->sizeW == sizeW);
      return s_bin->bin;
    }
    s_bin = (omSpecBin) omAlloc(sizeof(omSpecBin_t));
    s_bin->ref = 1;
    s_bin->next = NULL;
    s_bin->max_blocks = max_blocks;
    s_bin->bin = (omBin) omAlloc(sizeof(omBin_t));
    s_bin->bin->current_page = om_ZeroPage;
    s_bin->bin->last_page = NULL;
    s_bin->bin->next = NULL;
    s_bin->bin->sizeW = sizeW;
    s_bin->bin->max_blocks = max_blocks;
    s_bin->bin->sticky = 0;
#ifdef OM_HAVE_TRACK
    if (track)
    {
      om_SpecTrackBin = omInsertInSortedGList(om_SpecTrackBin, next, max_blocks, s_bin);
    }
    else
#endif
      om_SpecBin = omInsertInSortedGList(om_SpecBin, next, max_blocks, s_bin);
    return s_bin->bin;
  }
  else
  {
    return om_new_specBin;
  }
}

void _omUnGetSpecBin(omBin *bin_p, int force)
{
  omBin bin = *bin_p;
  if (! omIsStaticBin(bin))
  {
#ifdef OM_HAVE_TRACK
    int track_bin = 0;
#endif
    omSpecBin s_bin;

#ifdef OM_HAVE_TRACK
    s_bin = omFindInGList(om_SpecTrackBin, next, bin, bin);
    if (s_bin != NULL)
      track_bin = 1;
    else
#endif
        s_bin = omFindInSortedGList(om_SpecBin, next, max_blocks, bin->max_blocks);

    omAssume(s_bin != NULL && bin == s_bin->bin);
    if (s_bin != NULL)
    {
      (s_bin->ref)--;
      if (s_bin->ref == 0 || force)
      {
#ifdef OM_HAVE_TRACK
        if (! track_bin)
#endif
          omFreeKeptAddrFromBin(s_bin->bin);
        if(s_bin->bin->last_page == NULL || force)
        {
#ifdef OM_HAVE_TRACK
          if (track_bin)
            om_SpecTrackBin = omRemoveFromSortedGList(om_SpecTrackBin, next, max_blocks, s_bin);
          else
#endif
            om_SpecBin = omRemoveFromSortedGList(om_SpecBin, next, max_blocks, s_bin);
          omFreeSize(s_bin->bin, sizeof(omBin_t));
          omFreeSize(s_bin, sizeof(omSpecBin_t));
        }
      }
    }
  }
  *bin_p = NULL;
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
  omBin s_bin = (omBin) omAlloc(sizeof(omBin_t));
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
  omAssume(to_bin->sizeW == from_bin->sizeW);
  omAssume(to_bin != from_bin);

  if (page == NULL) return;
  do
  {
    omSetTopBinAndStickyOfPage(page, to_bin, to_bin->sticky);
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
    omFreeSize(sticky_bin, sizeof(omBin_t));
  }
}


/*****************************************************************
 *
 * Sticky bins
 *
 *****************************************************************/
omBin om_StickyBins = NULL;
omBin omGetStickyBinOfBin(omBin bin)
{
  omBin new_bin = omAlloc(sizeof(omBin_t));
  omAssume(omIsKnownTopBin(bin, 1) && ! omIsStickyBin(bin));
  new_bin->sticky = SIZEOF_VOIDP;
  new_bin->max_blocks = bin->max_blocks;
  new_bin->sizeW = bin->sizeW;
  new_bin->next = om_StickyBins;
  om_StickyBins = new_bin;
  new_bin->last_page = NULL;
  new_bin->current_page = om_ZeroPage;
#if 0
  if (omIsSpecBin(bin))
  {
    omSpecBin s_bin = omFindInSortedGList(om_SpecBin, next, max_blocks, bin->max_blocks);
    omAssume(s_bin != NULL);
    if (s_bin != NULL)
      s_bin->ref++;
  }
#endif
  return new_bin;
}

void omMergeStickyBinIntoBin(omBin sticky_bin, omBin into_bin)
{
  if (! omIsOnGList(om_StickyBins, next, sticky_bin) ||
      !sticky_bin->sticky ||
      sticky_bin->max_blocks != into_bin->max_blocks ||
      sticky_bin == into_bin ||
      !omIsKnownTopBin(into_bin, 1) ||
      omIsStickyBin(into_bin))
  {
#ifndef OM_NDEBUG
    omReportError(omError_StickyBin, omError_NoError, OM_FLR,
                  (! omIsOnGList(om_StickyBins, next, sticky_bin)  ? "unknown sticky_bin" :
                   (!sticky_bin->sticky ? "sticky_bin is not sticky" :
                    (sticky_bin->max_blocks != into_bin->max_blocks ? "sticky_bin and into_bin have different block sizes" :
                     (sticky_bin == into_bin ? "sticky_bin == into_bin" :
                      (!omIsKnownTopBin(into_bin, 1) ? "unknown into_bin" :
                       (omIsStickyBin(into_bin) ? "into_bin is sticky" :
                        "unknown sticky_bin error")))))));
#endif
    return;
  }
  omFreeKeptAddrFromBin(sticky_bin);
  om_StickyBins = omRemoveFromGList(om_StickyBins, next, sticky_bin);
  omMergeStickyPages(into_bin, sticky_bin);

#if 0
  if (! omIsStaticBin(into_bin))
  {
    omBin _ibin = into_bin;
    omUnGetSpecBin(&_ibin);
  }
#endif
  omFreeSize(sticky_bin, sizeof(omBin_t));
#if defined(OM_INTERNAL_DEBUG) && !defined(OM_NDEBUG)
  omTestBin(into_bin, 2);
#endif
}

/*****************************************************************
*
* AllBin business
*
*****************************************************************/
#ifndef OM_NDEBUG
int omIsKnownTopBin(omBin bin, int normal_bin)
{
  omBin to_check;
  omSpecBin s_bin;
  int i;

  omAssume(normal_bin == 1 || normal_bin == 0);

#ifdef OM_HAVE_TRACK
  if (! normal_bin)
  {
    to_check = om_StaticTrackBin;
    s_bin = om_SpecTrackBin;
  }
  else
#endif
  {
    omAssume(normal_bin);
    to_check = om_StaticBin;
    s_bin = om_SpecBin;
  }

  for (i=0; i<= OM_MAX_BIN_INDEX; i++)
  {
    if (bin == &(to_check[i]))
      return 1;
  }

  while (s_bin != NULL)
  {
    if (bin == s_bin->bin) return 1;
    s_bin = s_bin->next;
  }
  to_check = om_StickyBins;

  while (to_check != NULL)
  {
    if (bin == to_check) return 1;
    to_check = to_check->next;
  }
  return 0;
}
#endif

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

#if 0
void omPrintMissing(omBin bin)
{
  omBinPage page = bin->last_page;

  while (page != NULL)
  {
    void* addr = (void*) page + SIZEOF_OM_BIN_PAGE_HEADER;
    int i;

    for (i=0; i<bin->max_blocks; i++)
    {
      if (! omIsOnList(page->current, addr))
        printf("%d:%p\n", i, addr);
      addr += bin->sizeW*SIZEOF_LONG;
    }
    page = page->prev;
  }
}
#endif

/*****************************************************************
 *
 * Statistics
 *
 *****************************************************************/
static void omGetBinStat(omBin bin, long *pages_p, long *used_blocks_p,
                          long *free_blocks_p)
{
  long pages = 0, used_blocks = 0, free_blocks = 0;
  int where = 1;

  omBinPage page = bin->last_page;
  while (page != NULL)
  {
    pages++; if (where == 1)
    {
      used_blocks += omGetUsedBlocksOfPage(page) + 1;
      if (bin->max_blocks > 0)
        free_blocks += bin->max_blocks - omGetUsedBlocksOfPage(page) -1;
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

static void omGetTotalBinStat(omBin bin, long *pages_p, long *used_blocks_p,
                               long *free_blocks_p)
{
  long t_pages = 0, t_used_blocks = 0, t_free_blocks = 0;
  long pages = 0, used_blocks = 0, free_blocks = 0;

  while (bin != NULL)
  {
    omGetBinStat(bin, &pages, &used_blocks, &free_blocks);
    t_pages += pages;
    t_used_blocks += used_blocks;
    t_free_blocks += free_blocks;
    if (!omIsStickyBin(bin))
      bin = bin->next;
    else
      bin = NULL;
  }
  *pages_p = t_pages;
  *used_blocks_p = t_used_blocks;
  *free_blocks_p = t_free_blocks;
}

static void omPrintBinStat(FILE * fd, omBin bin, int track, long* pages, long* used_blocks, long* free_blocks)
{
  if (track)
  {
    fputs("T \t \t",fd);
  }
  else
  {
    fprintf(fd, "%s%ld\t%ld\t", (omIsStaticNormalBin(bin) ? " " :
                                (omIsStickyBin(bin) ? "S" :
                                 (omIsTrackBin(bin) ? "T" : "*"))),
            (long)bin->sizeW, bin->max_blocks);
  }
  omGetTotalBinStat(bin, pages, used_blocks, free_blocks);
  fprintf(fd, "%ld\t%ld\t%ld\n", *pages, *free_blocks, *used_blocks);
  if (bin->next != NULL && !omIsStickyBin(bin))
  {
    long s_pages, s_free_blocks, s_used_blocks;
    while (bin != NULL)
    {
      omGetBinStat(bin, &s_pages, &s_used_blocks, &s_free_blocks);
      fprintf(fd, " \t \t%ld\t%ld\t%ld\t%d\n", s_pages, s_free_blocks, s_used_blocks,
              (int) bin->sticky);
      bin = bin->next;
      *pages += s_pages;
      *used_blocks += s_used_blocks;
      *free_blocks += s_free_blocks;
    }
  }
}

void omPrintBinStats(FILE* fd)
{
  int i = OM_MAX_BIN_INDEX;
  long pages=0, used_blocks=0, free_blocks=0;
  long pages_p, used_blocks_p, free_blocks_p;
  omSpecBin s_bin = om_SpecBin;
  omBin sticky;

  fputs(" SizeW\tBlocks\tUPages\tFBlocks\tUBlocks\tSticky\n",fd);
  fflush(fd);
  while (s_bin != NULL || i >= 0)
  {
    if (s_bin == NULL || (i >= 0 && (unsigned long) om_StaticBin[i].max_blocks < (unsigned long) s_bin->bin->max_blocks))
    {
       omPrintBinStat(fd, &om_StaticBin[i], 0, &pages_p, &used_blocks_p, &free_blocks_p);
       pages += pages_p;
       used_blocks += used_blocks_p;
       free_blocks += free_blocks_p;
#ifdef OM_HAVE_TRACK
       if (om_StaticTrackBin[i].current_page != om_ZeroPage)
       {
         omPrintBinStat(fd, &om_StaticTrackBin[i], 1, &pages_p, &used_blocks_p, &free_blocks_p);
         pages += pages_p;
         used_blocks += used_blocks_p;
         free_blocks += free_blocks_p;
       }
#endif
       i--;
    }
    else
    {
      omPrintBinStat(fd, s_bin->bin,0, &pages_p, &used_blocks_p, &free_blocks_p);
      pages += pages_p;
      used_blocks += used_blocks_p;
      free_blocks += free_blocks_p;
      s_bin = s_bin->next;
    }
  }
#ifdef OM_HAVE_TRACK
  s_bin = om_SpecTrackBin;
  while (s_bin != NULL)
  {
    omPrintBinStat(fd, s_bin->bin, 0,  &pages_p, &used_blocks_p, &free_blocks_p);
    s_bin = s_bin->next;
    pages += pages_p;
    used_blocks += used_blocks_p;
    free_blocks += free_blocks_p;
  }
#endif
  sticky = om_StickyBins;
  while (sticky != NULL)
  {
    omPrintBinStat(fd, sticky, 0,  &pages_p, &used_blocks_p, &free_blocks_p);
    sticky = sticky->next;
    pages += pages_p;
    used_blocks += used_blocks_p;
    free_blocks += free_blocks_p;
  }
  fputs("----------------------------------------\n",fd);
  fprintf(fd, "      \t      \t%ld\t%ld\t%ld\n", pages, free_blocks, used_blocks);
}

static long omGetUsedBytesOfBin(omBin bin)
{
  long pages = 0, used_blocks = 0, free_blocks = 0;
  omGetTotalBinStat(bin, &pages, &used_blocks, &free_blocks);
  return (used_blocks)*((long)bin->sizeW)*SIZEOF_LONG;
}

long omGetUsedBinBytes()
{
  int i = OM_MAX_BIN_INDEX;
  omSpecBin s_bin = om_SpecBin;
  long used = 0;
  omBin sticky;

  for (; i>=0; i--)
  {
    used += omGetUsedBytesOfBin(&om_StaticBin[i]);
  }
  while (s_bin != NULL)
  {
    used += omGetUsedBytesOfBin(s_bin->bin);
    s_bin = s_bin->next;
  }
#ifdef OM_HAVE_TRACK
  for (i=OM_MAX_BIN_INDEX; i>=0; i--)
  {
    used += omGetUsedBytesOfBin(&om_StaticTrackBin[i]);
  }
  s_bin = om_SpecTrackBin;
  while (s_bin != NULL)
  {
    used += omGetUsedBytesOfBin(s_bin->bin);
    s_bin = s_bin->next;
  }
#endif

  sticky = om_StickyBins;
  while (sticky != NULL)
  {
    used += omGetUsedBytesOfBin(sticky);
    sticky = sticky->next;
  }
  return used;
}
#endif
