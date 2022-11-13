/*******************************************************************
 *  File:    omAllocSystem.c
 *  Purpose: implementation of main lowl-level alloc functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_ALLOC_SYSTEM_C
#define OM_ALLOC_SYSTEM_C

#include <unistd.h>
#include <limits.h>


#include "omConfig.h"

#ifdef HAVE_OMALLOC
#include "omDefaultConfig.h"
#include "omMalloc.h"
#include "omalloc.h"
/* include after omMalloc.h */
#include <string.h>

#define OM_MALLOC_FROM_SYSTEM   OM_MALLOC_MALLOC
#define OM_REALLOC_FROM_SYSTEM  OM_MALLOC_REALLOC
#define OM_FREE_TO_SYSTEM       OM_MALLOC_FREE

/*******************************************************************
 *
 *  AllocLarge/FreeLarge if malloc can not return sizeof(addr)
 *
 *******************************************************************/
/* allocation of large addr */

#if defined(HAVE_MALLOC_SIZE) || defined(HAVE_MALLOC_USABLE_SIZE)
  #include <stdlib.h>
  #ifdef HAVE_MALLOC_H
    #include <malloc.h>
  #elif defined(HAVE_MALLOC_MALLOC_H)
    #include <malloc/malloc.h>
  #endif
#endif

#if defined(HAVE_MALLOC_SIZE)
  #define _omSizeOfLargeAddr(addr) (malloc_size(addr))
#elif defined(HAVE_MALLOC_USABLE_SIZE)
  #define _omSizeOfLargeAddr(addr) (malloc_usable_size(addr))
#else
void* omAllocLarge(size_t size)
{
  char* addr;
  size = OM_ALIGN_SIZE(size);
  addr = omAllocFromSystem(size + SIZEOF_STRICT_ALIGNMENT);
  *((size_t*) addr) = size;
  return (void *)(addr + SIZEOF_STRICT_ALIGNMENT);
}

void* omReallocLarge(void* old_addr, size_t new_size)
{
  char* _old_addr;
  char* new_addr;

  omAssume(omIsLargeAddr(old_addr));

  new_size = OM_ALIGN_SIZE(new_size);
  _old_addr = (char *)old_addr - SIZEOF_STRICT_ALIGNMENT;
  new_addr = omReallocSizeFromSystem(_old_addr,
                                     *((size_t*) _old_addr) + SIZEOF_STRICT_ALIGNMENT,
                                     new_size + SIZEOF_STRICT_ALIGNMENT);
  *((size_t*) new_addr) = new_size;
  return (void *)(new_addr + SIZEOF_STRICT_ALIGNMENT);
}

void omFreeLarge(void* addr)
{
  char* _addr = (char *)addr - SIZEOF_STRICT_ALIGNMENT;
  omFreeSizeToSystem(_addr, *((size_t*) _addr) + SIZEOF_STRICT_ALIGNMENT);
}

#define _omSizeOfLargeAddr(addr)  (*((size_t*) ((char*) addr - SIZEOF_STRICT_ALIGNMENT)))
#endif /* HAVE_MALLOC_SIZE/HAVE_MALLOC_USABLE_SIZE */

void* omAlloc0Large(size_t size)
{
  void* addr = omAllocLarge(size);
  memset(addr, 0, size);
  return addr;
}

void* omRealloc0Large(void* old_addr, size_t new_size)
{
  size_t old_size;
  char* new_addr;

  omAssume(!omIsBinPageAddr(old_addr));

  old_size = omSizeOfLargeAddr(old_addr);

  new_addr = omReallocLarge(old_addr, new_size);
  new_size = omSizeOfLargeAddr(new_addr);
  if (new_size > old_size)
    memset(new_addr + old_size, 0, new_size - old_size);
  return (void *)new_addr;
}

size_t omSizeOfLargeAddr(void* addr)
{
  return _omSizeOfLargeAddr((char *)addr);
}

size_t omSizeOfAddr(const void* addr)
{
  /*if (addr==NULL) return 0; */

  return (omIsBinPageAddr(addr) ?
#ifdef OM_HAVE_TRACK
          (omIsBinAddrTrackAddr(addr) ? omOutSizeOfTrackAddr((char *)addr) : omSizeOfBinAddr(addr)) :
#else
          omSizeOfBinAddr(addr) :
#endif
          omSizeOfLargeAddr((char *)addr));
}

size_t omSizeWOfAddr(void* addr)
{

  return (omIsBinPageAddr(addr) ?
#ifdef OM_HAVE_TRACK
          (omIsBinAddrTrackAddr(addr) ? omOutSizeOfTrackAddr(addr) >> LOG_SIZEOF_LONG : omSizeWOfBinAddr(addr)) :
#else
          omSizeWOfBinAddr(addr) :
#endif
          omSizeOfLargeAddr(addr) >> LOG_SIZEOF_LONG);
}

