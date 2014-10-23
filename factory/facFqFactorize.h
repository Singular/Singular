/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.h
 *
 * This file provides functions for factorizing a multivariate polynomial over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_FACTORIZE_H
#define FAC_FQ_FACTORIZE_H

// #include "config.h"
#include "timing.h"

#include "facFqBivar.h"
#include "DegreePattern.h"
#include "ExtensionInfo.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "facFqBivarUtil.h"


TIMING_DEFINE_PRINT (fac_fq_squarefree)
TIMING_DEFINE_PRINT (fac_fq_factor_squarefree)

/// Factorization over a finite field
///
/// @return @a multiFactorize returns a factorization of F
/// @sa biFactorize(), extFactorize()
CFList
multiFactorize (const CanonicalForm& F,    ///< [in] sqrfree poly
                const ExtensionInfo& info  ///< [in] info about extension
               );

/// factorize a squarefree multivariate polynomial over \f$ F_{p} \f$
///
/// @return @a FpSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FqSqrfFactorize(), GFSqrfFactorize()
#ifdef HAVE_NTL
inline
CFList FpSqrfFactorize (const CanonicalForm & F ///< [in] a multivariate poly
                       )
{
  if (getNumVars (F) == 2)
    return FpBiSqrfFactorize (F);
  ExtensionInfo info= ExtensionInfo (false);
  CFList result= multiFactorize (F, info);
  result.insert (Lc(F));
  return result;
}

/// factorize a squarefree multivariate polynomial over \f$ F_{p} (\alpha ) \f$
///
/// @return @a FqSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FpSqrfFactorize(), GFSqrfFactorize()
inline
CFList FqSqrfFactorize (const CanonicalForm & F, ///< [in] a multivariate poly
                        const Variable& alpha    ///< [in] algebraic variable
                       )
{
  if (getNumVars (F) == 2)
    return FqBiSqrfFactorize (F, alpha);
  ExtensionInfo info= ExtensionInfo (alpha, false);
  CFList result= multiFactorize (F, info);
  result.insert (Lc(F));
  return result;
}

/// factorize a squarefree multivariate polynomial over GF
///
/// @return @a GFSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FpSqrfFactorize(), FqSqrfFactorize()
inline
CFList GFSqrfFactorize (const CanonicalForm & F ///< [in] a multivariate poly
                       )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  if (getNumVars (F) == 2)
    return GFBiSqrfFactorize (F);
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  CFList result= multiFactorize (F, info);
  result.insert (Lc(F));
  return result;
}

/// factorize a multivariate polynomial over \f$ F_{p} \f$
///
/// @return @a FpFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FqFactorize(), GFFactorize()
inline
CFFList FpFactorize (const CanonicalForm& G,///< [in] a multivariate poly
                     bool substCheck= true  ///< [in] enables substitute check
                    )
{
  if (getNumVars (G) == 2)
    return FpBiFactorize (G, substCheck);

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
      CFFList result= FpFactorize (F, false);
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
        tmp= FpFactorize (tmp2, false);
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

  ExtensionInfo info= ExtensionInfo (false);
  Variable a= Variable (1);
  CanonicalForm LcF= Lc (F);
  TIMING_START (fac_fq_squarefree);
  CFFList sqrf= FpSqrf (F, false);
  TIMING_END_AND_PRINT (fac_fq_squarefree,
                        "time for squarefree factorization over Fq: ");
  CFFList result;
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    TIMING_START (fac_fq_factor_squarefree);
    bufResult= multiFactorize (iter.getItem().factor(), info);
    TIMING_END_AND_PRINT (fac_fq_factor_squarefree,
                          "time to factorize sqrfree factor over Fq: ");
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (i.getItem(), iter.getItem().exp()));
  }
  result.insert (CFFactor (LcF, 1));
  return result;
}

