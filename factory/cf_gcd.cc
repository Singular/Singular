// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_gcd.cc,v 1.1 1996-06-03 08:32:56 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 11:56:37  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_chinese.h"
#include "templates/functions.h"

static CanonicalForm gcd_poly( const CanonicalForm & f, const CanonicalForm& g, bool modularflag );


static int
isqrt ( int a )
{
    int h, x0, x1 = a;
    do {
	x0 = x1;
	h = x0 * x0 + a - 1;
	if ( h % (2 * x0) == 0 )
	    x1 = h / (2 * x0);
	else
	    x1 = (h - 1)  / (2 * x0);
    } while ( x1 < x0 );
    return x1;
}

static bool
gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g )
{
    int count = 0;
    // assume polys have same level;
    CFRandom * sample = CFRandomFactory::generate();
    REvaluation e( 2, f.level(), *sample );
    delete sample;
    CanonicalForm lcf = swapvar( LC( f ), Variable(1), f.mvar() );
    CanonicalForm lcg = swapvar( LC( g ), Variable(1), f.mvar() );
    while ( ( e( lcf ).isZero() || e( lcg ).isZero() ) && count < 100 ) {
	e.nextpoint();
	count++;
    }
    if ( count == 100 )
	return false;
    CanonicalForm F=swapvar( f, Variable(1), f.mvar() );
    CanonicalForm G=swapvar( g, Variable(1), g.mvar() );
    if ( e(F).taildegree() > 0 && e(G).taildegree() > 0 )
	return false;
    return gcd( e( F ), e( G ) ).degree() < 1;
}

static CanonicalForm
maxnorm ( const CanonicalForm & f )
{
    CanonicalForm m = 0, h;
    CFIterator i;
    for ( i = f; i.hasTerms(); i++ )
	m = tmax( m, abs( i.coeff() ) );
    return m;
}

static void
chinesePoly ( const CanonicalForm & f1, const CanonicalForm & q1, const CanonicalForm & f2, const CanonicalForm & q2, CanonicalForm & f, CanonicalForm & q )
{
    CFIterator i1 = f1, i2 = f2;
    CanonicalForm c;
    Variable x = f1.mvar();
    f = 0;
    while ( i1.hasTerms() && i2.hasTerms() ) {
	if ( i1.exp() == i2.exp() ) {
	    chineseRemainder( i1.coeff(), q1, i2.coeff(), q2, c, q );
	    f += power( x, i1.exp() ) * c;
	    i1++; i2++;
	}
	else if ( i1.exp() > i2.exp() ) {
	    chineseRemainder( 0, q1, i2.coeff(), q2, c, q );
	    f += power( x, i2.exp() ) * c;
	    i2++;
	}
	else {
	    chineseRemainder( i1.coeff(), q1, 0, q2, c, q );
	    f += power( x, i1.exp() ) * c;
	    i1++;
	}
    }
    while ( i1.hasTerms() ) {
	chineseRemainder( i1.coeff(), q1, 0, q2, c, q );
	f += power( x, i1.exp() ) * c;
	i1++;
    }
    while ( i2.hasTerms() ) {
	chineseRemainder( 0, q1, i2.coeff(), q2, c, q );
	f += power( x, i2.exp() ) * c;
	i2++;
    }
}

static CanonicalForm
balance ( const CanonicalForm & f, const CanonicalForm & q )
{
    CFIterator i;
    CanonicalForm result = 0, qh = q / 2;
    Variable x = f.mvar();
    for ( i = f; i.hasTerms(); i++ ) {
	if ( i.coeff() > qh )
	    result += power( x, i.exp() ) * (i.coeff() - q);
	else
	    result += power( x, i.exp() ) * i.coeff();
    }
    return result;
}

CanonicalForm
igcd ( const CanonicalForm & f, const CanonicalForm & g )
{
    CanonicalForm a, b, c, dummy;

    if ( f.inZ() && g.inZ() && ! isOn( SW_RATIONAL ) ) {
	if ( f.sign() < 0 ) a = -f; else a = f;
	if ( g.sign() < 0 ) b = -g; else b = g;
	while ( ! b.isZero() ) {
	    divrem( a, b, dummy, c );
	    a = b;
	    b = c;
	}
	return a;
    }
    else
	return 1;
}

