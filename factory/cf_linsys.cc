/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_linsys.cc
 *
 * solve linear systems and compute determinants of matrices
**/

#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "cf_defs.h"
#include "cf_primes.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_algorithm.h"
#include "ffops.h"
#include "cf_primes.h"


TIMING_DEFINE_PRINT(det_mapping)
TIMING_DEFINE_PRINT(det_determinant)
TIMING_DEFINE_PRINT(det_chinese)
TIMING_DEFINE_PRINT(det_bound)
TIMING_DEFINE_PRINT(det_numprimes)


static bool solve ( int **extmat, int nrows, int ncols );
int determinant ( int **extmat, int n );

static CanonicalForm bound ( const CFMatrix & M );
CanonicalForm detbound ( const CFMatrix & M, int rows );

bool
matrix_in_Z( const CFMatrix & M, int rows )
{
    int i, j;
    for ( i = 1; i <= rows; i++ )
        for ( j = 1; j <= rows; j++ )
            if ( ! M(i,j).inZ() )
                return false;
    return true;
}

bool
matrix_in_Z( const CFMatrix & M )
{
    int i, j, rows = M.rows(), cols = M.columns();
    for ( i = 1; i <= rows; i++ )
        for ( j = 1; j <= cols; j++ )
            if ( ! M(i,j).inZ() )
                return false;
    return true;
}

bool
betterpivot ( const CanonicalForm & oldpivot, const CanonicalForm & newpivot )
{
    if ( newpivot.isZero() )
        return false;
    else  if ( oldpivot.isZero() )
        return true;
    else  if ( level( oldpivot ) > level( newpivot ) )
        return true;
    else  if ( level( oldpivot ) < level( newpivot ) )
        return false;
    else
        return ( newpivot.lc() < oldpivot.lc() );
}

THREAD_VAR bool fuzzy_result;

