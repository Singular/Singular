#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT - get the computing time
*/
/* $Id: timer.h,v 1.1.1.1 2003-10-06 12:16:04 Singular Exp $ */

extern int timerv;
void startTimer(void);
void writeTime(char* s);

int initTimer();
int  getTimer();

#ifdef HAVE_RTIMER
extern int rtimerv;
void startRTimer(void);
void writeRTime(char* s);

void initRTimer();
int  getRTimer();
#endif

void SetTimerResolution(int res);
void SetMinDisplayTime(double mtime);

#endif

