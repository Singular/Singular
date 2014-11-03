#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - interupt and signal handling
*/
#include <setjmp.h>
#include <misc/auxiliary.h>
#include <kernel/structs.h>

extern jmp_buf si_start_jmpbuf;
extern short si_restart;
extern int siRandomStart;
extern BOOLEAN siCntrlc;
void init_signals();

extern BOOLEAN singular_in_batchmode;
/* TRUE for child of a fork or started with --batch*/

/* for deferring the call of m2_end() in SIGTERM handler if necessary */
extern volatile BOOLEAN do_shutdown;
extern volatile int defer_shutdown;

#endif
