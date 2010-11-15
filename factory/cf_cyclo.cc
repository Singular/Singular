// -*- c++ -*-
//*****************************************************************************
/** @file cf_cyclo.cc
 *
 * @author Martin Lee
 * @date 29.01.2010
 *
 * Compute cyclotomic polynomials and factorize integers by brute force
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>

#include "canonicalform.h"
#include "cf_primes.h"
#include "cf_util.h"

#ifdef HAVE_NTL
#include <NTL/ZZ.h>
#endif


/// integer factorization using table look-ups,
/// function may fail if integer contains primes which exceed the largest prime
/// in our table
int* integerFactorizer (const long integer, int& length, bool& fail)
{
  ASSERT (integer != 0 && integer != 1 && integer != -1,
          "non-zero non-unit expected");
  int* result;
  length= 0;
  fail= false;
  int i= integer;
  if (integer < 0)
    i = -integer;

  int exp= 0;
  while ((i != 1) && (i%2 == 0))
  {
    i /= 2;
    exp++;
  }
  if (exp != 0)
  {
    result= new int [exp];
    for (int k= 0; k < exp; k++)
      result[k]= 2;
    length += exp;
  }
  if (i == 1) return result;

  long j= 0;
  exp= 0;
  int* buf;
  int next_prime;
  while ((i != 1) && (j < 31937))
  {
    next_prime= cf_getPrime (j);
    while ((i != 1) && (i%next_prime == 0))
    {
      i /= next_prime;
      exp++;
    }
    if (exp != 0)
    {
      buf= result;
      result= new int [length + exp];
      for (int k= 0; k < length; k++)
        result [k]= buf[k];
      for (int k= 0; k < exp; k++)
        result [k + length]= next_prime;
      length += exp;
    }
    exp= 0;
    j++;
  }
  if (j >= 31397)
    fail= true;
  ASSERT (j < 31397, "integer factorizer ran out of primes") //sic
  return result;
}

/// make prime factorization distinct
static inline
int* makeDistinct (int* factors, const int factors_length, int& length)
{
  length= 1;
  int* result= new int [length];
  int* buf;
  result[0]= factors [0];
  for (int i= 1; i < factors_length; i++)
  {
    if (factors[i - 1] != factors[i])
    {
      buf= result;
      result= new int [length + 1];
      for (int j= 0; j < length; j++)
        result[j]= buf [j];
      result[length]= factors[i];
      length++;
    }
  }
  return result;
}

/// compute the n-th cyclotomic polynomial,
/// function may fail if integer_factorizer fails to factorize n
CanonicalForm cyclotomicPoly (int n, bool& fail)
{
  fail= false;
  Variable x= Variable (1);
  CanonicalForm result= x - 1;
  if (n == 1)
    return result;
  int* prime_factors;
  int prime_factors_length;
  int distinct_factors_length;
  prime_factors= integerFactorizer (n, prime_factors_length, fail);
  int* distinct_factors= makeDistinct (prime_factors, prime_factors_length,
                                        distinct_factors_length);
  if (fail)
    return 1;
  CanonicalForm buf;
  int prod= 1;
  for (int i= 0; i < distinct_factors_length; i++)
  {
    result= result (power (x, distinct_factors[i]), x)/result;
    prod *= distinct_factors[i];
  }
  return result (power (x, n/prod), x);
}

#ifdef HAVE_NTL
/// checks if alpha is a primitive element, alpha is assumed to be an algebraic
/// variable over some finite prime field
bool isPrimitive (const Variable& alpha, bool& fail)
{
  int p= getCharacteristic();
  CanonicalForm mipo= getMipo (alpha);
  int order= ipower(p, degree(mipo)) - 1;
  CanonicalForm cyclo= cyclotomicPoly (order, fail);
  if (fail)
    return false;
  if (mod(cyclo, mipo (Variable(1), alpha)) == 0)
    return true;
  else
    return false;
}

#endif
