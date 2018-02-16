/*****************************************
 * Computer Algebra System SINGULAR      *
 *****************************************/
/*
 *  simple prime test for int
 */

#include <cmath>
#include "misc/prime.h"
#include "factory/cf_primes.h"

static int iiIsPrime0(unsigned p)  /* brute force !!!! */
{
  unsigned i,j=0 /*only to avoid compiler warnings*/;
  if (p<=32749) // max. small prime in factory
  {
    int a=0;
    int e=cf_getNumSmallPrimes()-1;
    i=e/2;
    do
    {
      j=cf_getSmallPrime(i);
      if (p==j) return p;
      if (p<j) e=i-1;
      else     a=i+1;
      i=a+(e-a)/2;
    } while ( a<= e);
    if (p>j) return j;
    else     return cf_getSmallPrime(i-1);
  }
  unsigned end_i=cf_getNumSmallPrimes()-1;
  unsigned end_p=(unsigned)sqrt((double)p);
restart:
  for (i=0; i<end_i; i++)
  {
    j=cf_getSmallPrime(i);
    if ((p%j) == 0)
    {
      if (p<=32751) return iiIsPrime0(p-2);
      p-=2;
      goto restart;
    }
    if (j > end_p) return p;
  }
  if (i>=end_i)
  {
    while(j<=end_p)
    {
      j+=2;
      if ((p%j) == 0)
      {
        if (p<=32751) return iiIsPrime0(p-2);
        p-=2;
        goto restart;
      }
    }
  }
  return p;
}

int IsPrime(int p)  /* brute force !!!! */
{
  if      (p == 0)    return 0;
  else if (p == 1)    return 1/*1*/;
  else if ((p == 2)||(p==3))    return p;
  else if (p < 0)     return 2; //(iiIsPrime0((unsigned)(-p)));
  else if ((p & 1)==0) return iiIsPrime0((unsigned)(p-1));
  return iiIsPrime0((unsigned)(p));
}

