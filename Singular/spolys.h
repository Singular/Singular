#ifndef SPOLYS_H
#define SPOLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: spolys.h,v 1.2 1997-03-24 14:25:53 Singular Exp $ */
#include "structs.h"

/* reduction */
extern poly (*spSpolyRed)(poly p1, poly p2,poly spNoether);
extern void (*spSpolyTail)(poly p1, poly q, poly q2, poly spNoether);
extern poly (*spSpolyRedNew)(poly p1, poly p2,poly spNoether);

/* s-polynomial */
extern poly (*spSpolyCreate)(poly p1, poly p2,poly spNoether);

/* short s-polynomial for ordering in Bba and Mora */
extern poly (*spSpolyShortBba)(poly p1, poly p2);

/* init */
void spSet(ring r);
#endif

