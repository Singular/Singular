// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_primes.cc,v 1.4 1997-04-07 16:08:15 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.3  1996/06/18 12:14:37  stobbe
"smallprimes and bigprimes are now stored in cf_primetab.h.
minor changes in all functions.
"

Revision 1.2  1996/06/18 06:53:42  stobbe
"Now stores more prime numbers.
"

Revision 1.1  1996/06/03 08:34:35  stobbe
"Now stores the first 3500 primes counted from 3.
cf_getBigPrime: gives primes in reversed order.
"

Revision 1.0  1996/05/17 10:59:44  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_globals.h"
#include "cf_primes.h"
#include "cf_primetab.h"


int cf_getPrime( int i )
{
    ASSERT( i >= 0 && i < NUMPRIMES, "index to primes too high" );
    if ( i >= NUMSMALLPRIMES )
	return bigprimes[i-NUMSMALLPRIMES];
    else
	return smallprimes[i];
}

int cf_getNumPrimes()
{
    return NUMPRIMES;
}

int cf_getSmallPrime( int i )
{
    ASSERT( i >= 0 && i < NUMSMALLPRIMES, "index to primes too high" );
    return smallprimes[i];
}

int cf_getNumSmallPrimes()
{
    return NUMSMALLPRIMES;
}

int cf_getBigPrime( int i )
{
    ASSERT( i >= 0 && i < NUMBIGPRIMES, "index to primes too high" );
    return bigprimes[i];
}

int cf_getNumBigPrimes()
{
    return NUMBIGPRIMES;
}
