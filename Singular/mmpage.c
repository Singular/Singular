/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmpage.c,v 1.1 1998-12-02 13:57:35 obachman Exp $ */

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include "mod2.h"
#ifdef HAVE_MALLOC_H
#include "malloc.h"
#endif

#include "mmpage.h"
#include "tok.h" /* needed only for m2_end */

static void* mm_FreePages = NULL;
static int mm_NumberOfFreePages = 0;
int mm_bytesValloc = 0;

void* mmGetPage()
{
  void* page;

#ifndef PAGE_DEBUG
  if (mm_FreePages != NULL)
  {
    page = mm_FreePages;
    mm_FreePages = *((void**)mm_FreePages);
    mm_NumberOfFreePages--;
  }
  else
#endif
  {
    page = PALLOC(SIZE_OF_PAGE);
    if (page == NULL)
    {
      (void)fprintf( stderr, "\nerror: no more memory\n" );
      m2_end(14);
    }
      
#ifdef HAVE_PAGE_ALIGNMENT
    assume(mmIsAddrPageAligned(page));
#endif    
    mm_bytesValloc += SIZE_OF_PAGE;
  }
  
  if (BVERBOSE(V_SHOW_MEM)) mmCheckPrint();
  return (void*) page;
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
    PFREE(mm_FreePages);
    mm_FreePages = next;
    mm_bytesValloc -= SIZE_OF_PAGE;
  }
  mm_NumberOfFreePages = 0;
#endif
}

#ifdef HAVE_PAGE_ALIGNMENT
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
#endif

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

    
