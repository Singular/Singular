/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo n
*/

#include "config.h"
#include <misc/auxiliary.h>

#ifdef HAVE_RINGS

#include <misc/mylimits.h>
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/mpr_complex.h>
#include <coeffs/rmodulon.h>
#include <coeffs/si_gmp.h>

#include <string.h>

extern omBin gmp_nrz_bin;

void    nrnCoeffWrite  (const coeffs r)
{
  long l = (long)mpz_sizeinbase(r->modBase, 10) + 2;
  char* s = (char*) omAlloc(l);
  if (nCoeff_is_Ring_ModN(r)) Print("//  Z/%s\n", s);
  else if (nCoeff_is_Ring_PtoM(r)) Print("//  Z/%s^%lu\n", s, r->modExponent);
  omFreeSize((ADDRESS)s, l);
}

static BOOLEAN nrnCoeffsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  return (n==n_Zn) && (mpz_cmp(r->modNumber,(mpz_ptr)parameter)==0);
}


/* for initializing function pointers */
BOOLEAN nrnInitChar (coeffs r, void* p)
{
  
  nrnInitExp((int)(long)(p), r);

  r->cfInit        = nrnInit;
  r->cfDelete      = nrnDelete;
  r->cfCopy        = nrnCopy;
  r->cfSize        = nrnSize;
  r->cfInt         = nrnInt;
  r->cfAdd         = nrnAdd;
  r->cfSub         = nrnSub;
  r->cfMult        = nrnMult;
  r->cfDiv         = nrnDiv;
  r->cfIntDiv      = nrnIntDiv;
  r->cfIntMod      = nrnMod;
  r->cfExactDiv    = nrnDiv;
  r->cfNeg         = nrnNeg;
  r->cfInvers      = nrnInvers;
  r->cfDivBy       = nrnDivBy;
  r->cfDivComp     = nrnDivComp;
  r->cfGreater     = nrnGreater;
  r->cfEqual       = nrnEqual;
  r->cfIsZero      = nrnIsZero;
  r->cfIsOne       = nrnIsOne;
  r->cfIsMOne      = nrnIsMOne;
  r->cfGreaterZero = nrnGreaterZero;
  r->cfWrite       = nrnWrite;
  r->cfRead        = nrnRead;
  r->cfPower       = nrnPower;
  r->cfSetMap      = nrnSetMap;
  r->cfNormalize   = ndNormalize;
  r->cfLcm         = nrnLcm;
  r->cfGcd         = nrnGcd;
  r->cfIsUnit      = nrnIsUnit;
  r->cfGetUnit     = nrnGetUnit;
  r->cfExtGcd      = nrnExtGcd;
  r->cfName        = ndName;
  r->cfCoeffWrite  = nrnCoeffWrite;
  r->nCoeffIsEqual = nrnCoeffsEqual;
#ifdef LDEBUG
  r->cfDBTest      = nrnDBTest;
#endif
  return FALSE;
}

/*
 * create a number from int
 */
number nrnInit(int i, const coeffs r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

void nrnDelete(number *a, const coeffs r)
{
  if (*a == NULL) return;
  mpz_clear((int_number) *a);
  omFreeBin((void *) *a, gmp_nrz_bin);
  *a = NULL;
}

number nrnCopy(number a, const coeffs r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (int_number) a);
  return (number) erg;
}

int nrnSize(number a, const coeffs r)
{
  if (a == NULL) return 0;
  return sizeof(mpz_t);
}

/*
 * convert a number to int
 */
int nrnInt(number &n, const coeffs r)
{
  return (int)mpz_get_si((int_number) n);
}

/*
 * Multiply two numbers
 */
