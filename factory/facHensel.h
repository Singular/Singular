/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facHensel.h
 *
 * This file defines functions for Hensel lifting and modular multiplication.
 *
 * ABSTRACT: function are used for bi- and multivariate factorization over
 * finite fields
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_HENSEL_H
#define FAC_HENSEL_H

#include <config.h>
#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "cf_iter.h"
#include "ftmpl_functions.h"
#include "algext.h"

CanonicalForm mul (const CanonicalForm& F, const CanonicalForm& G);
CanonicalForm mulMod3 (const CanonicalForm& F, const CanonicalForm& G, const CFList& MOD);
/// multiplication of univariate polys over a finite field using NTL, if we are
/// in GF factory's default multiplication is used.
///
/// @return @a mulNTL returns F*G
static inline
CanonicalForm
mulNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G  ///< [in] a univariate poly
       );

/// mod of univariate polys over a finite field using NTL, if we are
/// in GF factory's default mod is used.
///
/// @return @a modNTL returns F mod G
static inline
CanonicalForm
modNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G  ///< [in] a univariate poly
       );

/// division of univariate polys over a finite field using NTL, if we are
/// in GF factory's default division is used.
///
/// @return @a divNTL returns F/G
static inline
CanonicalForm
divNTL (const CanonicalForm& F, ///< [in] a univariate poly
        const CanonicalForm& G  ///< [in] a univariate poly
       );

/*/// division with remainder of univariate polys over a finite field using NTL,
/// if we are in GF factory's default division with remainder is used.
static inline
void
divremNTL (const CanonicalForm& F, ///< [in] a univariate poly
           const CanonicalForm& G, ///< [in] a univariate poly
           CanonicalForm& Q,       ///< [in,out] dividend
           CanonicalForm& R        ///< [in,out] remainder
          );*/

/// splits @a F into degree (F, x)/m polynomials each of degree less than @a m
/// in @a x.
///
/// @return @a split returns a list of polynomials of degree less than @a m in
///         @a x. If degree (F, x) <= 0, F is returned.
/// @sa divrem32(), divrem21()
static inline
CFList split (const CanonicalForm& F, ///< [in] some poly
              const int m,            ///< [in] some int
              const Variable& x       ///< [in] some Variable
             );

/// division with remainder of @a F by
/// @a G wrt Variable (1) modulo @a M.
///
/// @sa divrem(), divrem21(), divrem2()
static inline
void divrem32 (const CanonicalForm& F, ///< [in] poly, s.t. 3*(degree (G, 1)/2)>
                                       ///< degree (F, 1)
               const CanonicalForm& G, ///< [in] some poly
               CanonicalForm& Q,       ///< [in,out] dividend
               CanonicalForm& R,       ///< [in,out] remainder, degree (R, 1) <
                                       ///< degree (G, 1)
               const CFList& M         ///< [in] only contains powers of
                                       ///< Variables of level higher than 1
              );

/// division with remainder of @a F by
/// @a G wrt Variable (1) modulo @a M.
///
/// @sa divrem(), divrem32(), divrem2()
static inline
void divrem21 (const CanonicalForm& F, ///< [in] poly, s.t. 2*degree (G, 1) >
                                       ///< degree (F, 1)
               const CanonicalForm& G, ///< [in] some poly
               CanonicalForm& Q,       ///< [in,out] dividend
               CanonicalForm& R,       ///< [in,out] remainder, degree (R, 1) <
                                       ///< degree (G, 1)
               const CFList& M         ///< [in] only contains powers of
                                       ///< Variables of level higher than 1
              );

/// division with remainder of @a F by
/// @a G wrt Variable (1) modulo @a M.
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

/// division with remainder of @a F by
/// @a G wrt Variable (1) modulo @a MOD.
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

