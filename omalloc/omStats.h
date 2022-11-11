/*******************************************************************
 *  File:    omStats.h
 *  Purpose: declaration of statistics related stuff
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_STATS_H
#define OM_STATS_H

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
  long InternalUsedBytesMalloc;
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

/* returns a copy of omallinfo struct */
extern struct omInfo_s omGetInfo(void);
/* the struct itself which is always up-to-date */
/* use read-only */
extern struct omInfo_s om_Info;
/* update the global info struct */
extern void omUpdateInfo(void);
/* initialize such that sbrk can be measured */
extern void omInitInfo(void);
extern void omPrintStats(FILE* fd);
extern void omPrintInfo(FILE* fd);

/*BEGINPRIVATE*/
/* used internally to keep track of sbrk */
extern unsigned long om_SbrkInit;
/*ENDPRIVATE*/
#endif /* OM_STATS_H */
