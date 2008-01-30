/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulon.cc,v 1.10 2008-01-30 16:14:25 wienand Exp $ */
/*
* ABSTRACT: numbers modulo n
*/

#include <string.h>
#include "mod2.h"
#include <mylimits.h>
#include "structs.h"
#include "febase.h"
#include "omalloc.h"
#include "numbers.h"
#include "longrat.h"
#include "mpr_complex.h"
#include "ring.h"
#include "rmodulon.h"
#include "gmp.h"

#ifdef HAVE_RINGMODN

typedef MP_INT *int_number;

int_number nrnModul = NULL;
int_number nrnMinusOne = NULL;
unsigned long nrnExponent = 0;
unsigned long long nrnBase = 0;

/*
 * Multiply two numbers
 */
number nrnMult (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_mul(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, nrnModul);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrnLcm (number a,number b,ring r)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_lcm(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, nrnModul);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrnGcd (number a,number b,ring r)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_gcd(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
number  nrnExtGcd (number a, number b, number *s, number *t)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  int_number bs = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  int_number bt = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (int_number) a, (int_number) b);
  *s = (number) bs;
  *t = (number) bt;
  return (number) erg;
}

void nrnPower (number a, int i, number * result)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_pow_ui(erg, (int_number) a, i);
  mpz_mod(erg, erg, nrnModul);
//  int_number tmp = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
//  mpz_init(tmp);
//  mpz_clear(erg);
//  omFree(erg);
//  erg = tmp;
  *result = (number) erg;
}

/*
 * create a number from int
 */
number nrnInit (int i)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init_set_si(erg, i);
  return (number) erg;
}

void nrnDelete(number *a, const ring r)
{
  mpz_clear((int_number) *a);
  omFree((ADDRESS) *a);
}

/*
 * convert a number to int (-p/2 .. p/2)
 */
int nrnInt(number &n)
{
  return (int) mpz_get_si( (int_number) &n);
}

number nrnAdd (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_add(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, nrnModul);
  return (number) erg;
}

number nrnSub (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_sub(erg, (int_number) a, (int_number) b);
  mpz_mod(erg, erg, nrnModul);
  return (number) erg;
}

number  nrnGetUnit (number a)
{
  return nrnDiv((number) nrnModul, nrnGcd(a, (number) nrnModul, NULL));
}

BOOLEAN nrnIsUnit (number a)
{
  return nrnIsOne(nrnGcd(a, (number) nrnModul, NULL));
}

BOOLEAN nrnIsZero (number  a)
{
  return 0 == mpz_cmpabs_ui((int_number) a, 0);
}

BOOLEAN nrnIsOne (number a)
{
  return 0 == mpz_cmp_si((int_number) a, 1);
}

BOOLEAN nrnIsMOne (number a)
{
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
  return 0 <= mpz_cmp_si((int_number) k, 0);
}

int nrnComp(number a, number b)
{
  if (nrnEqual(a, b)) return 0;
  if (nrnDivBy(a, b)) return -1;
  if (nrnDivBy(b, a)) return 1;
  return 2;
}

BOOLEAN nrnDivBy (number a,number b)
{
  return mpz_divisible_p((int_number) a, (int_number) b) != 0;
}

number nrnDiv (number a,number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  if (nrnDivBy(a, b))
  {
    mpz_divexact(erg, (int_number) a, (int_number) b);
    return (number) erg;
  }
  else
  {
    int_number gcd = (int_number) nrnGcd(a, b, NULL);
    int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
    mpz_init(erg);
    mpz_divexact(erg, (int_number) b, gcd);
    if (!nrnIsUnit((number) erg))
    {
      WarnS("Division not possible, even by cancelling zero divisors.");
      WarnS("Result is zero.");
      mpz_set_ui(erg, 0);
      mpz_clear(gcd);
      omFree(gcd);
      return (number) erg;
    }
    int_number tmp = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
    mpz_init(tmp);
    tmp = (int_number) nrnInvers((number) erg);
    mpz_divexact(erg, (int_number) a, gcd);
    mpz_mul(erg, erg, tmp);
    mpz_clear(gcd);
    omFree(gcd);
    mpz_clear(tmp);
    omFree(tmp);
    mpz_mod(erg, erg, nrnModul);
    return (number) erg;
  }
}

number nrnIntDiv (number a,number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_tdiv_q(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number  nrnInvers (number c)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_invert(erg, (int_number) c, nrnModul);
  return (number) erg;
}

number nrnNeg (number c)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_sub(erg, nrnModul, (int_number) c);
  return (number) erg;
}

nMapFunc nrnSetMap(ring src, ring dst)
{
  return NULL;      /* default */
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void mpz_set_ull(int_number res, unsigned long long xx)
{
  unsigned long h = xx >> 32;
  mpz_set_ui (res, h);
  mpz_mul_2exp (res, res, 32);
  mpz_add_ui (res, res, (unsigned long) xx);
}

void nrnSetExp(int m, ring r)
{
  if ((nrnBase == r->ringflaga) && (nrnExponent == r->ringflagb)) return;
  nrnBase = r->ringflaga;
  nrnExponent = r->ringflagb;
  if (nrnModul != NULL)
  {
    mpz_clear(nrnModul);
    omFree(nrnModul);
    mpz_clear(nrnMinusOne);
    omFree(nrnMinusOne);
  }
  nrnModul = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(nrnModul);
  mpz_set_ull(nrnModul, nrnBase);
  mpz_pow_ui(nrnModul, nrnModul, nrnExponent);

  nrnMinusOne = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(nrnMinusOne);
  mpz_sub_ui(nrnMinusOne, nrnModul, 1);
}

void nrnInitExp(int m, ring r)
{
  nrnSetExp(m, r);
  if (mpz_cmp_ui(nrnModul,2) <= 0)
  {
    WarnS("nInitChar failed");
  }
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, char *f, int l)
{
  if ( (mpz_cmp_si((int_number) a, 0) < 0) || (mpz_cmp((int_number) a, nrnModul) > 0) )
  {
    return FALSE;
  }
  return TRUE;
}
#endif

void nrnWrite (number &a)
{
  char *s,*z;
  if (a==NULL)
  {
    StringAppendS("o");
  }
  else
  {
    int l=mpz_sizeinbase((int_number) a, 10);
    if (a->s<2) l=si_max(l,mpz_sizeinbase((int_number) a,10));
    l+=2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,(int_number) a);
    StringAppendS(z);
    omFreeSize((ADDRESS)s,l);
  }
}

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
char * nlCPEatLongC(char *s, MP_INT *i)
{
  char * start=s;

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

char * nrnRead (char *s, number *a)
{
  int_number z = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  {
    mpz_init(z);
    s = nlCPEatLongC(s, z);
  }
  *a = (number) z;
  return s;
}
#endif
