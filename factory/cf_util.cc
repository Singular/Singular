/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_util.cc,v 1.4 1997-07-22 12:10:51 schmidt Exp $ */

//{{{ docu
//
// cf_util.cc - miscellaneous functions, not necessarily related
//   to canonical forms.
//
// Used by: fac_cantzass.cc, gfops.cc
//
//}}}

#include <config.h>

#include "cf_util.h"

//{{{ int ipower ( int b, int m )
//{{{ docu
//
// ipower() - calculate b^m in standard integer arithmetic.
//
// Note: Beware of overflows.
//
//}}}
int
ipower ( int b, int m )
{
    int prod = 1;

    while ( m != 0 ) {
	if ( m % 2 != 0 )
	    prod *= b;
	m /= 2;
	if ( m != 0 )
	    b *= b;
    }
    return prod;
}
//}}}
