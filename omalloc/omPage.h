/*******************************************************************
 *  File:    omPage.h
 *  Purpose: declaration of routines for primitve page managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omPage.h,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/
#ifndef OM_PAGE_H
#define OM_PAGE_H

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
