// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_util.cc,v 1.1 1997-04-07 16:14:18 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:45  stobbe
Initial revision

*/

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