/// factorize a multivariate polynomial over \f$ F_{p} (\alpha ) \f$
///
/// @return @a FqFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpFactorize(), GFFactorize()
inline
CFFList FqFactorize (const CanonicalForm& G, ///< [in] a multivariate poly
                     const Variable& alpha,  ///< [in] algebraic variable
                     bool substCheck= true   ///< [in] enables substitute check
                    )
{
  if (getNumVars (G) == 2)
    return FqBiFactorize (G, alpha, substCheck);

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
      CFFList result= FqFactorize (F, alpha, false);
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
        tmp= FqFactorize (tmp2, alpha, false);
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

  ExtensionInfo info= ExtensionInfo (alpha, false);
  CanonicalForm LcF= Lc (F);
  TIMING_START (fac_fq_squarefree);
  CFFList sqrf= FqSqrf (F, alpha, false);
  TIMING_END_AND_PRINT (fac_fq_squarefree,
                        "time for squarefree factorization over Fq: ");
  CFFList result;
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    TIMING_START (fac_fq_factor_squarefree);
    bufResult= multiFactorize (iter.getItem().factor(), info);
    TIMING_END_AND_PRINT (fac_fq_factor_squarefree,
                          "time to factorize sqrfree factor over Fq: ");
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (i.getItem(), iter.getItem().exp()));
  }
  result.insert (CFFactor (LcF, 1));
  return result;
}

/// factorize a multivariate polynomial over GF
///
/// @return @a GFFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpFactorize(), FqFactorize()
inline
CFFList GFFactorize (const CanonicalForm& G, ///< [in] a multivariate poly
                     bool substCheck= true   ///< [in] enables substitute check
                    )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  if (getNumVars (G) == 2)
    return GFBiFactorize (G, substCheck);

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
      CFFList result= GFFactorize (F, false);
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
        tmp= GFFactorize (tmp2, false);
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

  Variable a= Variable (1);
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  CanonicalForm LcF= Lc (F);
  CFFList sqrf= GFSqrf (F, false);
  CFFList result;
  CFList bufResult;
  sqrf.removeFirst();
  CFListIterator i;
  for (CFFListIterator iter= sqrf; iter.hasItem(); iter++)
  {
    bufResult= multiFactorize (iter.getItem().factor(), info);
    for (i= bufResult; i.hasItem(); i++)
      result.append (CFFactor (i.getItem(), iter.getItem().exp()));
  }
  result.insert (CFFactor (LcF, 1));
  return result;
}

#endif

/// Naive factor recombination for multivariate factorization over an extension
/// of the initial field. No precomputed is used to exclude combinations.
///
/// @return @a extFactorRecombination returns a list of factors of @a F, whose
///         shift to zero is reversed.
/// @sa factorRecombination()
CFList
extFactorRecombination (
                 const CFList& factors,     ///< [in] list of lifted factors
                                            ///< that are monic wrt Variable (1)
                 const CanonicalForm& F,    ///< [in] poly to be factored
                 const CFList& M,           ///< [in] a list of powers of
                                            ///< Variables
                 const ExtensionInfo& info, ///< [in] info about extension
                 const CFList& evaluation   ///< [in] evaluation point
                       );

/// Naive factor recombination for multivariate factorization.
/// No precomputed is used to exclude combinations.
///
/// @return @a factorRecombination returns a list of factors of @a F
/// @sa extFactorRecombination()
CFList
factorRecombination (const CanonicalForm& F,///< [in] poly to be factored
                     const CFList& factors, ///< [in] list of lifted factors
                                            ///< that are monic wrt Variable (1)
                     const CFList& M        ///< [in] a list of powers of
                                            ///< Variables
                    );

/// recombination of bivariate factors @a factors1 s. t. the result evaluated
/// at @a evalPoint coincides with @a factors2
CFList
recombination (const CFList& factors1,        ///<[in] list of bivariate factors
               const CFList& factors2,        ///<[in] list univariate factors
               int s,                         ///<[in] algorithm starts checking
                                              ///<  subsets of size s
               int thres,                     ///<[in] threshold for the size of
                                              ///<  subsets which are checked
               const CanonicalForm& evalPoint,///<[in] evaluation point
               const Variable& x              ///<[in] second variable of
                                              ///< bivariate factors
              );

/// Lift bound adaption. Essentially an early factor detection but only the lift
/// bound is adapted.
///
/// @return @a liftBoundAdaption returns an adapted lift bound.
/// @sa earlyFactorDetect(), earlyFactorDetection()
int
liftBoundAdaption (const CanonicalForm& F, ///< [in] a poly
                   const CFList& factors,  ///< [in] list of list of lifted
                                           ///< factors that are monic wrt
                                           ///< Variable (1)
                   bool& success,          ///< [in,out] indicates that no
                                           ///< further lifting is necessary
                   const int deg,          ///< [in] stage of Hensel lifting
                   const CFList& MOD,      ///< [in] a list of powers of
                                           ///< Variables
                   const int bound         ///< [in] initial lift bound
                  );

