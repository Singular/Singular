#ifndef SPOLYS_H
#define SPOLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: s-polynomials
*/
/* $Id: spolys.h,v 1.8 1999-09-29 10:59:40 obachman Exp $ */
#include "structs.h"

/* reduction */
extern poly (*spSpolyRed)(poly p1, poly p2,poly spNoether,
                          spSpolyLoopProc spSpolyLoop);
extern void (*spSpolyTail)(poly p1, poly q, poly q2, poly spNoether,
                           spSpolyLoopProc spSpolyLoop);
extern poly (*spSpolyRedNew)(poly p1, poly p2,poly spNoether,
                             spSpolyLoopProc spSpolyLoop);

/* s-polynomial */
extern poly (*spSpolyCreate)(poly p1, poly p2,poly spNoether,
                             spSpolyLoopProc spSpolyLoop);

/* short s-polynomial for ordering in Bba and Mora */
extern poly (*spSpolyShortBba)(poly p1, poly p2);

/* init */
void spSet(ring r);
int spCheckCoeff(number *a, number *b);
#endif

