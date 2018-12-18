/*******************************************************************
 *  File:    omDebug.c
 *  Purpose: implementation of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include <limits.h>
#include <string.h>
#include "omConfig.h"

#ifdef HAVE_OMALLOC
#include "omDerivedConfig.h"

#ifdef OM_HAVE_TRACK
#include "omDefaultConfig.h"
#include "omalloc.h"

/*******************************************************************
 *
 * Declarations
 *
 *******************************************************************/
omBinPage om_JustFreedPage = NULL;
omSpecBin om_SpecTrackBin = NULL;

/* number of bytes for padding before addr: needs to > 0 and a multiple of OM_SIZEOF_STRICT_ALIGNMENT */
#ifndef OM_MIN_SIZEOF_FRONT_PATTERN
#define OM_MIN_SIZEOF_FRONT_PATTERN (OM_MIN_SIZEWOF_FRONT_PATTERN*SIZEOF_STRICT_ALIGNMENT)
#endif
/* number of bytes for padding after addr: needs to be a multiple of OM_SIZEOF_STRICT_ALIGNMENT */
#ifndef OM_MIN_SIZEOF_BACK_PATTERN
#define OM_MIN_SIZEOF_BACK_PATTERN (OM_MIN_SIZEWOF_BACK_PATTERN*SIZEOF_STRICT_ALIGNMENT)
#endif

struct omTrackAddr_s;
typedef struct omTrackAddr_s omTrackAddr_t;
typedef omTrackAddr_t * omTrackAddr;
struct omTrackAddr_s
{
  void*             next;   /* reserved for page->current queue */
  char              track;  /* > 0; determines size of header */
  omTrackFlags_t    flags;
    #ifdef OM_TRACK_FILE_LINE
  short             alloc_line;
  const char*       alloc_file;
    #endif
    #ifdef OM_TRACK_RETURN
  const char*       alloc_r;
    #endif
    #ifdef OM_TRACK_BACKTRACE
      #define OM_TRACK_ADDR_MEM_1 alloc_frames

  /* track > 1 */
  char*             alloc_frames[OM_MAX_KEPT_FRAMES];
    #else
      #define OM_TRACK_ADDR_MEM_1 bin_size
    #endif
    #define OM_TRACK_ADDR_MEM_2 bin_size

  /* track > 2 */
  void*             bin_size;
    #ifdef OM_TRACK_CUSTOM
  void*             custom;
    #endif
    #ifdef OM_TRACK_FILE_LINE
      #define OM_TRACK_ADDR_MEM_3 free_line

  /* track > 3 */
  short             free_line;
  const char*       free_file;
    #endif
    #ifdef OM_TRACK_RETURN
       #ifndef OM_TRACK_ADDR_MEM_3
       #define OM_TRACK_ADDR_MEM_3 free_r
       #endif
  const void*       free_r;
    #endif
    #ifdef OM_TRACK_BACKTRACE
      #define OM_TRACK_ADDR_MEM_4 free_frames

  /* track > 4 */
  void*             free_frames[OM_MAX_KEPT_FRAMES];
    #endif
};

static omError_t omDoCheckTrackAddr(omTrackAddr d_addr, void* addr, void* bin_size, omTrackFlags_t flags, char level,
                                    omError_t report_error, OM_FLR_DECL);
static int omCheckFlags(omTrackFlags_t flag);
static int omCheckPattern(char* s, char p, size_t size);

#define OM_TRACK_MAX 5
static struct omTrackAddr_s track_addr; /* this is only needed to determine OM_SIZEOF_TRACK_ADDR(i) */
#if 0
#define OM_SIZEOF_TRACK_ADDR_1  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.alloc_frames-(char*)&track_addr))
#define OM_SIZEOF_TRACK_ADDR_2  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.bin_size-(char*)&track_addr))
#define OM_SIZEOF_TRACK_ADDR_3  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.free_line-(char*)&track_addr)+OM_MIN_SIZEOF_FRONT_PATTERN)
#define OM_SIZEOF_TRACK_ADDR_4  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.free_frames-(char*)&track_addr)+OM_MIN_SIZEOF_FRONT_PATTERN)
#define OM_SIZEOF_TRACK_ADDR_5  OM_STRICT_ALIGN_SIZE(sizeof(struct omTrackAddr_s)+OM_MIN_SIZEOF_FRONT_PATTERN)
#endif

