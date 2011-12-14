////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
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
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#define ISTREAM std::istream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#define ISTREAM istream
#endif
#define TIMING_START(t) { struct tms timing_ ## t ## _start, timing_ ## t ## _end; \
  times( &timing_ ## t ## _start );
#define TIMING_END(t) times( &timing_ ## t ## _end ); \
  timing_ ## t ## _time += timing_ ## t ## _end.tms_utime - timing_ ## t ## _start.tms_utime; }
#define TIMING_DEFINE_PRINT(t) long timing_ ## t ## _time; \
void timing_print_ ## t ( char * msg ) { CERR.setf( ios::fixed, ios::floatfield); CERR.precision(2); \
  CERR << msg << float(timing_ ## t ## _time) / 60 << " sec" << "\n"; }
#define TIMING_PRINT(t,msg) timing_print_ ## t ( msg );
#define TIMING_DEFINE_PRINTPROTO(t) void timing_print_ ## t ( char * );
#else
#define TIMING_START(t)
#define TIMING_END(t)
#define TIMING_DEFINE_PRINT(t)
#define TIMING_PRINT(t,msg)
#define TIMING_DEFINE_PRINTPROTO(t)
#endif /* TIMING */

