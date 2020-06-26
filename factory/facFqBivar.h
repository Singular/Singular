/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqBivar.h
 *
 * This file provides functions for factorizing a bivariate polynomial over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_BIVAR_H
#define FAC_FQ_BIVAR_H

// #include "config.h"

#include "timing.h"
#include "cf_assert.h"

#include "facFqBivarUtil.h"
#include "DegreePattern.h"
#include "ExtensionInfo.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "cf_map.h"
#include "cfNewtonPolygon.h"
#include "fac_util.h"
#include "cf_algorithm.h"

TIMING_DEFINE_PRINT(fac_fq_bi_sqrf)
TIMING_DEFINE_PRINT(fac_fq_bi_factor_sqrf)

static const double log2exp= 1.442695041;

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
/// Factorization of a squarefree bivariate polynomials over an arbitrary finite
/// field, information on the current field we work over is in @a info. @a info
/// may also contain information about the initial field if initial and current
/// field do not coincide. In this case the current field is an extension of the
/// initial field and the factors returned are factors of F over the initial
/// field.
///
/// @return @a biFactorize returns a list of factors of F. If F is not monic
///         its leading coefficient is not outputted.
/// @sa extBifactorize()
CFList
biFactorize (const CanonicalForm& F,       ///< [in] a sqrfree bivariate poly
             const ExtensionInfo& info     ///< [in] information about extension
            );

inline CFList
biSqrfFactorizeHelper (const CanonicalForm& G, const ExtensionInfo& info)
{
  CFMap N;
  CanonicalForm F= compress (G, N);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  if (info.getAlpha().level() != 1)
  {
    contentXFactors= factorize (contentX, info.getAlpha());
    contentYFactors= factorize (contentY, info.getAlpha());
  }
  else if (info.getAlpha().level() == 1 && info.getGFDegree() == 1)
  {
    contentXFactors= factorize (contentX);
    contentYFactors= factorize (contentY);
  }
  else if (info.getAlpha().level() == 1 && info.getGFDegree() != 1)
  {
    CFList bufContentX, bufContentY;
    bufContentX= biFactorize (contentX, info);
    bufContentY= biFactorize (contentY, info);
    for (CFListIterator iter= bufContentX; iter.hasItem(); iter++)
      contentXFactors.append (CFFactor (iter.getItem(), 1));
    for (CFListIterator iter= bufContentY; iter.hasItem(); iter++)
      contentYFactors.append (CFFactor (iter.getItem(), 1));
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
    normalize (result);
    result.insert (Lc (G));
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
  CFList result= biFactorize (F, info);
  for (CFListIterator i= result; i.hasItem(); i++)
    i.getItem()= N (decompress (i.getItem(), M, S));
  for (CFFListIterator i= contentXFactors; i.hasItem(); i++)
    result.append (N(i.getItem().factor()));
  for (CFFListIterator i= contentYFactors; i.hasItem(); i++)
    result.append (N (i.getItem().factor()));
  normalize (result);
  result.insert (Lc(G));

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

/// factorize a squarefree bivariate polynomial over \f$ F_{p} \f$.
///
/// @return @a FpBiSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FqBiSqrfFactorize(), GFBiSqrfFactorize()
inline
CFList FpBiSqrfFactorize (const CanonicalForm & G ///< [in] a bivariate poly
                         )
{
  ExtensionInfo info= ExtensionInfo (false);
  return biSqrfFactorizeHelper (G, info);
}

/// factorize a squarefree bivariate polynomial over \f$ F_{p}(\alpha ) \f$.
///
/// @return @a FqBiSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FpBiSqrfFactorize(), GFBiSqrfFactorize()
inline
CFList FqBiSqrfFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
                          const Variable& alpha    ///< [in] algebraic variable
                         )
{
  ExtensionInfo info= ExtensionInfo (alpha, false);
  return biSqrfFactorizeHelper (G, info);
}

/// factorize a squarefree bivariate polynomial over GF
///
/// @return @a GFBiSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FpBiSqrfFactorize(), FqBiSqrfFactorize()
inline
CFList GFBiSqrfFactorize (const CanonicalForm & G ///< [in] a bivariate poly
                         )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  return biSqrfFactorizeHelper (G, info);
}