#define OM_SIZEOF_TRACK_ADDR_1  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.OM_TRACK_ADDR_MEM_1-(char*)&track_addr))
#define OM_SIZEOF_TRACK_ADDR_2  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.OM_TRACK_ADDR_MEM_2-(char*)&track_addr))
#define OM_SIZEOF_TRACK_ADDR_3  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.OM_TRACK_ADDR_MEM_3-(char*)&track_addr)+OM_MIN_SIZEOF_FRONT_PATTERN)
#ifdef OM_TRACK_ADDR_MEM_4
#define OM_SIZEOF_TRACK_ADDR_4  OM_STRICT_ALIGN_SIZE(((char*)&track_addr.OM_TRACK_ADDR_MEM_4-(char*)&track_addr)+OM_MIN_SIZEOF_FRONT_PATTERN)
#else
#define OM_SIZEOF_TRACK_ADDR_4 OM_SIZEOF_TRACK_ADDR_5
#endif
#define OM_SIZEOF_TRACK_ADDR_5  OM_STRICT_ALIGN_SIZE(sizeof(struct omTrackAddr_s)+OM_MIN_SIZEOF_FRONT_PATTERN)

#define OM_SIZEOF_TRACK_ADDR(i)                                                                     \
(i > 3 ?                                                                                            \
 (i == 4 ? OM_SIZEOF_TRACK_ADDR_4 : OM_SIZEOF_TRACK_ADDR_5) :                                       \
  (i == 3 ? OM_SIZEOF_TRACK_ADDR_3 :  (i == 2 ? OM_SIZEOF_TRACK_ADDR_2 : OM_SIZEOF_TRACK_ADDR_1)))

OM_INLINE_LOCAL omTrackAddr omOutAddr_2_TrackAddr(void* addr);

#define _omOutSize_2_TrackAddrSize(size, track)                                                     \
  (size + OM_SIZEOF_TRACK_ADDR(track) + (track > 2 ? OM_MIN_SIZEOF_BACK_PATTERN : 0))

#define _omTrackAddr_2_SizeOfTrackAddrHeader(d_addr) ((size_t) OM_SIZEOF_TRACK_ADDR(((omTrackAddr) (d_addr))->track))
#define _omTrackAddr_2_OutSize(d_addr)                                                                      \
  (((omTrackAddr) (d_addr))->track > 2 ?                                                                    \
   omTrack3Addr_2_OutSize(d_addr) : omSizeOfBinAddr(d_addr) - omTrackAddr_2_SizeOfTrackAddrHeader(d_addr))
#define _omTrack3Addr_2_OutSize(d_addr)                                         \
  ((((omTrackAddr) (d_addr))->flags & OM_FBIN) ?                            \
   (((omBin)((omTrackAddr) (d_addr))->bin_size)->sizeW) << LOG_SIZEOF_LONG :    \
   ((size_t)((omTrackAddr) (d_addr))->bin_size))

/* assume track > 2 */
#define _omTrackAddr_2_FrontPattern(d_addr) \
          ((void*)((unsigned long)d_addr + omTrackAddr_2_SizeOfTrackAddrHeader(d_addr) - OM_MIN_SIZEOF_FRONT_PATTERN))
#define _omTrackAddr_2_SizeOfFrontPattern(d_addr) \
  ((char*) omTrackAddr_2_OutAddr(d_addr) - (char*) omTrackAddr_2_FrontPattern(d_addr))
#define _omTrackAddr_2_BackPattern(d_addr) \
  ((char*) ((unsigned long)d_addr + omTrackAddr_2_SizeOfTrackAddrHeader(d_addr) + _omTrack3Addr_2_OutSize(d_addr)))