/*******************************************************************
 *
 *  Valloc
 *
 *******************************************************************/
#ifdef OM_HAVE_VALLOC_MMAP

#include "omMmap.c"

#define OM_VALLOC_FROM_SYSTEM   omVallocMmap
#define OM_VFREE_TO_SYSTEM      omVfreeMmap

#elif defined(OM_HAVE_VALLOC_MALLOC)

#define OM_VALLOC_FROM_SYSTEM OM_MALLOC_VALLOC
#define OM_VFREE_TO_SYSTEM    OM_MALLOC_VFREE

#else

#define OM_VALLOC_FROM_SYSTEM   omEmulateValloc
#define OM_VFREE_TO_SYSTEM      omEmulateVfree

#define OM_ALIGN_PAGE(addr) ( ((long)addr + (SIZEOF_SYSTEM_PAGE -1)) & ~(SIZEOF_SYSTEM_PAGE - 1))
/* now we implement an emulation */
void* omEmulateValloc(size_t size)
{
  void* addr;
  size_t padding = SIZEOF_VOIDP;
  size = OM_ALIGN_SIZE(size);
  while (1)
  {
    addr = OM_MALLOC_FROM_SYSTEM(size + padding);
    if (addr == NULL) return NULL;
    if ((OM_ALIGN_PAGE(addr) + SIZEOF_VOIDP) - (long) addr <= padding)
    {
      void* ret_addr = (void*) OM_ALIGN_PAGE(addr);
      *((void**) ((void*) ret_addr + size)) = addr;
      return ret_addr;
    }
    else
    {
      OM_FREE_TO_SYSTEM(addr);
      padding = padding << 1;
    }
  }
}

void omEmulateVfree(void* addr, size_t size)
{
  size = OM_ALIGN_SIZE(size);
  OM_FREE_TO_SYSTEM( *((void**) ((void*) addr + size)) );
}
#endif /* OM_HAVE_VALLOC_MMAP */

/*******************************************************************
 *
 *  System-level Alloc/Free
 *
 *******************************************************************/
void* omAllocFromSystem(size_t size)
{
  void* ptr;

  ptr = OM_MALLOC_FROM_SYSTEM(size);
  if (ptr == NULL)
  {
    OM_MEMORY_LOW_HOOK();
    ptr = OM_MALLOC_FROM_SYSTEM(size);
    if (ptr == NULL)
    {
      OM_OUT_OF_MEMORY_HOOK();
      exit(1);
    }
  }
#if defined(HAVE_MALLOC_SIZE) || defined(HAVE_MALLOC_USABLE_SIZE)
  size=_omSizeOfLargeAddr(ptr);
#else
  size=omSizeOfAddr(ptr);
#endif
#ifndef OM_NDEBUG
  if (((unsigned long) ptr) + size > om_MaxAddr)
    om_MaxAddr = ((unsigned long) ptr) + size;
  if (((unsigned long) ptr) < om_MinAddr)
    om_MinAddr = ((unsigned long) ptr);
#endif

  om_Info.CurrentBytesFromMalloc += size;
  if (om_Info.CurrentBytesFromMalloc > om_Info.MaxBytesFromMalloc)
  {
    om_Info.MaxBytesFromMalloc = om_Info.CurrentBytesFromMalloc;
#if defined(OM_HAVE_VALLOC_MMAP) && defined(OM_MALLOC_MAX_BYTES_SYSTEM)
    if (om_Info.CurrentBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM > om_Info.MaxBytesSystem)
      om_Info.MaxBytesSystem = om_Info.CurrentBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM;
#endif
#if defined(HAVE_SBRK) && !defined(OM_MALLOC_MAX_BYTES_SBRK)
    if (! om_SbrkInit) om_SbrkInit = (unsigned long) sbrk(0) - size;
    if (om_Info.MaxBytesFromMalloc
#ifndef OM_HAVE_VALLOC_MMAP
        + om_Info.CurrentBytesFromValloc
#endif
        > om_Info.MaxBytesSbrk)
    {
      om_Info.MaxBytesSbrk = (unsigned long) sbrk(0) - om_SbrkInit;
    }
#endif
  }
  OM_MALLOC_HOOK(size);
  return ptr;
}

void* omReallocFromSystem(void* addr, size_t newsize)
{
  return omReallocSizeFromSystem(addr, omSizeOfAddr(addr), newsize);
}

