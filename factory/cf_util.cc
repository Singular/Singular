/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_util.cc,v 1.2 1997-06-19 12:23:52 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_util.h"


int ipower ( int b, int m )
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
