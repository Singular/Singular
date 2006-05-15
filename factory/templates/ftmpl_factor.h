/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_factor.h,v 1.6 2006-05-15 08:17:56 Singular Exp $ */

#ifndef INCL_FACTOR_H
#define INCL_FACTOR_H

#include <factoryconf.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
#endif /* NOSTREAMIO */


template <class T>
class Factor {
private:
    T _factor;
    int _exp;
public:
    Factor() : _factor(1), _exp(0) {}
    Factor( const Factor<T> & f ) : _factor(f._factor), _exp(f._exp) {}
    Factor( const T & f, int e ) : _factor(f), _exp(e) {}
    Factor( const T & f ) : _factor(f), _exp(1) {}
    ~Factor() {}
    Factor<T>& operator= ( const Factor<T>& );
    Factor<T>& operator= ( const T& );
    T factor() const { return _factor; }
    int exp() const { return _exp; }
    T value() const { return power( _factor, _exp ); }
    Factor<T>& operator+= ( int i ) { _exp += i; return *this; }
    Factor<T>& operator*= ( int i ) { _exp *= i; return *this; }
    Factor<T>& operator*= ( const T & f ) { _factor *= f; return *this; }
#ifndef NOSTREAMIO
    void print ( ostream& ) const;
#endif /* NOSTREAMIO */
};

template <class T> int
operator== ( const Factor<T>&, const Factor<T>& );

#ifndef NOSTREAMIO
template <class T>
ostream& operator<< ( ostream & os, const Factor<T> & f );
#endif /* NOSTREAMIO */

#endif /* ! INCL_FACTOR_H */
