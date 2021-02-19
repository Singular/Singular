/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file singext.h
 *
 * helper functions for conversion to and from Singular
**/
#ifndef INCL_SINGEXT_H
#define INCL_SINGEXT_H

// #include "config.h"

#include "canonicalform.h"
#include "factory/cf_gmp.h"

#ifdef SINGULAR
#include "resources/feFopen.h"
#endif

//TODO make these functions members of CanonicalForm?
/*BEGINPUBLIC*/

void FACTORY_PUBLIC gmp_numerator ( const CanonicalForm & f, mpz_ptr result );

void FACTORY_PUBLIC gmp_denominator ( const CanonicalForm & f, mpz_ptr result );

int gf_value (const CanonicalForm & f );

CanonicalForm FACTORY_PUBLIC make_cf ( const mpz_ptr n );

CanonicalForm FACTORY_PUBLIC make_cf ( const mpz_ptr n, const mpz_ptr d, bool normalize );

CanonicalForm make_cf_from_gf ( const int z );

/*ENDPUBLIC*/

#endif /* ! INCL_SINGEXT_H */
