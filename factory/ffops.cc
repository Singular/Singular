/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ffops.cc,v 1.8 1998-04-14 15:17:16 pohl Exp $ */

#include <config.h>

#include <string.h>

#include "assert.h"

#include "cf_defs.h"
#include "ffops.h"

int ff_prime = 31991;
int ff_halfprime = 31991 / 2;
bool ff_big = false;
short * ff_invtab = new short [32767];

#ifdef __MWERKS__
#include <stuff_64.h>
int ff_mul( const int a, const int b )
{
  Int_64 c;
  unsigned int ua, ub;
  int res;
  
  if ((!a) || (!b)) return 0;
  if (a>0)
    ua = a;
  else
    ua = -a;
  if (b>0)
    ub = b;
  else
    ub = -b;
  mul_64(&c, ua, ub);
  res = rem_64(&c, ff_prime);
  if ((a>0) != (b>0))
    res = ff_prime-res;
  return res;
}
#endif

void ff_setprime ( const int p )
{
    if ( p != ff_prime ) {
	ff_prime = p;
	ff_halfprime = ff_prime / 2;
	if ( ! ff_big )
	    memset( ff_invtab, 0, ff_prime*sizeof(short) );
    }
}

int ff_newinv ( const int a )
{
    int p, q, r1, r2, y1, y2;
    if (a < 2)
	  return (ff_invtab[a] = a);
    r1 = p = ff_prime;
    q = r1 / a;
    y1 = -q;
    r1 -= a * q;
    if (r1 == 1)
    {
      y1 += p;
      ff_invtab[y1] = a;
      return (ff_invtab[a] = y1);
    }
    r2 = a;
    y2 = 1;
    for (;;)
    {
      q = r2 / r1;
      y2 -= y1 * q;
      r2 -= r1 * q;
      if (r2 == 1)
      {
        if (y2 < 0)
          y2 += p;
        ff_invtab[y2] = a;
        return (ff_invtab[a] = y2);
      }
      q = r1 / r2;
      y1 -= y2 * q;
      r1 -= r2 * q;
      if (r1 == 1)
      {
        if (y1 < 0)
          y1 += p;
        ff_invtab[y1] = a;
        return (ff_invtab[a] = y1);
      }
    }
}

int ff_biginv ( const int a )
{
    int p, q, r1, r2, y1, y2;
    if (a < 2)
      return a;
    r1 = p = ff_prime;
    q = r1 / a;
    y1 = -q;
    r1 -= a * q;
    if (r1 == 1)
      return p + y1;
    r2 = a;
    y2 = 1;
    for (;;)
    {
      q = r2 / r1;
      y2 -= y1 * q;
      r2 -= r1 * q;
      if (r2 == 1)
      {
        if (y2 > 0)
          return y2;
        else
          return p + y2;
      }
      q = r1 / r2;
      y1 -= y2 * q;
      r1 -= r2 * q;
      if (r1 == 1)
      {
        if (y1 > 0)
          return y1;
        else
          return p + y1;
      }
    }
}
