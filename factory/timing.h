/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: timing.h,v 1.8 2008-08-20 13:35:55 Singular Exp $ */

/* It should be possible to include this file multiple times for different */
/* settings of TIMING */

#undef TIMING_START
#undef TIMING_END
#undef TIMING_END_AND_PRINT
#undef TIMING_DEFINE_PRINT
#undef TIMING_DEFINE_PRINTPROTO
#undef TIMING_PRINT

#ifdef TIMING
#include <time.h>
#if ! defined(WINNT) || defined(__GNUC__)
#include <sys/times.h>
#include <sys/param.h>
#endif
#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif

// need to be adjusted on your machine:
// the number of ticks per second: HZ
#if ! defined (HZ) && defined (CLOCKS_PER_SEC)
#define HZ CLOCKS_PER_SEC
#endif
#if ! defined (HZ)  && defined (CLK_TCK)
#define HZ CLK_TCK
#endif
#ifndef HZ
#ifdef sun
#define HZ 60.0
#else
#define HZ 100.0
#endif
#endif

#if defined(WINNT) && ! defined(__GNUC__)

#define TIMING_START(t) { clock_t timing_ ## t ## _start, timing_ ## t ## _end; \
  timing_ ## t ## _start = clock();
#define TIMING_END(t) timing_ ## t ## _end = clock(); \
timing_ ## t ## _time += timing_ ## t ## _end - timing_ ## t ## _start; }
#define TIMING_END_AND_PRINT(t, msg) times( &timing_ ## t ## _end ); \
  fprintf( stderr, "%s%.2f sec\n", msg, \
	   float( timing_ ## t ## _end - timing_ ## t ## _start ) / HZ ); \
  timing_ ## t ## _time += timing_ ## t ## _end - timing_ ## t ## _start; }
#define TIMING_DEFINE_PRINT(t) clock_t timing_ ## t ## _time; \
void timing_print_ ## t ( char * msg ) { \
  fprintf( stderr, "%s%.2f sec\n", msg, float(timing_ ## t ## _time) / HZ ); \
} \
void timing_reset_ ## t () { \
  timing_ ## t ## _time = 0; \
}

#else /* ! WINNT */

#define TIMING_START(t) { struct tms timing_ ## t ## _start, timing_ ## t ## _end; \
  times( &timing_ ## t ## _start );
#define TIMING_END(t) times( &timing_ ## t ## _end ); \
  timing_ ## t ## _time += timing_ ## t ## _end.tms_utime - timing_ ## t ## _start.tms_utime; }
#define TIMING_END_AND_PRINT(t, msg) times( &timing_ ## t ## _end ); \
  fprintf( stderr, "%s%.2f sec\n", msg, \
	   float( timing_ ## t ## _end.tms_utime - timing_ ## t ## _start.tms_utime ) / HZ ); \
  timing_ ## t ## _time += timing_ ## t ## _end.tms_utime - timing_ ## t ## _start.tms_utime; }
#define TIMING_DEFINE_PRINT(t) long timing_ ## t ## _time; \
void timing_print_ ## t ( char * msg ) { \
  fprintf( stderr, "%s%.2f sec\n", msg, float(timing_ ## t ## _time) / HZ ); \
} \
void timing_reset_ ## t () { \
  timing_ ## t ## _time = 0; \
}
#endif /* ! WINNT */

/* macros common to all platforms */
#define TIMING_DEFINE_PRINTPROTO(t) void timing_print_ ## t ( char * ); \
  void timing_reset_ ## t ();
#define TIMING_PRINT(t, msg) timing_print_ ## t ( msg );
#define TIMING_RESET(t) timing_reset_ ## t ();

#else /* TIMING */
#define TIMING_START(t)
#define TIMING_END(t)
#define TIMING_END_AND_PRINT(t, msg)
#define TIMING_DEFINE_PRINT(t)
#define TIMING_DEFINE_PRINTPROTO(t)
#define TIMING_PRINT(t, msg)
#define TIMING_RESET(t)
#endif /* TIMING */
