// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_univar.cc,v 1.4 1996-07-12 08:37:20 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.3  1996/06/26 13:17:03  stobbe
"ZFactorizeUnivariate: now handles the sign of the argument right.
"

Revision 1.2  1996/06/13 10:43:49  stobbe
"ZFactorizeUnivariate: fix to last bug fix (no assignment)
"

Revision 1.1  1996/06/13 10:34:04  stobbe
"ZFactorizeUnivariate: do not use Berlekamp-Algorithm since there is a
                      bug in the Factory-Implementation of Berlekamp
"

Revision 1.0  1996/05/17 10:59:45  stobbe
Initial revision

*/

//#define TIMING

#ifndef NDEBUG
//#define DEBUGOUTPUT
#endif

#ifdef TIMING
#include <sys/times.h>
#endif

#include <math.h>
#include "assert.h"
#include "cf_defs.h"
#include "fac_util.h"
#include "fac_univar.h"
#include "fac_cantzass.h"
#include "fac_berlekamp.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "fac_sqrfree.h"

#define MAX_FP_FAC 3

static modpk theModulus;

CanonicalForm
iextgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b );

#ifndef NDEBUG

static void
hprint ( int * a )
{
    int n = a[0];
    cerr << "( " << n << ": ";
    int i = 1;
    while ( i < n ) {
	if ( a[i] != 0 )
	    cerr << i << " ";
	i++;
    }
    cerr << ")" << endl;
}

#endif

static void
hgroup ( int * a )
{
    int n = a[0];
    int i, j, k;
    for ( i = 1; i < n; i++ )
	if ( a[i] != 0 )
	    for ( j = 1; j <= i; j++ )
		if ( a[j] != 0 )
		    for ( k = i; k < n; k += j )
			a[k] = 1;
}

static void
hintersect( int * a, const int * const b )
{
    int i, n, na = a[0], nb = b[0];
    if ( nb < na )
	n = nb;
    else
	n = na;
    for ( i = 1; i < n; i++ )
	if ( b[i] == 0 )
	    a[i] = 0;
    a[0] = n;
}

/*
static int
hcount ( const int * const a )
{
    int n = a[0], sum = 0, i;
    for ( i = 1; i < n; i++ )
	if ( a[i] != 0 ) sum++;
    return sum;
}
*/

static void
initHG ( int * a, const CFFList & F )
{
    ListIterator<CFFactor> i;

    int n = a[0], k;
    for ( int j = 1; j < n; j++ ) a[j] = 0;
    for ( i = F; i.hasItem(); i++ )
	if ( (k = i.getItem().factor().degree()) < n )
	    if ( k != 0 )
		a[k] = 1;
}

static void
initHG ( int * a, const Array<CanonicalForm> & F )
{
    int i, n = a[0], m = F.size(), k;
    for ( i = 1; i < n; i++ ) a[i] = 0;
    for ( i = 1; i < m; i++ )
	if ( (k = F[i].degree()) < n )
	    if ( k != 0 )
		a[k] = 1;
}

static int
cmpFactor( const CFFactor & a, const CFFactor & b )
{
    CFFactor A( a ), B( b );
    return degree( A.factor() ) > degree( B.factor() );
}

static double
cf2double ( const CanonicalForm & f )
{
    CanonicalForm a = f, q, r;
    double m = 1, res = 0;
    if ( a.sign() < 0 ) a = -a;
    while ( ! a.isZero() ) {
	divrem( a, 10, q, r );
	res += m * (double)(r.intval());
	m *= 10;
	a = q;
    }
    if ( f.sign() < 0 ) res = -res;
    return res;
}

static double
dnorm ( const CanonicalForm & f )
{
    CFIterator i;
    CanonicalForm sum = 0;
    for ( i = f; i.hasTerms(); i++ ) sum += i.coeff() * i.coeff();
    DEBOUTLN( cerr, "sum = ", sum );
    return sqrt( cf2double( sum ) );
}

