// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_sqrfree.cc,v 1.3 1996-12-05 18:24:55 schmidt Exp $

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"

/*
$Log: not supported by cvs2svn $
Revision 1.2  1996/06/26 13:15:28  stobbe
"sqrFreeZ: Now handles the sign of the argument right.
"

Revision 1.1  1996/05/20 13:39:48  stobbe
"sqrFree: Now the product of all factors found by sqrFree is equal to the
         parameter of sqrFree. The bug resulted from an incorrect handling
         of the leading coefficient of the argument of sqrFree.
"

// Revision 1.0  1996/05/17  10:59:45  stobbe
// Initial revision
//
*/

static int divexp = 1;

static void divexpfunc ( CanonicalForm &, int & e )
{
    e /= divexp;
}

CFFList sqrFreeFp ( const CanonicalForm & f )
{
    CanonicalForm t0 = f, t, v, w, h;
    CanonicalForm leadcf = t0.lc();
    Variable x = f.mvar();
    CFFList F;
    int p = getCharacteristic();
    int k, e = 1;

    if ( ! leadcf.isOne() )
	t0 /= leadcf;

    divexp = p;
    while ( t0.degree(x) > 0 ) {
	t = gcd( t0, t0.deriv() );
	v = t0 / t;
	k = 0;
	while ( v.degree(x) > 0 ) {
	    k = k+1;
	    if ( k % p == 0 ) {
		t /= v;
		k = k+1;
	    }
	    w = gcd( t, v );
	    h = v / w;
	    v = w;
	    t /= v;
	    if ( h.degree(x) > 0 )
		F.append( CFFactor( h/h.lc(), e*k ) );
	}
	t0 = apply( t, divexpfunc );
	e = p * e;
    }
    if ( ! leadcf.isOne() ) {
	if ( F.getFirst().exp() == 1 ) {
	    leadcf = F.getFirst().factor() * leadcf;
	    F.removeFirst();
	}
	F.insert( CFFactor( leadcf, 1 ) );
    }
    return F;
}

bool isSqrFreeFp( const CanonicalForm & f )
{
  CFFList F = sqrFreeFp( f );
  return ( F.length() == 1 && F.getFirst().exp() == 1 );
}

int isSqrFreeZ ( const CanonicalForm & f )
{
    return gcd( f, f.deriv() ).degree() == 0;
}

/*

CFFList sqrFreeZ ( const CanonicalForm & a )
{
    CanonicalForm b = a.deriv(), c = gcd( a, b );
    CanonicalForm y, z, w = a / c;
    int i = 1;
    CFFList F;

    while ( ! c.degree() == 0 ) {
	y = gcd( w, c ); z = w / y;
	if ( degree( z ) > 0 )
	    if ( lc( z ).sign() < 0 )
		F.append( CFFactor( -z, i ) );
	    else
		F.append( CFFactor( z, i ) );
	i++;
	w = y; c = c / y;
    }
    if ( degree( w ) > 0 )
	if ( lc( w ).sign() < 0 )
	    F.append( CFFactor( -w, i ) );
	else
	    F.append( CFFactor( w, i ) );
    return F;
}
*/

CFFList sqrFreeZ ( const CanonicalForm & a )
{
    if ( a.inCoeffDomain() )
	return CFFactor( a, 1 );
    CanonicalForm cont = content( a );
    CanonicalForm aa = a / cont;
    CanonicalForm b = aa.deriv(), c = gcd( aa, b );
    CanonicalForm y, z, w = aa / c;
    int i = 1;
    CFFList F;
    Variable v = aa.mvar();
    while ( ! c.degree(v) == 0 ) {
	y = gcd( w, c ); z = w / y;
	if ( degree( z, v ) > 0 )
	    if ( lc( z ).sign() < 0 )
		F.append( CFFactor( -z, i ) );
	    else
		F.append( CFFactor( z, i ) );
	i++;
	w = y; c = c / y;
    }
    if ( degree( w,v ) > 0 )
	if ( lc( w ).sign() < 0 )
	    F.append( CFFactor( -w, i ) );
	else
	    F.append( CFFactor( w, i ) );
    if ( ! cont.isOne() )
	F = Union( F, sqrFreeZ( cont ) );
    if ( lc( a ).sign() < 0 ) {
	if ( F.getFirst().exp() == 1 ) {
	    CanonicalForm f = F.getFirst().factor();
	    CFFListIterator(F).getItem() = CFFactor( -f, 1 );
	}
	else
	    F.insert( CFFactor( -1, 1 ) );
    }
    return F;
}
