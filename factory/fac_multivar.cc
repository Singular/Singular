// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_multivar.cc,v 1.1 1996-07-23 09:18:35 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:45  stobbe
Initial revision

*/

#define TIMING
#undef DEBUGOUTPUT

#include "timing.h"

#include "assert.h"
#include "cf_defs.h"
#include "fac_multivar.h"
#include "fac_univar.h"
#include "cf_reval.h"
#include "cf_map.h"
#include "fac_util.h"
#include "cf_binom.h"
#include "cf_iter.h"


TIMING_DEFINE_PRINT(fac_content);
TIMING_DEFINE_PRINT(fac_findeval);
TIMING_DEFINE_PRINT(fac_distrib);


static CFArray
conv_to_factor_array( const CFFList & L )
{
    int n;
    CFFListIterator I = L;
    bool negate = false;

    if ( ! I.hasItem() )
	n = 0;
    else  if ( I.getItem().factor().inBaseDomain() ) {
	negate = I.getItem().factor().sign() < 0;
	I++;
	n = L.length();
    }
    else
	n = L.length() + 1;
    CFFListIterator J = I;
    while ( J.hasItem() ) {
	n += J.getItem().exp() - 1;
	J++;
    }
    CFArray result( 1, n-1 );
    int i, j, k;
    i = 1;
    while ( I.hasItem() ) {
	k = I.getItem().exp();
	for ( j = 1; j <= k; j++ ) {
	    result[i] = I.getItem().factor();
	    i++;
	}
	I++;
    }
    if ( negate )
	result[1] = -result[1];
    return result;
}

static modpk
coeffBound ( const CanonicalForm & f, int p )
{
    int * degs = degrees( f );
    int M = 0, i, k = f.level();
    for ( i = 1; i <= k; i++ )
	M += degs[i];
    CanonicalForm b = 2 * maxCoeff( f ) * power( CanonicalForm( 3 ), M );
    CanonicalForm B = p;
    k = 1;
    while ( B < b ) {
	B *= p;
	k++;
    }
    return modpk( p, k );
}

static bool
nonDivisors ( CanonicalForm omega, CanonicalForm delta, const CFArray & F, CFArray & d )
{
    DEBOUTLN( cerr, "nondivisors omega = ", omega );
    DEBOUTLN( cerr, "nondivisors delta = ", delta );
    DEBOUTLN( cerr, "nondivisors F = ", F );
    CanonicalForm q, r;
    int k = F.size();
    d = CFArray( 0, k );
    d[0] = delta * omega;
    for ( int i = 1; i <= k; i++ ) {
	q = abs(F[i]);
	for ( int j = i-1; j >= 0; j-- ) {
	    r = d[j];
	    do {
		r = gcd( r, q );
		q = q / r;
	    } while ( r != 1 );
	    if ( q == 1 )
		return false;
	}
	d[i] = q;
    }
    return true;
}

static void
findEvaluation ( const CanonicalForm & U, const CanonicalForm & V, const CanonicalForm & omega, const CFFList & F, Evaluation & A, CanonicalForm & U0, CanonicalForm & delta, CFArray & D, int r )
{
    CanonicalForm Vn;
    CFFListIterator I;
    int j;
    bool found = false;
    CFArray FF = CFArray( 1, F.length() );
    if ( r > 0 )
	A.nextpoint();
    while ( ! found ) {
	Vn = A( V );
	if ( Vn != 0 ) {
	    U0 = A( U );
	    DEBOUTLN( cerr, "findev U0 = ", U0 );
	    if ( isSqrFree( U0 ) ) {
		delta = content( U0 );
		DEBOUTLN( cerr, "findev content( U0 ) = ", delta );
		for ( I = F, j = 1; I.hasItem(); I++, j++ )
		    FF[j] = A( I.getItem().factor() );
		found = nonDivisors( omega, delta, FF, D );
	    }
	    else {
		DEBOUTLN( cerr, "findev not sqrfree :", sqrFree( U0 ) );
	    }
	}
	if ( ! found )
	    A.nextpoint();
    }
}

