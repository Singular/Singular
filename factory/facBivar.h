/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facBivar.h
 *
 * bivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_BIVAR_H
#define FAC_BIVAR_H

#include <config.h>

#include "assert.h"

#include "facFqBivarUtil.h"
#include "DegreePattern.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "cf_map.h"
#include "cfNewtonPolygon.h"
#include "algext.h"

/// @return @a biFactorize returns a list of factors of F. If F is not monic
///         its leading coefficient is not outputted.
CFList
biFactorize (const CanonicalForm& F,       ///< [in] a bivariate poly
             const Variable& v             ///< [in] some algebraic variable
            );

/// factorize a squarefree bivariate polynomial over \f$ Q(\alpha) \f$.
///
/// @ return @a ratBiSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
inline
CFList ratBiSqrfFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
                           const Variable& v
                         )
{
  CFMap N;
  CanonicalForm F= compress (G, N);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  contentXFactors= factorize (contentX, v);
  contentYFactors= factorize (contentY, v);
  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  if (F.inCoeffDomain())
  {
    CFList result;
    for (CFFListIterator i= contentXFactors; i.hasItem(); i++)
      result.append (N (i.getItem().factor()));
    for (CFFListIterator i= contentYFactors; i.hasItem(); i++)
      result.append (N (i.getItem().factor()));
    if (isOn (SW_RATIONAL))
    {
      normalize (result);
      result.insert (Lc (G));
    }
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  F= compress (F, M, S);
  CFList result= biFactorize (F, v);
  for (CFListIterator i= result; i.hasItem(); i++)
    i.getItem()= N (decompress (i.getItem(), M, S));
  for (CFFListIterator i= contentXFactors; i.hasItem(); i++)
    result.append (N(i.getItem().factor()));
  for (CFFListIterator i= contentYFactors; i.hasItem(); i++)
    result.append (N (i.getItem().factor()));
  if (isOn (SW_RATIONAL))
  {
    normalize (result);
    result.insert (Lc (G));
  }
  return result;
}

/// factorize a bivariate polynomial over \f$ Q(\alpha) \f$
///
/// @return @a ratBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
inline
CFFList ratBiFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
                        const Variable& v
                      )
{
  CFMap N;
  CanonicalForm F= compress (G, N);
  CanonicalForm LcF= Lc (F);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  contentXFactors= factorize (contentX, v);
  contentYFactors= factorize (contentY, v);
  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  decompress (contentXFactors, N);
  decompress (contentYFactors, N);
  CFFList result, resultRoot;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    if (isOn (SW_RATIONAL))
    {
      normalize (result);
      result.insert (CFFactor (LcF, 1));
    }
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  F= compress (F, M, S);
  CanonicalForm sqrfP= sqrfPart (F);
  CFList buf;
  buf= biFactorize (sqrfP, v);
  result= multiplicity (F, buf);
  for (CFFListIterator i= result; i.hasItem(); i++)
    i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  if (isOn (SW_RATIONAL))
  {
    normalize (result);
    result.insert (CFFactor (LcF, 1));
  }
  return result;
}

/// convert a CFFList to a CFList by dropping the multiplicity
CFList conv (const CFFList& L ///< [in] a CFFList
            );

#endif

