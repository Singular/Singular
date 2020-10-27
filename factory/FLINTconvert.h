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

// #include "config.h"
#include "canonicalform.h"
#include "fac_util.h"

#ifdef HAVE_FLINT
#ifdef __cplusplus
extern "C"
{
#endif
#ifndef __GMP_BITS_PER_MP_LIMB
#define __GMP_BITS_PER_MP_LIMB GMP_LIMB_BITS
#endif
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_mod_poly.h>
#include <flint/fmpq_poly.h>
#include <flint/nmod_poly.h>
#include <flint/nmod_mat.h>
#include <flint/fmpz_mat.h>
#if ( __FLINT_RELEASE >= 20400)
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_nmod.h>
#include <flint/fq_nmod_poly.h>
#include <flint/fq_nmod_mat.h>
#if ( __FLINT_RELEASE >= 20503)
#include <flint/fmpq_mpoly.h>
#endif
#endif

EXTERN_VAR flint_rand_t FLINTrandom;

#ifdef __cplusplus
}
#endif

#include "factory/cf_gmp.h"

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
convertFmpz2CF (const fmpz_t coefficient ///< [in] a FLINT integer
               );

/// conversion of a FLINT poly over Z to CanonicalForm
CanonicalForm
convertFmpz_poly_t2FacCF (const fmpz_poly_t poly, ///< [in] an fmpz_poly_t
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
convertnmod_poly_t2FacCF (const nmod_poly_t poly, ///< [in] a nmod_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a factory rationals to fmpq_t
void
convertCF2Fmpq (fmpq_t result,         ///< [in,out] an fmpq_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping a
                                       ///< rational
               );

/// conversion of a FLINT rational to CanonicalForm
CanonicalForm convertFmpq2CF (const fmpq_t q);

/// conversion of a factory univariate polynomials over Q to fmpq_poly_t
void
convertFacCF2Fmpq_poly_t (fmpq_poly_t result,    ///< [in,out] an fmpq_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Q
                         );

/// conversion of a FLINT poly over Q to CanonicalForm
CanonicalForm
convertFmpq_poly_t2FacCF (const fmpq_poly_t p,    ///< [in] an fmpq_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a FLINT factorization over Z/p (for word size p) to a
/// CFFList
CFFList
convertFLINTnmod_poly_factor2FacCFFList (
                   const nmod_poly_factor_t fac, ///< [in] a nmod_poly_factor_t
                   const mp_limb_t leadingCoeff, ///< [in] leading coefficient
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        );

/// conversion of a FLINT factorization over Z to a CFFList
CFFList
convertFLINTfmpz_poly_factor2FacCFFList (
                   const fmpz_poly_factor_t fac, ///< [in] a fmpz_poly_factor_t
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        );

/// conversion of a factory univariate poly over Z to a FLINT poly over
/// Z/p (for non word size p)
void
convertFacCF2Fmpz_mod_poly_t (
                          fmpz_mod_poly_t result, ///< [in,out] fmpz_mod_poly_t
                          const CanonicalForm& f, ///< [in] univariate poly over
                                                  ///< Z
                          const fmpz_t p          ///< [in] some integer p
                             );

/// conversion of a FLINT poly over Z/p (for non word size p) to a CanonicalForm
/// over Z
CanonicalForm
convertFmpz_mod_poly_t2FacCF (
                          const fmpz_mod_poly_t poly, ///< [in] fmpz_mod_poly_t
                          const Variable& x,    ///< [in] variable the result
                                                ///< should have
                          const modpk& b        ///< [in] coeff bound to map
                                                ///< coeffs in (-p/2,p/2)
                             );

#if __FLINT_RELEASE >= 20400
/// conversion of a FLINT element of F_q to a CanonicalForm with alg. variable
/// alpha
CanonicalForm
convertFq_nmod_t2FacCF (const fq_nmod_t poly, ///< [in] fq_nmod_t
                        const Variable& alpha, ///< [in] algebraic variable
			const fq_nmod_ctx_t ctx ///<[in] context
                       );

/// conversion of a FLINT element of F_q with non-word size p to a CanonicalForm
/// with alg. variable alpha
CanonicalForm
convertFq_t2FacCF (const fq_t poly,      ///< [in] fq_t
                   const Variable& alpha ///< [in] algebraic variable
                  );

/// conversion of a factory element of F_q to a FLINT fq_nmod_t, does not do any
/// memory allocation for poly
void
convertFacCF2Fq_nmod_t (fq_nmod_t result,       ///< [in,out] fq_nmod_t
                        const CanonicalForm& f, ///< [in] element of Fq
                        const fq_nmod_ctx_t ctx ///< [in] Fq context
                       );

/// conversion of a factory element of F_q (for non-word size p) to a FLINT fq_t
void
convertFacCF2Fq_t (fq_t result,            ///< [in,out] fq_t
                   const CanonicalForm& f, ///< [in] element of Fq
                   const fq_ctx_t ctx      ///< [in] Fq context
                  );

/// conversion of a factory univariate poly over F_q (for non-word size p) to a
/// FLINT fq_poly_t
void
convertFacCF2Fq_poly_t (fq_poly_t result,      ///< [in,out] fq_poly_t
                        const CanonicalForm& f,///< [in] univariate poly over Fq
                        const fq_ctx_t ctx     ///< [in] Fq context
                       );

/// conversion of a factory univariate poly over F_q to a FLINT fq_nmod_poly_t
void
convertFacCF2Fq_nmod_poly_t (fq_nmod_poly_t result, ///< [in,out] fq_nmod_poly_t
                             const CanonicalForm& f,///< [in] univariate poly
                                                    ///< over Fq
                             const fq_nmod_ctx_t ctx///< [in] Fq context
                            );

/// conversion of a FLINT poly over Fq (for non-word size p) to a CanonicalForm
/// with alg. variable alpha and polynomial variable x
CanonicalForm
convertFq_poly_t2FacCF (const fq_poly_t p,     ///< [in] fq_poly_t
                        const Variable& x,     ///< [in] polynomial variable
                        const Variable& alpha, ///< [in] algebraic variable
                        const fq_ctx_t ctx     ///< [in] Fq context
                       );

/// conversion of a FLINT poly over Fq to a CanonicalForm with alg. variable
/// alpha and polynomial variable x
CanonicalForm
convertFq_nmod_poly_t2FacCF (const fq_nmod_poly_t p, ///< [in] fq_nmod_poly_t
                             const Variable& x,      ///< [in] polynomial var.
                             const Variable& alpha,  ///< [in] algebraic var.
                             const fq_nmod_ctx_t ctx ///< [in] Fq context
                            );
#endif

/// conversion of a factory matrix over Z to a fmpz_mat_t
void convertFacCFMatrix2Fmpz_mat_t (fmpz_mat_t M,      ///<[in,out] fmpz_mat_t
                                    const CFMatrix &m  ///<[in] matrix over Z
                                   );

/// conversion of a FLINT matrix over Z to a factory matrix
CFMatrix* convertFmpz_mat_t2FacCFMatrix(const fmpz_mat_t m ///<[in] fmpz_mat_t
                                       );

/// conversion of a factory matrix over Z/p to a nmod_mat_t
void convertFacCFMatrix2nmod_mat_t (nmod_mat_t M,     ///<[in,out] nmod_mat_t
                                    const CFMatrix &m ///<[in] matrix over Z/p
                                   );

/// conversion of a FLINT matrix over Z/p to a factory matrix
CFMatrix* convertNmod_mat_t2FacCFMatrix(const nmod_mat_t m ///<[in] nmod_mat_t
                                       );

#if __FLINT_RELEASE >= 20400
/// conversion of a FLINT matrix over F_q to a factory matrix
CFMatrix*
convertFq_nmod_mat_t2FacCFMatrix(const fq_nmod_mat_t m,       ///< [in] fq_nmod_mat_t
                                 const fq_nmod_ctx_t& fq_con, ///< [in] Fq context
                                 const Variable& alpha ///< [in] algebraic variable
                                );

/// conversion of a factory matrix over F_q to a fq_nmod_mat_t
void
convertFacCFMatrix2Fq_nmod_mat_t (fq_nmod_mat_t M,            ///< [in, out] fq_nmod_mat_t
                                  const fq_nmod_ctx_t fq_con, ///< [in] Fq context
                                  const CFMatrix &m           ///< [in] matrix over Fq
                                 );

/// conversion of a FLINT factorization over Fq (for word size p) to a
/// CFFList
CFFList
convertFLINTFq_nmod_poly_factor2FacCFFList (const fq_nmod_poly_factor_t fac, ///< [in] fq_nmod_poly_factor_t
                                          const Variable& x,     ///< [in] polynomial variable
                                          const Variable& alpha, ///< [in] algebraic variable
                                          const fq_nmod_ctx_t fq_con ///< [in] Fq context
                                           );
#endif


#if __FLINT_RELEASE >= 20503
CanonicalForm mulFlintMP_Zp(const CanonicalForm& F,int lF, const CanonicalForm& Gi, int lG, int m);
CanonicalForm mulFlintMP_QQ(const CanonicalForm& F,int lF, const CanonicalForm& Gi, int lG, int m);
CanonicalForm gcdFlintMP_Zp(const CanonicalForm& F, const CanonicalForm& G);
CanonicalForm gcdFlintMP_QQ(const CanonicalForm& F, const CanonicalForm& G);
void convFactoryPFlintMP ( const CanonicalForm & f, nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, int N );
void convFactoryPFlintMP ( const CanonicalForm & f, fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, int N );
void convFactoryPFlintMP ( const CanonicalForm & f, fmpz_mpoly_t res, fmpz_mpoly_ctx_t ctx, int N );
CanonicalForm convFlintMPFactoryP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, int N);
CanonicalForm convFlintMPFactoryP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, int N);
CanonicalForm convFlintMPFactoryP(fmpz_mpoly_t f, fmpz_mpoly_ctx_t ctx, int N);
#endif
#endif
#endif
