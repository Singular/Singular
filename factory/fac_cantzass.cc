/* emacs edit mode for this file is -*- C++ -*- */

#include <config.h>

#include "factory/cf_gmp.h"

#include "assert.h"

#include "cf_defs.h"
#include "cf_random.h"
#include "cf_util.h"
#include "fac_cantzass.h"
#include "fac_sqrfree.h"
#include "gmpext.h"

#ifdef HAVE_FLINT
#include"FLINTconvert.h"
#endif

#if !defined(HAVE_NTL)
#if !defined(HAVE_FLINT)||(__FLINT_RELEASE==20600)
static CanonicalForm randomPoly( int n, const Variable & x, const CFRandom & gen );

static CFFList CantorZassenhausFactorFFGF( const CanonicalForm & f, int d, int q, const CFRandom & );

static CFFList CantorZassenhausFactorExt( const CanonicalForm & g, int s, mpz_t q, const CFRandom & gen );

static CFFList distinctDegreeFactorFFGF ( const CanonicalForm & f, int q );

static CFFList distinctDegreeFactorExt ( const CanonicalForm & f, int p, int n );

// calculates f^m % d

static CanonicalForm powerMod( const CanonicalForm & f, int m, const CanonicalForm & d );

// calculates f^(p^s) % d

static CanonicalForm powerMod( const CanonicalForm & f, int p, int s, const CanonicalForm & d );

// calculates f^((p^s-1)/2) % d

static CanonicalForm powerMod2( const CanonicalForm & f, int p, int s, const CanonicalForm & d );

static CanonicalForm powerMod2( const CanonicalForm & f, mpz_t q, int s, const CanonicalForm & d );

CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f, bool issqrfree, int numext, const Variable alpha, const Variable beta )
{
    CFFList F, G, H, HH;
    CanonicalForm fac;
    ListIterator<CFFactor> i, j, k;
    int d, q, n = 0;
    bool galoisfield = getGFDegree() > 1;
    mpz_t qq;

    if ( galoisfield )
        q = ipower( getCharacteristic(), getGFDegree() );
    else
        q = getCharacteristic();
    if ( numext > 0 ) {
        if ( numext == 1 )
            n = getMipo( alpha ).degree();
        else
            n = getMipo( alpha ).degree() * getMipo( beta ).degree();
        mpz_init( qq );
        mpz_ui_pow_ui ( qq, q, n );
    }
    if ( LC( f ).isOne() )
        if ( issqrfree )
            F.append( CFFactor( f, 1 ) );
        else
            F = sqrFreeFp( f );
    else {
        if ( issqrfree )
            F.append( CFFactor( f / LC( f ), 1 ) );
        else
            F = sqrFreeFp( f / LC( f ) );
        H.append( LC( f ) );
    }
    for ( i = F; i.hasItem(); ++i ) {
        d = i.getItem().exp();
        if ( numext > 0 )
            G = distinctDegreeFactorExt( i.getItem().factor(), q, n );
        else
            G = distinctDegreeFactorFFGF( i.getItem().factor(), q );
        for ( j = G; j.hasItem(); ++j ) {
            if ( numext > 0 ) {
             if ( numext == 1 ) {
                   AlgExtRandomF tmpalpha( alpha );
                    HH = CantorZassenhausFactorExt( j.getItem().factor(), j.getItem().exp(), qq, tmpalpha );
             }
             else {
                   AlgExtRandomF tmpalphabeta( alpha, beta );
                    HH = CantorZassenhausFactorExt( j.getItem().factor(), j.getItem().exp(), qq, tmpalphabeta );
             }
            }
            else  if ( galoisfield )
                HH = CantorZassenhausFactorFFGF( j.getItem().factor(), j.getItem().exp(), q, GFRandom() );
            else
                HH = CantorZassenhausFactorFFGF( j.getItem().factor(), j.getItem().exp(), q, FFRandom() );
            for ( k = HH; k.hasItem(); ++k ) {
                fac = k.getItem().factor();
                H.append( CFFactor( fac / LC( fac ), d ) );
            }
        }
    }
    if ( numext > 0 )
        mpz_clear( qq );
    return H;
}

CFFList distinctDegreeFactorFFGF ( const CanonicalForm & f, int q )
{
    int i;
    Variable x = f.mvar();
    CanonicalForm g = f, h, r = x;
    CFFList F;
    i = 1;
    while ( g.degree(x) > 0 && i <= g.degree(x) ) {
        r = powerMod( r, q, g );
        h = gcd( g, r - x );
        if ( h.degree(x) > 0 ) {
            F.append( CFFactor( h, i ) );
            g /= h;
        }
        i++;
    }
    ASSERT( g.degree(x) == 0, "fatal fatal" );
    return F;
}

CFFList distinctDegreeFactorExt ( const CanonicalForm & f, int p, int n )
{
    int i;
    Variable x = f.mvar();
    CanonicalForm g = f, h, r = x;
    CFFList F;
    i = 1;
    while ( g.degree(x) > 0 && i <= g.degree(x) ) {
        r = powerMod( r, p, n, g );
        h = gcd( g, r - x );
        if ( h.degree(x) > 0 ) {
            F.append( CFFactor( h, i ) );
            g /= h;
        }
        i++;
    }
    ASSERT( g.degree(x) == 0, "fatal fatal" );
    return F;
}

