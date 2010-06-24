/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file misc.cc
 *
 * This file provides miscellaneous functionality.
 *
 * For more general information, see the documentation in
 * misc.h.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

// include header files
#include "mod2.h"
#include "lists.h"
#include "longrat.h" /* We only need bigints. */
#include "misc.h"

/* This works by Newton iteration, i.e.,
      a(1)   = n;
      a(i+1) = a(i)/2 + n/2/a(i), i > 0.
   This sequence is guaranteed to decrease monotonously and
   it is known to converge fast.
   All used numbers are bigints. */
number approximateSqrt(const number n)
{
  if (nlIsZero(n)) { number zero = nlInit(0, NULL); return zero; }
  number temp1; number temp2;
  number one = nlInit(1, NULL);
  number two = nlInit(2, NULL);
  number m = nlCopy(n);
  number mOld = nlSub(m, one); /* initially required to be different from m */
  number nHalf = nlIntDiv(n, two);
  bool check = true;
  while (!nlEqual(m, mOld) && check)
  {
    temp1 = nlIntDiv(m, two);
    temp2 = nlIntDiv(nHalf, m);
    mOld = m;
    m = nlAdd(temp1, temp2);
    nlDelete(&temp1, NULL); nlDelete(&temp2, NULL);
    temp1 = nlMult(m, m);
    check = nlGreater(temp1, n);
    nlDelete(&temp1, NULL);
  }
  nlDelete(&mOld, NULL); nlDelete(&two, NULL); nlDelete(&nHalf, NULL);
  while (!check)
  {
    temp1 = nlAdd(m, one);
    nlDelete(&m, NULL);
    m = temp1;
    temp1 = nlMult(m, m);
    check = nlGreater(temp1, n);
    nlDelete(&temp1, NULL);
  }
  temp1 = nlSub(m, one);
  nlDelete(&m, NULL);
  nlDelete(&one, NULL);
  m = temp1;
  return m;
}

/* returns the quotient resulting from division of n by the prime as many
   times as possible without remainder; afterwards, the parameter times
   will contain the highest exponent e of p such that p^e divides n
   e.g., divTimes(48, 4, t) = 3 with t = 2, since 48 = 4*4*3;
   n is expected to be a bigint; returned type is also bigint */
number divTimes(const number n, const int p, int* times)
{
  number nn = nlCopy(n);
  number dd = nlInit(p, NULL);
  number rr = nlIntMod(nn, dd);
  *times = 0;
  while (nlIsZero(rr))
  {
    (*times)++;
    number temp = nlIntDiv(nn, dd);
    nlDelete(&nn, NULL);
    nn = temp;
    nlDelete(&rr, NULL);
    rr = nlIntMod(nn, dd);
  }
  nlDelete(&rr, NULL); nlDelete(&dd, NULL);
  return nn;
}

/* returns an object of type lists which contains the entries
   theInts[0..(length-1)] as INT_CMDs*/
lists makeListsObject(const int* theInts, int length)
{
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(length);
  for (int i = 0; i < length; i++)
    { L->m[i].rtyp = INT_CMD; L->m[i].data = (void*)theInts[i]; }
  return L;
}

/* returns the i-th bit of the binary number which arises by
   concatenating array[length-1], ..., array[1], array[0],
   where array[0] contains the 32 lowest bits etc.;
   i is assumed to be small enough to address a valid index
   in the given array */
bool getValue(const int i, const unsigned int* ii)
{
  int index = i / 32;
  int offset = i % 32;
  unsigned int v = 1 << offset;
  return ((ii[index] & v) != 0);
}

/* sets the i-th bit of the binary number which arises by
   concatenating array[length-1], ..., array[1], array[0],
   where array[0] contains the 32 lowest bits etc.;
   i is assumed to be small enough to address a valid index
   in the given array */
void setValue(const int i, bool value, unsigned int* ii)
{
  int index = i / 32;
  int offset = i % 32;
  unsigned int v = 1 << offset;
  if (value && ((ii[index] & v) != 0)) return;
  if ((!value) && ((ii[index] & v) == 0)) return;
  if (value && ((ii[index] & v) == 0)) { ii[index] += v; return; }
  if ((!value) && ((ii[index] & v) != 0)) { ii[index] -= v; return; }
}

