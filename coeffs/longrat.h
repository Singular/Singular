#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computation with long rational numbers
*/
#include "coeffs.h"
// #include "config.h"
#include "si_gmp.h"
#include <auxiliary.h>
#include <omalloc.h>

#define SR_HDL(A) ((long)(A))

#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1

#define mpz_size1(A) (ABS((A)->_mp_size))
//#define mpz_size1(A) mpz_size(A)

struct snumber;
typedef struct snumber  *number;
struct snumber
{
  mpz_t z;
  mpz_t n;
#if defined(LDEBUG)
  int debug;
#endif
  BOOLEAN s;
};

// allow inlining only from p_Numbers.h and if ! LDEBUG

#if defined(DO_LINLINE) && defined(P_NUMBERS_H) && !defined(LDEBUG)
#define LINLINE static inline
#else
#define LINLINE
#undef DO_LINLINE
#endif // DO_LINLINE

LINLINE BOOLEAN  nlEqual(number a, number b, const coeffs r);
LINLINE number   nlInit(int i, const coeffs r);
number nlRInit (int i);
LINLINE BOOLEAN  nlIsOne(number a, const coeffs r);
LINLINE BOOLEAN  nlIsZero(number za, const coeffs r);
LINLINE number   nlCopy(number a, const coeffs r);
LINLINE number   nl_Copy(number a, const coeffs r);
LINLINE void     nlDelete(number *a, const coeffs r);
LINLINE number   nlNeg(number za, const coeffs r);
LINLINE number   nlAdd(number la, number li, const coeffs r);
LINLINE number   nlSub(number la, number li, const coeffs r);
LINLINE number   nlMult(number a, number b, const coeffs r);

BOOLEAN nlInitChar(coeffs r, void*);

number   nlInit2 (int i, int j, const coeffs r);
number   nlInit2gmp (mpz_t i, mpz_t j);
number   nlGcd(number a, number b, const coeffs r);
number   nlLcm(number a, number b, const coeffs r);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b, const coeffs r);
BOOLEAN  nlIsMOne(number a, const coeffs r);
int      nlInt(number &n, const coeffs r);
number   nlBigInt(number &n);

#ifdef HAVE_RINGS
void     nlGMP(number &i, number n, const coeffs r);
#endif

BOOLEAN  nlGreaterZero(number za, const coeffs r);
number   nlInvers(number a, const coeffs r);
void     nlNormalize(number &x, const coeffs r);
number   nlDiv(number a, number b, const coeffs r);
number   nlExactDiv(number a, number b, const coeffs r);
number   nlIntDiv(number a, number b, const coeffs r);
number   nlIntMod(number a, number b, const coeffs r);
void     nlPower(number x, int exp, number *lu, const coeffs r);
const char *   nlRead (const char *s, number *a, const coeffs r);
void     nlWrite(number &a, const coeffs r);
int      nlModP(number n, int p);
int      nlSize(number n, const coeffs r);
number   nlGetDenom(number &n, const coeffs r);
number   nlGetNumerator(number &n, const coeffs r);
number   nlChineseRemainder(number *x, number *q,int rl);
#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, const char *f, const int l);
#endif
extern number nlOne;

nMapFunc nlSetMap(const coeffs src, const coeffs dst);

extern omBin rnumber_bin;


#define FREE_RNUMBER(x) omFreeBin((void *)x, rnumber_bin)
#define ALLOC_RNUMBER() (number)omAllocBin(rnumber_bin)
#define ALLOC0_RNUMBER() (number)omAlloc0Bin(rnumber_bin)

// in-place operations
void nlInpGcd(number &a, number b, const coeffs r);
void nlInpIntDiv(number &a, number b, const coeffs r);
LINLINE number nlInpAdd(number la, number li, const coeffs r);
LINLINE void nlInpMult(number &a, number b, const coeffs r);

number nlFarey(number nP, number nN);

#ifdef LDEBUG
#define nlTest(a, r) nlDBTest(a,__FILE__,__LINE__, r)
BOOLEAN nlDBTest(number a, char *f,int l, const coeffs r);
#else
#define nlTest(a, r) ((void)0)
#endif

#endif


