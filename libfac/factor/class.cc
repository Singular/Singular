/* Copyright 1997 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// static char * rcsid = "@(#) $Id: class.cc,v 1.1.1.1 1997-05-02 17:00:46 Singular Exp $";
////////////////////////////////////////////////////////////

// #pragma implementation

#include "class.h"

template <class T>
Substitution<T>& Substitution<T>::operator= ( const Substitution<T>& f )
{
    if ( this != &f ) {
	_factor = f._factor;
	_exp = f._exp;
    }
    return *this;
}

template <class T>
Substitution<T>& Substitution<T>::operator= ( const T & f )
{
    _factor = f;
    _exp = 1;
    return *this;
}

template <class T>
int operator== ( const Substitution<T> &f1, const Substitution<T> &f2 )
{
    return (f1.exp() == f2.exp()) && (f1.factor() == f2.factor());
}

template <class T>
void Substitution<T>::print ( ostream& s ) const
{
#ifndef NOSTREAMIO
	s << "(" << factor() << ")^" << exp();
#endif	
}

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/04/25 22:38:04  michael
Version for libfac-0.2.1

*/
