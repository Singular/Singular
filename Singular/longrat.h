#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longrat.h,v 1.13 1999-09-16 12:33:59 Singular Exp $ */
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
#ifdef LDEBUG
  int debug;
#endif
  BOOLEAN s;
};

number   nlGcd(number a, number b);
number   nlLcm(number a, number b);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b);
BOOLEAN  nlEqual(number a, number b);
BOOLEAN  nlIsOne(number a);
BOOLEAN  nlIsMOne(number a);
void     nlNew(number *r);
number   nlInit(int i);
number   nlInit(number i);
int      nlInt(number &n);
BOOLEAN  nlIsZero(number za);
BOOLEAN  nlGreaterZero(number za);
number   nlNeg(number za);
number   nlInvers(number a);
void     nlNormalize(number &x);
number   nlAdd(number la, number li);
number   nlSub(number la, number li);
number   nlMult(number a, number b);
number   nlDiv(number a, number b);
number   nlExactDiv(number a, number b);
number   nlIntDiv(number a, number b);
number   nlIntMod(number a, number b);
void     nlPower(number x, int exp, number *lu);
number   nlCopy(number a);
char *   nlRead (char *s, number *a);
void     nlWrite(number &a);
int      nlModP(number n, int p);
int      nlSize(number n);
number   nlGetDenom(number &n);
#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, char *f, int l);
void     nlDBDelete(number *a, char *f, int l);
#define  nlDelete(A) nlDBDelete(A,__FILE__,__LINE__)
#else
void     nlDelete(number *a);
#endif

BOOLEAN nlSetMap(ring r);

#endif


