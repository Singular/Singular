#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longrat.h,v 1.19 2000-12-15 18:49:33 Singular Exp $ */
/*
* ABSTRACT: computation with long rational numbers
*/
#include "structs.h"

extern "C" {
#include <gmp.h>
}
#ifdef HAVE_SMALLGMP
#  define HAVE_LIBGMP2
#else
#if (__GNU_MP_VERSION > 1) && (__GNU_MP_VERSION_MINOR >= 0)
#  define HAVE_LIBGMP2
#else
#  define HAVE_LIBGMP1
#endif
#endif

typedef MP_INT lint;

#define SR_INT    1
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)


#define MP_SMALL 1

#ifdef HAVE_LIBGMP1
#define mpz_size1(A) (ABS((A)->size))
#else
#define mpz_size1(A) (ABS((A)->_mp_size))
#endif
//#define mpz_size1(A) mpz_size(A)

struct snumber;
typedef struct snumber rnumber;
typedef rnumber * number;
struct snumber
{
  lint z;
  lint n;
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

LINLINE BOOLEAN  nlEqual(number a, number b);
LINLINE number   nlInit(int i);
LINLINE BOOLEAN  nlIsOne(number a);
LINLINE BOOLEAN  nlIsZero(number za);
LINLINE number   nlCopy(number a);
LINLINE void     nlNew(number *r);
#ifndef LDEBUG
LINLINE void     nlDelete(number *a);
#endif
LINLINE number   nlNeg(number za);
LINLINE number   nlAdd(number la, number li);
LINLINE number   nlSub(number la, number li);
LINLINE number   nlMult(number a, number b);

number   nlGcd(number a, number b);
number   nlLcm(number a, number b);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b);
BOOLEAN  nlIsMOne(number a);
number   nlInit(number i);
int      nlInt(number &n);
BOOLEAN  nlGreaterZero(number za);
number   nlInvers(number a);
void     nlNormalize(number &x);
number   nlDiv(number a, number b);
number   nlExactDiv(number a, number b);
number   nlIntDiv(number a, number b);
number   nlIntMod(number a, number b);
void     nlPower(number x, int exp, number *lu);
char *   nlRead (char *s, number *a);
void     nlWrite(number &a);
int      nlModP(number n, int p);
int      nlSize(number n);
number   nlGetDenom(number &n);
#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, char *f, int l);
void     nlDBDelete(number *a, char *f, int l);
#define  nlDelete(A) nlDBDelete(A,__FILE__,__LINE__)
#endif

nMapFunc nlSetMap(ring src, ring dst);

#ifndef OM_ALLOC_H
struct omBin_s;
#endif
extern omBin_s* rnumber_bin;

#endif


