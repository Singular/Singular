/*******************************************************************
 *  File:    omPrivate.h
 *  Purpose: declaration of "private" (but visible to the outside) 
 *           routines for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omPrivate.h,v 1.2 1999-11-22 18:12:59 obachman Exp $
 *******************************************************************/
#ifndef OM_PRIVATE_H
#define OM_PRIVATE_H

#include <stdlib.h>
#include <string.h>

/*******************************************************************
 *  
 *  Misc defines
 *  
 *******************************************************************/

#ifndef NULL
#define NULL ((void*)  0)
#endif

#ifdef OM_ALIGN_8
#define SIZEOF_OM_ALIGNMENT 8
#define SIZEOF_OM_ALIGNMENT_1 7
#define LOG_SIZEOF_OM_ALIGNMENT 3 
#else
#define SIZEOF_OM_ALIGNMENT 4
#define SIZEOF_OM_ALIGNMENT_1 3
#define LOG_SIZEOF_OM_ALIGNMENT 2
#endif

#define OM_ALIGN_SIZE(size) \
   ((size + SIZEOF_OM_ALIGNMENT_1) & (~SIZEOF_OM_ALIGNMENT_1))
 
/*******************************************************************
 *  
 *  Definitions of structures we work with
 *  
 *******************************************************************/
struct omBinPage_s;
struct omBin_s;
struct omSpecBin_s;
typedef struct omBinPage_s omBinPage_t;
typedef struct omBin_s     omBin_t;
typedef struct omSpecBin_s omSpecBin_t;
typedef omBinPage_t*       omBinPage;
typedef omBin_t*           omBin;
typedef omSpecBin_t*       omSpecBin;


/* Need to define it here, has to be known to macros */
struct omBinPage_s
{
  long          used_blocks;    /* number of used blocks of this page */
  void*         current;        /* pointer to current freelist */
  omBinPage    next;           /* next/prev pointer of pages */
  omBinPage    prev;
  void*         bin_sticky;   /* bin this page belongs to with 
                                  sticky tag of page hidden in ptr*/
};
/* Change this appropriately, if you change omBinPage           */
/* However, make sure that omBinPage is a multiple of SIZEOF_MAX_TYPE */
#define _SIZEOF_OM_BIN_PAGE_HEADER (4*SIZEOF_VOIDP + SIZEOF_LONG + SIZEOF_LONG) 
/* make sure SIZEOF_OM_BIN_PAGE_HEADER is divisible by SIZEOF_OM_ALIGNMENT*/
#define SIZEOF_OM_BIN_PAGE_HEADER OM_ALIGN_SIZE(_SIZEOF_OM_BIN_PAGE_HEADER)
#define SIZEOF_OM_BIN_PAGE (SIZEOF_OM_PAGE - SIZEOF_OM_BIN_PAGE_HEADER)

/* keep all members of omBin_s a sizeof(long) type, 
   otherwise list operations will fail */
struct omBin_s
{
  omBinPage    current_page;   /* page of current freelist */
  omBinPage    last_page;      /* pointer to last page of freelist */
  omBin        next;           /* sticky bins of the same size */
  long          sizeW;          /* size in words */
  long          max_blocks;     /* if > 0   # blocks in one page, 
                                   if < 0   # pages for one block */
  unsigned long sticky;         /* sticky tag */
};

struct omSpecBin_s
{
  omSpecBin        next;       /* pointer to next bin */
  omBin            bin;       /* pointer to bin itself */
  long             max_blocks; /* max_blocks of bin*/
  long             ref;        /* ref count */
};

#ifndef OM_GENERATE_INC
extern  omBinPage_t om_ZeroPage[];
extern  omBinPage_t om_CheckPage[];
extern  omBinPage_t om_LargePage[];
extern  omBin_t     om_LargeBin[];
extern  omBin_t     om_CheckBin[];
extern  omBin_t     om_StaticBin[];
extern  omBin       om_Size2Bin[];

#include "omTables.inc"
 


/*******************************************************************
 *  
 *  lowest level alloc/free macros
 *  
 *******************************************************************/
extern void* omAllocBinFromFullPage(omBin bin);
extern void  omFreeToPageFault(omBinPage page, void* addr);

