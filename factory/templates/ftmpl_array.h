// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_array.h,v 1.0 1996-05-17 11:06:32 stobbe Exp $

#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>

template <class T>
class Array {
private:
    T * data;
    int _min;
    int _max;
    int _size;
public:
    Array();
    Array( const Array<T>& );
    Array( int size );
    Array( int min, int max );
    ~Array();
    Array<T>& operator= ( const Array<T>& );
    T& operator[] ( int i ) const;
    int size() const;
    int min() const;
    int max() const;
    Array<T>& operator*= ( const T& );
    Array<T>& operator+= ( const T& );
    Array<T>& operator+= ( const Array<T>& );
    void print ( ostream& ) const;
};

template <class T>
ostream& operator<< ( ostream & os, const Array<T> & a );

template <class T>
T sum ( const Array<T>&, int f, int l );

template <class T>
T prod ( const Array<T>&, int f, int l );

// and because HP CC does not support default arguments in templates:

template <class T>
T sum ( const Array<T>& );

template <class T>
T prod ( const Array<T>& );

template <class T>
T crossprod ( const Array<T>&, const Array<T>& );

#endif /* INCL_ARRAY_H */
