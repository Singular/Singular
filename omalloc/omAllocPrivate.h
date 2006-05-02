/*******************************************************************
 *  File:    omAllocPrivate.h
 *  Purpose: declaration of "private" (but visible to the outside)
 *           routines for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAllocPrivate.h,v 1.8 2006-05-02 11:39:02 Singular Exp $
 *******************************************************************/
#ifndef OM_ALLOC_PRIVATE_H
#define OM_ALLOC_PRIVATE_H

/*******************************************************************
 *
 *  Definitions of structures we work with
 *
 *******************************************************************/
/* Need to define it here, has to be known to macros */
struct omBinPage_s
{
  long          used_blocks;    /* number of used blocks of this page */
  void*         current;        /* pointer to current freelist */
  omBinPage     next;           /* next/prev pointer of pages */
  omBinPage     prev;
  void*         bin_sticky;     /* bin this page belongs to with
                                   sticky tag of page hidden in ptr */
  omBinPageRegion region;       /* BinPageRegion of this page */
};

/* Change this appropriately, if you change omBinPage                 */
/* However, make sure that omBinPage is a multiple of SIZEOF_MAX_TYPE */
#define SIZEOF_OM_BIN_PAGE_HEADER (5*SIZEOF_VOIDP + SIZEOF_LONG)
#define SIZEOF_OM_BIN_PAGE (SIZEOF_SYSTEM_PAGE - SIZEOF_OM_BIN_PAGE_HEADER)

/* keep all members of omBin_s a sizeof(long) type,
   otherwise list operations will fail */
struct omBin_s
{
  omBinPage     current_page;   /* page of current freelist */
  omBinPage     last_page;      /* pointer to last page of freelist */
  omBin         next;           /* sticky bins of the same size */
  size_t        sizeW;          /* size in words */
  long          max_blocks;     /* if > 0   # blocks in one page,
                                   if < 0   # pages for one block */
  unsigned long sticky;         /* sticky tag */
};

struct omSpecBin_s
{
  omSpecBin        next;       /* pointer to next bin */
  omBin            bin;        /* pointer to bin itself */
  long             max_blocks; /* max_blocks of bin*/
  long             ref;        /* ref count */
};

extern  omSpecBin   om_SpecBin;
extern  omBin       om_StickyBins;
extern  omBinPage_t om_ZeroPage[];
extern  omBin       om_Size2Bin[];

/*******************************************************************
 *
 *  Working with pages/bins
 *
 *******************************************************************/
#define omGetTopBinOfPage(page) \
  ((omBin) ( ((unsigned long) ((page)->bin_sticky)) & ~((unsigned long)SIZEOF_VOIDP - 1)))
#define omGetStickyOfPage(page) \
  (((unsigned long) ((page)->bin_sticky)) & ((unsigned long)SIZEOF_VOIDP-1))
#define omSetTopBinOfPage(page, bin) \
  (page)->bin_sticky= (void*)((unsigned long)bin + omGetStickyOfPage(page))
#define omSetStickyOfPage(page, sticky) \
  (page)->bin_sticky = (void*)(((unsigned long)sticky & ((unsigned long)SIZEOF_VOIDP-1)) + \
                                (unsigned long)omGetTopBinOfPage(page))
#define omSetTopBinAndStickyOfPage(page, bin, sticky) \
  (page)->bin_sticky= (void*)(((unsigned long)sticky & (SIZEOF_VOIDP-1)) \
                               + (unsigned long)bin)

#define omGetTopBinOfAddr(addr) \
  omGetTopBinOfPage(((omBinPage) omGetPageOfAddr(addr)))
#define omGetBinOfAddr(addr) omGetBinOfPage(omGetBinPageOfAddr(addr))

#ifndef OM_GENERATE_INC
extern omBin_t om_StaticBin[];
extern omBin om_Size2Bin[];
#ifdef OM_ALIGNMENT_NEEDS_WORK
extern omBin om_Size2AlignedBin[];
#endif

