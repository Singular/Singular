/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rintegers.cc,v 1.3 2007-06-20 13:19:41 wienand Exp $ */
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

#ifdef HAVE_RINGZ

typedef MP_INT *int_number;

/*
 * Multiply two numbers
 */
number nrzMult (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_mul(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrzLcm (number a,number b,ring r)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
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
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
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

void nrzPower (number a, int i, number * result)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_pow_ui(erg, (int_number) a, i);
  *result = (number) erg;
}

/*
 * create a number from int
 */
number nrzInit (int i)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init_set_si(erg, i);
  return (number) erg;
}

void nrzDelete(number *a, const ring r)
{
  mpz_clear((int_number) *a);
  omFree((ADDRESS) *a);
}

/*
 * convert a number to int (-p/2 .. p/2)
 */
int nrzInt(number &n)
{
  return (int) mpz_get_si( (int_number) &n);
}

number nrzAdd (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_add(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number nrzSub (number a, number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_sub(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number  nrzGetUnit (number a)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init_set_si(erg, 1);
  return (number) erg;
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
  return 0 == mpz_cmp_si((int_number) a, 1);
}

BOOLEAN nrzEqual (number a,number b)
{
  return 0 == mpz_cmp((int_number) a, (int_number) b);
}

BOOLEAN nrzGreater (number a,number b)
{
  return 0 < mpz_cmp((int_number) a, (int_number) b);
}

int nrzComp(number a, number b)
{
  if (nrnEqual(a, b)) return 0;
  if (nrnDivBy(a, b)) return -1;
  if (nrnDivBy(b, a)) return 1;
  return 2;
}

BOOLEAN nrzDivBy (number a,number b)
{
  return mpz_divisible_p((int_number) a, (int_number) b) > 0;//TODO
}

BOOLEAN nrzGreaterZero (number k)
{
  return 0 <= mpz_cmp_si((int_number) k, 0);
}

BOOLEAN nrzIsMOne (number a)
{
  return 0 == mpz_cmp_si((int_number) a, 1);
}

number nrzDiv (number a,number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  int_number r = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_tdiv_qr(erg, r, (int_number) a, (int_number) b);
  if (!nrzIsZero((number) r))
  {
    WarnS("Division by non divisible element.");
    WarnS("Result is without remainder.");
  }
  mpz_clear(r);
  omFree(r);
  return (number) erg;
}

number nrzIntDiv (number a,number b)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_tdiv_q(erg, (int_number) a, (int_number) b);
  return (number) erg;
}

number  nrzInvers (number c)
{
  if (!nrzIsUnit((number) c))
  {
    WarnS("Non invertible element.");
    return (number)0; //TODO
  }
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_set(erg, (int_number) c);
  return (number) erg;
}

number nrzNeg (number c)
{
  int_number erg = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  mpz_init(erg);
  mpz_mul_si(erg, (int_number) c, -1);
  return (number) erg;
}

nMapFunc nrzSetMap(ring src, ring dst)
{
  return NULL;      /* default */
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
BOOLEAN nrzDBTest (number a, char *f, int l)
{
  return TRUE;//TODO
}
#endif

void nrzWrite (number &a)
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
char * nlEatLongC(char *s, MP_INT *i)
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

char * nrzRead (char *s, number *a)
{
  int_number z = (int_number) omAlloc(sizeof(MP_INT)); // evtl. spaeter mit bin
  {
    mpz_init(z);
    s = nlEatLongC(s, z);
  }
  *a = (number) z;
  return s;
}
#endif
