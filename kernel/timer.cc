/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: timer.cc,v 1.3 2007-02-07 10:44:44 Singular Exp $ */

/*
*  ABSTRACT - get the computing time
*/

#include "mod2.h"

//the mpw timer is quite the same as the dos timer:

int        timerv = 0;
static double timer_resolution = TIMER_RESOLUTION;

static double mintime = 0.5;

void SetTimerResolution(int res)
{
  timer_resolution = (double) res;
}

void SetMinDisplayTime(double mtime)
{
  mintime = mtime;
}


#ifndef MSDOS

/*tested on HP9000/700, HP9000/300, Linux 0.99, SUN Sparc*/
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <float.h>
#include <mylimits.h>
#include <sys/types.h>

#ifdef TIME_WITH_SYS_TIME
# include <time.h>
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# endif
#else
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# else
#   include <time.h>
# endif
#endif
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif


// need to be adjusted on your machine: the number of ticks per second: HZ
#ifndef HZ
#include <sys/param.h>
#endif
#if !defined(HZ) && defined(CLOCKS_PER_SEC)
#define HZ CLOCKS_PER_SEC
#endif
#if !defined(HZ) && defined(CLK_TCK)
#define HZ CLK_TCK
#endif
#ifndef HZ
#ifdef sun
#define HZ 60.0
#else
#define HZ 100.0
#endif
#endif

#include "timer.h"
#include "febase.h"
/*3
* the start time of the timer
*/
static clock_t startl;
static clock_t siStartTime;

/*3
* temp structure to get the time
*/
static struct tms t_rec;

/*0 implementation*/

int initTimer()
{
  times(&t_rec);
  siStartTime = t_rec.tms_utime+t_rec.tms_stime;
  return (int)time(NULL);
}

void startTimer()
{
  times(&t_rec);
  startl = t_rec.tms_utime+t_rec.tms_stime;
}

/*2
* returns the time since a fixed point in seconds
*/
int getTimer()
{
  clock_t curr;

  times(&t_rec);
  curr = t_rec.tms_utime+t_rec.tms_stime - siStartTime;

  double f =  ((double)curr) * timer_resolution / (double)HZ;
  return (int)(f+0.5);
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > mintime sec
*/
#ifdef EXTEND_TIMER_D
extern int iiOp;
#endif
void writeTime(char* v)
{
  clock_t curr;

  times(&t_rec);
  curr = t_rec.tms_utime+t_rec.tms_stime - startl;

  double f =  ((double)curr) / (double)HZ;
  if (f > mintime)
  {
#ifdef EXTEND_TIMER_D
    Print("//%s %.2f sec (%d) >>%s<<\n" ,v ,f,iiOp,my_yylinebuf);
#else
    Print("//%s %.2f sec\n" ,v ,f);
#endif
  }
}

#ifdef HAVE_RTIMER
/*0 Real timer implementation*/
int rtimerv = 0;
static struct timeval  startRl;
static struct timeval  siStartRTime;
static struct timezone tzp;
void initRTimer()
{
  gettimeofday(&startRl, &tzp);
  gettimeofday(&siStartRTime, &tzp);
}

void startRTimer()
{
  gettimeofday(&siStartRTime, &tzp);
}

/*2
* returns the time since a fixed point in resolutions
*/
int getRTimer()
{
  struct timeval now;

  gettimeofday(&now, &tzp);

  if (startRl.tv_usec > now.tv_usec)
  {
    now.tv_usec += 1000000;
    now.tv_sec --;
  }

  double f =((double)  (now.tv_sec - startRl.tv_sec))*timer_resolution +
    ((double) (now.tv_usec - startRl.tv_usec))*timer_resolution /
    (double) 1000000;

  return (int)(f+0.5);
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > mintime
*/
void writeRTime(char* v)
{
  struct timeval now;

  gettimeofday(&now, &tzp);

  if (siStartRTime.tv_usec > now.tv_usec)
  {
    now.tv_usec += 1000000;
    now.tv_sec --;
  }

  double f =((double)  (now.tv_sec - siStartRTime.tv_sec)) +
    ((double) (now.tv_usec - siStartRTime.tv_usec)) /
    (double) 1000000;

  if (f > mintime)
   Print("//%s %.2f sec \n" ,v ,f);
}
#endif

#else /* #defined MSDOS */

/*tested on MSDOS-GCC, Macintosh*/
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "timer.h"
#include "febase.h"

/*3
* the start time of the timer
*/
static clock_t startl;
static clock_t siStartTime;

/*2
* starts the timer; used with getTime
*/
int initTimer()
{
  siStartTime = clock();
  return (int)siStartTime;
}

/*2
* starts the timer; used with writeTime
*/
void startTimer()
{
  startl = clock();
}

/*2
* returns the time since a fixed point in seconds
*/
int getTimer()
{
  clock_t curr = clock() - siStartTime;
  double f =  ((double)curr)*timer_resolution/ (double)CLOCKS_PER_SEC;
  return (int)(f+0.5);
}
/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > 0.5 sec
*/
void writeTime(void* v)
{
  clock_t curr = clock() - startl;
  double f =  ((double)curr) / CLOCKS_PER_SEC;
  if (f > mintime)
    Print("//%s %.1f sec\n" ,v ,f);
}
#endif /* MSDOS */
