/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.h
 *
 * This file provides functions for factorizing a multivariate polynomial over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF.
 *
 * ABSTRACT: So far factor recombination is done naive!
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_FACTORIZE_H
#define FAC_FQ_FACTORIZE_H

#include <config.h>

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
inline
CFList FpSqrfFactorize (const CanonicalForm & F ///< [in] a multivariate poly
                       )
{
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

/// compute the content of @a F wrt Variable (1) using routines from
/// @a cf_gcd_smallp.h
///
/// @return @a myContent returns the content of F wrt Variable (1)
static inline
CanonicalForm
myContent (const CanonicalForm& F ///< [in] a poly
          );

/// compute the content of @a F wrt @a x using routines from
/// @a cf_gcd_smallp.h
///
/// @return @a myContent returns the content of F wrt x
static inline
CanonicalForm
myContent (const CanonicalForm& F, ///< [in] a poly
           const Variable& x       ///< [in] a variable
          );

/// compute the GCD of all element in @a L using routines from
/// @a cf_gcd_smallp.h
///
/// @return @a listGCD returns the GCD of all elements in @a L
static inline
CanonicalForm
listGCD (const CFList& L ///< [in] a list of polys
        );

/// compute the LCM of @a F and @a G using routines from
/// @a cf_gcd_smallp.h
///
/// @return @a myLcm returns the LCM of @a F and @a G
static inline
CanonicalForm
myLcm (const CanonicalForm& F,   ///< [in] a poly
       const CanonicalForm& G    ///< [in] a poly
      );

/// compute the LCM of the contents of @a A wrt to each variable occuring in @a
/// A.
///
/// @return @a lcmContent returns the LCM of the contents of @a A wrt to each
///         variable occuring in @a A.
static inline
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
static inline
CanonicalForm myCompress (const CanonicalForm& F, ///< [in] a poly
                          CFMap& N                ///< [in,out] a map to
                                                  ///< decompress
                         );

/// naive factor recombination for bivariate factorization.
/// Uses precomputed data to exclude combinations that are not possible.
///
/// @return @a monicFactorRecombi returns a list of factors of F, that are
///         monic wrt Variable (1).
/// @sa extFactorRecombination(), factorRecombination(), biFactorizer()
CFList
monicFactorRecombi (
                  const CFList& factors,    ///< [in] list of lifted factors
                                            ///< that are monic wrt Variable (1)
                  const CanonicalForm& F,   ///< [in] bivariate poly
                  const CanonicalForm& M,   ///< [in] Variable (2)^liftBound
                  const DegreePattern& degs ///< [in] degree pattern
                   );

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound and possible
/// degree pattern are updated.
///
/// @return @a earlyMonicFactorDetect returns a list of factors of F (possibly
///         incomplete) that are monic wrt Variable (1)
/// @sa monicFactorRecombi(), earlyFactorDetection(), monicFactorDetect(),
///     biFactorizer()
CFList
earlyMonicFactorDetect (
           CanonicalForm& F,       ///< [in,out] poly to be factored, returns
                                   ///< poly divided by detected factors in case
                                   ///< of success
           CFList& factors,        ///< [in,out] list of factors lifted up to
                                   ///< @a deg, returns a list of factors
                                   ///< without detected factors
           int& adaptedLiftBound,  ///< [in,out] adapted lift bound
           DegreePattern& degs,    ///< [in,out] degree pattern, is updated
                                   ///< whenever we find a factor
           bool& success,          ///< [in,out] indicating success
           int deg,                ///< [in] stage of Hensel lifting
           const int bound         ///< [in] degree (A, 2) + 1 +
                                   ///< degree (LC (A, 1), 2), where A is the
                                   ///< multivariate polynomial corresponding to
                                   ///< F.
                       );

/// Bivariate factorization. In contrast to biFactorize() the factors returned
/// are monic wrt Variable (1), if @a F is not irreducible. And no factorization
/// wrt Variable (2) are performed. However,
///
/// @return @a biFactorizer returns a list of factors that are monic wrt
///         Variable (1), if @a F is irreducible @a F is returned
/// @sa multiFactorize(), biFactorize()
CFList biFactorizer (const CanonicalForm& F,   ///< [in] a bivariate poly
                     const Variable& alpha,    ///< [in] algebraic variable
                     CanonicalForm& bivarEval, ///< [in,out] a valid evaluation
                                               ///< point
                     int& liftBound            ///< [in,out] lift bound, may be
                                               ///< adapted during Hensel
                                               ///< lifting
                    );

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
/// polynomial has main variable F.mvar(), is squarefree and its degree
/// coincides with degree(F) and the bivariate one is primitive wrt.
/// Variable(1), fails if there are no valid evaluation points, eval contains
/// the intermediate evaluated polynomials.
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

/// looks for a new main variable of level higher than lev which omitts a valid
/// evaluation, and returns its level. If there is no such variable 0 is
/// returned
///
/// @return @a newMainVariableSearch returns the level of the new main variable,
///         if there no variable which omitts a valid evaluation 0 is returned
static inline
int newMainVariableSearch (
              CanonicalForm& A,     ///< [in,out] a compressed poly, returns
                                    ///< the swapped initial poly or the
                                    ///< initial poly if 0 is returned
              CFList& Aeval,        ///< [in,out] @a A successively evaluated,
                                    ///< empty list if 0 is returned
              CFList& evaluation,   ///< [in,out] evaluation point, empty list
                                    ///< if 0 is returned
              const Variable& alpha,///< [in] algebraic variable
              const int lev         ///< [in] some int <= A.level()
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

#endif
/* FAC_FQ_FACTORIZE_H */

