#ifndef SPOLYS0_H
#define SPOLYS0_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: spolys0.h,v 1.11 1999-09-27 14:43:44 obachman Exp $ */
/*
* ABSTRACT: s-polynomials, internal header
*/
#include "polys.h"
#include "binom.h"
#include "polys-impl.h"
#include "spSpolyLoop.h"


#define spMemcpy(A,B)      pCopy2(A,B)


#define spMonAdd(a,m)  pMonAddFast(a,m)
#define spMonSub(a,m)  pMonSubFast(a,m)

void spModuleToPoly(poly a1);

/* reduction */
poly spGSpolyRed(poly p1, poly p2,poly spNoether,
                 spSpolyLoopProc spSpolyLoop);
void spGSpolyTail(poly p1, poly q, poly q2, poly spNoether,
                  spSpolyLoopProc spSpolyLoop);
poly spGSpolyRedNew(poly p1, poly p2,poly spNoether,
                    spSpolyLoopProc spSpolyLoop);

/* s-polynomial */
poly spGSpolyCreate(poly p1, poly p2,poly spNoether,
                    spSpolyLoopProc spSpolyLoop);

/* short s-polynomial for ordering in Bba and Mora */
poly spGSpolyShortBba(poly p1, poly p2);
//poly spGSpolyShortMora(poly p1, poly p2, int *ecart);

extern void spMultCopyX(poly p, poly m, poly n, number exp, poly spNoether);
extern void spGMultCopyX(poly p, poly m, poly n, number exp, poly spNoether);

#endif

