/*******************************************************************
 *  File:    omAlloc.c
 *  Purpose: implementation of main omalloc functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAlloc.c,v 1.5 2000-05-31 13:34:29 obachman Exp $
 *******************************************************************/
#ifndef OM_ALLOC_C
#define OM_ALLOC_C

#include "omAlloc.h"

/*******************************************************************
 *  
 *  global variables
 *  
 *******************************************************************/

omBinPage_t om_ZeroPage[] = {{0, NULL, NULL, NULL, NULL}};
omBinPage_t om_CheckPage[] = {{0, NULL, NULL, NULL, NULL}};
omBinPage_t om_LargePage[] = {{0, NULL, NULL, NULL, NULL}};
omBin_t     om_LargeBin[] = {{om_LargePage, NULL, NULL, 0, 0, 0}};
omBin_t     om_CheckBin[] = {{om_CheckPage, NULL, NULL, 0, 0, 0}};
omSpecBin om_SpecBin = NULL;

/*******************************************************************
 *  
 *  Definition of Func
 *  
 *******************************************************************/

#define ALLOCBIN_FUNC_WRAPPER(func)             \
void* omFunc##func (omBin bin)                  \
{                                               \
  void* addr;                                   \
  __omType##func (void*, addr, bin);            \
  return addr;                                  \
}

#define REALLOCBIN_FUNC_WRAPPER(func)                               \
void* omFunc##func (void* old_addr, omBin old_bin, omBin new_bin)   \
{                                                                   \
  void* new_addr;                                                   \
  __omType##func (old_addr, old_bin, void*, new_addr, new_bin);     \
  return new_addr;                                                  \
}

#define ALLOCSIZE_FUNC_WRAPPER(func)            \
void* omFunc##func (size_t size)                \
{                                               \
  void* addr;                                   \
  __omType##func (void*, addr, size);           \
  return addr;                                  \
}

#define REALLOCSIZE_FUNC_WRAPPER(func)                                  \
void* omFunc##func (void* old_addr, size_t old_size, size_t new_size)   \
{                                                                       \
  void* new_addr;                                                       \
  __omType##func (old_addr, old_size, void*, new_addr, new_size);       \
  return new_addr;                                                      \
}

#define REALLOC_FUNC_WRAPPER(func)                      \
void* omFunc##func (void* old_addr, size_t new_size)    \
{                                                       \
  void* new_addr;                                       \
  __omType##func (old_addr, void*, new_addr, new_size); \
  return new_addr;                                      \
}

#define FREESIZE_FUNC_WRAPPER(func)             \
void omFunc##func (void* addr, size_t size)     \
{                                               \
  __om##func (addr, size);                      \
}

#define FREEBIN_FUNC_WRAPPER(func)             \
void omFunc##func (void* addr, omBin bin)     \
{                                               \
  __om##func (addr, bin);                      \
}

#define FREE_FUNC_WRAPPER(func)                 \
void omFunc##func (void* addr)                  \
{                                               \
  __om##func (addr);                            \
}

ALLOCBIN_FUNC_WRAPPER(AllocBin)
ALLOCBIN_FUNC_WRAPPER(Alloc0Bin)
REALLOCBIN_FUNC_WRAPPER(ReallocBin)
REALLOCBIN_FUNC_WRAPPER(Realloc0Bin)
FREEBIN_FUNC_WRAPPER(FreeBin)

ALLOCSIZE_FUNC_WRAPPER(Alloc)
ALLOCSIZE_FUNC_WRAPPER(Alloc0)
REALLOCSIZE_FUNC_WRAPPER(ReallocSize)
REALLOCSIZE_FUNC_WRAPPER(Realloc0Size)
REALLOC_FUNC_WRAPPER(Realloc)
REALLOC_FUNC_WRAPPER(Realloc0)
FREESIZE_FUNC_WRAPPER(FreeSize)
FREE_FUNC_WRAPPER(Free)

#ifdef OM_ALIGNMENT_NEEDS_WORK
ALLOCSIZE_FUNC_WRAPPER(AllocAligned)
ALLOCSIZE_FUNC_WRAPPER(Alloc0Aligned)
REALLOCSIZE_FUNC_WRAPPER(ReallocAlignedSize)
REALLOCSIZE_FUNC_WRAPPER(Realloc0AlignedSize)
REALLOC_FUNC_WRAPPER(ReallocAligned)
REALLOC_FUNC_WRAPPER(Realloc0Aligned)
#endif /* OM_ALIGNMENT_NEEDS_WORK */

/*******************************************************************
 *  
 *  Local stuff
 *  
 *******************************************************************/

/* Get new page and initialize */
static omBinPage omAllocNewBinPage(omBin bin)
{
  omBinPage newpage;
  void* tmp;
  int i = 1;

  if (bin->max_blocks > 0) newpage = omAllocBinPage();
  else newpage = omAllocBinPages(-bin->max_blocks);

  omAssume(omIsAddrPageAligned((void*) newpage));

  omSetTopBinAndStickyOfPage(newpage, bin, bin->sticky);
  newpage->used_blocks = -1;
  newpage->current = (void*) (((void*)newpage) + SIZEOF_OM_BIN_PAGE_HEADER);
  tmp = newpage->current;
  while (i < bin->max_blocks)
  {
    tmp = *((void**)tmp) = ((void**) tmp) + bin->sizeW;
    i++;
  }
  *((void**)tmp) = NULL;
  omAssume(omListLength(newpage->current) ==  
           (bin->max_blocks > 1 ? bin->max_blocks : 1));
  return newpage;
}

