// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_ops.cc,v 1.1 1997-04-07 16:04:13 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 11:56:37  stobbe
Initial revision

*/

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

CanonicalForm
resultant( const CanonicalForm & f, const CanonicalForm& g, const Variable & x )
{
    CanonicalForm Hi, bi, pi, pi1, pi2, F, G;
    int delta;
    Variable v = f.mvar();

    if ( f.mvar() < x || g.mvar() < x )
	return 1;

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
    if ( F.degree( v ) < 1 || G.degree( v ) < 1 )
	return 1;

    if ( f.degree( v ) >= g.degree( v ) ) {
	pi = F; pi1 = G;
    }
    else {
	pi = G; pi1 = F;
    }
    delta = degree( pi, v ) - degree( pi1, v );
    Hi = power( LC( pi1, v ), delta );
    if ( (delta+1) % 2 )
	bi = 1;
    else
	bi = -1;
    while ( degree( pi1, v ) >= 0 ) {
	pi2 = psr( pi, pi1, v );
	pi2 = pi2 / bi;
	pi = pi1; pi1 = pi2;
	if ( degree( pi1, v ) >= 0 ) {
	    delta = degree( pi, v ) - degree( pi1, v );
	    if ( (delta+1) % 2 )
		bi = LC( pi, v ) * power( Hi, delta );
	    else
		bi = -LC( pi, v ) * power( Hi, delta );
	    Hi = power( LC( pi1, v ), delta ) / power( Hi, delta-1 );
	}
    }
    if ( v == x )
	return Hi;
    else
	return swapvar( Hi, v, x );
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
