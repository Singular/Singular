/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFactorize.h
 *
 * multivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FACTORIZE_H
#define FAC_FACTORIZE_H

#include <config.h>

#include "facBivar.h"
#include "facFqBivarUtil.h"

/// Factorization over Q (a)
///
/// @return @a multiFactorize returns a factorization of F
CFList
multiFactorize (const CanonicalForm& F,     ///< [in] poly to be factored
                const Variable& v           ///< [in] some algebraic variable
               );

/// factorize a squarefree multivariate polynomial over \f$ Q(\alpha) \f$
///
/// @return @a ratSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
inline
CFList ratSqrfFactorize (const CanonicalForm & G, ///< [in] a multivariate poly
                         const Variable& v
                       )
{
  if (getNumVars (G) == 2)
    return ratBiSqrfFactorize (G, v);
  CanonicalForm F= G;
  if (isOn (SW_RATIONAL))
    F *= bCommonDen (F);
  CFList result= multiFactorize (F, v);
  if (isOn (SW_RATIONAL))
  {
    normalize (result);
    result.insert (Lc(F));
  }
  return result;
}

/// factorize a multivariate polynomial over \f$ Q(\alpha) \f$
///
/// @return @a ratFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
inline
CFFList ratFactorize (const CanonicalForm& G, ///< [in] a multivariate poly
                      const Variable& v
                    )
{
  if (getNumVars (G) == 2)
  {
    CFFList result= ratBiFactorize (G,v);
    return result;
  }
  CanonicalForm F= G;
  CanonicalForm LcF= Lc (F);
  if (isOn (SW_RATIONAL))
    F *= bCommonDen (F);

  CFFList result;
  CFFList sqrfFactors= sqrFree (F);
  for (CFFListIterator i= sqrfFactors; i.hasItem(); i++)
  {
    CFList tmp= ratSqrfFactorize (i.getItem().factor(), v);
    for (CFListIterator j= tmp; j.hasItem(); j++)
    {
      if (j.getItem().inCoeffDomain()) continue;
      result.append (CFFactor (j.getItem(), i.getItem().exp()));
    }
  }
  if (isOn (SW_RATIONAL))
  {
    normalize (result);
    result.insert (CFFactor (LcF, 1));
  }
  return result;
}

#endif

