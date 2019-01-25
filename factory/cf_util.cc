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


#include "factory/globaldefs.h"
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

int ilog2 (int a)
{
  int n = -1;
  while ( a > 0 )
  {
    n++;
    a /=2;
  }
  return n;
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


