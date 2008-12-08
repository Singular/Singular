#ifndef GMPCOMPLEX_H
#define GMPCOMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpc.h,v 1.3 2008-12-08 17:47:35 Singular Exp $ */
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
number   ngc_Copy(number a, ring r);
const char * ngcRead (const char *s, number *a);
void     ngcWrite(number &a);
number   ngcRePart(number a);
number   ngcImPart(number a);


#ifdef LDEBUG
BOOLEAN  ngcDBTest(number a, const char *f, const int l);
#endif
void     ngcDelete(number *a, const ring r);

nMapFunc  ngcSetMap(ring src, ring dst);

number ngcMapQ(number from);
#endif


// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: ***
