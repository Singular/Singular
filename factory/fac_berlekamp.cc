/* emacs edit mode for this file is -*- C++ -*- */

#include <config.h>

#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "fac_berlekamp.h"
#include "ffops.h"
#include "gfops.h"
#include "imm.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_generator.h"
#include "fac_sqrfree.h"

#if !defined(HAVE_FLINT) && !defined(HAVE_NTL)

#ifdef DEBUGOUTPUT
void QprintFF( int ** Q, int n )
{
    for ( int i = 0; i < n; i++ ) {
        for ( int j = 0; j < n; j++ )
            std::cerr << Q[i][j] << "  ";
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
}
#endif /* DEBUGOUTPUT */

#ifdef DEBUGOUTPUT
void QprintGF( int ** Q, int n )
{
    for ( int i = 0; i < n; i++ ) {
        for ( int j = 0; j < n; j++ ) {
            gf_print( std::cerr, Q[i][j] );
            std::cerr << "  ";
        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
}
#endif /* DEBUGOUTPUT */

void QmatFF ( const CanonicalForm & f, int ** Q, int p )
{
    int n = degree( f ), nn = (n-1)*p + 1;
    int i, m, rn;
    int * a = new int [n];
    int * r = new int [n];
    int * q;

    q = Q[0]; *q = r[0] = 1; a[0] = 0; q++;

    for ( i = 1; i < n; i++, q++ )
        *q = r[i] = a[i] = 0;
    CFIterator I = f; I++;
    while ( I.hasTerms() ) {
        a[I.exp()] = I.coeff().intval();
        I++;
    }
    for ( m = 1; m < nn; m++ ) {
        rn = r[n-1];
        for ( i = n-1; i > 0; i-- )
            r[i] = ff_sub( r[i-1], ff_mul( rn, a[i] ) );
        r[0] = ff_mul( ff_neg( rn ), a[0] );
        if ( m % p == 0 ) {
            q = Q[m/p];
            for ( i = 0; i < n; i++, q++ )
                *q = r[i];
        }
    }
    for ( i = 0; i < n; i++ )
        Q[i][i] = ff_sub( Q[i][i], 1 );

    delete [] a;
    delete [] r;
}

void QmatGF ( const CanonicalForm & f, int ** Q, int p )
{
    int n = degree( f ), nn = (n-1)*p + 1;
    int i, m, rn;
    int * a = new int [n];
    int * r = new int [n];
    int * q;

    q = Q[0]; *q = r[0] = gf_one(); a[0] = gf_zero(); q++;

    for ( i = 1; i < n; i++, q++ )
        *q = r[i] = a[i] = gf_zero();
    CFIterator I = f; I++;
    while ( I.hasTerms() ) {
        a[I.exp()] = imm2int( I.coeff().getval() );
        I++;
    }
    for ( m = 1; m < nn; m++ ) {
        rn = r[n-1];
        for ( i = n-1; i > 0; i-- )
            r[i] = gf_sub( r[i-1], gf_mul( rn, a[i] ) );
        r[0] = gf_mul( gf_neg( rn ), a[0] );
        if ( m % p == 0 ) {
            q = Q[m/p];
            for ( i = 0; i < n; i++, q++ )
                *q = r[i];
        }
    }
    for ( i = 0; i < n; i++ )
        Q[i][i] = gf_sub( Q[i][i], gf_one() );

    delete [] a;
    delete [] r;
}

int nullSpaceFF ( int ** Q, int ** b, int n )
{
    int * c = new int[n];
    int r, i, j, k, h, s, d;

    r = 0;
    for ( s = 0; s < n; s++ ) c[s] = -1;
    for ( h = 0; h < n; h++ ) {
        j = 0;
        while ( j < n && ! ( Q[h][j] != 0 && c[j] < 0 ) ) j++;
        if ( j < n ) {
            d = ff_neg( ff_inv( Q[h][j] ) );
            for ( s = 0; s < n; s++ )
                Q[s][j] = ff_mul( d, Q[s][j] );
            for ( i = 0; i < n; i++ ) {
                if ( i != j ) {
                    d = Q[h][i];
                    for ( s = 0; s < n; s++ )
                        Q[s][i] = ff_add( ff_mul( d, Q[s][j] ), Q[s][i] );
                }
            }
            c[j] = h;
        }
        else {
            b[r] = new int[n];
            for ( j = 0; j < n; j++ ) {
                if ( j == h )
                    b[r][j] = 1;
                else {
                    k = 0;
                    while ( k < n && c[k] != j ) k++;
                    if ( k < n )
                        b[r][j] = Q[h][k];
                    else
                        b[r][j] = 0;
                }
            }
            r++;
        }
    }
    delete [] c;
    return r;
}

int nullSpaceGF ( int ** Q, int ** b, int n )
{
    int * c = new int[n];
    int r, i, j, k, h, s, d;

    r = 0;
    for ( s = 0; s < n; s++ ) c[s] = -1;
    for ( h = 0; h < n; h++ ) {
        j = 0;
        while ( j < n && ! ( ! gf_iszero( Q[h][j] ) && c[j] < 0 ) ) j++;
        if ( j < n ) {
            d = gf_neg( gf_inv( Q[h][j] ) );
            for ( s = 0; s < n; s++ )
                Q[s][j] = gf_mul( d, Q[s][j] );
            for ( i = 0; i < n; i++ ) {
                if ( i != j ) {
                    d = Q[h][i];
                    for ( s = 0; s < n; s++ )
                        Q[s][i] = gf_add( gf_mul( d, Q[s][j] ), Q[s][i] );
                }
            }
            c[j] = h;
        }
        else {
            b[r] = new int[n];
            for ( j = 0; j < n; j++ ) {
                if ( j == h )
                    b[r][j] = gf_one();
                else {
                    k = 0;
                    while ( k < n && c[k] != j ) k++;
                    if ( k < n )
                        b[r][j] = Q[h][k];
                    else
                        b[r][j] = gf_zero();
                }
            }
            r++;
        }
    }
    delete [] c;
    return r;
}

CanonicalForm cfFromIntVec( int * a, int n, const Variable & x )
{
    CanonicalForm result = power( x, n-1 ) * a[n-1];
    for ( int i = n-2; i >= 0; i-- )
        if ( a[i] != 0 )
            result += power( x, i ) * a[i];
    return result;
}

CanonicalForm cfFromGFVec( int * a, int n, const Variable & x )
{
    CanonicalForm result = power( x, n-1 ) * CanonicalForm( int2imm_gf( a[n-1] ) );
    for ( int i = n-2; i >= 0; i-- )
        if ( ! gf_iszero( a[i] ) )
            result += power( x, i ) * CanonicalForm( int2imm_gf( a[i] ) );
    return result;
}

typedef int * intptr;

CFFList BerlekampFactorFF ( const CanonicalForm & f )
{
    CFFList F;
    int p = getCharacteristic();
    int r, s, len, i, k, n = degree( f );
    Variable x = f.mvar();
    CanonicalForm u, g;
    intptr* Q = new intptr [n];
    intptr* B = new intptr [n];
    for ( i = 0; i < n; i++ )
        Q[i] = new int[n];
    QmatFF( f, Q, p );
#ifdef DEBUGOUTPUT
    DEBOUTLN( cerr, "Q = " );
    QprintFF( Q, n );
#endif /* DEBUGOUTPUT */
    k = nullSpaceFF( Q, B, n );
#ifdef DEBUGOUTPUT
    DEBOUTLN( cerr, "Q = " );
    QprintFF( Q, n );
#endif /* DEBUGOUTPUT */
    F.insert( CFFactor( f, 1 ) );
    r = 1;
    len = 1;
    while ( len < k ) {
        ASSERT( r < k, "fatal fatal" );
        ListIterator<CFFactor> I = F;
        while ( I.hasItem() && len < k ) {
            u = I.getItem().factor();
            for ( s = 0; s < p && len < k; s++ ) {
                g = gcd( cfFromIntVec( B[r], n, x ) - s, u );
                if ( degree( g ) > 0 && g != u ) {
                    u /= g;
                    I.append( CFFactor( g, 1 ) );
                    I.append( CFFactor( u, 1 ) );
                    I.remove( 1 );
                    len++;
                }
            }
            I++;
        }
        r++;
    }
    for ( i = 0; i < n; i++ )
        delete [] Q[i];
    for ( i = 0; i < r; i++ )
        delete [] B[i];
    delete [] B;
    delete [] Q;
    return F;
}

CFFList BerlekampFactorGF ( const CanonicalForm & f )
{
    CFFList F;
    int r, len, i, k, n = degree( f );
    Variable x = f.mvar();
    CanonicalForm u, g;
    intptr* Q = new intptr [n];
    intptr* B = new intptr [n];
    for ( i = 0; i < n; i++ )
        Q[i] = new int[n];
    QmatGF( f, Q, gf_q );
#ifdef DEBUGOUTPUT
    DEBOUTLN( cerr, "Q = " );
    QprintGF( Q, n );
#endif /* DEBUGOUTPUT */
    k = nullSpaceGF( Q, B, n );
#ifdef DEBUGOUTPUT
    DEBOUTLN( cerr, "Q = " );
    QprintFF( Q, n );
#endif /* DEBUGOUTPUT */
    F.insert( CFFactor( f, 1 ) );
    r = 1;
    len = 1;
    GFGenerator s;
    while ( len < k ) {
        ASSERT( r < k, "fatal fatal" );
        ListIterator<CFFactor> I = F;
        while ( I.hasItem() && len < k ) {
            u = I.getItem().factor();
            for ( s.reset(); s.hasItems() && len < k; s++ ) {
                g = gcd( cfFromGFVec( B[r], n, x ) - s.item(), u );
                if ( degree( g ) > 0 && g != u ) {
                    u /= g;
                    I.append( CFFactor( g, 1 ) );
                    I.append( CFFactor( u, 1 ) );
                    I.remove( 1 );
                    len++;
                }
            }
            I++;
        }
        r++;
    }
    for ( i = 0; i < n; i++ )
        delete [] Q[i];
    for ( i = 0; i < r; i++ )
        delete [] B[i];
    delete [] B;
    delete [] Q;
    return F;
}
// {
//     CFFList F;
//     int p = getCharacteristic();
//     int r, len, k, n = degree( f );
//     Variable x = f.mvar();
//     CanonicalForm u, g;
//     intptr* Q = new intptr [n];
//     for ( int i = 0; i < n; i++ )
//         Q[i] = new int[n];
//     QmatGF( f, Q, p );
// //  Qprint( Q, n );
//     k = nullSpaceGF( Q, n );
// //  Qprint( Q, n );
//     F.insert( CFFactor( f, 1 ) );
//     r = 1;
//     len = 1;
//     GFIterator s;
//     while ( len < k ) {
//         ListIterator<CFFactor> I = F;
//         while ( I.hasItem() && len < k ) {
//             u = I.getItem().factor();
//             for ( s.reset(); s.hasItems() && len < k; s++ ) {
//                 g = gcd( cfFromGFVec( Q[r], n, x ) - s.item(), u );
//                 if ( degree( g ) > 0 && g != u ) {
//                     u /= g;
//                     I.append( CFFactor( g, 1 ) );
//                     I.append( CFFactor( u, 1 ) );
//                     I.remove( 1 );
//                     len++;
//                 }
//             }
//             I++;
//         }
//         r++;
//     }
//     for ( i = 0; i < n; i++ )
//         delete [] Q[i];
//     return F;
// }

CFFList FpFactorizeUnivariateB( const CanonicalForm& f, bool issqrfree )
{
    CFFList F, G, H;
    CanonicalForm fac;
    ListIterator<CFFactor> i, k;
    int d;
    bool galoisfield = getGFDegree() > 1;

    if ( LC( f ).isOne() )
        if ( issqrfree )
            F.append( CFFactor( f, 1 ) );
        else
            F = sqrFreeFp( f );
    else {
        H.append( LC( f ) );
        if ( issqrfree )
            F.append( CFFactor( f / LC( f ), 1 ) );
        else
            F = sqrFreeFp( f / LC( f ) );
    }
    for ( i = F; i.hasItem(); ++i ) {
        d = i.getItem().exp();
        fac = i.getItem().factor();
        if ( galoisfield )
            G = BerlekampFactorGF( fac / LC( fac ) );
        else
            G = BerlekampFactorFF( fac / LC( fac ) );
        for ( k = G; k.hasItem(); ++k ) {
            fac = k.getItem().factor();
            H.append( CFFactor( fac / LC( fac ), d ) );
        }
    }
    return H;
}
#endif
