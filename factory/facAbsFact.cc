/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.cc
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "timing.h"
#include "debug.h"

#include "facAbsBiFact.h"
#include "facAbsFact.h"
#include "facFqFactorize.h"
#include "facFqFactorizeUtil.h"
#include "facHensel.h"
#include "facSparseHensel.h"
#include "facFactorize.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "cfModResultant.h"
#include "cfUnivarGcd.h"
#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#if defined(HAVE_NTL)
TIMING_DEFINE_PRINT(abs_fac_bi_factorizer)
TIMING_DEFINE_PRINT(abs_fac_hensel_lift)
TIMING_DEFINE_PRINT(abs_fac_factor_recombination)
TIMING_DEFINE_PRINT(abs_fac_shift_to_zero)
TIMING_DEFINE_PRINT(abs_fac_precompute_leadcoeff)
TIMING_DEFINE_PRINT(abs_fac_evaluation)
TIMING_DEFINE_PRINT(abs_fac_recover_factors)
TIMING_DEFINE_PRINT(abs_fac_bifactor_total)
TIMING_DEFINE_PRINT(abs_fac_luckswang)
TIMING_DEFINE_PRINT(abs_fac_lcheuristic)
TIMING_DEFINE_PRINT(abs_fac_cleardenom)
TIMING_DEFINE_PRINT(abs_fac_compress)

/// steps 4)-8) of Algorithm B.7.8. from Greuel, Pfister "A Singular
/// Introduction to Commutative Algebra"
CFAFList
RothsteinTragerResultant (const CanonicalForm& F, const CanonicalForm& w, int s,
                          const CFList& evaluation, const Variable& y)
{
  CFList terms;
  for (CFIterator i= w; i.hasTerms(); i++)
    terms.append (i.coeff());

  Variable x= Variable (1);
  CanonicalForm derivF= deriv (F, x);
  CanonicalForm g, geval, derivFeval, Feval, H, res, sqrfPartRes;
  CFListIterator iter;

  REvaluation E (1, terms.length(), IntRandom (25));

  do
  {
    E.nextpoint();
    g= 0;
    iter= terms;
    for (int i= terms.length(); i >= 1; i--, iter++)
      g += E[i]*iter.getItem();

    geval= g;
    Feval= F;
    derivFeval= derivF;
    iter= evaluation;
    for (int i= F.level(); i >= 2; iter++, i--)
    {
      Feval= Feval (iter.getItem(), i);
      geval= geval (iter.getItem(), i);
      derivFeval= derivFeval (iter.getItem(), i);
    }

    H= y*derivFeval-geval;

    if (degree (Feval, x) >= 8 || degree (H, x) >= 8)
      res= resultantZ (Feval, H, x);
    else
      res= resultant (Feval, H, x);

    sqrfPartRes= sqrfPart (res); //univariate poly in y
  }
  while (degree (sqrfPartRes) != s);

  Variable beta= rootOf (sqrfPartRes);

  CanonicalForm factor= gcd (F, beta*derivF-g);

  return CFAFList (CFAFactor (factor, getMipo (beta), 1));
}


/// Algorithm B.7.8 from Greuel, Pfister "A Singular Introduction to Commutative
/// Algebra"
CFAFList
RothsteinTrager (const CanonicalForm& F, const CFList& factors,
                 const Variable& alpha, const CFList& evaluation)
{
  Variable x= Variable (1);
  ASSERT (factors.length() == 2, "expected two factors");
  CanonicalForm G, H;
  if (totaldegree (factors.getFirst()) > totaldegree (factors.getLast()))
  {
    H= factors.getLast();
    G= factors.getFirst();
  }
  else
  {
    H= factors.getFirst();
    G= factors.getLast();
  }
  CanonicalForm derivH= deriv (H, x);
  CanonicalForm w= G*derivH;
  Variable y= Variable (F.level()+1);
  w= replacevar (w, alpha, y);

  int s= totaldegree (F)/totaldegree (H);

  return RothsteinTragerResultant (F, w, s, evaluation, y);
}

