#ifndef GMPFLOAT_H
#define GMPFLOAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpfl.h,v 1.1 1999-04-29 11:38:42 Singular Exp $ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include "structs.h"

BOOLEAN  ngfGreaterZero(number za);
BOOLEAN  ngfGreater(number a, number b);
BOOLEAN  ngfEqual(number a, number b);
BOOLEAN  ngfIsOne(number a);
BOOLEAN  ngfIsMOne(number a);
BOOLEAN  ngfIsZero(number za);
void     ngfNew(number *r);
number   ngfInit(int i);
int      ngfInt(number &n);
number   ngfNeg(number za);
number   ngfInvers(number a);
number   ngfAdd(number la, number li);
number   ngfSub(number la, number li);
number   ngfMult(number a, number b);
number   ngfDiv(number a, number b);
void     ngfPower(number x, int exp, number *lu);
number   ngfCopy(number a);
char *   ngfRead (char *s, number *a);
void     ngfWrite(number &a);

#ifdef LDEBUG
BOOLEAN  ngfDBTest(number a, char *f, int l);
void     ngfDBDelete(number *a, char *f, int l);
#define  ngfDelete(A) nlDBDelete(A,__FILE__,__LINE__)
#else
void     ngfDelete(number *a);
#endif

void     ngfNormalize(number &x);
number   ngfGcd(number a, number b);

number   ngfIntMod(number a, number b);
BOOLEAN  ngfSetMap(int c, char ** par, int nop, number minpol);


void setGMPFloatPrecBytes( unsigned long int bytes );
#endif


// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: *** 
