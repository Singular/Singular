/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_algorithm.cc,v 1.5 1997-10-09 14:48:19 schmidt Exp $ */

//{{{ docu
//
// cf_algorithm.cc - simple mathematical algorithms.
//
// A 'mathematical' algorithm is an algorithm which calculates
// some mathematical function in contrast to a 'structural'
// algorithm which gives structural information on polynomials.
//
// Compare these functions to the functions in cf_ops.cc, which are
// structural algorithms.
//
// Used by: cf_gcd.cc, cf_resultant.cc, fac_distrib.cc,
//   fac_ezgcd.cc, sm_sparsemod.cc
//
//}}}

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "variable.h"
#include "cf_iter.h"

//{{{ CanonicalForm psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// psr() - calculate pseudo remainder of f and g with respect to x.
//
// x should be a polynomial variable.
//
// For f and g in R[x], R an integral domain, g != 0, there is a
// unique representation
//
//   lc(g)^s*f = g*q + r
//
// with r = 0 or deg(r) < deg(g) and s = 0 if f = 0 or
// s = max( 0, deg(f)-deg(g)+1 ) otherwise.
// Then r = psr(f, g) and q = psq(f, g) are called pseudo
// remainder and pseudo quotient, resp.
//
// See H.-J. Reiffen/G. Scheja/U. Vetter - 'Algebra', 2nd ed.,
// par. 15 for a reference.
//
//}}}
CanonicalForm
psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate pseudo remainder/quotient with respect to algebraic variables" );
    ASSERT( g != 0, "division by zero" );

    int m = degree( f, x );
    int n = degree( g, x );
    if ( m < 0 || m < n )
	return f;
    else
	return ( power( LC( g, x ), m-n+1 ) * f ) % g;
}
//}}}

//{{{ CanonicalForm psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// psq() - calculate pseudo quotient of f and g with respect to x.
//
// x should be a polynomial variable.  See psr() for more
// detailed information.
//
//}}}
CanonicalForm
psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate pseudo remainder/quotient with respect to algebraic variables" );
    ASSERT( g != 0, "division by zero" );

    int m = degree( f, x );
    int n = degree( g, x );
    if ( m < 0 || m < n )
	return 0;
    else
	return ( power( LC( g, x ), m-n+1 ) * f ) / g;
}
//}}}

//{{{ void psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable & x )
//{{{ docu
//
// psqr() - calculate pseudo quotient and remainder of f and g with respect to x.
//
// x should be a polynomial variable.  See psr() for more
// detailed information.
//
//}}}
void
psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable& x )
{
    ASSERT( x.level() > 0, "cannot calculate pseudo remainder/quotient with respect to algebraic variables" );
    ASSERT( g != 0, "division by zero" );

    int m = degree( f, x );
    int n = degree( g, x );
    if ( m < 0 || m < n ) {
	q = 0; r = f;
    }
    else
	divrem( power( LC( g, x ), m-n+1 ) * f, g, q, r );
}
//}}}

//{{{ static CanonicalForm cden ( const CanonicalForm & f )
//{{{ docu
//
// cden() - recursively calculate multivariate common denominator
//   of coefficients of f.
//
// Used by: common_den()
//
//}}}
static CanonicalForm
cden ( const CanonicalForm & f )
{
    if ( f.inBaseDomain() )
	return f.den();
    else {
	CFIterator i;
	CanonicalForm cd = 1;
	for ( i = f; i.hasTerms(); i++ )
	    cd = lcm( cd, cden( i.coeff() ) );
	return cd;
    }
}
//}}}

//{{{ CanonicalForm common_den ( const CanonicalForm & f )
//{{{ docu
//
// common_den() - calculate multivariate common denominator of
//   coefficients of f.
//
// The common denominator is calculated with respect to all
// coefficients of f which are in a base domain.  In other words,
// common_den( f ) * f is guaranteed to have integer
// coefficients only.
//
// Returns one for domains other than Q.
//
//}}}
CanonicalForm
common_den ( const CanonicalForm & f )
{
    if ( getCharacteristic() == 0 && isOn( SW_RATIONAL ) ) {
	// switch to Z so lcm() will work correctly
	Off( SW_RATIONAL );
	CanonicalForm cd = cden( f );
	On( SW_RATIONAL );
	return cd;
    }
    else
	return 1;
}
//}}}

//{{{ bool divides ( const CanonicalForm & f, const CanonicalForm & g )
//{{{ docu
//
// divides() - check whether f divides g.
//
// Uses some extra checks to avoid polynomial division.
//
//}}}
bool
divides ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( g.level() > 0 && g.level() == f.level() )
	if ( divides( f.tailcoeff(), g.tailcoeff() ) && divides( f.LC(), g.LC() ) ) {
	    CanonicalForm q, r;
	    bool ok = divremt( g, f, q, r );
	    return ok && r == 0;
	}
	else
	    return false;
    else {
	CanonicalForm q, r;
	bool ok = divremt( g, f, q, r );
	return ok && r == 0;
    }
}
//}}}
