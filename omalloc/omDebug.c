/*******************************************************************
 *  File:    omTest.c
 *  Purpose: implementation of main omTest functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 7/00
 *  Version: $Id$
 *******************************************************************/
#include <mylimits.h>
#include <string.h>
#include <omalloc/omConfig.h>
#include <omalloc/om_Alloc.h>
#include <omalloc/omDebug.h>
#include <omalloc/omReturn.h>

#ifndef OM_NDEBUG
/*******************************************************************
 *
 * Declarations
 *
 *******************************************************************/

static void* __omDebugAlloc(void* size_bin, omTrackFlags_t  flags, char track, OM_FLR_DECL);
static void* __omDebugRealloc(void* old_addr, void* old_size_bin, void* new_size_bin,
                              omError_t old_status, omTrackFlags_t  old_flags,
                              omTrackFlags_t  new_flags,
                              char track, OM_FLR_DECL);
static void __omDebugFree(void* addr, void* size_bin, omTrackFlags_t  flags, OM_FLR_DECL);

void* om_KeptAddr = NULL;
static unsigned long om_NumberOfKeptAddrs = 0;
void* om_LastKeptAddr = NULL;
void* om_AlwaysKeptAddrs = NULL;


/*******************************************************************
 *
 * Test routines
 *
 *******************************************************************/
#define OM_CLFL check_level OM_FL_KOMMA OM_FL
omError_t omTestAddrBin(void* addr, omBin bin, int check_level)
{
  return _omDebugAddr(addr,bin,OM_FBIN,OM_CLFL);
}
omError_t omTestBinAddrSize(void* addr, size_t size, int check_level)
{
  return _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FBINADDR,OM_CLFL);
}
omError_t omTestAddrSize(void* addr, size_t size, int check_level)
{
  return _omDebugAddr(addr,(void*)(size),OM_FSIZE,OM_CLFL);
}
omError_t omTestBinAddr(void* addr, int check_level)
{
  return _omDebugAddr(addr,NULL, OM_FBINADDR, OM_CLFL);
}
omError_t omTestAddr(void* addr, int check_level)
{
  return _omDebugAddr(addr,NULL, 0, OM_CLFL);
}
omError_t omtestAddrSize(void* addr, size_t size, int check_level)
{
  return _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FSLOPPY,OM_CLFL);
}
omError_t omtestAddr(void* addr, int check_level)
{
  return _omDebugAddr(addr,NULL, OM_FSLOPPY, OM_CLFL);
}

omError_t omTestAddrAlignedBin(void* addr, omBin bin, int check_level)
{
  return _omDebugAddr(addr,bin,OM_FBIN|OM_FALIGN,OM_CLFL);
}
omError_t omTestAddrAlignedSize(void* addr, size_t size, int check_level)
{
  return _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FALIGN,OM_CLFL);
}
omError_t omTestAddrAligned(void* addr, int check_level)
{
  return _omDebugAddr(addr,NULL, OM_FALIGN, OM_CLFL);
}
omError_t omtestAddrAlignedSize(void* addr, size_t size, int check_level)
{
  return _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FSLOPPY|OM_FALIGN,OM_CLFL);
}
omError_t omtestAddrAligned(void* addr, int check_level)
{
  return _omDebugAddr(addr,NULL, OM_FSLOPPY|OM_FALIGN, OM_CLFL);
}

omError_t omTestBin(omBin bin, int check_level)
{
  return _omDebugBin(bin, OM_CLFL);
}
omError_t omTestMemory(int check_level)
{
  return _omDebugMemory(OM_CLFL);
}

#undef MAX
#define MAX(a,b) (a > b ? a : b)
#undef MIN
#define MIN(a,b) (a < b ? a : b)

/*******************************************************************
 *
 * First level _omDebug alloc/free routines: call respective checks and dispatch
 * to routines which do the actual work
 *
 *******************************************************************/
void* _omDebugAlloc(void* size_bin, omTrackFlags_t flags, OM_CTFL_DECL)
{
  void* addr;
  OM_R_DEF;
  check = MAX(check, om_Opts.MinCheck);
  track = MAX(track, om_Opts.MinTrack);
  check = MIN(check, om_Opts.MaxCheck);
  track = MIN(track, om_Opts.MaxTrack);

  if (check)
  {
    if (check > 1)
    {
      if (flags & OM_FBIN)
        (void) _omCheckBin((omBin)size_bin, 1, check-1, omError_MemoryCorrupted, OM_FLR_VAL);
      else if (check > 2)
      {
        omAssume(flags & OM_FSIZE);
        (void) _omCheckMemory(check-2, omError_MemoryCorrupted, OM_FLR_VAL);
      }
    }
    if (size_bin == NULL && ! (flags & OM_FSLOPPY))
    {
      omReportError(omError_NullSizeAlloc, omError_NoError, OM_FLR_VAL, "");
    }
  }

  addr = __omDebugAlloc(size_bin, flags, track, OM_FLR_VAL);

#ifdef OM_INTERNAL_DEBUG
  (void) _omCheckAddr(addr, size_bin,flags|OM_FUSED,check, omError_InternalBug, OM_FLR);
#endif

  return addr;
}