static CanonicalForm
icontent ( const CanonicalForm & f, const CanonicalForm & c )
{
    if ( f.inCoeffDomain() )
	return gcd( f, c );
    else {
	CanonicalForm g = c;
	for ( CFIterator i = f; i.hasTerms() && ! g.isOne(); i++ )
	    g = icontent( i.coeff(), g );
	return g;
    }
}

CanonicalForm
icontent ( const CanonicalForm & f )
{
    return icontent( f, 0 );
}

CanonicalForm
iextgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
{
    CanonicalForm p0 = f, p1 = g;
    CanonicalForm f0 = 1, f1 = 0, g0 = 0, g1 = 1, q, r;

    while ( ! p1.isZero() ) {
	divrem( p0, p1, q, r );
	p0 = p1; p1 = r;
	r = g0 - g1 * q;
	g0 = g1; g1 = r;
	r = f0 - f1 * q;
	f0 = f1; f1 = r;
    }
    a = f0;
    b = g0;
    if ( p0.sign() < 0 ) {
	p0 = -p0;
	a = -a;
	b = -b;
    }
    return p0;
}

CanonicalForm
extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
{
    CanonicalForm contf = content( f );
    CanonicalForm contg = content( g );

    CanonicalForm p0 = f / contf, p1 = g / contg;
    CanonicalForm f0 = 1, f1 = 0, g0 = 0, g1 = 1, q, r;

    while ( ! p1.isZero() ) {
	divrem( p0, p1, q, r );
	p0 = p1; p1 = r;
	r = g0 - g1 * q;
	g0 = g1; g1 = r;
	r = f0 - f1 * q;
	f0 = f1; f1 = r;
    }
    CanonicalForm contp0 = content( p0 );
    a = f0 / ( contf * contp0 );
    b = g0 / ( contg * contp0 );
    p0 /= contp0;
    if ( p0.sign() < 0 ) {
	p0 = -p0;
	a = -a;
	b = -b;
    }
    return p0;
}

static CanonicalForm
gcd_poly_univar0( const CanonicalForm & F, const CanonicalForm & G, bool primitive )
{
    CanonicalForm f, g, c, cg, cl, BB, B, M, q, Dp, newD, D, newq;
    int p, i, n;

    if ( primitive ) {
	f = F;
	g = G;
	c = 1;
    }
    else {
	CanonicalForm cF = content( F ), cG = content( G );
	f = F / cF;
	g = G / cG;
	c = igcd( cF, cG );
    }
    cg = gcd( f.lc(), g.lc() );
    cl = ( f.lc() / cg ) * g.lc();
//     B = 2 * cg * tmin( 
// 	maxnorm(f)*power(CanonicalForm(2),f.degree())*isqrt(f.degree()+1),
// 	maxnorm(g)*power(CanonicalForm(2),g.degree())*isqrt(g.degree()+1)
// 	)+1;
    M = tmin( maxnorm(f), maxnorm(g) );
    BB = power(CanonicalForm(2),tmin(f.degree(),g.degree()))*M;
    q = 0;
    i = 1;
    n = cf_getNumBigPrimes();
    while ( true ) {
	B = BB;
	while ( i < n && q < B ) {
	    p = cf_getBigPrime( i );
	    i++;
	    while ( i < n && mod( cl, p ) == 0 ) {
		p = cf_getBigPrime( i );
		i++;
	    }
	    setCharacteristic( p );
	    Dp = gcd( mapinto( f ), mapinto( g ) );
	    Dp = ( Dp / Dp.lc() ) * mapinto( cg );
	    setCharacteristic( 0 );
	    if ( Dp.degree() == 0 ) return c;
	    if ( q.isZero() ) {
		D = mapinto( Dp );
		q = p;
		B = power(CanonicalForm(2),D.degree())*M+1;
	    }
	    else {
		if ( Dp.degree() == D.degree() ) {
		    chinesePoly( D, q, mapinto( Dp ), p, newD, newq );
		    q = newq;
		    D = newD;
		}
		else if ( Dp.degree() < D.degree() ) {
		    // all previous p's are bad primes
		    q = p;
		    D = mapinto( Dp );
		    B = power(CanonicalForm(2),D.degree())*M+1;
		}
		// else p is a bad prime
	    }
	}
	if ( i < n ) {
	    // now balance D mod q
	    D = pp( balance( cg * D, q ) );
	    if ( divides( D, f ) && divides( D, g ) )
		return D * c;
	    else
		q = 0;
	}
	else {
	    return gcd_poly( F, G, false );
	}
    }
}


