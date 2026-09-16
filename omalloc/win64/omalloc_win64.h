/*******************************************************************
 * Native Win64 backend for the public omalloc interface.
 *
 * Keep Windows implementation details here.  Unix builds continue to
 * include the existing omalloc headers directly from omalloc/omalloc.h.
 *******************************************************************/
#ifndef OMALLOC_WIN64_H
#define OMALLOC_WIN64_H

#if !defined(_WIN32)
#error "the native Win64 omalloc backend is Windows-only"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef REGISTER
# if defined(__cplusplus) && (__cplusplus >= 201402L || defined(__clang__))
#  define REGISTER
# else
#  define REGISTER register
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct omBin_s
{
  size_t sizeW;
  void* win_lock;
  void* free_list;
  size_t cached;
  size_t cache_limit;
  unsigned long flags;
  unsigned long sticky;
};
typedef struct omBin_s omBin_t;
typedef omBin_t* omBin;

struct omInfo_s
{
  long MaxBytesSystem;
  long CurrentBytesSystem;
  long MaxBytesSbrk;
  long CurrentBytesSbrk;
  long MaxBytesMmap;
  long CurrentBytesMmap;
  long UsedBytes;
  long AvailBytes;
  long UsedBytesMalloc;
  long InternalUsedBytesMalloc;
  long AvailBytesMalloc;
  long MaxBytesFromMalloc;
  long CurrentBytesFromMalloc;
  long MaxBytesFromValloc;
  long CurrentBytesFromValloc;
  long UsedBytesFromValloc;
  long AvailBytesFromValloc;
  long MaxPages;
  long UsedPages;
  long AvailPages;
  long MaxRegionsAlloc;
  long CurrentRegionsAlloc;
};
typedef struct omInfo_s omInfo_t;

struct omOpts_s
{
  int MinTrack;
  int MinCheck;
  int MaxTrack;
  int MaxCheck;
  int Keep;
  int HowToReportErrors;
  int MarkAsStatic;
  unsigned int PagesPerRegion;
  void (*OutOfMemoryFunc)(void);
  void (*MemoryLowFunc)(void);
  void (*ErrorHook)(void);
};
typedef struct omOpts_s omOpts_t;

enum omError_e
{
  omError_NoError = 0,
  omError_Unknown,
  omError_InternalBug,
  omError_MemoryCorrupted,
  omError_NullAddr,
  omError_InvalidRangeAddr,
  omError_FalseAddr,
  omError_FalseAddrOrMemoryCorrupted,
  omError_WrongSize,
  omError_FreedAddr,
  omError_FreedAddrOrMemoryCorrupted,
  omError_WrongBin,
  omError_UnknownBin,
  omError_NotBinAddr,
  omError_UnalignedAddr,
  omError_NullSizeAlloc,
  omError_ListCycleError,
  omError_SortedListError,
  omError_KeptAddrListCorrupted,
  omError_FreePattern,
  omError_BackPattern,
  omError_FrontPattern,
  omError_NotString,
  omError_StickyBin,
  omError_MaxError
};
typedef enum omError_e omError_t;

/* These guards are used by Singular/extra.cc to expose allocator commands. */
#define OM_STATS_H 1
#define OM_OPTS_H 1
#define OM_ERROR_H 1

extern struct omInfo_s om_Info;
extern struct omOpts_s om_Opts;
extern omError_t om_ErrorStatus;
extern omError_t om_InternalErrorStatus;
extern int om_sing_opt_show_mem;

void* omWinAlloc(size_t size, int zero);
void* omWinRealloc(void* addr, size_t old_size, size_t new_size, int zero);
void omWinFree(void* addr);
void omWinFreeSize(void* addr, size_t size);
void* omWinAllocBin(omBin bin, int zero);
void omWinFreeBin(void* addr, omBin bin);
void* omWinReallocBin(void* addr, omBin old_bin, omBin new_bin, int zero);

omBin _omGetSpecBin(size_t size, int align, int track);
void _omUnGetSpecBin(omBin* bin, int force);
long omGetUsedBinBytes(void);
omBin omGetStickyBinOfBin(omBin bin);
void omMergeStickyBinIntoBin(omBin sticky_bin, omBin into_bin);
unsigned long omGetNewStickyBinTag(omBin bin);
void omSetStickyBinTag(omBin bin, unsigned long sticky);
void omUnSetStickyBinTag(omBin bin, unsigned long sticky);
void omDeleteStickyBinTag(omBin bin, unsigned long sticky);
unsigned long omGetNewStickyAllBinTag(void);
void omSetStickyAllBinTag(unsigned long sticky);
void omUnSetStickyAllBinTag(unsigned long sticky);
void omDeleteStickyAllBinTag(unsigned long sticky);