bool
linearSystemSolve( CFMatrix & M )
{
    typedef int* int_ptr;

    if ( ! matrix_in_Z( M ) ) {
        int nrows = M.rows(), ncols = M.columns();
        int i, j, k;
        CanonicalForm rowpivot, pivotrecip;
        // triangularization
        for ( i = 1; i <= nrows; i++ ) {
            //find "pivot"
            for (j = i; j <= nrows; j++ )
                if ( M(j,i) != 0 ) break;
            if ( j > nrows ) return false;
            if ( j != i )
                M.swapRow( i, j );
            pivotrecip = 1 / M(i,i);
            for ( j = 1; j <= ncols; j++ )
                M(i,j) *= pivotrecip;
            for ( j = i+1; j <= nrows; j++ ) {
                rowpivot = M(j,i);
                if ( rowpivot == 0 ) continue;
                for ( k = i; k <= ncols; k++ )
                    M(j,k) -= M(i,k) * rowpivot;
            }
        }
        // matrix is now upper triangular with 1s down the diagonal
        // back-substitute
        for ( i = nrows-1; i > 0; i-- ) {
            for ( j = nrows+1; j <= ncols; j++ ) {
                for ( k = i+1; k <= nrows; k++ )
                    M(i,j) -= M(k,j) * M(i,k);
            }
        }
        return true;
    }
    else {
        int rows = M.rows(), cols = M.columns();
        CFMatrix MM( rows, cols );
        int ** mm = new int_ptr[rows];
        CanonicalForm Q, Qhalf, mnew, qnew, B;
        int i, j, p, pno;
        bool ok;

        // initialize room to hold the result and the result mod p
        for ( i = 0; i < rows; i++ ) {
            mm[i] = new int[cols];
        }

        // calculate the bound for the result
        B = bound( M );
        DEBOUTLN( cerr, "bound = " <<  B );

        // find a first solution mod p
        pno = 0;
        do {
            DEBOUTSL( cerr );
            DEBOUT( cerr, "trying prime(" << pno << ") = " );
            p = cf_getBigPrime( pno );
            DEBOUT( cerr, p );
            DEBOUTENDL( cerr );
            setCharacteristic( p );
            // map matrix into char p
            for ( i = 1; i <= rows; i++ )
                for ( j = 1; j <= cols; j++ )
                    mm[i-1][j-1] = mapinto( M(i,j) ).intval();
            // solve mod p
            ok = solve( mm, rows, cols );
            pno++;
        } while ( ! ok );

        // initialize the result matrix with first solution
        setCharacteristic( 0 );
        for ( i = 1; i <= rows; i++ )
            for ( j = rows+1; j <= cols; j++ )
                MM(i,j) = mm[i-1][j-1];

        // Q so far
        Q = p;
        while ( Q < B && pno < cf_getNumBigPrimes() ) {
            do {
                DEBOUTSL( cerr );
                DEBOUT( cerr, "trying prime(" << pno << ") = " );
                p = cf_getBigPrime( pno );
                DEBOUT( cerr, p );
                DEBOUTENDL( cerr );
                setCharacteristic( p );
                for ( i = 1; i <= rows; i++ )
                    for ( j = 1; j <= cols; j++ )
                        mm[i-1][j-1] = mapinto( M(i,j) ).intval();
                // solve mod p
                ok = solve( mm, rows, cols );
                pno++;
            } while ( ! ok );
            // found a solution mod p
            // now chinese remainder it to a solution mod Q*p
            setCharacteristic( 0 );
            for ( i = 1; i <= rows; i++ )
                for ( j = rows+1; j <= cols; j++ )
                {
                    chineseRemainder( MM[i][j], Q, CanonicalForm(mm[i-1][j-1]), CanonicalForm(p), mnew, qnew );
                    MM(i, j) = mnew;
                }
            Q = qnew;
        }
        if ( pno == cf_getNumBigPrimes() )
            fuzzy_result = true;
        else
            fuzzy_result = false;
        // store the result in M
        Qhalf = Q / 2;
        for ( i = 1; i <= rows; i++ ) {
            for ( j = rows+1; j <= cols; j++ )
                if ( MM(i,j) > Qhalf )
                    M(i,j) = MM(i,j) - Q;
                else
                    M(i,j) = MM(i,j);
            delete [] mm[i-1];
        }
        delete [] mm;
        return ! fuzzy_result;
    }
}

static bool
fill_int_mat( const CFMatrix & M, int ** m, int rows )
{
    int i, j;
    bool ok = true;
    for ( i = 0; i < rows && ok; i++ )
        for ( j = 0; j < rows && ok; j++ )
        {
            if ( M(i+1,j+1).isZero() )
                m[i][j] = 0;
            else
            {
                m[i][j] = mapinto( M(i+1,j+1) ).intval();
//                ok = m[i][j] != 0;
            }
        }
    return ok;
}

