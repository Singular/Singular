#ifndef GMPCOMPLEX_H
#define GMPCOMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpc.h,v 1.7 2000-12-15 18:49:29 Singular Exp $ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include "structs.h"

BOOLEAN  ngcGreaterZero(number za);      // !!! MAY NOT WORK AS EXPECTED !!!
BOOLEAN  ngcGreater(number a, number b);
BOOLEAN  ngcEqual(number a, number b);
BOOLEAN  ngcIsOne(number a);
BOOLEAN  ngcIsMOne(number a);
BOOLEAN  ngcIsZero(number za);
void     ngcNew(number *r);
number   ngcInit(int i);
int      ngcInt(number &n);
number   ngcNeg(number za);
number   ngcInvers(number a);
number   ngcPar(int i);
number   ngcAdd(number la, number li);
number   ngcSub(number la, number li);
number   ngcMult(number a, number b);
number   ngcDiv(number a, number b);
void     ngcPower(number x, int exp, number *lu);
number   ngcCopy(number a);
char *   ngcRead (char *s, number *a);
void     ngcWrite(number &a);
number   ngcRePart(number a);
number   ngcImPart(number a);


#ifdef LDEBUG
BOOLEAN  ngcDBTest(number a, char *f, int l);
void     ngcDBDelete(number *a, char *f, int l);
#define  ngcDelete(A) nlDBDelete(A,__FILE__,__LINE__)
#else
void     ngcDelete(number *a);
#endif

nMapFunc  ngcSetMap(ring src, ring dst);

#endif


// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: *** 
