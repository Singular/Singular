/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqBivarUtil.h
 *
 * This file provides utility functions for bivariate factorization
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_BIVAR_UTIL_H
#define FAC_FQ_BIVAR_UTIL_H

// #include "config.h"

#include "cf_map.h"
#include "ExtensionInfo.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

/// append @a factors2 on @a factors1
void append (CFList& factors1,       ///< [in,out] a list of polys
             const CFList& factors2  ///< [in] a list of polys
            );

/// decompress a list of polys @a factors using the map @a N
void decompress (CFList& factors, ///< [in,out] a list of polys
                 const CFMap& N   ///< [in] a map
                );

/// as above
void decompress (CFFList& factors, ///< [in,out] a list of factors
                 const CFMap& N    ///< [in] a map
                );

/// first swap Variables in @a factors1 if necessary, then append @a factors2
/// and @a factors3 on @a factors1 and finally decompress @a factors1
void appendSwapDecompress (CFList& factors1,       ///< [in,out] a list of polys
                           const CFList& factors2, ///< [in] a list of polys
                           const CFList& factors3, ///< [in] a list of polys
                           const bool swap1,       ///< [in] indicates the need
                                                   ///< to swap
                           const bool swap2,       ///< [in] indicates the need
                                                   ///< to swap
                           const CFMap& N          ///< [in] a map
                          );

/// swap Variables in @a factors, then decompress @a factors
void swapDecompress (CFList& factors, ///< [in,out] a list of polys
                     const bool swap, ///< [in] indicates the need to swap
                     const CFMap& N   ///< [in] a map
                    );

/// tests if F is not contained in a subfield defined by @a gamma (Fq case) or
/// @a k (GF case)
///
/// @return @a isInExtension returns true if @a F is not contained in a subfield
///         defined by @a gamma (Fq case) or @a k (GF case), false else
/// @sa appendTestMapDown()
bool isInExtension (const CanonicalForm& F,      ///< [in] a poly over
                                                 ///< F_p (alpha)=Fq or GF(p^l)
                    const CanonicalForm& gamma,  ///< [in] a primitive element
                                                 ///< defining a subfield of
                                                 ///< Fq if we are not over some
                                                 ///< GF
                    const int k,                 ///< some int k such that k 
                                                 ///< divides l if we are not
                                                 ///< over some Fq
                    const CanonicalForm& delta,  ///< [in] image of gamma
                    CFList& source,              ///< [in,out] list of preimages
                    CFList& dest                 ///< [in,out] list of images
                   );

/// map a poly into a subfield of the current field, no testing is performed!
///
/// @return @a mapDown returns @a F mapped into a subfield of the current field
/// @sa appendTestMapDown(), appendMapDown()
CanonicalForm
mapDown (const CanonicalForm& F,    ///< [in] a poly
         const ExtensionInfo& info, ///< [in] information about the sub- and
                                    ///< current field
         CFList& source,            ///< [in,out] in case we are over some
                                    ///< F_p (alpha) and want to map down into
                                    ///< F_p (beta) source contains powers of
                                    ///< the primitive element of F_p (alpha)
         CFList& dest               ///< [in,out] as source but contains
                                    ///< the corresponding powers of the
                                    ///< primitive element of F_p (beta)
        );

/// test if @a g is in a subfield of the current field, if so map it down and
/// append it to @a factors
///
/// @sa mapDown(), isInExtension()
void appendTestMapDown (CFList& factors,          ///< [in,out] a list of polys
                        const CanonicalForm& f,   ///< [in] a poly
                        const ExtensionInfo& info,///< [in] information about
                                                  ///< extension
                        CFList& source,           ///< [in,out] @sa mapDown()
                        CFList& dest              ///< [in,out] @sa mapDown()
                       );

/// map @a g down into a subfield of the current field and append it to @a
/// factors
///
/// @sa mapDown(), appendTestMapDown()
void
appendMapDown (CFList& factors,          ///< [in,out] a list of polys
               const CanonicalForm& g,   ///< [in] a poly
               const ExtensionInfo& info,///< [in] information about extension
               CFList& source,           ///< [in,out] @sa mapDown()
               CFList& dest              ///< [in,out] @sa mapDown()
              );

/// normalize factors, i.e. make factors monic
void normalize (CFList& factors ///< [in,out] a list of polys
               );

/// as above
void normalize (CFFList& factors ///< [in,out] a list of factors
               );

/// extract a subset given by @a index of size @a s from @a elements, if there
/// is no subset we have not yet considered noSubset is set to true. @a index
/// encodes the next subset, e.g. if s= 3, elements= {a,b,c,d},
/// index= {1, 2, 4, 0}, then subset= {a,c,d}. @a index is of size
/// @a elements.size().
///
/// @return @a subset returns a list of polys of length @a s if there is a
///         subset we have not yet considered.
CFList subset (int index [],            ///< [in,out] an array encoding the next
                                        ///< subset
               const int& s,            ///< [in] size of the subset
               const CFArray& elements, ///< [in] an array of polys
               bool& noSubset           ///< [in,out] if there is no subset we
                                        ///< have not yet considered @a noSubset
                                        ///< is true
              );

/// write elements of @a list into an array
///
/// @return an array of polys
CFArray copy (const CFList& list ///< [in] a list of polys
             );

/// update @a index
void indexUpdate (int index [],          ///< [in,out] an array encoding a
                                         ///< subset of size subsetSize
                  const int& subsetSize, ///< [in] size of the subset
                  const int& setSize,    ///< [in] size of the given set
                  bool& noSubset         ///< [in,out] if there is no subset we
                                         ///< have not yet considered @a
                                         ///< noSubset is true
                 );

