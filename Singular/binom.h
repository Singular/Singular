#ifndef BINOM_H
#define BINOM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.h,v 1.12 1999-11-15 17:19:49 obachman Exp $ */
/*
* ABSTRACT - set order (=number of monomial) for dp
*/
// #define TEST_MAC_ORDER
// #define TEST_MAC_DEBUG

#ifdef TEST_MAC_ORDER

#include <limits.h>

extern int      bHighdeg;
extern int      bHighdeg_1;
extern BOOLEAN  bNoAdd;
extern int *    bBinomials;

void            bSetm(poly p);
void            bSetm0(poly p);
void            bBinomSet(int * orders);
int             bComp1dpc(poly p1, poly p2);
int             bComp1cdp(poly p1, poly p2);

#else
#undef TEST_MAC_DEBUG
#endif
#endif
