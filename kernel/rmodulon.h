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
#include <kernel/structs.h>

#ifndef NATNUMBER
#define NATNUMBER unsigned long
#endif

number  nrnCopy        (number a);
number  cfrnCopy       (number a, const ring r);
int     nrnSize        (number a);
void    nrnDelete      (number *a, const ring r);
BOOLEAN nrnGreaterZero (number k);
number  nrnMult        (number a, number b);
number  nrnInit        (int i, const ring r);
int     nrnInt         (number &n, const ring r);
number  nrnAdd         (number a, number b);
number  nrnSub         (number a, number b);
void    nrnPower       (number a, int i, number * result);
BOOLEAN nrnIsZero      (number a);
BOOLEAN nrnIsOne       (number a);
BOOLEAN nrnIsMOne      (number a);
BOOLEAN nrnIsUnit      (number a);
number  nrnGetUnit     (number a);
number  nrnDiv         (number a, number b);
number  nrnMod         (number a,number b);
number  nrnIntDiv      (number a,number b);
number  nrnNeg         (number c);
number  nrnInvers      (number c);
BOOLEAN nrnGreater     (number a, number b);
BOOLEAN nrnDivBy       (number a, number b);
int     nrnDivComp     (number a, number b);
BOOLEAN nrnEqual       (number a, number b);
number  nrnLcm         (number a,number b, ring r);
number  nrnGcd         (number a,number b,ring r);
number  nrnExtGcd      (number a, number b, number *s, number *t);
nMapFunc nrnSetMap     (const ring src, const ring dst);
#define  nrnWrite      nrzWrite
const char *  nrnRead  (const char *s, number *a);
char *  nrnName        (number n);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, const char *f, const int l);
#endif
void    nrnSetExp(int c, ring r);
void    nrnInitExp(int c, ring r);

number nrnMapQ(number from);
number nrnMapGMP(number from);
#endif
#endif
