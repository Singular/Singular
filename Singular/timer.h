#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: timer.h,v 1.2 1997-03-24 14:26:05 Singular Exp $ */

extern int timerv;
void startTimer(void);
void writeTime(void* s);

int initTimer();
int  getTimer(); /* unit: sec */
#endif