static int
kBound ( const CanonicalForm & f, int p )
{
    DEBOUTLN( cerr, "lc(f) = ", lc(f) );
    return (int)((f.degree()*log(2)+log( fabs(cf2double(lc(f))) )+log( dnorm( f ) )) / log( p ) + 0.5) + 1;
}

modpk
getZFacModulus()
{
    return theModulus;
}

static bool
liftDegreeFactRec( CFArray & theFactors, CanonicalForm & F, const CanonicalForm & recip_lf, const CanonicalForm & f, const modpk & pk, int i, int d, CFFList & ZF, int exp )
{
    if ( i >= theFactors.size() )
	return false;
    else  if ( degree( f ) + degree( theFactors[i] ) == d ) {
	DEBOUTLN( cerr, "ldfr (f) = ", f );
	DEBOUTLN( cerr, "ldfr (g) = ", theFactors[i] );
	CanonicalForm g = pp( pk( recip_lf * f * theFactors[i] ) );
	DEBOUTLN( cerr, "ldfr (pk(f*g)) = ", g );
	CanonicalForm gg, hh;
	DEBOUTLN( cerr, "F = ", F );
	DEBOUTLN( cerr, "g = ", g );
	if ( divremt( F, g, gg, hh ) && hh.isZero() ) {
	    ZF.append( CFFactor( g, exp ) );
	    F = gg;
	    theFactors[i] = 1;
	    return true;
	}
	else {
	    return liftDegreeFactRec( theFactors, F, recip_lf, f, pk, i+1, d, ZF, exp );
	}
    }
    else  if ( degree( f ) + degree( theFactors[i] ) > d )
	return false;
    else {
	bool ok = liftDegreeFactRec( theFactors, F, recip_lf, pk( recip_lf * f * theFactors[i] ), pk, i+1, d, ZF, exp );
	if ( ok )
	    theFactors[i] = 1;
	else
	    ok = liftDegreeFactRec( theFactors, F, recip_lf, f, pk, i+1, d, ZF, exp );
	return ok;
    }
}

static int
choosePrimes ( int * p, const CanonicalForm & f )
{
    int ptr = 0;
    int i = 0;
    int maxp = cf_getNumPrimes();
    int prime;

    while ( ptr < maxp && i < MAX_FP_FAC ) {
	prime = cf_getPrime( ptr );
	if ( mod( lc( f ), prime ) != 0 ) {
	    setCharacteristic( prime );
	    if ( isSqrFreeFp( mapinto( f ) ) ) {
		p[i] = prime;
		i++;
	    }
	    setCharacteristic( 0 );
	}
	ptr++;
    }
    return ( i == MAX_FP_FAC );
}


