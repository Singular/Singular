#ifndef KHSTD_H
#define KHSTD_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:hilbert driven std
*/

#include <kernel/structs.h>

void khCheck(ideal Q, intvec *w, intvec *hilb, int &eledeg, int &count,
             kStrategy strat);
                        /* ideal S=strat->Shdl, poly p=strat->P.p */

void khCheckLocInhom(ideal Q, intvec *w, intvec *hilb, int &count,
             kStrategy strat);
#endif