/// Lift bound adaption over an extension of the initial field. Essentially an
///early factor detection but only the lift bound is adapted.
///
/// @return @a liftBoundAdaption returns an adapted lift bound.
/// @sa earlyFactorDetect(), earlyFactorDetection()
int
extLiftBoundAdaption (
            const CanonicalForm& F,    ///< [in] a poly
            const CFList& factors,     ///< [in] list of list of lifted
                                       ///< factors that are monic wrt
            bool& success,             ///< [in,out] indicates that no further
                                       ///< lifting is necessary
            const ExtensionInfo& info, ///< [in] info about extension
            const CFList& eval,        ///< [in] evaluation point
            const int deg,             ///< [in] stage of Hensel lifting
            const CFList& MOD,         ///< [in] a list of powers of
                                       ///< Variables
            const int bound            ///< [in] initial lift bound
                     );

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound is adapted.
///
/// @return @a earlyFactorDetect returns a list of factors of F (possibly
///         incomplete), in case of success. Otherwise an empty list.
/// @sa factorRecombination(), extEarlyFactorDetect()
CFList
earlyFactorDetect (
                CanonicalForm& F,      ///< [in,out] poly to be factored,
                                       ///< returns poly divided by detected
                                       ///< factors in case of success
                CFList& factors,       ///< [in,out] list of factors lifted up
                                       ///< to @a deg, returns a list of factors
                                       ///< without detected factors
                int& adaptedLiftBound, ///< [in,out] adapted lift bound
                bool& success,         ///< [in,out] indicating success
                const int deg,         ///< [in] stage of Hensel lifting
                const CFList& MOD,     ///< [in] a list of powers of
                                       ///< Variables
                const int bound        ///< [in] initial lift bound
                  );

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound is adapted.
///
/// @return @a extEarlyFactorDetect returns a list of factors of F (possibly
///         incomplete), whose shift to zero is reversed, in case of success.
///         Otherwise an empty list.
/// @sa factorRecombination(), earlyFactorDetection()
CFList
extEarlyFactorDetect (
            CanonicalForm& F,          ///< [in,out] poly to be factored,
                                       ///< returns poly divided by detected
                                       ///< factors in case of success
            CFList& factors,           ///< [in,out] list of factors lifted up
                                       ///< to @a deg, returns a list of factors
                                       ///< without detected factors
            int& adaptedLiftBound,     ///< [in,out] adapted lift bound
            bool& success,             ///< [in,out] indicating succes
            const ExtensionInfo& info, ///< [in] info about extension
            const CFList& eval,        ///< [in] evaluation point
            const int deg,             ///< [in] stage of Hensel lifting
            const CFList& MOD,         ///< [in] a list of powers of Variables
            const int bound            ///< [in] initial lift bound
                     );

/// evaluation point search for multivariate factorization,
/// looks for a (F.level() - 1)-tuple such that the resulting univariate
/// polynomial has main variable Variable (1), is squarefree and its degree
/// coincides with degree(F) and the bivariate one is primitive wrt.
/// Variable(1), and successively evaluated polynomials have the same degree in
/// their main variable as F has, fails if there are no valid evaluation points,
/// eval contains the intermediate evaluated polynomials.
///
/// @return @a evalPoints returns an evaluation point, which is valid if and
///         only if fail == false.
CFList
evalPoints (const CanonicalForm& F,  ///< [in] a compressed poly
            CFList & eval,           ///< [in,out] an empty list, returns @a F
                                     ///< successive evaluated
            const Variable& alpha,   ///< [in] algebraic variable
            CFList& list,            ///< [in,out] a list of points already
                                     ///< considered, a point is encoded as a
                                     ///< poly of degree F.level()-1 in
                                     ///< Variable(1)
            const bool& GF,          ///< [in] GF?
            bool& fail               ///< [in,out] indicates failure
           );

