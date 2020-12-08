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

// #include "config.h"
#include "timing.h"

#include "facBivar.h"
#include "facFqBivarUtil.h"

TIMING_DEFINE_PRINT (fac_squarefree)
TIMING_DEFINE_PRINT (fac_factor_squarefree)

/// Factorization of A wrt. to different second vars
void
factorizationWRTDifferentSecondVars (
                          const CanonicalForm& A, ///<[in] poly
                          CFList*& Aeval,         ///<[in,out] A evaluated wrt.
                                                  ///< different second vars
                                                  ///< returns bivariate factors
                          int& minFactorsLength,  ///<[in,out] minimal length of
                                                  ///< bivariate factors
                          bool& irred,            ///<[in,out] Is A irreducible?
                          const Variable& w       ///<[in] alg. variable
                                    );

/// Factorization over Q (a)
///
/// @return @a multiFactorize returns a factorization of F
CFList
multiFactorize (const CanonicalForm& F,     ///< [in] sqrfree poly
                const Variable& v           ///< [in] some algebraic variable
               );
#ifdef HAVE_NTL // ratBiSqrfFactorize
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
#endif

#ifdef HAVE_NTL // multiFactorize, henselLiftAndEarly, nonMonicHenselLift
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
  TIMING_START (fac_squarefree);
  CFFList sqrfFactors= sqrFree (F);
  TIMING_END_AND_PRINT (fac_squarefree,
                        "time for squarefree factorization over Q: ");

  CFList tmp;
  for (CFFListIterator i= sqrfFactors; i.hasItem(); i++)
  {
    TIMING_START (fac_factor_squarefree);
    tmp= ratSqrfFactorize (i.getItem().factor(), v);
    TIMING_END_AND_PRINT (fac_factor_squarefree,
                          "time to factorize sqrfree factor over Q: ");
    for (CFListIterator j= tmp; j.hasItem(); j++)
    {
      if (j.getItem().inCoeffDomain()) continue;
      result.append (CFFactor (j.getItem(), i.getItem().exp()));
    }
  }
  if (isOn (SW_RATIONAL))
  {
    normalize (result);
    if (v.level() == 1)
    {
      for (CFFListIterator i= result; i.hasItem(); i++)
      {
        LcF /= power (bCommonDen (i.getItem().factor()), i.getItem().exp());
        i.getItem()= CFFactor (i.getItem().factor()*
                     bCommonDen(i.getItem().factor()), i.getItem().exp());
      }
    }
    result.insert (CFFactor (LcF, 1));
  }
  return result;
}
#endif
#endif

