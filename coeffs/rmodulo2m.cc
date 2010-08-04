/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo 2^m
*/

#include "config.h"
#include <auxiliary.h>

#ifdef HAVE_RINGS

#include <mylimits.h>
#include "coeffs.h"
#include "reporter.h"
#include "omalloc.h"
#include "numbers.h"
#include "longrat.h"
#include "mpr_complex.h"
#include "rmodulo2m.h"
#include "si_gmp.h"

#include <string.h>

int nr2mExp;

extern omBin gmp_nrz_bin; /* init in rintegers*/

/* for initializing function pointers */
void nr2mInitChar (coeffs r, void* p)
{
  
  nr2mInitExp((int)(long)(p), r);

     r->cfInit       = nr2mInit;
     r->cfCopy       = ndCopy;
     r->cfInt        = nr2mInt;
     r->cfAdd         = nr2mAdd;
     r->cfSub         = nr2mSub;
     r->cfMult        = nr2mMult;
     r->cfDiv         = nr2mDiv;
     r->cfIntDiv      = nr2mIntDiv;
     r->cfIntMod      = nr2mMod;
     r->cfExactDiv    = nr2mDiv;
     r->cfNeg         = nr2mNeg;
     r->cfInvers      = nr2mInvers;
     r->cfDivBy       = nr2mDivBy;
     r->cfDivComp     = nr2mDivComp;
     r->cfGreater     = nr2mGreater;
     r->cfEqual       = nr2mEqual;
     r->cfIsZero      = nr2mIsZero;
     r->cfIsOne       = nr2mIsOne;
     r->cfIsMOne      = nr2mIsMOne;
     r->cfGreaterZero = nr2mGreaterZero;
     r->cfWrite      = nr2mWrite;
     r->cfRead        = nr2mRead;
     r->cfPower       = nr2mPower;
     r->cfSetMap     = nr2mSetMap;
     r->cfNormalize   = ndNormalize;
     r->cfLcm         = nr2mLcm;
     r->cfGcd         = nr2mGcd;
     r->cfIsUnit      = nr2mIsUnit;
     r->cfGetUnit     = nr2mGetUnit;
     r->cfExtGcd      = nr2mExtGcd;
     r->cfName        = ndName;
#ifdef LDEBUG
     r->cfDBTest      = nr2mDBTest;
#endif
     r->has_simple_Alloc=TRUE;
}

/*
 * Multiply two numbers
 */