void* omReallocSizeFromSystem(void* addr, size_t oldsize, size_t newsize)
{
  void* res;

  /*oldsize=omSizeOfLargeAddr(addr);*/
  res = OM_REALLOC_FROM_SYSTEM(addr, newsize);
  if (res == NULL)
  {
    OM_MEMORY_LOW_HOOK();
    /* Can do a realloc again: manpage reads:
       "If realloc() fails the original block is left untouched -
       it is not freed or moved." */
    res = OM_REALLOC_FROM_SYSTEM(addr, newsize);
    if (res == NULL)
    {
      OM_OUT_OF_MEMORY_HOOK();
      /* should never get here */
      omAssume(0);
      exit(1);
    }
  }
  /*newsize=omSizeOfAddr(res);*/

#ifndef OM_NDEBUG
  if (((unsigned long) res) + newsize > om_MaxAddr)
    om_MaxAddr = ((unsigned long) res) + newsize;
  if (((unsigned long) res) < om_MinAddr)
    om_MinAddr = ((unsigned long) res);
#endif

  om_Info.CurrentBytesFromMalloc += (long) newsize - (long) oldsize;


  if (om_Info.CurrentBytesFromMalloc > om_Info.MaxBytesFromMalloc)
  {
    om_Info.MaxBytesFromMalloc = om_Info.CurrentBytesFromMalloc;
#if defined(OM_HAVE_VALLOC_MMAP) && defined(OM_MALLOC_MAX_BYTES_SYSTEM)
    if (om_Info.CurrentBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM > om_Info.MaxBytesSystem)
      om_Info.MaxBytesSystem = om_Info.CurrentBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM;
#endif
#if defined(HAVE_SBRK) && !defined(OM_MALLOC_MAX_BYTES_SBRK)
    if (om_Info.MaxBytesFromMalloc
#ifndef OM_HAVE_VALLOC_MMAP
        + om_Info.CurrentBytesFromValloc
#endif
        > om_Info.MaxBytesSbrk)
    {
      om_Info.MaxBytesSbrk = (unsigned long) sbrk(0) - om_SbrkInit;
    }
#endif
  }

  OM_REALLOC_HOOK(oldsize, newsize);
  return res;
}

void omFreeToSystem(void* addr)
{
  omFreeSizeToSystem(addr, omSizeOfAddr(addr));
}

void omFreeSizeToSystem(void* addr, size_t size)
{
  OM_FREE_TO_SYSTEM( addr );
  om_Info.CurrentBytesFromMalloc -= size;
  OM_FREE_HOOK(size);
}

void* _omVallocFromSystem(size_t size, int fail)
{
  void* page = OM_VALLOC_FROM_SYSTEM(size);
  if (page == NULL)
  {
    OM_MEMORY_LOW_HOOK();
    page = OM_VALLOC_FROM_SYSTEM(size);
    if (page == NULL)
    {
      if (fail) return NULL;
      else
      {
        OM_OUT_OF_MEMORY_HOOK();
        /* should never get here */
        omAssume(0);
        exit(1);
      }
    }
  }

#ifndef OM_NDEBUG
  if (((unsigned long) page) + size > om_MaxAddr)
    om_MaxAddr = ((unsigned long) page) + size;
  if (((unsigned long) page) < om_MinAddr)
    om_MinAddr = ((unsigned long) page);
#endif

  omAssume(omIsAddrPageAligned(page));
  om_Info.CurrentBytesFromValloc += size;
  if (om_Info.CurrentBytesFromValloc > om_Info.MaxBytesFromValloc)
  {
    om_Info.MaxBytesFromValloc = om_Info.CurrentBytesFromValloc;
#if defined(OM_HAVE_VALLOC_MMAP) && defined(OM_MALLOC_MAX_BYTES_SYSTEM)
    if (om_Info.MaxBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM > om_Info.MaxBytesSystem)
      om_Info.MaxBytesSystem = om_Info.MaxBytesFromValloc + OM_MALLOC_MAX_BYTES_SYSTEM;
#endif
#if defined(HAVE_SBRK) && !defined(OM_HAVE_VALLOC_MMAP) && !defined(OM_MALLOC_MAX_BYTES_SBRK)
    if (! om_SbrkInit) om_SbrkInit = (unsigned long) sbrk(0) - size;
    if (om_Info.CurrentBytesFromMalloc + om_Info.CurrentBytesFromValloc > om_Info.MaxBytesSbrk)
    {
      om_Info.MaxBytesSbrk = (unsigned long) sbrk(0) - om_SbrkInit;
      omAssume(om_Info.MaxBytesSbrk >= om_Info.CurrentBytesFromMalloc
               + om_Info.CurrentBytesFromValloc);
    }
#endif
  }
  OM_VALLOC_HOOK(size);
  return page;
}

void omVfreeToSystem(void* page, size_t size)
{
  omAssume(omIsAddrPageAligned(page));
  OM_VFREE_TO_SYSTEM(page, size);
  om_Info.CurrentBytesFromValloc  -= size;
  OM_VFREE_HOOK(size);
}

#endif /*HAVE_OMALLOC*/
#endif /* OM_ALLOC_SYSTEM_C */
