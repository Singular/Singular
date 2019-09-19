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
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
  #ifdef HAVE_MALLOC_H
  #include <malloc.h>
  #elif defined(HAVE_MALLOC_MALLOC_H)
  #include <malloc/malloc.h>
  #endif
#endif
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

static inline void * omalloc(size_t s)
{ if (s!=0)
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
   { return malloc(s); }
#else
  {long *d=(long*)malloc(s+sizeof(long)); *d=s;d++;return d; }
#endif
  else return NULL;
}
static inline void * omAlloc(size_t s)
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
{ return malloc(s); }
#else
{ long *d=(long*)malloc(s+sizeof(long)); *d=s;d++;return d; }
#endif
static inline void * omAlloc0(size_t s)
{ void *d=omAlloc(s);memset(d,0,s); return d; }
static inline void * omalloc0(size_t s)
{ if (s!=0) { void *d=omAlloc(s);memset(d,0,s); return d;} else return NULL; }

static inline void *omRealloc(void *d, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
  return realloc(d,ns);
#else
  {
    long *dd=(long*)d; dd--; dd=(long*)realloc(dd,ns+sizeof(long));
    *dd=ns+sizeof(long);dd++; return dd;
  }
#endif
}
#define omReallocAligned(A,B) omRealloc(A,B)
static inline void *omReallocSize(void *d, __attribute__((unused)) size_t os, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
  return realloc(d,ns);
#else
  {
    long *dd=(long*)d; dd--; dd=(long*)realloc(dd,ns+sizeof(long));
    *dd=ns+sizeof(long);dd++; return dd;
  }
#endif
}
static inline long omSizeOfAddr(void *d)
#ifdef HAVE_MALLOC_USABLE_SIZE
{ return malloc_usable_size(d); }
#elif defined(HAVE_AMLLOC_SIZE)
{ return malloc_size(d); }
#else
{ long *dd=(long*)d; dd--; return *dd;}
#endif

static inline void omFree(void *d)
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
{ free(d); }
#else
{ if (d!=NULL) { long *dd=(long*)d; dd--; free(dd);}}
#endif

static inline void *omRealloc0(void *d, size_t ns)
{
#ifdef HAVE_MALLOC_USABLE_SIZE
  size_t os=0;
  if (d!=NULL) os=malloc_usable_size(d);
  if (os>=ns)
  {
    void *n=realloc(d,ns);
    return n;
  }
  else
  {
    char *n=(char*)realloc(d,ns);
    memset(n+(ns-os),0,ns-os);
    return (void*)n;
  }
#elif defined(HAVE_MALLOC_SIZE)
  size_t os=0;
  if (d!=NULL) os=malloc_size(d);
  if (os>=ns)
  {
    void *n=realloc(d,ns);
    return n;
  }
  else
  {
    char *n=(char*)realloc(d,ns);
    memset(n+(ns-os),0,ns-os);
    return (void*)n;
  }
#else
  void *n=omAlloc0(ns);
  if (d!=NULL)
  {
    size_t c;
    size_t os=omSizeOfAddr(d);
    if (ns>os) c=os; else c=ns;
    memcpy(n,d,c);
    omFree(d);
  }
  return n;
#endif
}
static inline void omFreeSize(void *d, __attribute__((unused)) size_t s)
#if defined(HAVE_MALLOC_USABLE_SIZE) || defined(HAVE_MALLOC_SIZE)
{ free(d); }
#else
{ if (d!=NULL) { long *dd=(long*)d; dd--; free(dd);}}
#endif

static inline char * omStrDup(const char *s)
{ size_t l=strlen(s);char *ns=(char *)omAlloc(l+1);
  return strcpy(ns,s);
}
static inline void * omMemDup(void * s)
#ifdef HAVE_MALLOC_USABLE_SIZE
{ size_t l=malloc_usable_size(s);
  void *n=malloc(l);
  memcpy(n,s,l);
  return n;
}
#elif defined(HAVE_MALLOC_SIZE)
{ size_t l=malloc_size(s);
  void *n=malloc(l);
  memcpy(n,s,l);
  return n;
}
#else
{ long *n;long *d=(long*)s; d--;
  n=(long*)malloc(*d+sizeof(long));
  memcpy(n,d,(*d)+sizeof(long));
  n++;
  return n;
}
#endif

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
#define omfree(P)                omFree(P)
#define omFreeBin(P,B)           omFree(P)
#define omfreeSize(P,S)          omFreeSize(P,S)
#define omFreeFunc               omFree
#define omFreeBinAddr(P)         omFree(P)
#define omrealloc(A,NS)          omRealloc(A,NS)
#define omreallocSize(A,OS,NS)   omRealloc(A,NS)
#define omRealloc0Size(A,OS,NS)  omRealloc0(A,NS)
#define omrealloc0Size(A,OS,NS)  omRealloc(A,NS)
#define omMarkAsStaticAddr(A)
#define omMemCpyW(A,B,S)         memcpy(A,B,(S)<<2)
#define omMemcpyW(A,B,S)         memcpy(A,B,(S)<<2)
#define omGetSpecBin(A)          (A)
#define omUnGetSpecBin(A)        do {} while (0)
#define memcpyW(A,B,C)         memcpy(A,B,(C)*sizeof(long))
#define omGetStickyBinOfBin(B) omGetSpecBin(B)


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