/*******************************************************************
 *
 *  SizeOfAddr
 *
 *******************************************************************/
#ifdef OM_INTERNAL_DEBUG
size_t omSizeOfBinAddr(void* addr);
#else
#define omSizeOfBinAddr(addr) _omSizeOfBinAddr(addr)
#endif

#define _omSizeOfBinAddr(addr)  ((omSizeWOfBinAddr(addr)) << LOG_SIZEOF_LONG)
#define omSizeWOfBinAddr(addr) ((omGetTopBinOfAddr(addr))->sizeW)

/*******************************************************************
 *
 *  lowest level alloc/free macros
 *
 *******************************************************************/
extern void* omAllocBinFromFullPage(omBin bin);
extern void  omFreeToPageFault(omBinPage page, void* addr);

/*******************************************************************/
/* Page                                                            */
#define __omTypeAllocFromNonEmptyPage(type, addr, page) \
do                                                      \
{                                                       \
  ((page)->used_blocks)++;                              \
  addr = (type)((page)->current);                       \
  (page)->current =  *((void**) (page)->current);       \
}                                                       \
while (0)

#define __omFreeToPage(addr, page)              \
do                                              \
{                                               \
  if ((page)->used_blocks > 0L)                 \
  {                                             \
    *((void**) (addr)) = (page)->current;       \
    ((page)->used_blocks)--;                    \
    (page)->current = (addr);                   \
  }                                             \
  else                                          \
  {                                             \
    omFreeToPageFault(page, addr);              \
  }                                             \
}                                               \
while (0)


/*******************************************************************/
/* Bin                                                             */
#define __omTypeAllocBin(type, addr, bin)                   \
do                                                          \
{                                                           \
  register omBinPage __om_page = (bin)->current_page;       \
  if (__om_page->current != NULL)                           \
    __omTypeAllocFromNonEmptyPage(type, addr, __om_page);   \
  else                                                      \
    addr = (type) omAllocBinFromFullPage(bin);              \
}                                                           \
while (0)

#define __omTypeAlloc0Bin(type, addr, bin)      \
do                                              \
{                                               \
  __omTypeAllocBin(type, addr, bin);            \
  omMemsetW(addr, 0, (bin)->sizeW);             \
}                                               \
while (0)


#define __omFreeBinAddr(addr)                                   \
do                                                              \
{                                                               \
  register void* __om_addr = (void*) (addr);                    \
  register omBinPage __om_page = omGetBinPageOfAddr(__om_addr); \
  __omFreeToPage(__om_addr, __om_page);                         \
}                                                               \
while (0)

#define __omTypeReallocBin(old_addr, old_bin, new_type, new_addr, new_bin)                              \
do                                                                                                      \
{                                                                                                       \
  if (old_bin != new_bin)                                                                               \
  {                                                                                                     \
    size_t old_sizeW = (omIsNormalBinPageAddr(old_addr) ? old_bin->sizeW : omSizeWOfAddr(old_addr));    \
    __omTypeAllocBin(new_type, new_addr, new_bin);                                                      \
    omMemcpyW(new_addr, old_addr, (new_bin->sizeW > old_sizeW ? old_sizeW : new_bin->sizeW));           \
    __omFreeBinAddr(old_addr);                                                                     \
  }                                                                                                     \
  else                                                                                                  \
  {                                                                                                     \
    new_addr = (new_type) old_addr;                                                                     \
  }                                                                                                     \
}                                                                                                       \
while (0)


#define __omTypeRealloc0Bin(old_addr, old_bin, new_type, new_addr, new_bin)                             \
do                                                                                                      \
{                                                                                                       \
  if (old_bin != new_bin)                                                                               \
  {                                                                                                     \
    size_t old_sizeW = (omIsNormalBinPageAddr(old_addr) ? old_bin->sizeW : omSizeWOfAddr(old_addr));    \
    __omTypeAllocBin(new_type, new_addr, new_bin);                                                      \
    omMemcpyW(new_addr, old_addr, (new_bin->sizeW > old_sizeW ? old_sizeW : new_bin->sizeW));           \
    if (new_bin->sizeW > old_sizeW)                                                                     \
       omMemsetW((void**)new_addr + old_sizeW, 0, new_bin->sizeW - old_sizeW);                          \
    __omFreeBinAddr(old_addr);                                                                     \
  }                                                                                                     \
  else                                                                                                  \
  {                                                                                                     \
    new_addr = (new_type) old_addr;                                                                     \
  }                                                                                                     \
}                                                                                                       \
while (0)