void* _omDebugRealloc(void* old_addr, void* old_size_bin, void* new_size_bin,
                      omTrackFlags_t old_flags, omTrackFlags_t new_flags, OM_CTFL_DECL)
{
  void* new_addr = NULL;
  omError_t status = omError_NoError;
  OM_R_DEF;
  check = MAX(check, om_Opts.MinCheck);
  track = MAX(track, om_Opts.MinTrack);
  check = MIN(check, om_Opts.MaxCheck);
  track = MIN(track, om_Opts.MaxTrack);

  if (check)
  {
    status = _omCheckAddr(old_addr, old_size_bin, old_flags|OM_FUSED, check, omError_NoError, OM_FLR_VAL);

    if (status == omError_NoError && check > 1 && new_flags & OM_FBIN)
      status = omDoCheckBin((omBin)new_size_bin, 1, check-1, omError_MemoryCorrupted, OM_FLR_VAL);

    if (new_size_bin == NULL && !(new_flags & OM_FSLOPPY))
    {
      omReportError(omError_NullSizeAlloc, omError_NoError, OM_FLR_VAL, "");
      new_size_bin = (void*) 1;
    }
  }

  new_addr = __omDebugRealloc(old_addr, old_size_bin, new_size_bin,
                              status, old_flags, new_flags, track, OM_FLR_VAL);
#ifdef OM_INTERNAL_DEBUG
  if (status == omError_NoError)
    (void) _omCheckAddr(new_addr, new_size_bin,new_flags|OM_FUSED,check, omError_InternalBug, OM_FLR);
#endif

  return new_addr;
}

void _omDebugFree(void* addr, void* size_bin,
                  omTrackFlags_t flags, OM_CFL_DECL)
{
  OM_R_DEF;
  check = MAX(check, om_Opts.MinCheck);
  check = MIN(check, om_Opts.MaxCheck);

  if (check && _omCheckAddr(addr, size_bin, flags|OM_FUSED, check, omError_NoError, OM_FLR_VAL)) return;

  __omDebugFree(addr,size_bin,flags, OM_FLR_VAL);

#ifdef OM_INTERNAL_DEBUG
  if (flags & OM_FBIN)
    (void) _omCheckBin((omBin)size_bin, 1, check-1,omError_InternalBug, OM_FLR);
  else
    (void) _omCheckMemory(check-2,omError_InternalBug,OM_FLR);
#endif
}

void* _omDebugMemDup(void* addr, omTrackFlags_t flags, OM_CTFL_DECL)
{
  void* ret;
  size_t sizeW;
  OM_R_DEF;

  check = MAX(check, om_Opts.MinCheck);
  track = MAX(track, om_Opts.MinTrack);
  check = MIN(check, om_Opts.MaxCheck);
  track = MIN(track, om_Opts.MaxTrack);

  if (check & _omCheckAddr(addr, 0, OM_FUSED, check, omError_NoError, OM_FLR_VAL))
  {
    return NULL;
  }
  else
  {
    sizeW = omSizeWOfAddr(addr);
  }

  ret = __omDebugAlloc((void*) (sizeW << LOG_SIZEOF_LONG), OM_FSIZE | flags, track, OM_FLR_VAL);
  omMemcpyW(ret, addr, sizeW);

#ifdef OM_INTERNAL_DEBUG
  (void) _omCheckAddr(ret, (void*) (sizeW << LOG_SIZEOF_LONG),OM_FUSED|OM_FSIZE,
                     check, omError_InternalBug, OM_FLR);
#endif
  return ret;
}

