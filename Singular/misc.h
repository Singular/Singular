/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file misc.h
 * 
 * This file provides miscellaneous functionality.
 *
 * ABSTRACT: This file provides the following miscellaneous functionality:
 *           - prime factorisation of bigints with prime factors < 2^31
 *            (This will require at most 256 MByte of RAM.)
 *           - approximate square root of a bigint
 *
 *           Most of the functioanlity implemented here had earlier been
 *           coded in SINGULAR in some library. Due to performance reasons
 *           these algorithms have been moved to the C/C++ kernel.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef MISC_H
#define MISC_H

// include basic SINGULAR structures
/* So far nothing is required. */

/**
 * Computes an approximation of the square root of a bigint number.
 *
 * Expects a non-negative bigint number n and returns the bigint
 * number m which satisfies m*m <= n < (m+1)*(m+1). This implementation
 * does not require a valid currRing.
 *
 * @return an approximation of the square root of n
 **/
number approximateSqrt(const number n   /**< [in]  a number   */
                      );

/**
 * Factorises a given positive bigint number n into its prime factors less
 * than or equal to a given bound, with corresponding multiplicities.
 *
 * The method is capable of finding only prime factors < 2^31, and it looks
 * only for those less than or equal the given bound, if this bound is not 0.
 * Therefore, after dividing out all prime factors which have been found,
 * some number m > 1 may survive.
 * The method returns a list L filled with three entries:
 * L[1] contains m as int or bigint, depending on the size,
 * L[2] a list; L[2][i] contains the i-th prime factor as int
 *                     (sorted in ascending order),
 * L[3] a list; L[3][i] contains the multiplicity of L[2, i] in n as int
 *
 * We thus have: n = L[1] * L[2][1]^L[3][1] * ... * L[2][k]^L[3][k], where
 * k is the number of mutually distinct prime factors < 2^31 and <= the
 * provided bound (if this is not zero).
 *
 * @return the factorisation data in a list
 **/
lists primeFactorisation(
       const number n,    /**< [in]  the bigint to be factorised       */
       const int pBound   /**< [in]  bound on the prime factors seeked */
                        );

#endif
/* MISC_H */
