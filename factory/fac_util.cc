// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_util.cc,v 1.1 1996-06-27 11:34:24 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:46  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
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
