/*******************************************************************
 *  File:    omtTestAlloc.c
 *  Purpose: alloc function to be included in omMain.c
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include "omtTest.h"

#ifdef TEST_CHECK
#define OM_CHECK CHECK_LEVEL
#define omtTestAlloc   omtTestAllocDebug
#define omtTestRealloc omtTestReallocDebug
#define omtTestFree    omtTestFreeDebug
#define omtTestDup     omtTestDupDebug
#endif

#ifdef TEST_KEEP
#define OM_CHECK CHECK_LEVEL
#define OM_KEEP  1
#define omtTestAlloc   omtTestAllocKeep
#define omtTestRealloc omtTestReallocKeep
#define omtTestFree    omtTestFreeKeep
#define omtTestDup     omtTestDupKeep
#endif

#include "omalloc.h"

#ifndef OM_ALIGNMENT_NEEDS_WORK
#define omSmallSize2AlignedBin omSmallSize2Bin
#endif

#ifdef HAVE_OMALLOC
void omtTestAlloc(omMemCell cell, unsigned long spec)
{
  size_t size = GET_SIZE(spec);
  void* addr;
  omBin bin = NULL;
  omBin orig_bin = NULL;

  if (IS_BIN(spec) && (size <= OM_MAX_BLOCK_SIZE || IS_SPEC_BIN(spec)))
  {
    if (IS_SPEC_BIN(spec))
    {
      if (IS_ALIGNED(spec))
        bin = omGetAlignedSpecBin(size);
      else
        bin = omGetSpecBin(size);
    }
    else
    {
      if (IS_ALIGNED(spec))
        bin = omSmallSize2AlignedBin(size);
      else
        bin = omSmallSize2Bin(size);
    }

    if (IS_STICKY_BIN(spec))
    {
      orig_bin = bin;
      bin = omtGetStickyBin(bin);
    }

    if (IS_INLINE(spec))
    {
      if (IS_ZERO(spec))
        addr = omAlloc0Bin(bin);
      else
        addr = omAllocBin(bin);
    }
    else
    {
      if (IS_ZERO(spec))
        omTypeAlloc0Bin(void*, addr, bin);
      else
        omTypeAllocBin(void*, addr, bin);
    }
  }
  else
  {
    if (IS_INLINE(spec))
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
        {
          if (IS_SLOPPY(spec))
            addr = omalloc0(size);
          else
            addr = omAlloc0Aligned(size);
        }
        else
          addr = omAlloc0(size);
      }
      else
      {
        if (IS_ALIGNED(spec))
        {
          if (IS_SLOPPY(spec))
            addr = omalloc(size);
          else
            addr = omAllocAligned(size);
        }
        else
          addr = omAlloc(size);
      }
    }
    else
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
          omTypeAlloc0Aligned(void*, addr, size);
        else
          omTypeAlloc0(void*, addr, size);
      }
      else
      {
        if (IS_ALIGNED(spec))
          omTypeAllocAligned(void*, addr, size);
        else
          omTypeAlloc(void*, addr, size);
      }
    }
  }
  cell->addr = addr;
  cell->bin = bin;
  cell->orig_bin = orig_bin;
  cell->spec = spec;

  InitCellAddrContent(cell);

  omtTestDebug(cell);
}

void omtTestFree(omMemCell cell)
{
  void* addr = cell->addr;
  unsigned long spec = cell->spec;
  omBin bin = cell->bin;
  omBin orig_bin = cell->orig_bin;
  size_t size = GET_SIZE(spec);

  omtTestDebug(cell);

  if (IS_FREE_SIZE(spec))
  {
    if (IS_SLOPPY(spec))
      omfreeSize(addr, size);
    else
      omFreeSize(addr, size);
  }
  else if (bin != NULL && IS_FREE_BIN(spec))
    omFreeBin(addr, bin);
  else if (IS_FREE_BINADDR(spec) && (bin != NULL) && (size <= OM_MAX_BLOCK_SIZE))
  {
    omFreeBinAddr(addr);
  }
  else
  {
    if (IS_SLOPPY(spec))
      omfree(addr);
    else
      omFree(addr);
  }

  if (bin != NULL && IS_SPEC_BIN(spec))
  {
    if (orig_bin != NULL)
      omUnGetSpecBin(&orig_bin);
    else
      omUnGetSpecBin(&bin);
  }

  cell->addr = NULL;
  cell->spec = 0;
  cell->bin = NULL;
  cell->orig_bin = NULL;
}

void omtTestRealloc(omMemCell cell, unsigned long new_spec)
{
  void* old_addr = cell->addr;
  unsigned long old_spec = cell->spec;
  omBin old_bin = cell->bin;
  omBin old_orig_bin = cell->orig_bin;
  size_t old_size = GET_SIZE(old_spec);
  void* new_addr;
  omBin new_bin = NULL;
  omBin new_orig_bin = NULL;
  size_t new_size = GET_SIZE(new_spec);
  size_t real_old_size = omSizeOfAddr(old_addr);
  size_t min_size;

  omtTestDebug(cell);

  if (old_bin != NULL && IS_FREE_BIN(old_spec) &&
      IS_BIN(new_spec) && ((new_size <= OM_MAX_BLOCK_SIZE) || IS_SPEC_BIN(new_spec)))
  {
    if (IS_SPEC_BIN(new_spec))
    {
      if (IS_ALIGNED(new_spec))
        new_bin = omGetAlignedSpecBin(new_size);
      else
        new_bin = omGetSpecBin(new_size);
    }
    else
    {
      if (IS_ALIGNED(new_spec))
        new_bin = omSmallSize2AlignedBin(new_size);
      else
        new_bin = omSmallSize2Bin(new_size);
    }

    if (IS_STICKY_BIN(new_spec))
    {
      new_orig_bin = new_bin;
      new_bin = omtGetStickyBin(new_bin);
    }

    if (IS_INLINE(new_spec))
    {
      if (IS_ZERO(new_spec)) new_addr = omRealloc0Bin(old_addr, old_bin, new_bin);
      else new_addr = omReallocBin(old_addr, old_bin, new_bin);
    }
    else
    {
      if (IS_ZERO(new_spec)) omTypeRealloc0Bin(old_addr, old_bin, void*, new_addr, new_bin);
      else omTypeReallocBin(old_addr, old_bin, void*, new_addr, new_bin);
    }
  }
  else
  {
    if (IS_FREE_SIZE(old_spec))
    {
      if (IS_INLINE(new_spec))
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec))
          {
            if (IS_SLOPPY(new_spec))
              new_addr = omrealloc0Size(old_addr, old_size, new_size);
            else
              new_addr = omRealloc0AlignedSize(old_addr, old_size, new_size);
          }
          else
            new_addr = omRealloc0Size(old_addr, old_size, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec))
          {
            if (IS_SLOPPY(new_spec))
              new_addr = omreallocSize(old_addr, old_size, new_size);
            else
              new_addr = omReallocAlignedSize(old_addr, old_size, new_size);
          }
          else  new_addr = omReallocSize(old_addr, old_size, new_size);
        }
      }
      else
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec)) omTypeRealloc0AlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else  omTypeRealloc0Size(old_addr, old_size, void*, new_addr, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec))  omTypeReallocAlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else  omTypeReallocSize(old_addr, old_size, void*, new_addr, new_size);
        }
      }
    }
    else
    {
      if (IS_INLINE(new_spec))
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec))
          {
            if (IS_SLOPPY(new_spec))
              new_addr = omrealloc0(old_addr, new_size);
            else
              new_addr = omRealloc0Aligned(old_addr, new_size);
          }
          else  new_addr = omRealloc0(old_addr, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec))
          {
            if (IS_SLOPPY(new_spec))
              new_addr = omrealloc(old_addr, new_size);
            else
              new_addr = omReallocAligned(old_addr, new_size);
          }
          else  new_addr = omRealloc(old_addr, new_size);
        }
      }
      else
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec)) omTypeRealloc0Aligned(old_addr, void*, new_addr, new_size);
          else  omTypeRealloc0(old_addr, void*, new_addr, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec))  omTypeReallocAligned(old_addr, void*, new_addr, new_size);
          else  omTypeRealloc(old_addr, void*, new_addr, new_size);
        }
      }
    }
  }

  if (old_bin != NULL && IS_SPEC_BIN(old_spec))
  {
    if (old_orig_bin != NULL)
      omUnGetSpecBin(&old_orig_bin);
    else
      omUnGetSpecBin(&old_bin);
  }

  new_size = omSizeOfAddr(new_addr);
  old_size = real_old_size;
  min_size = (new_size < old_size ? new_size : old_size);

  if (IS_ZERO(old_spec) && IS_ZERO(new_spec))
    TestAddrContent(new_addr, 0, new_size);
  else
  {
    TestAddrContent(new_addr, (IS_ZERO(old_spec) ? 0 : old_spec), min_size);
    if (IS_ZERO(new_spec) &&  old_size < new_size)
      TestAddrContent((char *)new_addr + old_size, 0, new_size - old_size);
  }

  cell->addr = new_addr;
  cell->spec = new_spec;
  cell->bin = new_bin;
  cell->orig_bin = new_orig_bin;
  InitCellAddrContent(cell);
  omtTestDebug(cell);
}

#define DO_STRDUP(l) (l & 1)
void omtTestDup(omMemCell cell, unsigned long spec)
{
  omtTestDebug(cell);

  if (DO_STRDUP(spec))
  {
    size_t size = omSizeOfAddr(cell->addr);
    void* new_addr;
    memset(cell->addr, 'a', size - 1);
    ((char*) cell->addr)[size-1] = '\0';
    new_addr = omStrDup(cell->addr);
    TestAddrContentEqual(new_addr, cell->addr, size);
    omFree(new_addr);
    InitCellAddrContent(cell);
  }
  else
  {
    void* new_addr = omMemDup(cell->addr);
    TestAddrContentEqual(new_addr, cell->addr, omSizeOfAddr(cell->addr));
    omFree(new_addr);
    new_addr = omMemDupAligned(cell->addr);
    TestAddrContentEqual(new_addr, cell->addr, omSizeOfAddr(cell->addr));
    omDebugAddrAlignedSize(new_addr,  omSizeOfAddr(cell->addr));
    omFree(new_addr);
  }
}
#endif
