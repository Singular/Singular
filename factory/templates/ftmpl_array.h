/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_array.h,v 1.5 1997-06-19 13:11:13 schmidt Exp $ */

#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H

#include <factoryconf.h>

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

#endif /* ! INCL_ARRAY_H */
