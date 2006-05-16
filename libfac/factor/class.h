/* Copyright 1996 Michael Messollen. All rights reserved. */
///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: class.h,v 1.6 2006-05-16 14:46:50 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef INCL_CLASS_H
#define INCL_CLASS_H

// #pragma interface

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#define ISTREAM std::istream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#define ISTREAM istream
#endif
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
#ifndef NOSTREAMIO
    void print ( OSTREAM& ) const;
    friend OSTREAM& operator<< ( OSTREAM & os, const Substitution<T> & f )
    {
	f.print( os );
	return os;
    }
#endif
};
template <class T>
int operator== ( const Substitution<T>&, const Substitution<T>& );

// #ifdef IMPL_CLASS_H
// #include "class.cc"
// #endif

#endif /* INCL_CLASS_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.5  2000/05/29 08:06:47  pohl
no artificial friend

Revision 1.4  1997/09/12 07:19:54  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:21:26  michael
Version for libfac-0.2.1

*/