number nrnMult(number a, number b, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (int_number)a, (int_number) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

void nrnPower(number a, int i, number * result, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_powm_ui(erg, (int_number)a, i, r->modNumber);
  *result = (number) erg;
}

number nrnAdd(number a, number b, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (int_number)a, (int_number) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

number nrnSub(number a, number b, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (int_number)a, (int_number) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

number nrnNeg(number c, const coeffs r)
{
  if( !nrnIsZero(c, r) )
    // Attention: This method operates in-place.
    mpz_sub((int_number)c, r->modNumber, (int_number)c);  
  return c;
}

number nrnInvers(number c, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_invert(erg, (int_number)c, r->modNumber);
  return (number) erg;
}

/*
 * Give the smallest k, such that a * x = k = b * y has a solution
 * TODO: lcm(gcd,gcd) better than gcd(lcm) ?
 */
number nrnLcm(number a, number b, const coeffs r)
{
  number erg = nrnGcd(NULL, a, r);
  number tmp = nrnGcd(NULL, b, r);
  mpz_lcm((int_number)erg, (int_number)erg, (int_number)tmp);
  nrnDelete(&tmp, NULL);
  return (number)erg;
}

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrnGcd(number a, number b, const coeffs r)
{
  if ((a == NULL) && (b == NULL)) return nrnInit(0,r);
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, r->modNumber);
  if (a != NULL) mpz_gcd(erg, erg, (int_number)a);
  if (b != NULL) mpz_gcd(erg, erg, (int_number)b);
  return (number)erg;
}

/* Not needed any more, but may have room for improvement
   number nrnGcd3(number a,number b, number c,ring r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number)r->modNumber;
  if (b == NULL) b = (number)r->modNumber;
  if (c == NULL) c = (number)r->modNumber;
  mpz_gcd(erg, (int_number)a, (int_number)b);
  mpz_gcd(erg, erg, (int_number)c);
  mpz_gcd(erg, erg, r->modNumber);
  return (number)erg;
}
*/

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
number nrnExtGcd(number a, number b, number *s, number *t, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  int_number bs  = (int_number)omAllocBin(gmp_nrz_bin);
  int_number bt  = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (int_number)a, (int_number)b);
  mpz_mod(bs, bs, r->modNumber);
  mpz_mod(bt, bt, r->modNumber);
  *s = (number)bs;
  *t = (number)bt;
  return (number)erg;
}

BOOLEAN nrnIsZero(number a, const coeffs r)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmpabs_ui((int_number)a, 0);
}

BOOLEAN nrnIsOne(number a, const coeffs r)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmp_si((int_number)a, 1);
}

BOOLEAN nrnIsMOne(number a, const coeffs r)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  mpz_t t; mpz_init_set(t, (int_number)a);
  mpz_add_ui(t, t, 1);
  bool erg = (0 == mpz_cmp(t, r->modNumber));
  mpz_clear(t);
  return erg;
}

BOOLEAN nrnEqual(number a, number b, const coeffs r)
{
  return 0 == mpz_cmp((int_number)a, (int_number)b);
}

BOOLEAN nrnGreater(number a, number b, const coeffs r)
{
  return 0 < mpz_cmp((int_number)a, (int_number)b);
}

BOOLEAN nrnGreaterZero(number k, const coeffs r)
{
  return 0 < mpz_cmp_si((int_number)k, 0);
}

BOOLEAN nrnIsUnit(number a, const coeffs r)
{
  number tmp = nrnGcd(a, (number)r->modNumber, r);
  bool res = nrnIsOne(tmp, r);
  nrnDelete(&tmp, NULL);
  return res;
}

number nrnGetUnit(number k, const coeffs r)
{
  if (mpz_divisible_p(r->modNumber, (int_number)k)) return nrnInit(1,r);

  int_number unit = (int_number)nrnGcd(k, 0, r);
  mpz_tdiv_q(unit, (int_number)k, unit);
  int_number gcd = (int_number)nrnGcd((number)unit, 0, r);
  if (!nrnIsOne((number)gcd,r))
  {
    int_number ctmp;
    // tmp := unit^2
    int_number tmp = (int_number) nrnMult((number) unit,(number) unit,r);
    // gcd_new := gcd(tmp, 0)
    int_number gcd_new = (int_number) nrnGcd((number) tmp, 0, r);
    while (!nrnEqual((number) gcd_new,(number) gcd,r))
    {
      // gcd := gcd_new
      ctmp = gcd;
      gcd = gcd_new;
      gcd_new = ctmp;
      // tmp := tmp * unit
      mpz_mul(tmp, tmp, unit);
      mpz_mod(tmp, tmp, r->modNumber);
      // gcd_new := gcd(tmp, 0)
      mpz_gcd(gcd_new, tmp, r->modNumber);
    }
    // unit := unit + modNumber / gcd_new
    mpz_tdiv_q(tmp, r->modNumber, gcd_new);
    mpz_add(unit, unit, tmp);
    mpz_mod(unit, unit, r->modNumber);
    nrnDelete((number*) &gcd_new, NULL);
    nrnDelete((number*) &tmp, NULL);
  }
  nrnDelete((number*) &gcd, NULL);
  return (number)unit;
}

