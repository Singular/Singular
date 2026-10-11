/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
*  ABSTRACT - get the computing time
*/




#include "kernel/mod2.h"

#ifdef _WIN32
# include <chrono>
# include <time.h>
#else
# include <sys/resource.h>
# include <unistd.h>
#endif

VAR int        timerv = 0;
STATIC_VAR double timer_resolution = TIMER_RESOLUTION;

STATIC_VAR double mintime = 0.5;

void SetTimerResolution(int res)
{
  timer_resolution = (double) res;
}

void SetMinDisplayTime(double mtime)
{
  mintime = mtime;
}

#include <stdio.h>

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


#include "reporter/reporter.h"
#include "kernel/oswrapper/timer.h"

/*3
* the start time of the timer
*/
STATIC_VAR int64 siStartTime;

/*3
* temp structure to get the time
*/
#ifndef _WIN32
STATIC_VAR struct rusage t_rec;
#endif
/*0 implementation*/

#ifdef _WIN32
static int64 siWallTime()
{
  using namespace std::chrono;
  return (int64)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

static int64 siCpuTime()
{
  return ((int64)clock() * 1000000) / CLOCKS_PER_SEC;
}
#endif

int startTimer()
{
#ifdef _WIN32
  siStartTime = siCpuTime();
#else
  getrusage(RUSAGE_SELF,&t_rec);
  siStartTime = (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
  getrusage(RUSAGE_CHILDREN,&t_rec);
  siStartTime += (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
               +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
#endif
  return (int)time(NULL);
}

/*2
* returns the time since a fixed point in seconds
*/
long getTimer()
{
#ifdef _WIN32
  int64 curr = siCpuTime();
#else
  int64 curr;
  getrusage(RUSAGE_SELF,&t_rec);
  curr = (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
         +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
  getrusage(RUSAGE_CHILDREN,&t_rec);
  curr += (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
         +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
#endif
  double f =  ((double)curr) * timer_resolution / (double)1000000;
  return (long)(f+0.5);
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > mintime sec
*/
#ifdef EXTEND_TIMER_D
EXTERN_VAR int iiOp;
#endif

void writeTime(const char* v)
{
#ifdef _WIN32
  int64 curr = siCpuTime();
#else
  int64 curr;
  getrusage(RUSAGE_SELF,&t_rec);
  curr = (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
        +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
  getrusage(RUSAGE_CHILDREN,&t_rec);
  curr += (int64)t_rec.ru_utime.tv_sec*1000000+(int64)t_rec.ru_utime.tv_usec
         +(int64)t_rec.ru_stime.tv_sec*1000000+(int64)t_rec.ru_stime.tv_usec;
#endif
  curr -= siStartTime;
  double f =  ((double)curr) * timer_resolution / (double)1000000;
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

/*0 Real timer implementation*/
VAR int rtimerv = 0;
#ifdef _WIN32
STATIC_VAR int64 startRl;
STATIC_VAR int64 siStartRTime;
#else
STATIC_VAR struct timeval  startRl;
STATIC_VAR struct timeval  siStartRTime;
STATIC_VAR struct timezone tzp;
#endif

void startRTimer()
{
#ifdef _WIN32
  siStartRTime = siWallTime();
#else
  gettimeofday(&siStartRTime, &tzp);
#endif
}

void initRTimer()
{
#ifdef _WIN32
  startRl = siWallTime();
  siStartRTime = startRl;
#else
#ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&startRl, &tzp);
  gettimeofday(&siStartRTime, &tzp);
#else
  memset(&startRl,0,sizeof(startRl));
  memset(&siStartRTime,0,sizeof(siStartRTime));
#endif
#endif
}

/*2
* returns the time since a fixed point in resolutions
*/
int getRTimer()
{
#ifdef _WIN32
  double f = ((double)(siWallTime() - startRl)) * timer_resolution / 1000.0;
#else
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
#endif

  return (int)(f+0.5);
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > mintime
*/
void writeRTime(const char* v)
{
#ifdef _WIN32
  double f = ((double)(siWallTime() - siStartRTime)) / 1000.0;
#else
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
#endif

  if (f > mintime)
   Print("//%s %.2f sec \n" ,v ,f);
}
