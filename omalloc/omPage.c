/*******************************************************************
 *  File:    omPage.c
 *  Purpose: implementation of routines for primitve page managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omPage.c,v 1.2 1999-12-13 16:27:58 obachman Exp $
 *******************************************************************/

#include <limits.h>
#include "omConfig.h"
#include "omPage.h"
#include "omPrivate.h"
#include "omList.h"
#include "omLocal.h"

/***********************************************************************
 *
 * Addr identification
 */

unsigned long omMaxPageIndex = 0;
unsigned long omMinPageIndex = ULONG_MAX;
unsigned long *omPageIndicies = NULL;
/* omPageIndicies has to be ReAlloced after OM_PAGE_INDEX_STEPWIDTH*BIT_SIZEOF_LONG
   new pages were allocated */
#define OM_PAGE_INDEX_STEPWIDTH 128

void omPageIndexFault(unsigned long index)
{
  omAssume(index > omMaxPageIndex || index < omMinPageIndex);
  
  if (omPageIndicies == NULL)
  {
    omTypeAlloc0Block(omPageIndicies, unsigned long *, SIZEOF_LONG);
    omMaxPageIndex = index;
    omMinPageIndex = index;
  }
  else
  {
    if (index < omMinPageIndex)
    {
      unsigned long i;
      unsigned long old_length = omMaxPageIndex - omMinPageIndex + 1;
      unsigned long new_length = omMaxPageIndex - index + OM_PAGE_INDEX_STEPWIDTH;
      unsigned long offset = old_length - new_length;
      
      omPageIndicies 
        = omReallocBlock(omPageIndicies, old_length*SIZEOF_LONG, new_length*SIZEOF_LONG);
      
      for (i=old_length -1; i >= 0; i--)
      {
        omPageIndicies[i+offset] = omPageIndicies[i];
      }
      for (i=0; i<offset; i++)
      {
        omPageIndicies[i] = 0;
      }
      omMinPageIndex -= offset;
    }
    else
    {
      omPageIndicies 
        = omRealloc0Block(omPageIndicies, 
                          (omMaxPageIndex - omMinPageIndex + 1)*SIZEOF_LONG,
                          (index - omMinPageIndex + OM_PAGE_INDEX_STEPWIDTH)*SIZEOF_LONG);
      omMaxPageIndex =  index + OM_PAGE_INDEX_STEPWIDTH - omMaxPageIndex;
    }
  }
}
  

static void* om_FreePages = NULL;
static int om_NumberOfAllocatedPages = 0;

void* omGetPage()
{
  void* page;

  if (om_FreePages != NULL)
  {
    page = om_FreePages;
    om_FreePages = *((void**)om_FreePages);
    return page;
  }
  else
  {
    om_NumberOfAllocatedPages++;
    return omVallocFromSystem(SIZEOF_OM_PAGE);
  }
}

void omFreePage(void* page)
{
  omAssume(page != NULL);
  *((void**)page) = om_FreePages;
  om_FreePages = page;
}

void omReleaseFreePages()
{
  void** next;
  
  while (om_FreePages != NULL)
  {
    next = *((void**)om_FreePages);
    omVfreeToSystem(om_FreePages, SIZEOF_OM_PAGE);
    om_FreePages = next;
    om_NumberOfAllocatedPages--;
  }
}

int omIsAddrOnFreePage(void* addr)
{
  void* page = om_FreePages;
  while (page != NULL)
  {
    if (omIsAddrOnPage(addr, page)) return 1;
    page = *((void**) page);
  }
  return 0;
}

int omGetNumberOfFreePages()
{
  return omListLength(om_FreePages);
}

int omGetNumberOfAllocatedPages()
{
  return  om_NumberOfAllocatedPages;
}

int omGetNumberOfUsedPages()
{
  return omGetNumberOfAllocatedPages() - omGetNumberOfFreePages();
}

    
