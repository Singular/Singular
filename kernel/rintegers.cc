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
#include <kernel/rintegers.h>
#include <kernel/si_gmp.h>

#ifdef HAVE_RINGS

omBin gmp_nrz_bin = omGetSpecBin(sizeof(mpz_t));

/*
 * Multiply two numbers
 */
number nrzMult (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrzLcm (number a,number b,ring r)
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
number nrzGcd (number a,number b,ring r)
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
number  nrzExtGcd (number a, number b, number *s, number *t)
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

void nrzPower (number a, int i, number * result)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_pow_ui(erg, (int_number) a, i);
  *result = (number) erg;
}

/*
 * create a number from int
 */
number nrzInit (int i, const ring r)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  return (number) erg;
}

void nrzDelete(number *a, const ring r)
{
  if (*a == NULL) return;
  mpz_clear((int_number) *a);
  omFreeBin((ADDRESS) *a, gmp_nrz_bin);
  *a = NULL;
}

number nrzCopy(number a)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (int_number) a);
  return (number) erg;
}

number cfrzCopy(number a, const ring r)
{
  return nrzCopy(a);
}

int nrzSize(number a)
{
  if (a == NULL) return 0;
  return sizeof(mpz_t);
}

/*
 * convert a number to int
 */
int nrzInt(number &n, const ring r)
{
  return (int) mpz_get_si( (int_number)n);
}

number nrzAdd (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number nrzSub (number a, number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number  nrzGetUnit (number a)
{
  return nrzInit(1, currRing);
}

BOOLEAN nrzIsUnit (number a)
{
  return 0 == mpz_cmpabs_ui((int_number) a, 1);
}

BOOLEAN nrzIsZero (number  a)
{
  return 0 == mpz_cmpabs_ui((int_number) a, 0);
}

BOOLEAN nrzIsOne (number a)
{
  return (a!=NULL) && (0 == mpz_cmp_si((int_number) a, 1));
}

BOOLEAN nrzIsMOne (number a)
{
  return (a!=NULL) && (0 == mpz_cmp_si((int_number) a, -1));
}

BOOLEAN nrzEqual (number a,number b)
{
  return 0 == mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrzGreater (number a,number b)
{
  return 0 < mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrzGreaterZero (number k)
{
  return 0 < mpz_cmp_si((int_number) k, 0);
}

int nrzDivComp(number a, number b)
{
  if (nrzDivBy(a, b))
  {
    if (nrzDivBy(b, a)) return 2;
    return -1;
  }
  if (nrzDivBy(b, a)) return 1;
  return 0;
}

BOOLEAN nrzDivBy (number a,number b)
{
  return mpz_divisible_p((int_number) a, (int_number) b) != 0;
}

number nrzDiv (number a,number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number r = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (int_number) a, (int_number) b);
  if (!nrzIsZero((number) r))
  {
    WerrorS("Division by non divisible element.");
    WerrorS("Result is without remainder.");
  }
  mpz_clear(r);
  omFreeBin(r, gmp_nrz_bin);
  return (number) erg;
}

number nrzIntDiv (number a,number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_tdiv_q(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number nrzIntMod (number a,number b)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  int_number r = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (int_number) a, (int_number) b);
  mpz_clear(erg);
  return (number) r;
}

number  nrzInvers (number c)
{
  if (!nrzIsUnit((number) c))
  {
    WerrorS("Non invertible element.");
    return (number)0; //TODO
  }
  return nrzCopy(c);
}

number nrzNeg (number c)
{
// nNeg inplace !!!
  mpz_mul_si((int_number) c, (int_number) c, -1);
  return c;
}

number nrzMapMachineInt(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_ui(erg, (NATNUMBER) from);
  return (number) erg;
}

number nrzMapZp(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, (long) from);
  return (number) erg;
}

number nrzMapQ(number from)
{
  int_number erg = (int_number) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlGMP(from, (number) erg);
  return (number) erg;
}

nMapFunc nrzSetMap(const ring src, const ring dst)
{
  /* dst = currRing */
  if (rField_is_Ring_Z(src) || rField_is_Ring_ModN(src) || rField_is_Ring_PtoM(src))
  {
    return nrzCopy;
  }
  if (rField_is_Ring_2toM(src))
  {
    return nrzMapMachineInt;
  }
  if (rField_is_Zp(src))
  {
    return nrzMapZp;
  }
  if (rField_is_Q(src))
  {
    return nrzMapQ;
  }
  return NULL;      // default
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrzSetExp(int m, ring r)
{
}

void nrzInitExp(int m, ring r)
{
}

#ifdef LDEBUG
//BOOLEAN nrzDBTest (number a, const char *f, const int l)
//{
//  return TRUE;//TODO
//}
#endif

void nrzWrite (number &a, const ring r)
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

const char * nrzRead (const char *s, number *a)
{
  int_number z = (int_number) omAllocBin(gmp_nrz_bin);
  {
    mpz_init(z);
    s = nlEatLongC((char *) s, z);
  }
  *a = (number) z;
  return s;
}
#endif
