/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_functions.h,v 1.3 1997-06-19 13:11:16 schmidt Exp $ */

#ifndef INCL_FUNCTIONS_H
#define INCL_FUNCTIONS_H

#include <factoryconf.h>

template<class T>
inline T tmax( const T & a, const T & b )
{
  return (a > b) ? a : b;
}

template<class T>
inline T tmin( const T & a, const T & b )
{
  return (a < b) ? a : b;
}

#endif /* ! INCL_FUNCTIONS_H */
