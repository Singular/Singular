#ifndef GMPFLOAT_H
#define GMPFLOAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpfl.h,v 1.4 1999-09-16 12:33:56 Singular Exp $ */
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

BOOLEAN  ngfSetMap(ring r);

void setGMPFloatDigits( size_t digits );
void setGMPFloatPrecBytes( unsigned long int bytes );
#endif


// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: *** 
