#ifndef XMEMORY_H
#define XMEMORY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmemory.h,v 1.9 1998/06/02 15:29:58 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
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

static inline void * omAlloc(size_t s)
{ if (s!=0) {long *d=(long*)malloc(s+sizeof(long)); *d=s;d++;return d; }
  else return NULL;
}
static inline void * omAlloc0(size_t s)
{ void *d=omAlloc(s);memset(d,0,s); return d; }

static inline void *omRealloc(void *d, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else 
  {
    long *dd=(long*)d; dd--; dd=(long*)realloc(dd,ns+sizeof(long)); 
    *dd=ns+sizeof(long);dd++; return dd;
  }
}
static inline void *omReallocSize(void *d, size_t os, size_t ns)
{ if (d==NULL) return omAlloc(ns);
  else 
  {
    long *dd=(long*)d; dd--; dd=(long*)realloc(dd,ns+sizeof(long)); 
    *dd=ns+sizeof(long);dd++; return dd;
  }
}
static inline long omSizeOfAddr(void *d)
{ long *dd=(long*)d; dd--; return *dd;}

static inline void omFree(void *d)
{ if (d!=NULL) { long *dd=(long*)d; dd--; free(dd);}}

static inline void *omRealloc0(void *d, size_t ns)
{
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
}
static inline void omFreeSize(void *d, size_t s)
{ if (d!=NULL) { long *dd=(long*)d; dd--; free(dd);}}

static inline char * omStrDup(const char *s)
{ size_t l=strlen(s);char *ns=(char *)omAlloc(l+1);
  return strcpy(ns,s);
}
static inline void * omMemDup(void * s)
{ long *n;long *d=(long*)s; d--;
  n=(long*)malloc(*d+sizeof(long));
  memcpy(n,d,(*d)+sizeof(long));
  n++;
  return n;
}

#define omSizeWOfAddr(P)         (omSizeOfAddr(P)/sizeof(long))
#define omTypeAllocBin(T,P,B)    P=(T)omAlloc(B)
#define omTypeAlloc(T,P,S)       P=(T)omAlloc(S)
#define omTypeAlloc0Bin(T,P,B)   P=(T)omAlloc0(B)
#define omalloc(S)               omAlloc(S)
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
#define omFree(P)                omFree(P)
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
#define omUnGetSpecBin(A)        ((void)0)
#define memcpyW(A,B,C)         memcpy(A,B,(C)*sizeof(long))
#define omGetStickyBinOfBin(B) omGetSpecBin(B)


/* debug dummies: */
#define omTypeReallocAlignedSize     omTypeReallocSize
#define omTypeRealloc0AlignedSize    omTypeRealloc0Size
#define omReallocAlignedSize         omReallocSize
#define omRealloc0AlignedSize        omRealloc0Size
#define omMemDupAligned     omMemDup
#define omCheckIf(cond, test)                    ((void) 0)
#define omCheckBinAddr(addr)                     ((void) 0)
#define omCheckAddrBin(addr,bin)                 ((void) 0)
#define omCheckBinAddrSize(addr,size)            ((void) 0)
#define omCheckAddrSize(addr,size)               ((void) 0)
#define omCheckAddr(addr)                        ((void) 0)
#define omcheckAddrSize(addr,size)               ((void) 0)
#define omcheckAddr(addr)                        ((void) 0)
#define omCheckBin(bin)                          ((void) 0)
#define omCheckMemory()                          ((void) 0)
#define omPrintCurrentBackTraceMax(A,B)          ((void) 0)
#define omdebugAddrSize(A,B)                     ((void) 0)
#define omPrintAddrInfo(A,B,C)                   ((void) 0)
#define omIsBinPageAddr(A)                       ((void) 0)
#define omTestBinAddrSize(A,B,C)                 ((void) 0)
#define omInitRet_2_Info(argv0)                  ((void) 0)
#define omMergeStickyBinIntoBin(A,B)             ((void) 0)



char * omFindExec (const char *name, char* executable);

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
