/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_matrix.h,v 1.11 2005-06-13 09:46:35 Singular Exp $ */

#ifndef INCL_MATRIX_H
#define INCL_MATRIX_H

#include <factoryconf.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

template <class T>
class SubMatrix;

template <class T>
class Matrix;

#ifndef NOSTREAMIO
template <class T>
ostream& operator<< (ostream &, const Matrix<T> &);
#endif

template <class T>
class Matrix
{
private:
    int NR, NC;
    T ** elems;
#ifndef NOSTREAMIO
    void printrow ( ostream & s, int i ) const;
#endif /* NOSTREAMIO */
    typedef T* T_ptr;
public:
    Matrix() : NR(0), NC(0), elems(0) {}
    Matrix( int nr, int nc );
    Matrix( const Matrix<T>& M );
    ~Matrix();
    Matrix<T>& operator= ( const Matrix<T>& M );
    int rows() const { return NR; }
    int columns() const { return NC; }
    SubMatrix<T> operator[] ( int i );
    const SubMatrix<T> operator[] ( int i ) const;
    T& operator() ( int row, int col );
    T operator() ( int row, int col ) const;
    SubMatrix<T> operator() ( int rmin, int rmax, int cmin, int cmax );
    const SubMatrix<T> operator() ( int rmin, int rmax, int cmin, int cmax ) const;
    void swapRow( int i, int j );
    void swapColumn( int i, int j );
#ifndef NOSTREAMIO
    void print( ostream& s ) const;
    friend ostream & operator<< <T>( ostream & s, const Matrix<T>& M );
#endif /* NOSTREAMIO */
    friend class SubMatrix<T>;
};
    template <class T>
    Matrix<T> operator+ ( const Matrix<T>& lhs, const Matrix<T>& rhs );
    template <class T>
    Matrix<T> operator- ( const Matrix<T>& lhs, const Matrix<T>& rhs );
    template <class T>
    Matrix<T> operator* ( const Matrix<T>& lhs, const Matrix<T>& rhs );
    template <class T>
    Matrix<T> operator* ( const Matrix<T>& lhs, const T& rhs );
    template <class T>
    Matrix<T> operator* ( const T& lhs, const Matrix<T>& rhs );

template <class T>
class SubMatrix
{
private:
    int r_min, r_max, c_min, c_max;
    Matrix<T>& M;
    // we do not provide a default ctor, so nobody can declare an empty SubMatrix
    SubMatrix( int rmin, int rmax, int cmin, int cmax, const Matrix<T> & m );
public:
    SubMatrix( const SubMatrix<T> & S );
    SubMatrix<T>& operator= ( const SubMatrix<T>& S );
    SubMatrix<T>& operator= ( const Matrix<T>& S );
    operator Matrix<T>() const;
    T operator[] ( int i ) const;
    T& operator[] ( int i );
    friend class Matrix<T>;
};

#ifndef NOSTREAMIO
template <class T>
ostream & operator<< ( ostream & s, const Matrix<T>& M );
#endif /* NOSTREAMIO */

#endif /* ! INCL_MATRIX_H */
