#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - interupt and signal handling
*/
#include <setjmp.h>
#include "kernel/mod2.h"
#include "kernel/structs.h"

THREAD_VAR extern jmp_buf si_start_jmpbuf;
THREAD_VAR extern short si_restart;
THREAD_VAR extern int siRandomStart;
void init_signals();

THREAD_VAR extern BOOLEAN singular_in_batchmode;
/* TRUE for child of a fork or started with --batch*/

/* for deferring the call of m2_end() in SIGTERM handler if necessary */
THREAD_VAR extern volatile BOOLEAN do_shutdown;
THREAD_VAR extern volatile int defer_shutdown;

#endif
