/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_AFACTOR_H
#define INCL_AFACTOR_H

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
class AFactor {
private:
    T _factor;
    T _minpoly;
    int _exp;
public:
    AFactor() : _factor(1), _minpoly (1), _exp(0) {}
    AFactor( const AFactor<T> & f ) : _factor(f._factor), _minpoly (f._minpoly), _exp(f._exp) {}
    AFactor( const T & f, const T & m, int e ) : _factor(f), _minpoly (m), _exp(e) {}
    AFactor( const T & f, const T & m ) : _factor(f), _minpoly (m), _exp(1) {}
    ~AFactor() {}
    AFactor<T>& operator= ( const AFactor<T>& );
    T factor() const { return _factor; }
    T minpoly() const { return _minpoly; }
    int exp() const { return _exp; }
    T value() const { return power( _factor, _exp ); }
#ifndef NOSTREAMIO
    void print ( OSTREAM& ) const;
#endif /* NOSTREAMIO */
};

template <class T> int
operator== ( const AFactor<T>&, const AFactor<T>& );

#ifndef NOSTREAMIO
template <class T>
OSTREAM& operator<< ( OSTREAM & os, const AFactor<T> & f );
#endif /* NOSTREAMIO */

#endif /* ! INCL_AFACTOR_H */