CFFList CantorZassenhausFactorFFGF( const CanonicalForm & g, int s, int q, const CFRandom & gen )
{
    CanonicalForm f = g;
    CanonicalForm b, f1;
    int d, d1;
    Variable x = f.mvar();

    if ( (d=f.degree(x)) == s )
        return CFFactor( f, 1 );
    else while ( 1 ) {
        b = randomPoly( d, x, gen );
        f1 = gcd( b, f );
        if ( (d1 = f1.degree(x)) > 0 && d1 < d ) {
            CFFList firstFactor = CantorZassenhausFactorFFGF( f1, s, q, gen );
            CFFList secondFactor = CantorZassenhausFactorFFGF( f/f1, s, q, gen );
            return Union( firstFactor, secondFactor );
        } else {
            f1 = gcd( f, powerMod2( b, q, s, f ) - 1 );
            if ( (d1 = f1.degree(x)) > 0 && d1 < d ) {
                CFFList firstFactor = CantorZassenhausFactorFFGF( f1, s, q, gen );
                CFFList secondFactor = CantorZassenhausFactorFFGF( f/f1, s, q, gen );
                return Union( firstFactor, secondFactor );
            }
        }
    }
}

CFFList CantorZassenhausFactorExt( const CanonicalForm & g, int s, mpz_t q, const CFRandom & gen )
{
    CanonicalForm f = g;
    CanonicalForm b, f1;
    int d, d1;
    Variable x = f.mvar();

    if ( (d=f.degree(x)) == s )
        return CFFactor( f, 1 );
    else while ( 1 ) {
        b = randomPoly( d, x, gen );
        f1 = gcd( b, f );
        if ( (d1 = f1.degree(x)) > 0 && d1 < d ) {
            CFFList firstFactor = CantorZassenhausFactorExt( f1, s, q, gen );
            CFFList secondFactor = CantorZassenhausFactorExt( f/f1, s, q, gen );
            return Union( firstFactor, secondFactor );
        } else {
            f1 = gcd( f, powerMod2( b, q, s, f ) - 1 );
            if ( (d1 = f1.degree(x)) > 0 && d1 < d ) {
                CFFList firstFactor = CantorZassenhausFactorExt( f1, s, q, gen );
                CFFList secondFactor = CantorZassenhausFactorExt( f/f1, s, q, gen );
                return Union( firstFactor, secondFactor );
            }
        }
    }
}

CanonicalForm randomPoly( int d, const Variable & x, const CFRandom & g )
{
    CanonicalForm result = 0;
    for ( int i = 0; i < d; i++ )
        result += power( x, i ) * g.generate();
    result += power( x, d );
    return result;
}

CanonicalForm powerMod( const CanonicalForm & f, int m, const CanonicalForm & d )
{
    CanonicalForm prod = 1;
    CanonicalForm b = f % d;

    while ( m != 0 ) {
        if ( m % 2 != 0 )
            prod = (prod * b) % d;
        m /= 2;
        if ( m != 0 )
            b = (b * b) % d;
    }
    return prod;
}

CanonicalForm powerMod( const CanonicalForm & f, int p, int s, const CanonicalForm & d )
{
    CanonicalForm prod = 1;
    CanonicalForm b = f % d;
    int odd;

    mpz_t m;

    mpz_init( m );
    mpz_ui_pow_ui ( m, p, s );
    while ( mpz_cmp_si( m, 0 ) != 0 )
    {
        odd = mpz_fdiv_q_ui( m, m, 2 );
        if ( odd != 0 )
            prod = (prod * b) % d;
        if ( mpz_cmp_si( m, 0 ) != 0 )
            b = (b*b) % d;
    }
    mpz_clear( m );
    return prod;
}

CanonicalForm powerMod2( const CanonicalForm & f, int p, int s, const CanonicalForm & d )
{
    CanonicalForm prod = 1;
    CanonicalForm b = f % d;
    int odd;

    mpz_t m;

    mpz_init( m );
    mpz_ui_pow_ui ( m, p, s );
    mpz_sub_ui( m, m, 1 );
    mpz_fdiv_q_ui( m, m, 2 );
    while ( mpz_cmp_si( m, 0 ) != 0 )
    {
        odd = mpz_fdiv_q_ui( m, m, 2 );
        if ( odd != 0 )
            prod = (prod * b) % d;
        if ( mpz_cmp_si( m, 0 ) != 0 )
            b = (b*b) % d;
    }
    mpz_clear( m );
    return prod;
}

CanonicalForm powerMod2( const CanonicalForm & f, mpz_t q, int s, const CanonicalForm & d )
{
    CanonicalForm prod = 1;
    CanonicalForm b = f % d;
    int odd;

    mpz_t m;

    mpz_init( m );
    mpz_pow_ui( m, q, s );
    mpz_sub_ui( m, m, 1 );
    mpz_fdiv_q_ui( m, m, 2 );
    while ( mpz_cmp_si( m, 0 ) != 0 )
    {
        odd = mpz_fdiv_q_ui( m, m, 2 );
        if ( odd != 0 )
            prod = (prod * b) % d;
        if ( mpz_cmp_si( m, 0 ) != 0 )
            b = (b*b) % d;
    }
    mpz_clear( m );
    return prod;
}
#endif
#endif