/// hensel Lifting and early factor detection
///
/// @return @a henselLiftAndEarly returns monic (wrt Variable (1)) lifted
///         factors without factors which have been detected at an early stage
///         of Hensel lifting
/// @sa earlyFactorDetectn(), extEarlyFactorDetect()
CFList
henselLiftAndEarly (
            CanonicalForm& A,         ///< [in,out] poly to be factored,
                                      ///< returns poly divided by detected
                                      ///< factors, in case of success
            CFList& MOD,              ///< [in,out] a list of powers of
                                      ///< Variables
            int*& liftBounds,         ///< [in,out] initial lift bounds, returns
                                      ///< adapted lift bounds
            bool& earlySuccess,       ///< [in,out] indicating success
            CFList& earlyFactors,     ///< [in,out] early factors
            const CFList& Aeval,      ///< [in] @a A successively evaluated at
                                      ///< elements of @a evaluation
            const CFList& biFactors,  ///< [in] bivariate factors
            const CFList& evaluation, ///< [in] evaluation point
            const ExtensionInfo& info ///< [in] info about extension
                   );

/// Factorization over an extension of initial field
///
/// @return @a extFactorize returns factorization of F over initial field
/// @sa extBiFactorize(), multiFactorize()
CFList
extFactorize (const CanonicalForm& F,   ///< [in] poly to be factored
              const ExtensionInfo& info ///< [in] info about extension
             );

/// compute the LCM of the contents of @a A wrt to each variable occuring in @a
/// A.
///
/// @return @a lcmContent returns the LCM of the contents of @a A wrt to each
///         variable occuring in @a A.
CanonicalForm
lcmContent (const CanonicalForm& A, ///< [in] a compressed multivariate poly
            CFList& contentAi       ///< [in,out] an empty list, returns a list
                                    ///< of the contents of @a A wrt to each
                                    ///< variable occuring in @a A starting from
                                    ///< @a A.mvar().
           );

/// compress a polynomial s.t. \f$ deg_{x_{i}} (F) >= deg_{x_{i+1}} (F) \f$ and
/// no gaps between the variables occur
///
/// @return a compressed poly with the above properties
CanonicalForm myCompress (const CanonicalForm& F, ///< [in] a poly
                          CFMap& N                ///< [in,out] a map to
                                                  ///< decompress
                         );

/// evaluate a poly A with main variable at level 1 at an evaluation point in
/// K^(n-1) wrt different second variables. If this evaluation is valid (see
/// evalPoints) then Aeval contains A successively evaluated at this point,
/// otherwise this entry is empty
void
evaluationWRTDifferentSecondVars (
                    CFList*& Aeval,          ///<[in,out] an array of length n-2
                                             ///< if variable at level i > 2
                                             ///< admits a valid evaluation
                                             ///< this entry contains A
                                             ///< successively evaluated at this
                                             ///< point otherwise an empty list
                    const CFList& evaluation,///<[in] a valid evaluation point
                                             ///< for main variable at level 1
                                             ///< and second variable at level 2
                    const CanonicalForm& A   ///<[in] some poly
                                 );

/// refine a bivariate factorization of A with l factors to one with
/// minFactorsLength if possible
void
refineBiFactors (const CanonicalForm& A,  ///< [in] some poly
                 CFList& biFactors,       ///< [in,out] list of bivariate to be
                                          ///< refined, returns refined factors
                 CFList* const& factors,  ///< [in] list of bivariate
                                          ///< factorizations of A wrt different
                                          ///< second variables
                 const CFList& evaluation,///< [in] the evaluation point
                 int minFactorsLength     ///< [in] the minimal number of factors
                );

/// plug in evalPoint for y in a list of polys
///
/// @return returns a list of the evaluated polys, these evaluated polys are
/// made monic
CFList
buildUniFactors (const CFList& biFactors,       ///< [in] a list of polys
                 const CanonicalForm& evalPoint,///< [in] some evaluation point
                 const Variable& y              ///< [in] some variable
                );


/// sort bivariate factors in Aeval such that their corresponding univariate
/// factors coincide with uniFactors, uniFactors and biFactors may get
/// recombined if necessary
void sortByUniFactors (CFList*& Aeval,          ///< [in,out] array of bivariate
                                                ///< factors
                       int AevalLength,         ///< [in] length of Aeval
                       CFList& uniFactors,      ///< [in,out] univariate factors
                       CFList& biFactors,       ///< [in,out] bivariate factors
                       const CFList& evaluation ///< [in] evaluation point
                      );

/// extract leading coefficients wrt Variable(1) from bivariate factors obtained
/// from factorizations of A wrt different second variables
void
getLeadingCoeffs (const CanonicalForm& A,  ///< [in] some poly
                  CFList*& Aeval           ///< [in,out] array of bivariate
                                           ///< factors, returns the leading
                                           ///< coefficients of these factors
                 );

