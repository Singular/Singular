/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: timing.h,v 1.1 1996-12-06 14:49:07 schmidt Exp $ */

/*
$Log: not supported by cvs2svn $
 * Revision 1.0  1996/05/17  10:59:37  stobbe
 * Initial revision
 *
*/

/* It should be possible to include this file multiple times for different */
/* settings of NDEBUG */

#undef TIMING_START
#undef TIMING_END
#undef TIMING_DEFINE_PRINT
#undef TIMING_DEFINE_PRINTPROTO
#undef TIMING_PRINT

#ifdef TIMING
#include <time.h>
#include <sys/times.h>
#include <iostream.h>
#define TIMING_START(t) { struct tms timing_ ## t ## _start, timing_ ## t ## _end; \
  times( &timing_ ## t ## _start );
#define TIMING_END(t) times( &timing_ ## t ## _end ); \
  timing_ ## t ## _time += timing_ ## t ## _end.tms_utime - timing_ ## t ## _start.tms_utime; }
#define TIMING_DEFINE_PRINT(t) long timing_ ## t ## _time; \
void timing_print_ ## t ( char * msg ) { cerr.setf( ios::fixed, ios::floatfield ); cerr.precision(2); \
  cerr << msg << float(timing_ ## t ## _time) / CLK_TCK << " sec" << endl; }
#define TIMING_PRINT(t,msg) timing_print_ ## t ( msg );
#define TIMING_DEFINE_PRINTPROTO(t) void timing_print_ ## t ( char * );
#else
#define TIMING_START(t)
#define TIMING_END(t)
#define TIMING_DEFINE_PRINT(t)
#define TIMING_PRINT(t,msg)
#define TIMING_DEFINE_PRINTPROTO(t)
#endif /* TIMING */
