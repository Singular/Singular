/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_resultant.cc,v 1.1 1997-09-01 09:01:15 schmidt Exp $ */

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

//{{{ CFArray subResChain ( const CanonicalForm & f, const CanonicalForm & g, Variable x )
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
subResChain ( const CanonicalForm & f, const CanonicalForm & g, Variable x )
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
