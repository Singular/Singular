// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_linsys.cc,v 1.1 1996-07-08 08:22:51 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:44  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_primes.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_chinese.h"
#include "ffops.h"
#include "cf_primes.h"


static bool solve ( int **extmat, int nrows, int ncols );
int determinant ( int **extmat, int n );

static CanonicalForm bound ( CanonicalForm ** M, int rows, int cols );
CanonicalForm detbound ( const CFMatrix & M, int rows );

bool matrix_in_Z( const CFMatrix & M, int rows )
{
    int i, j;
    for ( i = 1; i <= rows; i++ )
	for ( j = 1; j <= rows; j++ )
	    if ( ! M(i,j).inZ() )
		return false;
    return true;
}

bool betterpivot ( const CanonicalForm & oldpivot, const CanonicalForm & newpivot )
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

bool fuzzy_result;

void
linearSystemSolve( CanonicalForm ** M, int rows, int cols )
{
    CanonicalForm ** MM = new (CanonicalForm*)[rows];
    int ** mm = new (int*)[rows];
    CanonicalForm Q, Qhalf, mnew, qnew, B;
    int i, j, p, pno;
    bool ok;

    // initialize room to hold the result and the result mod p
    for ( i = 0; i < rows; i++ ) {
	MM[i] = new CanonicalForm[cols];
	mm[i] = new int[cols];
    }

    // calculate the bound for the result
    B = bound( M, rows, cols );
    cout << "bound = " << B << endl;

    // find a first solution mod p
    pno = 0;
    do {
	cout << "trying prime(" << pno << ") = " << flush;
	p = cf_getBigPrime( pno );
	cout << p << endl;
	setCharacteristic( p );
	// map matrix into char p
	for ( i = 0; i < rows; i++ )
	    for ( j = 0; j < cols; j++ )
		mm[i][j] = mapinto( M[i][j] ).intval();
	// solve mod p
	ok = solve( mm, rows, cols );
	pno++;
    } while ( ! ok );
    // initialize the result matrix with first solution
    setCharacteristic( 0 );
    for ( i = 0; i < rows; i++ )
	for ( j = rows; j < cols; j++ )
	    MM[i][j] = mm[i][j];
    // Q so far
    Q = p;
    while ( Q < B && pno < cf_getNumBigPrimes() ) {
	do {
	    cout << "trying prime(" << pno << ") = " << flush;
	    p = cf_getBigPrime( pno );
	    cout << p << endl;
	    setCharacteristic( p );
	    for ( i = 0; i < rows; i++ )
		for ( j = 0; j < cols; j++ )
		    mm[i][j] = mapinto( M[i][j] ).intval();
	    // solve mod p
	    ok = solve( mm, rows, cols );
	    pno++;
	} while ( ! ok );
	// found a solution mod p
	// now chinese remainder it to a solution mod Q*p
	setCharacteristic( 0 );
	for ( i = 0; i < rows; i++ )
	    for ( j = rows; j < cols; j++ ) {
		chineseRemainder( MM[i][j], Q, CanonicalForm(mm[i][j]), CanonicalForm(p), mnew, qnew );
		MM[i][j] = mnew;
	    }
	Q = qnew;
    }
    if ( pno == cf_getNumBigPrimes() )
	fuzzy_result = true;
    else
	fuzzy_result = false;
    // store the result in M
    Qhalf = Q / 2;
    for ( i = 0; i < rows; i++ ) {
	for ( j = rows; j < cols; j++ )
	    if ( MM[i][j] > Qhalf )
		M[i][j] = MM[i][j] - Q;
	    else
		M[i][j] = MM[i][j];
	delete [] MM[i];
	delete [] mm[i];
    }
    delete [] MM;
    delete [] mm;
}

CanonicalForm
determinant( const CFMatrix & M, int rows )
{
    ASSERT( rows <= M.rows() && rows <= M.columns() && rows > 0, "undefined determinant" );
    if ( rows == 1 )
	return M(1,1);
    else  if ( rows == 2 )
	return M(1,1)*M(2,2)-M(2,1)*M(1,2);
    else  if ( matrix_in_Z( M, rows ) ) {
	int ** mm = new (int*)[rows];
	CanonicalForm Q, Qhalf, mnew, qnew, B;
	CanonicalForm det, detnew;
	int i, j, p, pno, intdet;
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
	    ok = true;
	    // map matrix into char p
	    for ( i = 0; i < rows && ok; i++ )
		for ( j = 0; j < rows && ok; j++ ) {
		    if ( M(i+1,j+1).isZero() )
			mm[i][j] = 0;
		    else {
			mm[i][j] = mapinto( M(i+1,j+1) ).intval();
			ok = mm[i][j] != 0;
		    }
		}
	    pno++;
	} while ( ! ok && pno < cf_getNumPrimes() );
	// initialize the result matrix with first solution
	// solve mod p
	intdet = determinant( mm, rows );
	setCharacteristic( 0 );
	det = intdet;
	// Q so far
	Q = p;
	while ( Q < B && cf_getNumPrimes() > pno ) {
	    do {
		p = cf_getBigPrime( pno );
		setCharacteristic( p );
		ok = true;
		// map matrix into char p
		for ( i = 0; i < rows && ok; i++ )
		    for ( j = 0; j < rows && ok; j++ ) {
			if ( M(i+1,j+1).isZero() )
			    mm[i][j] = 0;
			else {
			    mm[i][j] = mapinto( M(i+1,j+1) ).intval();
			    ok = mm[i][j] != 0;
			}
		    }
		pno++;
	    } while ( ! ok && cf_getNumPrimes() > pno );
	    // solve mod p
	    intdet = determinant( mm, rows );
	    // found a solution mod p
	    // now chinese remainder it to a solution mod Q*p
	    setCharacteristic( 0 );
	    chineseRemainder( det, Q, intdet, p, detnew, qnew );
	    det = detnew;
	    Q = qnew;
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
bound ( CanonicalForm ** M, int rows, int cols )
{
    CanonicalForm sum = 0;
    int i, j;
    for ( i = 0; i < rows; i++ )
	for ( j = 0; j < rows; j++ )
	    sum += M[i][j] * M[i][j];
    CanonicalForm vmax = 0, vsum;
    for ( j = rows; j < cols; j++ ) {
	vsum = 0;
	for ( i = 0; i < rows; i++ )
	    vsum += M[i][j] * M[i][j];
	if ( vsum > vmax ) vmax = vsum;
    }
    sum += vmax;
    return sqrt( sum );
}


CanonicalForm
detbound ( const CFMatrix & M, int rows )
{
    CanonicalForm sum = 0, prod = 1;
    int i, j;
    for ( i = 1; i <= rows; i++ ) {
	sum = 0;
	for ( j = 1; j <= rows; j++ )
	    sum += M(i,j) * M(i,j);
	prod *= sum;
    }
    return 2 * sqrt( prod );
}


// solve returns false if computation failed
// extmat is overwritten: output is Id mat followed by solution(s)

bool
solve ( int **extmat, int nrows, int ncols )
{
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
	if ( j == nrows ) return false;
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
