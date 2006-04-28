/*******************************************************************
 *  File:    omInline.c
 *  Purpose: implementation of omalloc functions which could
 *           be inlined
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omInline.h,v 1.9 2006-04-28 15:17:26 Singular Exp $
 *******************************************************************/
#if defined(OM_INLINE) || defined(OM_ALLOC_C)

#ifndef OM_INLINE_H
#define OM_INLINE_H
#include <string.h>

OM_INLINE_IMPL omBin omGetBinOfPage(omBinPage page)
{
  unsigned long sticky = omGetStickyOfPage(page);
  omBin bin = omGetTopBinOfPage(page);

  if (! omIsStickyBin(bin))
  {
    while (bin->sticky != sticky && bin->next != NULL)
    {
      bin = bin->next;
    }
  }
  return bin;
}

OM_INLINE_IMPL int _omIsBinPageAddr(const void* addr)
{
  unsigned long index = omGetPageIndexOfAddr(addr);
  if (index >= om_MinBinPageIndex && index <= om_MaxBinPageIndex)
  {
    unsigned long shift = omGetPageShiftOfAddr(addr);
    return ((om_BinPageIndicies[index - om_MinBinPageIndex] & (((unsigned long) 1) << shift)) != 0L);
  }
  return 0;
}

OM_INLINE_IMPL void* _omAllocBin(omBin bin)
{
  void* addr;
  __omTypeAllocBin(void*, addr, bin);
  return addr;
}
OM_INLINE_IMPL void* _omAlloc0Bin(omBin bin)
{
  void* addr;
  __omTypeAlloc0Bin(void*, addr, bin);
  return addr;
}
OM_INLINE_IMPL void* _omReallocBin(void* addr, omBin old_bin, omBin new_bin)
{
  void* new_addr;
  __omTypeReallocBin(addr, old_bin, void*, new_addr, new_bin);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc0Bin(void* addr, omBin old_bin, omBin new_bin)
{
  void* new_addr;
  __omTypeRealloc0Bin(addr, old_bin, void*, new_addr, new_bin);
  return new_addr;
}


OM_INLINE_IMPL void* _omAlloc(size_t size)
{
  void* addr;
  __omTypeAlloc(void*, addr, size);
  return addr;
}
OM_INLINE_IMPL void* _omAlloc0(size_t size)
{
  void* addr;
  __omTypeAlloc0(void*, addr, size);
  return addr;
}
OM_INLINE_IMPL void* _omReallocSize(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  __omTypeReallocSize(addr, old_size, void*, new_addr, new_size);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc0Size(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  __omTypeRealloc0Size(addr, old_size, void*, new_addr, new_size);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc(void* addr, size_t size)
{
  void* new_addr;
  __omTypeRealloc(addr, void*, new_addr, size);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc0(void* addr, size_t size)
{
  void* new_addr;
  __omTypeRealloc0(addr, void*, new_addr, size);
  return new_addr;
}

#ifdef OM_ALIGNMENT_NEEDS_WORK
OM_INLINE_IMPL void* _omAllocAligned(size_t size)
{
  void* addr;
  __omTypeAllocAligned(void*, addr, size);
  return addr;
}
OM_INLINE_IMPL void* _omAlloc0Aligned(size_t size)
{
  void* addr;
  __omTypeAlloc0Aligned(void*, addr, size);
  return addr;
}
OM_INLINE_IMPL void* _omReallocAlignedSize(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  __omTypeReallocAlignedSize(addr, old_size, void*, new_addr, new_size);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc0AlignedSize(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;
  __omTypeRealloc0AlignedSize(addr, old_size, void*, new_addr, new_size);
  return new_addr;
}
OM_INLINE_IMPL void* _omReallocAligned(void* addr, size_t size)
{
  void* new_addr;
  __omTypeReallocAligned(addr, void*, new_addr, size);
  return new_addr;
}
OM_INLINE_IMPL void* _omRealloc0Aligned(void* addr, size_t size)
{
  void* new_addr;
  __omTypeRealloc0Aligned(addr, void*, new_addr, size);
  return new_addr;
}

OM_INLINE_IMPL void* _omMemDupAligned(void* addr)
{
  void* r;
  size_t sizeW = omSizeWOfAddr(addr);
  __omTypeAllocAligned(void*, r, sizeW << LOG_SIZEOF_LONG);
  omMemcpyW(r, addr, sizeW);
  return r;
}

#endif /* OM_ALIGNMENT_NEEDS_WORK */

OM_INLINE_IMPL char* _omStrDup(const char* s)
{
  void* r;
  size_t i=(size_t)0;

  while (s[i]) i++;
  i++;
  __omTypeAlloc(void*, r, i);
  memcpy(r, (void*) s, i);
  return (char*) r;
}

OM_INLINE_IMPL void* _omMemDup(void* addr)
{
  void* r;
  if (omIsNormalBinPageAddr(addr))
  {
    omBin bin = omGetTopBinOfAddr(addr);
    __omTypeAllocBin(void*, r, bin);
    omMemcpyW(r, addr, bin->sizeW);
  }
  else
  {
    size_t size = omSizeWOfAddr(addr);
    __omTypeAlloc(void*, r, size << LOG_SIZEOF_LONG);
    omMemcpyW(r, addr, size);
  }
  return r;
}

OM_INLINE_IMPL void* _omalloc(size_t size)
{
  void* addr;
  if (! size) size = (size_t)1;
  __omTypeAllocAligned(void*, addr,size);
  return addr;
}

OM_INLINE_IMPL void* _omalloc0(size_t size)
{
  void* addr;
  if (! size) size = (size_t)1;
  __omTypeAlloc0Aligned(void*,addr, size);
  return addr;
}

OM_INLINE_IMPL void* _omreallocSize(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;

  if (!new_size) new_size = (size_t)1;
  if (addr != NULL)
  {
    __omTypeReallocAlignedSize(addr, old_size, void* , new_addr, new_size);
  }
  else
  {
    __omTypeAllocAligned(void* , new_addr, new_size);
  }
  return new_addr;
}

OM_INLINE_IMPL void* _omrealloc0Size(void* addr, size_t old_size, size_t new_size)
{
  void* new_addr;

  if (!new_size) new_size = (size_t)1;
  if (addr != NULL && old_size > ((size_t)0))
  {
    __omTypeRealloc0AlignedSize(addr, old_size, void* , new_addr, new_size);
  }
  else
  {
    __omTypeAlloc0(void* , new_addr, new_size);
  }
  return new_addr;
}

OM_INLINE_IMPL void* _omrealloc(void* addr, size_t size)
{
  void* new_addr;

  if (!size) size = (size_t)1;
  if (addr != NULL)
  {
    __omTypeReallocAligned(addr, void* , new_addr, size);
  }
  else
  {
    __omTypeAlloc(void* , new_addr, size);
  }
  return new_addr;
}

OM_INLINE_IMPL void* _omrealloc0(void* addr, size_t size)
{
  void* new_addr;

  if (!size) size = (size_t)1;
  if (addr != NULL)
  {
    __omTypeRealloc0Aligned(addr, void* , new_addr, size);
  }
  else
  {
    __omTypeAlloc0(void* , new_addr, size);
  }
  return new_addr;
}
#endif /* OM_INLINE_H */

#endif /* defined(OM_INLINE) || defined(OM_ALLOC_C) */
