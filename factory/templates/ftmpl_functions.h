/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_FUNCTIONS_H
#define INCL_FUNCTIONS_H

//{{{ docu
//
// ftmpl_functions.h - some useful template functions.
//
// Header file corresponds to: nothing
//
// Hierarchy: bottom, templates
//
// Developer note:
// ---------------
// Sooner or later you need them: functions to calculate the
// minimum or maximum of two values or the absolute value.  Here
// they are.  All of them are inlined, hence there is no source
// file corresponding to `ftmpl_functions.h'.
//
// The functions are for internal use only (i.e., to build the
// library), hence they should not be included from `factory.h'.
// However, we have to install `ftmpl_functions.h' with the other
// templates since the functions have to be instantiated.
//
//}}}

#include <factory/factoryconf.h>

//{{{ template <class T> inline T tmax ( const T & a, const T & b )
//{{{ docu
//
// tmax() - return the maximum of `a' and `b'.
//
// Developers note:
// ----------------
// `T' should have an `operator >()'.
//
//}}}
template <class T>
inline T tmax ( const T & a, const T & b )
{
    return (a > b) ? a : b;
}
//}}}

//{{{ template <class T> inline T tmin ( const T & a, const T & b )
//{{{ docu
//
// tmin() - return the minimum of `a' and `b'.
//
// Developers note:
// ----------------
// `T' should have an `operator <()'.
//
//}}}
template <class T>
inline T tmin ( const T & a, const T & b )
{
    return (a < b) ? a : b;
}
//}}}

//{{{ template <class T> inline T tabs ( const T & a )
//{{{ docu
//
// tabs() - return the absolute value of `a'.
//
// `a' is negated iff it is less or equal `T( 0 )'.
//
// Developers note:
// ----------------
// `T' should have an `operator >()', an `operator -()', and a
// `T::T( int )' constructor.
//
//}}}
template <class T>
inline T tabs ( const T & a )
{
    return (a > T( 0 )) ? a : -a;
}
//}}}

#endif /* ! INCL_FUNCTIONS_H */
