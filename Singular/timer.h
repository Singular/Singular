#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT - get the computing time
*/
/* $Id: timer.h,v 1.4 1997-04-09 12:20:17 Singular Exp $ */

extern int timerv;
void startTimer(void);
void writeTime(void* s);

int initTimer();
int  getTimer();

#ifdef HAVE_RTIMER
extern int rtimerv;
void startRTimer(void);
void writeRTime(void* s);

void initRTimer();
int  getRTimer();
#endif

void SetTimerResolution(int res);
void SetMinDisplayTime(double mtime);

#endif

