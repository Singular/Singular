/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_sqrfree.cc,v 1.11 2008-05-31 17:21:11 Singular Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"

static int divexp = 1;

static void divexpfunc ( CanonicalForm &, int & e )
{
    e /= divexp;
}

static int
compareFactors( const CFFactor & f, const CFFactor & g )
{
    return f.exp() > g.exp();
}

CFFList
sortCFFList( CFFList & F )
{
    F.sort( compareFactors );

    int exp;
    CanonicalForm f;
    CFFListIterator I = F;
    CFFList result;

    // join elements with the same degree
    while ( I.hasItem() ) {
	f = I.getItem().factor();
	exp = I.getItem().exp();
	I++;
	while ( I.hasItem() && I.getItem().exp() == exp ) {
	    f *= I.getItem().factor();
	    I++;
	}
	result.append( CFFactor( f, exp ) );
    }

    return result;
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
    while ( t0.degree(x) > 0 )
    {
	t = gcd( t0, t0.deriv() );
	v = t0 / t;
	k = 0;
	while ( v.degree(x) > 0 )
        {
	    k = k+1;
	    if ( k % p == 0 )
            {
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
    if ( ! leadcf.isOne() )
    {
	if ( !F.isEmpty() && (F.getFirst().exp() == 1) )
        {
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

bool isSqrFreeZ ( const CanonicalForm & f )
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