size_t omSizeOfAddr(const void* addr);
size_t omSizeWOfAddr(const void* addr);
char* omWinStrDup(const char* addr);
void* omWinMemDup(const void* addr);

struct omInfo_s omGetInfo(void);
void omUpdateInfo(void);
void omInitInfo(void);
void omPrintStats(FILE* fd);
void omPrintInfo(FILE* fd);
void omPrintBinStats(FILE* fd);

const char* omError2String(omError_t error);
const char* omError2Serror(omError_t error);
void omErrorBreak(void);
void omPrintAddrInfo(FILE* fd, void* addr, const char* text);
omError_t omWinTestAddr(const void* addr);
omError_t omWinTestAddrSize(const void* addr, size_t size);
omError_t omWinTestAddrBin(const void* addr, omBin bin);
omError_t omTestBinAddrSize(void* addr, size_t size, int check_level);
omError_t omTestMemory(int check_level);

void* omCallocFunc(size_t nmemb, size_t size);
void* omMallocFunc(size_t size);
void omFreeFunc(void* addr);
void* omVallocFunc(size_t size);
void* omReallocFunc(void* old_addr, size_t new_size);
char* omStrdupFunc(const char* addr);
void* omReallocSizeFunc(void* old_addr, size_t old_size, size_t new_size);
void omFreeSizeFunc(void* addr, size_t size);

#define omGetAlignedSpecBin(size) _omGetSpecBin((size), 1, 0)
#define omGetSpecBin(size) _omGetSpecBin((size), 0, 0)
#define omUnGetSpecBin(bin_ptr) _omUnGetSpecBin((bin_ptr), 0)
#define omDeleteSpecBin(bin_ptr) _omUnGetSpecBin((bin_ptr), 1)
#define omSizeWOfBin(bin) ((bin)->sizeW)

#define omTypeAllocBin(type, addr, bin) ((addr) = (type)omWinAllocBin((bin), 0))
#define omTypeAlloc0Bin(type, addr, bin) ((addr) = (type)omWinAllocBin((bin), 1))
#define omAllocBin(bin) omWinAllocBin((bin), 0)
#define omAlloc0Bin(bin) omWinAllocBin((bin), 1)

#define omTypeAlloc(type, addr, size) ((addr) = (type)omWinAlloc((size), 0))
#define omTypeAlloc0(type, addr, size) ((addr) = (type)omWinAlloc((size), 1))
#define omAlloc(size) omWinAlloc((size), 0)
#define omAlloc0(size) omWinAlloc((size), 1)
#define omalloc(size) omWinAlloc((size), 0)
#define omalloc0(size) omWinAlloc((size), 1)

#define omTypeReallocBin(old_addr, old_bin, type, addr, bin) \
  ((addr) = (type)omWinReallocBin((old_addr), (old_bin), (bin), 0))
#define omTypeRealloc0Bin(old_addr, old_bin, type, addr, bin) \
  ((addr) = (type)omWinReallocBin((old_addr), (old_bin), (bin), 1))
#define omReallocBin(old_addr, old_bin, bin) \
  omWinReallocBin((old_addr), (old_bin), (bin), 0)
#define omRealloc0Bin(old_addr, old_bin, bin) \
  omWinReallocBin((old_addr), (old_bin), (bin), 1)

#define omTypeReallocSize(old_addr, old_size, type, addr, size) \
  ((addr) = (type)omWinRealloc((old_addr), (old_size), (size), 0))
#define omTypeRealloc0Size(old_addr, old_size, type, addr, size) \
  ((addr) = (type)omWinRealloc((old_addr), (old_size), (size), 1))
#define omReallocSize(addr, old_size, size) \
  omWinRealloc((addr), (old_size), (size), 0)
#define omRealloc0Size(addr, old_size, size) \
  omWinRealloc((addr), (old_size), (size), 1)
#define omTypeRealloc(old_addr, type, addr, size) \
  ((addr) = (type)omWinRealloc((old_addr), omSizeOfAddr(old_addr), (size), 0))
#define omTypeRealloc0(old_addr, type, addr, size) \
  ((addr) = (type)omWinRealloc((old_addr), omSizeOfAddr(old_addr), (size), 1))
