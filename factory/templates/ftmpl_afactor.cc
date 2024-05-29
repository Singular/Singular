/* emacs edit mode for this file is -*- C++ -*- */

#include "factory/templates/ftmpl_afactor.h"

template <class T>
AFactor<T>& AFactor<T>::operator= ( const AFactor<T>& f )
{
    if ( this != &f )
    {
      _minpoly = f._minpoly;
      _factor = f._factor;
      _exp = f._exp;
    }
    return *this;
}

template <class T>
int operator== ( const AFactor<T> &f1, const AFactor<T> &f2 )
{
    return (f1.exp() == f2.exp()) && (f1.factor() == f2.factor()) && (f1.minpoly() == f2.minpoly()); //minpoly comparison may not be enough but checking the fields they define are equal
}

#ifndef NOSTREAMIO
template <class T>
void AFactor<T>::print ( OSTREAM& s ) const
{
    if ( exp() == 1 )
        s << "(" << factor() << ", " << minpoly() << ")";
    else
        s << "((" << factor() << ")^" << exp() << ", " << minpoly() << ")";
}

template <class T>
OSTREAM& operator<< ( OSTREAM & os, const AFactor<T> & f )
{
    f.print( os );
    return os;
}
#endif /* NOSTREAMIO */
