/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include <string.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "ffops.h"

int ff_prime = 0;
int ff_halfprime = 0;
bool ff_big = false;
short * ff_invtab = new short [32767];

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
