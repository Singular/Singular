/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_util.h"

STATIC_INST_VAR CanonicalForm PK, PKHALF;

static CanonicalForm mappk ( const CanonicalForm& );

static CanonicalForm mappksymmetric ( const CanonicalForm& );


modpk::modpk()
{
    p = 0;
    k = 0;
    pk = 1;
    pkhalf = 0;
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
}

CanonicalForm
replaceLc( const CanonicalForm & f, const CanonicalForm & c )
{
    if ( f.inCoeffDomain() )
        return c;
    else
        return f + ( c - LC( f ) ) * power( f.mvar(), degree( f ) );
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

