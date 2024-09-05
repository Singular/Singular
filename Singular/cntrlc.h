#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - interrupt and signal handling
*/
#include <setjmp.h>
#include "kernel/mod2.h"
#include "kernel/structs.h"

EXTERN_VAR jmp_buf si_start_jmpbuf;
EXTERN_VAR short si_restart;
EXTERN_VAR int siRandomStart;
void init_signals(void);

EXTERN_VAR BOOLEAN singular_in_batchmode;
/* TRUE for child of a fork or started with --batch*/

/* for deferring the call of m2_end() in SIGTERM handler if necessary */
EXTERN_VAR volatile BOOLEAN do_shutdown;
EXTERN_VAR volatile int defer_shutdown;

void si_set_signals();
void si_reset_signals();
void sig_term_hdl_child(int /*sig*/);

typedef void (*si_hdl_typ)(int);
si_hdl_typ si_set_signal ( int sig, si_hdl_typ signal_handler);
#endif
