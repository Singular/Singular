/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// static char * rcsid = "$Id: timing.h,v 1.2 1997-06-09 15:56:12 Singular Exp $";
////////////////////////////////////////////////////////////
// It is possible to include this file multiple times for different 
// settings of TIMING (but now this isn't used)

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
void timing_print_ ## t ( char * msg ) { cout.setf( ios::fixed, ios::floatfield); cout.precision(2); \
  cout << msg << float(timing_ ## t ## _time) / 60 << " sec" << endl; }
#define TIMING_PRINT(t,msg) timing_print_ ## t ( msg );
#define TIMING_DEFINE_PRINTPROTO(t) void timing_print_ ## t ( char * );
#else
#define TIMING_START(t)
#define TIMING_END(t)
#define TIMING_DEFINE_PRINT(t)
#define TIMING_PRINT(t,msg)
#define TIMING_DEFINE_PRINTPROTO(t)
#endif /* TIMING */

/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:13:56  michael
Version for libfac-0.2.1

Revision 1.1  1996/01/17 06:22:05  michael
Initial revision

*/
