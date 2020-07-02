/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_util.cc
 *
 * miscellaneous functions, not necessarily related
 *   to canonical forms.
 *
 * Used by: fac_cantzass.cc, gfops.cc
 *
**/


#include "globaldefs.h"
#include "config.h"


/** int ipower ( int b, int m )
 *
 * ipower() - calculate b^m in standard integer arithmetic.
 *
 * Note: Beware of overflows.
 *
**/
int ipower ( int b, int m )
{
    int prod = 1;

    while ( m != 0 )
    {
        if ( m % 2 != 0 )
            prod *= b;
        m /= 2;
        if ( m != 0 )
            b *= b;
    }
    return prod;
}

int ilog2 (int v)
{
  const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  const unsigned int S[] = {1, 2, 4, 8, 16};

  unsigned int r = 0; // result of log2(v) will go here
  if (v & b[4]) { v >>= S[4]; r |= S[4]; }
  if (v & b[3]) { v >>= S[3]; r |= S[3]; }
  if (v & b[2]) { v >>= S[2]; r |= S[2]; }
  if (v & b[1]) { v >>= S[1]; r |= S[1]; }
  if (v & b[0]) { v >>= S[0]; r |= S[0]; }
  return (int)r;
}

int igcd( int a, int b )
{
    if ( a < 0 ) a = -a;
    if ( b < 0 ) b = -b;

    int c;

    while ( b != 0 )
    {
        c = a % b;
        a = b;
        b = c;
    }
    return a;
}

#include<stdio.h>
#include<stdlib.h>

void factoryError_intern(const char *s)
{
  fputs(s,stderr);
  abort();
}
VAR void (*factoryError)(const char *s) = factoryError_intern;


