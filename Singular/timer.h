#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: timer.h,v 1.3 1997-03-27 20:26:05 obachman Exp $ */

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

