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

#include <string.h>
#include <misc/mylimits.h>
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/mpr_complex.h>
#include <coeffs/rintegers.h>
#include "si_gmp.h"

/// Our Type!
static const n_coeffType ID = n_Z;

omBin gmp_nrz_bin = omGetSpecBin(sizeof(mpz_t));

/*
 * Multiply two numbers
 */
number nrzMult (number a, number b, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrzLcm (number a,number b,const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_lcm(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrzGcd (number a,number b,const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_gcd(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
number  nrzExtGcd (number a, number b, number *s, number *t, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  int_number bs = (int_number) omAllocBin(gmp_nrz_bin);
  int_number bt = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (int_number) a, (int_number) b);
  *s = (number) bs;
  *t = (number) bt;
  return (number) erg;
}

void nrzPower (number a, int i, number * result, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_pow_ui(erg, (int_number) a, i);
  *result = (number) erg;
}

/*
 * create a number from int
 */
number nrzInit (int i, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  return (number) erg;
}

void nrzDelete(number *a, const coeffs)
{
  if (*a == NULL) return;
  mpz_clear((int_number) *a);
  omFreeBin((ADDRESS) *a, gmp_nrz_bin);
  *a = NULL;
}

number nrzCopy(number a, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (int_number) a);
  return (number) erg;
}

#if 0
number nrzCopyMap(number a, const coeffs /*src*/, const coeffs dst)
{
  return nrzCopy(a,dst);
}
#endif

int nrzSize(number a, const coeffs)
{
  if (a == NULL) return 0;
  return sizeof(mpz_t);
}

/*
 * convert a number to int
 */
int nrzInt(number &n, const coeffs)
{
  return (int) mpz_get_si( (int_number)n);
}

number nrzAdd (number a, number b, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number nrzSub (number a, number b, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number  nrzGetUnit (number, const coeffs r)
{
  return nrzInit(1, r);
}

BOOLEAN nrzIsUnit (number a, const coeffs)
{
  return 0 == mpz_cmpabs_ui((int_number) a, 1);
}

BOOLEAN nrzIsZero (number  a, const coeffs)
{
  return 0 == mpz_cmpabs_ui((int_number) a, 0);
}

BOOLEAN nrzIsOne (number a, const coeffs)
{
  return (a!=NULL) && (0 == mpz_cmp_si((int_number) a, 1));
}

BOOLEAN nrzIsMOne (number a, const coeffs)
{
  return (a!=NULL) && (0 == mpz_cmp_si((int_number) a, -1));
}

BOOLEAN nrzEqual (number a,number b, const coeffs)
{
  return 0 == mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrzGreater (number a,number b, const coeffs)
{
  return 0 < mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrzGreaterZero (number k, const coeffs)
{
  return 0 < mpz_cmp_si((int_number) k, 0);
}

int nrzDivComp(number a, number b, const coeffs r)
{
  if (nrzDivBy(a, b, r))
  {
    if (nrzDivBy(b, a, r)) return 2;
    return -1;
  }
  if (nrzDivBy(b, a, r)) return 1;
  return 0;
}

BOOLEAN nrzDivBy (number a,number b, const coeffs)
{
  return mpz_divisible_p((int_number) a, (int_number) b) != 0;
}

number nrzDiv (number a,number b, const coeffs R)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number r = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (int_number) a, (int_number) b);
  if (!nrzIsZero((number) r, R))
  {
    WerrorS("Division by non divisible element.");
    WerrorS("Result is without remainder.");
  }
  mpz_clear(r);
  omFreeBin(r, gmp_nrz_bin);
  return (number) erg;
}

number nrzIntDiv (number a,number b, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_tdiv_q(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number nrzIntMod (number a,number b, const coeffs)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number r = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (int_number) a, (int_number) b);
  mpz_clear(erg);
  return (number) r;
}

number  nrzInvers (number c, const coeffs r)
{
  if (!nrzIsUnit((number) c, r))
  {
    WerrorS("Non invertible element.");
    return (number)0; //TODO
  }
  return nrzCopy(c,r);
}

number nrzNeg (number c, const coeffs)
{
// nNeg inplace !!!
  mpz_mul_si((int_number) c, (int_number) c, -1);
  return c;
}

number nrzMapMachineInt(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_ui(erg, (NATNUMBER) from);
  return (number) erg;
}

number nrzMapZp(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, (long) from);
  return (number) erg;
}

number nrzMapQ(number from, const coeffs src, const coeffs /*dst*/)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlGMP(from, (number) erg, src);
  return (number) erg;
}

nMapFunc nrzSetMap(const coeffs src, const coeffs dst)
{
  /* dst = currRing */
  if (nCoeff_is_Ring_Z(src) || nCoeff_is_Ring_ModN(src) || nCoeff_is_Ring_PtoM(src))
  {
    return ndCopyMap; //nrzCopyMap;
  }
  if (nCoeff_is_Ring_2toM(src))
  {
    return nrzMapMachineInt;
  }
  if (nCoeff_is_Zp(src))
  {
    return nrzMapZp;
  }
  if (nCoeff_is_Q(src))
  {
    return nrzMapQ;
  }
  return NULL;      // default
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrzSetExp(int, coeffs)
{
}

void nrzInitExp(int, coeffs)
{
}

#ifdef LDEBUG
BOOLEAN nrzDBTest (number, const char *, const int, const coeffs)
{
  return TRUE;//TODO
}
#endif

void nrzWrite (number &a, const coeffs)
{
  char *s,*z;
  if (a==NULL)
  {
    StringAppendS("o");
  }
  else
  {
    int l=mpz_sizeinbase((int_number) a, 10) + 2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,(int_number) a);
    StringAppendS(z);
    omFreeSize((ADDRESS)s,l);
  }
}

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
static const char * nlEatLongC(char *s, mpz_ptr i)
{
  const char * start=s;

  if (*s<'0' || *s>'9')
  {
    mpz_set_si(i,1);
    return s;
  }
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

const char * nrzRead (const char *s, number *a, const coeffs)
{
  int_number z = (int_number) omAllocBin(gmp_nrz_bin);
  {
    mpz_init(z);
    s = nlEatLongC((char *) s, z);
  }
  *a = (number) z;
  return s;
}

void    nrzCoeffWrite  (const coeffs)
{
  PrintS("//   characteristic : 0 (Integers)\n");
}


BOOLEAN nrzInitChar(coeffs r,  void *)
{
  assume( getCoeffType(r) == ID );
  r->cfSetChar= NULL;
  r->nCoeffIsEqual = ndCoeffIsEqual;
  r->cfKillChar = ndKillChar;
  r->cfMult  = nrzMult;
  r->cfSub   = nrzSub;
  r->cfAdd   = nrzAdd;
  r->cfDiv   = nrzDiv;
  r->cfIntDiv= nrzDiv;
  r->cfIntMod= nrzIntMod;
  r->cfExactDiv= nrzDiv;
  r->cfInit = nrzInit;
  r->cfSize  = nrzSize;
  r->cfInt  = nrzInt;
  //#ifdef HAVE_RINGS
  r->cfDivComp = nrzDivComp; // only for ring stuff
  r->cfIsUnit = nrzIsUnit; // only for ring stuff
  r->cfGetUnit = nrzGetUnit; // only for ring stuff
  r->cfExtGcd = nrzExtGcd; // only for ring stuff
  r->cfDivBy = nrzDivBy; // only for ring stuff
  //#endif
  r->cfNeg   = nrzNeg;
  r->cfInvers= nrzInvers;
  r->cfCopy  = nrzCopy;
  r->cfWrite = nrzWrite;
  r->cfRead = nrzRead;
  r->cfGreater = nrzGreater;
  r->cfEqual = nrzEqual;
  r->cfIsZero = nrzIsZero;
  r->cfIsOne = nrzIsOne;
  r->cfIsMOne = nrzIsMOne;
  r->cfGreaterZero = nrzGreaterZero;
  r->cfPower = nrzPower;
  r->cfGcd  = nrzGcd;
  r->cfLcm  = nrzGcd;
  r->cfDelete= nrzDelete;
  r->cfSetMap = nrzSetMap;
  r->cfCoeffWrite = nrzCoeffWrite;
  // debug stuff

#ifdef LDEBUG
  r->cfDBTest=nrzDBTest;
#endif

  r->nNULL = 0;
  r->ch = 0;
  r->ringtype = 4;
  r->has_simple_Alloc=FALSE;
  r->has_simple_Inverse=FALSE;
  return FALSE;
}

#endif
