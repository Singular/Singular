// -*- c++ -*-
//*****************************************************************************
/** @file cf_cyclo.h
 *
 * Compute cyclotomic polynomials and factorize integers by brute force
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
**/
//*****************************************************************************

#ifndef CF_CYCLO_H
#define CF_CYCLO_H

// #include "config.h"

/// integer factorization using table look-ups,
/// function may fail if integer contains primes which exceed the largest prime
/// in our table
int* integerFactorizer (const long integer, ///< [in] some integer
                        int& length,        ///< [in,out] number of factors
                        bool& fail          ///< [in,out] failure?
                       );

/// compute the n-th cyclotomic polynomial,
/// function may fail if integer_factorizer fails to factorize n
CanonicalForm
cyclotomicPoly (int n,     ///< [in] some integer
                bool& fail ///< [in,out] failure?
               );

/// checks if alpha is a primitive element, alpha is assumed to be an algebraic
/// variable over some finite prime field
bool isPrimitive (const Variable& alpha, ///< [in] some algebraic variable
                  bool& fail             ///< [in,out] failure?
                 );

#endif

