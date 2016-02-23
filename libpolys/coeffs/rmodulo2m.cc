/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo 2^m
*/
#include <misc/auxiliary.h>

#include <omalloc/omalloc.h>

#include <misc/mylimits.h>
#include <reporter/reporter.h>

#include "si_gmp.h"
#include "coeffs.h"
#include "numbers.h"
#include "longrat.h"
#include "mpr_complex.h"

#include "rmodulo2m.h"
#include "rmodulon.h"

#include <string.h>

#ifdef HAVE_RINGS

/// Our Type!
static const n_coeffType ID = n_Z2m;

number  nr2mCopy        (number a, const coeffs r);
BOOLEAN nr2mGreaterZero (number k, const coeffs r);
number  nr2mMult        (number a, number b, const coeffs r);
number  nr2mInit        (long i, const coeffs r);
long    nr2mInt         (number &n, const coeffs r);
number  nr2mAdd         (number a, number b, const coeffs r);
number  nr2mSub         (number a, number b, const coeffs r);
void    nr2mPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nr2mIsZero      (number a, const coeffs r);
BOOLEAN nr2mIsOne       (number a, const coeffs r);
BOOLEAN nr2mIsMOne      (number a, const coeffs r);
BOOLEAN nr2mIsUnit      (number a, const coeffs r);
number  nr2mGetUnit     (number a, const coeffs r);
number  nr2mDiv         (number a, number b, const coeffs r);
number  nr2mIntDiv      (number a,number b, const coeffs r);
number  nr2mMod         (number a,number b, const coeffs r);
number  nr2mNeg         (number c, const coeffs r);
number  nr2mInvers      (number c, const coeffs r);
BOOLEAN nr2mGreater     (number a, number b, const coeffs r);
BOOLEAN nr2mDivBy       (number a, number b, const coeffs r);
int     nr2mDivComp     (number a, number b, const coeffs r);
BOOLEAN nr2mEqual       (number a, number b, const coeffs r);
number  nr2mLcm         (number a, number b, const coeffs r);
number  nr2mGcd         (number a, number b, const coeffs r);
number  nr2mExtGcd      (number a, number b, number *s, number *t, const coeffs r);
nMapFunc nr2mSetMap     (const coeffs src, const coeffs dst);
void    nr2mWrite       (number a, const coeffs r);
const char *  nr2mRead  (const char *s, number *a, const coeffs r);
char *  nr2mName        (number n, const coeffs r);
void    nr2mCoeffWrite  (const coeffs r, BOOLEAN details);
coeffs  nr2mQuot1(number c, const coeffs r);
#ifdef LDEBUG
BOOLEAN nr2mDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nr2mSetExp(int c, const coeffs r);
void    nr2mInitExp(int c, const coeffs r);

number nr2mMapQ(number from, const coeffs src, const coeffs dst);

static inline number nr2mMultM(number a, number b, const coeffs r)
{
  return (number)
    ((((unsigned long) a) * ((unsigned long) b)) & ((unsigned long)r->mod2mMask));
}

static inline number nr2mAddM(number a, number b, const coeffs r)
{
  return (number)
    ((((unsigned long) a) + ((unsigned long) b)) & ((unsigned long)r->mod2mMask));
}

static inline number nr2mSubM(number a, number b, const coeffs r)
{
  return (number)((unsigned long)a < (unsigned long)b ?
                       r->mod2mMask - (unsigned long)b + (unsigned long)a + 1:
                       (unsigned long)a - (unsigned long)b);
}

#define nr2mNegM(A,r) (number)((r->mod2mMask - (unsigned long)(A) + 1) & r->mod2mMask)
#define nr2mEqualM(A,B)  ((A)==(B))

extern omBin gmp_nrz_bin; /* init in rintegers*/

void    nr2mCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  PrintS("//   coeff. ring is : ");
  Print("Z/2^%lu\n", r->modExponent);
}

BOOLEAN nr2mCoeffIsEqual(const coeffs r, n_coeffType n, void * p)
{
  if (n==n_Z2m)
  {
    int m=(int)(long)(p);
    unsigned long mm=r->mod2mMask;
    if (((mm+1)>>m)==1L) return TRUE;
  }
  return FALSE;
}

static char* nr2mCoeffString(const coeffs r)
{
  char* s = (char*) omAlloc(11+11);
  sprintf(s,"integer,2,%lu",r->modExponent);
  return s;
}

