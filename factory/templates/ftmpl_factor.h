/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_factor.h,v 1.4 1997-06-19 13:11:15 schmidt Exp $ */

#ifndef INCL_FACTOR_H
#define INCL_FACTOR_H

#include <factoryconf.h>

#ifndef NOSTREAMIO
#include <iostream.h>
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
    friend int operator== ( const Factor<T>&, const Factor<T>& );
#ifndef NOSTREAMIO
    void print ( ostream& ) const;
#endif /* NOSTREAMIO */
};

#ifndef NOSTREAMIO
template <class T>
ostream& operator<< ( ostream & os, const Factor<T> & f );
#endif /* NOSTREAMIO */

#endif /* ! INCL_FACTOR_H */