#define _omTrackAddr_2_SizeOfBackPattern(d_addr) \
  ((char*) d_addr + omSizeOfBinAddr(d_addr) - omTrackAddr_2_BackPattern(d_addr))
#define omTrackAddr_2_OutAddr(d_addr) ((void*)((unsigned long)d_addr + omTrackAddr_2_SizeOfTrackAddrHeader(d_addr)))


#ifdef OM_INTERNAL_DEBUG
static size_t omTrackAddr_2_SizeOfTrackAddrHeader(omTrackAddr d_addr)
{
  size_t size;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 0 && d_addr->track <= 5);
  size = _omTrackAddr_2_SizeOfTrackAddrHeader(d_addr);
  return size;
}
static void* omTrackAddr_2_FrontPattern(omTrackAddr d_addr)
{
  void* addr;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 2 && d_addr->track <= 5);
  addr = _omTrackAddr_2_FrontPattern(d_addr);
  return addr;
}
static size_t omTrackAddr_2_SizeOfFrontPattern(omTrackAddr d_addr)
{
  size_t size;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 2 && d_addr->track <= 5);
  omAssume((unsigned long) omTrackAddr_2_OutAddr(d_addr) > (unsigned long) omTrackAddr_2_FrontPattern(d_addr));
  size = _omTrackAddr_2_SizeOfFrontPattern(d_addr);
  omAssume(size > 0);
  return size;
}
static char* omTrackAddr_2_BackPattern(omTrackAddr d_addr)
{
  char* addr;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 2 && d_addr->track <= 5);
  addr = _omTrackAddr_2_BackPattern(d_addr);
  omAssume(OM_ALIGN_SIZE((unsigned long) addr) == (unsigned long) addr);
  return addr;
}
static size_t omTrackAddr_2_SizeOfBackPattern(omTrackAddr d_addr)
{
  size_t size;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 2 && d_addr->track <= 5);
  size = _omTrackAddr_2_SizeOfBackPattern(d_addr);
  omAssume(size > 0 && OM_ALIGN_SIZE(size) == size);
  return size;
}
static size_t omTrack3Addr_2_OutSize(omTrackAddr d_addr)
{
  size_t size;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 2 && d_addr->track <= 5);
  omAssume(d_addr->flags > 0 && d_addr->flags < OM_FMAX &&
           ! ((d_addr->flags & OM_FBIN) && (d_addr->flags & OM_FSIZE)));

  size = _omTrack3Addr_2_OutSize(d_addr);
  return size;
}
static size_t omTrackAddr_2_OutSize(omTrackAddr d_addr)
{
  size_t size;
  omAssume(omIsTrackAddr(d_addr) && omOutAddr_2_TrackAddr(d_addr) == d_addr &&
           d_addr->track > 0 && d_addr->track <= 5);

  size = _omTrackAddr_2_OutSize(d_addr);
  return size;
}
static size_t omOutSize_2_TrackAddrSize(size_t size, char track)
{
  size_t da_size;
  omAssume(track > 0 && track <= 5);
  da_size = _omOutSize_2_TrackAddrSize(size, track);
  return da_size;
}
#else
#define omTrackAddr_2_SizeOfTrackAddrHeader _omTrackAddr_2_SizeOfTrackAddrHeader
#define omTrackAddr_2_FrontPattern          _omTrackAddr_2_FrontPattern
#define omTrackAddr_2_BackPattern           _omTrackAddr_2_BackPattern
#define omTrack3Addr_2_OutSize              _omTrack3Addr_2_OutSize
#define omTrackAddr_2_OutSize               _omTrackAddr_2_OutSize
#define omOutSize_2_TrackAddrSize           _omOutSize_2_TrackAddrSize
#define omTrackAddr_2_SizeOfFrontPattern     _omTrackAddr_2_SizeOfFrontPattern
#define omTrackAddr_2_SizeOfBackPattern     _omTrackAddr_2_SizeOfBackPattern
#endif

