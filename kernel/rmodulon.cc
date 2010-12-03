/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo n
*/

#include <string.h>
#include <kernel/mod2.h>
#include <omalloc/mylimits.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/mpr_complex.h>
#include <kernel/ring.h>
#include <kernel/rmodulon.h>
#include <kernel/si_gmp.h>

#ifdef HAVE_RINGS
  extern omBin gmp_nrz_bin;

int_number nrnMinusOne = NULL;
unsigned long nrnExponent = 0;

/*
 * create a number from int
 */
number nrnInit (int i, const ring r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  mpz_mod(erg, erg, r->nrnModul);
  return (number) erg;
}

void nrnDelete(number *a, const ring r)
{
  if (*a == NULL) return;
  mpz_clear((int_number) *a);
  omFreeBin((ADDRESS) *a, gmp_nrz_bin);
  *a = NULL;
}

number nrnCopy(number a)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (int_number) a);
  return (number) erg;
}

number cfrnCopy(number a, const ring r)
{
  return nrnCopy(a);
}

int nrnSize(number a)
{
  if (a == NULL) return 0;
  return sizeof(mpz_t);
}

/*
 * convert a number to int
 */
int nrnInt(number &n, const ring r)
{
  return (int) mpz_get_si( (int_number) n);
}

/*
 * Multiply two numbers
 */
number nrnMult (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

void nrnPower (number a, int i, number * result)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_powm_ui(erg, (int_number) a, i, currRing->nrnModul);
  *result = (number) erg;
}

number nrnAdd (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

number nrnSub (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

number nrnNeg (number c)
{
// nNeg inplace !!!
  mpz_sub((int_number) c, currRing->nrnModul, (int_number) c);
  return c;
}

number  nrnInvers (number c)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_invert(erg, (int_number) c, currRing->nrnModul);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 * TODO: lcm(gcd,gcd) besser als gcd(lcm) ?
 */
number nrnLcm (number a,number b,ring r)
{
  number erg = nrnGcd(NULL, a, r);
  number tmp = nrnGcd(NULL, b, r);
  mpz_lcm((int_number) erg, (int_number) erg, (int_number) tmp);
  nrnDelete(&tmp, NULL);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrnGcd (number a,number b,ring r)
{
  if ((a == NULL) && (b == NULL)) return nrnInit(0,r);
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, r->nrnModul);
  if (a != NULL) mpz_gcd(erg, erg, (int_number) a);
  if (b != NULL) mpz_gcd(erg, erg, (int_number) b);
  return (number) erg;
}

/* Not needed any more, but may have room for improvement
number nrnGcd3 (number a,number b, number c,ring r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number) r->nrnModul;
  if (b == NULL) b = (number) r->nrnModul;
  if (c == NULL) c = (number) r->nrnModul;
  mpz_gcd(erg, (int_number) a, (int_number) b);
  mpz_gcd(erg, erg, (int_number) c);
  mpz_gcd(erg, erg, r->nrnModul);
  return (number) erg;
}
*/

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
number  nrnExtGcd (number a, number b, number *s, number *t)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  int_number bs = (int_number) omAllocBin(gmp_nrz_bin);
  int_number bt = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (int_number) a, (int_number) b);
  mpz_mod(bs, bs, currRing->nrnModul);
  mpz_mod(bt, bt, currRing->nrnModul);
  *s = (number) bs;
  *t = (number) bt;
  return (number) erg;
}

BOOLEAN nrnIsZero (number  a)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmpabs_ui((int_number) a, 0);
}

BOOLEAN nrnIsOne (number a)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmp_si((int_number) a, 1);
}

BOOLEAN nrnIsMOne (number a)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmp((int_number) a, nrnMinusOne);
}