coeffs nr2mQuot1(number c, const coeffs r)
{
    coeffs rr;
    long ch = r->cfInt(c, r);
    mpz_t a,b;
    mpz_init_set(a, r->modNumber);
    mpz_init_set_ui(b, ch);
    mpz_ptr gcd;
    gcd = (mpz_ptr) omAlloc(sizeof(mpz_t));
    mpz_init(gcd);
    mpz_gcd(gcd, a,b);
    if(mpz_cmp_ui(gcd, 1) == 0)
        {
            WerrorS("constant in q-ideal is coprime to modulus in ground ring");
            WerrorS("Unable to create qring!");
            return NULL;
        }
    if(mpz_cmp_ui(gcd, 2) == 0)
    {
        rr = nInitChar(n_Zp, (void*)2);
    }
    else
    {
        ZnmInfo info;
        info.base = r->modBase;
        int kNew = 1;
        mpz_t baseTokNew;
        mpz_init(baseTokNew);
        mpz_set(baseTokNew, r->modBase);
        while(mpz_cmp(gcd, baseTokNew) > 0)
        {
          kNew++;
          mpz_mul(baseTokNew, baseTokNew, r->modBase);
        }
        info.exp = kNew;
        mpz_clear(baseTokNew);
        rr = nInitChar(n_Z2m, (void*)(long)kNew);
    }
    return(rr);
}

static number nr2mAnn(number b, const coeffs r);
/* for initializing function pointers */
BOOLEAN nr2mInitChar (coeffs r, void* p)
{
  assume( getCoeffType(r) == ID );
  nr2mInitExp((int)(long)(p), r);

  r->is_field=FALSE;
  r->is_domain=FALSE;
  r->rep=n_rep_int;

  //r->cfKillChar    = ndKillChar; /* dummy*/
  r->nCoeffIsEqual = nr2mCoeffIsEqual;
  r->cfCoeffString = nr2mCoeffString;

  r->modBase = (mpz_ptr) omAllocBin (gmp_nrz_bin);
  mpz_init_set_si (r->modBase, 2L);
  r->modNumber= (mpz_ptr) omAllocBin (gmp_nrz_bin);
  mpz_init (r->modNumber);
  mpz_pow_ui (r->modNumber, r->modBase, r->modExponent);

  /* next cast may yield an overflow as mod2mMask is an unsigned long */
  r->ch = (int)r->mod2mMask + 1;

  r->cfInit        = nr2mInit;
  //r->cfCopy        = ndCopy;
  r->cfInt         = nr2mInt;
  r->cfAdd         = nr2mAdd;
  r->cfSub         = nr2mSub;
  r->cfMult        = nr2mMult;
  r->cfDiv         = nr2mDiv;
  r->cfAnn         = nr2mAnn;
  r->cfIntMod      = nr2mMod;
  r->cfExactDiv    = nr2mDiv;
  r->cfInpNeg         = nr2mNeg;
  r->cfInvers      = nr2mInvers;
  r->cfDivBy       = nr2mDivBy;
  r->cfDivComp     = nr2mDivComp;
  r->cfGreater     = nr2mGreater;
  r->cfEqual       = nr2mEqual;
  r->cfIsZero      = nr2mIsZero;
  r->cfIsOne       = nr2mIsOne;
  r->cfIsMOne      = nr2mIsMOne;
  r->cfGreaterZero = nr2mGreaterZero;
  r->cfWriteLong       = nr2mWrite;
  r->cfRead        = nr2mRead;
  r->cfPower       = nr2mPower;
  r->cfSetMap      = nr2mSetMap;
//  r->cfNormalize   = ndNormalize; // default
  r->cfLcm         = nr2mLcm;
  r->cfGcd         = nr2mGcd;
  r->cfIsUnit      = nr2mIsUnit;
  r->cfGetUnit     = nr2mGetUnit;
  r->cfExtGcd      = nr2mExtGcd;
  r->cfCoeffWrite  = nr2mCoeffWrite;
  r->cfQuot1       = nr2mQuot1;
#ifdef LDEBUG
  r->cfDBTest      = nr2mDBTest;
#endif
  r->has_simple_Alloc=TRUE;
  return FALSE;
}

/*
 * Multiply two numbers
 */
