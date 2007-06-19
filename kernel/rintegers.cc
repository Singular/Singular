/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rintegers.cc,v 1.1 2007-06-19 10:47:30 wienand Exp $ */
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

/*
 * Multiply two numbers
 */
number nrzMult (number a, number b)
{
  mpz_t erg;
  mpz_init(erg);
  mpz_mul(erg, (mpz_t) &a, (mpz_t) &b);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrzLcm (number a,number b,ring r)
{
  mpz_t erg;
  mpz_init(erg);
  return (number) erg;//TODO
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrzGcd (number a,number b,ring r)
{
  mpz_t erg;
  mpz_init(erg);
  return (number) erg;//TODO
}

void nrzPower (number a, int i, number * result)
{
  mpz_t erg;
  mpz_init(erg);
  return (number) erg;/TODO
}

/*
 * create a number from int
 */
number nrzInit (int i)
{
  mpz_t erg;
  mpz_init_set_si(erg, i);
  return (number) erg;
}

/*
 * convert a number to int (-p/2 .. p/2)
 */
int nrzInt(number &n)
{
  return (int) mpz_get_si(&n)
}

number nrzAdd (number a, number b)
{
  mpz_t erg;
  mpz_init(erg);
  mpz_add(erg, (mpz_t) a, (mpz_t) b);
  return (number) erg;
}

number nrzSub (number a, number b)
{
  mpz_t erg;
  mpz_init(erg);
  mpz_sub(erg, (mpz_t) a, (mpz_t) b);
  return (number) erg;
}

BOOLEAN nrzIsZero (number  a)
{
  return 0 == mpz_cmpabs_ui((mpz_t) a, 0);
}

BOOLEAN nrzIsOne (number a)
{
  return 0 == mpz_cmp_si((mpz_t) a, 1);
}

BOOLEAN nrzEqual (number a,number b)
{
  return 0 == mpz_cmp((mpz_t) a, (mpz_t) b);
}

BOOLEAN nrzGreater (number a,number b)
{
  return 0 < mpz_cmp((mpz_t) a, (mpz_t) b);
}

int nrzComp(number a, number b)
{
   return 2;//TODO
}

BOOLEAN nrzDivBy (number a,number b)
{
  return FALSE;//TODO
}

BOOLEAN nrzGreaterZero (number k)
{
  return 0 <= mpz_cmp_si((mpz_t) a, 0);
}

number nrzDiv (number a,number b)
{
  return (number)0; //TODO
}

number nrzIntDiv (number a,number b)
{
  return (number)0; //TODO
}

number  nrzInvers (number c)
{
  return (number)0; //TODO
}

number nrzNeg (number c)
{
  return (number)0; //TODO
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
  if ((NATNUMBER)a > (nrzModul >>1)) StringAppend("-%d",(int)(-((NATNUMBER)a)));
  else                          StringAppend("%d",(int)((NATNUMBER)a));
}

char* nrzEati(char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) % nrzModul;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((*i) >= nrzModul) (*i) = (*i) % nrzModul;
  }
  else (*i) = 1;
  return s;
}

char* nrzRead (char *s, number *a)
{
  int z;
  int n=1;

  s = nrzEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = nrzEati(s, &n);
  }
  if (n == 1)
    *a = (number)z;
  else
      *a = nrzDiv((number)z,(number)n);
  return s;
}
#endif