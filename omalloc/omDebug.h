/*******************************************************************
 *  File:    omDebug.h
 *  Purpose: declaration of common Debug/Check/Track stuff
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 7/00
 *  Version: $Id: omDebug.h,v 1.15 2003-05-12 14:45:32 Singular Exp $
 *******************************************************************/
#ifndef OM_DEBUG_H
#define OM_DEBUG_H


#if defined(OM_NDEBUG) || ! defined(OM_HAVE_TRACK)
#define omIsTrackAddr(addr)             0
#define omIsTrackAddrPage(page)         0
#define omIsNormalBinPageAddr(addr)     omIsBinPageAddr(addr)
#define omIsBinAddrTrackAddr(addr)      0
#else
#define omIsTrackAddrBinPage(bin_page)  ((bin_page)->used_blocks < 0)
#define omIsBinAddrTrackAddr(addr)      omIsTrackAddrBinPage(omGetBinPageOfAddr(addr))
#define omIsTrackAddr(addr)             (omIsBinPageAddr(addr) && omIsBinAddrTrackAddr(addr))
#define omIsNormalBinPageAddr(addr)     (omIsBinPageAddr(addr) && !omIsBinAddrTrackAddr(addr))
#endif

#if defined(OM_NDEBUG)
#define omMarkAsStaticAddr(addr)    ((void)0)
#define omMarkMemoryAsStatic()      ((void)0)
#define omUnMarkAsStaticAddr()      ((void)0)
#define omUnMarkMemoryAsStatic()    ((void)0)
#define omFreeKeptAddr()            ((void)0)
#define omPrintUsedAddrs(fd,m)      ((void)0)
#define omPrintUsedTrackAddrs(fd,m) ((void)0)
#else
#define OM_FBIN     1           /* size_bin is bin */
#define OM_FSIZE    2           /* size_bin is size */
#define OM_FUSED    4           /* is in use, if set */
#define OM_FKEPT    8           /* had been freed, if set */
#define OM_FSTATIC  16          /* if set, considered to be static, i.e. never be freed */
#define OM_FZERO    32          /* for Alloc0 */
#define OM_FALIGN   64          /* for AllocAligned */
#define OM_FSLOPPY  128         /* be sloppy about arguments */
#define OM_FBINADDR 256         /* addr is bin addr */
#define OM_FKEEP    512         /* addr is never really freed */
/* maximal flag: OM_FBIN and OM_FSIZE can not be at the same time,
   and so can't OM_USED and OM_KEPT. Hence 1024 - BIN - USED*/
#define OM_FMAX     1024 - OM_FBIN - OM_FUSED
typedef unsigned short omTrackFlags_t;

void* _omDebugAlloc(void* size_bin, omTrackFlags_t flags, OM_CTFL_DECL);
void* _omDebugRealloc(void* old_addr, void* old_size_bin, void* new_size_bin,
                      omTrackFlags_t old_flags, omTrackFlags_t new_flags, OM_CTFL_DECL);
void  _omDebugFree(void* addr, void* size_bin, omTrackFlags_t flags, OM_CFL_DECL);
void* _omDebugMemDup(void* addr, omTrackFlags_t flags, OM_CTFL_DECL);
char* _omDebugStrDup(const char* addr, OM_TFL_DECL);

omError_t _omDebugBin(omBin bin, OM_CFL_DECL);
omError_t _omDebugMemory(OM_CFL_DECL);
omError_t _omDebugAddr(void* addr, void* bin_size, omTrackFlags_t flags, OM_CFL_DECL);

void omFreeKeptAddr();
void omPrintUsedAddrs(FILE* fd, int max_frames);
void omPrintUsedTrackAddrs(FILE* fd, int max_frames);

void omMarkAsStaticAddr(void* addr);
void omMarkMemoryAsStatic();
void omUnMarkAsStaticAddr(void* addr);
void omUnMarkMemoryAsStatic();
#endif /* ! OM_NDEBUG */


#ifdef OM_TRACK_CUSTOM
#ifdef OM_NDEBUG
#define omSetCustomOfAddr(addr,value) ((void)0)
#define omGetCustomOfAddr(addr) ((void*)0)
#define omSetCustomOfTrackAddr(addr,value) ((void)0)
#define omGetCustomOfTrackAddr(addr) ((void*)0)
#else
void omSetCustomOfTrackAddr(void* addr, void* value);
void* omGetCustomOfTrackAddr(void* addr);
#define omSetCustomOfAddr(addr,value) \
  do{if (omIsTrackAddr(addr)) omSetCustomOfTrackAddr(addr,value);}while(0)
#define omGetCustomOfAddr(addr) \
  (omIsTrackAddr(addr) ? omGetCustomOfTrackAddr(addr) : NULL)
#endif /* OM_NDEBUG */
#endif /* OM_TRACK_CUSTOM */

/*BEGINPRIVATE*/

#if defined(OM_NDEBUG) || ! defined(OM_HAVE_TRACK)
#define omGetUsedBlocksOfPage(page)     (page->used_blocks)
#else
#define omGetUsedBlocksOfPage(page)     (page->used_blocks & ~(((unsigned long) 1) << (BIT_SIZEOF_LONG -1)))
#endif

#ifndef OM_NDEBUG
extern void* om_KeptAddr;
extern void* om_LastKeptAddr;
extern unsigned long om_MaxAddr;
extern unsigned long om_MinAddr;
extern void* om_AlwaysKeptAddrs;

