#ifndef FFIELDS_H
#define FFIELDS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: finite fields with a none-prime number of elements (via tables)
*/
#include <kernel/structs.h>


BOOLEAN nfGreaterZero (number k);
number  nfMult        (number a, number b);
number  nfInit        (int i, const ring r);
number  nfPar         (int i);
int     nfParDeg      (number n);
int     nfInt         (number &n, const ring r);
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
void    nfWrite       (number &a, const ring r);
const char *  nfRead  (const char *s, number *a);
#ifdef LDEBUG
BOOLEAN nfDBTest      (number a, const char *f, const int l);
#endif
void    nfSetChar     (int c, char ** param);

nMapFunc nfSetMap     (const ring src, const ring dst);
char *  nfName        (number n);
extern  int nfMinPoly[];
extern  int nfCharP;  /* the characteristic: p*/
extern  int nfCharQ;  /* p^n = number of el. = repr. of 0 */
void    nfShowMipo();
#endif

