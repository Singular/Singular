/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facHensel.h
 *
 * This file defines functions for Hensel lifting.
 *
 * ABSTRACT: function are used for bi- and multivariate factorization over
 * finite fields. Described in "Efficient Multivariate Factorization over Finite
 * Fields" by L. Bernardin & M. Monagon and "Algorithms for Computer Algebra" by
 * Geddes, Czapor, Labahn
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_HENSEL_H
#define FAC_HENSEL_H

// #include "config.h"
#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "fac_util.h"

/// sort a list of polynomials by their degree in @a x.
///
void sortList (CFList& list,     ///< [in, out] list of polys, sorted list
               const Variable& x ///< [in] some Variable
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
              CFMatrix& M,            ///< [in,out] stores intermediate results
              modpk& b,               ///< [in] coeff bound
              bool sort= true         ///< [in] sort factors by degree in
                                      ///< Variable(1)
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
              CFMatrix& M,            ///< [in,out] stores intermediate results
              bool sort= true        ///< [in] sort factors by degree in
                                      ///< Variable(1)
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
                    CFMatrix& M,            ///< [in,out] stores intermediate
                                            ///< results
                    const modpk& b= modpk() ///< [in] coeff bound
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
              int* l,          ///< [in] l[0]: precision of bivariate
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
            int lOld,       ///< [in] lifting precision of F.mvar()
            int lNew        ///< [in] lifting precision of G.mvar()
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
            int* l,          ///< [in] lifting bounds
            int lLength,     ///< [in] length of l
            bool sort= true        ///< [in] sort factors by degree in
                                   ///< Variable(1)
           );

/// Hensel lifting from univariate to bivariate, factors need not to be monic
void
nonMonicHenselLift12 (const CanonicalForm& F,///< [in] a bivariate poly
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
nonMonicHenselLift2 (const CFList& eval,///< [in] a list of polynomials the last
                                   ///< element is a compressed multivariate
                                   ///< poly, last but one element equals the
                                   ///< last elements modulo its main variable
                                   ///< and so on. The first element is a
                                   ///< compressed bivariate poly.
             const CFList& factors,///< [in] bivariate factors
             int* l,               ///< [in] lift bounds
             int lLength,          ///< [in] length of l
             bool sort,            ///< [in] if true factors are sorted by
                                   ///< their degree in Variable(1)
             const CFList& LCs1,   ///< [in] a list of evaluated LC of first
                                   ///< factor
             const CFList& LCs2,   ///< [in] a list of evaluated LC of second
                                   ///< factor
             const CFArray& Pi,    ///< [in] intermediate result
             const CFList& diophant,///< [in] result of diophantine
             bool& noOneToOne      ///< [in,out] check for one to one
                                   ///< correspondence
            );

/// Hensel lifting of non monic factors, needs correct leading coefficients of
/// factors and a one to one corresponds between bivariate and multivariate
/// factors to succeed
///
/// @return @a nonMonicHenselLift returns a list of lifted factors
/// such that prod (factors) == eval.getLast() if there is a one to one
/// correspondence
CFList
nonMonicHenselLift (const CFList& eval,    ///< [in] a list of polys the last
                                           ///< element is a compressed
                                           ///< multivariate poly, last but one
                                           ///< element equals the last elements
                                           ///< modulo its main variable and so
                                           ///< on. The first element is a
                                           ///< compressed poly in 3 variables
                    const CFList& factors, ///< [in] a list of bivariate factors
                    CFList* const& LCs,    ///< [in] leading coefficients,
                                           ///< evaluated in the same way as
                                           ///< eval
                    CFList& diophant,      ///< [in, out] solution of univariate
                                           ///< diophantine equation
                    CFArray& Pi,           ///< [in, out] buffer intermediate
                                           ///< results
                    int* liftBound,        ///< [in] lifting bounds
                    int length,            ///< [in] length of lifting bounds
                    bool& noOneToOne       ///< [in, out] check for one to one
                                           ///< correspondence
                   );
#endif
/* FAC_HENSEL_H */