OM_INLINE_LOCAL omTrackAddr omOutAddr_2_TrackAddr(void* addr)
{
  omTrackAddr d_addr;
  char* page = omGetPageOfAddr(addr);
  size_t size = omGetTopBinOfPage((omBinPage) page)->sizeW << LOG_SIZEOF_LONG;

  omAssume(omIsBinPageAddr(addr));

  page += SIZEOF_OM_BIN_PAGE_HEADER;
  d_addr = (omTrackAddr) ((unsigned long) page + (unsigned long) ((((unsigned long)addr - (unsigned long)page) / size)*size));
  return d_addr;
}

size_t omOutSizeOfTrackAddr(void* addr)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));
  return omTrackAddr_2_OutSize(d_addr);
}

void* omAddr_2_OutAddr(void* addr)
{
  if (omIsTrackAddr(addr))
  {
    return omTrackAddr_2_OutAddr(omOutAddr_2_TrackAddr(addr));
  }
  else
  {
    return addr;
  }
}

/*******************************************************************
 *
 * Low level allocation/free routines: do the actual work,
 * no checks/tests, assume that everything in
 * environment is ok
 *
 *******************************************************************/

static omTrackAddr _omAllocTrackAddr(size_t d_size)
{
  omTrackAddr d_addr;
  omBin bin;

  if (d_size <= OM_MAX_BLOCK_SIZE)
    bin = omSmallSize2TrackBin(d_size);
  else
    bin = omGetSpecTrackBin(d_size);

  __omTypeAllocBin(omTrackAddr, d_addr, bin);

  omAssume(bin->current_page == omGetPageOfAddr(d_addr));

  omSetTrackOfUsedBlocks(bin->current_page->used_blocks);

  return d_addr;
}
void* omAllocTrackAddr(void* bin_size,
                       omTrackFlags_t flags, char track, OM_FLR_DECL)
{
  void* o_addr;
  size_t o_size = (flags & OM_FBIN ? ((omBin)bin_size)->sizeW << LOG_SIZEOF_LONG :
                   (bin_size != NULL ? OM_ALIGN_SIZE((size_t) bin_size) : OM_ALIGN_SIZE(1)));
  omTrackAddr d_addr;
  size_t d_size;
  if (track <= 0) track = 1;
  else if (track >  5) track = 5;

  if ((flags & OM_FBIN) && !omIsStaticNormalBin((omBin)bin_size))
    /* Need to set track >= 3 such that bin_size is kept: Needed
       for om_KeptAddr */
    track = (track > 3 ? track : 3);
  d_size = omOutSize_2_TrackAddrSize(o_size, track);

  d_addr = _omAllocTrackAddr(d_size);
  d_addr->next = (void*)-1;
  d_addr->track = track;
  d_addr->flags = flags | OM_FUSED;
  if (om_Opts.MarkAsStatic)
    d_addr->flags |= OM_FSTATIC;

#ifdef OM_TRACK_FILE_LINE
  d_addr->alloc_file = f;
  d_addr->alloc_line = (l > SHRT_MAX || l < 0 ? 0 : l);
#endif
#ifdef OM_TRACK_RETURN
  d_addr->alloc_r = r;
#endif

  o_addr = omTrackAddr_2_OutAddr(d_addr);
  if (track > 1)
  {
#ifdef OM_INTERNAL_DEBUG
#define FROM_FRAMES 0
#else
#define FROM_FRAMES 2
#endif

#ifdef OM_TRACK_BACKTRACE
    omGetBackTrace((void **)d_addr->alloc_frames,  FROM_FRAMES, OM_MAX_KEPT_FRAMES);
#endif

    if (track > 2)
    {
      if (flags & OM_FBIN && ((omBin) bin_size)->sticky)
      {
        d_addr->bin_size = (void*)(((omBin) bin_size)->sizeW<<LOG_SIZEOF_LONG);
        d_addr->flags &= ~OM_FBIN;
        d_addr->flags |= OM_FSIZE;
      }
      else
        d_addr->bin_size = (flags & OM_FBIN ? bin_size : (void*) o_size);
      omAssume(OM_ALIGN_SIZE((size_t)d_addr->bin_size) == (size_t) d_addr->bin_size);

      memset(omTrackAddr_2_FrontPattern(d_addr), OM_FRONT_PATTERN, omTrackAddr_2_SizeOfFrontPattern(d_addr));
      if (! (flags & OM_FZERO)) memset(o_addr, OM_INIT_PATTERN, o_size);
      memset(omTrackAddr_2_BackPattern(d_addr), OM_BACK_PATTERN, omTrackAddr_2_SizeOfBackPattern(d_addr));

#ifdef OM_TRACK_CUSTOM
      d_addr->custom = NULL;
#endif
      if (track > 3)
      {
#ifdef OM_TRACK_FILE_LINE
        d_addr->free_line = -1;
        d_addr->free_file = (char*) -1;
#endif
#ifdef OM_TRACK_RETURN
        d_addr->free_r = (void*) -1;
#endif

#ifdef OM_TRACK_BACKTRACE
        if (track > 4)
          memset(&d_addr->free_frames, 0, OM_MAX_KEPT_FRAMES*SIZEOF_VOIDP);
#endif
      }
    }
  }
  if (flags & OM_FZERO) omMemsetW(o_addr, 0, o_size >> LOG_SIZEOF_LONG);
  return o_addr;
}


