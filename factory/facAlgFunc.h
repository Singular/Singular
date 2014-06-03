/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFunc.h
 *
 * Factorization over algebraic function fields
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ALG_FUNC_H
#define FAC_ALG_FUNC_H

#include "canonicalform.h"

CanonicalForm alg_gcd(const CanonicalForm &, const CanonicalForm &, const CFList &);
/*BEGINPUBLIC*/

/// factorize a polynomial @a f that is irreducible over the ground field modulo
/// an extension given by an irreducible characteristic set @a as, @a f is
/// assumed to be integral, i.e. \f$ f\in K[x_1,\ldots,x_n]/(as) \f$, and each
/// element of @a as is assumed to be integral as well. \f$ K \f$ must be either
/// \f$ F_p \f$ or \f$ Q \f$.
///
/// @return the returned factors are not necessarily monic but only primitive
/// and the product of the factors equals @a f up to a unit.
CFFList facAlgFunc2 (const CanonicalForm & f,///<[in] univariate poly
                     const CFList & as       ///<[in] irreducible characteristic
                                             ///< set
                    );

/// factorize a polynomial @a f modulo an extension given by an irreducible
/// characteristic set as, @a f is assumed to be integral, i.e.
/// \f$ f\in K[x_1,\ldots,x_n]/(as) \f$, and each element of @a as is assumed to
/// be integral as well. \f$ K \f$ must be either \f$ F_p \f$ or \f$ Q \f$.
///
/// @return the returned factors are not necessarily monic but only primitive
/// and the product of the factors equals @a f up to a unit.
CFFList facAlgFunc (const CanonicalForm & f, ///<[in] univariate poly
                    const CFList & as        ///<[in] irreducible characteristic
                                             ///< set
                   );
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */
