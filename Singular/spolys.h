#ifndef SPOLYS_H
#define SPOLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: s-polynomials
*/
/* $Id: spolys.h,v 1.4 1997-12-03 16:59:04 obachman Exp $ */
#include "structs.h"

/* reduction */
extern poly (*spSpolyRed)(poly p1, poly p2,poly spNoether,
                          spSpolyLoopProc spSpolyLoop=NULL);
extern void (*spSpolyTail)(poly p1, poly q, poly q2, poly spNoether,
                           spSpolyLoopProc spSpolyLoop=NULL);
extern poly (*spSpolyRedNew)(poly p1, poly p2,poly spNoether,
                             spSpolyLoopProc spSpolyLoop=NULL);

/* s-polynomial */
extern poly (*spSpolyCreate)(poly p1, poly p2,poly spNoether);

/* short s-polynomial for ordering in Bba and Mora */
extern poly (*spSpolyShortBba)(poly p1, poly p2);

/* init */
void spSet(ring r);
#endif

