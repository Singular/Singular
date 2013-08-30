/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsBiFact.h
 *
 * bivariate absolute factorization over Q described in "Modular Las Vegas
 * Algorithms for Polynomial Absolute Factorization" by Bertone, Chèze, Galligo
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_BI_FACT_H
#define FAC_ABS_BI_FACT_H

#include "canonicalform.h"

#ifdef HAVE_NTL
/// main absolute factorization routine, expects bivariate poly which is
/// primitive wrt. any of its variables and irreducible over Q
///
/// @return absBiFactorizeMain returns a list whose entries contain three
///         entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined, and the multiplicity of the absolute irreducible
///         factor
CFAFList absBiFactorizeMain (const CanonicalForm& F, ///<[in] s.a.
                             bool full= false        ///<[in] true if all
                                                     ///< factors should be
                                                     ///< returned
                            );
#endif

/// normalize factors, i.e. make factors monic
static inline
void normalize (CFAFList & L)
{
  for (CFAFListIterator i= L; i.hasItem(); i++)
    i.getItem()= CFAFactor (i.getItem().factor()/Lc (i.getItem().factor()),
                            i.getItem().minpoly(), i.getItem().exp());
}

/// univariate absolute factorization over Q
///
/// @return uniAbsFactorize returns a list whose entries contain three entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined, and the multiplicity of the absolute irreducible
///         factor
CFAFList uniAbsFactorize (const CanonicalForm& F, ///<[in] univariate poly
                                                  ///< irreducible over Q
                          bool full= false        ///<[in] true if all factors
                                                  ///< should be returned
                         );

#endif
