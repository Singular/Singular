#ifndef NUMBERS_H
#define NUMBERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: compatility interface to coeffs
*/
#include <coeffs/coeffs.h>

/*
// the access methods
//
// the routines w.r.t. currRing:
// (should only be used in the context of currRing, i.e. in t
#define nCopy(n)          n_Copy(n, currRing->cf)
#define nDelete(n)        n_Delete(n, currRing->cf)
#define nMult(n1, n2)     n_Mult(n1, n2, currRing->cf)
#define nAdd(n1, n2)      n_Add(n1, n2, currRing->cf)
#define nIsZero(n)        n_IsZero(n, currRing->cf)
#define nEqual(n1, n2)    n_Equal(n1, n2, currRing->cf)
#define nNeg(n)           n_Neg(n, currRing->cf)
#define nSub(n1, n2)      n_Sub(n1, n2, currRing->cf)
#define nGetChar()        nInternalChar(currRing->cf)
#define nInit(i)          n_Init(i, currRing->cf)
#define nIsOne(n)         n_IsOne(n, currRing->cf)
#define nIsMOne(n)        n_IsMOne(n, currRing->cf)
#define nGreaterZero(n)   n_GreaterZero(n, currRing->cf)
#define nWrite(n)         n_Write(n,currRing->cf)
#define nNormalize(n)     n_Normalize(n,currRing->cf)
#define nGcd(a, b)        n_Gcd(a,b,currRing->cf)
#define nIntDiv(a, b)     n_IntDiv(a,b,currRing->cf)
#define nDiv(a, b)        n_Div(a,b,currRing->cf)
#define nInvers(a)        n_Invers(a,currRing->cf)
#define nExactDiv(a, b)   n_ExactDiv(a,b,currRing->cf)
#define nTest(a)          n_Test(a,currRing->cf)

#define nInpMult(a, b)    n_InpMult(a,b,currRing->cf)
#define nPower(a, b, res) n_Power(a,b,res,currRing->cf)
#define nSize(n)          n_Size(n,currRing->cf)
#define nGetDenom(N)      n_GetDenom((N),currRing->cf)
#define nGetNumerator(N)  n_GetNumerator((N),currRing->cf)

#define nSetMap(R)        n_SetMap(R,currRing->cf)
*/


// --------------------------------------------------------------
// internal to coeffs, but public for all realizations

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits

/* the dummy routines: */
void nDummy1(number* d);
void ndDelete(number* d, const coeffs r);
number ndGcd(number a, number b, const coeffs);
number ndCopy(number a, const coeffs r);
number ndCopyMap(number a, const coeffs src, const coeffs dst);
int ndSize(number a, const coeffs r);
char * ndName(number n, const coeffs r);
number ndPar(int i, const coeffs r);
int    ndParDeg(number n, const coeffs r);
number ndGetDenom(number &n, const coeffs r);
number ndGetNumerator(number &a,const coeffs r);
number ndReturn0(number n, const coeffs r);
number ndIntMod(number a, number b, const coeffs r);

void   ndInpMult(number &a, number b, const coeffs r);
void   ndInpAdd(number &a, number b, const coeffs r);

void ndKillChar(coeffs);

number  ndInit_bigint(number i, const coeffs dummy, const coeffs dst);

BOOLEAN ndCoeffIsEqual(const coeffs r, n_coeffType n, void * parameter);

/// Test whether a is a zero divisor in r
/// i.e. not coprime with char. of r
/// very inefficient implementation:
/// should ONLY be used for debug stuff /tests
BOOLEAN n_IsZeroDivisor( number a, const coeffs r);

#ifdef LDEBUG
void nDBDummy1(number* d,char *f, int l);
BOOLEAN ndDBTest(number a, const char *f, const int l, const coeffs r);
#endif

#define nDivBy0 "div by 0"

// dummy routines
void   ndNormalize(number& d, const coeffs); // nNormalize...

/// initialize an object of type coeff, return FALSE in case of success
typedef BOOLEAN (*cfInitCharProc)(coeffs, void *);
n_coeffType nRegister(n_coeffType n, cfInitCharProc p);

#endif