CanonicalForm
determinant( const CFMatrix & M, int rows )
{
    typedef int* int_ptr;

    ASSERT( rows <= M.rows() && rows <= M.columns() && rows > 0, "undefined determinant" );
    if ( rows == 1 )
        return M(1,1);
    else  if ( rows == 2 )
        return M(1,1)*M(2,2)-M(2,1)*M(1,2);
    else  if ( matrix_in_Z( M, rows ) )
    {
        int ** mm = new int_ptr[rows];
        CanonicalForm x, q, Qhalf, B;
        int n, i, intdet, p, pno;
        for ( i = 0; i < rows; i++ )
        {
            mm[i] = new int[rows];
        }
        pno = 0; n = 0;
        TIMING_START(det_bound);
        B = detbound( M, rows );
        TIMING_END(det_bound);
        q = 1;
        TIMING_START(det_numprimes);
        while ( B > q && n < cf_getNumBigPrimes() )
        {
            q *= cf_getBigPrime( n );
            n++;
        }
        TIMING_END(det_numprimes);

        CFArray X(1,n), Q(1,n);

        while ( pno < n )
        {
            p = cf_getBigPrime( pno );
            setCharacteristic( p );
            // map matrix into char p
            TIMING_START(det_mapping);
            fill_int_mat( M, mm, rows );
            TIMING_END(det_mapping);
            pno++;
            DEBOUT( cerr, "." );
            TIMING_START(det_determinant);
            intdet = determinant( mm, rows );
            TIMING_END(det_determinant);
            setCharacteristic( 0 );
            X[pno] = intdet;
            Q[pno] = p;
        }
        TIMING_START(det_chinese);
        chineseRemainder( X, Q, x, q );
        TIMING_END(det_chinese);
        Qhalf = q / 2;
        if ( x > Qhalf )
            x = x - q;
        for ( i = 0; i < rows; i++ )
            delete [] mm[i];
        delete [] mm;
        return x;
    }
    else
    {
        CFMatrix m( M );
        CanonicalForm divisor = 1, pivot, mji;
        int i, j, k, sign = 1;
        for ( i = 1; i <= rows; i++ ) {
            pivot = m(i,i); k = i;
            for ( j = i+1; j <= rows; j++ ) {
                if ( betterpivot( pivot, m(j,i) ) ) {
                    pivot = m(j,i);
                    k = j;
                }
            }
            if ( pivot.isZero() )
                return 0;
            if ( i != k )
            {
                m.swapRow( i, k );
                sign = -sign;
            }
            for ( j = i+1; j <= rows; j++ )
            {
                if ( ! m(j,i).isZero() )
                {
                    divisor *= pivot;
                    mji = m(j,i);
                    m(j,i) = 0;
                    for ( k = i+1; k <= rows; k++ )
                        m(j,k) = m(j,k) * pivot - m(i,k)*mji;
                }
            }
        }
        pivot = sign;
        for ( i = 1; i <= rows; i++ )
            pivot *= m(i,i);
        return pivot / divisor;
    }
}

