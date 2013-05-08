/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_SINGEXT_H
#define INCL_SINGEXT_H

// #include "config.h"

#include "canonicalform.h"
#include <factory/cf_gmp.h>

#include <resources/feFopen.h>

/*BEGINPUBLIC*/

void gmp_numerator ( const CanonicalForm & f, mpz_ptr result );

void gmp_denominator ( const CanonicalForm & f, mpz_ptr result );

int gf_value (const CanonicalForm & f );

CanonicalForm make_cf ( const mpz_ptr n );

CanonicalForm make_cf ( const mpz_ptr n, const mpz_ptr d, bool normalize );

CanonicalForm make_cf_from_gf ( const int z );

/*ENDPUBLIC*/

#endif /* ! INCL_SINGEXT_H */
