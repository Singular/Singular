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
/* use of Bins: define for optimal performancei(6%), undef for valgrind */
#define XALLOC_BIN 1

/* performancce of xalloc+XALLOC_BIN: +32.6 %, xalloc w/o XALLOC_BIN: +40.7 %
 * (omalloc=100 %) */
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
{ void *d=malloc(s);memset(d,0,s); return d; }
static inline void * omalloc0(size_t s)
{ if (s!=0) { void *d=malloc(s);memset(d,0,s); return d;} else return NULL; }

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

#define omStrDup(s) strdup(s)

#ifdef XALLOC_BIN
typedef struct omBin_next_s omBin_next_t;
typedef omBin_next_t*            omBin_next;
struct omBin_next_s
{
  omBin_next         next;
};

struct omBin_s
{
  omBin_next     curr;          /* current freelist */
  size_t        size;          /* size in bytes */
};
typedef struct omBin_s      omBin_t;
typedef omBin_t*            omBin;
#define omSizeWOfBin(bin_ptr) (((bin_ptr->size)+SIZEOF_LONG-1)/SIZEOF_LONG)
static inline void* omAllocBin(omBin b)
{
  if (b->curr!=NULL)
  {
    omBin_next p=b->curr;
    b->curr=p->next;
    return p;
  }
  else return omAlloc(b->size);
}
static inline void* omAlloc0Bin(omBin b)
{
  if (b->curr!=NULL)
  {
    omBin_next p=b->curr;
    b->curr=p->next;
    memset(p,0,b->size);
    return p;
  }
  else return omAlloc0(b->size);
}
static inline void omFreeBin(void *p, omBin b)
{
  *((void**) p) = b->curr;
  b->curr=(omBin_next)p;
}

#define omTypeAllocBin(T,P,B)    P=(T)omAllocBin(B)
#define omTypeAlloc0Bin(T,P,B)   P=(T)omAlloc0Bin(B)
static inline omBin omGetSpecBin(size_t s)
{
  omBin b=(omBin)omAlloc(sizeof(*b));
  b->size=s;
  b->curr=NULL;
  return b;
}
static inline void omUnGetSpecBin(omBin *A)
{
  omBin_next p=(*A)->curr;
  omBin_next pp;
  while(p!=NULL)
  {
    pp=p->next;
    omFree(p);
    p=pp;
  }
  (*A)->curr=NULL;
  omFree(*A);
}

#else

typedef size_t            omBin;
#define omSizeWOfBin(bin_ptr) (((bin_ptr)+SIZEOF_LONG-1)/SIZEOF_LONG)
#define omTypeAllocBin(T,P,B)    P=(T)omAlloc(B)
#define omTypeAlloc0Bin(T,P,B)   P=(T)omAlloc0(B)
#define omAllocBin(B)            omAlloc(B)
#define omAlloc0Bin(B)           omAlloc0(B)
#define omFreeBin(P,B)           omFree(P)
#define omGetSpecBin(A)          (A)
#define omUnGetSpecBin(A)        do {} while (0)
#endif

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

#define omSizeWOfAddr(P)         (omSizeOfAddr(P)/SIZEOF_LONG)

#define omTypeAlloc(T,P,S)       P=(T)omAlloc(S)
#define omAlloc0Aligned(S)       omAlloc0(S)
#define omAllocAligned(S)        omAlloc(S)
#define omInitInfo()
#define omInitGetBackTrace()
#define omUpdateInfo()
#define omPrintStats(F)
#define omPrintInfo(F)
#define omPrintBinStats(F)
#define omMarkMemoryAsStatic()
#define omfreeSize(P,S)          free(P)
#define omFreeFunc               free
#define omFreeBinAddr(P)         free(P)
#define omrealloc(A,NS)          realloc(A,NS)
#define omreallocSize(A,OS,NS)   realloc(A,NS)
#define omrealloc0Size(A,OS,NS)  omRealloc0Size(A,OS,NS)
#define omRealloc(A,B)           realloc(A,B)
#define omReallocAligned(A,B)    realloc(A,B)
#define omReallocSize(A,B,C)     realloc(A,C)
#define omReallocAlignedSize(A,B) realloc(A,B)
#define omMarkAsStaticAddr(A)
#define omMemcpyW(A,B,C)         memcpy(A,B,(C)*SIZEOF_LONG)
#define omGetStickyBinOfBin(B)   (B)


/* debug dummies: */
#define omTypeReallocAlignedSize     omTypeReallocSize
#define omTypeRealloc0AlignedSize    omTypeRealloc0Size
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
#define omMallocFunc malloc
#define omFreeSizeFunc omFreeSize
#define omReallocSizeFunc realloc
/* #define OM_NDEBUG */
#undef OM_SING_KEEP

#endif