/* primitives for handling of list of pages */
OM_INLINE_LOCAL void omTakeOutBinPage(omBinPage page, omBin bin)
{
  if (bin->current_page == page) 
  {
    if (page->next == NULL) 
    {
      if (page->prev == NULL)
      {
        omAssume(bin->last_page == page);
        bin->last_page = NULL;
        bin->current_page = om_ZeroPage;
        return;
      }
      bin->current_page = page->prev;
    }
    else
      bin->current_page = page->next;
  }
  if (bin->last_page == page)
  {
    omAssume(page->prev != NULL && page->next == NULL);
    bin->last_page = page->prev;
  }
  else
  {
    omAssume(page->next != NULL);
    page->next->prev = page->prev;
  }
  if (page->prev != NULL) page->prev->next = page->next;
}

OM_INLINE_LOCAL void omInsertBinPage(omBinPage after, omBinPage page, omBin bin)
{
  if (bin->current_page == om_ZeroPage)
  {
    omAssume(bin->last_page == NULL);
    page->next = NULL;
    page->prev = NULL;
    bin->current_page = page;
    bin->last_page = page;
  }
  else
  {
    omAssume(after != NULL && bin->last_page != NULL);
    if (after == bin->last_page) 
    {
      bin->last_page = page;
    }
    else
    {
      omAssume(after->next != NULL);
      after->next->prev = page;
    }
    page->next = after->next;
    after->next = page;
    page->prev = after;
  }
}

/* bin->current_page is empty, get new bin->current_page, return addr*/
void* omAllocBinFromFullPage(omBin bin)
{
  void* addr;
  omBinPage newpage;
  omAssume(bin->current_page->current == NULL);
  
  if (bin->current_page != om_ZeroPage)
  {
    omAssume(bin->last_page != NULL);
    bin->current_page->used_blocks = 0;
  }

  if (bin->current_page->next != NULL)
  {
    omAssume(bin->current_page->next->current != NULL);
    newpage = bin->current_page->next;
  }
  else
  {
    // need to Allocate new page
    newpage = omAllocNewBinPage(bin);
    omInsertBinPage(bin->current_page, newpage, bin);
  }
  bin->current_page = newpage;
  
  omAssume(newpage != NULL && newpage != om_ZeroPage && 
           newpage->current != NULL);
  __omTypeAllocFromNonEmptyPage(void*, addr, newpage);
  return addr;
}


/* page->used_blocks == 0, so, either free page or reallocate to 
   the right of current_page */
/*
 * Now: there are three different strategies here, on what to do with 
 * pages which were full and now have a free block:
 * 1.) Insert at the end (default)
 * 2.) Insert after current_page  => #define PAGE_AFTER_CURRENT
 * 3.) Let it be new current_page => #define PAGE_BEFORE_CURRENT
 * Still need to try out which is best 
 */
void  omFreeToPageFault(omBinPage page, void* addr)
{
  omBin bin;
  omAssume(page->used_blocks <= 0);

#ifdef OM_HAVE_DEBUG
  if (page->used_blocks < 0) omDebugFree(addr, 0);
#endif
    
  bin = omGetBinOfPage(page);
  if (page->current != NULL || bin->max_blocks <= 1)
  {
    // all blocks of page are now collected
    omTakeOutBinPage(page, bin);
    // page can be freed
    if (bin->max_blocks > 0)
      omFreeBinPage(page);
    else
      omFreeBinPages(page, - bin->max_blocks);
#ifdef OM_HAVE_DEBUG
    om_JustFreedPage = page;
#endif    
  }
  else
  {
    // page was full
    page->current = addr;
    page->used_blocks = bin->max_blocks - 2;
    *((void**)addr) = NULL;

    omTakeOutBinPage(page, bin);
#if defined(PAGE_BEFORE_CURRENT)
    if (bin->current_page->prev != NULL)
      omInsertBinPage(bin->current_page->prev, page);
    else
      omInsertBinPage(bin->current_page, page, bin);
    bin->current_page = page;
#else
#  if defined(PAGE_AFTER_CURRENT)
    omInsertBinPage(bin->current_page, page, bin);
#  else
    omInsertBinPage(bin->last_page, page, bin);
#  endif
#endif
  }
}

/*******************************************************************
 *  
 *  ANSI-C malloc-conforming functions
 *  
 *******************************************************************/

#include "ommalloc.c"

#ifdef OM_PROVIDE_MALLOC
#undef malloc
#undef realloc
#undef free
#undef calloc
#define ommallocFunc malloc
#define omreallocFunc realloc
#define omfreeFunc free
#define omcallocFunc calloc
#include "ommalloc.c"
#endif


#endif /* OM_ALLOC_C */
