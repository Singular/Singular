/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_ops.cc,v 1.6 1997-09-01 09:06:19 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"

CanonicalForm
psr( const CanonicalForm &f, const CanonicalForm &g, const Variable & x )
{
    int m = f.degree( x );
    int n = g.degree( x );
    if ( m < n )
	return f;
    else
	return ( power( LC(g,x), m-n+1 ) * f ) % g;
}

CanonicalForm
psq( const CanonicalForm &f, const CanonicalForm &g, const Variable & x )
{
    return ( power( LC(g,x), degree(f,x)-degree(g,x)+1 ) * f ) / g;
}

void
psqr( const CanonicalForm &f, const CanonicalForm &g, CanonicalForm &q, CanonicalForm &r, const Variable& x )
{
    divrem( power( LC(g,x), degree(f,x)-degree(g,x)+1 ) * f, g, q, r );
}

static void swapvar_rec ( const CanonicalForm &f, CanonicalForm &result, const CanonicalForm &term );

static void swapvar_between ( const CanonicalForm &f, CanonicalForm &result, const CanonicalForm &term, int expx2 );

static Variable sv_x1, sv_x2;

CanonicalForm
swapvar ( const CanonicalForm &f, const Variable &x1, const Variable &x2 )
{
    ASSERT( x1.level() > 0 && x2.level() > 0, "cannot swap algebraic Variables" );
    if ( f.inCoeffDomain() || x1 == x2 || ( x1 > f.mvar() && x2 > f.mvar() ) )
	return f;
    else {
	CanonicalForm result = 0;
	if ( x1 > x2 ) {
	    sv_x1 = x2; sv_x2 = x1;
	}
	else {
	    sv_x1 = x1; sv_x2 = x2;
	}
	if ( f.mvar() < sv_x2 )
	    swapvar_between( f, result, 1, 0 );
	else
	    swapvar_rec( f, result, 1 );
	return result;
    }
}

void
swapvar_rec ( const CanonicalForm &f, CanonicalForm &result, const CanonicalForm &term )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	result += term * f;
    else  if ( f.mvar() == sv_x2 )
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_between( i.coeff(), result, term, i.exp() );
    else  if ( f.mvar() < sv_x2 )
	swapvar_between( f, result, term, 0 );
    else
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_rec( i.coeff(), result, term*power( f.mvar(), i.exp() ) );
}

void
swapvar_between ( const CanonicalForm &f, CanonicalForm &result, const CanonicalForm &term, int expx2 )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	result += term * power( sv_x1, expx2 ) * f;
    else  if ( f.mvar() == sv_x1 )
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += power( sv_x2, i.exp() ) * term * i.coeff() * power( sv_x1, expx2 );
    else
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_between( i.coeff(), result, term*power( f.mvar(), i.exp() ), expx2 );
}

CanonicalForm
apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) )
{
    if ( f.inCoeffDomain() ) {
	int exp = 0;
	CanonicalForm result;
	mf( result, exp );
	ASSERT( exp == 0, "illegal result, do not know what variable to use" );
	return result;
    }
    else {
	CanonicalForm result, coeff;
	CFIterator i;
	int exp;
	Variable x = f.mvar();
	for ( i = f; i.hasTerms(); i++ ) {
	    coeff = i.coeff();
	    exp = i.exp();
	    mf( coeff, exp );
	    if ( ! coeff.isZero() )
		result += power( x, exp ) * coeff;
	}
	return result;
    }
}

//{{{ static void degreesRec ( const CanonicalForm & f, int * degs )
//{{{ docu
//
// degreesRec() - recursively get degrees of f.
//
//}}}
static void
degreesRec ( const CanonicalForm & f, int * degs )
{
    if ( ! f.inCoeffDomain() ) {
	int level = f.level();
	int deg = f.degree();
	// calculate the maximum degree of all coefficients which
	// are in the same level
	if ( degs[level] < deg )
	    degs[level] = f.degree();
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    degreesRec( i.coeff(), degs );
    }
}
//}}}

//{{{ int * degrees ( const CanonicalForm & f, int * degs )
//{{{ docu
//
// degress() - return the degrees of all polynomial variables in f.
//
// Returns 0 if f is in a coefficient domain, the degrees of f in
// all its polynomial variables in an array of int otherwise:
//
//   degrees( f, 0 )[i] = degree( f, Variable(i) )
//
// If degs is not the zero pointer the degrees are stored in this
// array.  In this case degs should be larger than the level of
// f.  If degs is the zero pointer, an array of sufficient size
// is allocated automatically.
//
//}}}
int *
degrees ( const CanonicalForm & f, int * degs )
{
    if ( f.inCoeffDomain() )
	return 0;
    else {
	int level = f.level();
	if ( degs == 0 )
	    degs = new int[level+1];
	for ( int i = 0; i <= level; i++ )
	    degs[i] = 0;
	degreesRec( f, degs );
	return degs;
    }
}
//}}}