CanonicalForm
determinant2( const CFMatrix & M, int rows )
{
    typedef int* int_ptr;

    ASSERT( rows <= M.rows() && rows <= M.columns() && rows > 0, "undefined determinant" );
    if ( rows == 1 )
        return M(1,1);
    else  if ( rows == 2 )
        return M(1,1)*M(2,2)-M(2,1)*M(1,2);
    else  if ( matrix_in_Z( M, rows ) ) {
        int ** mm = new int_ptr[rows];
        CanonicalForm QQ, Q, Qhalf, mnew, q, qnew, B;
        CanonicalForm det, detnew, qdet;
        int i, p, pcount, pno, intdet;
        bool ok;

        // initialize room to hold the result and the result mod p
        for ( i = 0; i < rows; i++ ) {
            mm[i] = new int[rows];
        }

        // calculate the bound for the result
        B = detbound( M, rows );

        // find a first solution mod p
        pno = 0;
        do {
            p = cf_getBigPrime( pno );
            setCharacteristic( p );
            // map matrix into char p
            ok = fill_int_mat( M, mm, rows );
            pno++;
        } while ( ! ok && pno < cf_getNumPrimes() );
        // initialize the result matrix with first solution
        // solve mod p
        DEBOUT( cerr, "." );
        intdet = determinant( mm, rows );
        setCharacteristic( 0 );
        det = intdet;
        // Q so far
        Q = p;
        QQ = p;
        while ( Q < B && cf_getNumPrimes() > pno ) {
            // find a first solution mod p
            do {
                p = cf_getBigPrime( pno );
                setCharacteristic( p );
                // map matrix into char p
                ok = fill_int_mat( M, mm, rows );
                pno++;
            } while ( ! ok && pno < cf_getNumPrimes() );
            // initialize the result matrix with first solution
            // solve mod p
            DEBOUT( cerr, "." );
            intdet = determinant( mm, rows );
            setCharacteristic( 0 );
            qdet = intdet;
            // Q so far
            q = p;
            QQ *= p;
            pcount = 0;
            while ( QQ < B && cf_getNumPrimes() > pno && pcount < 500 ) {
                do {
                    p = cf_getBigPrime( pno );
                    setCharacteristic( p );
                    ok = true;
                    // map matrix into char p
                    ok = fill_int_mat( M, mm, rows );
                    pno++;
                } while ( ! ok && cf_getNumPrimes() > pno );
                // solve mod p
                DEBOUT( cerr, "." );
                intdet = determinant( mm, rows );
                // found a solution mod p
                // now chinese remainder it to a solution mod Q*p
                setCharacteristic( 0 );
                chineseRemainder( qdet, q, intdet, p, detnew, qnew );
                qdet = detnew;
                q = qnew;
                QQ *= p;
                pcount++;
            }
            DEBOUT( cerr, "*" );
            chineseRemainder( det, Q, qdet, q, detnew, qnew );
            Q = qnew;
            QQ = Q;
            det = detnew;
        }
        if ( ! ok )
            fuzzy_result = true;
        else
            fuzzy_result = false;
        // store the result in M
        Qhalf = Q / 2;
        if ( det > Qhalf )
            det = det - Q;
        for ( i = 0; i < rows; i++ )
            delete [] mm[i];
        delete [] mm;
        return det;
    }
    else {
        CFMatrix m( M );
        CanonicalForm divisor = 1, pivot, mji;
        int i, j, k, sign = 1;
        for ( i = 1; i <= rows; i++ ) {
            pivot = m(i,i); k = i;
            for ( j = i+1; j <= rows; j++ ) {
                if ( betterpivot( pivot, m(j,i) ) ) {
                    pivot = m(j,i);
                    k = j;
                }
            }
            if ( pivot.isZero() )
                return 0;
            if ( i != k ) {
                m.swapRow( i, k );
                sign = -sign;
            }
            for ( j = i+1; j <= rows; j++ ) {
                if ( ! m(j,i).isZero() ) {
                    divisor *= pivot;
                    mji = m(j,i);
                    m(j,i) = 0;
                    for ( k = i+1; k <= rows; k++ )
                        m(j,k) = m(j,k) * pivot - m(i,k)*mji;
                }
            }
        }
        pivot = sign;
        for ( i = 1; i <= rows; i++ )
            pivot *= m(i,i);
        return pivot / divisor;
    }
}

static CanonicalForm
bound ( const CFMatrix & M )
{
    DEBINCLEVEL( cerr, "bound" );
    int rows = M.rows(), cols = M.columns();
    CanonicalForm sum = 0;
    int i, j;
    for ( i = 1; i <= rows; i++ )
        for ( j = 1; j <= rows; j++ )
            sum += M(i,j) * M(i,j);
    DEBOUTLN( cerr, "bound(matrix)^2 = " << sum );
    CanonicalForm vmax = 0, vsum;
    for ( j = rows+1; j <= cols; j++ ) {
        vsum = 0;
        for ( i = 1; i <= rows; i++ )
            vsum += M(i,j) * M(i,j);
        if ( vsum > vmax ) vmax = vsum;
    }
    DEBOUTLN( cerr, "bound(lhs)^2 = " << vmax );
    sum += vmax;
    DEBOUTLN( cerr, "bound(overall)^2 = " << sum );
    DEBDECLEVEL( cerr, "bound" );
    return sqrt( sum ) + 1;
}


CanonicalForm
detbound ( const CFMatrix & M, int rows )
{
    CanonicalForm sum = 0, prod = 2;
    int i, j;
    for ( i = 1; i <= rows; i++ ) {
        sum = 0;
        for ( j = 1; j <= rows; j++ )
            sum += M(i,j) * M(i,j);
        prod *= 1 + sqrt(sum);
    }
    return prod;
}


// solve returns false if computation failed
// extmat is overwritten: output is Id mat followed by solution(s)

