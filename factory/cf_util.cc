/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_util.cc - miscellaneous functions, not necessarily related
//   to canonical forms.
//
// Used by: fac_cantzass.cc, gfops.cc
//
//}}}

#include <config.h>

//{{{ int ipower ( int b, int m )
//{{{ docu
//
// ipower() - calculate b^m in standard integer arithmetic.
//
// Note: Beware of overflows.
//
//}}}
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
//}}}

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

#include<stdio.h>

void (*factoryError_callback)(const char *s) = 0;

void factoryError(const char *s)
{
  if (factoryError_callback!=0) factoryError_callback(s);
  else fprintf(stderr,s);
}

