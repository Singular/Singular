// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_irred.cc,v 1.1 1997-04-07 15:11:00 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:44  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_random.h"

static bool
is_irreducible ( const CanonicalForm & f )
{
    CFFList F = factorize( f );
    return F.length() == 1 && F.getFirst().exp() == 1;
}

CanonicalForm
find_irreducible ( int deg, CFRandom & gen, const Variable & x )
{
    CanonicalForm result;
    int i;
    do {
	result = power( x, deg );
	for ( i = deg-1; i >= 0; i-- )
	    result += gen.generate() * power( x, i );
    } while ( ! is_irreducible( result ) );
    return result;
}
