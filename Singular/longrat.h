#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longrat.h,v 1.4 1997-05-05 10:14:08 Singular Exp $ */
/*
* ABSTRACT: computation with long rational numbers
*/
#include "structs.h"
#ifdef HAVE_GMP
extern "C" {
#ifdef macintosh
#include "b_gmp.h"
#else
#include "gmp.h"
#endif
}
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

#else

typedef unsigned short int16;
typedef int16 * lint;
#endif

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
number   nlIntDiv(number a, number b);
number   nlIntMod(number a, number b);
void     nlPower(number x, int exp, number *lu);
number   nlCopy(number a);
char *   nlRead (char *s, number *a);
void     nlWrite(number &a);
int      nlModP(number n, int p);
int      nlSize(number n);
#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, char *f, int l);
void     nlDBDelete(number *a, char *f, int l);
#define  nlDelete(A) nlDBDelete(A,__FILE__,__LINE__)
#else
void     nlDelete(number *a);
#endif

BOOLEAN nlSetMap(int c, char ** par, int nop, number minpol);

// internal use (longrat0) only:
#ifndef HAVE_GMP
void nlDivMod(lint a, int16 al, int16 b, int16 * r);
#endif
#endif


