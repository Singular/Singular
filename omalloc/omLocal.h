/*******************************************************************
 *  File:    omLocal.h
 *  Purpose: declaration of "local" (not visible to the outside) 
 *           routines for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omLocal.h,v 1.2 1999-11-22 18:12:59 obachman Exp $
 *******************************************************************/
#ifndef OM_LOCAL_H
#define OM_LOCAL_H

#include "omConfig.h"

/*******************************************************************
 *  
 *  Internal defines
 *  
 *******************************************************************/
/* define if you want to keep size of malloc'ed chunks */

#define OM_DEBUG_LEVEL 3
/*******************************************************************
 *  
 *  Misc things
 *  
 *******************************************************************/
extern omSpecBin om_SpecBin;

/*******************************************************************
 *  
 *  Working with pages/bins
 *  
 *******************************************************************/
#define omGetTopBinOfPage(page) \
  ((omBin) ( ((unsigned long) (page->bin_sticky)) & ~(SIZEOF_VOIDP - 1)))
#define omGetStickyOfPage(page) \
  (((unsigned long) (page->bin_sticky)) & (SIZEOF_VOIDP-1))
#define omSetTopBinOfPage(page, bin) \
  (page)->bin_sticky= (void*)((unsigned long)bin + omGetStickyOfPage(page))
#define omSetStickyOfPage(page, sticky) \
  (page)->bin_sticky = (void*)((unsigned long)sticky + \
                                (unsigned long)omGetTopBinOfPage(page))
#define omSetTopBinAndStickyOfPage(page, bin, sticky) \
  (page)->bin_sticky= (void*)((unsigned long)sticky + (unsigned long)bin)

#define omGetTopBinOfAddr(addr) \
  omGetTopBinOfPage(((omBinPage) omGetPageOfAddr(addr)))

#if defined(OM_INLINE) || defined(OM_ALLOC_C)
OM_INLINE omBin omGetBinOfPage(omBinPage page)
{
  unsigned long sticky = omGetStickyOfPage(page);
  omBin bin = omGetTopBinOfPage(page);
  
  while (bin->sticky != sticky && bin->next != NULL) 
  {
    bin = bin->next;
  }
  return bin;
}
OM_INLINE omBin omGetBinOfAddr(void* addr)
{
  return omGetBinOfPage(omGetPageOfAddr(addr));
}
#else
extern omBin omGetBinOfPage(omBinPage page);
extern omBin omGetBinOfAddr(void* addr);
#endif /* defined(OM_INLINE) || defined(OM_ALLOC_C) */

/*******************************************************************
 *  
 *  Declarations
 *  
 *******************************************************************/
#include <stdio.h>
extern int omIsStaticBin(omBin bin);
extern void omPrintBinStats(FILE* fd);
extern void omPrintBinStat(FILE * fd, omBin bin);

/*******************************************************************
 *
 * om_assume(x) -- a handy macro for assumptions
 *
 ******************************************************************/
#ifndef HAVE_OM_ASSUME

#define omAssume(x) ((void) 0)

#else /* ! HAVE_OM_ASSUME */

#define omAssume(x)                             \
do                                              \
{                                               \
  if (! (x))                                    \
  {                                             \
    omReportError("omAssume violation");        \
  }                                             \
}                                               \
while (0)

#endif /* HAVE_OM_ASSUME */

extern const char* omReportError(const char* msg);
#define omError(msg)  omReportError(msg)

/*******************************************************************
 *******************************************************************
 **
 ** Inlines
 **
 ******************************************************************
 ******************************************************************/

#if defined(OM_INLINE ) || defined(OM_ALLOC_C)
#include <stdio.h>

/**********************************************************************
 *
 * malloc/free routine from/to system
 *
 **********************************************************************/