/// reduce @a F modulo elements in @a M.
///
/// @return @a mod returns @a F modulo @a M
CanonicalForm mod (const CanonicalForm& F, ///< [in] compressed polynomial
                   const CFList& M         ///< [in] list containing only
                                           ///< univariate polynomials
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

/// sort a list of polynomials by their degree in @a x.
///
void sortList (CFList& list,     ///< [in, out] list of polys, sorted list
               const Variable& x ///< [in] some Variable
              );

/// solve the univariate diophantine equation
/// \f$ 1\equiv \sum_{i= 1}^{r} {\delta_{i} F/g_{i}} \f$.
/// Where \f$ F= \prod_{i= 1}^{r} {g_{i}} \f$ and \f$ F \f$ is squarefree
/// the \f$ \delta_{i} \f$ have degree less than the degree of \f$ g_{i} \f$.
///
/// @return @a diophantine returns a list of polynomials \f$ \delta_{i} \f$ as
/// specified above
/// @sa biDiophantine(), multiRecDiophantine()
static inline
CFList diophantine (const CanonicalForm& F, ///< [in] compressed, bivariate
                                            ///< polynomial
                    const CFList& factors   ///< [in] a list of factors, as
                                            ///< specified above, including
                                            ///< LC (F, Variable (1)) as first
                                            ///< element
                   );

/// Hensel lift from univariate to bivariate.
///
/// @sa henselLiftResume12(), henselLift23(), henselLiftResume(), henselLift()
void
henselLift12 (const CanonicalForm& F, ///< [in] compressed, bivariate poly
              CFList& factors,        ///< [in, out] monic univariate factors of
                                      ///< F, including leading coefficient as
                                      ///< first element. Returns monic lifted
                                      ///< factors without the leading
                                      ///< coefficient
              int l,                  ///< [in] lifting precision
              CFArray& Pi,            ///< [in,out] stores intermediate results
              CFList& diophant,       ///< [in,out] result of diophantine()
              CFMatrix& M             ///< [in,out] stores intermediate results
             );

/// resume Hensel lift from univariate to bivariate. Assumes factors are lifted
/// to precision Variable (2)^start and lifts them to precision Variable (2)^end
///
/// @sa henselLift12(), henselLift23(), henselLiftResume(), henselLift()
void
henselLiftResume12 (const CanonicalForm& F, ///< [in] compressed, bivariate poly
                    CFList& factors,        ///< [in,out] monic factors of F,
                                            ///< lifted to precision start,
                                            ///< including leading coefficient
                                            ///< as first element. Returns monic
                                            ///< lifted factors without the
                                            ///< leading coefficient
                    int start,              ///< [in] starting precision
                    int end,                ///< [in] end precision
                    CFArray& Pi,            ///< [in,out] stores intermediate
                                            ///< results
                    const CFList& diophant, ///< [in] result of diophantine
                    CFMatrix& M             ///< [in,out] stores intermediate
                                            ///< results
                   );

/// solves the bivariate polynomial diophantine equation
/// \f$ 1\equiv \sum_{i= 1}^{r} {\delta_{i} F/g_{i}} \ mod\ y^{d} \f$,
/// where \f$ F= \prod_{i= 1}^{r} {g_{i}} \ mod\ y^{d}\f$ and
/// \f$ F \in K[x][y]\f$ is squarefree, the \f$ \delta_{i} \f$ have degree less
/// than the degree of \f$ g_{i} \f$ in x.
///
/// @return @a biDiophantine returns a list of polynomials \f$ \delta_{i} \f$ as
///         specified above
/// @sa diophantine(), multiRecDiophantine()
static inline
CFList
biDiophantine (const CanonicalForm& F, ///< [in] compressed, bivariate poly
               const CFList& factors,  ///< [in] list of monic bivariate factors
                                       ///< including LC (F, Variable (1)) as
                                       ///< first element
               const int d             ///< [in] precision
              );

/// solve the multivariate polynomial diophantine equation
/// \f$ 1\equiv \sum_{i= 1}^{r} {\delta_{i} F/g_{i}} \ mod\ <M,F.mvar()^{d}>\f$,
/// where \f$ F= \prod_{i= 1}^{r} {g_{i}} \ mod\ <M,F.mvar()^{d}>\f$ and
/// \f$ F \in K[x][x_1,\ldots , x_n]\f$ is squarefree, the \f$ \delta_{i} \f$
/// have degree less than the degree of \f$ g_{i} \f$ in x.
///
/// @return @a multiDiophantine returns a list of polynomials \f$ \delta_{i} \f$
///         as specified above
/// @sa diophantine(), biDiophantine()
static inline
CFList
multiRecDiophantine (
               const CanonicalForm& F,   ///< [in] compressed,
                                         ///< multivariate polynomial
               const CFList& factors,    ///< [in] list of monic factors
                                         ///< including LC (F, Variable (1)) as
                                         ///< first element
               const CFList& recResult, ///< [in] result of above equation
                                         ///< modulo M
               const CFList& M,          ///< [in] a list of powers of Variables
                                         ///< of level higher than 1
               const int d               ///< [in] precision
                    );

/// Hensel lifting from bivariate to trivariate.
///
/// @return @a henselLift23 returns a list of polynomials lifted to precision
///          Variable (3)^l[1]
/// @sa henselLift12(), henselLiftResume12(), henselLiftResume(), henselLift()
CFList
henselLift23 (const CFList& eval,    ///< [in] contains compressed, bivariate
                                     ///< as first element and trivariate one as
                                     ///< second element
              const CFList& factors, ///< [in] monic bivariate factors,
                                     ///< including leading coefficient
                                     ///< as first element.
              const int* l,          ///< [in] l[0]: precision of bivariate
                                     ///< lifting, l[1] as above
              CFList& diophant,      ///< [in,out] returns the result of
                                     ///< biDiophantine()
              CFArray& Pi,           ///< [in,out] stores intermediate results
              CFMatrix& M            ///< [in,out] stores intermediate results
             );

/// resume Hensel lifting.
///
/// @sa henselLift12(), henselLiftResume12(), henselLift23(), henselLift()
void
henselLiftResume (
              const CanonicalForm& F, ///< [in] compressed, multivariate poly
              CFList& factors,        ///< [in,out] monic multivariate factors
                                      ///< lifted to precision F.mvar()^start,
                                      ///< including leading coefficient
                                      ///< as first element. Returns factors
                                      ///< lifted to precision F.mvar()^end
              int start,              ///< [in] starting precision
              int end,                ///< [in] end precision
              CFArray& Pi,            ///< [in,out] stores intermediate results
              const CFList& diophant, ///< [in] result of multiRecDiophantine()
              CFMatrix& M,            ///< [in, out] stores intermediate results
              const CFList& MOD       ///< [in] a list of powers of Variables
                                      ///< of level higher than 1
                 );

/// Hensel lifting
///
/// @return @a henselLift returns a list of polynomials lifted to
///          precision F.getLast().mvar()^l_new
/// @sa henselLift12(), henselLiftResume12(), henselLift23(), henselLiftResume()
CFList
henselLift (const CFList& F,       ///< [in] two compressed, multivariate
                                   ///< polys F and G
            const CFList& factors, ///< [in] monic multivariate factors
                                   ///< including leading coefficient
                                   ///< as first element.
            const CFList& MOD,     ///< [in] a list of powers of Variables
                                   ///< of level higher than 1
            CFList& diophant,      ///< [in,out] result of multiRecDiophantine()
            CFArray& Pi,           ///< [in,out] stores intermediate results
            CFMatrix& M,           ///< [in,out] stores intermediate results
            const int lOld,       ///< [in] lifting precision of F.mvar()
            const int lNew        ///< [in] lifting precision of G.mvar()
           );

/// Hensel lifting from bivariate to multivariate
///
/// @return @a henselLift returns a list of lifted factors
/// @sa henselLift12(), henselLiftResume12(), henselLift23(), henselLiftResume()
CFList
henselLift (const CFList& eval,    ///< [in] a list of polynomials the last
                                   ///< element is a compressed multivariate
                                   ///< poly, last but one element equals the
                                   ///< last elements modulo its main variable
                                   ///< and so on. The first element is a
                                   ///< compressed bivariate poly.
            const CFList& factors, ///< [in] bivariate factors, including
                                   ///< leading coefficient
            const int* l,          ///< [in] lifting bounds
            const int lLength     ///< [in] length of l
           );

/// two factor Hensel lifting from univariate to bivariate, factors need not to
/// be monic
void
henselLift122 (const CanonicalForm& F,///< [in] a bivariate poly
               CFList& factors,       ///< [in, out] a list of univariate polys
                                      ///< lifted factors
               int l,                 ///< [in] lift bound
               CFArray& Pi,           ///< [in, out] stores intermediate results
               CFList& diophant,      ///< [in, out] result of diophantine
               CFMatrix& M,           ///< [in, out] stores intermediate results
               const CFArray& LCs,    ///< [in] leading coefficients
               bool sort              ///< [in] if true factors are sorted by
                                      ///< their degree
              );

/// two factor Hensel lifting from bivariate to multivariate, factors need not
/// to be monic
///
/// @return @a henselLift122 returns a list of lifted factors
CFList
henselLift2 (const CFList& eval,   ///< [in] a list of polynomials the last
                                   ///< element is a compressed multivariate
                                   ///< poly, last but one element equals the
                                   ///< last elements modulo its main variable
                                   ///< and so on. The first element is a
                                   ///< compressed bivariate poly.
             const CFList& factors,///< [in] bivariate factors
             int* l,               ///< [in] lift bounds
             const int lLength,    ///< [in] length of l
             bool sort,            ///< [in] if true factors are sorted by
                                   ///< their degree in Variable(1)
             const CFList& LCs1,   ///< [in] a list of evaluated LC of first
                                   ///< factor
             const CFList& LCs2,   ///< [in] a list of evaluated LC of second
                                   ///< factor
             const CFArray& Pi,    ///< [in] intermediate result
             const CFList& diophant///< [in] result of diophantine
            );

#endif
/* FAC_HENSEL_H */

