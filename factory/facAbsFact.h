/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.h
 *
 * absolute multivariate factorization over Q
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_FACT_H
#define FAC_ABS_FACT_H

#include "facAbsBiFact.h"

#if defined(HAVE_NTL)
/// main absolute factorization routine, expects poly which is
/// irreducible over Q
///
/// @return absFactorizeMain returns a list whose entries contain three
///         entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined (note: in case the factor is already defined over
///         Q[t]/(t), 1 is returned as defining poly), and the
///         multiplicity of the absolute irreducible factor
CFAFList absFactorizeMain (const CanonicalForm& F ///<[in] irred poly over Q
                          );
#endif

/*BEGINPUBLIC*/
#if defined(HAVE_NTL)
/// absolute factorization of a multivariate poly over Q
///
/// @return absFactorize returns a list whose entries contain three
///         entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined (note: in case the factor is already defined over
///         Q[t]/(t), 1 is returned), and the multiplicity of the
///         absolute irreducible factor
CFAFList absFactorize (const CanonicalForm& G ///<[in] poly over Q
                      );
#endif
/*ENDPUBLIC*/


#endif