/*******************************************************************/
/* Size                                                            */
#define omSmallSize2Bin(size) om_Size2Bin[((size) -1)>>LOG_SIZEOF_OM_ALIGNMENT]

#define __omTypeAlloc(type, addr, size)         \
do                                              \
{                                               \
  size_t __size = size;                         \
  if (__size <= OM_MAX_BLOCK_SIZE)              \
  {                                             \
    omBin __om_bin = omSmallSize2Bin(__size);   \
    __omTypeAllocBin(type, addr, __om_bin);     \
  }                                             \
  else                                          \
  {                                             \
    addr = (type) omAllocLarge(__size);         \
  }                                             \
}                                               \
while(0)

#define __omTypeAlloc0(type, addr, size)        \
do                                              \
{                                               \
  size_t __size = size;                         \
  if (__size <= OM_MAX_BLOCK_SIZE)              \
  {                                             \
    omBin __om_bin = omSmallSize2Bin(__size);   \
    __omTypeAlloc0Bin(type, addr, __om_bin);    \
  }                                             \
  else                                          \
  {                                             \
    addr = (type) omAlloc0Large(__size);        \
  }                                             \
}                                               \
while (0)

#ifdef OM_ALIGNMENT_NEEDS_WORK
#define omSmallSize2AlignedBin(size) om_Size2AlignedBin[((size) -1)>>LOG_SIZEOF_OM_ALIGNMENT]

#define __omTypeAllocAligned(type, addr, size)          \
do                                                      \
{                                                       \
  size_t __size = size;                                 \
  if (__size <= OM_MAX_BLOCK_SIZE)                      \
  {                                                     \
    omBin __om_bin = omSmallSize2AlignedBin(__size);    \
    __omTypeAllocBin(type, addr, __om_bin);             \
  }                                                     \
  else                                                  \
  {                                                     \
    addr = (type) omAllocLarge(__size);                 \
  }                                                     \
}                                                       \
while(0)

#define __omTypeAlloc0Aligned(type, addr, size)         \
do                                                      \
{                                                       \
  size_t __size = size;                                 \
  if (__size <= OM_MAX_BLOCK_SIZE)                      \
  {                                                     \
    omBin __om_bin = omSmallSize2AlignedBin(__size);    \
    __omTypeAlloc0Bin(type, addr, __om_bin);            \
  }                                                     \
  else                                                  \
  {                                                     \
    addr = (type) omAlloc0Large(__size);                \
  }                                                     \
}                                                       \
while (0)
#else
#define __omTypeAllocAligned    __omTypeAlloc
#define __omTypeAlloc0Aligned   __omTypeAlloc0
#endif /* OM_ALIGNMENT_NEEDS_WORK */

#define __omFreeSize(addr, size)                            \
do                                                          \
{                                                           \
  if ((size <= OM_MAX_BLOCK_SIZE) || omIsBinPageAddr(addr)) \
  {                                                         \
    __omFreeBinAddr(addr);                                  \
  }                                                         \
  else                                                      \
  {                                                         \
    omFreeLarge(addr);                                      \
  }                                                         \
}                                                           \
while (0)

#define __omFree(addr)                          \
do                                              \
{                                               \
  if (omIsBinPageAddr(addr))                    \
  {                                             \
    __omFreeBinAddr(addr);                         \
  }                                             \
  else                                          \
  {                                             \
    omFreeLarge(addr);                          \
  }                                             \
}                                               \
while (0)

void* omDoRealloc(void* old_addr, size_t new_size, int flags);

