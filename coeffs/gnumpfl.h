#ifndef GMPFLOAT_H
#define GMPFLOAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include <kernel/structs.h>

BOOLEAN  ngfGreaterZero(number za);
BOOLEAN  ngfGreater(number a, number b);
BOOLEAN  ngfEqual(number a, number b);
BOOLEAN  ngfIsOne(number a);
BOOLEAN  ngfIsMOne(number a);
BOOLEAN  ngfIsZero(number za);
number   ngfInit(int i, const ring r);
int      ngfInt(number &n, const ring r);
number   ngfNeg(number za);
number   ngfInvers(number a);
number   ngfAdd(number la, number li);
number   ngfSub(number la, number li);
number   ngfMult(number a, number b);
int      ngfSize(number n);
number   ngfDiv(number a, number b);
void     ngfPower(number x, int exp, number *lu);
number   ngfCopy(number a);
number   ngf_Copy(number a, ring r);
const char *   ngfRead (const char *s, number *a);
void     ngfWrite(number &a, const ring r);

#ifdef LDEBUG
BOOLEAN  ngfDBTest(number a, const char *f, const int l);
#endif
void     ngfDelete(number *a, const ring r);

nMapFunc  ngfSetMap(const ring src, const ring dst);

void setGMPFloatDigits( size_t digits, size_t rest );
number ngfMapQ(number from);
#endif
