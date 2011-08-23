/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_SINGEXT_H
#define INCL_SINGEXT_H

// #include "config.h"

#include "canonicalform.h"
#include <factory/cf_gmp.h>

/*BEGINPUBLIC*/

void gmp_numerator ( const CanonicalForm & f, mpz_ptr result );

void gmp_denominator ( const CanonicalForm & f, mpz_ptr result );

int gf_value (const CanonicalForm & f );

CanonicalForm make_cf ( const MP_INT & n );

CanonicalForm make_cf ( const MP_INT & n, const MP_INT & d, bool normalize );

CanonicalForm make_cf_from_gf ( const int z );

/*ENDPUBLIC*/

#endif /* ! INCL_SINGEXT_H */