/// normalize precomputed leading coefficients such that leading coefficients
/// evaluated at @a evaluation in K^(n-2) equal the leading coeffs wrt
/// Variable(1) of bivariate factors and change @a A and @a Aeval accordingly
void
prepareLeadingCoeffs (CFList*& LCs,               ///<[in,out]
                      CanonicalForm& A,           ///<[in,out]
                      CFList& Aeval,              ///<[in,out]
                      int n,                      ///<[in] level of poly to be
                                                  ///< factored
                      const CFList& leadingCoeffs,///<[in] precomputed leading
                                                  ///< coeffs
                      const CFList& biFactors,    ///<[in] bivariate factors
                      const CFList& evaluation    ///<[in] evaluation point
                     );

/// obtain factors of F by reconstructing their leading coeffs
///
/// @return returns the reconstructed factors
/// @sa factorRecombination()
CFList
leadingCoeffReconstruction (const CanonicalForm& F,///<[in] poly to be factored
                            const CFList& factors, ///<[in] factors of f monic
                                                   ///< wrt Variable (1)
                            const CFList& M        ///<[in] a list of powers of
                                                   ///< Variables
                           );

/// distribute content
///
/// @return returns a list result of polys such that prod (result)= prod (L)
/// but the first entry of L may be (partially) factorized and these factors
/// are distributed onto other entries in L
CFList
distributeContent (
          const CFList& L,                        ///<[in] list of polys, first
                                                  ///< entry the content to be
                                                  ///< distributed
          const CFList* differentSecondVarFactors,///<[in] factorization wrt
                                                  ///< different second vars
          int length                              ///<[in] length of
                                                  ///<differentSecondVarFactors
                  );

/// gcd free basis of two lists of factors
void
gcdFreeBasis (CFFList& factors1, ///< [in,out] list of factors, returns gcd free
                                 ///< factors
              CFFList& factors2  ///< [in,out] list of factors, returns gcd free
                                 ///< factors
             );

/// computes a list l of length length(LCFFactors)+1 of polynomials such that
/// prod (l)=LCF, note that the first entry of l may be non constant. Intended
/// to be used to precompute coefficients of a polynomial f from its bivariate
/// factorizations.
///
/// @return see above
CFList
precomputeLeadingCoeff (const CanonicalForm& LCF,       ///<[in] a multivariate
                                                        ///< poly
                        const CFList& LCFFactors,       ///<[in] a list of
                                                        ///< univariate factors
                                                        ///< of LCF of level 2
                        const Variable& alpha,          ///<[in] algebraic var.
                        const CFList& evaluation,       ///<[in] an evaluation
                                                        ///< point having
                                                        ///< lSecondVarLCs+1
                                                        ///< components
                        CFList* & differentSecondVarLCs,///<[in] LCs of factors
                                                        ///< of f wrt different
                                                        ///< second variables
                        int lSecondVarLCs,              ///<[in] length of the
                                                        ///< above
                        Variable& y                     ///<[in,out] if y.level()
                                                        ///< is not 1 on output
                                                        ///< the second variable
                                                        ///< has been changed to
                                                        ///< y
                       );

/// changes the second variable to be @a w and updates all relevant data
void
changeSecondVariable (CanonicalForm& A,        ///<[in,out] a multivariate poly
                      CFList& biFactors,       ///<[in,out] bivariate factors
                      CFList& evaluation,      ///<[in,out] evaluation point
                      CFList*& oldAeval,       ///<[in,out] old bivariate factors
                                               ///< wrt. different second vars
                      int lengthAeval2,        ///<[in] length of oldAeval
                      const CFList& uniFactors,///<[in] univariate factors
                      const Variable& w        ///<[in] some variable
                     );

/// distributes a divisor LCmultiplier of LC(A,1) on the bivariate factors and
/// the precomputed leading coefficients
void
distributeLCmultiplier (CanonicalForm& A,               ///<[in,out] some poly
                        CFList& leadingCoeffs,          ///<[in,out] leading
                                                        ///< coefficients
                        CFList& biFactors,              ///<[in,out] bivariate
                                                        ///< factors
                        const CFList& evaluation,       ///<[in] eval. point
                        const CanonicalForm& LCmultipler///<[in] multiplier
                       );

