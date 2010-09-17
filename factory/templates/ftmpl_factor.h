/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_FACTOR_H
#define INCL_FACTOR_H

#include <factory/factoryconf.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
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
    void print ( OSTREAM& ) const;
#endif /* NOSTREAMIO */
};

template <class T> int
operator== ( const Factor<T>&, const Factor<T>& );

#ifndef NOSTREAMIO
template <class T>
OSTREAM& operator<< ( OSTREAM & os, const Factor<T> & f );
#endif /* NOSTREAMIO */

#endif /* ! INCL_FACTOR_H */
