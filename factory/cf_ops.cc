/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_ops.cc,v 1.3 1997-07-19 08:09:28 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"

CanonicalForm
psr( const CanonicalForm &f, const CanonicalForm &g, const Variable & x )
{
    return ( power( LC(g,x), degree(f,x)-degree(g,x)+1 ) * f ) % g;
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

void
degreesRec ( const CanonicalForm & f, int * degs )
{
    if ( ! f.inCoeffDomain() ) {
	int level = f.level();
	int deg = f.degree();
	if ( degs[level] < deg )
	    degs[level] = f.degree();
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    degreesRec( i.coeff(), degs );
    }
}

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
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff() ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}


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
	CFIterator i;
	int cdeg = 0, dummy;
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 )) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
    else {
	CFIterator i;
	int cdeg = 0, dummy;
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}

//{{{ CanonicalForm resultant( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
//{{{ docu
//
// resultant() - return resultant of f and g with respect to x.
//
// We calculate the resultant using a subresultant PSR.
//
// flipFactor: Res(f, g) = flipFactor * Res(g, f)
// F, G: f and g with x as main variable
// pi, pi1, pi2: used to compute PSR
// delta:
// bi, Hi:
//
//}}}
CanonicalForm
resultant( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    CanonicalForm Hi, bi, pi, pi1, pi2, F, G;
    int delta, flipFactor;
    Variable v;

    ASSERT( x.level() > 0, "cannot calculate resultant in respect to algebraic variables" );

    // some checks on triviality.  We will not use degree( v )
    // here because this may involve variable swapping.
    if ( f.isZero() || g.isZero() )
	return 0;
    if ( f.mvar() < x )
	return power( f, g.degree( x ) );
    if ( g.mvar() < x )
	return power( g, f.degree( x ) );

    // make x main variale
    if ( f.mvar() > x || g.mvar() > x ) {
	if ( f.mvar() > g.mvar() )
	    v = f.mvar();
	else
	    v = g.mvar();
	F = swapvar( f, v, x );
	G = swapvar( g, v, x );
    }
    else {
	v = x;
	F = f;
	G = g;
    }
    // at this point, we have to calculate resultant( F, G, v )
    // where v is equal to or greater than the main variables
    // of F and G

    // trivial case: F or G in R.  Swapping will not occur
    // when calling degree( v ).
    if ( F.degree( v ) < 1 )
	return power( f, G.degree( v ) );
    if ( G.degree( v ) < 1 )
	return power( g, F.degree( v ) );

    // start the pseudo remainder sequence
    if ( F.degree( v ) >= G.degree( v ) ) {
	pi = F; pi1 = G;
	flipFactor = 1;
    }
    else {
	if ( (F.degree( v ) * G.degree( v )) % 2 )
	    flipFactor = -1;
	else
	    flipFactor = 1;
	pi = G; pi1 = F;
    }

    delta = degree( pi, v ) - degree( pi1, v );
    Hi = power( LC( pi1, v ), delta );

    // Ist hier nicht if und else zweig vertauscht ???
    if ( (delta+1) % 2 )
	bi = 1;
    else
	bi = -1;

    // Ist pi1.isZero vielleich schneller ???
    while ( degree( pi1, v ) >= 0 ) {
	pi2 = psr( pi, pi1, v );
	pi2 = pi2 / bi;
	pi = pi1; pi1 = pi2;
	if ( degree( pi1, v ) >= 0 ) {
	    delta = degree( pi, v ) - degree( pi1, v );

	    // Ist hier nicht if und else zweig vertauscht ???
	    if ( (delta+1) % 2 )
		bi = LC( pi, v ) * power( Hi, delta );
	    else
		bi = -LC( pi, v ) * power( Hi, delta );

	    // Was ist f"ur delta == 0 ???
	    Hi = power( LC( pi1, v ), delta ) / power( Hi, delta-1 );
	}
    }

    // f and g have non-trivial common divisor
    // if ( degree( pi, v ) > 0 )
    // return 0;

    // undo variable swap
    if ( v == x )
	// Gibt man hier nicht den letzten Rest der PSR zur"uck
	// und nicht den Korrekturterm Hi ???
	return Hi * flipFactor;
    else
	return swapvar( Hi, v, x ) * flipFactor;
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