number nr2mMult(number a, number b, const coeffs r)
{
  if (((unsigned long)a == 0) || ((unsigned long)b == 0))
    return (number)0;
  else
    return nr2mMultM(a, b, r);
}

/*
 * Give the smallest k, such that a * x = k = b * y has a solution
 */
number nr2mLcm(number a, number b, const coeffs)
{
  unsigned long res = 0;
  if ((unsigned long)a == 0) a = (number) 1;
  if ((unsigned long)b == 0) b = (number) 1;
  while ((unsigned long)a % 2 == 0)
  {
    a = (number)((unsigned long)a / 2);
    if ((unsigned long)b % 2 == 0) b = (number)((unsigned long)b / 2);
    res++;
  }
  while ((unsigned long)b % 2 == 0)
  {
    b = (number)((unsigned long)b / 2);
    res++;
  }
  return (number)(1L << res);  // (2**res)
}

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution.
 */
number nr2mGcd(number a, number b, const coeffs)
{
  unsigned long res = 0;
  if ((unsigned long)a == 0 && (unsigned long)b == 0) return (number)1;
  while ((unsigned long)a % 2 == 0 && (unsigned long)b % 2 == 0)
  {
    a = (number)((unsigned long)a / 2);
    b = (number)((unsigned long)b / 2);
    res++;
  }
//  if ((unsigned long)b % 2 == 0)
//  {
//    return (number)((1L << res)); // * (unsigned long) a);  // (2**res)*a    a is a unit
//  }
//  else
//  {
    return (number)((1L << res)); // * (unsigned long) b);  // (2**res)*b    b is a unit
//  }
}

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution.
 */
number nr2mExtGcd(number a, number b, number *s, number *t, const coeffs r)
{
  unsigned long res = 0;
  if ((unsigned long)a == 0 && (unsigned long)b == 0) return (number)1;
  while ((unsigned long)a % 2 == 0 && (unsigned long)b % 2 == 0)
  {
    a = (number)((unsigned long)a / 2);
    b = (number)((unsigned long)b / 2);
    res++;
  }
  if ((unsigned long)b % 2 == 0)
  {
    *t = NULL;
    *s = nr2mInvers(a,r);
    return (number)((1L << res)); // * (unsigned long) a);  // (2**res)*a    a is a unit
  }
  else
  {
    *s = NULL;
    *t = nr2mInvers(b,r);
    return (number)((1L << res)); // * (unsigned long) b);  // (2**res)*b    b is a unit
  }
}

void nr2mPower(number a, int i, number * result, const coeffs r)
{
  if (i == 0)
  {
    *(unsigned long *)result = 1;
  }
  else if (i == 1)
  {
    *result = a;
  }
  else
  {
    nr2mPower(a, i-1, result, r);
    *result = nr2mMultM(a, *result, r);
  }
}

/*
 * create a number from int
 */
number nr2mInit(long i, const coeffs r)
{
  if (i == 0) return (number)(unsigned long)i;

  long ii = i;
  unsigned long j = (unsigned long)1;
  if (ii < 0) { j = r->mod2mMask; ii = -ii; }
  unsigned long k = (unsigned long)ii;
  k = k & r->mod2mMask;
  /* now we have: i = j * k mod 2^m */
  return (number)nr2mMult((number)j, (number)k, r);
}

/*
 * convert a number to an int in ]-k/2 .. k/2],
 * where k = 2^m; i.e., an int in ]-2^(m-1) .. 2^(m-1)];
 */
long nr2mInt(number &n, const coeffs r)
{
  unsigned long nn = (unsigned long)(unsigned long)n & r->mod2mMask;
  unsigned long l = r->mod2mMask >> 1; l++; /* now: l = 2^(m-1) */
  if ((unsigned long)nn > l)
    return (long)((unsigned long)nn - r->mod2mMask - 1);
  else
    return (long)((unsigned long)nn);
}

number nr2mAdd(number a, number b, const coeffs r)
{
  return nr2mAddM(a, b, r);
}

number nr2mSub(number a, number b, const coeffs r)
{
  return nr2mSubM(a, b, r);
}

BOOLEAN nr2mIsUnit(number a, const coeffs)
{
  return ((unsigned long)a % 2 == 1);
}

