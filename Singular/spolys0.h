#ifndef SPOLYS0_H
#define SPOLYS0_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: spolys0.h,v 1.4 1997-12-03 16:59:05 obachman Exp $ */
/*
* ABSTRACT: s-polynomials, internal header
*/
#include "polys.h"
#include "binom.h"
#include "polys-impl.h"

#define spMemcpy(A,B)      pCopy2(A,B)

#ifdef TEST_MAC_ORDER
#define spMonAdd0(a,m)  \
        {for(int ii=pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];}
#define spMonAdd(a,m)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
        pSetm(a);}
#define spMonSub(a,b)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] -= (b)->exp[ii];\
        pSetm(a);}
#else
#ifdef COMP_FAST
#define spMonAdd(a,m)  \
        {for(int ii=pVarLowIndex; ii<=pVarHighIndex; ii++) (a)->exp[ii] += (m)->exp[ii];\
        pGetOrder(a) += pGetOrder(m);}
#define spMonSub(a,b)  \
        {for(int ii=pVarLowIndex; ii<=pVarHighIndex; ii++) (a)->exp[ii] -= (b)->exp[ii];\
        pGetOrder(a) -= pGetOrder(b);}

#else // ! COMP_FAST

#define spMonAdd(a,m)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
        pGetOrder(a) += pGetOrder(m);}
//        (a)->order += (m)->order;}
#define spMonSub(a,b)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] -= (b)->exp[ii];\
        pGetOrder(a) -= pGetOrder(b);}
//        (a)->order -= (b)->order;}
#endif // COMP_FAST
#endif

void spModuleToPoly(poly a1);

/* reduction */
poly spGSpolyRed(poly p1, poly p2,poly spNoether,
                 spSpolyLoopProc spSpolyLoop);
void spGSpolyTail(poly p1, poly q, poly q2, poly spNoether,
                  spSpolyLoopProc spSpolyLoop);
poly spGSpolyRedNew(poly p1, poly p2,poly spNoether,
                    spSpolyLoopProc spSpolyLoop);

/* s-polynomial */
poly spGSpolyCreate(poly p1, poly p2,poly spNoether);

/* short s-polynomial for ordering in Bba and Mora */
void spShort1(poly b, poly a, poly m);
void spShort2(poly b, poly a, poly m);
poly spGSpolyShortBba(poly p1, poly p2);
//poly spGSpolyShortMora(poly p1, poly p2, int *ecart);
extern void spMultCopyX(poly p, poly m, poly n, number exp, poly spNoether);

#endif