/* returns whether i is less than or equal to the bigint number n */
bool isLeq(const int i, const number n)
{
  number iN = nlInit(i - 1, NULL);
  bool result = nlGreater(n, iN);
  nlDelete(&iN, NULL);
  return result;
}

lists primeFactorisation(const number n, const int pBound)
{
  number nn = nlCopy(n); int i;
  int pCounter = 0; /* for counting the number of mutually distinct
                       prime factors in n */
  /* we assume that there are at most 1000 mutually distinct prime
     factors in n */
  int* primes = new int[1000]; int* multiplicities = new int[1000];

  /* extra treatment for the primes 2 and 3;
     all other primes are equal to +1/-1 mod 6 */
  int e; number temp;
  temp = divTimes(nn, 2, &e); nlDelete(&nn, NULL); nn = temp;
  if (e > 0) { primes[pCounter] = 2; multiplicities[pCounter++] = e; }
  temp = divTimes(nn, 3, &e); nlDelete(&nn, NULL); nn = temp;
  if (e > 0) { primes[pCounter] = 3; multiplicities[pCounter++] = e; }

  /* now we simultaneously:
     - build the sieve of Erathostenes up to s,
     - divide out each prime factor of nn that we find along the way
       (This may result in an earlier termination.) */

  int s = 67108864;       /* = 2^26 */
  int maxP = 2147483647; /* = 2^31 - 1, by the way a Mersenne prime */
  if ((pBound != 0) && (pBound < maxP)) maxP = pBound;
  unsigned int* isPrime = new unsigned int[s];
  /* the lowest bit of isPrime[0] stores whether 0 is a prime,
     next bit is for 1, next for 2, etc. i.e.
     intended usage is: isPrime[0] = 10100000100010100010100010101100.
     This goes on up to isPrime[67108863]; the highest bit of this
     unsigned int corresponds to 67108863*32+31 = 2^31-1.
     We shall make use only of bits which correspond to numbers =
     -1 or +1 mod 6. */
  for (i = 0; i < s; i++) isPrime[i] = 4294967295; /* all 32 bits set */
  int p = 5; bool add2 = true;
  while ((p <= maxP) && (isLeq(p, nn)))
  {
    /* at this point, p is guaranteed to be a prime;
       we divide nn by the highest power of p and store p
       if nn is at all divisible by p */
    temp = divTimes(nn, p, &e);
    nlDelete(&nn, NULL); nn = temp;
    if (e > 0) { primes[pCounter] = p; multiplicities[pCounter++] = e; }
    /* invalidate all multiples of p, starting with 2*p */
    i = 2 * p;
    while (i <= s) { setValue(i, false, isPrime); i += p; }
    /* move on to the next prime in the sieve; we either add 2 or 4
       in order to visit just the numbers equal to -1/+1 mod 6 */
    if (add2) { p += 2; add2 = false; }
    else      { p += 4; add2 = true;  }
    while ((p <= maxP) && (isLeq(p, nn)) && (!getValue(p, isPrime)))
    {
      if (add2) { p += 2; add2 = false; }
      else      { p += 4; add2 = true;  }
    }
  }

  /* build return structure and clean up */
  delete [] isPrime;
  lists primesL = makeListsObject(primes, pCounter);
  lists multiplicitiesL = makeListsObject(multiplicities, pCounter);
  delete [] primes; delete [] multiplicities;
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp = BIGINT_CMD; L->m[0].data = (void *)nn;
  /* try to fit nn into an int: */
  int nnAsInt = nlInt(nn, NULL);
  if (nlIsZero(nn) || (nnAsInt != 0))
  { L->m[0].rtyp = INT_CMD; L->m[0].data = (void *)nnAsInt; }
  L->m[1].rtyp = LIST_CMD; L->m[1].data = (void *)primesL;
  L->m[2].rtyp = LIST_CMD; L->m[2].data = (void *)multiplicitiesL;
  return L;
}