number nr2mGetUnit(number k, const coeffs)
{
  if (k == NULL) return (number)1;
  unsigned long erg = (unsigned long)k;
  while (erg % 2 == 0) erg = erg / 2;
  return (number)erg;
}

BOOLEAN nr2mIsZero(number a, const coeffs)
{
  return 0 == (unsigned long)a;
}

BOOLEAN nr2mIsOne(number a, const coeffs)
{
  return 1 == (unsigned long)a;
}

BOOLEAN nr2mIsMOne(number a, const coeffs r)
{
  return (r->mod2mMask  == (unsigned long)a);
}

BOOLEAN nr2mEqual(number a, number b, const coeffs)
{
  return (a == b);
}

BOOLEAN nr2mGreater(number a, number b, const coeffs r)
{
  return nr2mDivBy(a, b,r);
}

/* Is 'a' divisible by 'b'? There are two cases:
   1) a = 0 mod 2^m; then TRUE iff b = 0 or b is a power of 2
   2) a, b <> 0; then TRUE iff b/gcd(a, b) is a unit mod 2^m */
BOOLEAN nr2mDivBy (number a, number b, const coeffs r)
{
  if (a == NULL)
  {
    unsigned long c = r->mod2mMask + 1;
    if (c != 0) /* i.e., if no overflow */
      return (c % (unsigned long)b) == 0;
    else
    {
      /* overflow: we need to check whether b
         is zero or a power of 2: */
      c = (unsigned long)b;
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

int nr2mDivComp(number as, number bs, const coeffs)
{
  unsigned long a = (unsigned long)as;
  unsigned long b = (unsigned long)bs;
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
BOOLEAN nr2mGreaterZero(number k, const coeffs r)
{
  if ((unsigned long)k == 0) return FALSE;
  if ((unsigned long)k > ((r->mod2mMask >> 1) + 1)) return FALSE;
  return TRUE;
}

/* assumes that 'a' is odd, i.e., a unit in Z/2^m, and computes
   the extended gcd of 'a' and 2^m, in order to find some 's'
   and 't' such that a * s + 2^m * t = gcd(a, 2^m) = 1;
   this code will always find a positive 's' */
void specialXGCD(unsigned long& s, unsigned long a, const coeffs r)
{
  mpz_ptr u = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(u, a);
  mpz_ptr u0 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(u0);
  mpz_ptr u1 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(u1, 1);
  mpz_ptr u2 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(u2);
  mpz_ptr v = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(v, r->mod2mMask);
  mpz_add_ui(v, v, 1); /* now: v = 2^m */
  mpz_ptr v0 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(v0);
  mpz_ptr v1 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(v1);
  mpz_ptr v2 = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(v2, 1);
  mpz_ptr q = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(q);
  mpz_ptr rr = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init(rr);

  while (mpz_cmp_ui(v, 0) != 0) /* i.e., while v != 0 */
  {
    mpz_div(q, u, v);
    mpz_mod(rr, u, v);
    mpz_set(u, v);
    mpz_set(v, rr);
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
    mpz_add_ui(u1, u1, r->mod2mMask);
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
  mpz_clear(rr); omFree((ADDRESS)rr);
}

unsigned long InvMod(unsigned long a, const coeffs r)
{
  assume((unsigned long)a % 2 != 0);
  unsigned long s;
  specialXGCD(s, a, r);
  return s;
}
//#endif

inline number nr2mInversM(number c, const coeffs r)
{
  assume((unsigned long)c % 2 != 0);
  // Table !!!
  unsigned long inv;
  inv = InvMod((unsigned long)c,r);
  return (number)inv;
}

number nr2mDiv(number a, number b, const coeffs r)
{
  if ((unsigned long)a == 0) return (number)0;
  else if ((unsigned long)b % 2 == 0)
  {
    if ((unsigned long)b != 0)
    {
      while (((unsigned long)b % 2 == 0) && ((unsigned long)a % 2 == 0))
      {
        a = (number)((unsigned long)a / 2);
        b = (number)((unsigned long)b / 2);
      }
    }
    if ((unsigned long)b % 2 == 0)
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      WerrorS("Result is integer division without remainder.");
      return (number) ((unsigned long) a / (unsigned long) b);
    }
  }
  return (number)nr2mMult(a, nr2mInversM(b,r),r);
}

number nr2mMod(number a, number b, const coeffs r)
{
  /*
    We need to return the number rr which is uniquely determined by the
    following two properties:
      (1) 0 <= rr < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + rr.
    Consider g := gcd(2^m, |b|). Note that then |b|/g is a unit in Z/2^m.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/2^m, i.e. |b| (and also b) divides a.
          Thus rr = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again rr = 0.
      (c) g <> 1 and g does not divide a
          Let's denote the division with remainder of a by g as follows:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. rr := t is the correct result. Hence
          in this third case, rr is the remainder of division of a by g in Z.
    This algorithm is the same as for the case Z/n, except that we may
    compute the gcd of |b| and 2^m "by hand": We just extract the highest
    power of 2 (<= 2^m) that is contained in b.
  */
  assume((unsigned long) b != 0);
  unsigned long g = 1;
  unsigned long b_div = (unsigned long) b;

  /*
   * b_div is unsigned, so that (b_div < 0) evaluates false at compile-time
   *
  if (b_div < 0) b_div = -b_div; // b_div now represents |b|, BUT b_div is unsigned!
  */

  unsigned long rr = 0;
  while ((g < r->mod2mMask ) && (b_div > 0) && (b_div % 2 == 0))
  {
    b_div = b_div >> 1;
    g = g << 1;
  } // g is now the gcd of 2^m and |b|

  if (g != 1) rr = (unsigned long)a % g;
  return (number)rr;
}

number nr2mIntDiv(number a, number b, const coeffs r)
{
  if ((unsigned long)a == 0)
  {
    if ((unsigned long)b == 0)
      return (number)1;
    if ((unsigned long)b == 1)
      return (number)0;
    unsigned long c = r->mod2mMask + 1;
    if (c != 0) /* i.e., if no overflow */
      return (number)(c / (unsigned long)b);
    else
    {
      /* overflow: c = 2^32 resp. 2^64, depending on platform */
      mpz_ptr cc = (mpz_ptr)omAlloc(sizeof(mpz_t));
      mpz_init_set_ui(cc, r->mod2mMask); mpz_add_ui(cc, cc, 1);
      mpz_div_ui(cc, cc, (unsigned long)(unsigned long)b);
      unsigned long s = mpz_get_ui(cc);
      mpz_clear(cc); omFree((ADDRESS)cc);
      return (number)(unsigned long)s;
    }
  }
  else
  {
    if ((unsigned long)b == 0)
      return (number)0;
    return (number)((unsigned long) a / (unsigned long) b);
  }
}

static number nr2mAnn(number b, const coeffs r)
{
  if ((unsigned long)b == 0)
    return NULL;
  if ((unsigned long)b == 1)
    return NULL;
  unsigned long c = r->mod2mMask + 1;
  if (c != 0) /* i.e., if no overflow */
    return (number)(c / (unsigned long)b);
  else
  {
    /* overflow: c = 2^32 resp. 2^64, depending on platform */
    mpz_ptr cc = (mpz_ptr)omAlloc(sizeof(mpz_t));
    mpz_init_set_ui(cc, r->mod2mMask); mpz_add_ui(cc, cc, 1);
    mpz_div_ui(cc, cc, (unsigned long)(unsigned long)b);
    unsigned long s = mpz_get_ui(cc);
    mpz_clear(cc); omFree((ADDRESS)cc);
    return (number)(unsigned long)s;
  }
}

number nr2mInvers(number c, const coeffs r)
{
  if ((unsigned long)c % 2 == 0)
  {
    WerrorS("division by zero divisor");
    return (number)0;
  }
  return nr2mInversM(c, r);
}

number nr2mNeg(number c, const coeffs r)
{
  if ((unsigned long)c == 0) return c;
  return nr2mNegM(c, r);
}

number nr2mMapMachineInt(number from, const coeffs /*src*/, const coeffs dst)
{
  unsigned long i = ((unsigned long)from) % dst->mod2mMask ;
  return (number)i;
}

number nr2mMapProject(number from, const coeffs /*src*/, const coeffs dst)
{
  unsigned long i = ((unsigned long)from) % (dst->mod2mMask + 1);
  return (number)i;
}

number nr2mMapZp(number from, const coeffs /*src*/, const coeffs dst)
{
  unsigned long j = (unsigned long)1;
  long ii = (long)from;
  if (ii < 0) { j = dst->mod2mMask; ii = -ii; }
  unsigned long i = (unsigned long)ii;
  i = i & dst->mod2mMask;
  /* now we have: from = j * i mod 2^m */
  return (number)nr2mMult((number)i, (number)j, dst);
}

number nr2mMapGMP(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_ptr k = (mpz_ptr)omAlloc(sizeof(mpz_t));
  mpz_init_set_ui(k, dst->mod2mMask);

  mpz_and(erg, (mpz_ptr)from, k);
  number res = (number) mpz_get_ui(erg);

  mpz_clear(erg); omFree((ADDRESS)erg);
  mpz_clear(k);   omFree((ADDRESS)k);

  return (number)res;
}

number nr2mMapQ(number from, const coeffs src, const coeffs dst)
{
  mpz_ptr gmp = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(gmp);
  nlGMP(from, (number)gmp, src); // FIXME? TODO? // extern void   nlGMP(number &i, number n, const coeffs r); // to be replaced with n_MPZ(erg, from, src); // ?
  number res=nr2mMapGMP((number)gmp,src,dst);
  mpz_clear(gmp); omFree((ADDRESS)gmp);
  return res;
}

number nr2mMapZ(number from, const coeffs src, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
  {
    long f_i=SR_TO_INT(from);
    return nr2mInit(f_i,dst);
  }
  return nr2mMapGMP(from,src,dst);
}

nMapFunc nr2mSetMap(const coeffs src, const coeffs dst)
{
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src)
     && (src->mod2mMask == dst->mod2mMask))
  {
    return ndCopyMap;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src)
     && (src->mod2mMask < dst->mod2mMask))
  { /* i.e. map an integer mod 2^s into Z mod 2^t, where t < s */
    return nr2mMapMachineInt;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src)
     && (src->mod2mMask > dst->mod2mMask))
  { /* i.e. map an integer mod 2^s into Z mod 2^t, where t > s */
    // to be done
    return nr2mMapProject;
  }
  if ((src->rep==n_rep_gmp) && nCoeff_is_Ring_Z(src))
  {
    return nr2mMapGMP;
  }
  if ((src->rep==n_rep_gap_gmp) /*&& nCoeff_is_Ring_Z(src)*/)
  {
    return nr2mMapZ;
  }
  if ((src->rep==n_rep_gap_rat) && (nCoeff_is_Q(src)||nCoeff_is_Ring_Z(src)))
  {
    return nr2mMapQ;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src) && (src->ch == 2))
  {
    return nr2mMapZp;
  }
  if ((src->rep==n_rep_gmp) &&
  (nCoeff_is_Ring_PtoM(src) || nCoeff_is_Ring_ModN(src)))
  {
    if (mpz_divisible_2exp_p(src->modNumber,dst->modExponent))
      return nr2mMapGMP;
  }
  return NULL;      // default
}

