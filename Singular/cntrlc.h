#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - interupt and signal handling
*/
#include <setjmp.h>
#include <kernel/structs.h>

extern jmp_buf si_start_jmpbuf;
extern short si_restart;
extern int siRandomStart;
extern BOOLEAN siCntrlc;
void init_signals();

/* TRUE for child of a fork or started with --batch*/
extern BOOLEAN singular_in_batchmode;

/* for deferring the call of m2_end() in SIGTERM handler if necessary */
extern volatile BOOLEAN do_shutdown;
extern volatile int defer_shutdown;

/* If non-zero, do not prompt on SIGINT. Instead, use
 * cntlr_prompt_answer as answer (this could be set to 'a' to skip the
 * prompt and always assume the "abort" answer) . */
// ((char*)(feOptSpec[FE_OPT_CNTRLC].value))[0];

#endif
