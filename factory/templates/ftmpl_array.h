// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_array.h,v 1.1 1996-07-16 12:24:57 stobbe Exp $

#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

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
    void print ( ostream& ) const;
};

template <class T>
ostream& operator<< ( ostream & os, const Array<T> & a );

#endif /* INCL_ARRAY_H */
