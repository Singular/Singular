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
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_BIVAR_H
#define FAC_FQ_BIVAR_H

// #include "config.h"

#include "cf_assert.h"

#include "facFqBivarUtil.h"
#include "DegreePattern.h"
#include "ExtensionInfo.h"
#include "cf_util.h"
#include "facFqSquarefree.h"
#include "cf_map.h"
#include "cfNewtonPolygon.h"

static const double log2exp= 1.442695041;

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
biFactorize (const CanonicalForm& F,       ///< [in] a bivariate poly
             const ExtensionInfo& info     ///< [in] information about extension
            );

/// factorize a squarefree bivariate polynomial over \f$ F_{p} \f$.
///
/// @return @a FpBiSqrfFactorize returns a list of monic factors, the first
///         element is the leading coefficient.
/// @sa FqBiSqrfFactorize(), GFBiSqrfFactorize()
#ifdef HAVE_NTL
inline
CFList FpBiSqrfFactorize (const CanonicalForm & G ///< [in] a bivariate poly
                         )
{
  ExtensionInfo info= ExtensionInfo (false);
  CFMap N;
  CanonicalForm F= compress (G, N);
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
  mat_ZZ M;
  vec_ZZ S;
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
  return result;
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
  CFMap N;
  CanonicalForm F= compress (G, N);
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
  mat_ZZ M;
  vec_ZZ S;
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
  return result;
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
  CFMap N;
  CanonicalForm F= compress (G, N);
  CanonicalForm contentX= content (F, 1);
  CanonicalForm contentY= content (F, 2);
  F /= (contentX*contentY);
  CFList contentXFactors, contentYFactors;
  contentXFactors= biFactorize (contentX, info);
  contentYFactors= biFactorize (contentY, info);
  if (contentXFactors.getFirst().inCoeffDomain())
    contentXFactors.removeFirst();
  if (contentYFactors.getFirst().inCoeffDomain())
    contentYFactors.removeFirst();
  if (F.inCoeffDomain())
  {
    CFList result;
    for (CFListIterator i= contentXFactors; i.hasItem(); i++)
      result.append (N (i.getItem()));
    for (CFListIterator i= contentYFactors; i.hasItem(); i++)
      result.append (N (i.getItem()));
    normalize (result);
    result.insert (Lc (G));
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  F= compress (F, M, S);
  CFList result= biFactorize (F, info);
  for (CFListIterator i= result; i.hasItem(); i++)
    i.getItem()= N (decompress (i.getItem(), M, S));
  for (CFListIterator i= contentXFactors; i.hasItem(); i++)
    result.append (N(i.getItem()));
  for (CFListIterator i= contentYFactors; i.hasItem(); i++)
    result.append (N (i.getItem()));
  normalize (result);
  result.insert (Lc(G));
  return result;
}

/// factorize a bivariate polynomial over \f$ F_{p} \f$
///
/// @return @a FpBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FqBiFactorize(), GFBiFactorize()
inline
CFFList FpBiFactorize (const CanonicalForm & G ///< [in] a bivariate poly
                      )
{
  ExtensionInfo info= ExtensionInfo (false);
  CFMap N;
  CanonicalForm F= compress (G, N);
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
  CFFList result, resultRoot;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  F= compress (F, M, S);
  CanonicalForm pthRoot, A;
  CanonicalForm sqrfP= sqrfPart (F/Lc(F), pthRoot, info.getAlpha());
  CFList buf, bufRoot;
  int p= getCharacteristic();
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, p, l);
    result= FpBiFactorize (pthRoot);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp()*ipower (p,l));
    result= Union (result, contentXFactors);
    result= Union (result, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= biFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
  }
  if (degree (A) > 0)
  {
    resultRoot= FpBiFactorize (A);
    resultRoot.removeFirst();
    for (CFFListIterator i= resultRoot; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
    result= Union (result, resultRoot);
  }
  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));
  return result;
}

/// factorize a bivariate polynomial over \f$ F_{p}(\alpha ) \f$
///
/// @return @a FqBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpBiFactorize(), FqBiFactorize()
inline
CFFList FqBiFactorize (const CanonicalForm & G, ///< [in] a bivariate poly
                       const Variable & alpha   ///< [in] algebraic variable
                      )
{
  ExtensionInfo info= ExtensionInfo (alpha, false);
  CFMap N;
  CanonicalForm F= compress (G, N);
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
  CFFList result, resultRoot;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  CanonicalForm oldF= F;
  F= compress (F, M, S);
  CanonicalForm pthRoot, A, tmp;
  CanonicalForm sqrfP= sqrfPart (F/Lc(F), pthRoot, alpha);
  CFList buf, bufRoot;
  int p= getCharacteristic();
  int q= ipower (p, degree (getMipo (alpha)));
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, q, l);
    result= FqBiFactorize (pthRoot, alpha);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp()*ipower (p,l));
    result= Union (result, contentXFactors);
    result= Union (result, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= biFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
  }
  if (degree (A) > 0)
  {
    resultRoot= FqBiFactorize (A, alpha);
    resultRoot.removeFirst();
    for (CFFListIterator i= resultRoot; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
    result= Union (result, resultRoot);
  }
  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));
  return result;
}

