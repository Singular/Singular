/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo 2^m
*/

#include <string.h>
#include <kernel/mod2.h>

#ifdef HAVE_RINGS
#include <mylimits.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/mpr_complex.h>
#include <kernel/ring.h>
#include <kernel/rmodulo2m.h>
#include <kernel/si_gmp.h>

int nr2mExp;

/*
 * Multiply two numbers
 */
number nr2mMult (number a, number b)
{
  if (((NATNUMBER)a == 0) || ((NATNUMBER)b == 0))
    return (number)0;
  else
    return nr2mMultM(a,b);
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nr2mLcm (number a,number b,ring r)
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
number nr2mGcd (number a,number b,ring r)
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
number nr2mExtGcd (number a, number b, number *s, number *t)
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
    *s = nr2mInvers(a);
    return (number) ((1L << res));// * (NATNUMBER) a);  // (2**res)*a    a ist Einheit
  }
  else
  {
    *s = NULL;
    *t = nr2mInvers(b);
    return (number) ((1L << res));// * (NATNUMBER) b);  // (2**res)*b    b ist Einheit
  }
}

void nr2mPower (number a, int i, number * result)
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
    nr2mPower(a,i-1,result);
    *result = nr2mMultM(a,*result);
  }
}

/*
 * create a number from int
 */
number nr2mInit (int i, const ring r)
{
  if (i == 0) return (number)(NATNUMBER)i;

  long ii = i;
  NATNUMBER j = (NATNUMBER)1;
  if (ii < 0) { j = currRing->nr2mModul; ii = -ii; }
  NATNUMBER k = (NATNUMBER)ii;
  k = k & currRing->nr2mModul;
  /* now we have: from = j * k mod 2^m */
  return (number)nr2mMult((number)j, (number)k);
}

/*
 * convert a number to an int in ]-k/2 .. k/2],
 * where k = 2^m; i.e., an int in ]-2^(m-1) .. 2^(m-1)];
 * note that the code computes a long which will then
 * automatically casted to int
 */
int nr2mInt(number &n, const ring r)
{
  NATNUMBER nn = (unsigned long)(NATNUMBER)n & r->nr2mModul;
  unsigned long l = r->nr2mModul >> 1; l++;
  if (l == 0)
    return (int)(signed long)(NATNUMBER)nn;
  else if ((NATNUMBER)nn > l)
    return (int)((NATNUMBER)nn - r->nr2mModul - 1);
  else
    return (int)((NATNUMBER)nn);
}

number nr2mAdd (number a, number b)
{
  return nr2mAddM(a,b);
}

number nr2mSub (number a, number b)
{
  return nr2mSubM(a,b);
}

BOOLEAN nr2mIsUnit (number a)
{
  return ((NATNUMBER) a % 2 == 1);
}

number  nr2mGetUnit (number k)
{
  if (k == NULL)
    return (number) 1;
  NATNUMBER tmp = (NATNUMBER) k;
  while (tmp % 2 == 0)
    tmp = tmp / 2;
  return (number) tmp;
}

BOOLEAN nr2mIsZero (number  a)
{
  return 0 == (NATNUMBER)a;
}

BOOLEAN nr2mIsOne (number a)
{
  return 1 == (NATNUMBER)a;
}

BOOLEAN nr2mIsMOne (number a)
{
  return (currRing->nr2mModul == (NATNUMBER)a);
}

BOOLEAN nr2mEqual (number a, number b)
{
  return nr2mEqualM(a,b);
}

BOOLEAN nr2mGreater (number a, number b)
{
  return nr2mDivBy(a, b);
}

BOOLEAN nr2mDivBy (number a, number b)
{
  if ((NATNUMBER)a == 0) return TRUE;
  if ((NATNUMBER)b == 0) return FALSE;
  return ((NATNUMBER)a % (NATNUMBER)b) == 0;
}

int nr2mDivComp(number as, number bs)
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
      return 0;
    }
    else
    {
      return 1;
    }
  }
}

/* TRUE iff 0 < k <= 2^m / 2 */
BOOLEAN nr2mGreaterZero (number k)
{
  if ((NATNUMBER)k == 0) return FALSE;
  if ((NATNUMBER)k > ((currRing->nr2mModul >> 1) + 1)) return FALSE;
  return TRUE;
}

/* assumes that 'a' is odd, i.e., a unit in Z/2^m, and computes
   the extended gcd of 'a' and 2^m, in order to find some 's'
   and 't' such that a * s + 2^m * t = gcd(a, 2^m) = 1;
   this code will always find a positive 's' */
void specialXGCD(unsigned long& s, unsigned long a)
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
  mpz_init_set_ui(v, currRing->nr2mModul);
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
    mpz_add_ui(u1, u1, currRing->nr2mModul);
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

NATNUMBER InvMod(NATNUMBER a)
{
  assume((NATNUMBER)a % 2 != 0);
  unsigned long s;
  specialXGCD(s, a);
  return s;
}
//#endif

