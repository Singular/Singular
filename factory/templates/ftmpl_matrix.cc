/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <factory/templates/ftmpl_matrix.h>

template <class T>
Matrix<T>::Matrix( int nr, int nc ) : NR(nr), NC(nc)
{
    ASSERT( (nr > 0 && nc > 0) || (nr == 0 && nc == 0), "illegal index" );
    if ( nr == 0 )
	elems = 0;
    else {
	int i;
	elems = new T_ptr[nr];
	for ( i = 0; i < nr; i++ )
	    elems[i] = new T[nc];
    }
}

template <class T>
Matrix<T>::Matrix( const Matrix<T>& M ) : NR(M.NR), NC(M.NC)
{
    if ( NR == 0 )
	elems = 0;
    else {
	int i, j;
	elems = new T_ptr[NR];
	for ( i = 0; i < NR; i++ ) {
	    elems[i] = new T[NC];
	    for ( j = 0; j < NC; j++ )
		elems[i][j] = M.elems[i][j];
	}
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    if ( elems != 0 ) {
	int i;
	for ( i = 0; i < NR; i++ )
	    delete [] elems[i];
	delete [] elems;
    }
}

template <class T>
Matrix<T>& Matrix<T>::operator= ( const Matrix<T>& M )
{
    if ( this != &M ) {
	int i, j;
	if ( NR != M.NR || NC != M.NC ) {
	    for ( i = 0; i < NR; i++ )
		delete [] elems[i];
	    delete elems;
	    NR = M.NR; NC = M.NC;
	    elems = new T_ptr[NR];
	    for ( i = 0; i < NR; i++ )
		elems[i] = new T[NC];
	}
	for ( i = 0; i < NR; i++ )
	    for ( j = 0; j < NC; j++ )
		elems[i][j] = M.elems[i][j];
    }
    return *this;
}

template <class T>
SubMatrix<T> Matrix<T>::operator[] ( int i )
{
    ASSERT( i > 0 && i <= NR, "illegal index" );
    return SubMatrix<T>( i, i, 1, NC, *this );
}

template <class T>
const SubMatrix<T> Matrix<T>::operator[] ( int i ) const
{
    ASSERT( i > 0 && i <= NR, "illegal index" );
    return SubMatrix<T>( i, i, 1, NC, *this );
}

template <class T>
T& Matrix<T>::operator() ( int row, int col )
{
    ASSERT( row > 0 && col > 0 && row <= NR && col <= NC, "illegal index" );
    return elems[row-1][col-1];
}

template <class T>
T Matrix<T>::operator() ( int row, int col ) const
{
    ASSERT( row > 0 && col > 0 && row <= NR && col <= NC, "illegal index" );
    return elems[row-1][col-1];
}

template <class T>
SubMatrix<T> Matrix<T>::operator() ( int rmin, int rmax, int cmin, int cmax )
{
    ASSERT( rmin > 0 && rmax <= NR && rmin <= rmax && cmin > 0 && cmax <= NC && cmin <= cmax , "illegal index" );
    return SubMatrix<T>( rmin, rmax, cmin, cmax, *this );
}

template <class T>
const SubMatrix<T> Matrix<T>::operator() ( int rmin, int rmax, int cmin, int cmax ) const
{
    ASSERT( rmin > 0 && rmax <= NR && rmin <= rmax && cmin > 0 && cmax <= NC && cmin <= cmax , "illegal index" );
    return SubMatrix<T>( rmin, rmax, cmin, cmax, *this );
}

template <class T>
void Matrix<T>::swapRow ( int i, int j )
{
    ASSERT( i > 0 && i <= NR && j > 0 && j <= NR, "illegal index" );
    if ( i != j ) {
	i--; j--;
	T * h = elems[i];
	elems[i] = elems[j];
	elems[j] = h;
    }
}

template <class T>
void Matrix<T>::swapColumn ( int i, int j )
{
    ASSERT( i > 0 && i <= NC && j > 0 && j <= NC, "illegal index" );
    if ( i != j ) {
	int k;
	i--; j--;
	for ( k = 0; k < NR; k++ ) {
	    T h = elems[k][i];
	    elems[k][i] = elems[k][j];
	    elems[k][j] = h;
	}
    }
}

#ifndef NOSTREAMIO
template <class T>
void Matrix<T>::printrow ( OSTREAM & s, int i ) const
{
    s << "( " << elems[i][0];
    for ( int j = 1; j < NC; j++ )
	s << ", " << elems[i][j];
    s << " )";
}

template <class T>
void Matrix<T>::print( OSTREAM& s ) const
{
    if ( NR == 0 )
	s << "( )";
    else if ( NR == 1 ) {
	s << "( ";
	printrow( s, 0 );
	s << " )";
    }
    else {
	int i;
	s << "(\n";
	printrow( s, 0 );
	for ( i = 1; i < NR; i++ ) {
	    s << ",\n";
	    printrow( s, i );
	}
	s << "\n)";
    }
}
#endif /* NOSTREAMIO */

template <class T>
Matrix<T> operator+ ( const Matrix<T>& lhs, const Matrix<T>& rhs )
{
    ASSERT( lhs.NR == rhs.NR && lhs.NC == rhs.NC, "incompatible matrices" );
    Matrix<T> res( lhs.NR, rhs.NR );
    int i, j;
    for ( i = 0; i < lhs.NR; i++ )
	for ( j = 0; j < lhs.NC; j++ )
	    res.elems[i][j] = lhs.elems[i][j] + rhs.elems[i][j];
    return res;
}

template <class T>
Matrix<T> operator- ( const Matrix<T>& lhs, const Matrix<T>& rhs )
{
    ASSERT( lhs.NR == rhs.NR && lhs.NC == rhs.NC, "incompatible matrices" );
    Matrix<T> res( lhs.NR, rhs.NR );
    int i, j;
    for ( i = 0; i < lhs.NR; i++ )
	for ( j = 0; j < lhs.NC; j++ )
	    res.elems[i][j] = lhs.elems[i][j] - rhs.elems[i][j];
    return res;
}

template <class T>
Matrix<T> operator* ( const Matrix<T>& lhs, const T& rhs )
{
    Matrix<T> res( lhs.NR, lhs.NC );
    int i, j;
    for ( i = 0; i < lhs.NR; i++ )
	for ( j = 0; j < lhs.NC; j++ )
	    res.elems[i][j] = lhs.elems[i][j] * rhs;
    return res;
}

template <class T>
Matrix<T> operator* ( const T& lhs, const Matrix<T>& rhs )
{
    Matrix<T> res( rhs.NR, rhs.NC );
    int i, j;
    for ( i = 0; i < rhs.NR; i++ )
	for ( j = 0; j < rhs.NC; j++ )
	    res.elems[i][j] = rhs.elems[i][j] * lhs;
    return res;
}

template <class T>
Matrix<T> operator* ( const Matrix<T>& lhs, const Matrix<T>& rhs )
{
    ASSERT( lhs.NC == rhs.NR, "incompatible matrices" );
    Matrix<T> res( lhs.NR, rhs.NC );
    int i, j, k;
    for ( i = 0; i < lhs.NR; i++ )
	for ( j = 0; j < rhs.NC; j++ ) {
	    res[i][j] = 0;
	    for ( k = 0; k < lhs.NC; k++ )
		res[i][j]+= lhs.elems[i][k] * rhs.elems[k][j];
	}
    return res;
}

template <class T>
SubMatrix<T>::SubMatrix( int rmin, int rmax, int cmin, int cmax, const Matrix<T> & m ) : r_min(rmin), r_max(rmax), c_min(cmin), c_max(cmax), M((Matrix<T>&)m) {}

template <class T>
SubMatrix<T>::SubMatrix( const SubMatrix<T> & S ) : r_min(S.r_min), r_max(S.r_max), c_min(S.c_min), c_max(S.c_max), M(S.M) {}

template <class T>
SubMatrix<T>& SubMatrix<T>::operator= ( const Matrix<T>& S )
{
    ASSERT( r_max - r_min + 1 == S.NR && c_max - c_min + 1 == S.NC, "incompatible matrices" );
    if ( M.elems != S.elems ) {
	int i, j;
	for ( i = 0; i < S.NR; i++ )
	    for ( j = 0; j < S.NC; j++ )
		M.elems[r_min+i-1][c_min+j-1] = S.elems[i][j];
    }
    return *this;
}

template <class T>
SubMatrix<T>& SubMatrix<T>::operator= ( const SubMatrix<T>& S )
{
    ASSERT( r_max - r_min == S.r_max - S.r_min && c_max - c_min == S.c_max - S.c_min, "incompatible matrices" );
    int i, j, n, m;
    n = r_max - r_min + 1;
    m = c_max - c_min + 1;
    if ( M.elems == S.M.elems ) {
	if ( r_min < S.r_min ) {
	    for ( i = 0; i < n; i++ )
		for ( j = 0; j < m; j++ )
		    M.elems[r_min+i-1][c_min+j-1] = S.M.elems[S.r_min+i-1][S.c_min+j-1];
	}
	else if ( r_min > S.r_min ) {
	    for ( i = n-1; i >= 0; i-- )
		for ( j = 0; j < m; j++ )
		    M.elems[r_min+i-1][c_min+j-1] = S.M.elems[S.r_min+i-1][S.c_min+j-1];
	}
	else if ( c_min < S.c_min ) {
	    for ( j = 0; j < m; j++ )
		for ( i = 0; i < n; i++ )
		    M.elems[r_min+i-1][c_min+j-1] = S.M.elems[S.r_min+i-1][S.c_min+j-1];
	}
	else if ( c_min > S.c_min ) {
	    for ( j = m-1; j >= 0; j-- )
		for ( i = 0; i < n; i++ )
		    M.elems[r_min+i-1][c_min+j-1] = S.M.elems[S.r_min+i-1][S.c_min+j-1];
	}
    }
    else {
	for ( i = 0; i < n; i++ )
	    for ( j = 0; j < m; j++ )
		M.elems[r_min+i-1][c_min+j-1] = S.M.elems[S.r_min+i-1][S.c_min+j-1];
    }
    return *this;
}

template <class T>
SubMatrix<T>::operator Matrix<T>() const
{
    Matrix<T> res( r_max - r_min + 1, c_max - c_min + 1 );
    int i, j;
    int n = r_max - r_min + 1, m = c_max - c_min + 1;
    for ( i = 0; i < n; i++ )
	for ( j = 0; j < m; j++ )
	    res.elems[i][j] = M.elems[r_min+i-1][c_min+j-1];
    return res;
}

template <class T>
T SubMatrix<T>::operator[] ( int i ) const
{
    ASSERT( r_min == r_max && i >= c_min && i <= c_max, "illegal index" );
    return M.elems[r_min-1][i-1];
}

template <class T>
T& SubMatrix<T>::operator[] ( int i )
{
    ASSERT( r_min == r_max && i >= c_min && i <= c_max, "illegal index" );
    return M.elems[r_min-1][i-1];
}

#ifndef NOSTREAMIO
template <class T>
OSTREAM & operator<< ( OSTREAM & s, const Matrix<T>& M )
{
   M.print( s );
   return s;
}
#endif /* NOSTREAMIO */