#include "omMemOps.h"
#include "omPage.h"


/*******************************************************************/
/* Page                                                            */
#define __omTypeAllocFromPage(type, addr, page)             \
do                                                          \
{                                                           \
  ((page)->used_blocks)++;                                  \
  addr = (type)((page)->current);                           \
  (page)->current =  *((void**) (page)->current);           \
}                                                           \
while (0)

#define __omFreeToPage(addr, page)              \
do                                              \
{                                               \
  if ((page)->used_blocks > 0)                  \
  {                                             \
    *((void**)addr) = (page)->current;          \
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
#define __omTypeAllocBin(type, addr, bin)               \
do                                                      \
{                                                       \
  register omBinPage __om_page = (bin)->current_page;   \
  if (__om_page->current != NULL)                       \
    __omTypeAllocFromPage(type, addr, __om_page);       \
  else                                                  \
    addr = (type) omAllocBinFromFullPage(bin);          \
}                                                       \
while (0)

#define __omTypeAlloc0Bin(type, addr, bin)      \
do                                              \
{                                               \
  __omTypeAllocBin(type, addr, bin);            \
  omMemsetW(addr, 0, (bin)->sizeW);             \
}                                               \
while (0)

#define __omFreeBin(addr)                                       \
do                                                              \
{                                                               \
  register void* __om_addr = (void*) (addr);                    \
  register omBinPage __om_page = omGetPageOfAddr(__om_addr);    \
  __omFreeToPage(__om_addr, __om_page);                         \
}                                                               \
while (0)


/*******************************************************************/
/* Block                                                           */
#define omSmallSize2Bin(size) om_Size2Bin[((size) -1) >> LOG_SIZEOF_OM_ALIGNMENT]
#define omSize2Bin(size) ((size) <= OM_MAX_BLOCK_SIZE ? omSmallSize2Bin(size) : om_LargeBin)

#define omAllocLargeBlock(size)       OM_MALLOC(size)
#define omFreeLargeBlock(addr, size)  OM_FREE(addr)

#define __omTypeAllocBlock(type, addr, size)    \
do                                              \
{                                               \
  if (size <= OM_MAX_BLOCK_SIZE)                \
  {                                             \
    omBin __om_bin = omSmallSize2Bin(size);     \
    __omTypeAllocBin(type, addr, __om_bin);     \
  }                                             \
  else                                          \
  {                                             \
    addr = (type) omAllocLargeBlock(size);      \
  }                                             \
}                                               \
while(0)

#define __omTypeAlloc0Block(type, addr, size)   \
do                                              \
{                                               \
  if (size <= OM_MAX_BLOCK_SIZE)                \
  {                                             \
    omBin __om_bin = omSmallSize2Bin(size);     \
    __omTypeAlloc0Bin(type, addr, __om_bin);    \
  }                                             \
  else                                          \
  {                                             \
    addr = (type) omAllocLargeBlock(size);      \
    memset(addr, 0, size);                      \
  }                                             \
}                                               \
while (0)

#define __omFreeBlock(addr, size)               \
do                                              \
{                                               \
  if (size <= OM_MAX_BLOCK_SIZE)                \
  {                                             \
    __omFreeBin(addr);                          \
  }                                             \
  else                                          \
  {                                             \
    omFreeLargeBlock(addr, size);               \
  }                                             \
}                                               \
while (0)

/*******************************************************************/
/* Chunk                                                           */
#define omAllocLargeChunk(size)  OM_MALLOC(size)
#define omFreeLargeChunk(addr)   OM_FREE(addr)
 
#define __omTypeAllocChunk(type, addr, size)                \
do                                                          \
{                                                           \
  void* __om_addr;                                          \
  size_t __om_size = (size) + SIZEOF_OM_ALIGNMENT;          \
  if (__om_size <= OM_MAX_BLOCK_SIZE)                       \
  {                                                         \
    omBin __om_bin = omSmallSize2Bin(__om_size);                 \
    __omTypeAllocBin(void*, __om_addr, __om_bin);           \
    *((void**) __om_addr) = (void*) __om_bin->current_page; \
  }                                                         \
  else                                                      \
  {                                                         \
    __om_addr = omAllocLargeChunk(__om_size);               \
    *((void**) __om_addr) = om_LargePage;                   \
  }                                                         \
  addr = (type) (__om_addr + SIZEOF_OM_ALIGNMENT);          \
}                                                           \
while (0)

#define __omTypeAlloc0Chunk(type, addr, size)               \
do                                                          \
{                                                           \
  void* __om_addr;                                          \
  size_t __om_size = (size) + SIZEOF_OM_ALIGNMENT;          \
  if (__om_size <= OM_MAX_BLOCK_SIZE)                       \
  {                                                         \
    omBin __om_bin = omSmallSize2Bin(__om_size);            \
    __omTypeAlloc0Bin(void*, __om_addr, __om_bin);          \
    *((void**) __om_addr) = (void*) __om_bin->current_page; \
  }                                                         \
  else                                                      \
  {                                                         \
    __om_addr = omAllocLargeChunk(__om_size);               \
    memset(__om_addr, 0, __om_size);                        \
    *((void**) __om_addr) = om_LargePage;                   \
  }                                                         \
  addr = (type) (__om_addr + SIZEOF_OM_ALIGNMENT);          \
}                                                           \
while (0)

#define __omFreeChunk(addr)                                 \
do                                                          \
{                                                           \
  void* __addr = ((void*) (addr)) - SIZEOF_OM_ALIGNMENT;    \
  omBinPage __om_page = *((omBinPage*) __addr);             \
  __omFreeToPage(__addr, __om_page);                        \
}                                                           \
while (0)

/*******************************************************************
 *  
 *  middle level
 *  
 *******************************************************************/

#if defined(OM_INLINE)

#define OM_ALLOCBIN_FUNC_WRAPPER(func)         \
OM_INLINE void* _om##func (omBin bin)         \
{                                               \
  void* addr;                                   \
  __omType##func (void*, addr, bin);           \
  return addr;                                  \
}

#define OM_ALLOCSIZE_FUNC_WRAPPER(func)         \
OM_INLINE void* _om##func (size_t size)         \
{                                               \
  void* addr;                                   \
  __omType##func (void*, addr, size);           \
  return addr;                                  \
}

#define OM_FREE_FUNC_WRAPPER(func)              \
OM_INLINE void _om##func (void* addr)           \
{                                               \
  __om##func (addr);                            \
}

#define OM_FREEBLOCK_FUNC_WRAPPER(func)             \
OM_INLINE void _om##func (void* addr, size_t size)  \
{                                                   \
  __om##func (addr, size);                          \
}

OM_ALLOCBIN_FUNC_WRAPPER(AllocBin)
OM_ALLOCBIN_FUNC_WRAPPER(Alloc0Bin)
OM_FREE_FUNC_WRAPPER(FreeBin)
OM_ALLOCSIZE_FUNC_WRAPPER(AllocBlock)
OM_ALLOCSIZE_FUNC_WRAPPER(Alloc0Block)
OM_FREEBLOCK_FUNC_WRAPPER(FreeBlock)
OM_ALLOCSIZE_FUNC_WRAPPER(AllocChunk)
OM_ALLOCSIZE_FUNC_WRAPPER(Alloc0Chunk)
OM_FREE_FUNC_WRAPPER(FreeChunk)

#else /* ! OM_INLINE */

#define _omAllocBin      omFuncAllocBin
#define _omAlloc0Bin     omFuncAlloc0Bin
#define _omFreeBin       omFuncFreeBin
#define _omAllocBlock     omFuncAllocBlock
#define _omAlloc0Block    omFuncAlloc0Block
#define _omFreeBlock      omFuncFreeBlock
#define _omAlloc          omFuncAlloc
#define _omAlloc0         omFuncAlloc0
#define _omFree           omFuncFree

#endif /* OM_INLINE */

omBin omGetSpecBin(size_t size);
void  omUnGetSpecBin(omBin *bin);
unsigned long omGetNewStickyAllBinTag();
void omSetStickyAllBinTag(unsigned long sticky);
void omUnSetStickyAllBinTag(unsigned long sticky);
void omDeleteStickyAllBinTag(unsigned long sticky);

#endif /* ! OM_GENERATE_INC */

#endif /* OM_PRIVATE_H */
