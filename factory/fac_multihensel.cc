/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_multihensel.cc 12231 2009-11-02 10:12:22Z hannes $ */

#include <config.h>

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "cf_defs.h"
#include "cf_eval.h"
#include "cf_binom.h"
#include "fac_util.h"
#include "fac_iterfor.h"
#include "cf_iter.h"

#ifndef HAVE_NTL

TIMING_DEFINE_PRINT(fac_solve);
TIMING_DEFINE_PRINT(fac_modpk);
TIMING_DEFINE_PRINT(fac_corrcoeff);
TIMING_DEFINE_PRINT(fac_extgcd);

static void
extgcdrest ( const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & s, const CanonicalForm & t, const CanonicalForm & c, CanonicalForm & S, CanonicalForm & T, const modpk & /*pk*/ )
{
    CanonicalForm sigma = s * c, tau = t * c;
//    divremainder( sigma, b, T, S, pk );
//    T = pk( tau + T * a );
    divrem( sigma, b, T, S );
    T = tau + T * a;
}

static void
solveF ( const CFArray & P, const CFArray & Q, const CFArray & S, const CFArray & T, const CanonicalForm & C, const modpk & pk, int r, CFArray & a )
{
    setCharacteristic( pk.getp(), pk.getk() );
    CanonicalForm g, bb, b = mapinto( C );
    int j;
    for ( j = 1; j < r; j++ )
    {
        extgcdrest( mapinto( P[j] ), mapinto( Q[j] ), mapinto( S[j] ), mapinto( T[j] ), b, bb, a[j], pk );
        b = bb;
    }
    a[r] = b;
    setCharacteristic( 0 );
    for ( j = 1; j <= r; j++ )
        a[j] = mapinto( a[j] );
}

static CanonicalForm
evalF ( const CFArray & P, const CFArray & Q, const CFArray & A, int r )
{
    CanonicalForm pprod = 1, sum = 0;
    for ( int i = 1; i <= r; i++ )
    {
        sum += pprod * A[i] * Q[i];
        pprod *= P[i];
    }
    return sum;
}

static CanonicalForm
derivAndEval ( const CanonicalForm & f, int n, const Variable & x, const CanonicalForm & a )
{
    if ( n == 0 )
        return f( a, x );
    else if ( f.degree( x ) < n )
        return 0;
    else
    {
        CFIterator i;
        CanonicalForm sum = 0, fact;
        int min, j;
        Variable v = Variable( f.level() + 1 );
        for ( i = swapvar( f, x, v); i.hasTerms() && i.exp() >= n; i++ )
        {
            fact = 1;
            min = i.exp() - n;
            for ( j = i.exp(); j > min; j-- )
                fact *= j;
            sum += fact * i.coeff() * power( v, min );
        }
        return sum( a, v );
    }
}

static CanonicalForm
checkCombination ( const CanonicalForm & W, const Evaluation & a, const IteratedFor & e, int k )
{
    CanonicalForm dW = W;
    int i, j;
    for ( i = k-1; i >= 2 && ! dW.isZero(); i-- )
        dW = derivAndEval( dW, e[i], Variable( i ), a[i] );
    if ( ! dW.isZero() ) {
        CanonicalForm fact = 1;
        for ( i = 2; i < k; i++ )
            for ( j = 2; j <= e[i]; j++ )
                fact *= j;
        dW /= fact;
    }
    return dW;
}

static CanonicalForm
prodCombination ( const Evaluation & a, const IteratedFor & e, int k )
{
    CanonicalForm p = 1;
    for ( int i = k-1; i > 1; i-- )
        p *= binomialpower( Variable(i), -a[i], e[i] );
    return p;
}

//static CanonicalForm check_dummy( const CFArray &a, const CFArray & P, const CFArray & Q )
//{
//    int i, r = a.size();
//    CanonicalForm res, prod;
//    res = 0;
//    prod = 1;
//    for ( i = 1; i <= r; i++ )
//    {
//        res += prod * a[i] * Q[i];
//        prod *= P[i];
//    }
//    return res;
//}

static bool check_e( const IteratedFor & e, int k, int m, int * n )
{
    int sum = 0;
    for ( int i = 2; i < k; i++ )
    {
        sum += e[i];
        if ( e[i] > n[i] )
            return false;
    }
    return sum == m+1;
}

