#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT - get the computing time
*/

THREAD_VAR extern int timerv;
void startTimer(void);
void writeTime(const char* s);

int initTimer();
int  getTimer();

THREAD_VAR extern int rtimerv;
void startRTimer(void);
void writeRTime(const char* s);

void initRTimer();
int  getRTimer();

void SetTimerResolution(int res);
void SetMinDisplayTime(double mtime);

#endif