CFList
evalPoints4AbsFact (const CanonicalForm& F, CFList& eval, Evaluation& E,
                    int& intervalSize)
{
  CFList result;
  Variable x= Variable (1);

  CanonicalForm LCF= LC (F, x);
  CFList LCFeval= CFList();

  bool found= false;
  bool allZero= true;
  bool foundZero= false;
  CanonicalForm deriv_x, gcd_deriv;
  CFFList uniFactors;
  CFListIterator iter;
  int count= 0;
  do
  {
    count++;
    if (count==E.max() - E.min() + 1)
    {
      count= 1;
      intervalSize++;
      E= REvaluation (E.min(), E.max(), IntRandom (intervalSize));
      E.nextpoint();
    }
    eval.insert (F);
    LCFeval.insert (LCF);
    bool bad= false;
    for (int i= E.max(); i >= E.min(); i--)
    {
      eval.insert (eval.getFirst()( E [i], i));
      LCFeval.insert (LCFeval.getFirst()( E [i], i));
      result.append (E[i]);
      if (!E[i].isZero())
        allZero= false;
      else
        foundZero= true;
      if (!allZero && foundZero)
      {
        result= CFList();
        eval= CFList();
        LCFeval= CFList();
        bad= true;
        foundZero= false;
        break;
      }
      if (degree (eval.getFirst(), i - 1) != degree (F, i - 1))
      {
        result= CFList();
        LCFeval= CFList();
        eval= CFList();
        bad= true;
        break;
      }
      if ((i != 2) && (degree (LCFeval.getFirst(), i-1) != degree (LCF, i-1)))
      {
        result= CFList();
        LCFeval= CFList();
        eval= CFList();
        bad= true;
        break;
      }
    }

    if (bad)
    {
      E.nextpoint();
      continue;
    }

    if (degree (eval.getFirst()) != degree (F, 1))
    {
      result= CFList();
      eval= CFList();
      LCFeval= CFList();
      E.nextpoint();
      continue;
    }

    deriv_x= deriv (eval.getFirst(), x);
    gcd_deriv= gcd (eval.getFirst(), deriv_x);
    if (degree (gcd_deriv) > 0)
    {
      result= CFList();
      eval= CFList();
      LCFeval= CFList();
      E.nextpoint();
      continue;
    }
    uniFactors= factorize (eval.getFirst());
    if (uniFactors.getFirst().factor().inCoeffDomain())
      uniFactors.removeFirst();
    if (uniFactors.length() > 1 || uniFactors.getFirst().exp() > 1)
    {
      result= CFList();
      eval= CFList();
      LCFeval= CFList();
      E.nextpoint();
      continue;
    }
    iter= eval;
    iter++;
    CanonicalForm contentx= content (iter.getItem(), x);
    if (degree (contentx) > 0)
    {
      result= CFList();
      eval= CFList();
      LCFeval= CFList();
      E.nextpoint();
      continue;
    }
    contentx= content (iter.getItem());
    if (degree (contentx) > 0)
    {
      result= CFList();
      eval= CFList();
      LCFeval= CFList();
      E.nextpoint();
      continue;
    }
    found= true;
  }
  while (!found);

  if (!eval.isEmpty())
    eval.removeFirst();
  return result;
}

CFAFList absFactorize (const CanonicalForm& G
                           )
{
  //TODO handle homogeneous input, is already done in LIB interface but still...
  ASSERT (getCharacteristic() == 0, "expected poly over Q");

  CanonicalForm F= G;

  CanonicalForm LcF= Lc (F);
  bool isRat= isOn (SW_RATIONAL);
  if (isRat)
    F *= bCommonDen (F);

  Off (SW_RATIONAL);
  F /= icontent (F);
  if (isRat)
    On (SW_RATIONAL);

  CFFList rationalFactors= factorize (F);

  CFAFList result, resultBuf;

  CFAFListIterator iter;
  CFFListIterator i= rationalFactors;
  i++;
  for (; i.hasItem(); i++)
  {
    resultBuf= absFactorizeMain (i.getItem().factor());
    for (iter= resultBuf; iter.hasItem(); iter++)
      iter.getItem()= CFAFactor (iter.getItem().factor(),
                                 iter.getItem().minpoly(), i.getItem().exp());
    result= Union (result, resultBuf);
  }

  if (isRat)
    normalize (result);
  result.insert (CFAFactor (LcF, 1, 1));

  return result;
}

