/*******************************************************************
 *  File:    omPage.c
 *  Purpose: implementation of routines for primitve page managment
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omPage.c,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/

#include "omConfig.h"
#include "omPage.h"
#include "omPrivate.h"
#include "omList.h"
#include "omLocal.h"

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

    
