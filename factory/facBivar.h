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

// #include "config.h"

#include "cf_assert.h"
#include "timing.h"

#include "facFqBivarUtil.h"
#include "DegreePattern.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "cf_map.h"
#include "cf_algorithm.h"
#include "cfNewtonPolygon.h"
#include "fac_util.h"
#include "cf_algorithm.h"

TIMING_DEFINE_PRINT(fac_bi_sqrf)
TIMING_DEFINE_PRINT(fac_bi_factor_sqrf)

/// @return @a biFactorize returns a list of factors of F. If F is not monic
///         its leading coefficient is not outputted.
CFList
biFactorize (const CanonicalForm& F,       ///< [in] a sqrfree bivariate poly
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
  CanonicalForm contentX= content (F, 1); //erwarte hier primitiven input: primitiv über Z bzw. Z[a]
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

  mpz_t * M=new mpz_t [4];
  mpz_init (M[0]);
  mpz_init (M[1]);
  mpz_init (M[2]);
  mpz_init (M[3]);

  mpz_t * S=new mpz_t [2];
  mpz_init (S[0]);
  mpz_init (S[1]);

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

  mpz_clear (M[0]);
  mpz_clear (M[1]);
  mpz_clear (M[2]);
  mpz_clear (M[3]);
  delete [] M;

  mpz_clear (S[0]);
  mpz_clear (S[1]);
  delete [] S;

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

  mpz_t * M=new mpz_t [4];
  mpz_init (M[0]);
  mpz_init (M[1]);
  mpz_init (M[2]);
  mpz_init (M[3]);

  mpz_t * S=new mpz_t [2];
  mpz_init (S[0]);
  mpz_init (S[1]);

  F= compress (F, M, S);
  TIMING_START (fac_bi_sqrf);
  CFFList sqrfFactors= sqrFree (F);
  TIMING_END_AND_PRINT (fac_bi_sqrf,
                       "time for bivariate sqrf factors over Q: ");
  for (CFFListIterator i= sqrfFactors; i.hasItem(); i++)
  {
    TIMING_START (fac_bi_factor_sqrf);
    CFList tmp= ratBiSqrfFactorize (i.getItem().factor(), v);
    TIMING_END_AND_PRINT (fac_bi_factor_sqrf,
                          "time to factor bivariate sqrf factors over Q: ");
    for (CFListIterator j= tmp; j.hasItem(); j++)
    {
      if (j.getItem().inCoeffDomain()) continue;
      result.append (CFFactor (N (decompress (j.getItem(), M, S)),
                               i.getItem().exp()));
    }
  }
  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
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

  mpz_clear (M[0]);
  mpz_clear (M[1]);
  mpz_clear (M[2]);
  mpz_clear (M[3]);
  delete [] M;

  mpz_clear (S[0]);
  mpz_clear (S[1]);
  delete [] S;

  return result;
}

/// convert a CFFList to a CFList by dropping the multiplicity
CFList conv (const CFFList& L ///< [in] a CFFList
            );

/// compute p^k larger than the bound on the coefficients of a factor of @a f
/// over Q (mipo)
modpk
coeffBound (const CanonicalForm & f,  ///< [in] poly over Z[a]
            int p,                    ///< [in] some positive integer
            const CanonicalForm& mipo ///< [in] minimal polynomial with
                                      ///< denominator 1
           );

/// find a big prime p from our tables such that no term of f vanishes mod p
void findGoodPrime(const CanonicalForm &f, ///< [in] poly over Z or Z[a]
                   int &start              ///< [in,out] index of big prime in
                                           /// cf_primetab.h
                  );

/// compute p^k larger than the bound on the coefficients of a factor of @a f
/// over Z
modpk
coeffBound (const CanonicalForm & f, ///< [in] poly over Z
            int p                    ///< [in] some positive integer
           );

#endif