/// heuristic to distribute @a LCmultiplier onto factors based on the variables
/// that occur in @a LCmultiplier and in the leading coeffs of bivariate factors
void
LCHeuristic (CanonicalForm& A,                 ///<[in,out] a poly
             const CanonicalForm& LCmultiplier,///<[in,out] divisor of LC (A,1)
             CFList& biFactors,                ///<[in,out] bivariate factors
             CFList*& leadingCoeffs,           ///<[in,out] leading coeffs
             const CFList* oldAeval,           ///<[in] bivariate factors wrt.
                                               ///< different second vars
             int lengthAeval,                  ///<[in] length of oldAeval
             const CFList& evaluation,         ///<[in] evaluation point
             const CFList& oldBiFactors        ///<[in] bivariate factors
                                               ///< without LCmultiplier
                                               ///< distributed on them
            );

/// checks if prod(LCs)==LC (oldA,1) and if so divides elements of leadingCoeffs
/// by elements in contents, sets A to oldA and sets foundTrueMultiplier to true
void
LCHeuristicCheck (const CFList& LCs,        ///<[in] leading coeffs computed
                  const CFList& contents,   ///<[in] content of factors
                  CanonicalForm& A,         ///<[in,out] oldA*LCmultiplier^m
                  const CanonicalForm& oldA,///<[in] some poly
                  CFList& leadingCoeffs,    ///<[in,out] leading coefficients
                  bool& foundTrueMultiplier ///<[in,out] success?
                 );

/// heuristic to distribute @a LCmultiplier onto factors based on the contents
/// of @a factors. @a factors are assumed to come from LucksWangSparseHeuristic.
/// If not successful @a contents will contain the content of each element of @a
/// factors and @a LCs will contain the LC of each element of @a factors divided
/// by its content
void
LCHeuristic2 (const CanonicalForm& LCmultiplier,///<[in] divisor of LC (A,1)
              const CFList& factors,            ///<[in] result of
                                                ///< LucksWangSparseHeuristic
              CFList& leadingCoeffs,            ///<[in,out] leading coeffs
              CFList& contents,                 ///<[in,out] content of factors
              CFList& LCs,                      ///<[in,out] LC of factors
                                                ///< divided by content of
                                                ///< factors
              bool& foundTrueMultiplier         ///<[in,out] success?
             );

/// heuristic to remove @a LCmultiplier from a factor based on the contents
/// of @a factors. @a factors are assumed to come from LucksWangSparseHeuristic.
void
LCHeuristic3 (const CanonicalForm& LCmultiplier,///<[in] divisor of LC (A,1)
              const CFList& factors,            ///<[in] result of
                                                ///< LucksWangSparseHeuristic
              const CFList& oldBiFactors,       ///<[in] bivariate factors
                                                ///< without LCmultiplier
                                                ///< distributed on them
              const CFList& contents,           ///<[in] content of factors
              const CFList* oldAeval,           ///<[in] bivariate factors wrt.
                                                ///< different second vars
              CanonicalForm& A,                 ///<[in,out] poly
              CFList*& leadingCoeffs,           ///<[in,out] leading coeffs
              int lengthAeval,                  ///<[in] length of oldAeval
              bool& foundMultiplier             ///<[in,out] success?
             );

/// heuristic to remove factors of @a LCmultiplier from @a factors.
/// More precisely checks if elements of @a contents divide @a LCmultiplier.
/// Assumes LCHeuristic3 is run before it and was successful.
void
LCHeuristic4 (const CFList& oldBiFactors,   ///<[in] bivariate factors
                                            ///< without LCmultiplier
                                            ///< distributed on them
              const CFList* oldAeval,       ///<[in] bivariate factors wrt.
                                            ///< different second vars
              const CFList& contents,       ///<[in] content of factors
              const CFList& factors,        ///<[in] result of
                                            ///< LucksWangSparseHeuristic
              const CanonicalForm& testVars,///<[in] product of second vars that
                                            ///< occur among oldAeval
              int lengthAeval,              ///<[in] length of oldAeval
              CFList*& leadingCoeffs,       ///<[in,out] leading coeffs
              CanonicalForm& A,             ///<[in,out] poly
              CanonicalForm& LCmultiplier,  ///<[in,out] divisor of LC (A,1)
              bool& foundMultiplier         ///<[in] success?
             );

#endif
/* FAC_FQ_FACTORIZE_H */
