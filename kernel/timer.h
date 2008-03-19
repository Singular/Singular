#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT - get the computing time
*/
/* $Id: timer.h,v 1.2 2008-03-19 17:44:13 Singular Exp $ */

extern int timerv;
void startTimer(void);
void writeTime(const char* s);

int initTimer();
int  getTimer();

#ifdef HAVE_RTIMER
extern int rtimerv;
void startRTimer(void);
void writeRTime(const char* s);

void initRTimer();
int  getRTimer();
#endif

void SetTimerResolution(int res);
void SetMinDisplayTime(double mtime);

#endif

