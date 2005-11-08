/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ffops.h,v 1.8 2005-11-08 16:03:18 Singular Exp $ */

#ifndef INCL_FFOPS_H
#define INCL_FFOPS_H

#include <config.h>

#include "cf_globals.h"

extern int ff_prime;
extern int ff_halfprime;
extern short * ff_invtab;
extern bool ff_big;

int ff_newinv ( const int );
int ff_biginv ( const int );
void ff_setprime ( const int );

inline int ff_norm ( const int a )
{
    int n = a % ff_prime;
    if ( n < 0 )
	return n + ff_prime;
    else
	return n;
}

inline int ff_symmetric( const int a )
{
    if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
	return ( a > ff_halfprime ) ? a - ff_prime : a;
    else
	return a;
}

inline int ff_longnorm ( const long a )
{
    int n = (int)(a % (long)ff_prime);
    if ( n < 0 )
	return n + ff_prime;
    else
	return n;
}

#ifndef __MWERKS__
inline int ff_bignorm ( const INT64 a )
{
    int n = (int)(a % (INT64)ff_prime);
    if ( n < 0 )
	return n + ff_prime;
    else
	return n;
}
#endif

inline int ff_add ( const int a, const int b )
{
    //return ff_norm( a + b );
    int r=( a + b );
    if (r >= ff_prime) r -= ff_prime;
    return r;
}

inline int ff_sub ( const int a, const int b )
{
    //return ff_norm( a - b );
    int r=( a - b );
    if (r <0) r += ff_prime;
    return r;
}

inline int ff_neg ( const int a )
{
    //return ff_norm( -a );
    if (a==0) return 0;
    return ff_prime-a;
}

#ifdef __MWERKS__
int ff_mul ( const int a, const int b );
#else
inline int ff_mul ( const int a, const int b )
{
    if ( ff_big )
	return ff_bignorm( (INT64)a * (INT64)b );
    else
	return ff_longnorm ( (long)a * (long)b );
}
#endif

inline int ff_inv ( const int a )
{
    if ( ff_big )
	return ff_biginv( a );
    else {
	register int b;
	if ( (b = (int)(ff_invtab[a])) )
	    return b;
	else
	    return ff_newinv( a );
    }

}

inline int ff_div ( const int a, const int b )
{
    return ff_mul( a, ff_inv( b ) );
}

#endif /* ! INCL_FFOPS_H */