static CanonicalForm modDeltak ( const CanonicalForm & f, const Evaluation & A, int k, int * n )
{
    CanonicalForm result = f;
    for ( int i = 2; i < k; i++ )
    {
        result.mod( binomialpower( Variable(i), -A[i], n[i]+1 ) );
    }
    return result;
}

static CFArray
findCorrCoeffs ( const CFArray & P, const CFArray & Q, const CFArray & P0, const CFArray & Q0, const CFArray & S, const CFArray & T, const CanonicalForm & C, const Evaluation & I, const modpk & pk, int r, int k, int h, int * n )
{
    DEBINCLEVEL( cerr, "findCorrCoeffs" );
    int i, m;
    CFArray A(1,r), a(1,r);
    CanonicalForm C0, Dm, g, prodcomb;

    TIMING_START(fac_solve);
    C0 = pk( I( C, 2, k-1 ), true );
    solveF( P0, Q0, S, T, 1, pk, r, a );
    TIMING_END(fac_solve);

    DEBOUTLN( cerr, "trying to find correction coefficients for " << C );
    DEBOUTLN( cerr, "which evaluates to " << C0 );

    for ( i = 1; i <= r; i++ )
        A[i] = remainder( pk( a[i] * C0 ), P0[i], pk );
    DEBOUTLN( cerr, "the first approximation of the correction coefficients is " << A );
#ifdef DEBUGOUTPUT
    if ( check_dummy( A, P, Q ) - C != 0 )
    {
        DEBOUTLN( cerr, "there is an error detected, the correction coefficients do not" );
        DEBOUTLN( cerr, "fulfill equation F(A)" );
        DEBOUTLN( cerr, "corresponding P " << P );
        DEBOUTLN( cerr, "              Q " << Q );
    }
#endif
    for ( m = 0; m <= h && ( m == 0 || Dm != 0 ); m++ )
    {
        Dm = pk( evalF( P, Q, A, r ) - C );
        if ( Dm != 0 )
        {
            if ( k == 2 )
            {
                TIMING_START(fac_solve);
                solveF( P0, Q0, S, T, Dm, pk, r, a );
                TIMING_END(fac_solve);
                for ( i = 1; i <= r; i++ )
                    A[i] -= a[i];
            }
            else
            {
                IteratedFor e( 2, k-1, m+1 );
                while ( e.iterations_left() )
                {
                    if ( check_e( e, k, m, n ) )
                    {
                        g = pk( checkCombination( Dm, I, e, k ) );
                        if ( ! g.isZero() && ! (g.mvar() > Variable(1)) )
                        {
                            prodcomb = prodCombination( I, e, k );
//                            Dm = Dm - g * prodcomb;
                            TIMING_START(fac_solve);
                            solveF( P0, Q0, S, T, g, pk, r, a );
                            TIMING_END(fac_solve);
                            for ( i = 1; i <= r; i++ )
                            {
//                                A[i] -= a[i] * prodcomb;
                                A[i] = pk( A[i] - a[i] * prodcomb );
                            }
                        }
                    }
                    e++;
                }
            }
        }
        DEBOUTLN( cerr, "the correction coefficients at step " << m );
        DEBOUTLN( cerr, "are now " << A );
#ifdef DEBUGOUTPUT
    if ( check_dummy( A, P, Q ) - C != 0 ) {
        DEBOUTLN( cerr, "there is an error detected, the correction coefficients do not" );
        DEBOUTLN( cerr, "fulfill equation F(A)" );
        DEBOUTLN( cerr, "corresponding P " << P );
        DEBOUTLN( cerr, "              Q " << Q );
    }
#endif
    }
    DEBDECLEVEL( cerr, "findCorrCoeffs" );
    return A;
}


