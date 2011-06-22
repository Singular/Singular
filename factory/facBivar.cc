/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facBivar.cc
 *
 * bivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "facFqBivar.h"
#include "facBivar.h"

TIMING_DEFINE_PRINT(fac_uni_factorizer)
TIMING_DEFINE_PRINT(fac_hensel_lift)
TIMING_DEFINE_PRINT(fac_factor_recombination)

CFList conv (const CFFList& L)
{
  CFList result;
  for (CFFListIterator i= L; i.hasItem(); i++)
    result.append (i.getItem().factor());
  return result;
}

bool testPoint (const CanonicalForm& F, CanonicalForm& G, int i)
{
  G= F (i, 2);
  if (G.inCoeffDomain() || degree (F, 1) > degree (G, 1))
    return false;

  if (degree (gcd (deriv (G, G.mvar()), G)) > 0)
    return false;
  return true;
}

CanonicalForm evalPoint (const CanonicalForm& F, int& i)
{
  Variable x= Variable (1);
  Variable y= Variable (2);
  CanonicalForm result;

  int k;

  if (i == 0)
  {
    if (testPoint (F, result, i))
      return result;
  }
  do
  {
    if (i > 0)
      k= 1;
    else
      k= 2;
    while (k < 3)
    {
      if (k == 1)
      {
        if (testPoint (F, result, i))
          return result;
      }
      else
      {
        if (testPoint (F, result, -i))
          return result;
      }
      k++;
    }
    i++;
  } while (1);
}