#define omRealloc(addr, size) omWinRealloc((addr), omSizeOfAddr(addr), (size), 0)
#define omRealloc0(addr, size) omWinRealloc((addr), omSizeOfAddr(addr), (size), 1)
#define omreallocSize(addr, old_size, size) omReallocSize((addr), (old_size), (size))
#define omrealloc0Size(addr, old_size, size) omRealloc0Size((addr), (old_size), (size))
#define omrealloc(addr, size) omRealloc((addr), (size))
#define omrealloc0(addr, size) omRealloc0((addr), (size))

#define omFreeBinAddr(addr) omWinFree((addr))
#define omFreeBin(addr, bin) omWinFreeBin((addr), (bin))
#define omFreeSize(addr, size) omWinFreeSize((addr), (size))
#define omfreeSize(addr, size) do { if ((addr) != NULL && (size) != 0) omWinFreeSize((addr), (size)); } while (0)
#define omFree(addr) omWinFree((addr))
#define omfree(addr) do { if ((addr) != NULL) omWinFree((addr)); } while (0)

#define omStrDup(addr) omWinStrDup((addr))
#define omMemDup(addr) omWinMemDup((addr))
#define omMemcpyW(dst, src, words) memcpy((dst), (src), (words) * SIZEOF_LONG)

/* Win64 HeapAlloc satisfies the strict alignment required by Singular. */
#define omTypeAllocAligned omTypeAlloc
#define omTypeAlloc0Aligned omTypeAlloc0
#define omAllocAligned omAlloc
#define omAlloc0Aligned omAlloc0
#define omTypeReallocAlignedSize omTypeReallocSize
#define omTypeRealloc0AlignedSize omTypeRealloc0Size
#define omReallocAlignedSize omReallocSize
#define omRealloc0AlignedSize omRealloc0Size
#define omTypeReallocAligned omTypeRealloc
#define omTypeRealloc0Aligned omTypeRealloc0
#define omReallocAligned omRealloc
#define omRealloc0Aligned omRealloc0
#define omMemDupAligned omMemDup

#ifdef OM_NDEBUG
# define omCheckIf(cond, statement) do {} while (0)
# define omCheckBinAddr(addr) do {} while (0)
# define omCheckAddrBin(addr, bin) do {} while (0)
# define omCheckBinAddrSize(addr, size) do {} while (0)
# define omCheckAddrSize(addr, size) do {} while (0)
# define omCheckAddr(addr) do {} while (0)
# define omcheckAddrSize(addr, size) do {} while (0)
# define omcheckAddr(addr) do {} while (0)
# define omCheckBin(bin) do {} while (0)
# define omCheckMemory() do {} while (0)
#else
# define omCheckIf(cond, statement) do { if (cond) { statement; } } while (0)
# define omCheckBinAddr(addr) ((void)omWinTestAddr((addr)))
# define omCheckAddrBin(addr, bin) ((void)omWinTestAddrBin((addr), (bin)))
# define omCheckBinAddrSize(addr, size) ((void)omWinTestAddrSize((addr), (size)))
# define omCheckAddrSize(addr, size) omWinTestAddrSize((addr), (size))
# define omCheckAddr(addr) ((void)omWinTestAddr((addr)))
# define omcheckAddrSize(addr, size) omWinTestAddrSize((addr), (size))
# define omcheckAddr(addr) omWinTestAddr((addr))
# define omCheckBin(bin) do { (void)(bin); } while (0)
# define omCheckMemory() ((void)omTestMemory(1))
#endif
#define omTestList(ptr, level) (omError_NoError)
#define omIsBinPageAddr(addr) (omWinTestAddr((addr)) == omError_NoError)

#define omInitGetBackTrace() do {} while (0)
#define omInitRet_2_Info(argv0) do { (void)(argv0); } while (0)
#define omPrintCurrentBackTrace(fd) do { (void)(fd); } while (0)
#define omPrintCurrentBackTraceMax(fd, max) do { (void)(fd); (void)(max); } while (0)
#define omPrintUsedAddrs(fd, max) do { (void)(fd); (void)(max); } while (0)
#define omPrintUsedTrackAddrs(fd, max) do { (void)(fd); (void)(max); } while (0)
#define omMarkAsStaticAddr(addr) do { (void)(addr); } while (0)
#define omMarkMemoryAsStatic() do {} while (0)
#define omUnMarkAsStaticAddr(addr) do { (void)(addr); } while (0)
#define omUnMarkMemoryAsStatic() do {} while (0)
#define omSetCustomOfAddr(addr, value) do { (void)(addr); (void)(value); } while (0)
#define omGetCustomOfAddr(addr) ((void)(addr), (void*)0)

#ifdef __cplusplus
}
#endif

#endif /* OMALLOC_WIN64_H */
