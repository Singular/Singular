#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: cntrlc.h,v 1.4 1997-04-25 15:03:55 obachman Exp $ */
/*
* ABSTRACT - interupt and signal handling
*/
#include "structs.h"

extern int siRandomStart;
void init_signals();
#ifndef MSDOS
#define test_int_std(A) test_int()
void test_int();
#else
#define test_int()
#define test_int_std(A)
#endif

#endif
