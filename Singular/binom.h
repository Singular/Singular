#ifndef BINOM_H
#define BINOM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.h,v 1.6 1998-01-16 08:24:02 Singular Exp $ */
/*
* ABSTRACT - set order (=number of monomial) for dp
*/
// #define TEST_MAC_ORDER
#ifdef TEST_MAC_ORDER
extern int      bHighdeg;
extern BOOLEAN  bNoAdd;
void            bSetm(poly p);
void            bSetm0(poly p);
void            bBinomSet(int * orders);
int             bComp1dpc(poly p1, poly p2);
int             bComp1cdp(poly p1, poly p2);
#endif
#endif