BOOLEAN nrnDivBy(number a, number b, const coeffs r)
{
  if (a == NULL)
    return mpz_divisible_p(r->modNumber, (int_number)b);
  else
  { /* b divides a iff b/gcd(a, b) is a unit in the given ring: */
    number n = nrnGcd(a, b, r);
    mpz_tdiv_q((int_number)n, (int_number)b, (int_number)n);
    bool result = nrnIsUnit(n, r);
    nrnDelete(&n, NULL);
    return result;
  }
}

int nrnDivComp(number a, number b, const coeffs r)
{
  if (nrnEqual(a, b,r)) return 2;
  if (mpz_divisible_p((int_number) a, (int_number) b)) return -1;
  if (mpz_divisible_p((int_number) b, (int_number) a)) return 1;
  return 0;
}

number nrnDiv(number a, number b, const coeffs r)
{
  if (a == NULL) a = (number)r->modNumber;
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (mpz_divisible_p((int_number)a, (int_number)b))
  {
    mpz_divexact(erg, (int_number)a, (int_number)b);
    return (number)erg;
  }
  else
  {
    int_number gcd = (int_number)nrnGcd(a, b, r);
    mpz_divexact(erg, (int_number)b, gcd);
    if (!nrnIsUnit((number)erg, r))
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      WerrorS("Result is integer division without remainder.");
      mpz_tdiv_q(erg, (int_number) a, (int_number) b);
      nrnDelete((number*) &gcd, NULL);
      return (number)erg;
    }
    // a / gcd(a,b) * [b / gcd (a,b)]^(-1)
    int_number tmp = (int_number)nrnInvers((number) erg,r);
    mpz_divexact(erg, (int_number)a, gcd);
    mpz_mul(erg, erg, tmp);
    nrnDelete((number*) &gcd, NULL);
    nrnDelete((number*) &tmp, NULL);
    mpz_mod(erg, erg, r->modNumber);
    return (number)erg;
  }
}

number nrnMod(number a, number b, const coeffs r)
{
  /*
    We need to return the number rr which is uniquely determined by the
    following two properties:
      (1) 0 <= rr < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + rr.
    Consider g := gcd(n, |b|). Note that then |b|/g is a unit in Z/n.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/n, i.e. |b| (and also b) divides a.
          Thus rr = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again rr = 0.
      (c) g <> 1 and g does not divide a
          Then denote the division with remainder of a by g as this:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. rr := t is the correct result. Hence
          in this third case, rr is the remainder of division of a by g in Z.
     Remark: according to mpz_mod: a,b are always non-negative
  */
  int_number g = (int_number)omAllocBin(gmp_nrz_bin);
  int_number rr = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(g);
  mpz_init_set_si(rr, 0);
  mpz_gcd(g, (int_number)r->modNumber, (int_number)b); // g is now as above
  if (mpz_cmp_si(g, (long)1) != 0) mpz_mod(rr, (int_number)a, g); // the case g <> 1
  mpz_clear(g);
  omFreeBin(g, gmp_nrz_bin);
  return (number)rr;
}

number nrnIntDiv(number a, number b, const coeffs r)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number)r->modNumber;
  mpz_tdiv_q(erg, (int_number)a, (int_number)b);
  return (number)erg;
}

/*
 * Helper function for computing the module
 */

int_number nrnMapCoef = NULL;

number nrnMapModN(number from, const coeffs src, const coeffs dst)
{
  return nrnMult(from, (number) nrnMapCoef, dst);
}