static int
UnivariateQuadraticLift ( const CanonicalForm &F, const  CanonicalForm & G, const CanonicalForm &H, const modpk & pk, const CanonicalForm & Gamma, CanonicalForm & gk, CanonicalForm & hk )
{
    CanonicalForm lf, f, gamma;
    CanonicalForm a, b, aa, bb, c, g, h, g1, h1, e, modulus, tmp, q, r;
    int i, j, save;
    int p = pk.getp(), k = pk.getk();
    int no_iter = (int)(log(k)/log(2)+2);
    int * kvals = new int[no_iter];

    DEBOUT( cerr, "quadratic lift called with p = ", p );
    DEBOUTLN( cerr, "  and k = ", k );
    for ( j = 0, i = k; i > 1; i = ( i+1 ) / 2, j++ ) kvals[j] = i;
    kvals[j] = 1;

    save = getCharacteristic(); setCharacteristic( 0 );

    lf = lc( F );
    f = lf * F;
    {
	setCharacteristic( p );
	g1 = mapinto( lf ) / lc( G ) * G;
	h1 = mapinto( lf ) / lc( H ) * H;
	(void)extgcd( g1, h1, a, b );
	setCharacteristic( 0 );
    }
    a = mapinto( a ); b = mapinto( b );
    g = mapinto( g1 ); h = mapinto( h1 );
    g = replaceLc( g, lf ); h = replaceLc( h, lf );
    e = f - g * h;
    modulus = p;
    i = 1;

    while ( ! e.isZero() && j > 0 ) {
	c = div( e, modulus );
	{
	    j--;
	    setCharacteristic( p, kvals[j+1] );
	    DEBOUT( cerr, "lifting from p^", kvals[j+1] );
	    DEBOUTLN( cerr, " to p^", kvals[j] );
	    c = mapinto( c );
	    DEBOUTLN( cerr, " !!! g = ", mapinto( g ) );
	    g1 = mapinto( lf ) / mapinto( lc( g ) ) * mapinto( g );
	    h1 = mapinto( lf ) / mapinto( lc( h ) ) * mapinto( h );
//	    (void)extgcd( g1, h1, a, b );
//	    DEBOUTLN( cerr, " a = ", aa );
//	    DEBOUTLN( cerr, " b = ", bb );
	    a = mapinto( a ); b = mapinto( b );
	    a += ( ( 1 - a * g1 ) *  a ) % h1;
	    b += ( ( 1 - b * h1 ) *  b ) % g1;
	    DEBOUTLN( cerr, " a = ", a );
	    DEBOUTLN( cerr, " b = ", b );
	    divrem( a * c, h1, q, r );
	    tmp = b * c + q * g1;
	    setCharacteristic( 0 );
	}
	a = mapinto( a ); b = mapinto( b );
	g += mapinto( tmp ) * modulus;
	h += mapinto( r ) * modulus;
//	g = replaceLc( g, lf ); h = replaceLc( h, lf );
	e = f - g * h;
	modulus = power( CanonicalForm(p), kvals[j] );
	if ( mod( f - g * h, modulus ) != 0 )
	    DEBOUTLN( cerr, "error at lift stage ", i );
	i++;
    }
    if ( e.isZero() ) {
	tmp = content( g );
	gk = g / tmp; hk = h / ( lf / tmp );
    }
    else {
	gk = pk(g); hk = pk(h);
    }
    setCharacteristic( save );
    return e.isZero();
}

static int
UnivariateLinearLift ( const CanonicalForm &F, const  CanonicalForm & G, const CanonicalForm &H, const modpk & pk, const CanonicalForm & Gamma, CanonicalForm & gk, CanonicalForm & hk )
{
    CanonicalForm lf, f, gamma;
    CanonicalForm a, b, c, g, h, g1, h1, e, modulus, tmp, q, r;
    int i, save;
    int p = pk.getp(), k = pk.getk();
    save = getCharacteristic(); setCharacteristic( 0 );

    lf = lc( F );
    f = lf * F;
    {
	setCharacteristic( p );
	g1 = mapinto( lf ) / lc( G ) * G;
	h1 = mapinto( lf ) / lc( H ) * H;
	(void)extgcd( g1, h1, a, b );
	setCharacteristic( 0 );
    }
    g = mapinto( g1 ); h = mapinto( h1 );
    g = replaceLc( g, lf ); h = replaceLc( h, lf );
    e = f - g * h;
    modulus = p;
    i = 1;

    while ( ! e.isZero() && i <= k ) {
	c = div( e, modulus );
	{
	    setCharacteristic( p );
	    c = mapinto( c );
	    divrem( a * c, h1, q, r );
	    tmp = b * c + q * g1;
	    setCharacteristic( 0 );
	}
	g += mapinto( tmp ) * modulus;
	h += mapinto( r ) * modulus;
//	g = replaceLc( g, lf ); h = replaceLc( h, lf );
	e = f - g * h;
	modulus *= p;
	ASSERT( mod( f - g * h, modulus ) == 0, "error at lift stage" );
	i++;
    }
    if ( e.isZero() ) {
	tmp = content( g );
	gk = g / tmp; hk = h / ( lf / tmp );
    }
    else {
	gk = pk(g); hk = pk(h);
    }
    setCharacteristic( save );
//    return e.isZero();
    return (F-gk*hk).isZero();
}

