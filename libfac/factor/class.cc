////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: class.cc,v 1.6 2008-06-10 14:49:16 Singular Exp $
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

#ifndef NOSTREAMIO
template <class T>
void Substitution<T>::print ( OSTREAM& s ) const
{
	s << "(" << factor() << ")^" << exp();
}
#endif

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.5  2008/03/18 17:46:15  Singular
*hannes: gcc 4.2

Revision 1.4  2006/05/16 14:58:54  hannes
*hannes: gcc-4.1 fixes

Revision 1.3  1997/09/12 07:19:53  Singular
* hannes/michael: libfac-0.3.0

Revision 1.3  1997/04/25 22:38:04  michael
Version for libfac-0.2.1

*/