void* omMarkAsFreeTrackAddr(void* addr, int keep, omTrackFlags_t *flags, OM_FLR_DECL)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));

  d_addr->next = (void*) -1;
  if (d_addr->track > 2)
  {
    if (d_addr->flags & OM_FUSED)
    {
      memset(omTrackAddr_2_OutAddr(d_addr), OM_FREE_PATTERN, omTrackAddr_2_OutSize(d_addr));
      if (d_addr->track > 3)
      {
#ifdef OM_TRACK_FILE_LINE
        d_addr->free_line = l;
        d_addr->free_file = f;
#endif
#ifdef OM_TRACK_RETURN
        d_addr->free_r = r;
#endif

#ifdef OM_TRACK_BACKTRACE
        if (d_addr->track > 4)
          omGetBackTrace(d_addr->free_frames,  FROM_FRAMES, OM_MAX_KEPT_FRAMES);
#endif
      }
    }
    else
    {
      omAssume(d_addr->flags & OM_FKEPT);
    }
  }
  if (d_addr->flags & OM_FKEEP) *flags |= OM_FKEEP;
  d_addr->flags &= ~OM_FUSED;
  if (keep) d_addr->flags |= OM_FKEPT;
  else d_addr->flags &= ~OM_FKEPT;

  return(void*) d_addr;
}

void omFreeTrackAddr(void* d_addr)
{
  omBinPage page;
  omBin bin;

  omAssume(omIsBinPageAddr(d_addr));
  omAssume(d_addr != NULL && omIsTrackAddr(d_addr));
  d_addr = omOutAddr_2_TrackAddr(d_addr);

  page = omGetBinPageOfAddr((void*) d_addr);
  bin = omGetTopBinOfPage(page);
  /* Ok, here is how it works:
    1. we unset the first bit of used_blocks
       ==> used_blocks >= 0
    2. we do a normal free
    3. if page of addr was freed, then om_JustFreedPage
          is != NULL ==> nothing to be done by us
       else
          page is still active ==> reset first bit of used_blocks
  */

  omUnsetTrackOfUsedBlocks(page->used_blocks);

  om_JustFreedPage = NULL;

  __omFreeBinAddr(d_addr);

  if (page != om_JustFreedPage)
    omSetTrackOfUsedBlocks(page->used_blocks);
  else
  {
    /* Still need to check wheter we need to get rid of SpecBin */
    if (bin->last_page == NULL && ! omIsStaticTrackBin(bin))
      omDeleteSpecBin(&bin);
  }
}

/*******************************************************************
 *
 * Checking a Track Addr
 *
 *
 *******************************************************************/

