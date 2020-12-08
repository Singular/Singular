/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFactorize.cc
 *
 * multivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "cf_algorithm.h"
#include "facFqFactorizeUtil.h"
#include "facFactorize.h"
#include "facFqFactorize.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_map_ext.h"
#include "cf_reval.h"
#include "facSparseHensel.h"
#include "cfUnivarGcd.h"

TIMING_DEFINE_PRINT(fac_bi_factorizer)
TIMING_DEFINE_PRINT(fac_hensel_lift)
TIMING_DEFINE_PRINT(fac_factor_recombination)
TIMING_DEFINE_PRINT(fac_shift_to_zero)
TIMING_DEFINE_PRINT(fac_precompute_leadcoeff)
TIMING_DEFINE_PRINT(fac_evaluation)
TIMING_DEFINE_PRINT(fac_recover_factors)
TIMING_DEFINE_PRINT(fac_preprocess_and_content)
TIMING_DEFINE_PRINT(fac_bifactor_total)
TIMING_DEFINE_PRINT(fac_luckswang)
TIMING_DEFINE_PRINT(fac_lcheuristic)
TIMING_DEFINE_PRINT(fac_cleardenom)
TIMING_DEFINE_PRINT(fac_content)
TIMING_DEFINE_PRINT(fac_compress)

