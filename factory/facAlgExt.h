/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgExt.h
 *
 * Univariate factorization over algebraic extension of Q using Trager's
 * algorithm
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
**/
//*****************************************************************************

#ifndef FAC_ALG_EXT_H
#define FAC_ALG_EXT_H

#include "cf_assert.h"

#include "canonicalform.h"

///factorize a univariate squarefree polynomial over algebraic extension of Q
///
/// @return @a AlgExtSqrfFactorize returns a list of factors of F
CFList
AlgExtSqrfFactorize (const CanonicalForm& F, ///<[in] a univariate squarefree
                                             ///< polynomial
                     const Variable& alpha   ///<[in] an algebraic variable
                    );

/// factorize a univariate polynomial over algebraic extension of Q
///
/// @return @a AlgExtFactorize returns a list of factors of F with multiplicity
CFFList
AlgExtFactorize (const CanonicalForm& F, ///<[in] a univariate polynomial
                 const Variable& alpha   ///<[in] an algebraic variable
                );

#endif

