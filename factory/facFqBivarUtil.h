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

#include <config.h>

#include "cf_map.h"
#include "ExtensionInfo.h"

/// append @a factors2 on @a factors1
void append (CFList& factors1,       ///< [in,out] a list of polys
             const CFList& factors2  ///< [in] a list of polys
            );

/// decompress a list of polys @a factors using the map @a N
void decompress (CFList& factors, ///< [in,out] a list of polys
                 const CFMap& N   ///< [in] a map
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
                    const int k                  ///< some int k such that k
                                                 ///< divides l if we are not
                                                 ///< over some Fq
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

#endif
/* FAC_FQ_BIVAR_UTIL_H */

