#ifndef XMEMORY_H
#define XMEMORY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: omalloc simulation
*/
/* debug routines of omalloc are not implemented, but as dummies provided: */
#define OM_NDEBUG 1

#include <stdlib.h>
#include <string.h>
#include "omalloc/omConfig.h"
#ifdef __cplusplus
extern "C" {
  #if __cplusplus >= 201402L
  /* clang 3.7, gcc 5.1 sets 201402L */
  #define REGISTER
  #elif defined(__clang__)
  #define REGISTER
  #else
  #define REGISTER register
  #endif
#else
  #define REGISTER register
#endif

typedef size_t            omBin;

struct omInfo_s;
typedef struct omInfo_s omInfo_t;
struct omInfo_s
{
  long MaxBytesSystem;      /* set in omUpdateInfo(), is more accurate with malloc support   */
  long CurrentBytesSystem;  /* set in omUpdateInfo(), is more accurate with malloc support */
  long MaxBytesSbrk;        /* always up-to-date, not very accurate, needs omInintInfo() */
  long CurrentBytesSbrk;    /* set in omUpdateInfo(), needs omInintInfo() */
  long MaxBytesMmap;        /* set in omUpdateInfo(), not very accurate */
  long CurrentBytesMmap;    /* set in omUpdateInfo(), not very accurate */
  long UsedBytes;           /* set in omUpdateInfo() */
  long AvailBytes;          /* set in omUpdateInfo() */
  long UsedBytesMalloc;     /* set in omUpdateInfo(), needs malloc support */
  long AvailBytesMalloc;    /* set in omUpdateInfo(), needs malloc support */
  long MaxBytesFromMalloc;      /* always kept up-to-date */
  long CurrentBytesFromMalloc;  /* always kept up-to-date */
  long MaxBytesFromValloc;      /* always kept up-to-date */
  long CurrentBytesFromValloc;  /* always kept up-to-date */
  long UsedBytesFromValloc; /* set in omUpdateInfo()  */
  long AvailBytesFromValloc;/* set in omUpdateInfo()  */
  long MaxPages;            /* always kept up-to-date */
  long UsedPages;           /* always kept up-to-date */
  long AvailPages;          /* always kept up-to-date */
  long MaxRegionsAlloc;     /* always kept up-to-date */
  long CurrentRegionsAlloc; /* always kept up-to-date */
};

extern struct omInfo_s om_Info;

struct omOpts_s;
extern struct omOpts_s
{
  int MinTrack;
  int MinCheck;
  int MaxTrack;
  int MaxCheck;
  int Keep;
  int HowToReportErrors;
  int MarkAsStatic;
  unsigned int PagesPerRegion;
  void (*OutOfMemoryFunc)();
  void (*MemoryLowFunc)();
  void (*ErrorHook)();
} om_Opts;

typedef struct omOpts_s omOpts_t;

extern int om_sing_opt_show_mem;

#define omalloc(s) malloc(s)
#define omAlloc(s) malloc(s)

static inline void * omAlloc0(size_t s)
{ void *d=omAlloc(s);memset(d,0,s); return d; }
static inline void * omalloc0(size_t s)
{ if (s!=0) { void *d=omAlloc(s);memset(d,0,s); return d;} else return NULL; }

static inline void *omRealloc(void *d, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else
  return realloc(d,ns);
}
#define omReallocAligned(A,B) omRealloc(A,B)
static inline void *omReallocSize(void *d, __attribute__((unused)) size_t os, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else
  return realloc(d,ns);
}

static inline void *omRealloc0Size(void *d, __attribute__((unused)) size_t os, size_t ns)
{ if (d==NULL)
    return omAlloc0(ns);
  else
  {
    char *p=(char *)realloc(d,ns);
    if (ns>os) memset(p+os,0,ns-os);
    return (void*)p;
  }
}

#define omfree(d) free(d)
#define omFree(d) free(d)
#define omFreeSize(d,s) free(d)

static inline char * omStrDup(const char *s)
{ size_t l=strlen(s);char *ns=(char *)omAlloc(l+1);
  return strcpy(ns,s);
}

/* #define omSizeWOfBin(bin_ptr) ((bin_ptr)->sizeW) */
#define omSizeWOfBin(bin_ptr) (((bin_ptr)+sizeof(long)-1)/sizeof(long))

/*******************************************************************
 *
 *  error codes
 *
 *******************************************************************/
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
// typedef enum omError_e omError_t;

#define omSizeWOfAddr(P)         (omSizeOfAddr(P)/sizeof(long))

#define omTypeAllocBin(T,P,B)    P=(T)omAlloc(B)
#define omTypeAlloc(T,P,S)       P=(T)omAlloc(S)
#define omTypeAlloc0Bin(T,P,B)   P=(T)omAlloc0(B)
#define omAlloc0Aligned(S)       omAlloc0(S)
#define omAllocAligned(S)        omAlloc(S)
#define omAllocBin(B)            omAlloc(B)
#define omAllocBin0(B)           omAlloc0(B)
#define omAlloc0Bin(B)           omAlloc0(B)
#define omInitInfo()
#define omInitGetBackTrace()
#define omUpdateInfo()
#define omPrintStats(F)
#define omPrintInfo(F)
#define omPrintBinStats(F)
#define omMarkMemoryAsStatic()
#define omFreeBin(P,B)           omFree(P)
#define omfreeSize(P,S)          omFreeSize(P,S)
#define omFreeFunc               omFree
#define omFreeBinAddr(P)         omFree(P)
#define omrealloc(A,NS)          omRealloc(A,NS)
#define omreallocSize(A,OS,NS)   omRealloc(A,NS)
#define omrealloc0Size(A,OS,NS)  omRealloc0Size(A,OS,NS)
#define omMarkAsStaticAddr(A)
#define omGetSpecBin(A)          (A)
#define omUnGetSpecBin(A)        do {} while (0)
#define omMemcpyW(A,B,C)         memcpy(A,B,(C)*sizeof(long))
#define omGetStickyBinOfBin(B)   omGetSpecBin(B)


/* debug dummies: */
#define omTypeReallocAlignedSize     omTypeReallocSize
#define omTypeRealloc0AlignedSize    omTypeRealloc0Size
#define omReallocAlignedSize         omReallocSize
#define omRealloc0AlignedSize        omRealloc0Size
#define omMemDupAligned              omMemDup
#define omCheckIf(cond, test)                    do {} while (0)
#define omCheckBinAddr(addr)                     do {} while (0)
#define omCheckAddrBin(addr,bin)                 do {} while (0)
#define omCheckBinAddrSize(addr,size)            do {} while (0)
#define omCheckAddrSize(addr,size)               do {} while (0)
#define omCheckAddr(addr)                        do {} while (0)
#define omcheckAddrSize(addr,size)               do {} while (0)
#define omcheckAddr(addr)                        do {} while (0)
#define omCheckBin(bin)                          do {} while (0)
#define omCheckMemory()                          do {} while (0)
#define omPrintCurrentBackTraceMax(A,B)          do {} while (0)
#define omPrintUsedTrackAddrs(F,max)             do {} while (0)
#define omPrintCurrentBackTrace(F)               do {} while (0)
#define omPrintUsedAddrs(F,max)                  do {} while (0)
#define omdebugAddrSize(A,B)                     do {} while (0)
#define omPrintAddrInfo(A,B,C)                   do {} while (0)
#define omIsBinPageAddr(A)                       (1)
#define omTestBinAddrSize(A,B,C)                 (omError_NoError)
#define omTestList(ptr, level)                   (omError_NoError)
#define omInitRet_2_Info(argv0)                  do {} while (0)
#define omMergeStickyBinIntoBin(A,B)             do {} while (0)


#ifdef __cplusplus
}
#endif

#undef OMALLOC_USES_MALLOC
#define X_OMALLOC
#define omMallocFunc omAlloc
#define omReallocSizeFunc omReallocSize
#define omFreeSizeFunc omFreeSize
/* #define OM_NDEBUG */
#undef OM_SING_KEEP

#endif
