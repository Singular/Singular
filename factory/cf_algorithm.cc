/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_algorithm.cc,v 1.1 1997-09-04 15:30:20 schmidt Exp $ */

//{{{ docu
//
// cf_algorithm.cc - simple mathematical algorithms.
//
// A 'mathematical' algorithm is an algorithm which calculates
// some mathematical function in contrast to a 'structural'
// algorithm which gives structural information on polynomials.
//
// Compare these functions with the functions in cf_ops.cc, which
// are structural algorithms.
//
//}}}

#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "variable.h"
#include "cf_iter.h"

CanonicalForm
psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    int m = f.degree( x );
    int n = g.degree( x );
    if ( m < n )
	return f;
    else
	return ( power( LC( g, x ), m-n+1 ) * f ) % g;
}

CanonicalForm
psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    return ( power( LC( g, x ), degree( f, x )-degree( g, x )+1 ) * f ) / g;
}

void
psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable& x )
{
    divrem( power( LC( g, x ), degree( f, x )-degree( g, x )+1 ) * f, g, q, r );
}

static CanonicalForm
cden ( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() )
	return f.den();
    else {
	CFIterator i;
	CanonicalForm cd = 1;
	for ( i = f; i.hasTerms(); i++ )
	    cd = lcm( cd, cden( i.coeff() ) );
	return cd;
    }
}

CanonicalForm
common_den ( const CanonicalForm & f )
{
    if ( getCharacteristic() == 0 && isOn( SW_RATIONAL ) ) {
	Off( SW_RATIONAL );
	CanonicalForm cd = cden( f );
	On( SW_RATIONAL );
	return cd;
    }
    else
	return 1;
}
