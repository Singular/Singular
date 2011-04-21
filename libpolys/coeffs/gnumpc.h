#ifndef GMPCOMPLEX_H
#define GMPCOMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include <coeffs/coeffs.h>

/// Get a mapping function from src into the domain of this type: long_C!
nMapFunc  ngcSetMap(const coeffs src, const coeffs dst);

/// Initialize r
BOOLEAN ngcInitChar(coeffs r, void*);


// Private interface should be hidden!!!
     
/// Note: MAY NOT WORK AS EXPECTED!
BOOLEAN  ngcGreaterZero(number za, const coeffs r); 
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
number   ngcCopy(number a, const coeffs r);
number   ngc_Copy(number a, coeffs r);
const char * ngcRead (const char *s, number *a, const coeffs r);
void     ngcWrite(number &a, const coeffs r);
number   ngcRePart(number a, const coeffs r);
number   ngcImPart(number a, const coeffs r);

void     ngcDelete(number *a, const coeffs r);
void     ngcCoeffWrite(const coeffs r);

#ifdef LDEBUG
BOOLEAN  ngcDBTest(number a, const char *f, const int l, const coeffs r);
#endif


// Why is this here? who needs it?
// number ngcMapQ(number from, const coeffs r, const coeffs aRing);
#endif
/* GMPCOMPLEX_H */
