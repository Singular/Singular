#ifndef SPOLYS0_H
#define SPOLYS0_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
*/
#include "polys.h"
#include "binom.h"

#define spMemcpy(A,B)      memcpy4(A,B,pMonomSize)
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
#define spMonAdd(a,m)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
        pGetOrder(a) += pGetOrder(m);}
//        (a)->order += (m)->order;}
#define spMonSub(a,b)  \
        {for(int ii =pVariables; ii; ii--) (a)->exp[ii] -= (b)->exp[ii];\
        pGetOrder(a) -= pGetOrder(b);}
//        (a)->order -= (b)->order;}
#endif

void spModuleToPoly(poly a1);

/* reduction */
poly spGSpolyRed(poly p1, poly p2,poly spNoether);
void spGSpolyTail(poly p1, poly q, poly q2, poly spNoether);
poly spGSpolyRedNew(poly p1, poly p2,poly spNoether);

/* s-polynomial */
poly spGSpolyCreate(poly p1, poly p2,poly spNoether);

/* short s-polynomial for ordering in Bba and Mora */
void spShort1(poly b, poly a, poly m);
void spShort2(poly b, poly a, poly m);
poly spGSpolyShortBba(poly p1, poly p2);
//poly spGSpolyShortMora(poly p1, poly p2, int *ecart);

#endif

