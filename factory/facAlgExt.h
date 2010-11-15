// -*- c++ -*-
//*****************************************************************************
/** @file facAlgExt.h
 *
 * @author Martin Lee
 * @date
 *
 * Univariate factorization over algebraic extension of Q using Trager's
 * algorithm
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

#include "assert.h"

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
