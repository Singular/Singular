// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_array.h,v 1.2 1997-03-27 10:29:30 schmidt Exp $

#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1996/07/16 12:24:57  stobbe
"operators += and *= deleted sincethey are not needed in an general
array class als well as the functions sum, prod, crossprod.
"

Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

*/

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

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
#ifndef NOSTREAMIO
    void print ( ostream& ) const;
#endif /* NOSTREAMIO */
};

#ifndef NOSTREAMIO
template <class T>
ostream& operator<< ( ostream & os, const Array<T> & a );
#endif /* NOSTREAMIO */

#endif /* INCL_ARRAY_H */