/// factorize a bivariate polynomial over \f$ F_{p} \f$
///
/// @return @a FpBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FqBiFactorize(), GFBiFactorize()
inline
CFFList
FpBiFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
               bool substCheck= true    ///< [in] enables substitute check
              )
{
  ExtensionInfo info= ExtensionInfo (false);
  CFMap N;
  CanonicalForm F= compress (G, N);

  if (substCheck)
  {
    bool foundOne= false;
    int * substDegree= NEW_ARRAY(int,F.level());
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
      CFFList result= FpBiFactorize (F, false);
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
        tmp= FpBiFactorize (tmp2, false);
        tmp.removeFirst();
        for (CFFListIterator j= tmp; j.hasItem(); j++)
          newResult.append (CFFactor (j.getItem().factor(),
                                      j.getItem().exp()*i.getItem().exp()));
      }
      decompress (newResult, N);
      DELETE_ARRAY(substDegree);
      return newResult;
    }
    DELETE_ARRAY(substDegree);
  }

  CanonicalForm LcF= Lc (F);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  contentXFactors= factorize (contentX);
  contentYFactors= factorize (contentY);
  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  decompress (contentXFactors, N);
  decompress (contentYFactors, N);
  CFFList result;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
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

  TIMING_START (fac_fq_bi_sqrf);
  CFFList sqrf= FpSqrf (F, false);
  TIMING_END_AND_PRINT (fac_fq_bi_sqrf,
                       "time for bivariate sqrf factors over Fp: ");
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    TIMING_START (fac_fq_bi_factor_sqrf);
    bufResult= biFactorize (iter.getItem().factor(), info);
    TIMING_END_AND_PRINT (fac_fq_bi_factor_sqrf,
                          "time to factor bivariate sqrf factors over Fp: ");
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (N (decompress (i.getItem(), M, S)),
                               iter.getItem().exp()));
  }

  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));

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

/// factorize a bivariate polynomial over \f$ F_{p}(\alpha ) \f$
///
/// @return @a FqBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpBiFactorize(), FqBiFactorize()
inline
CFFList
FqBiFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
               const Variable & alpha,  ///< [in] algebraic variable
               bool substCheck= true    ///< [in] enables substitute check
              )
{
  ExtensionInfo info= ExtensionInfo (alpha, false);
  CFMap N;
  CanonicalForm F= compress (G, N);

  if (substCheck)
  {
    bool foundOne= false;
    int * substDegree= NEW_ARRAY(int,F.level());
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
      CFFList result= FqBiFactorize (F, alpha, false);
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
        tmp= FqBiFactorize (tmp2, alpha, false);
        tmp.removeFirst();
        for (CFFListIterator j= tmp; j.hasItem(); j++)
          newResult.append (CFFactor (j.getItem().factor(),
                                      j.getItem().exp()*i.getItem().exp()));
      }
      decompress (newResult, N);
      DELETE_ARRAY(substDegree);
      return newResult;
    }
    DELETE_ARRAY(substDegree);
  }

  CanonicalForm LcF= Lc (F);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  contentXFactors= factorize (contentX, alpha);
  contentYFactors= factorize (contentY, alpha);
  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  decompress (contentXFactors, N);
  decompress (contentYFactors, N);
  CFFList result;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
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

  TIMING_START (fac_fq_bi_sqrf);
  CFFList sqrf= FqSqrf (F, alpha, false);
  TIMING_END_AND_PRINT (fac_fq_bi_sqrf,
                       "time for bivariate sqrf factors over Fq: ");
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    TIMING_START (fac_fq_bi_factor_sqrf);
    bufResult= biFactorize (iter.getItem().factor(), info);
    TIMING_END_AND_PRINT (fac_fq_bi_factor_sqrf,
                          "time to factor bivariate sqrf factors over Fq: ");
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (N (decompress (i.getItem(), M, S)),
                               iter.getItem().exp()));
  }

  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));

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

/// factorize a bivariate polynomial over GF
///
/// @return @a GFBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpBiFactorize(), FqBiFactorize()
inline
CFFList
GFBiFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
               bool substCheck= true    ///< [in] enables substitute check
              )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  CFMap N;
  CanonicalForm F= compress (G, N);

  if (substCheck)
  {
    bool foundOne= false;
    int * substDegree=NEW_ARRAY(int,F.level());
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
      CFFList result= GFBiFactorize (F, false);
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
        tmp= GFBiFactorize (tmp2, false);
        tmp.removeFirst();
        for (CFFListIterator j= tmp; j.hasItem(); j++)
          newResult.append (CFFactor (j.getItem().factor(),
                                      j.getItem().exp()*i.getItem().exp()));
      }
      decompress (newResult, N);
      DELETE_ARRAY(substDegree);
      return newResult;
    }
    DELETE_ARRAY(substDegree);
  }

  CanonicalForm LcF= Lc (F);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFFList contentXFactors, contentYFactors;
  contentXFactors= factorize (contentX);
  contentYFactors= factorize (contentY);
  if (contentXFactors.getFirst().factor().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().factor().inCoeffDomain())
    contentYFactors.removeFirst();
  decompress (contentXFactors, N);
  decompress (contentYFactors, N);
  CFFList result;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
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

  TIMING_START (fac_fq_bi_sqrf);
  CFFList sqrf= GFSqrf (F, false);
  TIMING_END_AND_PRINT (fac_fq_bi_sqrf,
                       "time for bivariate sqrf factors over GF: ");
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    TIMING_START (fac_fq_bi_factor_sqrf);
    bufResult= biFactorize (iter.getItem().factor(), info);
    TIMING_END_AND_PRINT (fac_fq_bi_factor_sqrf,
                          "time to factor bivariate sqrf factors over GF: ");
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (N (decompress (i.getItem(), M, S)),
                               iter.getItem().exp()));
  }

  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));

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

