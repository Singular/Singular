#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: cntrlc.h,v 1.3 1997-04-09 12:19:41 Singular Exp $ */
/*
* ABSTRACT - interupt and signal handling
*/
#include "structs.h"

extern int siRandomStart;
extern int sVERSION;
extern int sS_SUBVERSION;
void init_signals();
#ifndef MSDOS
#define test_int_std(A) test_int()
void test_int();
#else
#define test_int()
#define test_int_std(A)
#endif

#endif
