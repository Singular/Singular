#ifndef GMPFLOAT_H
#define GMPFLOAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpfl.h,v 1.9 2001-01-09 15:40:07 Singular Exp $ */
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
#endif
void     ngfDelete(number *a, ring r);

nMapFunc  ngfSetMap(ring src, ring dst);

void setGMPFloatDigits( size_t digits, size_t rest );
#endif
