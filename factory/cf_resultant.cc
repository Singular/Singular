/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_resultant.cc - algorithms for calculating resultants.
//
// Header file: cf_algorithm.h
//
//}}}

#include <config.h>

#include "assert.h"

#include "canonicalform.h"
#include "variable.h"
#include "cf_algorithm.h"

//{{{ CFArray subResChain ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// subResChain() - caculate extended subresultant chain.
//
// The chain is calculated from f and g with respect to variable
// x which should not be an algebraic variable.  If f or q equals
// zero, an array consisting of one zero entry is returned.
//
// Note: this is not the standard subresultant chain but the
// *extended* chain!
//
// This algorithm is from the article of R. Loos - 'Generalized
// Polynomial Remainder Sequences' in B. Buchberger - 'Computer
// Algebra - Symbolic and Algebraic Computation' with some
// necessary extensions concerning the calculation of the first
// step.
//
//}}}
CFArray
subResChain ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate subresultant sequence with respect to algebraic variables" );

    CFArray trivialResult( 0, 0 );
    CanonicalForm F, G;
    Variable X;

    // some checks on triviality
    if ( f.isZero() || g.isZero() ) {
        trivialResult[0] = 0;
        return trivialResult;
    }

    // make x main variable
    if ( f.mvar() > x || g.mvar() > x ) {
        if ( f.mvar() > g.mvar() )
            X = f.mvar();
        else
            X = g.mvar();
        F = swapvar( f, X, x );
        G = swapvar( g, X, x );
    }
    else {
        X = x;
        F = f;
        G = g;
    }
    // at this point, we have to calculate the sequence of F and
    // G in respect to X where X is equal to or greater than the
    // main variables of F and G

    // initialization of chain
    int m = degree( F, X );
    int n = degree( G, X );

    int j = (m <= n) ? n : m-1;
    int r;

    CFArray S( 0, j+1 );
    CanonicalForm R;
    S[j+1] = F; S[j] = G;

    // make sure that S[j+1] is regular and j < n
    if ( m == n && j > 0 ) {
        S[j-1] = LC( S[j], X ) * psr( S[j+1], S[j], X );
        j--;
    } else if ( m < n ) {
        S[j-1] = LC( S[j], X ) * LC( S[j], X ) * S[j+1];
        j--;
    } else if ( m > n && j > 0 ) {
        // calculate first step
        r = degree( S[j], X );
        R = LC( S[j+1], X );

        // if there was a gap calculate similar polynomial
        if ( j > r && r >= 0 )
            S[r] = power( LC( S[j], X ), j - r ) * S[j] * power( R, j - r );

        if ( r > 0 ) {
            // calculate remainder
            S[r-1] = psr( S[j+1], S[j], X ) * power( -R, j - r );
            j = r-1;
        }
    }

    while ( j > 0 ) {
        // at this point, 0 < j < n and S[j+1] is regular
        r = degree( S[j], X );
        R = LC( S[j+1], X );

        // if there was a gap calculate similar polynomial
        if ( j > r && r >= 0 )
            S[r] = (power( LC( S[j], X ), j - r ) * S[j]) / power( R, j - r );

        if ( r <= 0 ) break;
        // calculate remainder
        S[r-1] = psr( S[j+1], S[j], X ) / power( -R, j - r + 2 );

        j = r-1;
        // again 0 <= j < r <= jOld and S[j+1] is regular
    }

    for ( j = 0; j <= S.max(); j++ ) {
        // reswap variables if necessary
        if ( X != x ) {
            S[j] = swapvar( S[j], X, x );
        }
    }

    return S;
}
//}}}

//{{{ static CanonicalForm trivialResultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// trivialResultant - calculate trivial resultants.
//
// x's level should be larger than f's and g's levels.  Either f
// or g should be constant or both linear.
//
// Used by resultant().
//
//}}}
static CanonicalForm
trivialResultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    // f or g in R
    if ( degree( f, x ) == 0 )
        return power( f, degree( g, x ) );
    if ( degree( g, x ) == 0 )
        return power( g, degree( f, x ) );

    // f and g are linear polynomials
    return LC( f, x ) * g - LC( g, x ) * f;
}
//}}}

//{{{ CanonicalForm resultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// resultant() - return resultant of f and g with respect to x.
//
// The chain is calculated from f and g with respect to variable
// x which should not be an algebraic variable.  If f or q equals
// zero, zero is returned.  If f is a coefficient with respect to
// x, f^degree(g, x) is returned, analogously for g.
//
// This algorithm serves as a wrapper around other resultant
// algorithms which do the real work.  Here we use standard
// properties of resultants only.
//
//}}}
CanonicalForm
resultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate resultant with respect to algebraic variables" );

    // some checks on triviality.  We will not use degree( v )
    // here because this may involve variable swapping.
    if ( f.isZero() || g.isZero() )
        return 0;
    if ( f.mvar() < x )
        return power( f, g.degree( x ) );
    if ( g.mvar() < x )
        return power( g, f.degree( x ) );

    // make x main variale
    CanonicalForm F, G;
    Variable X;
    if ( f.mvar() > x || g.mvar() > x ) {
        if ( f.mvar() > g.mvar() )
            X = f.mvar();
        else
            X = g.mvar();
        F = swapvar( f, X, x );
        G = swapvar( g, X, x );
    }
    else {
        X = x;
        F = f;
        G = g;
    }
    // at this point, we have to calculate resultant( F, G, X )
    // where X is equal to or greater than the main variables
    // of F and G

    int m = degree( F, X );
    int n = degree( G, X );
    // catch trivial cases
    if ( m+n <= 2 || m == 0 || n == 0 )
        return swapvar( trivialResultant( F, G, X ), X, x );

    // exchange F and G if necessary
    int flipFactor;
    if ( m < n ) {
        CanonicalForm swap = F;
        F = G; G = swap;
        int degswap = m;
        m = n; n = degswap;
        if ( m & 1 && n & 1 )
            flipFactor = -1;
        else
            flipFactor = 1;
    } else
        flipFactor = 1;

    // this is not an effective way to calculate the resultant!
    CanonicalForm extFactor;
    if ( m == n ) {
        if ( n & 1 )
            extFactor = -LC( G, X );
        else
            extFactor = LC( G, X );
    } else
        extFactor = power( LC( F, X ), m-n-1 );

    CanonicalForm result;
    result = subResChain( F, G, X )[0] / extFactor;

    return swapvar( result, X, x ) * flipFactor;
}
//}}}
