// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_factor.h,v 1.3 1997-04-30 13:08:25 schmidt Exp $

#ifndef INCL_FACTOR_H
#define INCL_FACTOR_H

/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/15 10:08:06  schmidt
#include <config.h> added
the header config.h will be included be makeheader

Revision 1.1  1997/03/27 10:30:35  schmidt
stream-io wrapped by NOSTREAMIO

Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

*/

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

#endif /* INCL_FACTOR_H */
