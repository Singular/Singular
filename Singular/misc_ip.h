/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file misc_ip.h
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

#include <kernel/si_gmp.h>
#include <kernel/structs.h>

// include basic SINGULAR structures
/* So far nothing is required. */

/**
 * Converts a non-negative bigint number into a GMP number.
 *
 **/
void number2mpz(number n,   /**< [in]  a bigint number >= 0  */
                mpz_t m     /**< [out] the GMP equivalent    */
               );
               
/**
 * Converts a non-negative GMP number into a bigint number.
 *
 * @return the bigint number representing the given GMP number
 **/
number mpz2number(mpz_t m   /**< [in]  a GMP number >= 0  */
                 );

/**
 * Divides 'n' as many times as possible by 'd' and returns the number
 * of divisions (without remainder) in 'times',
 * e.g., n = 48, d = 4, divTimes(n, d, t) = 3 produces n = 3, t = 2,
 *       since 48 = 4*4*3;
 * assumes that d is positive
 **/
void divTimes(mpz_t n,   /**< [in]  a GMP number >= 0                      */
              mpz_t d,   /**< [in]  the divisor, a GMP number >= 0         */
              int* times /**< [out] number of divisions without remainder  */
             );

/**
 * Factorises a given positive bigint number n into its prime factors less
 * than or equal to a given bound, with corresponding multiplicities.
 *
 * The method finds all prime factors with multiplicities. If a non-zero
 * bound is given, then only the prime factors <= pBound are being found.
 * In this case, there may remain an unfactored portion m of n.
 * The method returns a list L filled with four entries:
 * L[1] contains the remainder m as int or bigint, depending on the size,
 * L[2] a list; L[2][i] contains the i-th prime factor as int or bigint
 *                     (sorted in ascending order),
 * L[3] a list; L[3][i] contains the multiplicity of L[2, i] in n as int
 * L[4] 1 iff the remainder m is probably a prime, 0 otherwise
 *
 * We thus have: n = L[1] * L[2][1]^L[3][1] * ... * L[2][k]^L[3][k], where
 * k is the number of mutually distinct prime factors (<= a provided non-
 * zero bound).
 *
 * @return the factorisation data in a SINGULAR-internal list
 **/
lists primeFactorisation(
       const number n,     /**< [in]  the bigint > 0 to be factorised   */
       const number pBound /**< [in]  bigint bound on the prime factors
                                      seeked                            */
                        );

#endif
/* MISC_H */
