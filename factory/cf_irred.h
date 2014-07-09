/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_irred.h
 *
 * generate random irreducible univariate polynomials
**/

#ifndef INCL_CF_IRRED_H
#define INCL_CF_IRRED_H

// #include "config.h"

#include "canonicalform.h"
#include "cf_random.h"

/** generate a random irreducible polynomial in x of degree deg
 *
 * @warning this is done in the most naive way, i.e. a random is generated and
 * then factorized
**/
CanonicalForm find_irreducible ( int deg, CFRandom & gen, const Variable & x );

CanonicalForm
randomIrredpoly (int i, const Variable & x);

#endif /* ! INCL_CF_IRRED_H */
