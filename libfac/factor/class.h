/* Copyright 1996 Michael Messollen. All rights reserved. */
///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: class.h,v 1.3 1997-08-01 10:51:28 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef INCL_CLASS_H
#define INCL_CLASS_H

// #pragma interface

#ifndef NOSTREAMIO
#include <iostream.h>
#endif


template <class T>
class Substitution {
private:
    T _factor;
    T _exp;
public:
    Substitution() : _factor(1), _exp(0) {}
    Substitution( const Substitution<T> & f ) : _factor(f._factor), _exp(f._exp) {}
    Substitution( const T & f, const T & e ) : _factor(f), _exp(e) {}
    Substitution( const T & f ) : _factor(f), _exp(1) {}
    ~Substitution() {}
    Substitution<T>& operator= ( const Substitution<T>& );
    Substitution<T>& operator= ( const T& );
    T factor() const { return _factor; }
    T exp() const { return _exp; }
//     T value() const { return power( _factor, _exp ); }
//     Factor<T>& operator+= ( int i ) { _exp += i; return *this; }
//     Factor<T>& operator*= ( int i ) { _exp *= i; return *this; }
//     Substitution<T>& operator*= ( const T & f ) { _factor *= f; _exp *= f; return *this; }
    friend int operator== ( const Substitution<T>&, const Substitution<T>& );
#ifndef NOSTREAMIO
    void print ( ostream& ) const;
    friend ostream& operator<< ( ostream & os, const Substitution<T> & f )
    {
	f.print( os );
	return os;
    }
#endif    
};

// #ifdef IMPL_CLASS_H
// #include "class.cc"
// #endif

#endif /* INCL_CLASS_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/06/09 15:56:06  Singular
* hannes/michael: libfac 0.2.4

Revision 1.2  1997/04/25 22:21:26  michael
Version for libfac-0.2.1

*/
