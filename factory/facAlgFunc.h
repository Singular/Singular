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
CFFList facAlgFunc2 (const CanonicalForm & f, const CFList & as);
CFFList facAlgFunc (const CanonicalForm & f, const CFList & as);
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */
