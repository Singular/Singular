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
#include <coeffs/coeffs.h>
#include <coeffs/rintegers.h>

#ifndef NATNUMBER
#define NATNUMBER unsigned long
#endif

BOOLEAN nrnInitChar    (coeffs r, void*);
number  nrnCopy        (number a, const coeffs r);
int     nrnSize        (number a, const coeffs r);
void    nrnDelete      (number *a, const coeffs r);
BOOLEAN nrnGreaterZero (number k, const coeffs r);
number  nrnMult        (number a, number b, const coeffs r);
number  nrnInit        (int i, const coeffs r);
int     nrnInt         (number &n, const coeffs r);
number  nrnAdd         (number a, number b, const coeffs r);
number  nrnSub         (number a, number b, const coeffs r);
void    nrnPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nrnIsZero      (number a, const coeffs r);
BOOLEAN nrnIsOne       (number a, const coeffs r);
BOOLEAN nrnIsMOne      (number a, const coeffs r);
BOOLEAN nrnIsUnit      (number a, const coeffs r);
number  nrnGetUnit     (number a, const coeffs r);
number  nrnDiv         (number a, number b, const coeffs r);
number  nrnMod         (number a,number b, const coeffs r);
number  nrnIntDiv      (number a,number b, const coeffs r);
number  nrnNeg         (number c, const coeffs r);
number  nrnInvers      (number c, const coeffs r);
BOOLEAN nrnGreater     (number a, number b, const coeffs r);
BOOLEAN nrnDivBy       (number a, number b, const coeffs r);
int     nrnDivComp     (number a, number b, const coeffs r);
BOOLEAN nrnEqual       (number a, number b, const coeffs r);
number  nrnLcm         (number a,number b, const coeffs r);
number  nrnGcd         (number a,number b, const coeffs r);
number  nrnExtGcd      (number a, number b, number *s, number *t, const coeffs r);
nMapFunc nrnSetMap     (const coeffs src, const coeffs dst);
#define  nrnWrite      nrzWrite
const char *  nrnRead  (const char *s, number *a, const coeffs r);
void     nrnCoeffWrite (const coeffs r, BOOLEAN details);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nrnSetExp(int c, const coeffs r);
void    nrnInitExp(int c, const coeffs r);

#endif
#endif