char* _omDebugStrDup(const char* addr, OM_TFL_DECL)
{
#if 0
  unsigned long size;
#endif
  unsigned long i=0;
  char* ret;
  OM_R_DEF;

  if (addr == NULL)
  {
    omReportAddrError(omError_NotString, omError_NoError, (char *)addr, 0, 0, OM_FLR_VAL, "NULL String");
    return NULL;
  }
  track = MAX(track, om_Opts.MinTrack);
  track = MIN(track, om_Opts.MaxTrack);

#if 0
  // this breaks if SizeOfAddr(addr) > PAGESIZE
  if (omIsBinPageAddr(addr))
  {
    size = omSizeOfAddr(addr);
  }
  else
  {
    size = ULONG_MAX;
  }
#endif
  while ((addr[i] != '\0') /* && (i < size)*/) i++;
// there seems to be no way to check if it is really a string
#if 0
  if (i == size)
  {
    omReportAddrError(omError_NotString, omError_NoError, addr, 0, 0, OM_FLR_VAL, "Not 0 terminated");
    i = size-1;
  }
#endif
  ret = __omDebugAlloc((char*)i+1, OM_FSIZE, track, OM_FLR_VAL);
  memcpy(ret, addr, i);
  ret[i] = '\0';

#ifdef OM_INTERNAL_DEBUG
  (void) _omCheckAddr(ret, (void*)i+1,OM_FUSED|OM_FSIZE,om_Opts.MinCheck, omError_InternalBug, OM_FLR);
#endif
  return ret;
}

omError_t _omDebugAddr(void* addr, void* bin_size, omTrackFlags_t flags, OM_CFL_DECL)
{
  OM_R_DEF;
  check = MAX(check,om_Opts.MinCheck);
  check = MIN(check,om_Opts.MaxCheck);
  return _omCheckAddr(addr, bin_size,
                      OM_FUSED|flags,check,omError_NoError,OM_FLR_VAL);
}
omError_t _omDebugMemory(OM_CFL_DECL)
{
  OM_R_DEF;
  check = MAX(check,om_Opts.MinCheck);
  check = MIN(check,om_Opts.MaxCheck);
  return _omCheckMemory(check, omError_NoError,OM_FLR_VAL);
}
omError_t _omDebugBin(omBin bin, OM_CFL_DECL)
{
  OM_R_DEF;
  return _omCheckBin(bin, 1, MAX(check, om_Opts.MinCheck), omError_NoError,OM_FLR_VAL);
}

/*******************************************************************
 *
 * Second level _omDebug alloc/free routines: do the actual work
 *
 *******************************************************************/
static void* __omDebugAlloc(void* size_bin, omTrackFlags_t flags, char track, OM_FLR_DECL)
{
  void* o_addr;
  size_t o_size = (flags & OM_FBIN ? ((omBin)size_bin)->sizeW << LOG_SIZEOF_LONG : (size_bin != NULL ? (size_t) size_bin: 1));

#ifdef OM_HAVE_TRACK
  if (track > 0)
  {
    o_addr = omAllocTrackAddr(size_bin, flags, track, OM_FLR_VAL);
  }
  else
#endif
  {
    if (flags & OM_FBIN)
    {
      omBin bin = (omBin) size_bin;

      if (flags & OM_FZERO)
        __omTypeAlloc0Bin(void*, o_addr, bin);
      else
        __omTypeAllocBin(void*, o_addr, bin);
    }
    else
    {
      if (flags & OM_FZERO)
      {
#ifdef OM_ALIGNMENT_NEEDS_WORK
        if (flags & OM_FALIGN)
          __omTypeAlloc0Aligned(void*, o_addr, o_size);
        else
#endif
          __omTypeAlloc0(void*, o_addr, o_size);
      }
      else
      {
#ifdef OM_ALIGNMENT_NEEDS_WORK
        if (flags & OM_FALIGN)
          __omTypeAllocAligned(void*, o_addr, o_size);
        else
#endif
          __omTypeAlloc(void*, o_addr, o_size);
      }
    }
  }

  return o_addr;
}

