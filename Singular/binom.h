#ifndef BINOM_H
#define BINOM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.h,v 1.3 1997-04-09 12:19:38 Singular Exp $ */
/*
* ABSTRACT - set order (=number of monomial) for dp
*/
//#define TEST_MAC_ORDER
#ifdef TEST_MAC_ORDER
extern int bHighdeg;
void       bBinomSet();
#endif
#endif

