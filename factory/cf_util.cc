// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_util.cc,v 1.0 1996-05-17 10:59:45 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

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
