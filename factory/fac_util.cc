// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_util.cc,v 1.3 1996-07-16 12:26:05 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.2  1996/07/08 08:22:02  stobbe
"New organization of the factorization stuff. Some functions moved from
fac_diophand.cc which no longer exists.
"

Revision 1.1  1996/06/27 11:34:24  stobbe
"New function dviremainder.
"

Revision 1.0  1996/05/17 10:59:46  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_util.h"

static CanonicalForm PK, PKHALF;

static CanonicalForm mappk ( const CanonicalForm& );

static CanonicalForm mappksymmetric ( const CanonicalForm& );


modpk::modpk()
{
    p = 3;
    k = 1;
    pk = power( CanonicalForm( p ), k );
    pkhalf = pk / 2;
}

modpk::modpk( int q, int l )
{
    p = q;
    k = l;
    pk = power( CanonicalForm( p ), k );
    pkhalf = pk / 2;
}

modpk::modpk( const modpk & m )
{
    p = m.p;
    k = m.k;
    pk = m.pk;
    pkhalf = m.pkhalf;
}

modpk&
modpk::operator= ( const modpk & m )
{
    if ( this != &m ) {
	p = m.p;
	k = m.k;
	pk = m.pk;
	pkhalf = m.pkhalf;
    }
    return *this;
}

CanonicalForm
modpk::inverse( const CanonicalForm & f, bool symmetric ) const
{
    CanonicalForm u, r0 = this->operator()( f, false ), r1 = pk, q0 = 1, q1 = 0;
    while ( ( r0 > 0 ) && ( r1 > 0 ) ) {
	u = r0 / r1;
	r0 = r0 % r1;
	q0 = u*q1 + q0;
	if ( r0 > 0 ) {
	    u = r1 / r0;
	    r1 = r1 % r0;
	    q1 = u*q0 + q1;
	}
    }
    if ( r0 == 0 )
	return this->operator()( pk-q1, symmetric );
    else
	return this->operator()( q0, symmetric );
}   

CanonicalForm
modpk::operator() ( const CanonicalForm & f, bool symmetric ) const
{
    PKHALF = pkhalf;
    PK = pk;
    if ( symmetric )
	return mapdomain( f, mappksymmetric );
    else
	return mapdomain( f, mappk );
};

CanonicalForm
replaceLc( const CanonicalForm & f, const CanonicalForm & c )
{
    if ( f.inCoeffDomain() )
	return c;
    else
	return f + ( c - LC( f ) ) * power( f.mvar(), degree( f ) );
}

CanonicalForm
remainder( const CanonicalForm & f, const CanonicalForm & g, const modpk & pk )
{
    ASSERT( (f.inCoeffDomain() || f.isUnivariate()) && (g.inCoeffDomain() || g.isUnivariate()) && (f.inCoeffDomain() || g.inCoeffDomain() || f.mvar() == g.mvar()), "can not build remainder" );
    if ( f.inCoeffDomain() )
	if ( g.inCoeffDomain() )
	    return pk( f % g );
	else
	    return pk( f );
    else {
	Variable x = f.mvar();
	CanonicalForm invlcg = pk.inverse( g.lc() );
	CanonicalForm result = f;
	int degg = g.degree();
	while ( result.degree() >= degg ) {
	    result = pk( result - lc( result ) * invlcg * g * power( x, result.degree() - degg ) );
	}
	return result;
    }
}

void
divremainder( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & quot, CanonicalForm & rem, const modpk & pk )
{
    ASSERT( (f.inCoeffDomain() || f.isUnivariate()) && (g.inCoeffDomain() || g.isUnivariate()) && (f.inCoeffDomain() || g.inCoeffDomain() || f.mvar() == g.mvar()), "can not build remainder" );
    if ( f.inCoeffDomain() )
	if ( g.inCoeffDomain() ) {
	    divrem( f, g, quot, rem );
	    quot = pk( quot );
	    rem = pk( rem );
	}
	else {
	    quot = 0;
	    rem = pk( f );
	}
    else {
	Variable x = f.mvar();
	CanonicalForm invlcg = pk.inverse( g.lc() );
	rem = f;
	int degg = g.degree();
	while ( rem.degree() >= degg ) {
	    quot += pk( lc( rem ) * invlcg ) * power( x, rem.degree() - degg );
	    rem = pk( rem - lc( rem ) * invlcg * g * power( x, rem.degree() - degg ) );
	}
    }
}

CanonicalForm
maxCoeff( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() )
	return abs( f );
    else {
	CanonicalForm M = 0, m;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    if ( (m = maxCoeff( i.coeff() )) > M )
		M = m;
	return M;
    }
}


CanonicalForm
mappksymmetric ( const CanonicalForm & f )
{
    CanonicalForm result = mod( f, PK );
    if ( result > PKHALF )
	return result - PK;
    else
	return result;
}

CanonicalForm
mappk ( const CanonicalForm & f )
{
    return mod( f, PK );
}

void
extgcd ( const CanonicalForm & a, const CanonicalForm & b, CanonicalForm & S, CanonicalForm & T, const modpk & pk )
{
    int p = pk.getp(), k = pk.getk(), j;
    CanonicalForm amodp, bmodp, smodp, tmodp, s, t, sigma, tau, e;
    CanonicalForm modulus = p, sigmat, taut, q;

    setCharacteristic( p );
    {
	amodp = mapinto( a ); bmodp = mapinto( b );
	(void)extgcd( amodp, bmodp, smodp, tmodp );
    }
    setCharacteristic( 0 );
    s = mapinto( smodp ); t = mapinto( tmodp );

    for ( j = 1; j < k; j++ ) {
	e = ( 1 - s * a - t * b ) / modulus;
	setCharacteristic( p );
	{
	    e = mapinto( e );
	    sigmat = smodp * e;
	    taut = tmodp * e;
	    divrem( sigmat, bmodp, q, sigma );
	    tau = taut + q * amodp;
	}
	setCharacteristic( 0 );
	s += mapinto( sigma ) * modulus;
	t += mapinto( tau ) * modulus;
	modulus *= p;
    }
    S = s; T = t;
}

CanonicalForm
sum ( const CFArray & a, int f, int l )
{
    if ( f < a.min() ) f = a.min();
    if ( l > a.max() ) l = a.max();
    CanonicalForm s = 0;
    for ( int i = f; i <= l; i++ )
	s += a[i];
    return s;
}

CanonicalForm
prod ( const CFArray & a, int f, int l )
{
    if ( f < a.min() ) f = a.min();
    if ( l > a.max() ) l = a.max();
    CanonicalForm p = 1;
    for ( int i = f; i <= l; i++ )
	p *= a[i];
    return p;
}

CanonicalForm
sum ( const CFArray & a )
{
    return sum( a, a.min(), a.max() );
}

CanonicalForm
prod ( const CFArray & a )
{
    return prod( a, a.min(), a.max() );
}

CanonicalForm
crossprod ( const CFArray & a, const CFArray & b )
{
    if ( a.size() != b.size() ) {
	cerr << "warning: array size mismatch." << endl;
	return 0;
    }
    CanonicalForm s = 0;
    int fa = a.min();
    int fb = b.min();
    int n = a.max();
    for ( ; fa <= n; fa++, fb++ )
	s += a[fa] * b[fb];
    return s;
}
