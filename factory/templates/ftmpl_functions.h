// emacs mode for this file is -*- C++ -*-
// $Id: ftmpl_functions.h,v 1.1 1997-04-15 10:09:07 schmidt Exp $

#ifndef INCL_TMPLFUNCTIONS_H
#define INCL_TMPLFUNCTIONS_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 11:06:32  stobbe
Initial revision

*/

/*MAKEHEADER*/
#include "templates/config.h"

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

#endif