CFFList
ZFactorizeUnivariate( const CanonicalForm& ff, bool issqrfree )
{
    bool symmsave = isOn( SW_SYMMETRIC_FF );
    CanonicalForm cont = content( ff );
    CanonicalForm lf, recip_lf, fp, f, g = ff / cont, dummy1, dummy2;
    int i, k, exp, n;
    bool ok;
    CFFList H, G, F[MAX_FP_FAC];
    CFFList ZF;
    int * p = new int [MAX_FP_FAC];
    int * D = 0;
    int * Dh = 0;
    ListIterator<CFFactor> J, I;
    On( SW_SYMMETRIC_FF );
    if ( issqrfree )
	H.append( CFFactor( g, 1 ) );
    else
	H = sqrFree( g );
    for ( J = H; J.hasItem(); ++J ) {
	f = J.getItem().factor();
	if ( f.inCoeffDomain() ) continue;
	n = f.degree() / 2 + 1;
	if ( D != 0 ) {
	    delete [] D;
	    delete [] Dh;
	}
	D = new int [n]; D[0] = n;
	Dh = new int [n]; Dh[0] = n;
	exp = J.getItem().exp();
#ifdef TIMING
	struct tms ts, te;
	times( &ts );
#endif
	ok = choosePrimes( p, f );
#ifdef TIMING
	times( &te );
	cout.setf( ios::fixed, ios::floatfield );
	cout.precision(2);
	cout << "time to choose the primes: "
	     << float(te.tms_utime-ts.tms_utime) / CLK_TCK << " sec" << endl;
#endif
	if ( ! ok ) {
	    cerr << "factorize warnig: no good prime found" << endl;
	    cerr << f << endl;
	    ZF.append( CFFactor( f, exp ) );
	}
	else {
#ifdef TIMING
	    times( &ts );
#endif
	    for ( i = 0; i < MAX_FP_FAC; i++ ) {
		setCharacteristic( p[i] );
		fp = mapinto( f );
		F[i] = FpFactorizeUnivariateCZ( fp, true );
//		if ( p[i] < 23 && fp.degree() < 10 )
//		    F[i] = FpFactorizeUnivariateB( fp, true );
//		else
//		    F[i] = FpFactorizeUnivariateCZ( fp, true );
		DEBOUT( cerr, "F[i] = ", F[i] );
		DEBOUTLN( cerr, ", p = ", p[i] );
	    }
#ifdef TIMING
	times( &te );
	cout.setf( ios::fixed, ios::floatfield );
	cout.precision(2);
	cout << "time to factorize mod primes: "
	     << float(te.tms_utime-ts.tms_utime) / CLK_TCK << " sec" << endl;
#endif
	    setCharacteristic( 0 );
#ifdef DEBUGOUTPUT
	    hprint( D );
#endif
	    initHG( D, F[0] );
	    hgroup( D );
#ifdef DEBUGOUTPUT
	    hprint( D );
#endif
	    for ( i = 1; i < MAX_FP_FAC; i++ ) {
		initHG( Dh, F[i] );
		hgroup( Dh );
#ifdef DEBUGOUTPUT
		cerr << "Dh = ";
		hprint( Dh );
#endif
		hintersect( D, Dh );
#ifdef DEBUGOUTPUT
		cerr << "D = ";
		hprint( D );
#endif

	    }
	    int min, j;
	    min = F[0].length(), j = 0;
	    for ( i = 1; i < MAX_FP_FAC; i++ ) {
		if ( min >= F[i].length() ) {
		    j = i; min = F[i].length();
		}
	    }
	    k = kBound( f, p[j] );
	    CFArray theFactors( F[j].length() );
//	    pk = power( CanonicalForm( p[j] ), k );
//	    pkhalf = pk / 2;
	    modpk pk( p[j], k );
	    DEBOUTLN( cerr, "coeff bound = ", pk.getpk() );
	    theModulus = pk;
	    setCharacteristic( p[j] );
	    fp = mapinto( f );
	    F[j].sort( cmpFactor );
	    I = F[j]; i = 0;
#ifdef TIMING
	    times( &ts );
#endif
	    while ( I.hasItem() ) {
		DEBOUTLN( cerr, "factor to lift = ", I.getItem().factor() );
		if ( isOn( SW_FAC_QUADRATICLIFT ) )
		    ok = UnivariateQuadraticLift( f, I.getItem().factor(), fp / I.getItem().factor(), pk, lc( f ), dummy1, dummy2 );
		else
		    ok = UnivariateLinearLift( f, I.getItem().factor(), fp / I.getItem().factor(), pk, lc( f ), dummy1, dummy2 );
		if ( ok ) {
		    // should be done in a more efficient way
		    DEBOUTLN( cerr, "dummy1 = ", dummy1 );
		    DEBOUTLN( cerr, "dummy2 = ", dummy2 );
		    f = dummy2;
		    fp /= I.getItem().factor();
		    ZF.append( CFFactor( dummy1, exp ) );
		    I.remove( 0 ); 
		    I = F[j];
		    i = 0;
		    DEBOUTLN( cerr, "F[j] = ", F[j] );
		}
		else {
		    DEBOUTLN( cerr, "i = ", i );
		    DEBOUTLN( cerr, "dummy1 = ", dummy1 );
		    setCharacteristic( 0 );
//		    theFactors[i] = pk( dummy1 * pk.inverse( lc( dummy1 ) ) );
		    theFactors[i] = pk( dummy1 );
		    setCharacteristic( p[j] );
		    i++;
		    I++;
		}
	    }
#ifdef TIMING
	times( &te );
	cout.setf( ios::fixed, ios::floatfield );
	cout.precision(2);
	cout << "time to lift the factors: "
	     << float(te.tms_utime-ts.tms_utime) / CLK_TCK << " sec" << endl;
#endif
	    DEBOUTLN( cerr, "ZF = ", ZF );
	    initHG( Dh, theFactors );
	    hgroup( Dh );
#ifdef DEBUGOUTPUT
	    cerr << "Dh = ";
	    hprint( Dh );
#endif
	    hintersect( D, Dh );
	    setCharacteristic( 0 );
	    for ( int l = i; l < F[j].length(); l++ )
		theFactors[l] = 1;
	    DEBOUTLN( cerr, "theFactors = ", theFactors );
	    DEBOUTLN( cerr, "f = ", f );
	    DEBOUT( cerr, "p = ", pk.getp() );
	    DEBOUTLN( cerr, ", k = ", pk.getk() );
#ifdef DEBUGOUTPUT
	    hprint( D );
#endif
	    lf = lc( f );
	    (void)iextgcd( pk.getpk(), lf, dummy1, recip_lf );
	    DEBOUTLN( cerr, "recip_lf = ", recip_lf );
#ifdef TIMING
	    times( &ts );
#endif
	    for ( i = 1; i < D[0]; i++ )
		if ( D[i] != 0 )
		    while ( liftDegreeFactRec( theFactors, f, recip_lf, lf, pk, 0, i, ZF, exp ) );
	    if ( degree( f ) > 0 )
		ZF.append( CFFactor( f, exp ) );
#ifdef TIMING
	times( &te );
	cout.setf( ios::fixed, ios::floatfield );
	cout.precision(2);
	cout << "time to combine the factors: "
	     << float(te.tms_utime-ts.tms_utime) / CLK_TCK << " sec" << endl;
#endif
	}
    }
    if ( ZF.getFirst().factor().inCoeffDomain() )
	ZF.removeFirst();
    if ( lc( ff ).sign() < 0 )
	ZF.insert( CFFactor( -cont, 1 ) );
    else
	ZF.insert( CFFactor( cont, 1 ) );
    if ( D != 0 ) {
	delete [] D;
	delete [] Dh;
    }
    if ( ! symmsave )
	Off( SW_SYMMETRIC_FF );
    return ZF;
}
