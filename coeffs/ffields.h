#ifndef FFIELDS_H
#define FFIELDS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: finite fields with a none-prime number of elements (via tables)
*/
#include "coeffs.h"


BOOLEAN nfGreaterZero (number k, const coeffs r);
number  nfMult        (number a, number b, const coeffs r);
number  nfInit        (int i, const coeffs r);
number  nfPar         (int i, const coeffs r);
int     nfParDeg      (number n, const coeffs r);
int     nfInt         (number &n, const coeffs r);
number  nfAdd         (number a, number b, const coeffs r);
number  nfSub         (number a, number b, const coeffs r);
void    nfPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nfIsZero      (number a, const coeffs r);
BOOLEAN nfIsOne       (number a, const coeffs r);
BOOLEAN nfIsMOne      (number a, const coeffs r);
number  nfDiv         (number a, number b, const coeffs r);
number  nfNeg         (number c, const coeffs r);
number  nfInvers      (number c, const coeffs r);
BOOLEAN nfGreater     (number a, number b, const coeffs r);
BOOLEAN nfEqual       (number a, number b, const coeffs r);
void    nfWrite       (number &a, const coeffs r);
const char *  nfRead  (const char *s, number *a, const coeffs r);
#ifdef LDEBUG
BOOLEAN nfDBTest      (number a, const char *f, const int l);
#endif
void    nfSetChar     (int c, char ** param);

nMapFunc nfSetMap     (const coeffs src, const coeffs dst);
char *  nfName        (number n, const coeffs r);
extern  int nfMinPoly[];
extern  int nfCharP;  /* the characteristic: p*/
extern  int nfCharQ;  /* p^n = number of el. = repr. of 0 */
void    nfShowMipo();
#endif

