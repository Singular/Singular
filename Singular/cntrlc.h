#ifndef CNTRLC_H
#define CNTRLC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: cntrlc.h,v 1.5 1997-06-09 12:17:49 Singular Exp $ */
/*
* ABSTRACT - interupt and signal handling
*/
#include "structs.h"

extern int siRandomStart;
extern BOOLEAN siCntrlc;
void init_signals();

#endif