CFAFList absFactorizeMain (const CanonicalForm& G)
{
  CanonicalForm F= bCommonDen (G)*G;

  Off (SW_RATIONAL);
  F /= icontent (F);
  On (SW_RATIONAL);

  if (F.inCoeffDomain())
    return CFAFList (CFAFactor (F, 1, 1));

  // compress and find main Variable
  CFMap N;
  TIMING_START (abs_fac_compress)
  CanonicalForm A= myCompress (F, N);
  TIMING_END_AND_PRINT (abs_fac_compress,
                        "time to compress poly in abs fact: ")

  //univariate case
  if (F.isUnivariate())
  {
    CFAFList result;
    result= uniAbsFactorize (F);
    if (result.getFirst().factor().inCoeffDomain())
      result.removeFirst();
    return result;
  }

  //bivariate case
  if (A.level() == 2)
  {
    CFAFList result= absBiFactorizeMain (A);
    decompress (result, N);
    return result; //needs compressed input
  }

  Variable x= Variable (1);
  Variable y= Variable (2);

  CFAFList factors;
  A *= bCommonDen (A);
  CFList Aeval, list, evaluation, bufEvaluation, bufAeval;
  int factorNums= 1;
  CFAFList absBiFactors, absBufBiFactors;
  CanonicalForm evalPoly;
  int lift, bufLift, lengthAeval2= A.level()-2;
  CFList* bufAeval2= new CFList [lengthAeval2];
  CFList* Aeval2= new CFList [lengthAeval2];
  int counter;
  int differentSecondVar= 0;
  CanonicalForm bufA;

  // several bivariate factorizations
  TIMING_START (abs_fac_bifactor_total);
  int absValue= 2;
  REvaluation E (2, A.level(), IntRandom (absValue));
  for (int i= 0; i < factorNums; i++)
  {
    counter= 0;
    bufA= A;
    bufAeval= CFList();
    TIMING_START (abs_fac_evaluation);
    bufEvaluation= evalPoints4AbsFact (bufA, bufAeval, E, absValue);
    TIMING_END_AND_PRINT (abs_fac_evaluation,
                          "time to find evaluation point in abs fact: ");
    E.nextpoint();
    evalPoly= 0;

    TIMING_START (abs_fac_evaluation);
    evaluationWRTDifferentSecondVars (bufAeval2, bufEvaluation, A);
    TIMING_END_AND_PRINT (abs_fac_evaluation,
                          "time to eval wrt diff second vars in abs fact: ");

    for (int j= 0; j < lengthAeval2; j++)
    {
      if (!bufAeval2[j].isEmpty())
        counter++;
    }

    bufLift= degree (A, y) + 1 + degree (LC(A, x), y);

    TIMING_START (abs_fac_bi_factorizer);
    absBufBiFactors= absBiFactorizeMain (bufAeval.getFirst(), true);
    TIMING_END_AND_PRINT (abs_fac_bi_factorizer,
                          "time for bivariate factorization in abs fact: ");

    if (absBufBiFactors.length() == 1)
    {
      factors.append (CFAFactor (A, 1, 1));
      decompress (factors, N);
      if (isOn (SW_RATIONAL))
        normalize (factors);
      delete [] bufAeval2;
      delete [] Aeval2;
      return factors;
    }

    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      absBiFactors= absBufBiFactors;
      lift= bufLift;
      for (int j= 0; j < lengthAeval2; j++)
        Aeval2 [j]= bufAeval2 [j];
      differentSecondVar= counter;
    }
    else
    {
      if (absBufBiFactors.length() < absBiFactors.length() ||
          ((bufLift < lift) &&
          (absBufBiFactors.length() == absBiFactors.length())) ||
          counter > differentSecondVar)
      {
        Aeval= bufAeval;
        evaluation= bufEvaluation;
        absBiFactors= absBufBiFactors;
        lift= bufLift;
        for (int j= 0; j < lengthAeval2; j++)
          Aeval2 [j]= bufAeval2 [j];
        differentSecondVar= counter;
      }
    }
    int k= 0;
    for (CFListIterator j= bufEvaluation; j.hasItem(); j++, k++)
      evalPoly += j.getItem()*power (x, k);
    list.append (evalPoly);
  }

  delete [] bufAeval2;

  CFList rationalFactors;
  Variable v= mvar (absBiFactors.getFirst().minpoly());

  CFList biFactors;
  for (CFAFListIterator iter= absBiFactors; iter.hasItem(); iter++)
    biFactors.append (iter.getItem().factor());

  sortList (biFactors, x);

  int minFactorsLength;
  bool irred= false;

  TIMING_START (abs_fac_bi_factorizer);
  factorizationWRTDifferentSecondVars (A, Aeval2, minFactorsLength, irred, v);
  TIMING_END_AND_PRINT (abs_fac_bi_factorizer,
         "time for bivariate factorization wrt diff second vars in abs fact: ");

  TIMING_END_AND_PRINT (abs_fac_bifactor_total,
                        "total time for eval and bivar factors in abs fact: ");
  if (irred)
  {
    factors.append (CFAFactor (A, 1, 1));
    decompress (factors, N);
    if (isOn (SW_RATIONAL))
      normalize (factors);
    delete [] Aeval2;
    return factors;
  }

  if (minFactorsLength == 0)
    minFactorsLength= biFactors.length();
  else if (biFactors.length() > minFactorsLength)
    refineBiFactors (A, biFactors, Aeval2, evaluation, minFactorsLength);
  minFactorsLength= tmin (minFactorsLength, biFactors.length());

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  sortByUniFactors (Aeval2, lengthAeval2, uniFactors, biFactors, evaluation);

  minFactorsLength= tmin (minFactorsLength, biFactors.length());

  if (minFactorsLength == 1)
  {
    factors.append (CFAFactor (A, 1, 1));
    decompress (factors, N);
    if (isOn (SW_RATIONAL))
      normalize (factors);
    delete [] Aeval2;
    return factors;
  }

  bool found= false;
  if (differentSecondVar == lengthAeval2)
  {
    bool zeroOccured= false;
    for (CFListIterator iter= evaluation; iter.hasItem(); iter++)
    {
      if (iter.getItem().isZero())
      {
        zeroOccured= true;
        break;
      }
    }
    if (!zeroOccured)
    {
      rationalFactors= sparseHeuristic (A, biFactors, Aeval2, evaluation,
                                       minFactorsLength);
      if (rationalFactors.length() == biFactors.length())
      {
        for (CFListIterator iter= rationalFactors; iter.hasItem(); iter++)
        {
          if (totaldegree(iter.getItem())*degree(getMipo(v)) == totaldegree (G))
          {
            factors= CFAFList (CFAFactor (iter.getItem(), getMipo (v), 1));
            found= true;
            break;
          }
        }
        // necessary since extension may be too large
        if (!found)
          factors= RothsteinTrager (A, rationalFactors, v, evaluation);

        decompress (factors, N);
        normalize (factors);
        delete [] Aeval2;
        return factors;
      }
      else
        rationalFactors= CFList();
      //TODO case where factors.length() > 0
    }
  }

  CFList * oldAeval= new CFList [lengthAeval2];
  for (int i= 0; i < lengthAeval2; i++)
    oldAeval[i]= Aeval2[i];

  getLeadingCoeffs (A, Aeval2);

  CFList biFactorsLCs;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
    biFactorsLCs.append (LC (i.getItem(), 1));

  Variable w;
  TIMING_START (abs_fac_precompute_leadcoeff);
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs, x,
                                          evaluation, Aeval2, lengthAeval2, w);

  if (w.level() != 1)
    changeSecondVariable (A, biFactors, evaluation, oldAeval, lengthAeval2,
                          uniFactors, w);

  CanonicalForm oldA= A;
  CFList oldBiFactors= biFactors;

  CanonicalForm LCmultiplier= leadingCoeffs.getFirst();
  bool LCmultiplierIsConst= LCmultiplier.inCoeffDomain();
  leadingCoeffs.removeFirst();

  if (!LCmultiplierIsConst)
    distributeLCmultiplier (A, leadingCoeffs, biFactors, evaluation,
                            LCmultiplier);

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [lengthAeval2];
  prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(), leadingCoeffs,
                        biFactors, evaluation);

  CFListIterator iter;
  CFList bufLeadingCoeffs2= leadingCoeffs2[lengthAeval2-1];
  CFList bufBiFactors= biFactors;
  bufA= A;
  CanonicalForm testVars, bufLCmultiplier= LCmultiplier;
  if (!LCmultiplierIsConst)
  {
    testVars= Variable (2);
    for (int i= 0; i < lengthAeval2; i++)
    {
      if (!oldAeval[i].isEmpty())
        testVars *= oldAeval[i].getFirst().mvar();
    }
  }
  TIMING_END_AND_PRINT(abs_fac_precompute_leadcoeff,
                       "time to precompute LC in abs fact: ");

  TIMING_START (abs_fac_luckswang);
  CFList bufFactors= CFList();
  bool LCheuristic= false;
  if (LucksWangSparseHeuristic (A, biFactors, 2, leadingCoeffs2[lengthAeval2-1],
                                rationalFactors))
  {
    int check= biFactors.length();
    int * index= new int [factors.length()];
    CFList oldFactors= rationalFactors;
    rationalFactors= recoverFactors (A, rationalFactors, index);

    if (check == rationalFactors.length())
    {
      if (w.level() != 1)
      {
        for (iter= rationalFactors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), w, y);
      }

      for (CFListIterator iter= rationalFactors; iter.hasItem(); iter++)
      {
        if (totaldegree(iter.getItem())*degree (getMipo (v)) == totaldegree (G))
        {
          factors= CFAFList (CFAFactor (iter.getItem(), getMipo (v), 1));
          found=true;
          break;
        }
      }
      // necessary since extension may be too large
      if (!found)
        factors= RothsteinTrager (A, rationalFactors, v, evaluation);

      decompress (factors, N);
      normalize (factors);
      delete [] index;
      delete [] Aeval2;
      TIMING_END_AND_PRINT (abs_fac_luckswang,
                            "time for successful LucksWang in abs fact: ");
      return factors;
    }
    else if (rationalFactors.length() > 0)
    {
      int oneCount= 0;
      CFList l;
      for (int i= 0; i < check; i++)
      {
        if (index[i] == 1)
        {
          iter=biFactors;
          for (int j=1; j <= i-oneCount; j++)
            iter++;
          iter.remove (1);
          for (int j= 0; j < lengthAeval2; j++)
          {
            l= leadingCoeffs2[j];
            iter= l;
            for (int k=1; k <= i-oneCount; k++)
              iter++;
            iter.remove (1);
            leadingCoeffs2[j]=l;
          }
          oneCount++;
        }
      }
      bufFactors= rationalFactors;
      rationalFactors= CFList();
    }
    else if (!LCmultiplierIsConst && rationalFactors.length() == 0)
    {
      LCheuristic= true;
      rationalFactors= oldFactors;
      CFList contents, LCs;
      bool foundTrueMultiplier= false;
      LCHeuristic2 (LCmultiplier,rationalFactors,leadingCoeffs2[lengthAeval2-1],
                    contents, LCs, foundTrueMultiplier);
      if (foundTrueMultiplier)
      {
          A= oldA;
          leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
          for (int i= lengthAeval2-1; i > -1; i--)
            leadingCoeffs2[i]= CFList();
          prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),
                                leadingCoeffs, biFactors, evaluation);
      }
      else
      {
        bool foundMultiplier= false;
        LCHeuristic3 (LCmultiplier, rationalFactors, oldBiFactors, contents,
                      oldAeval,A,leadingCoeffs2, lengthAeval2, foundMultiplier);
        // coming from above: divide out more LCmultiplier if possible
        if (foundMultiplier)
        {
          foundMultiplier= false;
          LCHeuristic4 (oldBiFactors, oldAeval, contents, rationalFactors,
                        testVars, lengthAeval2, leadingCoeffs2, A, LCmultiplier,
                        foundMultiplier);
        }
        else
        {
          LCHeuristicCheck (LCs, contents, A, oldA,
                            leadingCoeffs2[lengthAeval2-1], foundMultiplier);
          if (!foundMultiplier && fdivides (getVars (LCmultiplier), testVars))
          {
            LCHeuristic (A, LCmultiplier, biFactors, leadingCoeffs2, oldAeval,
                         lengthAeval2, evaluation, oldBiFactors);
          }
        }

        // patch everything together again
        leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
        for (int i= lengthAeval2-1; i > -1; i--)
          leadingCoeffs2[i]= CFList();
        prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),leadingCoeffs,
                              biFactors, evaluation);
      }
      rationalFactors= CFList();
      if (!fdivides (LC (oldA,1),prod (leadingCoeffs2[lengthAeval2-1])))
      {
        LCheuristic= false;
        A= bufA;
        biFactors= bufBiFactors;
        leadingCoeffs2[lengthAeval2-1]= bufLeadingCoeffs2;
        LCmultiplier= bufLCmultiplier;
      }
    }
    else
      rationalFactors= CFList();
    delete [] index;
  }
  TIMING_END_AND_PRINT (abs_fac_luckswang, "time for LucksWang in abs fact: ");

  TIMING_START (abs_fac_lcheuristic);
  if (!LCheuristic && !LCmultiplierIsConst && bufFactors.isEmpty()
      && fdivides (getVars (LCmultiplier), testVars))
  {
    LCheuristic= true;
    LCHeuristic (A, LCmultiplier, biFactors, leadingCoeffs2, oldAeval,
                 lengthAeval2, evaluation, oldBiFactors);

    leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
    for (int i= lengthAeval2-1; i > -1; i--)
      leadingCoeffs2[i]= CFList();
    prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),leadingCoeffs,
                          biFactors, evaluation);

    if (!fdivides (LC (oldA,1),prod (leadingCoeffs2[lengthAeval2-1])))
    {
      LCheuristic= false;
      A= bufA;
      biFactors= bufBiFactors;
      leadingCoeffs2[lengthAeval2-1]= bufLeadingCoeffs2;
      LCmultiplier= bufLCmultiplier;
    }
  }
  TIMING_END_AND_PRINT (abs_fac_lcheuristic,
                        "time for Lc heuristic in abs fact: ");