inline number nr2mInversM (number c)
{
  assume((NATNUMBER)c % 2 != 0);
  return (number)InvMod((NATNUMBER)c);
}

number nr2mDiv (number a,number b)
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
  return (number) nr2mMult(a, nr2mInversM(b));
}

number nr2mMod (number a, number b)
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
  while ((g < currRing->nr2mModul) && (b_div > 0) && (b_div % 2 == 0))
  {
    b_div = b_div >> 1;
    g = g << 1;
  } // g is now the gcd of 2^m and |b|

  if (g != 1) r = (NATNUMBER)a % g;
  return (number)r;
}

number nr2mIntDiv (number a,number b)
{
  assume((NATNUMBER)b != 0);
  return (number) ((NATNUMBER) a / (NATNUMBER) b);
}

number  nr2mInvers (number c)
{
  if ((NATNUMBER)c%2==0)
  {
    WerrorS("division by zero divisor");
    return (number)0;
  }
  return nr2mInversM(c);
}

number nr2mNeg (number c)
{
  if ((NATNUMBER)c==0) return c;
  return nr2mNegM(c);
}

number nr2mMapMachineInt(number from)
{
  NATNUMBER i = ((NATNUMBER) from) & currRing->nr2mModul;
  return (number) i;
}

number nr2mMapZp(number from)
{
  long ii = (long)from;
  NATNUMBER j = (NATNUMBER)1;
  if (ii < 0) { j = currRing->nr2mModul; ii = -ii; }
  NATNUMBER i = (NATNUMBER)ii;
  i = i & currRing->nr2mModul;
  /* now we have: from = j * i mod 2^m */
  return (number)nr2mMult((number)i, (number)j);
}

number nr2mMapQ(number from)
{
  int_number erg = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init(erg);
  int_number k = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(k, currRing->nr2mModul);

  nlGMP(from, (number)erg);
  mpz_and(erg, erg, k);
  number r = (number)mpz_get_ui(erg);

  mpz_clear(erg); omFree((ADDRESS)erg);
  mpz_clear(k);   omFree((ADDRESS)k);

  return (number) r;
}

number nr2mMapGMP(number from)
{
  int_number erg = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init(erg);
  int_number k = (int_number) omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(k, currRing->nr2mModul);

  mpz_and(erg, (int_number)from, k);
  number r = (number) mpz_get_ui(erg);

  mpz_clear(erg); omFree((ADDRESS)erg);
  mpz_clear(k);   omFree((ADDRESS)k);

  return (number) r;
}

nMapFunc nr2mSetMap(const ring src, const ring dst)
{
  if (rField_is_Ring_2toM(src)
     && (src->ringflagb >= dst->ringflagb))
  {
    return nr2mMapMachineInt;
  }
  if (rField_is_Ring_Z(src))
  {
    return nr2mMapGMP;
  }
  if (rField_is_Q(src))
  {
    return nr2mMapQ;
  }
  if (rField_is_Zp(src)
     && (src->ch == 2)
     && (dst->ringflagb == 1))
  {
    return nr2mMapZp;
  }
  if (rField_is_Ring_PtoM(src) || rField_is_Ring_ModN(src))
  {
    // Computing the n of Z/n
    int_number modul = (int_number) omAlloc(sizeof(mpz_t)); // evtl. spaeter mit bin
    mpz_init(modul);
    mpz_set(modul, src->ringflaga);
    mpz_pow_ui(modul, modul, src->ringflagb);
    if (mpz_divisible_2exp_p(modul, dst->ringflagb))
    {
      mpz_clear(modul);
      omFree((ADDRESS) modul);
      return nr2mMapGMP;
    }
    mpz_clear(modul);
    omFree((ADDRESS) modul);
  }
  return NULL;      // default
}

/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nr2mSetExp(int m, const ring r)
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

void nr2mInitExp(int m, const ring r)
{
  nr2mSetExp(m, r);
  if (m < 2) WarnS("nr2mInitExp failed: we go on with Z/2^2");
}

#ifdef LDEBUG
BOOLEAN nr2mDBTest (number a, const char *f, const int l)
{
  if ((NATNUMBER)a < 0) return FALSE;
  if (((NATNUMBER)a & currRing->nr2mModul) != (NATNUMBER)a) return FALSE;
  return TRUE;
}
#endif

void nr2mWrite (number &a, const ring r)
{
  int i = nr2mInt(a, r);
  StringAppend("%d", i);
}

static const char* nr2mEati(const char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) & currRing->nr2mModul;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    (*i) = (*i) & currRing->nr2mModul;
  }
  else (*i) = 1;
  return s;
}

const char * nr2mRead (const char *s, number *a)
{
  int z;
  int n=1;

  s = nr2mEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = nr2mEati(s, &n);
  }
  if (n == 1)
    *a = (number)z;
  else
      *a = nr2mDiv((number)z,(number)n);
  return s;
}
#endif
