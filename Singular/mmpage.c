/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmpage.c,v 1.4 1999-10-19 14:55:40 obachman Exp $ */

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include "mod2.h"
#include "mmprivate.h"
#ifdef HAVE_MALLOC_H
#include "malloc.h"
#endif

#include "mmpage.h"
#include "mmemory.h"

static void* mm_FreePages = NULL;
static int mm_NumberOfFreePages = 0;

void* mmGetPage()
{
  void* page;

#ifndef PAGE_DEBUG
  if (mm_FreePages != NULL)
  {
    page = mm_FreePages;
    mm_FreePages = *((void**)mm_FreePages);
    mm_NumberOfFreePages--;
    return page;
  }
  else
#endif
    return mmVallocFromSystem(SIZE_OF_PAGE);
}

void mmFreePage(void* page)
{
  assume(page != NULL);
  *((void**)page) = mm_FreePages;
  mm_FreePages = page;
  mm_NumberOfFreePages++;
}

void mmReleaseFreePages()
{
#ifndef PAGE_DEBUG
  void** next;
  
  while (mm_FreePages != NULL)
  {
    next = *((void**)mm_FreePages);
    mmVfreeToSystem(mm_FreePages, SIZE_OF_PAGE);
    mm_FreePages = next;
  }
  mm_NumberOfFreePages = 0;
#endif
}

int mmIsAddrOnFreePage(void* addr)
{
  void* page = mm_FreePages;
  while (page != NULL)
  {
    if (mmIsAddrOnPage(addr, page)) return 1;
    page = *((void**) page);
  }
  return 0;
}

int mmGetNumberOfFreePages()
{
  return mm_NumberOfFreePages;
}

int mmGetNumberOfAllocatedPages()
{
  return mm_bytesValloc / SIZE_OF_PAGE;
}

int mmGetNumberOfUsedPages()
{
  return mmGetNumberOfAllocatedPages() - mm_NumberOfFreePages;
}

    
