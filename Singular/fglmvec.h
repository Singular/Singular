// emacs edit mode for this file is -*- C++ -*-
// $Id: fglmvec.h,v 1.1.1.1 1997-03-19 13:18:55 obachman Exp $

#ifndef FGLMVEC_H
#define FGLMVEC_H

#include <iostream.h>
#include "numbers.h"

class Mat;

class fglmVectorRep;

class subFglmVector;

class fglmVector
{
protected:
    fglmVectorRep * rep;
    void makeUnique();
    fglmVector( fglmVectorRep * rep );
public:
    fglmVector();
    fglmVector( int size );
    fglmVector( int size, int basis );
    fglmVector( const fglmVector & v );
    ~fglmVector();
    int size() const;
    int numNonZeroElems() const;

    fglmVector & operator = ( const fglmVector & v );
    subFglmVector operator () ( int min, int max );
    const subFglmVector operator () ( int min, int max ) const;

    int operator == ( const fglmVector & );
    int operator != ( const fglmVector & );
    int isZero();

    fglmVector & operator += ( const fglmVector & );
    fglmVector & operator -= ( const fglmVector & );
    fglmVector & operator *= ( const number & );
    fglmVector & operator /= ( const number & );
    fglmVector & operator *= ( const Mat & );
    friend fglmVector operator - ( const fglmVector & v );
    friend fglmVector operator + ( const fglmVector & lhs, const fglmVector & rhs );
    friend fglmVector operator - ( const fglmVector & lhs, const fglmVector & rhs );
    friend fglmVector operator * ( const fglmVector & v, const number n );
    friend fglmVector operator * ( const number n, const fglmVector & v );
    friend fglmVector operator * ( const Mat & M, const fglmVector & v );

    const number getconstelem( int i ) const;
    number & getelem( int i );
    void setelem( int i, number & n );

    number gcd() const;
    number clearDenom();

#ifdef FGLM_DEBUG
    void test() const;
#endif

    void print( ostream & s ) const;
    friend ostream & operator<< ( ostream & s, const fglmVector & v )
    {
	v.print(s);
	return s;
    }
    friend class subFglmVector;
};

class subFglmVector
{
protected:
    int minelem;
    int maxelem;
    fglmVectorRep * rep;
public:
    subFglmVector( int min, int max, fglmVectorRep * rep );
    subFglmVector( const subFglmVector & v );

    subFglmVector & operator = ( const subFglmVector & );
    subFglmVector & operator = ( const fglmVector & );
//     const number & operator [] ( int i ) const;
//    const poly & operator [] ( int i ) const;
    operator fglmVector () const;

    int operator == ( const fglmVector & );
    int operator != ( const fglmVector & );

    subFglmVector & operator += ( const fglmVector & );
    subFglmVector & operator += ( const subFglmVector & );
    subFglmVector & operator -= ( const fglmVector & );
    subFglmVector & operator -= ( const subFglmVector & );
    subFglmVector & operator *= ( const number & );
    friend class fglmVector;
};

#endif
