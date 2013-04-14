/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.h
 *
 * bivariate absolute factorization over Q described in "Modular Las Vegas
 * Algorithms for Polynomial Absolute Factorization" by Bertone, Chèze, Galligo
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_FACT_H
#define FAC_ABS_FACT_H

#include "assert.h"

#include "canonicalform.h"
#include "cf_map.h"
#include "cfNewtonPolygon.h"

CFAFList absFactorizeMain (const CanonicalForm& F);

static inline
void normalize (CFAFList & L)
{
  for (CFAFListIterator i= L; i.hasItem(); i++)
    i.getItem()= CFAFactor (i.getItem().factor()/Lc (i.getItem().factor()),
                            i.getItem().minpoly(), i.getItem().exp());
}

static inline
CFAFList uniAbsFactorize (const CanonicalForm& F)
{
  CFFList rationalFactors= factorize (F);
  CFFListIterator i= rationalFactors;
  i++;
  CanonicalForm mipo;
  CFAFList result;
  for (; i.hasItem(); i++)
  {
    mipo= rootOf (i.getItem().factor());
    result.append (CFAFactor (i.getItem().factor(), mipo, i.getItem().exp()));
  }
  result.insert (CFAFactor (rationalFactors.getFirst().factor(), 1, 1));
  return result;
}

/*BEGINPUBLIC*/

CFAFList absFactorize (const CanonicalForm& G);

/*ENDPUBLIC*/

CFAFList absFactorize (const CanonicalForm& G)
{
  //TODO handle homogeneous input
  ASSERT (getNumVars (F) == 2, "expected bivariate input");
  ASSERT (getCharacteristic() == 0 && isOn (SW_RATIONAL), "expected poly over Q");

  CFMap N;
  CanonicalForm F= compress (G, N);
  bool isRat= isOn (SW_RATIONAL);
  if (isRat)
  {
    F *= bCommonDen (F);
    Off (SW_RATIONAL);
  }
  F /= icontent (F);
  if (isRat)
    On (SW_RATIONAL);

  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFAFList contentXFactors, contentYFactors;
  contentXFactors= uniAbsFactorize (contentX);
  contentYFactors= uniAbsFactorize (contentY);

  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  if (F.inCoeffDomain())
  {
    CFAFList result;
    for (CFAFListIterator i= contentXFactors; i.hasItem(); i++)
      result.append (CFAFactor (N (i.getItem().factor()), i.getItem().minpoly(),
                                i.getItem().exp()));
    for (CFAFListIterator i= contentYFactors; i.hasItem(); i++)
      result.append (CFAFactor (N (i.getItem().factor()),i.getItem().minpoly(),
                                i.getItem().exp()));
    normalize (result);
    result.insert (CFAFactor (Lc (G), 1, 1));
    return result;
  }
  CFFList rationalFactors= factorize (F);

  CFAFList result, resultBuf;

  CFAFListIterator iter;
  CFFListIterator i= rationalFactors;
  i++;
  for (; i.hasItem(); i++)
  {
    resultBuf= absFactorizeMain (i.getItem().factor());
    for (iter= resultBuf; iter.hasItem(); iter++)
      iter.getItem()= CFAFactor (iter.getItem().factor(),
                                 iter.getItem().minpoly(), i.getItem().exp());
    result= Union (result, resultBuf);
  }


  for (CFAFListIterator i= result; i.hasItem(); i++)
    i.getItem()= CFAFactor (N (i.getItem().factor()), i.getItem().minpoly(),
                            i.getItem().exp());
  for (CFAFListIterator i= contentXFactors; i.hasItem(); i++)
    result.append (CFAFactor (N(i.getItem().factor()), i.getItem().minpoly(),
                              i.getItem().exp()));
  for (CFAFListIterator i= contentYFactors; i.hasItem(); i++)
    result.append (CFAFactor (N(i.getItem().factor()), i.getItem().minpoly(),
                              i.getItem().exp()));
  normalize (result);
  result.insert (CFAFactor (Lc(G), 1, 1));
  return result;
}

#endif
