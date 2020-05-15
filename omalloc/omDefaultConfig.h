/*******************************************************************
 *  File:    omDefaultConfig.h
 *  Purpose: default declaration of of configurable stuff
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

/********************************************************************
  If you want to make changes to any of these defines, create a file,
  say, MyOmConfig.h, define your particular stuff there, and run configure with
  --external-config-h=MyOmConfig.h.

  If you also need to implement something, then implement it in, say,
  MyOmConfig.c, and run configure with
  --external-config-h=MyOmConfig.h --external-config-c=MyOmConfig.c

  For omTest to link, you need to make sure that your implementation can also be
  used when not linked with your application. I.e. you should also provide a
  stand-alone implementation if OM_STANDALONE is defined.

  Notice that some of these parameters can also be set at run-time, using
  the global om_Opts struct.
********************************************************************/
#include <limits.h>
#include "omConfig.h"
/* if external config was provided, 'make' makes links from it to omExternalConfig.h */
#ifdef OM_HAVE_EXTERNAL_CONFIG_H
#include "omExternalConfig.h"
#endif

/* If this is larger than the track parameter given to the omDebug routines,
   then this is used as TrackLevel: om_Opts.MinTrack is initalized with this */
#ifndef OM_DEFAULT_MIN_TRACK
#define OM_DEFAULT_MIN_TRACK 0
#endif

/* If this is larger than the check parameter given to the omDebug routines,
   then this is used as CheckLevel: om_Opts.MinCheck is initalized with this */
#ifndef OM_DEFAULT_MIN_CHECK
#define OM_DEFAULT_MIN_CHECK 0
#endif

/* MAX options. If Max < Min, then Max value is used. */
/* If this is smaller than the track parameter given to the omDebug routines,
   then this is used as TrackLevel: om_Opts.MaxTrack is initalized with this */
#ifndef OM_DEFAULT_MAX_TRACK
#define OM_DEFAULT_MAX_TRACK 5
#endif

/* If this is smaller than the check parameter given to the omDebug routines,
   then this is used as CheckLevel: om_Opts.MaxCheck is initalized with this */
#ifndef OM_DEFAULT_MAX_CHECK
#define OM_DEFAULT_MAX_CHECK 10
#endif

/* If this is greater than 0, then the omDebugFree omDebugRealloc delay freeing memory
   by that many blocks: Initalizes omOpts.Keep
   Setting this to LONG_MAX never frees memory */
#ifndef OM_DEFAULT_KEEP
#define OM_DEFAULT_KEEP 100
#endif

/* If this is set to
   0: errors are not reported, only the global variable om_ErrorStatus is set
   1: short error description, i.e. omError2String(om_ErrorStatus), is reported to stderr
   2: backtrace of current stack is printed to stderr
   3: more detailed error description is printed -- this might not make too much sense if
      you are not familiar with omalloc
   Initializes om_Opts.HowToReprotErrors
*/
#ifndef OM_DEFAULT_HOW_TO_REPORT_ERRORS
#if defined(OM_INTERNAL_DEBUG)
#define OM_DEFAULT_HOW_TO_REPORT_ERRORS 3
#else
#define OM_DEFAULT_HOW_TO_REPORT_ERRORS 2
#endif
#endif

/* if this is set, then all memory allocated with track >= 1 is marked as
   static, i.e. it is not mention in omPrintUsedAddrs */
#ifndef OM_DEFAULT_MARK_AS_STATIC
#define OM_DEFAULT_MARK_AS_STATIC 0
#endif

/* Number of pages per region, i.e., how many pages are allocated at once, after we
   run out of pages: Initalizes om_Opts.PagesPerRegion
   The higher this value is, the fewer calls to valloc (resp. mmap) need to be make,
   but the more unused memory the application might have allocated from the operating system
*/
#ifndef OM_DEFAULT_PAGES_PER_REGION
#define OM_DEFAULT_PAGES_PER_REGION 512
#endif

/* This is called if nothing goes any more, i.e., if
   memory request can not be serviced. If set, this function should never return.*/
#ifndef OM_DEFAULT_OUT_OF_MEMORY_FUNC
/* This initializes om_Opts.OutOfMemoryFunc which is declared as
   void (*OutOfMemoryFunc)(); */
#define OM_DEFAULT_OUT_OF_MEMORY_FUNC NULL
#endif
#ifndef OM_OUT_OF_MEMORY_HOOK
#define OM_OUT_OF_MEMORY_HOOK()                             \
do                                                          \
{                                                           \
  if (om_Opts.OutOfMemoryFunc != NULL)                      \
    om_Opts.OutOfMemoryFunc();                              \
   fprintf(stderr, "***Emergency Exit: Out of Memory\n");   \
   exit(1);                                                 \
}                                                           \
while (0)
#endif

/* This is called whenever no more memory could be obtained from the system.
   It should trigger the release of as much memory by the application as possible */
