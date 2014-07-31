#ifndef RINTEGERS_H
#define RINTEGERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#ifdef HAVE_RINGS
#include <coeffs/coeffs.h>

#if SI_INTEGER_VARIANT == 3
#define SR_HDL(A) ((long)(A))
#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)
#define n_Z_IS_SMALL(A)     (SR_HDL(A) & SR_INT)
#define INT_IS_SMALL(A) ( ((A << 1) >> 1) == A )
#endif

//extern int nrzExp;
//extern NATNUMBER nrzModul;

BOOLEAN nrzInitChar    (coeffs r,  void * parameter);
number  nrzCopy        (number a, const coeffs r);
int     nrzSize        (number a, const coeffs r);
void    nrzDelete      (number *a, const coeffs r);
BOOLEAN nrzGreaterZero (number k, const coeffs r);
number  nrzMult        (number a, number b, const coeffs r);
number  nrzInit        (long i, const coeffs r);
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
number  nrzExactDiv    (number a, number b, const coeffs r);
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
void    nrzCoeffWrite  (const coeffs r, BOOLEAN details);
#ifdef LDEBUG
BOOLEAN nrzDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nrzSetExp(int c, coeffs r);
void    nrzInitExp(int c, coeffs r);
void    nrzDelete(number *a, const coeffs r);
coeffs  nrzQuot1(number c, const coeffs r);

CanonicalForm nrzConvSingNFactoryN(number n, BOOLEAN setChar, const coeffs /*r*/);
number nrzConvFactoryNSingN(const CanonicalForm n, const coeffs r);

number nrzMapQ(number from, const coeffs src, const coeffs dst);
#endif
#endif