CFList biFactorize (const CanonicalForm& F, const Variable& v)
{
  if (F.inCoeffDomain())
    return CFList(F);

  bool extension= (v.level() != 1);
  CanonicalForm A;
  CanonicalForm multiplier= 1;
  if (isOn (SW_RATIONAL))
    A= F*bCommonDen (F);
  else
    A= F;

  if (A.isUnivariate())
  {
    CFFList buf;
    if (extension)
      buf= factorize (A, v);
    else
      buf= factorize (A, true);
    CFList result= conv (buf);
    if (result.getFirst().inCoeffDomain())
      result.removeFirst();
    return result;
  }

  CFMap N;
  A= compress (A, N);
  Variable y= A.mvar();

  if (y.level() > 2) return CFList (F);
  Variable x= Variable (1);

  CanonicalForm contentAx= content (A, x);
  CanonicalForm contentAy= content (A);

  A= A/(contentAx*contentAy);
  CFFList contentAxFactors, contentAyFactors;

  if (extension)
  {
    if (!contentAx.inCoeffDomain())
    {
      contentAxFactors= factorize (contentAx, v);
      if (contentAxFactors.getFirst().factor().inCoeffDomain())
        contentAxFactors.removeFirst();
    }
    if (!contentAy.inCoeffDomain())
    {
      contentAyFactors= factorize (contentAy, v);
      if (contentAyFactors.getFirst().factor().inCoeffDomain())
        contentAyFactors.removeFirst();
    }
  }
  else
  {
    if (!contentAx.inCoeffDomain())
    {
      contentAxFactors= factorize (contentAx, true);
      if (contentAxFactors.getFirst().factor().inCoeffDomain())
        contentAxFactors.removeFirst();
    }
    if (!contentAy.inCoeffDomain())
    {
      contentAyFactors= factorize (contentAy, true);
      if (contentAyFactors.getFirst().factor().inCoeffDomain())
        contentAyFactors.removeFirst();
    }
  }

  //check trivial case
  if (degree (A) == 1 || degree (A, 1) == 1)
  {
    CFList factors;
    factors.append (A);

    appendSwapDecompress (factors, conv (contentAxFactors),
                          conv (contentAyFactors), false, false, N);

    normalize (factors);
    return factors;
  }

  //trivial case
  CFList factors;
  if (A.inCoeffDomain())
  {
    append (factors, conv (contentAxFactors));
    append (factors, conv (contentAyFactors));
    decompress (factors, N);
    return factors;
  }
  else if (A.isUnivariate())
  {
    if (extension)
      factors= conv (factorize (A, v));
    else
      factors= conv (factorize (A, true));
    append (factors, conv (contentAxFactors));
    append (factors, conv (contentAyFactors));
    decompress (factors, N);
    return factors;
  }

  bool swap= false;
  if (degree (A) > degree (A, x))
  {
    A= swapvar (A, y, x);
    swap= true;
  }

  CanonicalForm Aeval, bufAeval, buf;
  CFList uniFactors, list, bufUniFactors;
  DegreePattern degs;
  DegreePattern bufDegs;

  CanonicalForm Aeval2, bufAeval2;
  CFList bufUniFactors2, list2, uniFactors2;
  DegreePattern degs2;
  DegreePattern bufDegs2;
  bool swap2= false;

  // several univariate factorizations to obtain more information about the
  // degree pattern therefore usually less combinations have to be tried during
  // the recombination process
  int factorNums= 2;
  int subCheck1= substituteCheck (A, x);
  int subCheck2= substituteCheck (A, y);
  buf= swapvar (A,x,y);
  int evaluation, evaluation2, bufEvaluation= 0, bufEvaluation2= 0;
  for (int i= 0; i < factorNums; i++)
  {
    bufAeval= A;
    bufAeval= evalPoint (A, bufEvaluation);

    bufAeval2= buf;
    bufAeval2= evalPoint (buf, bufEvaluation2);


    // univariate factorization
    TIMING_START (fac_uni_factorizer);

    if (extension)
      bufUniFactors= conv (factorize (bufAeval, v));
    else
      bufUniFactors= conv (factorize (bufAeval, true));
    TIMING_END_AND_PRINT (fac_uni_factorizer,
                          "time for univariate factorization: ");
    DEBOUTLN (cerr, "Lc (bufAeval)*prod (bufUniFactors)== bufAeval " <<
              prod (bufUniFactors)*Lc (bufAeval) == bufAeval);

    TIMING_START (fac_uni_factorizer);
    if (extension)
      bufUniFactors2= conv (factorize (bufAeval2, v));
    else
      bufUniFactors2= conv (factorize (bufAeval2, true));
    TIMING_END_AND_PRINT (fac_uni_factorizer,
                          "time for univariate factorization in y: ");
    DEBOUTLN (cerr, "Lc (Aeval2)*prod (uniFactors2)== Aeval2 " <<
              prod (bufUniFactors2)*Lc (bufAeval2) == bufAeval2);

    if (bufUniFactors.getFirst().inCoeffDomain())
      bufUniFactors.removeFirst();
    if (bufUniFactors2.getFirst().inCoeffDomain())
      bufUniFactors2.removeFirst();
    if (bufUniFactors.length() == 1 || bufUniFactors2.length() == 1)
    {
      factors.append (A);

      appendSwapDecompress (factors, conv (contentAxFactors),
                            conv (contentAyFactors), swap, swap2, N);

      if (isOn (SW_RATIONAL))
        normalize (factors);
      return factors;
    }

    if (i == 0)
    {
      if (subCheck1 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors);

        if (subCheck > 1 && (subCheck1%subCheck == 0))
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, x);
          factors= biFactorize (bufA, v);
          reverseSubst (factors, subCheck, x);
          appendSwapDecompress (factors, conv (contentAxFactors),
                                conv (contentAyFactors), swap, swap2, N);
          if (isOn (SW_RATIONAL))
            normalize (factors);
          return factors;
        }
      }

      if (subCheck2 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors2);

        if (subCheck > 1 && (subCheck2%subCheck == 0))
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, y);
          factors= biFactorize (bufA, v);
          reverseSubst (factors, subCheck, y);
          appendSwapDecompress (factors, conv (contentAxFactors),
                                conv (contentAyFactors), swap, swap2, N);
          if (isOn (SW_RATIONAL))
            normalize (factors);
          return factors;
        }
      }
    }

    // degree analysis
    bufDegs = DegreePattern (bufUniFactors);
    bufDegs2= DegreePattern (bufUniFactors2);

    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      uniFactors= bufUniFactors;
      degs= bufDegs;
      Aeval2= bufAeval2;
      evaluation2= bufEvaluation2;
      uniFactors2= bufUniFactors2;
      degs2= bufDegs2;
    }
    else
    {
      degs.intersect (bufDegs);
      degs2.intersect (bufDegs2);
      if (bufUniFactors2.length() < uniFactors2.length())
      {
        uniFactors2= bufUniFactors2;
        Aeval2= bufAeval2;
        evaluation2= bufEvaluation2;
      }
      if (bufUniFactors.length() < uniFactors.length())
      {
        if (evaluation != 0)
        {
          uniFactors= bufUniFactors;
          Aeval= bufAeval;
          evaluation= bufEvaluation;
        }
      }
    }
    bufEvaluation++;
    bufEvaluation2++;
  }

  if (evaluation != 0 && (uniFactors.length() > uniFactors2.length() ||
      (uniFactors.length() == uniFactors2.length()
       && degs.getLength() > degs2.getLength())))
  {
    degs= degs2;
    uniFactors= uniFactors2;
    evaluation= evaluation2;
    Aeval= Aeval2;
    A= buf;
    swap2= true;
  }


  if (degs.getLength() == 1) // A is irreducible
  {
    factors.append (A);
    appendSwapDecompress (factors, conv (contentAxFactors),
                          conv (contentAyFactors), swap, swap2, N);
    if (isOn (SW_RATIONAL))
      normalize (factors);
    return factors;
  }

  A= A (y + evaluation, y);

  int liftBound= degree (A, y) + 1 + degree (LC(A, x));

  ExtensionInfo dummy= ExtensionInfo (false);
  bool earlySuccess= false;
  CFList earlyFactors;
  TIMING_START (fac_hensel_lift);
  uniFactors= henselLiftAndEarly 
             (A, earlySuccess, earlyFactors, degs, liftBound,
              uniFactors, dummy, evaluation);
  TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");
  DEBOUTLN (cerr, "lifted factors= " << uniFactors);

  CanonicalForm MODl= power (y, liftBound);

  factors= factorRecombination (uniFactors, A, MODl, degs, 1,
                                uniFactors.length()/2);

  if (earlySuccess)
    factors= Union (earlyFactors, factors);
  else if (!earlySuccess && degs.getLength() == 1)
    factors= earlyFactors;

  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= i.getItem() (y - evaluation, y);

  appendSwapDecompress (factors, conv (contentAxFactors),
                        conv (contentAyFactors), swap, swap2, N);
  if (isOn (SW_RATIONAL))
    normalize (factors);

  return factors;
}
