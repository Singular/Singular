#ifndef KHSTD_H
#define KHSTD_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: khstd.h,v 1.4 1999-11-15 17:20:13 obachman Exp $ */
/*
* ABSTRACT:hilbert driven std
*/

#include "structs.h"

void khCheck(ideal Q, intvec *w, intvec *hilb, int &eledeg, int &count,
             kStrategy strat);
                        /* ideal S=strat->Shdl, poly p=strat->P.p */

#endif
