/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorizeUtil.h
 *
 * This file provides utility functions for multivariate factorization
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_FACTORIZE_UTIL_H
#define FAC_FQ_FACTORIZE_UTIL_H

// #include "config.h"
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
            const CFList& evaluation,///< [in] a valid evaluation point
            int l= 2                 ///< [in] level at which the evaluation
                                     ///< starts
           );

/// reverse shifting the evaluation point to zero
///
/// @return @a reverseShift returns a poly whose shift to zero is reversed
/// @sa shift2Zero(), evalPoints()
CanonicalForm reverseShift (const CanonicalForm& F,  ///< [in] a compressed poly
                            const CFList& evaluation,///< [in] a valid
                                                     ///< evaluation point
                            int l= 2                 ///< [in] level at which
                                                     ///< the evaluation starts
                           );

/// check if @a F consists of more than just the leading coeff wrt. Variable (1)
///
/// @return as described above
bool isOnlyLeadingCoeff (const CanonicalForm& F ///< [in] some poly
                        );

/// sort CFFList by the number variables in a factor
CFFList sortCFFListByNumOfVars (CFFList & F ///< [in,out] a list of factors
                               );

/// like getVars but each variable x occuring in @a F is raised to x^degree (F,x)
CanonicalForm myGetVars (const CanonicalForm& F ///< [in] a polynomial
                        );

/// evaluate @a F at @a evaluation
///
/// @return @a evaluateAtEval returns a list containing the successive
/// evaluations of @a F, last entry is @a F again
CFList
evaluateAtEval (const CanonicalForm& F,   ///<[in] some poly
                const CFArray& evaluation ///<[in] some evaluation point
               );

/// evaluate @a F at @a evaluation
///
/// @return @a evaluateAtEval returns a list containing the successive
/// evaluations of @a F starting at level @a l, last entry is @a F again
CFList
evaluateAtEval (const CanonicalForm& F,  ///<[in] some poly
                const CFList& evaluation,///<[in] some evaluation point
                int l                    ///<[in] level to start at
               );

/// evaluate F successively n-2 at 0
///
/// @return returns a list of successive evaluations of F, ending with F
CFList evaluateAtZero (const CanonicalForm& F ///< [in] some poly
                      );

/// divides factors by their content wrt. Variable(1) and checks if these polys
/// divide F
///
/// @return returns factors of F
CFList recoverFactors (const CanonicalForm& F, ///< [in] some poly F
                       const CFList& factors   ///< [in] some list of
                                               ///< factor candidates
                      );

/// divides factors shifted by evaluation by their content wrt. Variable(1) and
/// checks if these polys divide F
///
/// @return returns factors of F
CFList recoverFactors (const CanonicalForm& F,  ///< [in] some poly F
                       const CFList& factors,   ///< [in] some list of
                                                ///< factor candidates
                       const CFList& evaluation ///< [in] evaluation point
                      );

/// checks if factors divide F, if so F is divided by this factor and the factor
/// is divided by its content wrt. Variable(1) and the entry in index at the
/// position of the factor is set to 1, otherwise the entry in index is set to 0
///
/// @return returns factors of F
CFList recoverFactors (CanonicalForm& F,     ///< [in,out] some poly F
                       const CFList& factors,///< [in] some list of
                                             ///< factor candidates
                       int* index            ///< [in] position of real factors
                      );

#endif
/* FAC_FQ_FACTORIZE_UTIL_H */

