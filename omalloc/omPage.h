/*******************************************************************
 *  File:    omPage.h
 *  Purpose: declaration of routines for primitve page managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omPage.h,v 1.2 1999-12-13 16:27:59 obachman Exp $
 *******************************************************************/
#ifndef OM_PAGE_H
#define OM_PAGE_H

/***********************************************************************
 *
 * Identifying whether an address is from a page requested with omGetPage()
 * For this to work, omRegisterExternalAddr must be called with every address
 * gotten from an external malloc 
 */

/* Here is how it works (assume SIZEOF_LONG == 4, SIZEOF_SYSTEM_PAGE = 2^12)
   Let
   Addr: |    15               |  5       |    12        |
          PAGE_INDEX           PAGE_SHIFT  PAGE_OFFSET

                                   PAGE_BASE

   omPageIndicies is an array of bit-fields which is indexed by
                  PAGE_INDEX - omMinPageIndex. I'ts maximal length
                  is 2^15. PAGE_SHIFT is used as index into the bit-field.
                  If it's value is 1, then addr is from omPage, else
                  not.

   omMinPageIndex is iminimal page index of registered addr

   In other words: omIsPageAddr iff
   omPageIndicies[PAGE_INDEX - omMinPageIndex] & (1 << PAGE_SHIFT) */


#define OM_SIZEOF_INDEX_PAGE (SIZEOF_SYSTEM_PAGE << LOG_BIT_SIZEOF_LONG)

#define omGetPageShiftOfAddr(addr) \
  (((unsigned long) addr) & (OM_SIZEOF_INDEX_PAGE -1)) >> LOG_BIT_SIZEOF_SYSTEM_PAGE)

#define omGetPageIndexOfAddr(addr) \
  (((unsigned long) addr) >> (LOG_BIT_SIZEOF_LONG + LOG_BIT_SIZEOF_SYSTEM_PAGE))

#define omIsPageAddr(addr) \
   (omPageIndicies[omGetPageIndexOfAddr(addr) - omMinPageIndex] & \
    (1 << omGetPageShiftOfAddr(addr)))

extern void omPageIndexFault(unsigned long page_index);
extern unsigned long omMaxPageIndex;
extern unsigned long omMinPageIndex;
extern unsigned long *omPageIndicies;

#define omRegisterPageIndex(index)                      \
do                                                      \
{                                                       \
  if (index > omMinPageIndex || index < omMaxPageIndex) \
  {                                                     \
    omPageIndexFault(index);                            \
  }                                                     \
}                                                       \
while (0)

#define omRegisterExternalAddr(addr)                        \
do                                                          \
{                                                           \
  unsigned long _omPageIndex = omGetPageIndexOfAddr(addr);  \
  omRegisterPageIndex(_omPageIndex);                        \
  omPageIndicies[_omPageIndex - omMinPageIndex] &=          \
    ~ (1 << omGetPageShiftOfAddr(addr));                    \
}                                                           \
while (0)

#define omRegisterPageAddr(addr)                            \
do                                                          \
{                                                           \
  unsigned long _omPageIndex = omGetPageIndexOfAddr(addr);  \
  omRegisterPageIndex(_omPageIndex);                        \
  omPageIndicies[_omPageIndex - omMinPageIndex] |=          \
      (1 << omGetPageShiftOfAddr(addr));                    \
}                                                           \
while (0)

/***********************************************************************
 *
 * declarations of procedures
 */

void* omGetPage();

void omFreePage(void* page);

void omReleaseFreePages();

int omGetNumberOfFreePages();
int omGetNumberOfUsedPages();
int omGetNumberOfAllocatedPages();

/***********************************************************************
 *
 * Macros for page manipulations
 */

#define omIsAddrPageAligned(addr) \
  (((long) (addr) & (SIZEOF_SYSTEM_PAGE -1)) == 0)

#define omGetPageOfAddr(addr) \
  ((void*) ((long) (addr) & ~(SIZEOF_SYSTEM_PAGE -1)))

#define omIsAddrOnPage(addr, page) (omGetPageOfAddr(addr) == (void*) (page))

#define omAreAddrOnSamePage(a1, a2) \
  (omGetPageOfAddr(a1) == omGetPageOfAddr(a2))

int omIsAddrOnFreePage(void* addr);

#define omIsNotAddrOnFreePage(addr) (!omIsAddrOnFreePage(addr))

#endif /* OM_PAGE_H */