#ifndef OM_DEFAULT_MEMORY_LOW_FUNC
/* This initializes om_Opts.MemoryLowFunc which is declared as
   void (*MemoryLowFunc)(); */
#define OM_DEFAULT_MEMORY_LOW_FUNC NULL
#endif
#ifndef OM_DEFAULT_MEMORY_LOW_HOOK
#define OM_MEMORY_LOW_HOOK()                    \
do                                              \
{                                               \
  if (om_Opts.MemoryLowFunc != NULL)            \
    om_Opts.MemoryLowFunc();                    \
}                                               \
while(0)
#endif

/* This is called after an omError was reported.
   It is especially useful to set a debugger breakpoint
   to this func */
#ifndef OM_DEFAULT_ERROR_HOOK
#define OM_DEFAULT_ERROR_HOOK omErrorBreak
#endif

/*******************************************************************
 *  File:    omSingularConfig.h
 *  Purpose: declaration of External Config stuff for omalloc
 *           This file is part of omDefaultConfig.h, i.e., at the the time
 *           the omalloc library is built. Any changes to the default config
 *           of omalloc should be done here (and, of course, you need to
 *           rebuilt the library).
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>


#ifdef OM_ALLOC_SYSTEM_C
int om_sing_opt_show_mem = 0;
size_t om_sing_last_reported_size = 0;
#else
extern int om_sing_opt_show_mem;
extern size_t om_sing_last_reported_size;
#endif

/* number of bytes for difference to report: every 1 MByte */
#define SING_REPORT_THRESHOLD 1000*1024
#define OM_SINGULAR_HOOK                                                        \
do                                                                            \
{                                                                             \
  if (om_sing_opt_show_mem)                                                   \
  {                                                                           \
    size_t _current_bytes = om_Info.CurrentBytesFromMalloc +                  \
                            (om_Info.UsedPages << LOG_BIT_SIZEOF_SYSTEM_PAGE);\
    size_t _diff = (_current_bytes > om_sing_last_reported_size ?             \
                   _current_bytes - om_sing_last_reported_size :              \
                   om_sing_last_reported_size - _current_bytes);              \
    if (_diff >= SING_REPORT_THRESHOLD)                                       \
    {                                                                         \
      fprintf(stdout, "[%ldk]", (long)(_current_bytes + 1023)/1024);                \
      fflush(stdout);                                                         \
      om_sing_last_reported_size = _current_bytes;                            \
    }                                                                         \
  }                                                                           \
}                                                                             \
while (0)

#ifdef __cplusplus
}
#endif

/********************************************************************
 *
 * The following can NOT be set at run time
 *
 ********************************************************************/
/* The following hooks are called after the respective
   system routine was called, and the Stats struct was updated
   Not settable at run-time (makes no sense for these to be functions, for they would
   be called each time the underlying malloc/valloc is called !) */
#ifndef OM_REALLOC_HOOK
#define OM_REALLOC_HOOK(oldsize, newsize) do {} while (0)
#endif
#ifndef OM_VALLOC_HOOK
#define OM_VALLOC_HOOK(size) do {} while (0)
#endif
#ifndef OM_VFREE_HOOK
#define OM_VFREE_HOOK(size) do {} while (0)
#endif
#define OM_MALLOC_HOOK(size)                OM_SINGULAR_HOOK
#define OM_FREE_HOOK(size)                  OM_SINGULAR_HOOK
#define OM_ALLOC_BINPAGE_HOOK               OM_SINGULAR_HOOK
#define OM_FREE_BINPAGE_HOOK                OM_SINGULAR_HOOK

/*
 * Some stuff related to tracking of addresses
 */

/* minimal number of WORDS for padding before addr: needs to > 0: only relevant for track >= 3 */
#ifndef OM_MIN_SIZEWOF_FRONT_PATTERN
#define OM_MIN_SIZEWOF_FRONT_PATTERN 1
#endif

/* minimal number of WORDS for padding before addr: needs to > 0: only relevant for track >= 3 */
#ifndef OM_MIN_SIZEWOF_BACK_PATTERN
#define OM_MIN_SIZEWOF_BACK_PATTERN 1
#endif

/* maximal number of stack frames kept for stack at the allocation time of addr (track >= 2)
   and free time of addr (track >= 5) */
#ifndef OM_MAX_KEPT_FRAMES
#define OM_MAX_KEPT_FRAMES 10
#endif

/* pattern with which memory is initialized, for front and back padding,
   and for free memory: only relevant if track >= 3*/
#ifndef OM_INIT_PATTERN
#define OM_INIT_PATTERN    0xfe
#endif
#ifndef OM_FRONT_PATTERN
#define OM_FRONT_PATTERN   0xfd
#endif
#ifndef OM_BACK_PATTERN
#define OM_BACK_PATTERN    0xfc
#endif
#ifndef OM_FREE_PATTERN
#define OM_FREE_PATTERN    0xfb
#endif
