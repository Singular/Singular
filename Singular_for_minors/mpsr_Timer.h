/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Timer.h
 * Purpose:    declarations for a simple timer
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#ifndef __MPSR_TIMER__
#define __MPSR_TIMER__

#include <sys/times.h>

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

typedef struct mpsr_Timer_t
{
  clock_t t_time;
  clock_t s_time;
  clock_t u_time;
} mpsr_Timer_t, *mpsr_Timer_pt;


extern void mpsr_StartTimer(mpsr_Timer_pt t);
extern void mpsr_StopTimer(mpsr_Timer_pt t);
extern void mpsr_PrintTimer(mpsr_Timer_pt t, char *str);


#endif
