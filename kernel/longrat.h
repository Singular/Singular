#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computation with long rational numbers
*/
#include <kernel/structs.h>

#include <kernel/si_gmp.h>

#define SR_HDL(A) ((long)(A))

#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) ((((long)INT) << 2) + SR_INT))
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

LINLINE BOOLEAN  nlEqual(number a, number b);
LINLINE number   nlInit(int i, const ring r);
LINLINE number nlInitgmp (mpz_t i);
number nlRInit (long i);
LINLINE BOOLEAN  nlIsOne(number a);
LINLINE BOOLEAN  nlIsZero(number za);
LINLINE number   nlCopy(number a);
LINLINE void     nlDelete(number *a, const ring r);
LINLINE number   nlNeg(number za);
LINLINE number   nlAdd(number la, number li);
LINLINE number   nlSub(number la, number li);
LINLINE number   nlMult(number a, number b);

number   nlInit2 (int i, int j);
number   nlInit2gmp (mpz_t i, mpz_t j);
number   nlGcd(number a, number b, const ring r);
number   nlLcm(number a, number b, const ring r);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b);
BOOLEAN  nlIsMOne(number a);
int      nlInt(number &n, const ring r);
number   nlBigInt(number &n);
#ifdef HAVE_RINGS
void     nlGMP(number &i, number n);
#endif
BOOLEAN  nlGreaterZero(number za);
number   nlInvers(number a);
void     nlNormalize(number &x);
number   nlDiv(number a, number b);
number   nlExactDiv(number a, number b);
number   nlIntDiv(number a, number b);
number   nlIntMod(number a, number b);
void     nlPower(number x, int exp, number *lu);
const char *   nlRead (const char *s, number *a);
void     nlWrite(number &a, const ring r);
int      nlModP(number n, int p);
int      nlSize(number n);
number   nlGetDenom(number &n, const ring r);
number   nlGetNumerator(number &n, const ring r);
number   nlChineseRemainder(number *x, number *q,int rl, const ring);
#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, const char *f, const int l);
#endif
extern number nlOne;

nMapFunc nlSetMap(const ring src, const ring dst);
#ifdef HAVE_RINGS
number nlMapGMP(number from);
#endif

//#define SI_THREADS 1
#ifdef SI_THREADS
#define ALLOC_RNUMBER() (number)malloc(sizeof(snumber));
static inline number ALLOC0_RNUMBER() { number n=(number)malloc(sizeof(snumber));memset(n,0,sizeof(snumber));return n; }
#define FREE_RNUMBER(x) free(x)
#else
extern omBin rnumber_bin;
#define ALLOC_RNUMBER() (number)omAllocBin(rnumber_bin)
#define ALLOC0_RNUMBER() (number)omAlloc0Bin(rnumber_bin)
#define FREE_RNUMBER(x) omFreeBin((ADDRESS)x, rnumber_bin)
#endif

// in-place operations
void nlInpGcd(number &a, number b, const ring r);
void nlInpIntDiv(number &a, number b, const ring r);
LINLINE number   nlInpAdd(number la, number li, const ring r);
LINLINE void nlInpMult(number &a, number b, ring r);

number nlFarey(number nP, number nN, const ring);

#ifdef LDEBUG
#define nlTest(a) nlDBTest(a,__FILE__,__LINE__)
BOOLEAN nlDBTest(number a, char *f,int l);
#else
#define nlTest(a) ((void)1)
#endif

#endif