/*
 * set the exponent
 */

void nr2mSetExp(int m, coeffs r)
{
  if (m > 1)
  {
    /* we want mod2mMask to be the bit pattern
       '111..1' consisting of m one's: */
    r->modExponent= m;
    r->mod2mMask = 1;
    for (int i = 1; i < m; i++) r->mod2mMask = (r->mod2mMask << 1) + 1;
  }
  else
  {
    r->modExponent= 2;
    /* code unexpectedly called with m = 1; we continue with m = 2: */
    r->mod2mMask = 3; /* i.e., '11' in binary representation */
  }
}

void nr2mInitExp(int m, coeffs r)
{
  nr2mSetExp(m, r);
  if (m < 2)
    WarnS("nr2mInitExp unexpectedly called with m = 1 (we continue with Z/2^2");
}

#ifdef LDEBUG
BOOLEAN nr2mDBTest (number a, const char *, const int, const coeffs r)
{
  //if ((unsigned long)a < 0) return FALSE; // is unsigned!
  if (((unsigned long)a & r->mod2mMask) != (unsigned long)a) return FALSE;
  return TRUE;
}
#endif

void nr2mWrite (number a, const coeffs r)
{
  long i = nr2mInt(a, r);
  StringAppend("%ld", i);
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
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) & r->mod2mMask;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    (*i) = (*i) & r->mod2mMask;
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
    *a = (number)(long)z;
  else
      *a = nr2mDiv((number)(long)z,(number)(long)n,r);
  return s;
}
#endif
/* #ifdef HAVE_RINGS */
