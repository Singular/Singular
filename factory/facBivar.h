/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facBivar.h
 *
 * bivariate factorization over Q(a)
 *
 * @author Martin Lee
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

#ifdef HAVE_NTL
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
CFList
ratBiSqrfFactorize (const CanonicalForm & G,        ///< [in] a bivariate poly
                    const Variable& v= Variable (1) ///< [in] algebraic variable
                   )
{
  CFMap N;
  CanonicalForm F= compress (G, N);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  if (v.level() != 1)
  {
    contentXFactors= factorize (contentX, v);
    contentYFactors= factorize (contentY, v);
  }
  else
  {
    contentXFactors= factorize (contentX);
    contentYFactors= factorize (contentY);
  }
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
  Variable tmp1, tmp2;
  CanonicalForm mipoTmp1;
  bool substAlgVar= false;
  if (v.level() != 1)
  {
    mipoTmp1= getMipo (v);
    if (!bCommonDen (getMipo (v)).isOne())
    {
      tmp2= v;
      mipoTmp1 *= bCommonDen (mipoTmp1);
      mipoTmp1= replacevar (mipoTmp1, v, Variable (1));
      tmp1= rootOf (mipoTmp1);
      F= replacevar (F, tmp2, tmp1);
      substAlgVar= true;
    }
    else
      tmp1= v;
  }
  else
    tmp1= v;
  CFList result= biFactorize (F, tmp1);
  if (substAlgVar)
  {
    for (CFListIterator i= result; i.hasItem(); i++)
      i.getItem()= N (decompress (replacevar (i.getItem(), tmp1, tmp2), M, S));
  }
  else
  {
    for (CFListIterator i= result; i.hasItem(); i++)
      i.getItem()= N (decompress (i.getItem(), M, S));
  }
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
CFFList
ratBiFactorize (const CanonicalForm & G,         ///< [in] a bivariate poly
                const Variable& v= Variable (1), ///< [in] algebraic variable
                bool substCheck= true        ///< [in] enables substitute check
               )
{
  CFMap N;
  CanonicalForm F= compress (G, N);

  if (substCheck)
  {
    bool foundOne= false;
    int * substDegree= new int [F.level()];
    for (int i= 1; i <= F.level(); i++)
    {
      substDegree[i-1]= substituteCheck (F, Variable (i));
      if (substDegree [i-1] > 1)
      {
        foundOne= true;
        subst (F, F, substDegree[i-1], Variable (i));
      }
    }
    if (foundOne)
    {
      CFFList result= ratBiFactorize (F, v, false);
      CFFList newResult, tmp;
      CanonicalForm tmp2;
      newResult.insert (result.getFirst());
      result.removeFirst();
      for (CFFListIterator i= result; i.hasItem(); i++)
      {
        tmp2= i.getItem().factor();
        for (int j= 1; j <= F.level(); j++)
        {
          if (substDegree[j-1] > 1)
            tmp2= reverseSubst (tmp2, substDegree[j-1], Variable (j));
        }
        tmp= ratBiFactorize (tmp2, v, false);
        tmp.removeFirst();
        for (CFFListIterator j= tmp; j.hasItem(); j++)
          newResult.append (CFFactor (j.getItem().factor(),
                                      j.getItem().exp()*i.getItem().exp()));
      }
      decompress (newResult, N);
      delete [] substDegree;
      return newResult;
    }
    delete [] substDegree;
  }

  CanonicalForm LcF= Lc (F);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  if (v.level() != 1)
  {
    contentXFactors= factorize (contentX, v);
    contentYFactors= factorize (contentY, v);
  }
  else
  {
    contentXFactors= factorize (contentX);
    contentYFactors= factorize (contentY);
  }
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
  Variable tmp1, tmp2;
  CanonicalForm mipoTmp1;
  bool substAlgVar= false;
  if (v.level() != 1)
  {
    mipoTmp1= getMipo (v);
    if (!bCommonDen (getMipo (v)).isOne())
    {
      tmp2= v;
      mipoTmp1 *= bCommonDen (mipoTmp1);
      mipoTmp1= replacevar (mipoTmp1, v, Variable (1));
      tmp1= rootOf (mipoTmp1);
      F= replacevar (F, tmp2, tmp1);
      substAlgVar= true;
    }
    else
      tmp1= v;
  }
  else
    tmp1= v;
  CFFList sqrfFactors= sqrFree (F);
  for (CFFListIterator i= sqrfFactors; i.hasItem(); i++)
  {
    CFList tmp= ratBiSqrfFactorize (i.getItem().factor(), tmp1);
    if (substAlgVar)
    {
      for (CFListIterator j= tmp; j.hasItem(); j++)
      {
        if (j.getItem().inCoeffDomain()) continue;
        result.append (CFFactor (N (decompress (replacevar (j.getItem(),
                                 tmp1, tmp2), M, S)), i.getItem().exp()));
      }
    }
    else
    {
      for (CFListIterator j= tmp; j.hasItem(); j++)
      {
        if (j.getItem().inCoeffDomain()) continue;
        result.append (CFFactor (N (decompress (j.getItem(), M, S)),
                                 i.getItem().exp()));
      }
    }
  }
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

modpk
coeffBound ( const CanonicalForm & f, int p, const CanonicalForm& mipo );

void findGoodPrime(const CanonicalForm &f, int &start);

modpk
coeffBound ( const CanonicalForm & f, int p );

#endif

#endif

