// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_multivar.cc,v 1.0 1996-05-17 10:59:45 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

//#define TIMING

#ifndef NDEBUG
//#define DEBUGOUTPUT
#endif

#include <time.h>
#include <sys/times.h>

#include "assert.h"
#include "cf_defs.h"
#include "fac_multivar.h"
#include "fac_univar.h"
#include "cf_reval.h"
#include "cf_map.h"
#include "fac_util.h"
#include "cf_binom.h"
#include "fac_util.h"
#include "fac_diophant.h"
#include "fac_iterfor.h"
#include "cf_iter.h"


static CanonicalForm
check_dummy( const CFArray &a, const CFArray & P, const CFArray & Q )
{
    int i, r = a.size();
    CanonicalForm res, prod;
    res = 0;
    prod = 1;
    for ( i = 1; i <= r; i++ ) {
	res += prod * a[i] * Q[i];
	prod *= P[i];
    }
    return res;
}

static bool
check_e( const IteratedFor & e, int k, int m, int * n )
{
    int sum = 0;
    for ( int i = 2; i < k; i++ ) {
	sum += e[i];
	if ( e[i] > n[i] )
	    return false;
    }
    return sum == m+1;
}

static CFArray
conv_to_factor_array( const CFFList & L )
{
    int n;
    CFFListIterator I = L;
    if ( ! I.hasItem() )
	n = 0;
    else  if ( I.getItem().factor().inBaseDomain() ) {
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
    return result;
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

modpk
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
    DEBOUTLN( cerr, "omega = ", omega );
    DEBOUTLN( cerr, "delta = ", delta );
    DEBOUTLN( cerr, "F = ", F );
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
findEvaluation ( const CanonicalForm & U, const CanonicalForm & V, const CanonicalForm & omega, const CFFList & F, Evaluation & A, CanonicalForm & U0, CanonicalForm & delta, int r )
{
    CanonicalForm Vn;
    CFFListIterator I;
    int j;
    bool found = false;
    CFArray FF = CFArray( 1, F.length() ), D;
    if ( r > 0 )
	A.nextpoint();
    while ( ! found ) {
	Vn = A( V );
	if ( Vn != 0 ) {
	    U0 = A( U );
	    DEBOUTLN( cerr, "U0 = ", U0 );
	    if ( isSqrFree( U0 ) ) {
		delta = content( U0 );
		DEBOUTLN( cerr, "content( U0 ) = ", delta );
		for ( I = F, j = 1; I.hasItem(); I++, j++ )
		    FF[j] = A( I.getItem().factor() );
		found = nonDivisors( omega, delta, FF, D );
	    }
	    else {
		DEBOUTLN( cerr, "not sqrfree :", sqrFree( U0 ) );
	    }
	}
	if ( ! found )
	    A.nextpoint();
    }
}

static CanonicalForm
modDeltak ( const CanonicalForm & f, const Evaluation & A, int k, int * n )
{
    CanonicalForm result = f;
    for ( int i = 2; i < k; i++ ) {
	result.mod( binomialpower( Variable(i), -A[i], n[i]+1 ) );
    }
    return result;
}

CFArray
findCorrCoeffs ( const CFArray & P, const CFArray & Q, const CanonicalForm & C, const Evaluation & I, const modpk & pk, int r, int k, int h, int * n )
{
#ifdef TIMING
    static float totalsolve = 0.0;
    struct tms ts, te;
#endif
    int i, m;
    CFArray A(1,r), a(1,r), P0(1,r), Q0(1,r);
    CanonicalForm C0, Dm, g, prodcomb;

#ifdef TIMING
    times( &ts );
#endif
    for ( i = 1; i <= r; i++ ) {
	P0[i] = I( P[i], 2, k-1 );
	Q0[i] = I( Q[i], 2, k-1 );
    }
    C0 = pk( I( C, 2, k-1 ), true );
    DEBOUTLN( cerr, "C  = ", C );
    DEBOUTLN( cerr, "C0 = ", C0 );
    DEBOUTLN( cerr, "P0 = ", P0 );
    DEBOUTLN( cerr, "Q0 = ", Q0 );
    solveF( P0, Q0, 1, pk, r, a );
//    solveF( P0, Q0, C0, pk, r, a );
    DEBOUTLN( cerr, "a  = ", a );
#ifdef TIMING
    times( &te );
    cerr.setf( ios::fixed, ios::floatfield );
    cerr.precision(2);
    cerr << "time used for solve: "
	 << float(te.tms_utime-ts.tms_utime) / CLK_TCK << " sec, total time = ";
    totalsolve += float(te.tms_utime-ts.tms_utime) / CLK_TCK;
    cerr << totalsolve << endl;
#endif
    for ( i = 1; i <= r; i++ )
	A[i] = remainder( pk( a[i] * C0 ), P0[i], pk );
    for ( m = 0; m <= h && ( m == 0 || Dm != 0 ); m++ ) {
	Dm = pk( evalF( P, Q, A, r ) - C );
	if ( Dm != 0 ) {
	    if ( k == 2 ) {
		solveF( P0, Q0, Dm, pk, r, a );
		for ( i = 1; i <= r; i++ )
		    A[i] -= a[i];
	    }
	    else {
		IteratedFor e( k-1, m+1 );
		while ( e.iterations_left() ) {
		    if ( check_e( e, k, m, n ) ) {
			g = pk( checkCombination( Dm, I, e, k ) );
			if ( ! g.isZero() && ! (g.mvar() > Variable(1)) ) {
			    prodcomb = prodCombination( I, e, k );
//			    Dm = Dm - g * prodcomb;
			    solveF( P0, Q0, g, pk, r, a );
			    for ( i = 1; i <= r; i++ ) {
//				A[i] -= a[i] * prodcomb;
				A[i] = pk( A[i] - a[i] * prodcomb );
			    }
			}
		    }
		    e++;
		}
	    }
	}
    }
    return A;
}
		

bool
liftStep ( CFArray & P, int k, int r, int t, const modpk & b, const Evaluation & A, const CFArray & lcG, const CanonicalForm & Uk, int * n, int d, int h )
{
    CFArray K( 1, r ), Q( 1, r ), alpha( 1, r );
    CanonicalForm Rm, C, D, xa = Variable(k) - A[k];
    CanonicalForm xa1 = xa, xa2 = xa*xa;
    CanonicalForm dummy;
    int i, m;

    DEBOUTLN( cerr, " P = ", P );
    DEBOUTLN( cerr, " lc= ", lcG );
    for ( i = 1; i <= r; i++ ) {
	Variable vm = Variable( t + 1 );
	Variable v1 = Variable(1);
	K[i] = swapvar( replaceLc( swapvar( P[i], v1, vm ), A( lcG[i], k+1, t )), v1, vm );
    }
    DEBOUTLN( cerr, " K = ", K );

//    d = degree( Uk, Variable( k ) );

    Q[r] = 1;
    for ( i = r; i > 1; i-- )
	Q[i-1] = Q[i] * P[i];
    for ( m = 1; m <= n[k]+1; m++ ) {
	Rm = modDeltak( prod( K ) - Uk, A, k, n );
#ifdef 0
#ifdef DEBUGOUTPUT
	cerr << "p(K)-Uk = " << prod( K ) - Uk << endl;
	cerr << "p(K)-Uk mod deltak = " << Rm << endl;
	cerr << "xa1 = " << xa1 << endl;
	cerr << "Rm % xa1 = " << Rm % xa1 << endl;
	if ( mod( Rm, xa1 ) != 0 )
	    cerr << "Rm not ok : " << Rm << endl;
#endif
#endif
	if ( mod( Rm, xa2 ) != 0 ) {
	    C = derivAndEval( Rm, m, Variable( k ), A[k] );
	    D = 1;
	    for ( i = 2; i <= m; i++ ) D *= i;
	    C /= D;

	    alpha = findCorrCoeffs( P, Q, C, A, b, r, k, h, n ); // -> h berechnen
	    dummy = check_dummy( alpha, P, Q );
	    if ( b(modDeltak( dummy, A, k, n )) != b(modDeltak( C, A, k, n )) ) {
#ifdef DEBUGOUTPUT
		cerr << "fault in multi diophant" << endl
		     << " alpha = " << alpha << endl
		     << " dummy = " << modDeltak( dummy, A, k, n ) << endl
		     << " C = " << modDeltak( C, A, k, n ) << endl;
#endif
		cerr << "fault" << endl;
		return false;
	    }
	    for ( i = 1; i <= r; i++ )
		K[i] = b(K[i] - alpha[i] * xa1);
	}
	xa1 = xa2;
	xa2 *= xa;
    }
    for ( i = 1; i <= r; i++ )
	P[i] = K[i];
    DEBOUTLN( cerr, "K = ", K );
    if ( prod( K ) - Uk != 0 )
	return false;
    return true; // check for divisibility
}

CFArray
ZFactorizeMulti ( const CanonicalForm & arg )
{
    CFMap M;
    CanonicalForm UU, U = compress( arg, M );
    CanonicalForm delta, omega, V = LC( U, 1 );
    int t = U.level();
    CFFList F = factorize( V );
    CFFListIterator I, J;
    CFArray G, lcG;
    int i, j, k, m, r, maxdeg, h;
    REvaluation A( 2, t, IntRandom( 100 ) );
    CanonicalForm U0;
    CanonicalForm ft, ut, gt, d;
    modpk b;
    bool negate = false;

#ifdef DEBUGOUTPUT
    cerr << "U = " << U << endl;
    for ( i = 1; i <= level( U ); i++ )
	cerr << " deg(U," << Variable(i) << ") = "
	     << degree( U, Variable(i) ) << endl
	     << " lc(U," << Variable(i) << ") = "
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

    while ( ! goodeval ) {
	findEvaluation( U, V, omega, F, A, U0, delta, r );
	cerr << "A = " << A << endl;
	DEBOUTLN( cerr, "evaluation = ", A );
	DEBOUTLN( cerr, "fac(U0) = ", factorize( U0, false ) );
	G = conv_to_factor_array( factorize( U0, true ) );
	b = coeffBound( U, getZFacModulus().getp() );
#ifdef DEBUGOUTPUT
	cout << "(fac: U  = " << U << endl
	     << "      U0 = " << U0 << endl
	     << "      V  = " << V << endl
	     << "      F  = " << F << endl
	     << "      a  = " << A << endl
	     << "      d  = " << delta << endl
	     << "      b  = " << b.getp() << "^" << b.getk() << endl
	     << "      ub = " << b.getp() << "^" << getZFacModulus().getk() << endl
	     << "      G  = " << G << " )" << endl;
#endif
	r = G.size();
	DEBOUTLN( cerr, "SIZE OF UNIFAC = ", r );
	lcG = CFArray( 1, r );
	for ( j = 1; j <= r; j ++ )
	    lcG[j] = 1;

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
	if ( goodeval ) {
	    DEBOUTLN( cerr, "good evaluation, lcG = ", lcG );
	    DEBOUTLN( cerr, "                   G = ", G );
	    DEBOUTLN( cerr, "delta = ", delta );
	    if ( delta != 1 ) {
		for ( j = 1; j <= r; j++ ) {
		    gt = A( lcG[j] );
		    d = gcd( gt, lc( G[j] ) );
		    lcG[j] *= lc( G[j] ) / d;
		    gt /= d;
		    G[j] *= gt;
		    delta /= gt;
		}
		DEBOUTLN( cerr, "delta = ", delta );
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
	    DEBOUTLN( cerr, "good evaluation, lcG = ", lcG );
	    DEBOUTLN( cerr, "                   G = ", G );
	    CFArray Uk( 1, t );
	    Uk[t] = UU;
	    int * n = new int[t+1];
	    for ( k = t-1; k > 1; k-- ) {
		Uk[k] = Uk[k+1]( A[k+1], Variable( k+1 ) );
		n[k] = degree( Uk[k], Variable( k ) );
	    }
	    for ( k = 2; goodeval && (k <= t); k++ ) {
		DEBOUTLN( cerr, "begin lifting -------> k  = ", k );
		DEBOUTLN( cerr, "                       Uk = ", Uk[k] );  
		h = totaldegree( Uk[k], Variable(2), Variable(k-1) );
		DEBOUTLN( cerr, "                       h  = ", h );  
		for ( i = 2; i <= k; i++ )
		    n[i] = degree( Uk[k], Variable(i) );
		goodeval = liftStep( G, k, r, t, b, A, lcG, Uk[k], n, maxdeg, h );
	    }
	}
    }
    for ( i = 1; i <= r; i++ )
	G[i] /= icontent( G[i] );
    // negate noch beachten !
    if ( negate )
	G[1] = -G[1];
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
	    g = compress( i.getItem().factor(), M );
	    // now after compress g contains Variable(1)
	    vm = g.mvar();
	    g = swapvar( g, v1, vm );
	    cont = content( g );
	    g = swapvar( g / cont, v1, vm );
	    cont = swapvar( cont, v1, vm );
	    n = i.getItem().exp();
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
