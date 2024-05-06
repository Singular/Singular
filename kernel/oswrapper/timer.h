#ifndef TIMER_H
#define TIMER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT - get the computing time
*/

EXTERN_VAR int timerv;
int startTimer(void);
void writeTime(const char* s);

long getTimer();

EXTERN_VAR int rtimerv;
void startRTimer(void);
void writeRTime(const char* s);

void initRTimer();
int  getRTimer();

void SetTimerResolution(int res);
void SetMinDisplayTime(double mtime);

#endif

