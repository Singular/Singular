// emacs edit mode for this file is -*- C++ -*-
// $Id: ftmpl_factor.cc,v 1.2 1997-04-15 10:06:53 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.1  1997/03/27 10:30:00  schmidt
stream-io wrapped by NOSTREAMIO

Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

*/

/*MAKEHEADER*/
#include "templates/config.h"

#include <templates/factor.h>


template <class T>
Factor<T>& Factor<T>::operator= ( const Factor<T>& f )
{
    if ( this != &f ) {
	_factor = f._factor;
	_exp = f._exp;
    }
    return *this;
}

template <class T>
Factor<T>& Factor<T>::operator= ( const T & f )
{
    _factor = f;
    _exp = 1;
    return *this;
}

template <class T>
int operator== ( const Factor<T> &f1, const Factor<T> &f2 )
{
    return (f1.exp() == f2.exp()) && (f1.factor() == f2.factor());
}

#ifndef NOSTREAMIO
template <class T>
void Factor<T>::print ( ostream& s ) const
{
    if ( exp() == 1 )
	s << factor();
    else
	s << "(" << factor() << ")^" << exp();
}

template <class T>
ostream& operator<< ( ostream & os, const Factor<T> & f )
{
    f.print( os );
    return os;
}
#endif /* NOSTREAMIO */
