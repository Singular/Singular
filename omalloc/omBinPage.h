/*******************************************************************
 *  File:    omBinPage.h
 *  Purpose: declaration of routines for primitve BinPage managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_BIN_PAGE_H
#define OM_BIN_PAGE_H

/***********************************************************************
 *
 * Macros for page manipulations
 *
 **********************************************************************/

#define omIsAddrPageAligned(addr) \
  (((long) (addr) & (SIZEOF_SYSTEM_PAGE -1)) == 0)

#define omGetPageOfAddr(addr) \
  ((void*) (((long)addr) & ~(SIZEOF_SYSTEM_PAGE -1)))

#define omGetBinPageOfAddr(addr) \
  ((omBinPage) ((long) (addr) & ~(SIZEOF_SYSTEM_PAGE -1)))

#define omIsAddrOnPage(addr, page) (omGetPageOfAddr(addr) == (void*) (page))

#define omAreAddrOnSamePage(a1, a2) \
  (omGetPageOfAddr(a1) == omGetPageOfAddr(a2))

/***********************************************************************
 *
 * Identifying whether an address is a BinPageAddr:
 *
 *******************************************************************/

/* Here is how it works (assume SIZEOF_LONG == 4, SIZEOF_SYSTEM_PAGE = 2^12):
   Let
   Addr: |    15               |  5       |    12        |
          PAGE_INDEX            PAGE_SHIFT PAGE_OFFSET

                                      PAGE_BASE

   om_PageIndicies is an array of bit-fields which is indexed by
                  PAGE_INDEX - om_MinBinPageIndex. Its maximal length
                  is 2^15. PAGE_SHIFT is used as index into the bit-field.
                  If it's value is 1, then addr is from omBinPage, else
                  not.

   om_MinPageIndex is minimal page index of registered BinPageAddr

   In other words: omIsBinPageAddr iff PAGE_INDEX >= om_MinBinPageIndex && PAGE_INDEX <=  om_MaxBinPageIndex
   && om_PageIndicies[PAGE_INDEX - om_MinPageIndex] & (1 << PAGE_SHIFT) */

extern unsigned long om_MaxBinPageIndex;
extern unsigned long om_MinBinPageIndex;
extern unsigned long *om_BinPageIndicies;

#define OM_SIZEOF_INDEX_PAGE (((unsigned long) SIZEOF_SYSTEM_PAGE) << LOG_BIT_SIZEOF_LONG)

#define omGetPageShiftOfAddr(addr) \
  ((((unsigned long) addr) & (OM_SIZEOF_INDEX_PAGE -1)) >> LOG_BIT_SIZEOF_SYSTEM_PAGE)

#define omGetPageIndexOfAddr(addr) \
  (((unsigned long) addr) >> (LOG_BIT_SIZEOF_LONG + LOG_BIT_SIZEOF_SYSTEM_PAGE))


#if !defined(OM_INLINE) || defined(OM_INTERNAL_DEBUG)
#define omIsBinPageAddr(addr) _omIsBinPageAddr(addr)
#else
/* let's hope the compiler can eliminate common subexpressions well */      \
#define omIsBinPageAddr(addr)                                               \
  ((omGetPageIndexOfAddr(addr) >= om_MinBinPageIndex) &&                    \
   (omGetPageIndexOfAddr(addr) <= om_MaxBinPageIndex) &&                    \
   ((om_BinPageIndicies[omGetPageIndexOfAddr(addr) - om_MinBinPageIndex] &   \
     (((unsigned long) 1) << omGetPageShiftOfAddr(addr))) != 0))
#endif

/*BEGINPRIVATE*/
/*******************************************************************
 *
 * Alloc/Free of BinPages
 *
 *******************************************************************/
extern omBinPage omAllocBinPages(int how_many);
extern omBinPage omAllocBinPage(void);

extern void omFreeBinPages(omBinPage page, int how_many);
#define omFreeBinPage(addr) omFreeBinPages(addr, 1)
/*ENDPRIVATE*/

#endif /* OM_BIN_PAGE_H */