static CanonicalForm
gcd_poly( const CanonicalForm & f, const CanonicalForm& g, bool modularflag )
{
    CanonicalForm C, Ci, Ci1, Hi, bi, pi, pi1, pi2;
    int delta;
    Variable v = f.mvar();

    if ( f.degree( v ) >= g.degree( v ) ) {
	pi = f; pi1 = g;
    }
    else {
	pi = g; pi1 = f;
    }
    Ci = content( pi ); Ci1 = content( pi1 );
    C = gcd( Ci, Ci1 );
    pi1 = pi1 / Ci1; pi = pi / Ci;
    if ( ! pi.isUnivariate() ) {
	if ( gcd_test_one( pi1, pi ) )
	    return C;
    }
    else {
	// pi is univariate
	if ( modularflag )
	    return gcd_poly_univar0( pi, pi1, true ) * C;
    }
    delta = degree( pi, v ) - degree( pi1, v );
    Hi = power( LC( pi1, v ), delta );
    if ( (delta+1) % 2 )
	bi = 1;
    else
	bi = -1;
    while ( degree( pi1, v ) > 0 ) {
	pi2 = psr( pi, pi1, v );
	pi2 = pi2 / bi;
	pi = pi1; pi1 = pi2;
	if ( degree( pi1, v ) > 0 ) {
	    delta = degree( pi, v ) - degree( pi1, v );
	    if ( (delta+1) % 2 )
		bi = LC( pi, v ) * power( Hi, delta );
	    else
		bi = -LC( pi, v ) * power( Hi, delta );
	    Hi = power( LC( pi1, v ), delta ) / power( Hi, delta-1 );
	}
    }
    if ( degree( pi1, v ) == 0 )
	return C;
    else {
	return C * pp( pi );
    }
}


static CanonicalForm
cf_content ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) ) {
	CFIterator i = f;
	CanonicalForm result = g;
	while ( i.hasTerms() && ! result.isOne() ) {
	    result = gcd( result, i.coeff() );
	    i++;
	}
	return result;
    }
    else
	if ( f.sign() < 0 )
	    return -f;
	else
	    return f;
}

CanonicalForm
content ( const CanonicalForm & f )
{
    return cf_content( f, 0 );
}

CanonicalForm
content ( const CanonicalForm & f, const Variable & x )
{
    if ( f.mvar() == x )
	return cf_content( f, 0 );
    else  if ( f.mvar() < x )
	return f;
    else
	return swapvar( content( swapvar( f, f.mvar(), x ), f.mvar() ), f.mvar(), x );
}

CanonicalForm
vcontent ( const CanonicalForm & f, const Variable & x )
{
    if ( f.mvar() <= x )
	return content( f, x );
    else {
	CFIterator i;
	CanonicalForm d = 0;
	for ( i = f; i.hasTerms() && ! d.isOne(); i++ )
	    d = gcd( d, vcontent( i.coeff(), x ) );
	return d;
    }
}

CanonicalForm
pp ( const CanonicalForm & f )
{
    if ( f.isZero() )
	return f;
    else
	return f / content( f );
}

CanonicalForm
gcd ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.isZero() )
	return g;
    else  if ( g.isZero() )
	return f;
    else  if ( f.inBaseDomain() )
	if ( g.inBaseDomain() )
	    return igcd( f, g );
	else
	    return cf_content( g, f );
    else  if ( g.inBaseDomain() )
	return cf_content( f, g );
    else  if ( f.mvar() == g.mvar() )
	if ( f.inExtension() && getReduce( f.mvar() ) )
	    return 1;
	else {
	    if ( getCharacteristic() == 0 && isOn( SW_RATIONAL ) ) {
		Off( SW_RATIONAL );
		CanonicalForm l = lcm( common_den( f ), common_den( g ) );
		On( SW_RATIONAL );
		CanonicalForm F = f * l, G = g * l;
		Off( SW_RATIONAL );
		l = gcd_poly( F, G, true );
		On( SW_RATIONAL );
		return l;
	    }
	    else
		return gcd_poly( f, g, getCharacteristic()==0 );
	}
    else  if ( f.mvar() > g.mvar() )
	return cf_content( f, g );
    else
	return cf_content( g, f );
}

CanonicalForm
lcm ( const CanonicalForm & f, const CanonicalForm & g )
{
    return ( f / gcd( f, g ) ) * g;
}