/// compute the sum of degrees in Variable(1) of elements in S
///
/// @return @a subsetDegree returns the sum of degrees in Variable(1) of
///         elements in S
int subsetDegree (const CFList& S ///< [in] a list of polys
                 );

/// determine multiplicity of the factors
///
/// @return a list of factors of F with their multiplicity
CFFList multiplicity (CanonicalForm& F,     ///< [in] a poly
                      const CFList& factors ///< [in] a list of factors of F
                     );

/// compute the coefficients of the logarithmic derivative of G mod
/// Variable (2)^l over Fq
///
/// @return an array of coefficients of the logarithmic derivative of G mod
///         Variable (2)^l
CFArray logarithmicDerivative (const CanonicalForm& F,///<[in] a bivariate poly
                              const CanonicalForm& G, ///<[in] a factor of F
                              int l,                  ///<[in] lifting precision
                              CanonicalForm& Q        ///<[in,out] F/G
                              );

/// compute the coefficients of the logarithmic derivative of G mod
/// Variable (2)^l over Fq with oldQ= F/G mod Variable (2)^oldL
///
/// @return an array of coefficients of the logarithmic derivative of G mod
///         Variable (2)^l
CFArray
logarithmicDerivative (const CanonicalForm& F,   ///< [in] bivariate poly
                       const CanonicalForm& G,   ///< [in] a factor of F
                       int l,                    ///< [in] lifting precision
                       int oldL,                 ///< [in] old precision
                       const CanonicalForm& oldQ,///< [in] F/G mod
                                                 ///< Variable(2)^oldL
                       CanonicalForm& Q          ///< [in, out] F/G
                      );

/// compute bounds for logarithmic derivative as described in K. Belabas, 
/// M. van Hoeij, J. Klüners, and A. Steel, Factoring polynomials over global 
/// fields
///
/// @return @a computeBounds returns bounds as described above
int *
computeBounds (const CanonicalForm& F,///< [in] compressed bivariate polynomial
               int& n                 ///< [in,out] length of output
              );

/// extract coefficients of \f$ x^i \f$ for \f$i\geq k\f$ where \f$ x \f$ is
/// a variable of level 1
///
/// @return coefficients of \f$ x^i \f$ for \f$i\geq k\f$
/// @sa {getCoeffs (const CanonicalForm&, const int, const Variable&),
/// getCoeffs (const CanonicalForm&, const int, const int, const int, 
/// const Variable&, const CanonicalForm&, const mat_zz_p&)}
CFArray
getCoeffs (const CanonicalForm& F,///< [in] compressed bivariate poly over F_p
           const int k            ///< [in] some int
          );

/// extract coefficients of \f$ x^i \f$ for \f$i\geq k\f$ where \f$ x \f$ is
/// a variable of level 1
///
/// @return coefficients of \f$ x^i \f$ for \f$i\geq k\f$
/// @sa {getCoeffs (const CanonicalForm&, const int),
/// getCoeffs (const CanonicalForm&, const int, const int, const int, 
/// const Variable&, const CanonicalForm&, const mat_zz_p&)}
CFArray
getCoeffs (const CanonicalForm& F,///< [in] compressed bivariate poly over 
                                  ///< F_p(alpha)
           const int k,           ///< [in] some int
           const Variable& alpha  ///< [in] algebraic variable
          );

#ifdef HAVE_NTL
/// extract coefficients of \f$ x^i \f$ for \f$i\geq k\f$ where \f$ x \f$ is
/// a variable of level 1
///
/// @return coefficients of \f$ x^i \f$ for \f$i\geq k\f$
/// @sa {getCoeffs (const CanonicalForm&, const int, const Variable& alpha),
/// getCoeffs (const CanonicalForm&, const int)}
CFArray
getCoeffs (const CanonicalForm& F,         ///< [in] compressed bivariate poly
           const int k,                    ///< [in] some int
           const int l,                    ///< [in] precision
           const int degMipo,              ///< [in] degree of minimal poly
           const Variable& alpha,          ///< [in] algebraic variable
           const CanonicalForm& evaluation,///< [in] evaluation point
           const mat_zz_p& M               ///< [in] bases change matrix
          );
#endif

/// write A into M starting at row startIndex
void
writeInMatrix (CFMatrix& M,        ///< [in,out] some matrix
               const CFArray& A,   ///< [in] array of polys
               const int column,   ///< [in] column in which A is written
               const int startIndex///< [in] row in which to start
              );

/// checks if a substitution x^n->x is possible
///
/// @return an integer n > 1, if a substitution described as above is possible
///         else n <= 1
int substituteCheck (const CFList& L ///< [in] a list of univariate polys
                    );

/// substitute x^d by x in F
void
subst (const CanonicalForm& F, ///< [in] a polynomial 
       CanonicalForm& A,       ///< [in,out] returns F with x^d replaced by x
       const int d,            ///< d > 1 such that a substitution x^d -> x
                               ///< [in] is possible
       const Variable& x       ///< [in] a Variable
      );

/// reverse a substitution x^d->x
///
/// @return a poly with x replaced by x^d
CanonicalForm
reverseSubst (const CanonicalForm& F, ///< [in] a poly
              const int d,            ///< [in] an integer > 0
              const Variable& x       ///< [in] a Variable
             );

/// reverse a substitution x^d->x
void
reverseSubst (CFList& L,        ///< [in,out] a list of polys, returns the 
                                ///< given list with x replaced by x^d
              const int d,      ///< [in] an integer > 0
              const Variable& x ///< [in] a Variable
             );

/// check if a substitution x^n->x is possible
///
/// @return an integer n > 1, if a substitution described as above is possible
///         else n <= 1
int
substituteCheck (const CanonicalForm& F, ///<[in] a polynomial
                 const Variable& x       ///<[in] some variable
                );

#endif
/* FAC_FQ_BIVAR_UTIL_H */