CFList evalPoints (const CanonicalForm& F, CFList& eval, Evaluation& E)
{
  CFList result;
  Variable x= Variable (1);

  CanonicalForm LCF=LC (F,x);
  CFList LCFeval;

  bool found= false;
  bool allZero= true;
  bool foundZero= false;
  CanonicalForm deriv_x, gcd_deriv;
  CFListIterator iter;
  do
  {
    eval.insert (F);
    LCFeval.insert (LCF);
    bool bad= false;
    for (int i= E.max(); i >= E.min(); i--)
    {
      eval.insert (eval.getFirst()( E [i], i));
      LCFeval.insert (LCFeval.getFirst() (E [i], i));
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
        eval= CFList();
        LCFeval= CFList();
        bad= true;
        break;
      }
      if ((i != 2) && (degree (LCFeval.getFirst(), i-1) != degree (LCF, i-1)))
      {
        result= CFList();
        eval= CFList();
        LCFeval= CFList();
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


#ifdef HAVE_NTL // ratBiSqrfFactorize
void
factorizationWRTDifferentSecondVars (const CanonicalForm& A, CFList*& Aeval,
                                     int& minFactorsLength, bool& irred,
                                     const Variable& w)
{
  Variable x= Variable (1);
  minFactorsLength= 0;
  irred= false;
  Variable v;
  CFList factors;
  CanonicalForm LCA= LC (A,1);
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      v= Variable (Aeval[j].getFirst().level());

      factors= ratBiSqrfFactorize (Aeval[j].getFirst(), w);
      if (factors.getFirst().inCoeffDomain())
        factors.removeFirst();

      if (minFactorsLength == 0)
        minFactorsLength= factors.length();
      else
        minFactorsLength= tmin (minFactorsLength, factors.length());

      if (factors.length() == 1)
      {
        irred= true;
        return;
      }
      sortList (factors, x);
      Aeval [j]= factors;
    }
  }
}
#endif

#ifdef HAVE_NTL // precomputeLeadingCoeff,
// henselLiftAndEarly, nonMonicHenselLift
CFList
multiFactorize (const CanonicalForm& F, const Variable& v)
{
  if (F.inCoeffDomain())
    return CFList (F);

  TIMING_START (fac_preprocess_and_content);
  // compress and find main Variable
  CFMap N;
  TIMING_START (fac_compress)
  CanonicalForm A= myCompress (F, N);
  TIMING_END_AND_PRINT (fac_compress, "time to compress poly over Q: ")

  //univariate case
  if (F.isUnivariate())
  {
    CFList result;
    if (v.level() != 1)
      result= conv (factorize (F, v));
    else
      result= conv (factorize (F,true));
    if (result.getFirst().inCoeffDomain())
      result.removeFirst();
    return result;
  }

  //bivariate case
  if (A.level() == 2)
  {
    CFList buf= ratBiSqrfFactorize (F, v);
    if (buf.getFirst().inCoeffDomain())
      buf.removeFirst();
    return buf;
  }

  Variable x= Variable (1);
  Variable y= Variable (2);

  // remove content
  TIMING_START (fac_content);
  CFList contentAi;
  CanonicalForm lcmCont= lcmContent (A, contentAi);
  A /= lcmCont;
  TIMING_END_AND_PRINT (fac_content, "time to extract content over Q: ");

  // trivial after content removal
  CFList contentAFactors;
  if (A.inCoeffDomain())
  {
    for (CFListIterator i= contentAi; i.hasItem(); i++)
    {
      if (i.getItem().inCoeffDomain())
        continue;
      else
      {
        lcmCont /= i.getItem();
        contentAFactors=
        Union (multiFactorize (lcmCont, v),
               multiFactorize (i.getItem(), v));
        break;
      }
    }
    decompress (contentAFactors, N);
    if (isOn (SW_RATIONAL))
      normalize (contentAFactors);
    return contentAFactors;
  }

  // factorize content
  TIMING_START (fac_content);
  contentAFactors= multiFactorize (lcmCont, v);
  TIMING_END_AND_PRINT (fac_content, "time to factor content over Q: ");

  // univariate after content removal
  CFList factors;
  if (A.isUnivariate ())
  {
    if (v.level() != 1)
      factors= conv (factorize (A, v));
    else
      factors= conv (factorize (A,true));
    append (factors, contentAFactors);
    decompress (factors, N);
    return factors;
  }

  A *= bCommonDen (A);
  CFList Aeval, list, evaluation, bufEvaluation, bufAeval;
  int factorNums= 2;
  //p is irreducible. But factorize does not recognizes this. However, if you
  //change factorNums to 2 at line 281 in facFactorize.cc it will. That change
  //might impair performance in some cases since you do twice as many
  //bivariate factorizations as before. Otherwise you need to change
  //precomputeLeadingCoeff to detect these cases and trigger more bivariate
  // factorizations.
  // (http://www.singular.uni-kl.de:8002/trac/ticket/666)
  CFList biFactors, bufBiFactors;
  CanonicalForm evalPoly;
  int lift, bufLift, lengthAeval2= A.level()-2;
  CFList* bufAeval2= new CFList [lengthAeval2];
  CFList* Aeval2= new CFList [lengthAeval2];
  int counter;
  int differentSecondVar= 0;
  CanonicalForm bufA;
  TIMING_END_AND_PRINT (fac_preprocess_and_content,
                       "time to preprocess poly and extract content over Q: ");

  // several bivariate factorizations
  TIMING_START (fac_bifactor_total);
  REvaluation E (2, A.level(), IntRandom (25));
  for (int i= 0; i < factorNums; i++)
  {
    counter= 0;
    bufA= A;
    bufAeval= CFList();
    TIMING_START (fac_evaluation);
    bufEvaluation= evalPoints (bufA, bufAeval, E);
    TIMING_END_AND_PRINT (fac_evaluation,
                          "time to find evaluation point over Q: ");
    E.nextpoint();
    evalPoly= 0;

    TIMING_START (fac_evaluation);
    evaluationWRTDifferentSecondVars (bufAeval2, bufEvaluation, A);
    TIMING_END_AND_PRINT (fac_evaluation,
                          "time to eval wrt diff second vars over Q: ");

    for (int j= 0; j < lengthAeval2; j++)
    {
      if (!bufAeval2[j].isEmpty())
        counter++;
    }

    bufLift= degree (A, y) + 1 + degree (LC(A, x), y);

    TIMING_START (fac_bi_factorizer);
    bufBiFactors= ratBiSqrfFactorize (bufAeval.getFirst(), v);
    TIMING_END_AND_PRINT (fac_bi_factorizer,
                          "time for bivariate factorization: ");
    bufBiFactors.removeFirst();

    if (bufBiFactors.length() == 1)
    {
      factors.append (A);
      appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
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
      biFactors= bufBiFactors;
      lift= bufLift;
      for (int j= 0; j < lengthAeval2; j++)
        Aeval2 [j]= bufAeval2 [j];
      differentSecondVar= counter;
    }
    else
    {
      if (bufBiFactors.length() < biFactors.length() ||
          ((bufLift < lift) && (bufBiFactors.length() == biFactors.length())) ||
          counter > differentSecondVar)
      {
        Aeval= bufAeval;
        evaluation= bufEvaluation;
        biFactors= bufBiFactors;
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

  sortList (biFactors, x);

  int minFactorsLength;
  bool irred= false;
  TIMING_START (fac_bi_factorizer);
  factorizationWRTDifferentSecondVars (A, Aeval2, minFactorsLength, irred, v);
  TIMING_END_AND_PRINT (fac_bi_factorizer,
             "time for bivariate factorization wrt diff second vars over Q: ");

  TIMING_END_AND_PRINT (fac_bifactor_total,
                        "total time for eval and bivar factors over Q: ");
  if (irred)
  {
    factors.append (A);
    appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
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
    factors.append (A);
    appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
    if (isOn (SW_RATIONAL))
      normalize (factors);
    delete [] Aeval2;
    return factors;
  }

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
      factors= sparseHeuristic (A, biFactors, Aeval2, evaluation,
                                minFactorsLength);
      if (factors.length() == biFactors.length())
      {
        appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
        normalize (factors);
        delete [] Aeval2;
        return factors;
      }
      else
        factors= CFList();
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
  TIMING_START (fac_precompute_leadcoeff);
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
    distributeLCmultiplier (A, leadingCoeffs, biFactors, evaluation, LCmultiplier);

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [lengthAeval2];
  prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(), leadingCoeffs,
                        biFactors, evaluation);

  CFListIterator iter;
  CFList bufLeadingCoeffs2= leadingCoeffs2[lengthAeval2-1];
  bufBiFactors= biFactors;
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
  TIMING_END_AND_PRINT(fac_precompute_leadcoeff,
                       "time to precompute LC over Q: ");

  TIMING_START (fac_luckswang);
  CFList bufFactors= CFList();
  bool LCheuristic= false;
  if (LucksWangSparseHeuristic (A, biFactors, 2, leadingCoeffs2[lengthAeval2-1],
                                factors))
  {
    int check= biFactors.length();
    int * index= new int [factors.length()];
    CFList oldFactors= factors;
    factors= recoverFactors (A, factors, index);

    if (check == factors.length())
    {
      if (w.level() != 1)
      {
        for (iter= factors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), w, y);
      }

      appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
      normalize (factors);
      delete [] index;
      delete [] Aeval2;
      TIMING_END_AND_PRINT (fac_luckswang,
                            "time for successful LucksWang over Q: ");
      return factors;
    }
    else if (factors.length() > 0)
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
      bufFactors= factors;
      factors= CFList();
    }
    else if (!LCmultiplierIsConst && factors.length() == 0)
    {
      LCheuristic= true;
      factors= oldFactors;
      CFList contents, LCs;
      bool foundTrueMultiplier= false;
      LCHeuristic2 (LCmultiplier, factors, leadingCoeffs2[lengthAeval2-1],
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
        LCHeuristic3 (LCmultiplier, factors, oldBiFactors, contents, oldAeval,
                      A, leadingCoeffs2, lengthAeval2, foundMultiplier);
        // coming from above: divide out more LCmultiplier if possible
        if (foundMultiplier)
        {
          foundMultiplier= false;
          LCHeuristic4 (oldBiFactors, oldAeval, contents, factors, testVars,
                        lengthAeval2, leadingCoeffs2, A, LCmultiplier,
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
      factors= CFList();
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
      factors= CFList();
    delete [] index;
  }
  TIMING_END_AND_PRINT (fac_luckswang, "time for LucksWang over Q: ");

  TIMING_START (fac_lcheuristic);
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
  TIMING_END_AND_PRINT (fac_lcheuristic, "time for Lc heuristic over Q: ");

tryAgainWithoutHeu:
  //shifting to zero
  TIMING_START (fac_shift_to_zero);
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
  TIMING_END_AND_PRINT (fac_shift_to_zero,
                        "time to shift evaluation point to zero: ");

  CFArray Pi;
  CFList diophant;
  int* liftBounds= new int [A.level() - 1];
  int liftBoundsLength= A.level() - 1;
  for (int i= 0; i < liftBoundsLength; i++)
    liftBounds [i]= degree (A, i + 2) + 1;

  Aeval.removeFirst();
  bool noOneToOne= false;

  TIMING_START (fac_cleardenom);
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
  TIMING_END_AND_PRINT (fac_cleardenom, "time to clear denominators: ");

  TIMING_START (fac_hensel_lift);
  factors= nonMonicHenselLift (Aeval, biFactors, leadingCoeffs2, diophant,
                               Pi, liftBounds, liftBoundsLength, noOneToOne);
  TIMING_END_AND_PRINT (fac_hensel_lift,
                        "time for non monic hensel lifting over Q: ");

  if (!noOneToOne)
  {
    int check= factors.length();
    A= oldA;
    TIMING_START (fac_recover_factors);
    factors= recoverFactors (A, factors, evaluation);
    TIMING_END_AND_PRINT (fac_recover_factors,
                          "time to recover factors over Q: ");
    if (check != factors.length())
      noOneToOne= true;
    else
      factors= Union (factors, bufFactors);
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
    TIMING_START (fac_hensel_lift);
    liftedFactors= henselLiftAndEarly
                   (A, MOD, liftBounds, earlySuccess, earlyFactors,
                    Aeval, biFactors, evaluation, info);
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");

    TIMING_START (fac_factor_recombination);
    factors= factorRecombination (A, liftedFactors, MOD);
    TIMING_END_AND_PRINT (fac_factor_recombination,
                          "time for factor recombination: ");

    if (earlySuccess)
      factors= Union (factors, earlyFactors);

    for (CFListIterator i= factors; i.hasItem(); i++)
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
    for (CFListIterator iter= factors; iter.hasItem(); iter++)
      iter.getItem()= swapvar (iter.getItem(), w, y);
  }

  append (factors, contentAFactors);
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
