#ifndef BINOM_H
#define BINOM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.h,v 1.5 1998-01-14 14:42:17 obachman Exp $ */
/*
* ABSTRACT - set order (=number of monomial) for dp
*/
// #define TEST_MAC_ORDER
#ifdef TEST_MAC_ORDER
extern int bHighdeg;
extern BOOLEAN bNoAdd;
void       bBinomSet(int * orders);
#endif
#endif
