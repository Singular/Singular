/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_primes.h
 *
 * access to prime tables
**/

#ifndef INCL_CF_PRIMES_H
#define INCL_CF_PRIMES_H

#include "globaldefs.h"

/*BEGINPUBLIC*/

int FACTORY_PUBLIC cf_getPrime( int i );

int FACTORY_PUBLIC cf_getNumPrimes();

int FACTORY_PUBLIC cf_getSmallPrime( int i );

int FACTORY_PUBLIC cf_getNumSmallPrimes();

int FACTORY_PUBLIC cf_getBigPrime( int i );

int FACTORY_PUBLIC cf_getNumBigPrimes();

/*ENDPUBLIC*/

#endif /* ! INCL_CF_PRIMES_H */