#ifdef OM_HAVE_STAT
static size_t om_bytesMalloc = 0;
static size_t om_bytesValloc = 0;
#define omAddCounter(c, s) 
do
{
  (c) += (s);
  OM_CHECK_PRINT(om_bytesMalloc, om_bytesValloc);
}
while (0)
#define OM_STAT_REAL_SIZE(size) size = size + SIZEOF_OM_ALIGNMENT
#define OM_STAT_GET_SIZEOF_ADDR(addr, size)     \
do                                              \
{                                               \
  addr = addr - SIZEOF_OM_ALIGNMENT;            \
  size = *((size_t*) addr);                     \
}                                               \
while (0)

#define OM_STAT_SET_SIZEOF_PTR(ptr, size)       \
do                                              \
{                                               \
  *((size_t*) ptr) = size;                      \
  ptr = ptr + SIZEOF_OM_ALIGNMENT;              \
}                                               \
while (0)
#define OM_STAT_DECLARE(x) x
#else

#define omAddCounter(c, s) do {} while(0)
#define OM_STAT_REAL_SIZE(size) do {} while(0)
#define OM_STAT_GET_SIZEOF_ADDR(addr, size) do {} while(0)
#define OM_STAT_SET_SIZEOF_PTR(ptr, size) do {} while(0)
#define OM_STAT_DECLARE(x) do {} while(0)

#endif /* OM_HAVE_STAT */

OM_INLINE void* omMallocFromSystem(size_t size)
{
  void* ptr;
  OM_STAT_REAL_SIZE(size);

  ptr = OM_MALLOC(size);
  if (ptr == NULL)
  {
    OM_OUT_OF_MEMORY_HOOK();
    omReleaseFreePages();
    ptr = OM_MALLOC(size);
    if (ptr == NULL)
    {
      OM_OUT_OF_MEMORY();
      exit(1);
    }
  }
  omAddCounter(om_bytesMalloc, size);
  OM_STAT_SET_SIZEOF_PTR(ptr, size);
  return ptr;
}

OM_INLINE void* omReallocFromSystem(void* addr, size_t newsize)
{
  void* res;
  OM_STAT_DECLARE(size_t old_size);
  
  OM_STAT_REAL_SIZE(newsize);
  OM_STAT_GET_SIZEOF_ADDR(addr, old_size);

  res = OM_REALLOC(addr, newsize);
  if (res == NULL)
  {
    OM_OUT_OF_MEMORY_HOOK();
    omReleaseFreePages();
    /* Can do a realloc again: manpage reads:
       "If realloc() fails the original block is left untouched - 
       it is not freed or moved." */
    res = OM_REALLOC(addr, newsize); 
    if (res == NULL)
    {
      OM_OUT_OF_MEMORY();
      /* should never get here */
      exit(1);
    }
  }
  
  omAddCounter(om_bytesMalloc, (int) newsize - (int) new_size);
  OM_STAT_SET_SIZEOF_PTR(res, new_size);
 return res;
}
 
OM_INLINE void omFreeToSystem(void* addr)
{
  OM_STAT_DECLARE(size_t size);
  OM_STAT_GET_SIZEOF_ADDR(addr, size);
  OM_FREE( addr );
  omAddCounter(om_bytesMalloc, - size);
}

OM_INLINE void* omVallocFromSystem(size_t size)
{
  void* page = OM_VALLOC(size);
  if (page == NULL)
  {
    OM_OUT_OF_MEMORY_HOOK();
    omReleaseFreePages();
    page = OM_VALLOC(size);
    if (page == NULL)
    {
      OM_OUT_OF_MEMORY();
      /* should never get here */
      exit(1);
    }
  }
  omAddCounter(om_bytesValloc, size);
  return page;
}

OM_INLINE void omVfreeToSystem(void* page, size_t size)
{
  OM_VFREE(page);
  omAddCounter(om_bytesValloc, -size);
}

#else /* ! defined(OM_INLINE ) || defined(OM_ALLOC_C) */

extern void* omMallocFromSystem(size_t size);
extern void* omReallocFromSystem(void* addr, size_t new_size);
extern void  omFreeToSystem(void* addr);
extern void* omVallocFromSystem(size_t size);
extern void  omVfreeToSystem(void* addr, size_t size);

#endif /* defined(OM_INLINE ) || defined(OM_ALLOC_C) */

#endif /* OM_LOCAL_H */
