/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFactorize.h
 *
 * multivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_FACTORIZE_H
#define FAC_FACTORIZE_H

#include <config.h>

#include "facBivar.h"
#include "facFqBivarUtil.h"

#ifdef HAVE_NTL
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
CFList
ratSqrfFactorize (const CanonicalForm & G,        ///<[in] a multivariate poly
                  const Variable& v= Variable (1) ///<[in] algebraic variable
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
CFFList
ratFactorize (const CanonicalForm& G,          ///<[in] a multivariate poly
              const Variable& v= Variable (1), ///<[in] algebraic variable
              bool substCheck= true            ///<[in] enables substitute check
             )
{
  if (getNumVars (G) == 2)
  {
    CFFList result= ratBiFactorize (G,v);
    return result;
  }
  CanonicalForm F= G;

  if (substCheck)
  {
    bool foundOne= false;
    int * substDegree= new int [F.level()];
    for (int i= 1; i <= F.level(); i++)
    {
      if (degree (F, i) > 0)
      {
        substDegree[i-1]= substituteCheck (F, Variable (i));
        if (substDegree [i-1] > 1)
        {
          foundOne= true;
          subst (F, F, substDegree[i-1], Variable (i));
        }
      }
      else
        substDegree[i-1]= -1;
    }
    if (foundOne)
    {
      CFFList result= ratFactorize (F, v, false);
      CFFList newResult, tmp;
      CanonicalForm tmp2;
      newResult.insert (result.getFirst());
      result.removeFirst();
      for (CFFListIterator i= result; i.hasItem(); i++)
      {
        tmp2= i.getItem().factor();
        for (int j= 1; j <= G.level(); j++)
        {
          if (substDegree[j-1] > 1)
            tmp2= reverseSubst (tmp2, substDegree[j-1], Variable (j));
        }
        tmp= ratFactorize (tmp2, v, false);
        tmp.removeFirst();
        for (CFFListIterator j= tmp; j.hasItem(); j++)
          newResult.append (CFFactor (j.getItem().factor(),
                                      j.getItem().exp()*i.getItem().exp()));
      }
      delete [] substDegree;
      return newResult;
    }
    delete [] substDegree;
  }

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

#endif

