#ifndef FFIELDS_H
#define FFIELDS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ffields.h,v 1.8 1999-11-15 17:19:59 obachman Exp $ */
/*
* ABSTRACT: finite fields with a none-prime number of elements (via tables)
*/
#include "structs.h"


BOOLEAN nfGreaterZero (number k);
number  nfMult        (number a, number b);
number  nfInit        (int i);
number  nfPar         (int i);
int     nfParDeg      (number n);
int     nfInt         (number &n);
number  nfAdd         (number a, number b);
number  nfSub         (number a, number b);
void    nfPower       (number a, int i, number * result);
BOOLEAN nfIsZero      (number a);
BOOLEAN nfIsOne       (number a);
BOOLEAN nfIsMOne      (number a);
number  nfDiv         (number a, number b);
number  nfNeg         (number c);
number  nfInvers      (number c);
BOOLEAN nfGreater     (number a, number b);
BOOLEAN nfEqual       (number a, number b);
void    nfWrite       (number &a);
char *  nfRead        (char *s, number *a);
#ifdef LDEBUG
BOOLEAN nfDBTest      (number a, char *f, int l);
#endif
void    nfSetChar     (int c, char ** param);

BOOLEAN nfSetMap     (ring r);
char *  nfName        (number n);
extern  int nfMinPoly[];
void    nfShowMipo();
#endif