static CFArray
ZFactorizeMulti ( const CanonicalForm & arg )
{
    DEBOUTLN( cerr, "MULTIFACTOR START ----------------------------------- level = ", arg.level() );
    CFMap M;
    CanonicalForm UU, U = compress( arg, M );
    CanonicalForm delta, omega, V = LC( U, 1 );
    int t = U.level();
    CFFList F = factorize( V );
    CFFListIterator I, J;
    CFArray G, lcG, D;
    int i, j, k, m, r, maxdeg, h;
    REvaluation A( 2, t, IntRandom( 100 ) );
    CanonicalForm U0;
    CanonicalForm ft, ut, gt, d;
    modpk b;
    bool negate = false;

#ifdef DEBUGOUTPUT
    cerr << "fac U = " << U << endl;
    for ( i = 1; i <= level( U ); i++ )
	cerr << "fac deg(U," << Variable(i) << ") = "
	     << degree( U, Variable(i) ) << endl
	     << "fac lc(U," << Variable(i) << ") = "
	     << LC( U, Variable(i) ) << endl;
#endif
    maxdeg = 0;
    for ( i = 2; i <= t; i++ ) {
	j = U.degree( Variable( i ) );
	if ( j > maxdeg ) maxdeg = j;
    }

    if ( F.getFirst().factor().inCoeffDomain() ) {
	omega = F.getFirst().factor();
	F.removeFirst();
	if ( omega < 0 ) {
	    negate = true;
	    omega = -omega;
	    U = -U;
	}
    }
    else
	omega = 1;

    bool goodeval = false;
    r = 0;

//    for ( i = 0; i < 10*t; i++ )
//	A.nextpoint();

    DEBOUTLN( cerr, "---------------------------------------", ' ' );

    while ( ! goodeval ) {
	TIMING_START(fac_findeval);
	findEvaluation( U, V, omega, F, A, U0, delta, D, r );
	TIMING_END(fac_findeval);
	DEBOUTLN( cerr, "fac evaluation = ", A );
	G = conv_to_factor_array( factorize( U0, false ) );
	DEBOUTLN( cerr, "fac fac(U0) = ", G );
	b = coeffBound( U, getZFacModulus().getp() );
#ifdef DEBUGOUTPUT
	cerr << "p^k(" << U.level() << ") = " << b.getp() << "^" << b.getk() << endl;
	cerr << "(fac: U  = " << U << endl
	     << "      U0 = " << U0 << endl
	     << "      V  = " << V << endl
	     << "      F  = " << F << endl
	     << "      a  = " << A << endl
	     << "      d  = " << delta << endl
	     << "      b  = " << b.getp() << "^" << b.getk() << endl
	     << "      ub = " << b.getp() << "^" << getZFacModulus().getk() << endl
	     << "      G  = " << G << endl
	     << "      D  = " << D << " )" << endl;
#endif
	r = G.size();
	DEBOUTLN( cerr, "fac SIZE OF UNIFAC = ", r );
	lcG = CFArray( 1, r );
	for ( j = 1; j <= r; j ++ )
	    lcG[j] = 1;

	TIMING_START(fac_distrib);
	goodeval = true;
	for ( I = F; goodeval && I.hasItem(); I++ ) {
	    ft = A( I.getItem().factor() );
	    m = I.getItem().exp();
	    j = 1;
	    while ( m > 0 && j <= r ) {
		ut = lc( G[j] ) * delta;
		while ( m > 0 && divides( ft, ut ) ) {
		    m--; ut /= ft;
		    lcG[j] *= I.getItem().factor();
		}
		j++;
	    }
	    goodeval = (m == 0);
	}
	TIMING_END(fac_distrib);
	if ( goodeval ) {
	    if ( delta != 1 ) {
		for ( j = 1; j <= r; j++ ) {
		    gt = A( lcG[j] );
		    d = gcd( gt, lc( G[j] ) );
		    lcG[j] *= lc( G[j] ) / d;
		    gt /= d;
		    G[j] *= gt;
		    delta /= gt;
		}
		DEBOUTLN( cerr, "fac delta = ", delta );
		if ( delta != 1 ) {
		    for ( j = 1; j <= r; j++ ) {
			G[j] *= delta;
			lcG[j] *= delta;
		    }
		    UU = U * power( delta, r-1 );
		}
		else
		    UU = U;
	    }
	    else
		UU = U;
	    DEBOUTLN( cerr, "fac good evaluation, lcG = ", lcG );
	    DEBOUTLN( cerr, "fac                    G = ", G );
	    DEBOUTLN( cerr, "fac delta = ", delta );
	    DEBOUTLN( cerr, "fac omega = ", omega );
	    for ( j = 1; j <= r; j++ ) {
		gt = A( lcG[j] );
		if ( gt != lc( G[j] ) ) {
		    gt = lc( G[j] ) / gt;
		    lcG[j] *= gt;
		    omega /= gt;
		}
	    }
	    DEBOUTLN( cerr, "fac good evaluation, lcG = ", lcG );
	    DEBOUTLN( cerr, "fac                    G = ", G );
	    DEBOUTLN( cerr, "fac delta = ", delta );
	    DEBOUTLN( cerr, "fac omega = ", omega );
	    goodeval = Hensel( UU, G, lcG, A, b, Variable(1) );
	}
    }
    for ( i = 1; i <= r; i++ )
	G[i] /= icontent( G[i] );
    // negate noch beachten !
    if ( negate )
	G[1] = -G[1];
    DEBOUTLN( cerr, "MULTIFACTOR END   ----------------------------------- level = ", arg.level() );
    return G;
}