//{{{ CanonicalForm mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) )
//{{{ docu
//
// mapdomain() - map all coefficients of f through mf.
//
// Recursively descends down through f to the coefficients which
// are in a coefficient domain mapping each such coefficient
// through mf and returns the result.
//
//}}}
CanonicalForm
mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) )
{
    if ( f.inCoeffDomain() )
	return mf( f );
    else {
	CanonicalForm result = 0;
	CFIterator i;
	Variable x = f.mvar();
	for ( i = f; i.hasTerms(); i++ )
	    result += power( x, i.exp() ) * mapdomain( i.coeff(), mf );
	return result;
    }
}
//}}}

//{{{ int totaldegree ( const CanonicalForm & f )
//{{{ docu
//
// totaldegree() - return the total degree of f.
//
// If f is zero, return -1.  If f is in a coefficient domain,
// return 0.  Otherwise return the total degree of f in all
// polynomial variables.
//
//}}}
int
totaldegree ( const CanonicalForm & f )
{
    if ( f.isZero() )
	return -1;
    else if ( f.inCoeffDomain() )
	return 0;
    else {
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f, taking
	// in account our own exponent
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff() ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}
//}}}

//{{{ int totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 )
//{{{ docu
//
// totaldegree() - return the total degree of f as a polynomial
//   in the polynomial variables between v1 and v2 (inclusively).
//
// If f is zero, return -1.  If f is in a coefficient domain,
// return 0.  Also, return 0 if v1 > v2.  Otherwise, take f to be
// a polynomial in the polynomial variables between v1 and v2 and
// return its total degree.
//
//}}}
int
totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 )
{
    if ( f.isZero() )
	return -1;
    else if ( v1 > v2 )
	return 0;
    else if ( f.inCoeffDomain() )
	return 0;
    else if ( f.mvar() < v1 )
	return 0;
    else if ( f.mvar() == v1 )
	return f.degree();
    else if ( f.mvar() > v2 ) {
	// v2's level is larger than f's level, descend down
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 )) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
    else {
	// v1 < f.mvar() <= v2
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f, taking
	// in account our own exponent
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}
//}}}

//{{{ static void fillVarsRec ( const CanonicalForm & f, int * vars )
//{{{ docu
//
// fillVarsRec - fill array describing occurences of variables in f.
//
// Only polynomial variables are looked up.  The information is
// stored in the arrary vars.  vars should be large enough to
// hold all information, i.e. larger than the level of f.
//
//}}}
static void
fillVarsRec ( const CanonicalForm & f, int * vars )
{
    int n;
    if ( (n = f.level()) > 0 ) {
	vars[n] = 1;
	CFIterator i;
	for ( i = f; i.hasTerms(); ++i )
	    fillVarsRec( i.coeff(), vars );
    }
}
//}}}

//{{{ int getNumVars( const CanonicalForm & f )
//{{{ docu
//
// getNumVars() - get number of polynomial variables in f.
//
//}}}
int
getNumVars( const CanonicalForm & f )
{
    int n;
    if ( f.inCoeffDomain() )
	return 0;
    else  if ( (n = f.level()) == 1 )
	return 1;
    else {
	int * vars = new int[ n+1 ];
	int i;
	for ( i = 0; i < n; i++ ) vars[i] = 0;

	// look for variables
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );

	// count them
	int m = 0;
	for ( i = 1; i < n; i++ )
	    if ( vars[i] != 0 ) m++;

	delete [] vars;
	// do not forget to count our own variable
	return m+1;
    }
}
//}}}

//{{{ CanonicalForm getVars( const CanonicalForm & f )
//{{{ docu
//
// getVars() - get polynomial variables of f.
//
// Return the product of all of them, 1 if there are not any.
//
//}}}
CanonicalForm
getVars( const CanonicalForm & f )
{
    int n;
    if ( f.inCoeffDomain() )
	return 1;
    else  if ( (n = f.level()) == 1 )
	return Variable( 1 );
    else {
	int * vars = new int[ n+1 ];
	int i;
	for ( i = 0; i <= n; i++ ) vars[i] = 0;

	// look for variables
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );

	// multiply them all
	CanonicalForm result = 1;
	for ( i = n; i > 0; i-- )
	    if ( vars[i] != 0 ) result *= Variable( i );

	delete [] vars;
	// do not forget our own variable
	return f.mvar() * result;
    }
}
//}}}

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
