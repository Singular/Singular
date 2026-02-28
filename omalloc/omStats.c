/*******************************************************************
 *  File:    omStats.c
 *  Purpose: definitions of stats related stuff
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include <unistd.h>
#include "omConfig.h"

#ifdef HAVE_OMALLOC
#include "omDefaultConfig.h"
#include "omMalloc.h"
#include "omalloc.h"

omInfo_t om_Info = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void omUpdateInfo()
{
#ifdef OM_MALLOC_UPDATE_INFO
  OM_MALLOC_UPDATE_INFO;
#endif

  /* this can happen, since sizes are added as requested, and
     subtracted as the real size of the memory */
  if (om_Info.CurrentBytesFromMalloc < 0)
    om_Info.CurrentBytesFromMalloc = 0;

  om_Info.UsedBytesFromValloc = omGetUsedBinBytes();
  om_Info.AvailBytesFromValloc = om_Info.CurrentBytesFromValloc - om_Info.UsedBytesFromValloc;

#ifdef OM_MALLOC_USED_BYTES
  om_Info.UsedBytesMalloc = OM_MALLOC_USED_BYTES;
#else
  om_Info.UsedBytesMalloc = om_Info.CurrentBytesFromMalloc
                            -om_Info.InternalUsedBytesMalloc;
#endif
#ifdef OM_MALLOC_AVAIL_BYTES
  om_Info.AvailBytesMalloc = OM_MALLOC_AVAIL_BYTES;
#endif

  om_Info.UsedBytes = om_Info.UsedBytesMalloc + om_Info.UsedBytesFromValloc;
  om_Info.AvailBytes = om_Info.AvailBytesMalloc + om_Info.AvailBytesFromValloc;

#ifdef OM_HAVE_VALLOC_MMAP
  om_Info.CurrentBytesMmap = om_Info.CurrentBytesFromValloc;
  om_Info.MaxBytesMmap = om_Info.MaxBytesFromValloc;
#endif
#ifdef OM_MALLOC_CURRENT_BYTES_MMAP
  om_Info.CurrentBytesMmap += OM_MALLOC_CURRENT_BYTES_MMAP;
#endif
#ifdef OM_MALLOC_MAX_BYTES_MMAP
  om_Info.MaxBytesMmap += OM_MALLOC_MAX_BYTES_MMAP;
#endif

  /* CurrentBytesAlloc / MaxBytesAlloc: total bytes currently held by
     omalloc from the OS (malloc + valloc).  Replaces the old sbrk(0)-based
     counters which were inaccurate on macOS (always 0) and on Linux
     (missed mmap'd allocations). */
  om_Info.CurrentBytesAlloc = om_Info.CurrentBytesFromMalloc
                             + om_Info.CurrentBytesFromValloc;
  if (om_Info.CurrentBytesAlloc > om_Info.MaxBytesAlloc)
    om_Info.MaxBytesAlloc = om_Info.CurrentBytesAlloc;

  /* CurrentBytesSystem: the larger of what we actually hold from the OS
     (CurrentBytesAlloc) and what the application is using (UsedBytesMalloc
     + FromValloc).  The old code double-counted FromValloc when
     OM_HAVE_VALLOC_MMAP was defined; this version fixes that. */
  om_Info.CurrentBytesSystem =
    (om_Info.CurrentBytesAlloc > om_Info.UsedBytesMalloc + om_Info.CurrentBytesFromValloc ?
     om_Info.CurrentBytesAlloc : om_Info.UsedBytesMalloc + om_Info.CurrentBytesFromValloc);

  /* MaxBytesSystem: sum of individual peak values is an upper bound since
     FromMalloc and FromValloc may not peak simultaneously.  MaxBytesAlloc
     tracks max(FromMalloc + FromValloc) which is <= sum of individual
     maxes, so the RHS always wins. */
  om_Info.MaxBytesSystem = om_Info.MaxBytesFromMalloc + om_Info.MaxBytesFromValloc;
}

omInfo_t omGetInfo()
{
  omUpdateInfo();
  return om_Info;
}

void omPrintStats(FILE* fd)
{
  omUpdateInfo();
  fprintf(fd, "System %ldk:%ldk Appl %ldk/%ldk Malloc %ldk/%ldk Valloc %ldk/%ldk Pages %ld/%ld Regions %ld:%ld Internal: %ld\n",
          om_Info.CurrentBytesSystem/1024, om_Info.MaxBytesSystem/1024,
          om_Info.UsedBytes/1024, om_Info.AvailBytes/1024,
          om_Info.UsedBytesMalloc/1024, om_Info.AvailBytesMalloc/1024,
          om_Info.CurrentBytesFromValloc/1024, om_Info.AvailBytesFromValloc/1024,
          om_Info.UsedPages, om_Info.AvailPages,
          om_Info.CurrentRegionsAlloc, om_Info.MaxRegionsAlloc,
	  om_Info.InternalUsedBytesMalloc);
}


void omPrintInfo(FILE* fd)
{
  omUpdateInfo();
  fputs("                  Current:       Max:\n",fd);
  fprintf(fd, "BytesSystem:     %8ldk  %8ldk\n", om_Info.CurrentBytesSystem/1024, om_Info.MaxBytesSystem/1024);
  fprintf(fd, "BytesAlloc:      %8ldk  %8ldk\n", om_Info.CurrentBytesAlloc/1024, om_Info.MaxBytesAlloc/1024);
  fprintf(fd, "BytesMmap:       %8ldk  %8ldk\n", om_Info.CurrentBytesMmap/1024, om_Info.MaxBytesMmap/1024);
  fprintf(fd, "BytesFromMalloc: %8ldk  %8ldk\n", om_Info.CurrentBytesFromMalloc/1024, om_Info.MaxBytesFromMalloc/1024);
  fprintf(fd, "BytesFromValloc: %8ldk  %8ldk\n", om_Info.CurrentBytesFromValloc/1024, om_Info.MaxBytesFromValloc/1024);
  fprintf(fd, "PagesAlloc:      %8ld   %8ld \n", om_Info.UsedPages, om_Info.MaxPages);
  fprintf(fd, "RegionsAlloc:    %8ld   %8ld \n", om_Info.CurrentRegionsAlloc, om_Info.MaxRegionsAlloc);
  fputs("                     Used:     Avail:\n",fd);
  fprintf(fd, "BytesAppl:       %8ldk  %8ldk\n", om_Info.UsedBytes/1024, om_Info.AvailBytes/1024);
  fprintf(fd, "BytesMalloc:     %8ldk  %8ldk\n", om_Info.UsedBytesMalloc/1024, om_Info.AvailBytesMalloc/1024);
  fprintf(fd, "BytesValloc:     %8ldk  %8ldk\n", om_Info.UsedBytesFromValloc/1024, om_Info.AvailBytesFromValloc/1024);
  fprintf(fd, "Pages:           %8ld   %8ld\n", om_Info.UsedPages, om_Info.AvailPages);
}
#else
#include "omalloc/xalloc.h"
int om_sing_opt_show_mem; /* dummy */
struct omInfo_s om_Info; /* dummy */
struct omOpts_s om_Opts; /* dummy */
#endif
