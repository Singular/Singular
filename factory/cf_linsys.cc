// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_linsys.cc,v 1.0 1996-05-17 10:59:44 stobbe Exp $

/*
$Log: not supported by cvs2svn $
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

static CanonicalForm bound ( CanonicalForm ** M, int rows, int cols );


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
    while ( Q < B ) {
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
    return sum;
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