/// \f$ \prod_{f\in L} {f (0, x)} \ mod\ M \f$ via divide-and-conquer
///
/// @return @a prodMod0 computes the above defined product
/// @sa prodMod()
CanonicalForm prodMod0 (const CFList& L,       ///< [in] a list of compressed,
                                               ///< bivariate polynomials
                        const CanonicalForm& M,///< [in] a power of Variable (2)
                        const modpk& b= modpk()///< [in] coeff bound
                       );

/// find an evaluation point p, s.t. F(p,y) is squarefree and
/// \f$ deg_{y} (F(p,y))= deg_{y} (F(x,y)) \f$.
///
/// @return @a evalPoint returns an evaluation point, which is valid if and only
///         if fail == false.
CanonicalForm
evalPoint (const CanonicalForm& F, ///< [in] compressed, bivariate poly
           CanonicalForm & eval,   ///< [in,out] F (p, y)
           const Variable& alpha,  ///< [in] algebraic variable
           CFList& list,           ///< [in] list of points already considered
           const bool& GF,         ///< [in] GaloisFieldDomain?
           bool& fail              ///< [in,out] equals true, if there is no
                                   ///< valid evaluation point
          );

/// Univariate factorization of squarefree monic polys over finite fields via
/// NTL. If the characteristic is even special GF2 routines of NTL are used.
///
/// @return @a uniFactorizer returns a list of monic factors
CFList
uniFactorizer (const CanonicalForm& A, ///< [in] squarefree univariate poly
               const Variable& alpha,  ///< [in] algebraic variable
               const bool& GF          ///< [in] GaloisFieldDomain?
              );

/// naive factor recombination over an extension of the initial field.
/// Uses precomputed data to exclude combinations that are not possible.
///
/// @return @a extFactorRecombination returns a list of factors over the initial
///         field, whose shift to zero is reversed.
/// @sa factorRecombination(), extEarlyFactorDetection()
CFList
extFactorRecombination (
         CFList& factors,          ///< [in,out] list of lifted factors that are
                                   ///< monic wrt Variable (1),
                                   ///< original factors-factors found
         CanonicalForm& F,         ///< [in,out] poly to be factored,
                                   ///< F/factors found
         const CanonicalForm& M,   ///< [in] Variable (2)^liftBound
         const ExtensionInfo& info,///< [in] contains information about
                                   ///< extension
         DegreePattern& degs,
         const CanonicalForm& eval,///< [in] evaluation point
         int s,                    ///< [in] algorithm starts checking subsets
                                   ///< of size s
         int thres                 ///< [in] threshold for the size of subsets
                                   ///< which are checked, for a full factor
                                   ///< recombination choose
                                   ///< thres= factors.length()/2
                       );

/// naive factor recombination.
/// Uses precomputed data to exclude combinations that are not possible.
///
/// @return @a factorRecombination returns a list of factors of F.
/// @sa extFactorRecombination(), earlyFactorDetectection()
CFList
factorRecombination (
            CFList& factors,            ///< [in,out] list of lifted factors
                                        ///< that are monic wrt Variable (1)
            CanonicalForm& F,           ///< [in,out] poly to be factored
            const CanonicalForm& M,     ///< [in] Variable (2)^liftBound
            DegreePattern& degs,        ///< [in] degree pattern
            const CanonicalForm& eval,  ///< [in] evaluation point
            int s,                      ///< [in] algorithm starts checking
                                        ///< subsets of size s
            int thres,                  ///< [in] threshold for the size of
                                        ///< subsets which are checked, for a
                                        ///< full factor recombination choose
                                        ///< thres= factors.length()/2
            const modpk& b=modpk(),     ///< [in] coeff bound
            const CanonicalForm& den= 1 ///< [in] bound on the den if over Q (a)
                    );

/// chooses a field extension.
///
/// @return @a chooseExtension returns an extension specified by @a beta of
///         appropiate size
Variable chooseExtension (
                      const Variable & alpha, ///< [in] some algebraic variable
                      const Variable & beta,  ///< [in] some algebraic variable
                      int k                   ///< [in] some int
                         );