tryAgainWithoutHeu:
  //shifting to zero
  TIMING_START (abs_fac_shift_to_zero);
  CanonicalForm denA= bCommonDen (A);
  A *= denA;
  A= shift2Zero (A, Aeval, evaluation);
  A /= denA;

  for (iter= biFactors; iter.hasItem(); iter++)
    iter.getItem()= iter.getItem () (y + evaluation.getLast(), y);

  for (int i= 0; i < lengthAeval2-1; i++)
    leadingCoeffs2[i]= CFList();
  for (iter= leadingCoeffs2[lengthAeval2-1]; iter.hasItem(); iter++)
  {
    iter.getItem()= shift2Zero (iter.getItem(), list, evaluation);
    for (int i= A.level() - 4; i > -1; i--)
    {
      if (i + 1 == lengthAeval2-1)
        leadingCoeffs2[i].append (iter.getItem() (0, i + 4));
      else
        leadingCoeffs2[i].append (leadingCoeffs2[i+1].getLast() (0, i + 4));
    }
  }
  TIMING_END_AND_PRINT (abs_fac_shift_to_zero,
                        "time to shift evaluation point to zero in abs fact: ");

  CFArray Pi;
  CFList diophant;
  int* liftBounds= new int [A.level() - 1];
  int liftBoundsLength= A.level() - 1;
  for (int i= 0; i < liftBoundsLength; i++)
    liftBounds [i]= degree (A, i + 2) + 1;

  Aeval.removeFirst();
  bool noOneToOne= false;

  TIMING_START (abs_fac_cleardenom);
  CFList commonDenominators;
  for (iter=biFactors; iter.hasItem(); iter++)
    commonDenominators.append (bCommonDen (iter.getItem()));
  CanonicalForm tmp1, tmp2, tmp3=1;
  CFListIterator iter2;
  for (int i= 0; i < lengthAeval2; i++)
  {
    iter2= commonDenominators;
    for (iter= leadingCoeffs2[i]; iter.hasItem(); iter++, iter2++)
    {
      tmp1= bCommonDen (iter.getItem());
      Off (SW_RATIONAL);
      iter2.getItem()= lcm (iter2.getItem(), tmp1);
      On (SW_RATIONAL);
    }
  }
  tmp1= prod (commonDenominators);
  for (iter= Aeval; iter.hasItem(); iter++)
  {
    tmp2= bCommonDen (iter.getItem()/denA);
    Off (SW_RATIONAL);
    tmp3= lcm (tmp2,tmp3);
    On (SW_RATIONAL);
  }
  CanonicalForm multiplier;
  multiplier= tmp3/tmp1;
  iter2= commonDenominators;
  for (iter=biFactors; iter.hasItem(); iter++, iter2++)
    iter.getItem() *= iter2.getItem()*multiplier;

  for (iter= Aeval; iter.hasItem(); iter++)
    iter.getItem() *= tmp3*power (multiplier, biFactors.length() - 1)/denA;

  for (int i= 0; i < lengthAeval2; i++)
  {
    iter2= commonDenominators;
    for (iter= leadingCoeffs2[i]; iter.hasItem(); iter++, iter2++)
      iter.getItem() *= iter2.getItem()*multiplier;
  }

  TIMING_END_AND_PRINT (abs_fac_cleardenom,
                        "time to clear denominators in abs fact: ");

  TIMING_START (abs_fac_hensel_lift);
  rationalFactors= nonMonicHenselLift (Aeval, biFactors,leadingCoeffs2,diophant,
                               Pi, liftBounds, liftBoundsLength, noOneToOne);
  TIMING_END_AND_PRINT (abs_fac_hensel_lift,
                        "time for non monic hensel lifting in abs fact: ");

  if (!noOneToOne)
  {
    int check= rationalFactors.length();
    A= oldA;
    TIMING_START (abs_fac_recover_factors);
    rationalFactors= recoverFactors (A, rationalFactors, evaluation);
    TIMING_END_AND_PRINT (abs_fac_recover_factors,
                          "time to recover factors in abs fact: ");
    if (check != rationalFactors.length())
      noOneToOne= true;
    else
      rationalFactors= Union (rationalFactors, bufFactors);
  }
  if (noOneToOne)
  {
    if (!LCmultiplierIsConst && LCheuristic)
    {
      A= bufA;
      biFactors= bufBiFactors;
      leadingCoeffs2[lengthAeval2-1]= bufLeadingCoeffs2;
      delete [] liftBounds;
      LCheuristic= false;
      goto tryAgainWithoutHeu;
      //something probably went wrong in the heuristic
    }

    A= shift2Zero (oldA, Aeval, evaluation);
    biFactors= oldBiFactors;
    for (iter= biFactors; iter.hasItem(); iter++)
      iter.getItem()= iter.getItem () (y + evaluation.getLast(), y);
    CanonicalForm LCA= LC (Aeval.getFirst(), 1);
    CanonicalForm yToLift= power (y, lift);
    CFListIterator i= biFactors;
    lift= degree (i.getItem(), 2) + degree (LC (i.getItem(), 1)) + 1;
    i++;

    for (; i.hasItem(); i++)
      lift= tmax (lift, degree (i.getItem(), 2)+degree (LC (i.getItem(), 1))+1);

    lift= tmax (degree (Aeval.getFirst() , 2) + 1, lift);

    i= biFactors;
    yToLift= power (y, lift);
    CanonicalForm dummy;
    for (; i.hasItem(); i++)
    {
      LCA= LC (i.getItem(), 1);
      extgcd (LCA, yToLift, LCA, dummy);
      i.getItem()= mod (i.getItem()*LCA, yToLift);
    }

    liftBoundsLength= F.level() - 1;
    liftBounds= liftingBounds (A, lift);

    CFList MOD;
    bool earlySuccess;
    CFList earlyFactors;
    ExtensionInfo info= ExtensionInfo (false);
    CFList liftedFactors;
    TIMING_START (abs_fac_hensel_lift);
    liftedFactors= henselLiftAndEarly
                   (A, MOD, liftBounds, earlySuccess, earlyFactors,
                    Aeval, biFactors, evaluation, info);
    TIMING_END_AND_PRINT (abs_fac_hensel_lift,
                          "time for hensel lifting in abs fact: ");

    TIMING_START (abs_fac_factor_recombination);
    rationalFactors= factorRecombination (A, liftedFactors, MOD);
    TIMING_END_AND_PRINT (abs_fac_factor_recombination,
                          "time for factor recombination in abs fact: ");

    if (earlySuccess)
      rationalFactors= Union (rationalFactors, earlyFactors);

    for (CFListIterator i= rationalFactors; i.hasItem(); i++)
    {
      int kk= Aeval.getLast().level();
      for (CFListIterator j= evaluation; j.hasItem(); j++, kk--)
      {
        if (i.getItem().level() < kk)
          continue;
       i.getItem()= i.getItem() (Variable (kk) - j.getItem(), kk);
      }
    }
  }

  if (w.level() != 1)
  {
    for (CFListIterator iter= rationalFactors; iter.hasItem(); iter++)
      iter.getItem()= swapvar (iter.getItem(), w, y);
  }

  for (CFListIterator iter= rationalFactors; iter.hasItem(); iter++)
  {
    if (totaldegree (iter.getItem())*degree (getMipo (v)) == totaldegree (G))
    {
      factors= CFAFList (CFAFactor (iter.getItem(), getMipo (v), 1));
      found= true;
      break;
    }
  }

  // necessary since extension may be too large
  if (!found)
    factors= RothsteinTrager (A, rationalFactors, v, evaluation);

  decompress (factors, N);
  if (isOn (SW_RATIONAL))
    normalize (factors);

  delete [] leadingCoeffs2;
  delete [] oldAeval;
  delete [] Aeval2;
  delete[] liftBounds;

  return factors;
}

#endif
