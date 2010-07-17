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
#include <kernel/structs.h>

extern int nrzExp;
extern NATNUMBER nrzModul;

number  nrzCopy        (number a);
number  cfrzCopy       (number a, const ring r);
int     nrzSize        (number a);
void    nrzDelete      (number *a, const ring r);
BOOLEAN nrzGreaterZero (number k);
number  nrzMult        (number a, number b);
number  nrzInit        (int i, const ring r);
int     nrzInt         (number &n, const ring r);
number  nrzAdd         (number a, number b);
number  nrzSub         (number a, number b);
void    nrzPower       (number a, int i, number * result);
BOOLEAN nrzIsZero      (number a);
BOOLEAN nrzIsOne       (number a);
BOOLEAN nrzIsMOne      (number a);
BOOLEAN nrzIsUnit      (number a);
number  nrzGetUnit     (number a);
number  nrzDiv         (number a, number b);
number  nrzIntDiv      (number a, number b);
number  nrzIntMod      (number a, number b);
number  nrzNeg         (number c);
number  nrzInvers      (number c);
BOOLEAN nrzGreater     (number a, number b);
BOOLEAN nrzDivBy       (number a, number b);
int     nrzDivComp     (number a, number b);
BOOLEAN nrzEqual       (number a, number b);
number  nrzLcm         (number a,number b, ring r);
number  nrzGcd         (number a,number b,ring r);
number  nrzExtGcd      (number a, number b, number *s, number *t);
nMapFunc nrzSetMap     (const ring src, const ring dst);
void    nrzWrite       (number &a, const ring r);
const char *  nrzRead  (const char *s, number *a);
char *  nrzName        (number n);
#ifdef LDEBUG
BOOLEAN nrzDBTest      (number a, const char *f, const int l);
#endif
void    nrzSetExp(int c, ring r);
void    nrzInitExp(int c, ring r);
void    nrzDelete(number *a, const ring r);

number nrzMapQ(number from);
#endif
#endif