static void* __omDebugRealloc(void* old_addr, void* old_size_bin, void* new_size_bin,
                              omError_t old_status, omTrackFlags_t  old_flags, omTrackFlags_t  new_flags,
                              char track, OM_FLR_DECL)
{
  void* new_addr;
  size_t old_size = (old_flags & OM_FSIZE ? (size_t) old_size_bin :
                     (omSizeOfAddr(old_addr)));
  size_t new_size;

  omAssume(new_flags & OM_FSIZE || new_flags & OM_FBIN);

  if (old_addr == NULL || ((old_flags & OM_FSIZE) && old_size_bin == NULL))
  {
    new_addr = __omDebugAlloc(new_size_bin, new_flags, track, OM_FLR_VAL);
  }
  else if (om_Opts.Keep > 0 || track > 0 || omIsTrackAddr(old_addr) || old_status != omError_NoError ||
           old_flags & OM_FKEEP || new_flags & OM_FKEEP)
  {
    new_addr = __omDebugAlloc(new_size_bin, new_flags, track, OM_FLR_VAL);
    new_size =  omSizeOfAddr(new_addr);
    old_size = omSizeOfAddr(old_addr);

    memcpy(new_addr, old_addr, (old_size < new_size ? old_size : new_size));

    if ((new_flags & OM_FZERO) && new_size > old_size)
      memset((char *)new_addr + old_size, 0, new_size - old_size);
    if (old_status == omError_NoError)
      __omDebugFree(old_addr, old_size_bin, old_flags, OM_FLR_VAL);
  }
  else
  {
    if (new_flags & OM_FBIN)
    {
      omBin new_bin = (omBin) new_size_bin;
      omBin old_bin = (omBin) old_size_bin;

      omAssume(old_flags & OM_FBIN);
      if (new_flags & OM_FZERO)
        __omTypeRealloc0Bin(old_addr, old_bin, void*, new_addr, new_bin);
      else
        __omTypeReallocBin(old_addr, old_bin, void*, new_addr, new_bin);
    }
    else
    {
      new_size = (size_t) new_size_bin;
      if (new_size == 0) new_size = 1;
      omAssume(!(new_flags & OM_FBIN) && !(old_flags & OM_FBIN));

      if (old_flags & OM_FSIZE)
      {
        size_t old_size = (size_t) old_size_bin;

        if (new_flags & OM_FZERO)
        {
#ifdef OM_ALIGNMENT_NEEDS_WORK
          if (new_flags & OM_FALIGN)
            __omTypeRealloc0AlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else
#endif
            __omTypeRealloc0Size(old_addr, old_size, void*, new_addr, new_size);
        }
        else
        {
#ifdef OM_ALIGNMENT_NEEDS_WORK
          if (new_flags & OM_FALIGN)
            __omTypeReallocAlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else
#endif
            __omTypeReallocSize(old_addr, old_size, void*, new_addr, new_size);
        }
      }
      else
      {
        if (new_flags & OM_FZERO)
        {
#ifdef OM_ALIGNMENT_NEEDS_WORK
          if (new_flags & OM_FALIGN)
            __omTypeRealloc0Aligned(old_addr, void*, new_addr, new_size);
          else
#endif
            __omTypeRealloc0(old_addr, void*, new_addr, new_size);
        }
        else
        {
#ifdef OM_ALIGNMENT_NEEDS_WORK
          if (new_flags & OM_FALIGN)
            __omTypeReallocAligned(old_addr, void*, new_addr, new_size);
          else
#endif
            __omTypeRealloc(old_addr, void*, new_addr, new_size);
        }
      }
    }
  }
  return new_addr;
}

static omBin omGetOrigSpecBinOfAddr(void* addr)
{
  if (omIsBinPageAddr(addr))
  {
#ifdef OM_HAVE_TRACK
    if (omIsBinAddrTrackAddr(addr)) return omGetOrigSpecBinOfTrackAddr(addr);
    else
#endif
    {
      omBin bin = omGetTopBinOfAddr(addr);
      if (omIsSpecBin(bin)) return bin;
    }
  }
  return NULL;
}

