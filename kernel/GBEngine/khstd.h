#ifndef KHSTD_H
#define KHSTD_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:hilbert driven std
*/

#include "kernel/structs.h"
#include "coeffs/bigintmat.h"

void khCheck(ideal Q, intvec *w, bigintmat *hilb, int &eledeg, int &count,
             kStrategy strat);
void khCheck64(ideal Q, intvec *w, poly hilb, const ring Qt,
             int &eledeg, int &count, kStrategy strat);
                        /* ideal S=strat->Shdl, poly p=strat->P.p */

void khCheckLocInhom(ideal Q, intvec *w, bigintmat *hilb, int &count,
             kStrategy strat);
#endif
