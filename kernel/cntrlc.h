#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: cntrlc.h,v 1.1.1.1 2003-10-06 12:15:50 Singular Exp $ */
/*
* ABSTRACT - interupt and signal handling
*/
#include <setjmp.h>
#include "structs.h"

extern jmp_buf si_start_jmpbuf;
extern short si_restart;
extern int siRandomStart;
extern BOOLEAN siCntrlc;
void init_signals();

#endif
