// -*- c++ -*-
//*****************************************************************************
/** @file cf_cyclo.cc
 *
 * Compute cyclotomic polynomials and factorize integers by brute force
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
 * @date 29.01.2010
**/
//*****************************************************************************


#include "config.h"


#include "canonicalform.h"
#include "cf_primes.h"
#include "cf_util.h"
#include "cf_assert.h"

int* integerFactorizer (const long integer, int& length, bool& fail)
{
  ASSERT (integer != 0 && integer != 1 && integer != -1,
          "non-zero non-unit expected");
  int* result=NULL;
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
      int *buf= result;
      result= new int [length + exp];
      for (int k= 0; k < length; k++)
        result [k]= buf[k];
      for (int k= 0; k < exp; k++)
        result [k + length]= next_prime;
      length += exp;
      delete[] buf;
    }
    exp= 0;
    j++;
  }
  if (j >= 31397)
    fail= true;
  ASSERT (j < 31397, "integer factorizer ran out of primes"); //sic
  return result;
}

/// make prime factorization distinct
static inline
int* makeDistinct (int* factors, const int factors_length, int& length)
{
  length= 1;
  int* result= new int [length];
  result[0]= factors [0];
  for (int i= 1; i < factors_length; i++)
  {
    if (factors[i - 1] != factors[i])
    {
      int *buf= result;
      result= new int [length + 1];
      for (int j= 0; j < length; j++)
        result[j]= buf [j];
      result[length]= factors[i];
      delete[] buf;
      length++;
    }
  }
  return result;
}

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
  delete [] prime_factors;
  if (fail)
    return 1;
  CanonicalForm buf;
  int prod= 1;
  for (int i= 0; i < distinct_factors_length; i++)
  {
    result= leftShift (result, distinct_factors[i])/result;
    prod *= distinct_factors[i];
  }
  delete [] distinct_factors;
  return leftShift (result, n/prod);
}

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