omError_t omCheckTrackAddr(void* addr, void* bin_size, omTrackFlags_t flags, char level,
                           omError_t report, OM_FLR_DECL)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));
  omAssume(! omCheckPtr(addr, 0, OM_FLR));

  omAddrCheckReturnCorrupted(d_addr->track < 1 || d_addr->track > OM_TRACK_MAX);
  omAddrCheckReturnError((flags & OM_FUSED) && omTrackAddr_2_OutAddr(d_addr) != addr, omError_FalseAddrOrMemoryCorrupted);

  omCheckReturn(omDoCheckBinAddr(d_addr, 0, (flags & OM_FUSED ? OM_FUSED : (flags & OM_FKEPT ? OM_FKEPT: 0)),
                                 level, report, OM_FLR_VAL));
  return omDoCheckTrackAddr(d_addr, addr, bin_size, flags, level, report, OM_FLR_VAL);
}


static omError_t omDoCheckTrackAddr(omTrackAddr d_addr, void* addr, void* bin_size, omTrackFlags_t flags, char level,
                                    omError_t report, OM_FLR_DECL)
{
  if (flags & OM_FUSED)
    omAddrCheckReturnError(d_addr->next != ((void*) -1), omError_FreedAddrOrMemoryCorrupted);
  else
    omAddrCheckReturnError(d_addr->next != NULL && omCheckPtr(d_addr->next, omError_MaxError, OM_FLR_VAL),
                           omError_FreedAddrOrMemoryCorrupted);
  omAddrCheckReturnCorrupted(omCheckFlags(d_addr->flags));

  omAddrCheckReturnError(level > 1 && (flags & OM_FUSED) && omIsInKeptAddrList(d_addr), omError_FreedAddr);
  omAddrCheckReturnError((d_addr->flags & OM_FUSED) ^ (flags & OM_FUSED), omError_FreedAddrOrMemoryCorrupted);

  if (flags & OM_FBINADDR && flags & OM_FSIZE)
    omAddrCheckReturnError(omTrackAddr_2_OutSize(d_addr) != (size_t) bin_size, omError_WrongSize);

  if (d_addr->track > 2)
  {
    if (d_addr->flags & OM_FBIN)
    {
      omAddrCheckReturnCorrupted(!omIsKnownTopBin((omBin) d_addr->bin_size, 1));
    }
    else
    {
      omAssume(d_addr->flags & OM_FSIZE);

      omAddrCheckReturnCorrupted(!OM_IS_ALIGNED(d_addr->bin_size));
      omAddrCheckReturnCorrupted((size_t) d_addr->bin_size >
                                 omSizeOfBinAddr(d_addr)
                                 - omTrackAddr_2_SizeOfTrackAddrHeader(d_addr)
                                 - OM_MIN_SIZEOF_BACK_PATTERN);
      /* Hmm .. here I'd love to have a stricter bound */
      omAddrCheckReturnCorrupted((size_t) d_addr->bin_size < SIZEOF_OM_ALIGNMENT);
    }

    omAddrCheckReturnError((flags & OM_FBINADDR) && !((d_addr->flags & OM_FBIN) || ((size_t) d_addr->bin_size <= OM_MAX_BLOCK_SIZE)), omError_NotBinAddr);

    if (flags & OM_FBIN)
    {
      if (d_addr->flags & OM_FBIN)
        omAddrCheckReturnError(((omBin) d_addr->bin_size)->sizeW != ((omBin) bin_size)->sizeW, omError_WrongBin);
      else
        omAddrCheckReturnError((((omBin) bin_size)->sizeW << LOG_SIZEOF_LONG) != OM_ALIGN_SIZE((size_t) d_addr->bin_size), omError_WrongBin);
    }
    else if (flags & OM_FSIZE)
    {
      if (d_addr->flags & OM_FBIN)
      {
        omAddrCheckReturnError((((omBin) d_addr->bin_size)->sizeW << LOG_SIZEOF_LONG) < ((size_t) bin_size), omError_WrongSize);
      }
      else
      {
        omAddrCheckReturnError((size_t) d_addr->bin_size < (size_t) bin_size, omError_WrongSize);
      }
    }

    omAddrCheckReturnError(omCheckPattern(omTrackAddr_2_FrontPattern(d_addr), OM_FRONT_PATTERN,omTrackAddr_2_SizeOfFrontPattern(d_addr)),omError_FrontPattern);
    omAddrCheckReturnError(omCheckPattern(omTrackAddr_2_BackPattern(d_addr), OM_BACK_PATTERN,omTrackAddr_2_SizeOfBackPattern(d_addr)),omError_BackPattern);
    if (! (d_addr->flags & OM_FUSED))
      omAddrCheckReturnError(omCheckPattern(omTrackAddr_2_OutAddr(addr), OM_FREE_PATTERN, omTrackAddr_2_OutSize(d_addr)),omError_FreePattern);

    if (d_addr->track > 3)
    {
#ifdef OM_TRACK_FILE_LINE
      if (d_addr->flags & OM_FUSED)
      {
        omAddrCheckReturnCorrupted(d_addr->free_line != -1);
        omAddrCheckReturnCorrupted(d_addr->free_file != (void*) -1);
      }
      else
      {
        omAddrCheckReturnCorrupted(d_addr->free_line < 0);
        omAddrCheckReturnCorrupted(d_addr->free_file == (void*) -1);
      }
#endif
#ifdef OM_TRACK_RETURN
      omAddrCheckReturnCorrupted((d_addr->flags & OM_FUSED)
                                 && (d_addr->free_r != (void*) -1));
#endif
    }
  }
  else
  {
    /* track < 2 */
    if (flags & OM_FBIN)
    {
      size_t size = omTrackAddr_2_OutSize(d_addr);
      omAddrCheckReturnError(!omIsKnownTopBin((omBin) bin_size, 1), omError_UnknownBin);
      omAddrCheckReturnError(size < (((omBin)bin_size)->sizeW<<LOG_SIZEOF_LONG), omError_WrongBin);
    }
    else if (flags & OM_FSIZE
    && (!(flags & OM_FSLOPPY)
    || (size_t)bin_size > 0))
    {
      omAddrCheckReturnError(omTrackAddr_2_OutSize(d_addr) < (size_t) bin_size, omError_WrongSize);
    }
    else if (flags & OM_FBINADDR)
    {
      size_t size = omTrackAddr_2_OutSize(d_addr);
      omAddrCheckReturnError(size > OM_MAX_BLOCK_SIZE, omError_NotBinAddr);
    }
  }
  return omError_NoError;
}