number nrnMap2toM(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul_ui(erg, nrnMapCoef, (NATNUMBER)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

number nrnMapZp(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  // TODO: use npInt(...)
  mpz_mul_si(erg, nrnMapCoef, (NATNUMBER)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

number nrnMapGMP(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mod(erg, (int_number)from, dst->modNumber);
  return (number)erg;
}

number nrnMapQ(number from, const coeffs src, const coeffs dst)
{
  int_number erg = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlGMP(from, (number)erg, src);
  mpz_mod(erg, erg, src->modNumber);
  return (number)erg;
}

nMapFunc nrnSetMap(const coeffs src, const coeffs dst)
{
  /* dst = currRing->cf */
  if (nCoeff_is_Ring_Z(src))
  {
    return nrnMapGMP;
  }
  if (nCoeff_is_Q(src))
  {
    return nrnMapQ;
  }
  // Some type of Z/n ring / field
  if (nCoeff_is_Ring_ModN(src) || nCoeff_is_Ring_PtoM(src) || nCoeff_is_Ring_2toM(src) || nCoeff_is_Zp(src))
  {
    if (   (src->ringtype > 0)
        && (mpz_cmp(src->modBase, dst->modBase) == 0)
        && (src->modExponent == dst->modExponent)) return nrnMapGMP;
    else
    {
      int_number nrnMapModul = (int_number) omAllocBin(gmp_nrz_bin);
      // Computing the n of Z/n
      if (nCoeff_is_Zp(src))
      {
        mpz_init_set_si(nrnMapModul, src->ch);
      }
      else
      {
        mpz_init(nrnMapModul);
        mpz_set(nrnMapModul, src->modNumber);
      }
      // nrnMapCoef = 1 in dst       if dst is a subring of src
      // nrnMapCoef = 0 in dst / src if src is a subring of dst
      if (nrnMapCoef == NULL)
      {
        nrnMapCoef = (int_number) omAllocBin(gmp_nrz_bin);
        mpz_init(nrnMapCoef);
      }
      if (mpz_divisible_p(nrnMapModul, dst->modNumber))
      {
        mpz_set_si(nrnMapCoef, 1);
      }
      else
      if (nrnDivBy(NULL, (number) nrnMapModul,dst))
      {
        mpz_divexact(nrnMapCoef, dst->modNumber, nrnMapModul);
        int_number tmp = dst->modNumber;
        dst->modNumber = nrnMapModul;
        if (!nrnIsUnit((number) nrnMapCoef,dst))
        {
          dst->modNumber = tmp;
          nrnDelete((number*) &nrnMapModul, dst);
          return NULL;
        }
        int_number inv = (int_number) nrnInvers((number) nrnMapCoef,dst);
        dst->modNumber = tmp;
        mpz_mul(nrnMapCoef, nrnMapCoef, inv);
        mpz_mod(nrnMapCoef, nrnMapCoef, dst->modNumber);
        nrnDelete((number*) &inv, dst);
      }
      else
      {
        nrnDelete((number*) &nrnMapModul, dst);
        return NULL;
      }
      nrnDelete((number*) &nrnMapModul, dst);
      if (nCoeff_is_Ring_2toM(src))
        return nrnMap2toM;
      else if (nCoeff_is_Zp(src))
        return nrnMapZp;
      else
        return nrnMapModN;
    }
  }
  return NULL;      // default
}

/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrnSetExp(int m, coeffs r)
{
  /* clean up former stuff */
  if (r->modBase   != NULL) mpz_clear(r->modBase);
  if (r->modNumber != NULL) mpz_clear(r->modNumber);

  /* this is Z/m = Z/(m^1), hence set modBase = m, modExponent = 1: */
  r->modBase = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(r->modBase);
  mpz_set_ui(r->modBase, (unsigned long)m);
  r->modExponent = 1;
  r->modNumber = (int_number)omAllocBin(gmp_nrz_bin);
  mpz_init(r->modNumber);
  mpz_set(r->modNumber, r->modBase);
  /* mpz_pow_ui(r->modNumber, r->modNumber, r->modExponent); */
}

/* We expect this ring to be Z/m for some m > 2 which is not a prime. */
void nrnInitExp(int m, coeffs r)
{
  if (m <= 2) WarnS("nrnInitExp failed (m in Z/m too small)");
  nrnSetExp(m, r);
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, const char *f, const int l, const coeffs r)
{
  if (a==NULL) return TRUE;
  if ( (mpz_cmp_si((int_number) a, 0) < 0) || (mpz_cmp((int_number) a, r->modNumber) > 0) )
  {
    return FALSE;
  }
  return TRUE;
}
#endif

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
static const char * nlCPEatLongC(char *s, mpz_ptr i)
{
  const char * start=s;
  if (!(*s >= '0' && *s <= '9'))
  {
    mpz_init_set_si(i, 1);
    return s;
  }
  mpz_init(i);
  while (*s >= '0' && *s <= '9') s++;
  if (*s=='\0')
  {
    mpz_set_str(i,start,10);
  }
  else
  {
    char c=*s;
    *s='\0';
    mpz_set_str(i,start,10);
    *s=c;
  }
  return s;
}

const char * nrnRead (const char *s, number *a, const coeffs r)
{
  int_number z = (int_number) omAllocBin(gmp_nrz_bin);
  {
    s = nlCPEatLongC((char *)s, z);
  }
  mpz_mod(z, z, r->modNumber);
  *a = (number) z;
  return s;
}
#endif
/* #ifdef HAVE_RINGS */
