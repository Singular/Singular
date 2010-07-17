/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
*  ABSTRACT - get the computing time
*/

#include <kernel/mod2.h>
// undef GETRUSAGE to get the original timer code (w.r.t. CLOCK_TICKS)
#define GETRUSAGE
#ifdef GETRUSAGE
       #include <sys/time.h>
       #include <sys/resource.h>
       #include <unistd.h>
#endif

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

#if !defined(HZ) && defined(HAVE_SYSCONF)
#define HZ sysconf(_SC_CLK_TCK)
#endif

#ifndef HZ
  // last resort
  #ifdef sun
  #define HZ 60.0
  #else
  #define HZ 100.0
  #endif
#endif

#include <kernel/timer.h>
#include <kernel/febase.h>
/*3
* the start time of the timer
*/
#ifdef GETRUSAGE
static int64 siStartTime;
static int64 startl;
#else
static clock_t siStartTime;
static clock_t startl;
#endif

/*3
* temp structure to get the time
*/
#ifdef GETRUSAGE
static struct rusage t_rec;
#else
static struct tms t_rec;
#endif
/*0 implementation*/

int initTimer()
{
#ifdef GETRUSAGE
  getrusage(RUSAGE_SELF,&t_rec); 
  siStartTime = (t_rec.ru_utime.tv_sec*1000000+t_rec.ru_utime.tv_usec
               +t_rec.ru_stime.tv_sec*1000000+t_rec.ru_stime.tv_usec
               +5000)/10000; // unit is 1/100 sec
  getrusage(RUSAGE_CHILDREN,&t_rec); 
  siStartTime += (t_rec.ru_utime.tv_sec*1000000+t_rec.ru_utime.tv_usec
               +t_rec.ru_stime.tv_sec*1000000+t_rec.ru_stime.tv_usec
               +5000)/10000; // unit is 1/100 sec
#else
  times(&t_rec);
  siStartTime = t_rec.tms_utime+t_rec.tms_stime;
#endif
  return (int)time(NULL);
}

void startTimer()
{
#ifdef GETRUSAGE
  getrusage(RUSAGE_SELF,&t_rec); 
  startl = ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+t_rec.ru_stime.tv_usec
               +(int64)5000)/(int64)10000; // unit is 1/100 sec
  getrusage(RUSAGE_CHILDREN,&t_rec); 
  startl += ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+t_rec.ru_stime.tv_usec
               +(int64)5000)/(int64)10000; // unit is 1/100 sec
#else
  times(&t_rec);
  startl = t_rec.tms_utime+t_rec.tms_stime;
#endif
}

/*2
* returns the time since a fixed point in seconds
*/
int getTimer()
{
#ifdef GETRUSAGE
  int64 curr;
  getrusage(RUSAGE_SELF,&t_rec); 
  curr = ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
         +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec
         +(int64)5000)/(int64)10000; // unit is 1/100 sec
  getrusage(RUSAGE_CHILDREN,&t_rec); 
  curr += ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
         +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec
         +(int64)5000)/(int64)10000; // unit is 1/100 sec
  curr -= siStartTime;
  double f =  ((double)curr) * timer_resolution / (double)100;
#else
  clock_t curr;

  times(&t_rec);
  curr = t_rec.tms_utime+t_rec.tms_stime - siStartTime;

  double f =  ((double)curr) * timer_resolution / (double)HZ;
#endif
  return (int)(f+0.5);
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > mintime sec
*/
#ifdef EXTEND_TIMER_D
extern int iiOp;
#endif
void writeTime(const char* v)
{

#ifdef GETRUSAGE
  int64 curr;
  getrusage(RUSAGE_SELF,&t_rec); 
  curr = ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec
               +(int64)5000)/(int64)10000; // unit is 1/100 sec
  getrusage(RUSAGE_CHILDREN,&t_rec); 
  curr += ((int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec
               +(int64)5000)/(int64)10000; // unit is 1/100 sec
  curr -= startl;
  double f =  ((double)curr) * timer_resolution / (double)100;
#else
  clock_t curr;
  times(&t_rec);
  curr = t_rec.tms_utime+t_rec.tms_stime - startl;

  double f =  ((double)curr) / (double)HZ;
#endif
  if (f/timer_resolution > mintime)
  {
#ifdef EXTEND_TIMER_D
    Print("//%s %.2f/%d sec (%d) >>%s<<\n" ,v ,f,(int)timer_resolution,iiOp,my_yylinebuf);
#else
    if (timer_resolution==(double)1.0)
      Print("//%s %.2f sec\n" ,v ,f);
    else
      Print("//%s %.2f/%d sec\n" ,v ,f,(int)timer_resolution);
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
void writeRTime(const char* v)
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