static int omCheckFlags(omTrackFlags_t flag)
{
  if (flag > OM_FMAX) return 1;
  if (! ((flag & OM_FBIN) ^ (flag & OM_FSIZE))) return 1;
  if (flag & OM_FUSED && flag & OM_FKEPT) return 1;
  return 0;
}

static int omCheckPattern(char* s, char p, size_t size)
{
  int i;
  for (i=0; i<size; i++)
  {
    if (s[i] != p)
      return 1;
  }
  return 0;
}

#ifdef OM_TRACK_BACKTRACE
#define OM_ALLOC_FRAMES(d_addr) d_addr->alloc_frames
#define OM_FREE_FRAMES(d_addr)  d_addr->free_frames
#else
#define OM_ALLOC_FRAMES(d) NULL
#define OM_FREE_FRAMES(d)  NULL
#endif

void omPrintTrackAddrInfo(FILE* fd, void* addr, int max_frames)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(d_addr->track > 0);
  if (max_frames <= 0) return;
  if (! (d_addr->flags & OM_FUSED))
  {
    fputs(" freed\n",fd);
    return;
  }

  if (max_frames > OM_MAX_KEPT_FRAMES) max_frames = OM_MAX_KEPT_FRAMES;

  fputs(" allocated at ",fd);
  if (! _omPrintBackTrace((void **)OM_ALLOC_FRAMES(d_addr),
                          (d_addr->track > 1 ? max_frames : 0),
                          fd,
                          OM_FLR_ARG(d_addr->alloc_file, d_addr->alloc_line, d_addr->alloc_r)))
    fputs(" ??",fd);
  if (d_addr->track > 1)
  {
    if (d_addr->track > 3 && ! (d_addr->flags & OM_FUSED))
    {
      fputs("\n freed at ",fd);
      if (! _omPrintBackTrace(OM_FREE_FRAMES(d_addr),
                          (d_addr->track > 4 ? max_frames : 0),
                          fd,
                          OM_FLR_ARG(d_addr->free_file, d_addr->free_line, d_addr->free_r)))
        fputs(" ??",fd);
    }
  }
  fputc('\n',fd);
  fflush(fd);
}