number nr2mMult (number a, number b, const coeffs r)
{
  if (((NATNUMBER)a == 0) || ((NATNUMBER)b == 0))
    return (number)0;
  else
    return nr2mMultM(a,b,r);
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nr2mLcm (number a, number b, const coeffs r)
{
  NATNUMBER res = 0;
  if ((NATNUMBER) a == 0) a = (number) 1;
  if ((NATNUMBER) b == 0) b = (number) 1;
  while ((NATNUMBER) a % 2 == 0)
  {
    a = (number) ((NATNUMBER) a / 2);
    if ((NATNUMBER) b % 2 == 0) b = (number) ((NATNUMBER) b / 2);
    res++;
  }
  while ((NATNUMBER) b % 2 == 0)
  {
    b = (number) ((NATNUMBER) b / 2);
    res++;
  }
  return (number) (1L << res);  // (2**res)
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nr2mGcd (number a, number b, const coeffs r)
{
  NATNUMBER res = 0;
  if ((NATNUMBER) a == 0 && (NATNUMBER) b == 0) return (number) 1;
  while ((NATNUMBER) a % 2 == 0 && (NATNUMBER) b % 2 == 0)
  {
    a = (number) ((NATNUMBER) a / 2);
    b = (number) ((NATNUMBER) b / 2);
    res++;
  }
//  if ((NATNUMBER) b % 2 == 0)
//  {
//    return (number) ((1L << res));// * (NATNUMBER) a);  // (2**res)*a    a ist Einheit
//  }
//  else
//  {
    return (number) ((1L << res));// * (NATNUMBER) b);  // (2**res)*b    b ist Einheit
//  }
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nr2mExtGcd (number a, number b, number *s, number *t, const coeffs r)
{
  NATNUMBER res = 0;
  if ((NATNUMBER) a == 0 && (NATNUMBER) b == 0) return (number) 1;
  while ((NATNUMBER) a % 2 == 0 && (NATNUMBER) b % 2 == 0)
  {
    a = (number) ((NATNUMBER) a / 2);
    b = (number) ((NATNUMBER) b / 2);
    res++;
  }
  if ((NATNUMBER) b % 2 == 0)
  {
    *t = NULL;
    *s = nr2mInvers(a,r);
    return (number) ((1L << res));// * (NATNUMBER) a);  // (2**res)*a    a ist Einheit
  }
  else
  {
    *s = NULL;
    *t = nr2mInvers(b,r);
    return (number) ((1L << res));// * (NATNUMBER) b);  // (2**res)*b    b ist Einheit
  }
}

void nr2mPower (number a, int i, number * result, const coeffs r)
{
  if (i==0)
  {
    *(NATNUMBER *)result = 1;
  }
  else if (i==1)
  {
    *result = a;
  }
  else
  {
    nr2mPower(a,i-1,result,r);
    *result = nr2mMultM(a,*result,r);
  }
}

/*
 * create a number from int
 */
number nr2mInit (int i, const coeffs r)
{
  if (i == 0) return (number)(NATNUMBER)i;

  long ii = i;
  NATNUMBER j = (NATNUMBER)1;
  if (ii < 0) { j = r->nr2mModul; ii = -ii; }
  NATNUMBER k = (NATNUMBER)ii;
  k = k & r->nr2mModul;
  /* now we have: from = j * k mod 2^m */
  return (number)nr2mMult((number)j, (number)k,r);
}

/*
 * convert a number to an int in ]-k/2 .. k/2],
 * where k = 2^m; i.e., an int in ]-2^(m-1) .. 2^(m-1)];
 * note that the code computes a long which will then
 * automatically casted to int
 */
int nr2mInt(number &n, const coeffs r)
{
  NATNUMBER nn = (unsigned long)(NATNUMBER)n & r->nr2mModul;
  unsigned long l = r->nr2mModul >> 1; l++; /* now: l = 2^(m-1) */
  if ((NATNUMBER)nn > l)
    return (int)((NATNUMBER)nn - r->nr2mModul - 1);
  else
    return (int)((NATNUMBER)nn);
}

number nr2mAdd (number a, number b, const coeffs r)
{
  return nr2mAddM(a,b,r);
}

number nr2mSub (number a, number b, const coeffs r)
{
  return nr2mSubM(a,b,r);
}

BOOLEAN nr2mIsUnit (number a, const coeffs r)
{
  return ((NATNUMBER) a % 2 == 1);
}

number  nr2mGetUnit (number k, const coeffs r)
{
  if (k == NULL)
    return (number) 1;
  NATNUMBER tmp = (NATNUMBER) k;
  while (tmp % 2 == 0)
    tmp = tmp / 2;
  return (number) tmp;
}

BOOLEAN nr2mIsZero (number a, const coeffs r)
{
  return 0 == (NATNUMBER)a;
}

BOOLEAN nr2mIsOne (number a, const coeffs r)
{
  return 1 == (NATNUMBER)a;
}

BOOLEAN nr2mIsMOne (number a, const coeffs r)
{
  return (r->nr2mModul  == (NATNUMBER)a) 
        && (r->nr2mModul != 2);
}

BOOLEAN nr2mEqual (number a, number b, const coeffs r)
{
  return a==b;
}

BOOLEAN nr2mGreater (number a, number b, const coeffs r)
{
  return nr2mDivBy(a, b,r);
}

/* Is a divisible by b? There are two cases:
   1) a = 0 mod 2^m; then TRUE iff b = 0 or b is a power of 2
   2) a, b <> 0; then TRUE iff b/gcd(a, b) is a unit mod 2^m
   TRUE iff b(gcd(a, b) is a unit */
BOOLEAN nr2mDivBy (number a, number b, const coeffs r)
{
  if (a == NULL)
  {
    NATNUMBER c = r->nr2mModul + 1;
    if (c != 0) /* i.e., if no overflow */
      return (c % (NATNUMBER)b) == 0;
    else
    {
      /* overflow: we need to check whether b
         is zero or a power of 2: */
      c = (NATNUMBER)b;
      while (c != 0)
      {
        if ((c % 2) != 0) return FALSE;
        c = c >> 1;
      }
      return TRUE;
    }
  }
  else
  {
    number n = nr2mGcd(a, b, r);
    n = nr2mDiv(b, n, r);
    return nr2mIsUnit(n, r);
  }
}

int nr2mDivComp(number as, number bs, const coeffs r)
{
  NATNUMBER a = (NATNUMBER) as;
  NATNUMBER b = (NATNUMBER) bs;
  assume(a != 0 && b != 0);
  while (a % 2 == 0 && b % 2 == 0)
  {
    a = a / 2;
    b = b / 2;
  }
  if (a % 2 == 0)
  {
    return -1;
  }
  else
  {
    if (b % 2 == 1)
    {
      return 2;
    }
    else
    {
      return 1;
    }
  }
}

/* TRUE iff 0 < k <= 2^m / 2 */
BOOLEAN nr2mGreaterZero (number k, const coeffs r)
{
  if ((NATNUMBER)k == 0) return FALSE;
  if ((NATNUMBER)k > ((r->nr2mModul >> 1) + 1)) return FALSE;
  return TRUE;
}

/* assumes that 'a' is odd, i.e., a unit in Z/2^m, and computes
   the extended gcd of 'a' and 2^m, in order to find some 's'
   and 't' such that a * s + 2^m * t = gcd(a, 2^m) = 1;
   this code will always find a positive 's' */
void specialXGCD(unsigned long& s, unsigned long a, const coeffs R)
{
  int_number u = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(u, a);
  int_number u0 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(u0);
  int_number u1 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(u1, 1);
  int_number u2 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(u2);
  int_number v = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(v, R->nr2mModul);
  mpz_add_ui(v, v, 1); /* now: v = 2^m */
  int_number v0 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(v0);
  int_number v1 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(v1);
  int_number v2 = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(v2, 1);
  int_number q = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(q);
  int_number r = (int_number)omAlloc(sizeof(mpz_t));
  mpz_init(r);

  while (mpz_cmp_ui(v, 0) != 0) /* i.e., while v != 0 */
  {
    mpz_div(q, u, v);
    mpz_mod(r, u, v);
    mpz_set(u, v);
    mpz_set(v, r);
    mpz_set(u0, u2);
    mpz_set(v0, v2);
    mpz_mul(u2, u2, q); mpz_sub(u2, u1, u2); /* u2 = u1 - q * u2 */
    mpz_mul(v2, v2, q); mpz_sub(v2, v1, v2); /* v2 = v1 - q * v2 */
    mpz_set(u1, u0);
    mpz_set(v1, v0);
  }

  while (mpz_cmp_ui(u1, 0) < 0) /* i.e., while u1 < 0 */
  {
    /* we add 2^m = (2^m - 1) + 1 to u1: */
    mpz_add_ui(u1, u1, R->nr2mModul);
    mpz_add_ui(u1, u1, 1);
  }
  s = mpz_get_ui(u1); /* now: 0 <= s <= 2^m - 1 */

  mpz_clear(u);  omFree((ADDRESS)u);
  mpz_clear(u0); omFree((ADDRESS)u0);
  mpz_clear(u1); omFree((ADDRESS)u1);
  mpz_clear(u2); omFree((ADDRESS)u2);
  mpz_clear(v);  omFree((ADDRESS)v);
  mpz_clear(v0); omFree((ADDRESS)v0);
  mpz_clear(v1); omFree((ADDRESS)v1);
  mpz_clear(v2); omFree((ADDRESS)v2);
  mpz_clear(q); omFree((ADDRESS)q);
  mpz_clear(r); omFree((ADDRESS)r);
}

NATNUMBER InvMod(NATNUMBER a, const coeffs r)
{
  assume((NATNUMBER)a % 2 != 0);
  unsigned long s;
  specialXGCD(s, a, r);
  return s;
}
//#endif

inline number nr2mInversM (number c, const coeffs r)
{
  assume((NATNUMBER)c % 2 != 0);
  // Table !!!
  NATNUMBER inv;
  inv = InvMod((NATNUMBER)c,r);
  return (number) inv;
}

number nr2mDiv (number a, number b, const coeffs r)
{
  if ((NATNUMBER)a==0)
    return (number)0;
  else if ((NATNUMBER)b%2==0)
  {
    if ((NATNUMBER)b != 0)
    {
      while ((NATNUMBER) b%2 == 0 && (NATNUMBER) a%2 == 0)
      {
        a = (number) ((NATNUMBER) a / 2);
        b = (number) ((NATNUMBER) b / 2);
      }
    }
    if ((NATNUMBER) b%2 == 0)
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      WerrorS("Result is integer division without remainder.");
      return (number) ((NATNUMBER) a / (NATNUMBER) b);
    }
  }
  return (number) nr2mMult(a, nr2mInversM(b,r),r);
}

number nr2mMod (number a, number b, const coeffs R)
{
  /*
    We need to return the number r which is uniquely determined by the
    following two properties:
      (1) 0 <= r < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + r.
    Consider g := gcd(2^m, |b|). Note that then |b|/g is a unit in Z/2^m.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/2^m, i.e. |b| (and also b) divides a.
          Thus r = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again r = 0.
      (c) g <> 1 and g does not divide a
          Let's denote the division with remainder of a by g as follows:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. r := t is the correct result. Hence
          in this third case, r is the remainder of division of a by g in Z.
    This algorithm is the same as for the case Z/n, except that we may
    compute the gcd of |b| and 2^m "by hand": We just extract the highest
    power of 2 (<= 2^m) that is contained in b.
  */
  assume((NATNUMBER)b != 0);
  NATNUMBER g = 1;
  NATNUMBER b_div = (NATNUMBER)b;
  if (b_div < 0) b_div = -b_div; // b_div now represents |b|
  NATNUMBER r = 0;
  while ((g < R->nr2mModul ) && (b_div > 0) && (b_div % 2 == 0))
  {
    b_div = b_div >> 1;
    g = g << 1;
  } // g is now the gcd of 2^m and |b|

  if (g != 1) r = (NATNUMBER)a % g;
  return (number)r;
}

number nr2mIntDiv (number a, number b, const coeffs r)
{
  if ((NATNUMBER)a == 0)
  {
    if ((NATNUMBER)b == 0)
      return (number)1;
    if ((NATNUMBER)b == 1)
      return (number)0;
    NATNUMBER c = r->nr2mModul + 1;
    if (c != 0) /* i.e., if no overflow */
      return (number)(c / (NATNUMBER)b);
    else
    {
      /* overflow: c = 2^32 resp. 2^64, depending on platform */
      int_number cc = (int_number)omAlloc(sizeof(mpz_t));
      mpz_init_set_ui(cc, r->nr2mModul); mpz_add_ui(cc, cc, 1);
      mpz_div_ui(cc, cc, (unsigned long)(NATNUMBER)b);
      unsigned long s = mpz_get_ui(cc);
      mpz_clear(cc); omFree((ADDRESS)cc);
      return (number)(NATNUMBER)s;
    }
  }
  else
  {
    if ((NATNUMBER)b == 0)
      return (number)0;
    return (number)((NATNUMBER) a / (NATNUMBER) b);
  }
}

number  nr2mInvers (number c, const coeffs r)
{
  if ((NATNUMBER)c%2==0)
  {
    WerrorS("division by zero divisor");
    return (number)0;
  }
  return nr2mInversM(c,r);
}

number nr2mNeg (number c, const coeffs r)
{
  if ((NATNUMBER)c==0) return c;
  return nr2mNegM(c,r);
}

number nr2mMapMachineInt(number from, const coeffs src, const coeffs dst)
{
  NATNUMBER i = ((NATNUMBER) from) % dst->nr2mModul ;
  return (number) i;
}

number nr2mMapZp(number from, const coeffs src, const coeffs dst)
{
  NATNUMBER j = (NATNUMBER)1;
  long ii = (long) from;
  if (ii < 0) { j = dst->nr2mModul; ii = -ii; }
  NATNUMBER i = (NATNUMBER)ii;
  i = i & dst->nr2mModul;
  /* now we have: from = j * i mod 2^m */
  return (number)nr2mMult((number)i, (number)j, dst);
}

number nr2mMapQ(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)  omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number k = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(k, dst->nr2mModul);

  nlGMP(from, (number)erg, dst);
  mpz_and(erg, erg, k);
  number res = (number)mpz_get_ui(erg);

  mpz_clear(erg); omFree((ADDRESS)erg);
  mpz_clear(k);   omFree((ADDRESS)k);

  return (number) res;
}

number nr2mMapGMP(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)  omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number k = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(k, dst->nr2mModul);

  mpz_and(erg, (int_number)from, k);
  number res = (number) mpz_get_ui(erg);

  mpz_clear(erg); omFree((ADDRESS)erg);
  mpz_clear(k);   omFree((ADDRESS)k);

  return (number) res;
}