/// factorize a bivariate polynomial over GF
///
/// @return @a GFBiFactorize returns a list of monic factors with
///         multiplicity, the first element is the leading coefficient.
/// @sa FpBiFactorize(), FqBiFactorize()
inline
CFFList GFBiFactorize (const CanonicalForm & G ///< [in] a bivariate poly
                      )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  ExtensionInfo info= ExtensionInfo (getGFDegree(), gf_name, false);
  CFMap N;
  CanonicalForm F= compress (G, N);
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
  CFFList result, resultRoot;
  if (F.inCoeffDomain())
  {
    result= Union (contentXFactors, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  mat_ZZ M;
  vec_ZZ S;
  F= compress (F, M, S);
  CanonicalForm pthRoot, A;
  CanonicalForm sqrfP= sqrfPart (F/LcF, pthRoot, info.getAlpha());
  CFList buf;
  int p= getCharacteristic();
  int q= ipower (p, getGFDegree());
  int l;
  if (degree (pthRoot) > 0)
  {
    pthRoot= maxpthRoot (pthRoot, q, l);
    result= GFBiFactorize (pthRoot);
    result.removeFirst();
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp()*ipower (p,l));
    result= Union (result, contentXFactors);
    result= Union (result, contentYFactors);
    normalize (result);
    result.insert (CFFactor (LcF, 1));
    return result;
  }
  else
  {
    buf= biFactorize (sqrfP, info);
    A= F/LcF;
    result= multiplicity (A, buf);
    for (CFFListIterator i= result; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
  }
  if (degree (A) > 0)
  {
    resultRoot= GFBiFactorize (A);
    resultRoot.removeFirst();
    for (CFFListIterator i= resultRoot; i.hasItem(); i++)
      i.getItem()= CFFactor (N (decompress (i.getItem().factor(), M, S)),
                             i.getItem().exp());
    result= Union (result, resultRoot);
  }
  result= Union (result, contentXFactors);
  result= Union (result, contentYFactors);
  normalize (result);
  result.insert (CFFactor (LcF, 1));
  return result;
}

#endif

/// \f$ \prod_{f\in L} {f (0, x)} \ mod\ M \f$ via divide-and-conquer
///
/// @return @a prodMod0 computes the above defined product
/// @sa prodMod()
CanonicalForm prodMod0 (const CFList& L,       ///< [in] a list of compressed,
                                               ///< bivariate polynomials
                        const CanonicalForm& M ///< [in] a power of Variable (2)
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
                CFList& factors,       ///< [in,out] list of lifted factors
                                       ///< that are monic wrt Variable (1)
                CanonicalForm& F,      ///< [in,out] poly to be factored
                const CanonicalForm& M,///< [in] Variable (2)^liftBound
                DegreePattern& degs,   ///< [in] degree pattern
                int s,                 ///< [in] algorithm starts checking
                                       ///< subsets of size s
                int thres              ///< [in] threshold for the size of
                                       ///< subsets which are checked, for a
                                       ///< full factor recombination choose
                                       ///< thres= factors.length()/2
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

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound and possible
/// degree pattern are updated.
///
/// @return @a earlyFactorDetection returns a list of factors of F (possibly in-
///         complete), in case of success. Otherwise an empty list.
/// @sa factorRecombination(), extEarlyFactorDetection()
CFList
earlyFactorDetection (
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
           int deg                 ///< [in] stage of Hensel lifting
                     );

/// detects factors of @a F at stage @a deg of Hensel lifting.
/// No combinations of more than one factor are tested. Lift bound and possible
/// degree pattern are updated.
///
/// @return @a extEarlyFactorDetection returns a list of factors of F (possibly
///         incomplete), whose shift to zero is reversed, in case of success.
///         Otherwise an empty list.
/// @sa factorRecombination(), earlyFactorDetection()
CFList
extEarlyFactorDetection (
        CanonicalForm& F,          ///< [in,out] poly to be factored, returns
                                   ///< poly divided by detected factors in case
                                   ///< of success
        CFList& factors,           ///< [in,out] list of factors lifted up to
                                   ///< @a deg, returns a list of factors
                                   ///< without detected factors
        int& adaptedLiftBound,     ///< [in,out] adapted lift bound
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
/* FAC_FQ_BIVAR_H */