/// compute lifting precisions from the shape of the Newton polygon of F
///
/// @return @a getLiftPrecisions returns lifting precisions computed from the
/// shape of the Newton polygon of F
int *
getLiftPrecisions (const CanonicalForm& F, ///< [in] a bivariate poly
                   int& sizeOfOutput,      ///< [in,out] size of the output
                   int degreeLC            ///< [in] degree of the leading coeff
                                           ///< [in] of F wrt. Variable (1)
                  );


/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound and possible
/// degree pattern are updated.
///
/// @sa factorRecombination(), extEarlyFactorDetection()
void
earlyFactorDetection (
           CFList& reconstructedFactors, ///< [in,out] list of reconstructed
                                         ///< factors
           CanonicalForm& F,       ///< [in,out] poly to be factored, returns
                                   ///< poly divided by detected factors in case
                                   ///< of success
           CFList& factors,        ///< [in,out] list of factors lifted up to
                                   ///< @a deg, returns a list of factors
                                   ///< without detected factors
           int& adaptedLiftBound,  ///< [in,out] adapted lift bound
           int*& factorsFoundIndex,///< [in,out] factors already considered
           DegreePattern& degs,    ///< [in,out] degree pattern, is updated
                                   ///< whenever we find a factor
           bool& success,          ///< [in,out] indicating success
           int deg,                ///< [in] stage of Hensel lifting
           const CanonicalForm& eval, ///<[in] evaluation point
           const modpk& b= modpk()///< [in] coeff bound
                     );

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound and possible
/// degree pattern are updated.
///
/// @sa factorRecombination(), earlyFactorDetection()
void
extEarlyFactorDetection (
        CFList& reconstructedFactors, ///< [in,out] list of reconstructed
                                      ///< factors
        CanonicalForm& F,          ///< [in,out] poly to be factored, returns
                                   ///< poly divided by detected factors in case
                                   ///< of success
        CFList& factors,           ///< [in,out] list of factors lifted up to
                                   ///< @a deg, returns a list of factors
                                   ///< without detected factors
        int& adaptedLiftBound,     ///< [in,out] adapted lift bound
        int*& factorsFoundIndex,   ///< [in,out] factors already considered
        DegreePattern& degs,       ///< [in,out] degree pattern, is updated
                                   ///< whenever we find a factor
        bool& success,             ///< [in,out] indicating success
        const ExtensionInfo& info,  ///< [in] information about extension
        const CanonicalForm& eval, ///< [in] evaluation point
        int deg                    ///< [in] stage of Hensel lifting
                      );

/// hensel Lifting and early factor detection
///
/// @return @a henselLiftAndEarly returns monic (wrt Variable (1)) lifted
///         factors without factors which have been detected at an early stage
///         of Hensel lifting
/// @sa earlyFactorDetection(), extEarlyFactorDetection()

CFList
henselLiftAndEarly (
        CanonicalForm& A,          ///< [in,out] poly to be factored,
                                   ///< returns poly divided by detected factors
                                   ///< in case of success
        bool& earlySuccess,        ///< [in,out] indicating success
        CFList& earlyFactors,      ///< [in,out] list of factors detected
                                   ///< at early stage of Hensel lifting
        DegreePattern& degs,       ///< [in,out] degree pattern
        int& liftBound,            ///< [in,out] (adapted) lift bound
        const CFList& uniFactors,  ///< [in] univariate factors
        const ExtensionInfo& info, ///< [in] information about extension
        const CanonicalForm& eval, ///< [in] evaluation point
        modpk& b,                  ///< [in] coeff bound
        CanonicalForm& den         ///< [in] bound on the den if over Q(a)
                  );

/// hensel Lifting and early factor detection
///
/// @return @a henselLiftAndEarly returns monic (wrt Variable (1)) lifted
///         factors without factors which have been detected at an early stage
///         of Hensel lifting
/// @sa earlyFactorDetection(), extEarlyFactorDetection()

CFList
henselLiftAndEarly (
        CanonicalForm& A,          ///< [in,out] poly to be factored,
                                   ///< returns poly divided by detected factors
                                   ///< in case of success
        bool& earlySuccess,        ///< [in,out] indicating success
        CFList& earlyFactors,      ///< [in,out] list of factors detected
                                   ///< at early stage of Hensel lifting
        DegreePattern& degs,       ///< [in,out] degree pattern
        int& liftBound,            ///< [in,out] (adapted) lift bound
        const CFList& uniFactors,  ///< [in] univariate factors
        const ExtensionInfo& info, ///< [in] information about extension
        const CanonicalForm& eval  ///< [in] evaluation point
                  );

/// Factorization over an extension of initial field
///
/// @return @a extBiFactorize returns factorization of F over initial field
/// @sa biFactorize()
CFList
extBiFactorize (const CanonicalForm& F,    ///< [in] poly to be factored
                const ExtensionInfo& info  ///< [in] info about extension
               );

#endif
#endif
/* FAC_FQ_BIVAR_H */

