/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file ffops.h
 *
 * operations in a finite prime field F_p.
 * The largest supported p is 536870909, i.e. the largest prime less than 2^29.
 *
**/

#ifndef INCL_FFOPS_H
#define INCL_FFOPS_H

// #include "config.h"

#include "cf_globals.h"
#ifdef HAVE_NTL
#include <NTL/config.h>
#endif

/* define type of your compilers 64 bit integer type */
#ifndef FACTORY_INT64
#if SIZEOF_LONG == 8
#define FACTORY_INT64 long int
#else
#define FACTORY_INT64 long long int
#endif
#endif

EXTERN_VAR int ff_prime;
EXTERN_VAR int ff_halfprime;
EXTERN_VAR short * ff_invtab;
EXTERN_VAR bool ff_big;

int ff_newinv ( const int );
int ff_biginv ( const int );
void ff_setprime ( const int );

inline int ff_norm ( const int a )
{
    int n = a % ff_prime;
#if defined(NTL_AVOID_BRANCHING)
    n += (n >> 31) & ff_prime;
    return n;
#else
    if (n < 0) n += ff_prime;
    return n;
#endif
}

inline long ff_norm ( const long a )
{
    long n = a % (long)ff_prime;
#if defined(NTL_AVOID_BRANCHING)
    #if SIZEOF_LONG==8
    n += (n >> 63) & ff_prime;
    #else
    n += (n >> 31) & ff_prime;
    #endif
    return n;
#else
    if (n < 0) n += (long)ff_prime;
    return n;
#endif
}

inline int ff_symmetric( const int a )
{
    if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
        return ( a > ff_halfprime ) ? a - ff_prime : a;
    else
        return a;
}

inline long ff_symmetric( const long a )
{
    if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
        return ( a > ff_halfprime ) ? a - ff_prime : a;
    else
        return a;
}

#if SIZEOF_LONG==4
inline int ff_bignorm ( const FACTORY_INT64 a )
{
    int n = (int)(a % (FACTORY_INT64)ff_prime);
#if defined(NTL_AVOID_BRANCHING)
    n += (n >> 31) & ff_prime;
    return n;
#else
    if (n < 0) n += ff_prime;
    return n;
#endif
}
#endif

inline int ff_add ( const int a, const int b )
{
    //return ff_norm( a + b );
#if defined(NTL_AVOID_BRANCHING)
    int r=( a + b );
    r -= ff_prime;
    r += (r >> 31) & ff_prime;
    return r;
#else
    int r=( a + b );
    if (r >= ff_prime) r -= ff_prime;
    return r;
#endif
}

inline int ff_sub ( const int a, const int b )
{
    //return ff_norm( a - b );
#if defined(NTL_AVOID_BRANCHING)
    int r=( a - b );
    r += (r >> 31) & ff_prime;
    return r;
#else
    int r=( a - b );
    if (r < 0) r += ff_prime;
    return r;
#endif
}

inline int ff_neg ( const int a )
{
    //return ff_norm( -a );
// EXPERIMENT
#if defined(NTL_AVOID_BRANCHING)
    int r= -a;
    r += (r >> 31) & ff_prime;
    return r;
#else
    return ( a == 0 ? 0 : ff_prime-a );
#endif
}

inline int ff_mul ( const int a, const int b )
{
#if SIZEOF_LONG==4
    if ( ff_big )
        return ff_bignorm( (FACTORY_INT64)a * (FACTORY_INT64)b );
    else
#endif /* else: FACTORY_INT64 is long, i.e. ff_bignorm is the same as ff_norm(long) */
        return ff_norm ( (long)a * (long)b );
}

inline int ff_inv ( const int a )
{
    if ( ff_big )
        return ff_biginv( a );
    else {
        int b;
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