CFFList
ZFactorizeMultivariate ( const CanonicalForm & f, bool issqrfree )
{
    CFFList G, F, R;
    CFArray GG;
    CFFListIterator i, j;
    CFMap M;
    CanonicalForm g, cont;
    Variable v1, vm;
    int k, m, n;

    v1 = Variable(1);
    if ( issqrfree )
	F = CFFactor( f, 1 );
    else
	F = sqrFree( f );

    for ( i = F; i.hasItem(); i++ ) {
	if ( i.getItem().factor().inCoeffDomain() ) {
	    if ( ! i.getItem().factor().isOne() )
		R.append( CFFactor( i.getItem().factor(), i.getItem().exp() ) );
	}
	else {
	    TIMING_START(fac_content);
	    g = compress( i.getItem().factor(), M );
	    // now after compress g contains Variable(1)
	    vm = g.mvar();
	    g = swapvar( g, v1, vm );
	    cont = content( g );
	    g = swapvar( g / cont, v1, vm );
	    cont = swapvar( cont, v1, vm );
	    n = i.getItem().exp();
	    TIMING_END(fac_content);
	    DEBOUTLN( cerr, "now after content ...", ' ' );
	    if ( g.isUnivariate() ) {
		G = factorize( g, true );
		for ( j = G; j.hasItem(); j++ )
		    if ( ! j.getItem().factor().isOne() )
			R.append( CFFactor( M( j.getItem().factor() ), n ) );
	    }
	    else {
		GG = ZFactorizeMulti( g );
		m = GG.max();
		for ( k = GG.min(); k <= m; k++ )
		    if ( ! GG[k].isOne() )
			R.append( CFFactor( M( GG[k] ), n ) );
	    }
	    G = factorize( cont, true );
	    for ( j = G; j.hasItem(); j++ )
		if ( ! j.getItem().factor().isOne() )
		    R.append( CFFactor( M( j.getItem().factor() ), n ) );
	}
    }
    return R;
}
