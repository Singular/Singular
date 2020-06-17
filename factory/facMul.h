/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facMul.h
 *
 * This file defines functions for fast multiplication and division with
 * remainder
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_MUL_H
#define FAC_MUL_H

#include "canonicalform.h"
#include "fac_util.h"

/// multiplication of univariate polys using FLINT/NTL over F_p, F_q, Z/p^k,
/// Z/p^k[t]/(f), Z, Q, Q(a), if we are in GF factory's default multiplication
/// is used. If @a b!= 0 and getCharacteristic() == 0 the input will be
/// considered as elements over Z/p^k or Z/p^k[t]/(f).
///
/// @return @a mulNTL returns F*G
CanonicalForm
mulNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G, ///< [in] a univariate poly
        const modpk& b= modpk() ///< [in] coeff bound
       );

/// mod of univariate polys using FLINT/NTL over F_p, F_q, Z/p^k,
/// Z/p^k[t]/(f), Z, Q, Q(a), if we are in GF factory's default multiplication
/// is used. If @a b!= 0 and getCharacteristic() == 0 the input will be
/// considered as elements over Z/p^k or Z/p^k[t]/(f); in this case invertiblity
/// of Lc(G) is not checked
///
/// @return @a modNTL returns F mod G
CanonicalForm
modNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G, ///< [in] a univariate poly
        const modpk& b= modpk() ///< [in] coeff bound
       );

/// division of univariate polys using FLINT/NTL over F_p, F_q, Z/p^k,
/// Z/p^k[t]/(f), Z, Q, Q(a), if we are in GF factory's default multiplication
/// is used. If @a b!= 0 and getCharacteristic() == 0 the input will be
/// considered as elements over Z/p^k or Z/p^k[t]/(f); in this case invertiblity
/// of Lc(G) is not checked
///
/// @return @a divNTL returns F/G
CanonicalForm
divNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G, ///< [in] a univariate poly
        const modpk& b= modpk() ///< [in] coeff bound
       );

/// division with remainder of @a F by @a G wrt Variable (1) modulo @a M.
/// Uses an algorithm based on Burnikel, Ziegler "Fast recursive division".
///
/// @return @a Q returns the dividend, @a R returns the remainder.
/// @sa divrem()
void divrem2 (const CanonicalForm& F, ///< [in] bivariate, compressed polynomial
              const CanonicalForm& G, ///< [in] bivariate, compressed polynomial
              CanonicalForm& Q,       ///< [in,out] dividend
              CanonicalForm& R,       ///< [in,out] remainder, degree (R, 1) <
                                      ///< degree (G, 1)
              const CanonicalForm& M  ///< [in] power of Variable (2)
             );

/// division with remainder of @a F by @a G wrt Variable (1) modulo @a MOD.
/// Uses an algorithm based on Burnikel, Ziegler "Fast recursive division".
///
/// @sa divrem2()
void divrem (
           const CanonicalForm& F, ///< [in] multivariate, compressed polynomial
           const CanonicalForm& G, ///< [in] multivariate, compressed polynomial
           CanonicalForm& Q,       ///< [in,out] dividend
           CanonicalForm& R,       ///< [in,out] remainder, degree (R, 1) <
                                   ///< degree (G, 1)
           const CFList& MOD       ///< [in] only contains powers of
                                   ///< Variables of level higher than 1
            );


/// division with remainder of @a F by
/// @a G wrt Variable (1) modulo @a M using Newton inversion
///
/// @return @a Q returns the dividend, @a R returns the remainder.
/// @sa divrem2(), newtonDiv()
void
newtonDivrem (const CanonicalForm& F, ///< [in] bivariate, compressed polynomial
              const CanonicalForm& G, ///< [in] bivariate, compressed polynomial
                                      ///< which is monic in Variable (1)
              CanonicalForm& Q,       ///< [in,out] dividend
              CanonicalForm& R,       ///< [in,out] remainder, degree (R, 1) <
                                      ///< degree (G, 1)
              const CanonicalForm& M  ///< [in] power of Variable (2)
             );

/// division of @a F by
/// @a G wrt Variable (1) modulo @a M using Newton inversion
///
/// @return @a newtonDiv returns the dividend
/// @sa divrem2(), newtonDivrem()
CanonicalForm
newtonDiv (const CanonicalForm& F, ///< [in] bivariate, compressed polynomial
           const CanonicalForm& G, ///< [in] bivariate, compressed polynomial
                                   ///< which is monic in Variable (1)
           const CanonicalForm& M  ///< [in] power of Variable (2)
          );

/// divisibility test for univariate polys
///
/// @return @a uniFdivides returns true if A divides B
bool
uniFdivides (const CanonicalForm& A, ///< [in] univariate poly
             const CanonicalForm& B  ///< [in] univariate poly
            );

/// Karatsuba style modular multiplication for bivariate polynomials.
///
/// @return @a mulMod2 returns @a A * @a B mod @a M.
CanonicalForm
mulMod2 (const CanonicalForm& A, ///< [in] bivariate, compressed polynomial
         const CanonicalForm& B, ///< [in] bivariate, compressed polynomial
         const CanonicalForm& M  ///< [in] power of Variable (2)
        );

/// Karatsuba style modular multiplication for multivariate polynomials.
///
/// @return @a mulMod2 returns @a A * @a B mod @a MOD.
CanonicalForm
mulMod (const CanonicalForm& A, ///< [in] multivariate, compressed polynomial
        const CanonicalForm& B, ///< [in] multivariate, compressed polynomial
        const CFList& MOD       ///< [in] only contains powers of
                                ///< Variables of level higher than 1
       );

/// reduce @a F modulo elements in @a M.
///
/// @return @a mod returns @a F modulo @a M
CanonicalForm mod (const CanonicalForm& F, ///< [in] compressed polynomial
                   const CFList& M         ///< [in] list containing only
                                           ///< univariate polynomials
                  );

/// product of all elements in @a L modulo @a M via divide-and-conquer.
///
/// @return @a prodMod returns product of all elements in @a L modulo @a M.
CanonicalForm
prodMod (const CFList& L,       ///< [in] contains only bivariate, compressed
                                ///< polynomials
         const CanonicalForm& M ///< [in] power of Variable (2)
        );

/// product of all elements in @a L modulo @a M via divide-and-conquer.
///
/// @return @a prodMod returns product of all elements in @a L modulo @a M.
CanonicalForm
prodMod (const CFList& L, ///< [in] contains multivariate, compressed
                          ///< polynomials
         const CFList& M  ///< [in] contains only powers of Variables
        );

#ifdef HAVE_FLINT
/// division with remainder of univariate polynomials over Q and Q(a) using
/// Newton inversion, satisfying F=G*Q+R, deg(R) < deg(G)
void
newtonDivrem (const CanonicalForm& F, ///<[in] univariate poly
              const CanonicalForm& G, ///<[in] univariate poly
              CanonicalForm& Q,       ///<[in, out] quotient
              CanonicalForm& R        ///<[in, out] remainder
             );
#endif

#endif
/* FAC_MUL_H */