static bool
liftStep ( CFArray & P, int k, int r, int t, const modpk & b, const Evaluation & A, const CFArray & lcG, const CanonicalForm & Uk, int * n, int h )
{
    DEBINCLEVEL( cerr, "liftStep" );
    CFArray K( 1, r ), Q( 1, r ), Q0( 1, r ), P0( 1, r ), S( 1, r ), T( 1, r ), alpha( 1, r );
    CanonicalForm Rm, C, D, xa = Variable(k) - A[k];
    CanonicalForm xa1 = xa, xa2 = xa*xa;
    CanonicalForm dummy;
    int i, m;

    DEBOUTLN( cerr, "we are now performing the liftstep to reach " << Variable(k) );
    DEBOUTLN( cerr, "the factors so far are " << P );
    DEBOUTLN( cerr, "modulus p^k= " << b.getpk() << "=" << b.getp() <<"^"<< b.getk() );

    for ( i = 1; i <= r; i++ )
    {
        Variable vm = Variable( t + 1 );
        Variable v1 = Variable(1);
        K[i] = swapvar( replaceLc( swapvar( P[i], v1, vm ), swapvar( A( lcG[i], k+1, t ), v1, vm ) ), v1, vm );
        P[i] = A( K[i], k, t );
    }
    DEBOUTLN( cerr, "lift K = " << K );

//    d = degree( Uk, Variable( k ) );

    TIMING_START(fac_extgcd);
    Q[r] = 1;
    for ( i = r; i > 1; i-- )
    {
        Q[i-1] = Q[i] * P[i];
        P0[i] = A( P[i], 2, k-1 );
        Q0[i] = A( Q[i], 2, k-1 );
        extgcd( P0[i], Q0[i], S[i], T[i], b );
    }
    P0[1] = A( P[1], 2, k-1 );
    Q0[1] = A( Q[1], 2, k-1 );
    extgcd( P0[1], Q0[1], S[1], T[1], b );
    TIMING_END(fac_extgcd);

    for ( m = 1; m <= n[k]+1; m++ )
    {
        TIMING_START(fac_modpk);
        Rm = modDeltak( prod( K ) - Uk, A, k, n );
        TIMING_END(fac_modpk);
#ifdef DEBUGOUTPUT
        if ( mod( Rm, xa1 ) != 0 )
        {
            DEBOUTLN( cerr, "something seems not to be ok with Rm which is " << Rm );
            DEBOUTLN( cerr, "and should reduce to zero modulo " << xa1 );
        }
#endif
        if ( mod( Rm, xa2 ) != 0 )
        {
            C = derivAndEval( Rm, m, Variable( k ), A[k] );
            D = 1;
            for ( i = 2; i <= m; i++ ) D *= i;
            C /= D;

            TIMING_START(fac_corrcoeff);
            alpha = findCorrCoeffs( P, Q, P0, Q0, S, T, C, A, b, r, k, h, n ); // -> h berechnen
            TIMING_END(fac_corrcoeff);
// #ifdef DEBUGOUTPUT
//             dummy = check_dummy( alpha, P, Q );
//             if ( b(modDeltak( dummy, A, k, n )) != b(modDeltak( C, A, k, n )) )
//             {
//                 DEBOUTLN( cerr, "lift fault" );
//                 DEBDECLEVEL( cerr, "liftStep" );
//                 return false;
//             }
// #endif
            for ( i = 1; i <= r; i++ )
                K[i] = b(K[i] - alpha[i] * xa1);
            DEBOUTLN( cerr, "the corrected K's are now " << K );
        }
        xa1 = xa2;
        xa2 *= xa;
    }
    for ( i = 1; i <= r; i++ )
        P[i] = K[i];
    if ( prod( K ) - Uk != 0 )
    {
        DEBOUTLN( cerr, "the liftstep produced the wrong result" );
        DEBOUTLN( cerr, "the product of the factors calculated so far is " << prod(K) );
        DEBOUTLN( cerr, "and the Uk that should have been reached is " << Uk );
        DEBDECLEVEL( cerr, "liftStep" );
        return false;
    }
    DEBOUTLN( cerr, "the lift seems ok so far" );
    DEBDECLEVEL( cerr, "liftStep" );
    return true; // check for divisibility
}

bool
Hensel ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const modpk & bound, const Variable & /*x*/ )
{
    DEBINCLEVEL( cerr, "Hensel" );
    int k, i, h, t = A.max();
    bool goodeval = true;
    CFArray Uk( A.min(), A.max() );
    int * n = new int[t+1];

    Uk[t] = U;
    for ( k = t-1; k > 1; k-- )
    {
        Uk[k] = Uk[k+1]( A[k+1], Variable( k+1 ) );
        n[k] = degree( Uk[k], Variable( k ) );
    }
    for ( k = A.min(); goodeval && (k <= t); k++ )
    {
        h = totaldegree( Uk[k], Variable(A.min()), Variable(k-1) );
        for ( i = A.min(); i <= k; i++ )
            n[i] = degree( Uk[k], Variable(i) );
        goodeval = liftStep( G, k, G.max(), t, bound, A, lcG, Uk[k], n, h );
        DEBOUTLN( cerr, "Factorization so far: " << G );
    }
    DEBDECLEVEL( cerr, "Hensel" );
    delete[] n;
    return goodeval;
}
#endif
