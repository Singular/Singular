#ifndef RMODULON_H
#define RMODULON_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo n
*/
#ifdef HAVE_RINGS
#include "coeffs.h"

#ifndef NATNUMBER
#define NATNUMBER unsigned long
#endif

number  cfrnCopy       (number a, const ring r);
int     nrnSize        (number a, const ring r);
void    nrnDelete      (number *a, const ring r);
BOOLEAN nrnGreaterZero (number k, const ring r);
number  nrnMult        (number a, number b, const ring r);
number  nrnInit        (int i, const ring r);
int     nrnInt         (number &n, const ring r);
number  nrnAdd         (number a, number b, const ring r);
number  nrnSub         (number a, number b, const ring r);
void    nrnPower       (number a, int i, number * result, const ring r);
BOOLEAN nrnIsZero      (number a, const ring r);
BOOLEAN nrnIsOne       (number a, const ring r);
BOOLEAN nrnIsMOne      (number a, const ring r);
BOOLEAN nrnIsUnit      (number a, const ring r);
number  nrnGetUnit     (number a, const ring r);
number  nrnDiv         (number a, number b, const ring r);
number  nrnMod         (number a,number b, const ring r);
number  nrnIntDiv      (number a,number b, const ring r);
number  nrnNeg         (number c, const ring r);
number  nrnInvers      (number c, const ring r);
BOOLEAN nrnGreater     (number a, number b, const ring r);
BOOLEAN nrnDivBy       (number a, number b, const ring r);
int     nrnDivComp     (number a, number b, const ring r);
BOOLEAN nrnEqual       (number a, number b, const ring r);
number  nrnLcm         (number a,number b, const ring r);
number  nrnGcd         (number a,number b, const ring r);
number  nrnExtGcd      (number a, number b, number *s, number *t, const ring r);
nMapFunc nrnSetMap     (const ring src, const ring dst);
#define  nrnWrite      nrzWrite
const char *  nrnRead  (const char *s, number *a, const ring r);
char *  nrnName        (number n, const ring r);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, const char *f, const int l, const ring r);
#endif
void    nrnSetExp(int c, const ring r);
void    nrnInitExp(int c, const ring r);

#endif
#endif