bool
solve ( int **extmat, int nrows, int ncols )
{
    DEBINCLEVEL( cerr, "solve" );
    int i, j, k;
    int rowpivot, pivotrecip; // all FF
    int * rowi; // FF
    int * rowj; // FF
    int * swap; // FF
    // triangularization
    for ( i = 0; i < nrows; i++ ) {
        //find "pivot"
        for (j = i; j < nrows; j++ )
            if ( extmat[j][i] != 0 ) break;
        if ( j == nrows ) {
            DEBOUTLN( cerr, "solve failed" );
            DEBDECLEVEL( cerr, "solve" );
            return false;
        }
        if ( j != i ) {
            swap = extmat[i]; extmat[i] = extmat[j]; extmat[j] = swap;
        }
        pivotrecip = ff_inv( extmat[i][i] );
        rowi = extmat[i];
        for ( j = 0; j < ncols; j++ )
            rowi[j] = ff_mul( pivotrecip, rowi[j] );
        for ( j = i+1; j < nrows; j++ ) {
            rowj = extmat[j];
            rowpivot = rowj[i];
            if ( rowpivot == 0 ) continue;
            for ( k = i; k < ncols; k++ )
                rowj[k] = ff_sub( rowj[k], ff_mul( rowpivot, rowi[k] ) );
        }
    }
    // matrix is now upper triangular with 1s down the diagonal
    // back-substitute
    for ( i = nrows-1; i >= 0; i-- ) {
        rowi = extmat[i];
        for ( j = 0; j < i; j++ ) {
            rowj = extmat[j];
            rowpivot = rowj[i];
            if ( rowpivot == 0 ) continue;
            for ( k = i; k < ncols; k++ )
                rowj[k] = ff_sub( rowj[k], ff_mul( rowpivot, rowi[k] ) );
            // for (k=nrows; k<ncols; k++) rowj[k] = ff_sub(rowj[k], ff_mul(rowpivot, rowi[k]));
        }
    }
    DEBOUTLN( cerr, "solve succeeded" );
    DEBDECLEVEL( cerr, "solve" );
    return true;
}

int
determinant ( int **extmat, int n )
{
    int i, j, k;
    int divisor, multiplier, rowii, rowji; // all FF
    int * rowi; // FF
    int * rowj; // FF
    int * swap; // FF
    // triangularization
    multiplier = 1;
    divisor = 1;

    for ( i = 0; i < n; i++ ) {
        //find "pivot"
        for (j = i; j < n; j++ )
            if ( extmat[j][i] != 0 ) break;
        if ( j == n ) return 0;
        if ( j != i ) {
            multiplier = ff_neg( multiplier );
            swap = extmat[i]; extmat[i] = extmat[j]; extmat[j] = swap;
        }
        rowi = extmat[i];
        rowii = rowi[i];
        for ( j = i+1; j < n; j++ ) {
            rowj = extmat[j];
            rowji = rowj[i];
            if ( rowji == 0 ) continue;
            divisor = ff_mul( divisor, rowii );
            for ( k = i; k < n; k++ )
                rowj[k] = ff_sub( ff_mul( rowj[k], rowii ), ff_mul( rowi[k], rowji ) );
        }
    }
    multiplier = ff_mul( multiplier, ff_inv( divisor ) );
    for ( i = 0; i < n; i++ )
        multiplier = ff_mul( multiplier, extmat[i][i] );
    return multiplier;
}

void
solveVandermondeT ( const CFArray & a, const CFArray & w, CFArray & x, const Variable & z )
{
    CanonicalForm Q = 1, q, p;
    CFIterator j;
    int i, n = a.size();

    for ( i = 1; i <= n; i++ )
        Q *= ( z - a[i] );
    for ( i = 1; i <= n; i++ ) {
        q = Q / ( z - a[i] );
        p = q / q( a[i], z );
        x[i] = 0;
        for ( j = p; j.hasTerms(); j++ )
            x[i] += w[j.exp()+1] * j.coeff();
    }
}