nMapFunc nr2mSetMap(const coeffs src, const coeffs dst)
{
  if (nField_is_Ring_2toM(src)
     && (src->ringflagb == dst->ringflagb))
  {
    return ndCopyMap;
  }
  if (nField_is_Ring_2toM(src)
     && (src->ringflagb < dst->ringflagb))
  { /* i.e. map an integer mod 2^s into Z mod 2^t, where t < s */
    return nr2mMapMachineInt;
  }
  if (nField_is_Ring_2toM(src)
     && (src->ringflagb > dst->ringflagb))
  { /* i.e. map an integer mod 2^s into Z mod 2^t, where t > s */
    // to be done
  }
  if (nField_is_Ring_Z(src))
  {
    return nr2mMapGMP;
  }
  if (nField_is_Q(src))
  {
    return nr2mMapQ;
  }
  if (nField_is_Zp(src)
     && (src->ch == 2)
     && (dst->ringflagb == 1))
  {
    return nr2mMapZp;
  }
  if (nField_is_Ring_PtoM(src) || nField_is_Ring_ModN(src))
  {
    // Computing the n of Z/n
    int_number modul = (int_number)  omAllocBin(gmp_nrz_bin);
    mpz_init(modul);
    mpz_set(modul, src->ringflaga);
    mpz_pow_ui(modul, modul, src->ringflagb);
    if (mpz_divisible_2exp_p(modul, dst->ringflagb))
    {
      mpz_clear(modul);
      omFree((void *) modul);
      return nr2mMapGMP;
    }
    mpz_clear(modul);
    omFree((void *) modul);
  }
  return NULL;      // default
}

