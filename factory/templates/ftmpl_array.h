/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_array.h,v 1.6 2006-05-15 08:17:56 Singular Exp $ */

#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H

#include <factoryconf.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
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

#endif /* ! INCL_ARRAY_H */
