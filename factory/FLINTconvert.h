/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file FLINTconvert.h
 *
 * This file defines functions for conversion to FLINT (www.flintlib.org)
 * and back.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FLINT_CONVERT_H
#define FLINT_CONVERT_H

#include <config.h>
#include "canonicalform.h"

#ifdef HAVE_FLINT
#ifdef __cplusplus
extern "C"
{
#endif
#include <fmpz.h>
#include <fmpq.h>
#include <fmpz_poly.h>
#include <fmpq_poly.h>
#include <nmod_poly.h>
#ifdef __cplusplus
}
#endif

/// conversion of a factory integer to fmpz_t
void
convertCF2Fmpz (fmpz_t result,         ///< [in,out] an fmpz_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping an
                                       ///< integer
               );

/// conversion of a factory univariate polynomial over Z to a fmpz_poly_t
void
convertFacCF2Fmpz_poly_t (fmpz_poly_t result,    ///< [in,out] an fmpz_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Z
                         );

/// conversion of a FLINT integer to CanonicalForm
CanonicalForm
convertFmpz2CF (fmpz_t coefficient ///< [in] a FLINT integer
               );

/// conversion of a FLINT poly over Z to CanonicalForm
CanonicalForm
convertFmpz_poly_t2FacCF (fmpz_poly_t poly, ///< [in] an fmpz_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a factory univariate polynomials over Z/p (for word size p)
/// to nmod_poly_t
void
convertFacCF2nmod_poly_t (nmod_poly_t result,    ///< [in, out] a nmod_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Z/p
                         );

/// conversion of a FLINT poly over Z/p to CanonicalForm
CanonicalForm
convertnmod_poly_t2FacCF (nmod_poly_t poly, ///< [in] a nmod_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a factory rationals to fmpq_t
void
convertCF2Fmpq (fmpq_t result,         ///< [in,out] an fmpq_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping a
                                       ///< rational
               );

/// conversion of a factory univariate polynomials over Q to fmpq_poly_t
void
convertFacCF2Fmpq_poly_t (fmpq_poly_t result,    ///< [in,out] an fmpq_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Q
                         );

/// conversion of a FLINT poly over Q to CanonicalForm
CanonicalForm
convertFmpq_poly_t2FacCF (fmpq_poly_t p,    ///< [in] an fmpq_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a FLINT factorization over Z/p (for word size p) to a
/// CFFList
CFFList
convertFLINTnmod_poly_factor2FacCFFList (
                   nmod_poly_factor_t fac, ///< [in] a nmod_poly_factor_t
                   mp_limb_t leadingCoeff, ///< [in] leading coefficient
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        );

#endif
#endif