BOOLEAN nrnEqual (number a,number b)
{
  return 0 == mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrnGreater (number a,number b)
{
  return 0 < mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrnGreaterZero (number k)
{
  return 0 < mpz_cmp_si((int_number) k, 0);
}

BOOLEAN nrnIsUnit (number a)
{
  number tmp = nrnGcd(a, (number) currRing->nrnModul, currRing);
  bool res = nrnIsOne(tmp);
  nrnDelete(&tmp, NULL);
  return res;
}

number  nrnGetUnit (number k)
{
  if (mpz_divisible_p(currRing->nrnModul, (int_number) k)) return nrnInit(1,currRing);

  int_number unit = (int_number) nrnGcd(k, 0, currRing);
  mpz_tdiv_q(unit, (int_number) k, unit);
  int_number gcd = (int_number) nrnGcd((number) unit, 0, currRing);
  if (!nrnIsOne((number) gcd))
  {
    int_number ctmp;
    // tmp := unit^2
    int_number tmp = (int_number) nrnMult((number) unit,(number) unit);
    // gcd_new := gcd(tmp, 0)
    int_number gcd_new = (int_number) nrnGcd((number) tmp, 0, currRing);
    while (!nrnEqual((number) gcd_new,(number) gcd))
    {
      // gcd := gcd_new
      ctmp = gcd;
      gcd = gcd_new;
      gcd_new = ctmp;
      // tmp := tmp * unit
      mpz_mul(tmp, tmp, unit);
      mpz_mod(tmp, tmp, currRing->nrnModul);
      // gcd_new := gcd(tmp, 0)
      mpz_gcd(gcd_new, tmp, currRing->nrnModul);
    }
    // unit := unit + nrnModul / gcd_new
    mpz_tdiv_q(tmp, currRing->nrnModul, gcd_new);
    mpz_add(unit, unit, tmp);
    mpz_mod(unit, unit, currRing->nrnModul);
    nrnDelete((number*) &gcd_new, NULL);
    nrnDelete((number*) &tmp, NULL);
  }
  nrnDelete((number*) &gcd, NULL);
  return (number) unit;
}

BOOLEAN nrnDivBy (number a,number b)
{
  if (a == NULL)
    return mpz_divisible_p(currRing->nrnModul, (int_number) b);
  else
    return mpz_divisible_p((int_number) a, (int_number) b);
  /*
  number bs = nrnGcd(a, b, currRing);
  mpz_tdiv_q((int_number) bs, (int_number) b, (int_number) bs);
  bool res = nrnIsUnit(bs);
  nrnDelete(&bs, NULL);
  return res;
  */
}

int nrnDivComp(number a, number b)
{
  if (nrnEqual(a, b)) return 2;
  if (mpz_divisible_p((int_number) a, (int_number) b)) return -1;
  if (mpz_divisible_p((int_number) b, (int_number) a)) return 1;
  return 0;
}

number nrnDiv (number a,number b)
{
  if (a == NULL) a = (number) currRing->nrnModul;
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (mpz_divisible_p((int_number) a, (int_number) b))
  {
    mpz_divexact(erg, (int_number) a, (int_number) b);
    return (number) erg;
  }
  else
  {
    int_number gcd = (int_number) nrnGcd(a, b, currRing);
    mpz_divexact(erg, (int_number) b, gcd);
    if (!nrnIsUnit((number) erg))
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      WerrorS("Result is integer division without remainder.");
      mpz_tdiv_q(erg, (int_number) a, (int_number) b);
      nrnDelete((number*) &gcd, NULL);
      return (number) erg;
    }
    // a / gcd(a,b) * [b / gcd (a,b)]^(-1)
    int_number tmp = (int_number) nrnInvers((number) erg);
    mpz_divexact(erg, (int_number) a, gcd);
    mpz_mul(erg, erg, tmp);
    nrnDelete((number*) &gcd, NULL);
    nrnDelete((number*) &tmp, NULL);
    mpz_mod(erg, erg, currRing->nrnModul);
    return (number) erg;
  }
}

number nrnMod (number a, number b)
{
  /*
    We need to return the number r which is uniquely determined by the
    following two properties:
      (1) 0 <= r < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + r.
    Consider g := gcd(n, |b|). Note that then |b|/g is a unit in Z/n.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/n, i.e. |b| (and also b) divides a.
          Thus r = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again r = 0.
      (c) g <> 1 and g does not divide a
          Then denote the division with remainder of a by g as this:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. r := t is the correct result. Hence
          in this third case, r is the remainder of division of a by g in Z.
     Remark: according to mpz_mod: a,b are always non-negative
  */
  int_number g = (int_number) omAllocBin(gmp_nrz_bin);
  int_number r = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(g);
  mpz_init_set_si(r,(long)0);
  mpz_gcd(g, (int_number) currRing->nrnModul, (int_number)b); // g is now as above
  if (mpz_cmp_si(g, (long)1) != 0) mpz_mod(r, (int_number)a, g); // the case g <> 1
  mpz_clear(g);
  omFreeBin(g, gmp_nrz_bin);
  return (number)r;
}

number nrnIntDiv (number a,number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number) currRing->nrnModul;
  mpz_tdiv_q(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Helper function for computing the module
 */

int_number nrnMapCoef = NULL;

number nrnMapModN(number from)
{
  return nrnMult(from, (number) nrnMapCoef);
}

number nrnMap2toM(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul_ui(erg, nrnMapCoef, (NATNUMBER) from);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

number nrnMapZp(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul_si(erg, nrnMapCoef, (NATNUMBER) from);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

number nrnMapGMP(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mod(erg, (int_number) from, currRing->nrnModul);
  return (number) erg;
}

number nrnMapQ(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlGMP(from, (number) erg);
  mpz_mod(erg, erg, currRing->nrnModul);
  return (number) erg;
}

nMapFunc nrnSetMap(const ring src, const ring dst)
{
  /* dst = currRing */
  if (rField_is_Ring_Z(src))
  {
    return nrnMapGMP;
  }
  if (rField_is_Q(src))
  {
    return nrnMapQ;
  }
  // Some type of Z/n ring / field
  if (rField_is_Ring_ModN(src) || rField_is_Ring_PtoM(src) || rField_is_Ring_2toM(src) || rField_is_Zp(src))
  {
    if (   (src->ringtype > 0)
        && (mpz_cmp(src->ringflaga, dst->ringflaga) == 0)
        && (src->ringflagb == dst->ringflagb)) return nrnMapGMP;
    else
    {
      int_number nrnMapModul = (int_number) omAllocBin(gmp_nrz_bin);
      // Computing the n of Z/n
      if (rField_is_Zp(src))
      {
        mpz_init_set_si(nrnMapModul, src->ch);
      }
      else
      {
        mpz_init(nrnMapModul);
        mpz_set(nrnMapModul, src->ringflaga);
        mpz_pow_ui(nrnMapModul, nrnMapModul, src->ringflagb);
      }
      // nrnMapCoef = 1 in dst       if dst is a subring of src
      // nrnMapCoef = 0 in dst / src if src is a subring of dst
      if (nrnMapCoef == NULL)
      {
        nrnMapCoef = (int_number) omAllocBin(gmp_nrz_bin);
        mpz_init(nrnMapCoef);
      }
      if (mpz_divisible_p(nrnMapModul, currRing->nrnModul))
      {
        mpz_set_si(nrnMapCoef, 1);
      }
      else
      if (nrnDivBy(NULL, (number) nrnMapModul))
      {
        mpz_divexact(nrnMapCoef, currRing->nrnModul, nrnMapModul);
        int_number tmp = currRing->nrnModul;
        currRing->nrnModul = nrnMapModul;
        if (!nrnIsUnit((number) nrnMapCoef))
        {
          currRing->nrnModul = tmp;
          nrnDelete((number*) &nrnMapModul, currRing);
          return NULL;
        }
        int_number inv = (int_number) nrnInvers((number) nrnMapCoef);
        currRing->nrnModul = tmp;
        mpz_mul(nrnMapCoef, nrnMapCoef, inv);
        mpz_mod(nrnMapCoef, nrnMapCoef, currRing->nrnModul);
        nrnDelete((number*) &inv, currRing);
      }
      else
      {
        nrnDelete((number*) &nrnMapModul, currRing);
        return NULL;
      }
      nrnDelete((number*) &nrnMapModul, currRing);
      if (rField_is_Ring_2toM(src))
        return nrnMap2toM;
      else if (rField_is_Zp(src))
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

void nrnSetExp(int m, ring r)
{
  if ((r->nrnModul != NULL) && (mpz_cmp(r->nrnModul, r->ringflaga) == 0) && (nrnExponent == r->ringflagb)) return;

  nrnExponent = r->ringflagb;
  if (r->nrnModul == NULL)
  {
    r->nrnModul = (int_number) omAllocBin(gmp_nrz_bin);
    mpz_init(r->nrnModul);
    nrnMinusOne = (int_number) omAllocBin(gmp_nrz_bin);
    mpz_init(nrnMinusOne);
  }
  mpz_set(r->nrnModul, r->ringflaga);
  mpz_pow_ui(r->nrnModul, r->nrnModul, nrnExponent);
  mpz_sub_ui(nrnMinusOne, r->nrnModul, 1);
}

void nrnInitExp(int m, ring r)
{
  nrnSetExp(m, r);

  if (mpz_cmp_ui(r->nrnModul,2) <= 0)
  {
    WarnS("nrnInitExp failed");
  }
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, const char *f, const int l)
{
  if (a==NULL) return TRUE;
  if ( (mpz_cmp_si((int_number) a, 0) < 0) || (mpz_cmp((int_number) a, currRing->nrnModul) > 0) )
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

const char * nrnRead (const char *s, number *a)
{
  int_number z = (int_number) omAllocBin(gmp_nrz_bin);
  {
    s = nlCPEatLongC((char *)s, z);
  }
  mpz_mod(z, z, currRing->nrnModul);
  *a = (number) z;
  return s;
}
#endif
