/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorizeUtil.h
 *
 * This file provides utility functions for multivariate factorization
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_FACTORIZE_UTIL_H
#define FAC_FQ_FACTORIZE_UTIL_H

#include <config.h>

#include "canonicalform.h"
#include "cf_map.h"

/// append @a factors2 to @a factors1 and decompress
///
static inline
void decompressAppend (CFList& factors1,       ///< [in,out] a list of polys,
                                               ///< returns @a factors2 appended
                                               ///< to it and everything is
                                               ///< decompressed
                       const CFList& factors2, ///< [in] a list of polys
                       const CFMap& N          ///< [in] a map
                      )
{
  for (CFListIterator i= factors1; i.hasItem(); i++)
    i.getItem()= N (i.getItem());
  for (CFListIterator i= factors2; i.hasItem(); i++)
    factors1.append (N (i.getItem()));
}


/// swap elements in @a factors2 and append them to @a factors1
///
void appendSwap (CFList& factors1,       ///< [in,out] a list of polys,
                                         ///< returns swapped elements of @a
                                         ///< factors2 appended to it
                 const CFList& factors2, ///< [in] a list of polys
                 const int swapLevel1,   ///< [in] level of variable to be
                                         ///< swapped with x, 0 if no swapping
                 const int swapLevel2,   ///< [in] level of variable to be
                                         ///< swapped with x, 0 if no swapping
                 const Variable& x       ///< [in] a variable
                );

/// swap elements in @a factors
void swap (CFList& factors,       ///< [in] a list of polys, returns swapped
                                  ///< elements of factors
           const int swapLevel1, ///< [in] level of variable to be
                                  ///< swapped with x, 0 if no swapping
           const int swapLevel2, ///< [in] level of variable to be
                                  ///< swapped with x, 0 if no swapping
           const Variable& x      ///< [in] a variable
          );

/// swap elements of @a factors2, append them to @a factors1 and decompress
void appendSwapDecompress (
                  CFList& factors1,       ///< [in,out] a list of polys,
                                          ///< returns swapped elements of @a
                                          ///< factors2 appended to it and
                                          ///< everything is decompressed
                  const CFList& factors2, ///< [in] a list of polys
                  const CFMap& N,         ///< [in] a map
                  const int swapLevel,    ///< [in] level of variable to be
                                          ///< swapped with x, 0 if no swapping
                  const Variable& x       ///< [in] a variable
                          );

/// swap elements of @a factors2, append them to @a factors1 and decompress
void appendSwapDecompress (
                  CFList& factors1,       ///< [in,out] a list of polys,
                                          ///< returns swapped elements of @a
                                          ///< factors2 appended to it and
                                          ///< everything is decompressed
                  const CFList& factors2, ///< [in] a list of polys
                  const CFMap& N,         ///< [in] a map
                  const int swapLevel1,   ///< [in] level of variable to be
                                          ///< swapped with x, 0 if no swapping
                  const int swapLevel2,   ///< [in] level of variable to be
                                          ///< swapped with x, 0 if no swapping
                  const Variable& x       ///< [in] a variable
                          );


/// compute lifting bounds
///
/// @return @a liftingBounds returns an array containing the lift bounds for A
int* liftingBounds (const CanonicalForm& A,    ///< [in] a compressed poly
                    const int& bivarLiftBound  ///< [in] lift bound for
                                               ///< @a biFactorizer()
                   );

/// shift evaluation point to zero
/// @return @a shift2Zero returns @a F shifted by @a evaluation s.t. 0 is a
///         valid evaluation point
/// @sa evalPoints(), reverseShift()
CanonicalForm
shift2Zero (const CanonicalForm& F,  ///< [in] a compressed poly
            CFList& Feval,           ///< [in,out] an empty list, returns
                                     ///< @a F successively evaluated
                                     ///< at 0
            const CFList& evaluation ///< [in] a valid evaluation point
           );

/// reverse shifting the evaluation point to zero
///
/// @return @a reverseShift returns a poly whose shift to zero is reversed
/// @sa shift2Zero(), evalPoints()
CanonicalForm reverseShift (const CanonicalForm& F,  ///< [in] a compressed poly
                            const CFList& evaluation ///< [in] a valid
                                                     ///< evaluation point
                           );

#endif
/* FAC_FQ_FACTORIZE_UTIL_H */

