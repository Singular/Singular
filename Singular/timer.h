#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
*/
extern int timerv;
void startTimer(void);
void writeTime(void* s);

int initTimer();
int  getTimer(); /* unit: sec */
#endif

