/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Timer.cc
 * Purpose:    definitions for a simple timer
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *
 ***************************************************************/

#include"mpsr_Timer.h"
#include<stdio.h>

void mpsr_StartTimer(mpsr_Timer_pt t_mpsr)
{
  tms t_tms;

  t_mpsr->t_time = times(&t_tms);
  t_mpsr->s_time = t_tms.tms_stime;
  t_mpsr->u_time = t_tms.tms_utime;
}

void mpsr_StopTimer(mpsr_Timer_pt t_mpsr)
{
  tms t_tms;

  t_mpsr->t_time = times(&t_tms) - t_mpsr->t_time;
  t_mpsr->s_time = t_tms.tms_stime - t_mpsr->s_time;
  t_mpsr->u_time = t_tms.tms_utime - t_mpsr->u_time;
}

void mpsr_PrintTimer(mpsr_Timer_pt t_mpsr, char *str)
{
  printf("%s", str);
  printf("User   time: %.2f \n", (float) t_mpsr->u_time / (float) HZ);
  printf("System time: %.2f \n", (float) t_mpsr->u_time / (float) HZ);
  printf("Real   time: %.2f \n", (float) t_mpsr->u_time / (float) HZ);
}
