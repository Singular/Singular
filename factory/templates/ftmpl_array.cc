// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_array.cc,v 1.3 1997-04-15 09:56:36 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/03/27 10:28:51  schmidt
error check changed to ASSERT

Revision 1.1  1996/07/16 12:24:56  stobbe
"operators += and *= deleted sincethey are not needed in an general
array class als well as the functions sum, prod, crossprod.
"

Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

*/

/*MAKEHEADER*/
#include "templates/config.h"

/*MAKEHEADER*/
#include "templates/assert.h"

#include <templates/array.h>

template <class T>
Array<T>::Array() : data(0), _min(0), _max(-1), _size(0)
{
}

template <class T>
Array<T>::Array( const Array<T> & a )
{
    if ( a._size > 0 ) {
	_min = a._min;
	_max = a._max;
	_size = a._size;
	data = new T[_size];
	for ( int i = 0; i < _size; i++ )
	    data[i] = a.data[i];
    }
    else {
	data = 0;
	_min = _size = 0;
	_max = -1;
    }
}

template <class T>
Array<T>::Array( int i )
{
    _min = 0;
    _max = i-1;
    _size = i;
    if ( i == 0 )
	data = 0;
    else
	data = new T[_size];
}

template <class T>
Array<T>::Array( int min, int max )
{
    if ( max < min ) {
	_min = _size = 0;
	_max = -1;
	data = 0;
    }
    else {
	_min = min;
	_max = max;
	_size = _max - _min + 1;
	data = new T[_size];
    }
}

template <class T>
Array<T>::~Array()
{
    delete [] data;
}

template <class T>
Array<T>& Array<T>::operator= ( const Array<T> & a )
{
    if ( this != &a ) {
	delete [] data;
	_min = a._min;
	_max = a._max;
	_size = a._size;
	if ( a._size > 0 ) {
	    _size = a._size;
	    data = new T[_size];
	    for ( int i = 0; i < _size; i++ )
		data[i] = a.data[i];
	}
	else {
	    data = 0;
	    _size = 0;
	}
    }
    return *this;
}

template <class T>
T& Array<T>::operator[] ( int i ) const
{
    ASSERT( i >= _min && i <= _max, "warning: array size mismatch." );
    return data[i-_min];
}

template <class T>
int Array<T>::size() const
{
    return _size;
}

template <class T>
int Array<T>::min() const
{
    return _min;
}

template <class T>
int Array<T>::max() const
{
    return _max;
}

/*
template <class T>
Array<T>& Array<T>::operator*= ( const T & t )
{
    for ( int i = 0; i < _size; i++ )
	data[i] *= t;
    return *this;
}

template <class T>
Array<T>& Array<T>::operator+= ( const T & t )
{
    for ( int i = 0; i < _size; i++ )
	data[i] += t;
    return *this;
}

template <class T>
Array<T>& Array<T>::operator+= ( const Array<T> & a )
{
    ASSERT ( _size == a._size, "warning: array size mismatch." );
    for ( int i = 0; i < _size; i++ )
	data[i] += a.data[i];
    return *this;
}
*/

#ifndef NOSTREAMIO
template <class T>
void Array<T>::print ( ostream & os ) const
{
    if ( _size == 0 )
	os << "( )";
    else {
	os << "( " << data[0];
	for ( int i = 1; i < _size; i++ )
	    os << ", " << data[i];
	os << " )";
    }
}

template <class T>
ostream& operator<< ( ostream & os, const Array<T> & a )
{
    a.print( os );
    return os;
}
#endif /* NOSTREAMIO */
