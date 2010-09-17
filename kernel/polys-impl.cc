/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       polys-impl.cc
 * Purpose:    low-level procuders for polys.
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.[h,cc] should be used, instead.
 *
 ***************************************************************/
#ifndef POLYS_IMPL_CC
#define POLYS_IMPL_CC

#include <stdio.h>
#include <string.h>
#include <kernel/mod2.h>

#include <omalloc/omalloc.h>
#ifdef PDEBUG
#undef NO_INLINE3
#define NO_INLINE3
#endif
#include <kernel/polys-impl.h>

#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ring.h>
#include <kernel/p_Procs.h>
#include <kernel/dError.h>

#ifdef PDEBUG
int pDBsyzComp=0;
#endif

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/


poly pHeadProc(poly p)
{
  return pHead(p);
}


static inline unsigned long GetBitFields(long e,
                                         unsigned int s, unsigned int n)
{
#define Sy_bit_L(x)     (((unsigned long)1L)<<(x))
  unsigned int i = 0;
  unsigned long  ev = 0L;
  assume(n > 0 && s < BIT_SIZEOF_LONG);
  do
  {
    assume(s+i < BIT_SIZEOF_LONG);
    if (e > (long) i) ev |= Sy_bit_L(s+i);
    else break;
    i++;
  }
  while (i < n);
  return ev;
}

// Short Exponent Vectors are used for fast divisibility tests
// ShortExpVectors "squeeze" an exponent vector into one word as follows:
// Let n = BIT_SIZEOF_LONG / pVariables.
// If n == 0 (i.e. pVariables > BIT_SIZE_OF_LONG), let m == the number
// of non-zero exponents. If (m>BIT_SIZEOF_LONG), then sev = ~0, else
// first m bits of sev are set to 1.
// Otherwise (i.e. pVariables <= BIT_SIZE_OF_LONG)
// represented by a bit-field of length n (resp. n+1 for some
// exponents). If the value of an exponent is greater or equal to n, then
// all of its respective n bits are set to 1. If the value of an exponent
// is smaller than n, say m, then only the first m bits of the respective
// n bits are set to 1, the others are set to 0.
// This way, we have:
// exp1 / exp2 ==> (ev1 & ~ev2) == 0, i.e.,
// if (ev1 & ~ev2) then exp1 does not divide exp2
unsigned long p_GetShortExpVector(poly p, ring r)
{
  assume(p != NULL);
  if (p == NULL) return 0;
  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / r->N; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  unsigned int i = 0, j=1;

  if (n == 0)
  {
    if (r->N <2*BIT_SIZEOF_LONG)
    {
      n=1;
      m1=0;
    }
    else
    {
      for (; j<=(unsigned long) r->N; j++)
      {
        if (p_GetExp(p,j,r) > 0) i++;
        if (i == BIT_SIZEOF_LONG) break;
      }
      if (i>0)
      ev = ~((unsigned long)0) >> ((unsigned long) (BIT_SIZEOF_LONG - i));
      return ev;
    }
  }
  else
  {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*r->N);
  }

  n++;
  while (i<m1)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }
  return ev;
}

#endif // POLYS_IMPL_CC