/*******************************************************************
 *
 * Misc routines for marking, etc.
 *
 *******************************************************************/
int omIsStaticTrackAddr(void* addr)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));

  return (d_addr->flags & OM_FSTATIC);
}

omBin omGetOrigSpecBinOfTrackAddr(void* addr)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));

  if (d_addr->track > 2 && (d_addr->flags & OM_FBIN))
  {
    omBin bin = (omBin) d_addr->bin_size;
    if (omIsSpecBin(bin)) return bin;
  }
  return NULL;
}

void omMarkAsStaticAddr(void* addr)
{
  if (omIsTrackAddr(addr))
  {
    omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
    d_addr->flags |= OM_FSTATIC;
  }
}

void omUnMarkAsStaticAddr(void* addr)
{
  if (omIsTrackAddr(addr))
  {
    omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
    d_addr->flags &= ~OM_FSTATIC;
  }
}

static void _omMarkAsStatic(void* addr)
{
  omTrackAddr d_addr = (omTrackAddr) addr;
  if (!omCheckPtr(addr, omError_MaxError, OM_FLR))
  {
    omAssume(omIsTrackAddr(addr) && omOutAddr_2_TrackAddr(addr) == d_addr);
    d_addr->flags |= OM_FSTATIC;
  }
}

static void _omUnMarkAsStatic(void* addr)
{
  omTrackAddr d_addr = (omTrackAddr) addr;
  omAssume(omIsTrackAddr(addr) && omOutAddr_2_TrackAddr(addr) == d_addr);
  d_addr->flags &= ~OM_FSTATIC;
}

void omUnMarkMemoryAsStatic()
{
  omIterateTroughAddrs(0, 1, _omUnMarkAsStatic, NULL);
}

void omMarkMemoryAsStatic()
{
  omIterateTroughAddrs(0, 1, _omMarkAsStatic, NULL);
}

#ifdef OM_TRACK_CUSTOM
void omSetCustomOfTrackAddr(void* addr, void* value)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));

  if (d_addr->track > 2)
  {
    d_addr->custom = value;
  }
}

void* omGetCustomOfTrackAddr(void* addr)
{
  omTrackAddr d_addr = omOutAddr_2_TrackAddr(addr);
  omAssume(omIsTrackAddr(addr));

  if (d_addr->track > 2)
  {
    return d_addr->custom;
  }
  else
  {
    return NULL;
  }
}
#endif

#endif /* OM_HAVE_TRACK */

#ifndef OM_NDEBUG

#ifndef OM_HAVE_TRACK
#include "omalloc.h"
#endif

int omIsInKeptAddrList(void* addr)
{
  void* ptr = om_KeptAddr;
  int ret = 0;

#ifdef OM_HAVE_TRACK
  if (omIsTrackAddr(addr))
    addr = omOutAddr_2_TrackAddr(addr);
#endif

  if (om_LastKeptAddr != NULL)
    *((void**) om_LastKeptAddr) = om_AlwaysKeptAddrs;

  while (ptr != NULL)
  {
    if (ptr == addr)
    {
      ret = 1; break;
    }
    ptr = *((void**) ptr);
  }

  if (om_LastKeptAddr != NULL)
    *((void**) om_LastKeptAddr) = NULL;

  return ret;
}
#endif /*!OM_NDEBUG*/
#endif
