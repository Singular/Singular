#ifndef KHSTD_H
#define KHSTD_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: khstd.h,v 1.1.1.1 2003-10-06 12:15:56 Singular Exp $ */
/*
* ABSTRACT:hilbert driven std
*/

#include "structs.h"

void khCheck(ideal Q, intvec *w, intvec *hilb, int &eledeg, int &count,
             kStrategy strat);
                        /* ideal S=strat->Shdl, poly p=strat->P.p */

#endif
