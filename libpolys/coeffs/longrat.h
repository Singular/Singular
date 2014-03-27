#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computation with long rational numbers
*/
#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>
#include <reporter/reporter.h>

#include <coeffs/si_gmp.h>
#include <coeffs/coeffs.h>

struct snumber;
typedef struct snumber  *number;

/*-----------------------------------------------------------------*/
/**
**  'SR_INT' is the type of those integers small enough to fit into  29  bits.
**  Therefor the value range of this small integers is: $-2^{28}...2^{28}-1$.
**
**  Small integers are represented by an immediate integer handle, containing
**  the value instead of pointing  to  it,  which  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | guard | sign  | bit   | bit   |       | bit   | tag   | tag   |
**      | bit   | bit   | 27    | 26    |       | 0     | 0     | 1     |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  Immediate integers handles carry the tag 'SR_INT', i.e. the last bit is 1.
**  This distuingishes immediate integers from other handles which  point  to
**  structures aligned on 4 byte boundaries and therefor have last bit  zero.
**  (The second bit is reserved as tag to allow extensions of  this  scheme.)
**  Using immediates as pointers and dereferencing them gives address errors.
**
**  To aid overflow check the most significant two bits must always be equal,
**  that is to say that the sign bit of immediate integers has a  guard  bit.
**
**  The macros 'INT_TO_SR' and 'SR_TO_INT' should be used to convert  between
**  a small integer value and its representation as immediate integer handle.
**
**  Large integers and rationals are represented by z and n
**  where n may be undefined (if s==3)
**  NULL represents only deleted values
*/

struct snumber
{
  mpz_t z; //< Zaehler
  mpz_t n; //< Nenner
#if defined(LDEBUG)
  int debug;
#endif

  /**
   * parameter s in number:
   * 0 (or FALSE): not normalised rational
   * 1 (or TRUE):  normalised rational
   * 3          :  integer with n==NULL
   **/
  BOOLEAN s; //< integer parameter
};

#define SR_HDL(A) ((long)(A))

#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1



// allow inlining only from p_Numbers.h and if ! LDEBUG

#if defined(DO_LINLINE) && defined(P_NUMBERS_H) && !defined(LDEBUG)
#define LINLINE static inline
#else
#define LINLINE
#undef DO_LINLINE
#endif // DO_LINLINE

LINLINE BOOLEAN  nlEqual(number a, number b, const coeffs r);
LINLINE number   nlInit(long i, const coeffs r);
number nlRInit (long i);
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
number   nlInit2gmp (mpz_t i, mpz_t j, const coeffs r);

// number nlInitMPZ(mpz_t m, const coeffs r);
// void nlMPZ(mpz_t m, number &n, const coeffs r);

number   nlGcd(number a, number b, const coeffs r);
number nlExtGcd(number a, number b, number *s, number *t, const coeffs);
number   nlLcm(number a, number b, const coeffs r);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b, const coeffs r);
BOOLEAN  nlIsMOne(number a, const coeffs r);
int      nlInt(number &n, const coeffs r);
number   nlBigInt(number &n);

#ifdef HAVE_RINGS
number nlMapGMP(number from, const coeffs src, const coeffs dst);
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

/// Map q \in QQ \to Zp
number nlModP(number q, const coeffs Q, const coeffs Zp);

int      nlSize(number n, const coeffs r);

static inline int nlQlogSize (number n, const coeffs r)
{
  assume( nCoeff_is_Q (r) );

  long nl=nlSize(n,r);
  if (nl==0L) return 0;
  if (nl==1L)
  {
    long i = SR_TO_INT (n);
    unsigned long v;
    v = (i >= 0) ? i : -i;
    int r = 0;

    while(v >>= 1)
    {
      r++;
    }
    return r + 1;
  }
  //assume denominator is 0
  number nn=(number) n;
  return mpz_sizeinbase (nn->z, 2);
}

number   nlGetDenom(number &n, const coeffs r);
number   nlGetNumerator(number &n, const coeffs r);
void     nlCoeffWrite(const coeffs r, BOOLEAN details);
number   nlChineseRemainder(number *x, number *q,int rl, const coeffs C);
number   nlFarey(number nN, number nP, const coeffs CF);

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

LINLINE void nlInpAdd(number &a, number b, const coeffs r);
LINLINE void nlInpMult(number &a, number b, const coeffs r);

#ifdef LDEBUG
#define nlTest(a, r) nlDBTest(a,__FILE__,__LINE__, r)
BOOLEAN nlDBTest(number a, char *f,int l, const coeffs r);
#else
#define nlTest(a, r) do {} while (0)
#endif

#endif


