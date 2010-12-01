#ifndef RINTEGERS_H
#define RINTEGERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo n
*/
#ifdef HAVE_RINGS
#include <coeffs/coeffs.h>

extern int nrzExp;
extern NATNUMBER nrzModul;

BOOLEAN nrzInitChar    (coeffs r,  void * parameter);
number  nrzCopy        (number a, const coeffs r);
int     nrzSize        (number a, const coeffs r);
void    nrzDelete      (number *a, const coeffs r);
BOOLEAN nrzGreaterZero (number k, const coeffs r);
number  nrzMult        (number a, number b, const coeffs r);
number  nrzInit        (int i, const coeffs r);
int     nrzInt         (number &n, const coeffs r);
number  nrzAdd         (number a, number b, const coeffs r);
number  nrzSub         (number a, number b, const coeffs r);
void    nrzPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nrzIsZero      (number a, const coeffs r);
BOOLEAN nrzIsOne       (number a, const coeffs r);
BOOLEAN nrzIsMOne      (number a, const coeffs r);
BOOLEAN nrzIsUnit      (number a, const coeffs r);
number  nrzGetUnit     (number a, const coeffs r);
number  nrzDiv         (number a, number b, const coeffs r);
number  nrzIntDiv      (number a, number b, const coeffs r);
number  nrzIntMod      (number a, number b, const coeffs r);
number  nrzNeg         (number c, const coeffs r);
number  nrzInvers      (number c, const coeffs r);
BOOLEAN nrzGreater     (number a, number b, const coeffs r);
BOOLEAN nrzDivBy       (number a, number b, const coeffs r);
int     nrzDivComp     (number a, number b, const coeffs r);
BOOLEAN nrzEqual       (number a, number b, const coeffs r);
number  nrzLcm         (number a,number b, const coeffs r);
number  nrzGcd         (number a,number b, const coeffs r);
number  nrzExtGcd      (number a, number b, number *s, number *t, const coeffs r);
nMapFunc nrzSetMap     (const coeffs src, const coeffs dst);
void    nrzWrite       (number &a, const coeffs r);
const char *  nrzRead  (const char *s, number *a, const coeffs r);
char *  nrzName        (number n, const coeffs r);
#ifdef LDEBUG
BOOLEAN nrzDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nrzSetExp(int c, coeffs r);
void    nrzInitExp(int c, coeffs r);
void    nrzDelete(number *a, const coeffs r);

number nrzMapQ(number from);
#endif
#endif