void omFreeKeptAddrFromBin(omBin bin);
/***********************************************************************
 *
 * omDebugCheck.c
 *
 **********************************************************************/
omError_t omCheckPtr(const void* ptr, omError_t report, OM_FLR_DECL);
omError_t _omCheckAddr(void* addr, void* size_bin, omTrackFlags_t flags, char check,
                       omError_t report, OM_FLR_DECL);
omError_t omDoCheckBinAddr(void* addr, void* bin_size, omTrackFlags_t flags, char level,
                           omError_t report, OM_FLR_DECL);
omError_t _omCheckBin(omBin bin, int normal_bin, char check,  omError_t report, OM_FLR_DECL);
omError_t _omCheckMemory(char check,  omError_t report, OM_FLR_DECL);
omError_t omReportAddrError(omError_t error, omError_t report, void* addr,
                            void* bin_size, omTrackFlags_t flags,
                            OM_FLR_DECL, const char* fmt, ...);
omError_t omDoCheckBin(omBin bin, int normal_bin, char level,
                       omError_t report, OM_FLR_DECL);
void omIterateTroughAddrs(int normal, int track, void (*CallBackUsed)(void*), void (*CallBackFree)(void*));
void omIterateTroughBinAddrs(omBin bin, void (*CallBackUsed)(void*), void (*CallBackFree)(void*));
omError_t omDoCheckAddr(void* addr, void* bin_size, omTrackFlags_t flags, char level,
                        omError_t report, OM_FLR_DECL);
int omIsInKeptAddrList(void* addr);

/***********************************************************************
 *
 * omDebugTrack.c
 *
 **********************************************************************/
#ifdef OM_HAVE_TRACK
extern omBin_t om_StaticTrackBin[];
extern omBin om_Size2TrackBin[];
#define omSmallSize2TrackBin(size)      om_Size2TrackBin[((size) -1)>>LOG_SIZEOF_OM_ALIGNMENT]
extern omBinPage om_JustFreedPage;
extern void omFreeTrackAddr(void* addr);
extern size_t omOutSizeOfTrackAddr(void* addr);
extern omSpecBin om_SpecTrackBin;

void* omAllocTrackAddr(void* bin_size, omTrackFlags_t flags, char track, OM_FLR_DECL);
void* omMarkAsFreeTrackAddr(void* addr, int keep, omTrackFlags_t *flags, OM_FLR_DECL);
omError_t omCheckTrackAddr(void* addr, void* bin_size, omTrackFlags_t flags, char level,
                           omError_t report_error, OM_FLR_DECL);
void omPrintTrackAddrInfo(FILE* fd, void* addr, int max_frames);
omBin omGetOrigSpecBinOfTrackAddr(void* addr);
size_t omOutSizeOfTrackAddr(void* addr);
extern int omIsStaticTrackAddr(void* addr);
#define omSetTrackOfUsedBlocks(ub)      (ub |= (((unsigned long) 1) << (BIT_SIZEOF_LONG -1)))
#define omUnsetTrackOfUsedBlocks(ub)    (ub &= ~(((unsigned long) 1) << (BIT_SIZEOF_LONG -1)))
#define omIsSetTrackOfUsedBlocks(ub)    (ub & (((unsigned long) 1) << (BIT_SIZEOF_LONG -1)))
#else
#define omIsStaticTrackAddr(addr)   0
#endif
void* omAddr_2_OutAddr(void* addr);

/***********************************************************************
 *
 * omBinPage.c
 *
 **********************************************************************/
int omIsKnownMemoryRegion(omBinPageRegion region);
omError_t omCheckBinPageRegion(omBinPageRegion region, int level, omError_t report, OM_FLR_DECL);
omError_t omCheckBinPageRegions(int level, omError_t report, OM_FLR_DECL);
omBinPageRegion omFindRegionOfAddr(void* addr);
int omIsAddrOnFreeBinPage(void* addr);

/***********************************************************************
 *
 * Some Handy Macros
 *
 **********************************************************************/
#define omCheckReturn(cond) \
  do {omError_t _status = cond; if (_status) return _status;} while (0)
#define omCheckReturnError(cond, error) \
  do {if (cond) return omReportError(error, report, OM_FLR_VAL, "");} while (0)
#define omCheckReturnCorrupted(cond) \
  omCheckReturnError(cond, omError_MemoryCorrupted)
#define omAddrCheckReturn(cond)                                               \
do                                                                            \
{                                                                             \
  omError_t _status = cond;                                                   \
  if (_status && (_status != omError_MaxError))                               \
  {                                                                           \
    _omPrintAddrInfo(stderr, _status, addr, bin_size, flags, 10, "  occured for"); \
    return _status;                                                           \
  }                                                                           \
} while (0)
#define omAddrCheckReturnError(cond, error) \
  do {if (cond) return omReportAddrError(error, report, addr, bin_size, flags, OM_FLR_VAL, "");} while (0)
#define omAddrCheckReturnCorrupted(cond) \
  omAddrCheckReturnError(cond, omError_MemoryCorrupted)

#else
#define omFreeKeptAddrFromBin(bin) ((void)0)
#endif /* ! OM_NDEBUG */
/*ENDPRIVATE*/

#endif /* OM_DEBUG_H */
