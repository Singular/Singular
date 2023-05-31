/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_util.h"
#include "cfUnivarGcd.h"

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
        CanonicalForm result = f;
        int degg = g.degree();
        CanonicalForm invlcg = pk.inverse( g.lc() );
        CanonicalForm gg = pk( g*invlcg );
        if((gg.lc().isOne()))
        {
          while ( result.degree() >= degg )
          {
            result -= pk(lc( result ) * gg) * power( x, result.degree() - degg );
            result=pk(result);
          }
        }
        else
        // no inverse found
        {
          CanonicalForm ic=icontent(g);
          if (!ic.isOne())
          {
            gg=g/ic;
            return remainder(f,gg,pk);
          }
          while ( result.degree() >= degg )
          {
            if (gg.lc().isZero()) return result;
            CanonicalForm lcgf = result.lc() / gg.lc();
            if (lcgf.inZ())
              gg = pk( g*lcgf );
            else
            {
              //printf("!\n\n");
              return result;
            }
            result -=  gg * power( x, result.degree() - degg );
            result=pk(result);
          }
        }
        return result;
    }
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
prod ( const CFArray & a )
{
    return prod( a, a.min(), a.max() );
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
