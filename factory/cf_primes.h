// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_primes.h,v 1.1 1997-04-07 16:08:49 schmidt Exp $

#ifndef INCL_CF_PRIMES_H
#define INCL_CF_PRIMES_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:38  stobbe
Initial revision

*/

#include <config.h>

/*BEGINPUBLIC*/

int cf_getPrime( int i );

int cf_getNumPrimes();

int cf_getSmallPrime( int i );

int cf_getNumSmallPrimes();

int cf_getBigPrime( int i );

int cf_getNumBigPrimes();

/*ENDPUBLIC*/

#endif /* INCL_CF_PRIMES_H */