static void __omDebugFree(void* addr, void* size_bin, omTrackFlags_t flags, OM_FLR_DECL)
{
  omBin bin = NULL;

  if (addr == NULL || ((flags & OM_FSIZE) && size_bin == NULL)) return;
  if (om_Opts.Keep > 0)
  {
#ifdef OM_HAVE_TRACK
    if (omIsTrackAddr(addr))
      addr = omMarkAsFreeTrackAddr(addr, 1, &flags, OM_FLR_VAL);
#endif
    bin = omGetOrigSpecBinOfAddr(addr);
    if (bin != NULL)
    {
      omSpecBin s_bin = omFindInGList(om_SpecBin, next, bin, (unsigned long) bin);
      omAssume(s_bin != NULL);
      (s_bin->ref)++;
    }

    if (flags & OM_FKEEP)
    {
      *((void**) addr) = om_AlwaysKeptAddrs;
      om_AlwaysKeptAddrs = addr;
      return;
    }

    if (om_NumberOfKeptAddrs)
    {
      omAssume(om_KeptAddr != NULL && om_LastKeptAddr != NULL &&
               ((om_NumberOfKeptAddrs == 1 && om_LastKeptAddr == om_KeptAddr) ||
                (om_NumberOfKeptAddrs != 1 && om_LastKeptAddr != om_KeptAddr)));
      om_NumberOfKeptAddrs++;
      *((void**) om_LastKeptAddr) = addr;
      om_LastKeptAddr = addr;
      *((void**) addr) = NULL;
    }
    else
    {
      om_NumberOfKeptAddrs = 1;
      om_LastKeptAddr = addr;
      om_KeptAddr = addr;
      *((void**) om_LastKeptAddr) = NULL;
    }

    if (om_NumberOfKeptAddrs > om_Opts.Keep)
    {
      omError_t status = omDoCheckAddr(om_KeptAddr, NULL, OM_FKEPT, om_Opts.MinCheck,
                                       omError_MemoryCorrupted, OM_FLR_VAL);
      addr = om_KeptAddr;
      if (addr!=NULL) om_KeptAddr = *((void**) addr);
      om_NumberOfKeptAddrs--;
      if (status != omError_NoError) return;
    }
    else
      return;

    bin = omGetOrigSpecBinOfAddr(addr);
  }

#ifdef OM_HAVE_TRACK
  if (omIsTrackAddr(addr))
  {
    omMarkAsFreeTrackAddr(addr, 0, &flags, OM_FLR_VAL);
    omFreeTrackAddr(addr);
  }
  else
#endif
    if (om_Opts.Keep <= 0 && ((flags & OM_FBIN) || (flags & OM_FBINADDR)))
      __omFreeBinAddr(addr);
    else if (om_Opts.Keep <= 0 && (flags & OM_FSIZE))
      __omFreeSize(addr, (size_t) size_bin);
    else
      __omFree(addr);

  if (bin != NULL) omUnGetSpecBin(&bin);
}

void omFreeKeptAddrFromBin(omBin bin)
{
  void* addr = om_KeptAddr;
  void* prev_addr = NULL;
  void* next_addr;
  omTrackFlags_t flags;

  while (addr != NULL)
  {
    next_addr = *((void**) addr);
    if (omIsBinPageAddr(addr) && omGetTopBinOfAddr(addr) == bin)
    {
      if (prev_addr != NULL)
        *((void**) prev_addr) = next_addr;
      else
        om_KeptAddr =  next_addr;
      if (addr == om_LastKeptAddr)
        om_LastKeptAddr = prev_addr;
      om_NumberOfKeptAddrs--;
#ifdef OM_HAVE_TRACK
      if (omIsTrackAddr(addr))
      {
        omMarkAsFreeTrackAddr(addr, 0, &flags, OM_FLR);
        omFreeTrackAddr(addr);
      }
    else
#endif
      __omFree(addr);
      addr = next_addr;
    }
    else
    {
      prev_addr = addr;
      addr = next_addr;
    }
  }

  addr = om_AlwaysKeptAddrs;
  prev_addr = NULL;
  while (addr != NULL)
  {
    next_addr = *((void**) addr);
    if (omIsBinPageAddr(addr) && omGetTopBinOfAddr(addr) == bin)
    {
      if (prev_addr != NULL)
        *((void**) prev_addr) = next_addr;
      else
        om_AlwaysKeptAddrs = next_addr;
#ifdef OM_HAVE_TRACK
      if (omIsTrackAddr(addr))
      {
        omMarkAsFreeTrackAddr(addr, 0, &flags, OM_FLR);
        omFreeTrackAddr(addr);
      }
    else
#endif
      __omFree(addr);
      addr = next_addr;
    }
    else
    {
      prev_addr = addr;
      addr = next_addr;
    }
  }
}

void omFreeKeptAddr()
{
  void* next;
  omBin bin;
  omTrackFlags_t flags;
  void* addr = om_KeptAddr;

  if (om_LastKeptAddr != NULL)
    *((void**) om_LastKeptAddr) = om_AlwaysKeptAddrs;

  om_NumberOfKeptAddrs = 0;
  om_LastKeptAddr = NULL;
  om_AlwaysKeptAddrs = NULL;
  om_KeptAddr = NULL;

  while (addr != NULL)
  {
    next = *((void**)addr);
    bin = omGetOrigSpecBinOfAddr(addr);

#ifdef OM_HAVE_TRACK
    if (omIsTrackAddr(addr))
    {
      omMarkAsFreeTrackAddr(addr, 0, &flags, OM_FLR);
      omFreeTrackAddr(addr);
    }
    else
#endif
      __omFree(addr);

    addr = next;
    if (bin != NULL) omUnGetSpecBin(&bin);
  }
}

#endif /* ! OM_NDEBUG */