#define ___omTypeRealloc(old_addr, new_type, new_addr, new_size, SIZE_2_BIN, REALLOC_BIN, flags)    \
do                                                                                                  \
{                                                                                                   \
  size_t __new_size = new_size;                                                                     \
  if (__new_size <= OM_MAX_BLOCK_SIZE && omIsBinPageAddr(old_addr))                                 \
  {                                                                                                 \
    omBin __old_bin = omGetBinOfAddr(old_addr);                                                     \
    omBin __new_bin = SIZE_2_BIN(__new_size);                                                       \
    REALLOC_BIN(old_addr, __old_bin, new_type, new_addr, __new_bin);                                \
  }                                                                                                 \
  else                                                                                              \
  {                                                                                                 \
    new_addr = (new_type) omDoRealloc(old_addr, __new_size, flags);                                 \
  }                                                                                                 \
}                                                                                                   \
while (0)

#define ___omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size, SIZE_2_BIN, REALLOC_BIN, flags)  \
do                                                                                                              \
{                                                                                                               \
  size_t __new_size = new_size;                                                                                 \
  if (__new_size <= OM_MAX_BLOCK_SIZE && old_size <= OM_MAX_BLOCK_SIZE)                                         \
  {                                                                                                             \
    omBin __old_bin = omGetBinOfAddr(old_addr);                                                                 \
    omBin __new_bin = SIZE_2_BIN(__new_size);                                                                   \
    REALLOC_BIN(old_addr, __old_bin, new_type, new_addr, __new_bin);                                            \
  }                                                                                                             \
  else                                                                                                          \
  {                                                                                                             \
    new_addr = (new_type) omDoRealloc(old_addr, __new_size, flags);                                             \
  }                                                                                                             \
}                                                                                                               \
while (0)

#define __omTypeRealloc(old_addr, new_type, new_addr, new_size)                 \
  ___omTypeRealloc(old_addr, new_type, new_addr, new_size, omSmallSize2Bin, __omTypeReallocBin, 0)
#define __omTypeRealloc0(old_addr, new_type, new_addr, new_size)                \
  ___omTypeRealloc(old_addr, new_type, new_addr, new_size, omSmallSize2Bin, __omTypeRealloc0Bin, 1)
#define __omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size)     \
  ___omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size, omSmallSize2Bin, __omTypeReallocBin, 0)
#define __omTypeRealloc0Size(old_addr, old_size, new_type, new_addr, new_size)    \
  ___omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size, omSmallSize2Bin, __omTypeRealloc0Bin, 1)

#ifdef OM_ALIGNMENT_NEEDS_WORK
#define __omTypeReallocAligned(old_addr, new_type, new_addr, new_size)                 \
  ___omTypeRealloc(old_addr, new_type, new_addr, new_size, omSmallSize2AlignedBin, __omTypeReallocBin, 2)
#define __omTypeRealloc0Aligned(old_addr, new_type, new_addr, new_size)                \
  ___omTypeRealloc(old_addr, new_type, new_addr, new_size, omSmallSize2AlignedBin, __omTypeRealloc0Bin, 3)
#define __omTypeReallocAlignedSize(old_addr, old_size, new_type, new_addr, new_size)     \
  ___omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size, omSmallSize2AlignedBin, __omTypeReallocBin, 2)
#define __omTypeRealloc0AlignedSize(old_addr, old_size, new_type, new_addr, new_size)    \
  ___omTypeReallocSize(old_addr, old_size, new_type, new_addr, new_size, omSmallSize2AlignedBin, __omTypeRealloc0Bin, 3)
#else
#define __omTypeReallocAligned      __omTypeRealloc
#define __omTypeRealloc0Aligned     __omTypeRealloc0
#define __omTypeReallocAlignedSize      __omTypeReallocSize
#define __omTypeRealloc0AlignedSize     __omTypeRealloc0Size
#endif

#endif /* OM_GENERATE_INC */
#endif /* OM_ALLOC_PRIVATE_H */
