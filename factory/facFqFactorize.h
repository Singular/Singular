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
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_FACTORIZE_H
#define FAC_FQ_FACTORIZE_H

// #include "config.h"

#include "facFqBivar.h"
#include "DegreePattern.h"
#include "ExtensionInfo.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "facFqBivarUtil.h"

/// Factorization over a finite field
///
/// @return @a multiFactorize returns a factorization of F
/// @sa biFactorize(), extFactorize()
CFList
multiFactorize (const CanonicalForm& F,    ///< [in] poly to be factored
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
CFFList FpFactorize (const CanonicalForm& F ///< [in] a multivariate poly
                    )
{
  if (getNumVars (F) == 2)
    return FpBiFactorize (F);
  ExtensionInfo info= ExtensionInfo (false);
  Variable a= Variable (1);
  CanonicalForm LcF= Lc (F);
  CanonicalForm pthRoot, A;
  CanonicalForm sqrfP= sqrfPart (F/Lc(F), pthRoot, a);
  CFList buf, bufRoot;
  CFFList result, resultRoot;
  int p= getCharacteristic();
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, p, l);
    result= FpFactorize (pthRoot);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor(i.getItem().factor(),i.getItem().exp()*ipower(p,l));
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= multiFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
  }
  if (degree (A) > 0)
  {
    resultRoot= FpFactorize (A);
    resultRoot.removeFirst();
    result= Union (result, resultRoot);
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
CFFList FqFactorize (const CanonicalForm& F, ///< [in] a multivariate poly
                     const Variable& alpha   ///< [in] algebraic variable
                    )
{
  if (getNumVars (F) == 2)
    return FqBiFactorize (F, alpha);
  ExtensionInfo info= ExtensionInfo (alpha, false);
  CanonicalForm LcF= Lc (F);
  CanonicalForm pthRoot, A;
  CanonicalForm sqrfP= sqrfPart (F/Lc(F), pthRoot, alpha);
  CFList buf, bufRoot;
  CFFList result, resultRoot;
  int p= getCharacteristic();
  int q= ipower (p, degree (getMipo (alpha)));
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, q, l);
    result= FqFactorize (pthRoot, alpha);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor(i.getItem().factor(),i.getItem().exp()*ipower(p,l));
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= multiFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
  }
  if (degree (A) > 0)
  {
    resultRoot= FqFactorize (A, alpha);
    resultRoot.removeFirst();
    result= Union (result, resultRoot);
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
CFFList GFFactorize (const CanonicalForm& F ///< [in] a multivariate poly
                    )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  if (getNumVars (F) == 2)
    return GFBiFactorize (F);
  Variable a= Variable (1);
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  CanonicalForm LcF= Lc (F);
  CanonicalForm pthRoot, A;
  CanonicalForm sqrfP= sqrfPart (F/LcF, pthRoot, a);
  CFList buf;
  CFFList result, resultRoot;
  int p= getCharacteristic();
  int q= ipower (p, getGFDegree());
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, q, l);
    result= GFFactorize (pthRoot);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor(i.getItem().factor(),i.getItem().exp()*ipower(p,l));
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= multiFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
  }
  if (degree (A) > 0)
  {
    resultRoot= GFFactorize (A);
    resultRoot.removeFirst();
    result= Union (result, resultRoot);
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

/// evaluate F successively n-2 at 0
///
/// @return returns a list of successive evaluations of F, ending with F
CFList evaluateAtZero (const CanonicalForm& F ///< [in] some poly
                      );

/// divides factors by their content wrt. Variable(1) and checks if these polys
/// divide F
///
/// @return returns factors of F
CFList recoverFactors (const CanonicalForm& F, ///< [in] some poly F
                       const CFList& factors   ///< [in] some list of
                                               ///< factor candidates
                      );

/// refine a bivariate factorization of A with l factors to one with
/// minFactorsLength
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

/// extract leading coefficients wrt Variable(1) from bivariate factors obtained
/// from factorizations of A wrt different second variables
void
getLeadingCoeffs (const CanonicalForm& A,  ///< [in] some poly
                  CFList*& Aeval,          ///< [in,out] array of bivariate
                                           ///< factors, returns the leading
                                           ///< coefficients of these factors
                  const CFList& uniFactors,///< [in] univariate factors of A
                  const CFList& evaluation ///< [in] evaluation point
                 );

/// normalize precomputed leading coefficients such that leading coefficients
/// evaluated at 0 in K^(n-2) equal the leading coeffs wrt Variable(1) of 
/// bivariate factors
void
prepareLeadingCoeffs (CFList*& LCs,               ///<[in,out] 
                      int n,                      ///<[in] level of poly to be
                                                  ///< factored
                      const CFList& leadingCoeffs,///<[in] precomputed leading
                                                  ///< coeffs
                      const CFList& biFactors     ///<[in] bivariate factors
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

#endif
/* FAC_FQ_FACTORIZE_H */

