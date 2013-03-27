/* emacs edit mode for this file is -*- C++ -*- */

#include <factory/templates/ftmpl_afactor.h>

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

#ifndef NOSTREAMIO
template <class T>
void AFactor<T>::print ( OSTREAM& s ) const
{
    if ( exp() == 1 )
	s << "(" << factor(), ", " << minpoly() << ")";
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
