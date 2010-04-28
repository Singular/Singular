#ifndef GMPCOMPLEX_H
#define GMPCOMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include "coeffs.h"

BOOLEAN  ngcGreaterZero(number za, const coeffs r); // !!! MAY NOT WORK AS EXPECTED !!!
BOOLEAN  ngcGreater(number a, number b, const coeffs r);
BOOLEAN  ngcEqual(number a, number b, const coeffs r);
BOOLEAN  ngcIsOne(number a, const coeffs r);
BOOLEAN  ngcIsMOne(number a, const coeffs r);
BOOLEAN  ngcIsZero(number za, const coeffs r);
number   ngcInit(int i, const coeffs r);
int      ngcInt(number &n, const coeffs r);
number   ngcNeg(number za, const coeffs r);
number   ngcInvers(number a, const coeffs r);
number   ngcPar(int i, const coeffs r);
number   ngcAdd(number la, number li, const coeffs r);
number   ngcSub(number la, number li, const coeffs r);
number   ngcMult(number a, number b, const coeffs r);
number   ngcDiv(number a, number b, const coeffs r);
void     ngcPower(number x, int exp, number *lu, const coeffs r);
number   ngcCopy(number a, const coeffs);
number   ngc_Copy(number a, coeffs r);
const char * ngcRead (const char *s, number *a, const coeffs r);
void     ngcWrite(number &a, const coeffs r);
number   ngcRePart(number a, const coeffs r);
number   ngcImPart(number a, const coeffs r);


#ifdef LDEBUG
BOOLEAN  ngcDBTest(number a, const char *f, const int l, const coeffs);
#endif
void     ngcDelete(number *a, const coeffs r);

nMapFunc  ngcSetMap(const coeffs src, const coeffs dst);

number ngcMapQ(number from, const coeffs r);
#endif


// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: ***