/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nr2mSetExp(int m, coeffs r)
{
  if (m > 1)
  {
    nr2mExp = m;
    /* we want nr2mModul to be the bit pattern '11..1' consisting
       of m one's: */
    r->nr2mModul = 1;
    for (int i = 1; i < m; i++) r->nr2mModul = (r->nr2mModul * 2) + 1;
  }
  else
  {
    nr2mExp = 2;
    r->nr2mModul = 3; /* i.e., '11' in binary representation */
  }
}

void nr2mInitExp(int m, coeffs r)
{
  nr2mSetExp(m, r);
  if (m < 2) WarnS("nr2mInitExp failed: we go on with Z/2^2");
}

#ifdef LDEBUG
BOOLEAN nr2mDBTest (number a, const char *f, const int l, const coeffs r)
{
  if ((NATNUMBER)a < 0) return FALSE;
  if (((NATNUMBER)a & r->nr2mModul) != (NATNUMBER)a) return FALSE;
  return TRUE;
}
#endif

void nr2mWrite (number &a, const coeffs r)
{
  int i = nr2mInt(a, r);
  StringAppend("%d", i);
}

static const char* nr2mEati(const char *s, int *i, const coeffs r)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) & r->nr2mModul;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    (*i) = (*i) & r->nr2mModul;
  }
  else (*i) = 1;
  return s;
}

const char * nr2mRead (const char *s, number *a, const coeffs r)
{
  int z;
  int n=1;

  s = nr2mEati(s, &z,r);
  if ((*s) == '/')
  {
    s++;
    s = nr2mEati(s, &n,r);
  }
  if (n == 1)
    *a = (number)z;
  else
      *a = nr2mDiv((number)z,(number)n,r);
  return s;
}
#endif
/* #ifdef HAVE_RINGS */
