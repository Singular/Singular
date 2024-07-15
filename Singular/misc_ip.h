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
 *
 **/
/*****************************************************************************/

#ifndef MISC_H
#define MISC_H

#include "kernel/mod2.h"

#include "coeffs/si_gmp.h"
#include "coeffs/coeffs.h"

#include "Singular/lists.h"

/**
 * Factorises a given bigint number n into its prime factors less
 * than or equal to a given bound, with corresponding multiplicities.
 *
 * The method finds all prime factors with multiplicities. If a positive
 * bound is given, then only the prime factors <= pBound are being found.
 * In this case, there may remain an unfactored portion m of n.
 * Also, when n is negative, m will contain the sign. If n is zero, m will
 * be zero.
 * The method returns a list L filled with three entries:
 * L[1] a list; L[1][i] contains the i-th prime factor of |n| as int or
 *                      bigint (sorted in ascending order),
 * L[2] a list; L[2][i] contains the multiplicity of L[1, i] in |n| as int
 * L[3] contains the remainder m as int or bigint, depending on the size,
 *
 * We thus have: n = L[1][1]^L[2][1] * ... * L[1][k]^L[2][k] * L[3], where
 * k is the number of mutually distinct prime factors (<= a provided non-
 * zero bound).
 * Note that for n = 0, L[1] and L[2] will be empty lists and L[3] will be
 * zero.
 *
 * @return the factorisation data in a SINGULAR-internal list
 **/
lists primeFactorisation(
       const number n,     /**< [in]  the bigint > 0 to be factorised   */
       const int pBound    /**< [in]  bound on the prime factors
                                      seeked                            */
                        );



#ifdef PDEBUG
#if (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)
// #include "kernel/polys.h"
/* Needed for debug Version of p_SetRingOfLeftv, Oliver */
#include "kernel/structs.h"
void p_SetRingOfLeftv(leftv l, ring r);
#endif
#endif

#endif
/* MISC_H */
