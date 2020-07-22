#ifndef MODULOP2_H
#define MODULOP2_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo p (<=32749)
*/
#include "misc/auxiliary.h"
#include "coeffs/modulop.h"

static inline number npMult (number a,number b, const coeffs r)
{
  n_Test(a, r);
  n_Test(b, r);

  if (((long)a == 0) || ((long)b == 0))
    return (number)0;
  number c = npMultM(a,b, r);
  n_Test(c, r);
  return c;
}

/*2
* create a number from int
*/
static inline number npInit (long i, const coeffs r)
{
  long ii=i % (long)r->ch;
  if (ii <  0L)                         ii += (long)r->ch;

  number c = (number)ii;
  n_Test(c, r);
  return c;
}

#define npIsZeroM(A,B) (0==(long)A)
static inline BOOLEAN npIsZero (number  a, const coeffs r)
{
  n_Test(a, r);

  return npIsZeroM(a,r);
}

#ifdef NV_OPS
// -----------------------------------------------------------
//  operation for very large primes (32749< p < 2^31-1)
// ----------------------------------------------------------
#pragma GCC diagnostic ignored "-Wlong-long"
static inline number nvMult(number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_Zp );

#if SIZEOF_LONG == 4
#define ULONG64 (unsigned long long)(unsigned long)
#else
#define ULONG64 (unsigned long)
#endif
  return (number)
      (unsigned long)((ULONG64 a)*(ULONG64 b) % (ULONG64 r->ch));
}
#endif
#endif
