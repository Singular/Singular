/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Id: timer.cc,v 1.2 1997-03-24 14:26:05 Singular Exp $";
/*
*  ABSTRACT - get the computing time
*/

//the mpw timer is quite the same as the dos timer:
#ifdef macintosh
#define MSDOS
#endif

int        timerv = 0;

#ifndef MSDOS

/*tested on HP9000/700, HP9000/300, Linux 0.99, SUN Sparc*/
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <float.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/times.h>
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

#include "mod2.h"
#include "timer.h"
#include "febase.h"
#define HALF_SEC 30
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
  siStartTime = t_rec.tms_utime;
  return (int)time(NULL);
}

void startTimer()
{
  times(&t_rec);
  startl = t_rec.tms_utime;
}

/*2
* returns the time since a fixed point in seconds
*/
int getTimer()
{
  clock_t curr;

  times(&t_rec);
  curr = t_rec.tms_utime - siStartTime;

#ifdef TIME_SEC
  double f =  ((double)curr)/ (double)HZ;
  return (int)(f+0.5);
#else
  return (int)curr;
#endif
}

/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > 0.5 sec
*/
void writeTime(void* v)
{
  clock_t curr;

  times(&t_rec);
  curr = t_rec.tms_utime - startl;
  if (curr < (clock_t)HALF_SEC) return;

  double f =  ((double)curr)/ (double)HZ;
  Print("//%s %.2f sec\n" ,v ,f);
}
#else

/*tested on MSDOS-GCC, Macintosh*/
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "mod2.h"
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
#ifdef TIME_SEC
  double f =  ((double)curr)/ (double)CLOCKS_PER_SEC;
  return (int)(f+0.5);
#else
  return (int)curr;
#endif
}
/*2
* stops timer, writes string s and the time since last call of startTimer
* if this time is > 0.5 sec
*/
void writeTime(void* v)
{
  clock_t curr = clock() - startl;
  if (curr < (clock_t)CLOCKS_PER_SEC) return;

  double f =  ((double)curr) / CLOCKS_PER_SEC;
  Print("//%s %.1f sec\n" ,v ,f);
}
#endif
