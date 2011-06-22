/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.cc
 *
 * multivariate factorization over Q(a)
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

#include "facFqFactorizeUtil.h"
#include "facFactorize.h"
#include "facFqFactorize.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_gcd_smallp.h"
#include "cf_map_ext.h"
#include "algext.h"
#include "cf_reval.h"

CFList evalPoints (const CanonicalForm& F, CFList& eval, Evaluation& E)
{
  CFList result;
  Variable x= Variable (1);

  bool found= false;
  CanonicalForm deriv_x, gcd_deriv;
  do
  {
    eval.insert (F);
    bool bad= false;
    for (int i= E.max(); i >= E.min(); i--)
    {
      eval.insert (eval.getFirst()( E [i], i));
      result.append (E[i]);
      if (degree (eval.getFirst(), i - 1) != degree (F, i - 1))
      {
        result= CFList();
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
      E.nextpoint();
      continue;
    }

    deriv_x= deriv (eval.getFirst(), x);
    gcd_deriv= gcd (eval.getFirst(), deriv_x);
    if (degree (gcd_deriv) > 0)
    {
      result= CFList();
      eval= CFList();
      E.nextpoint();
      continue;
    }
    CFListIterator i= eval;
    i++;
    CanonicalForm contentx= content (i.getItem(), x);
    if (degree (contentx) > 0)
    {
      result= CFList();
      eval= CFList();
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

void
factorizationWRTDifferentSecondVars (const CanonicalForm& A, CFList*& Aeval,
                                     int& minFactorsLength, bool& irred,
                                     const Variable& w)
{
  Variable x= Variable (1);
  minFactorsLength= 0;
  irred= false;
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      Variable v= Variable (Aeval[j].getFirst().level());

      CFList factors= ratBiSqrfFactorize (Aeval[j].getFirst(), w);

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

int
testFactors (const CanonicalForm& G, const CFList& uniFactors,
             CanonicalForm& sqrfPartF, CFList& factors,
             CFFList*& bufSqrfFactors, CFList& evalSqrfPartF)
{
  for (CFListIterator i= uniFactors; i.hasItem(); i++)
  {
    CanonicalForm tmp= i.getItem();
    if (i.hasItem())
      i++;
    else
      break;
    for (CFListIterator j= i; j.hasItem(); j++)
    {
      if (tmp == j.getItem())
        return 0;
    }
  }

  CanonicalForm F= G;
  CFFList sqrfFactorization= sqrFree (F);

  sqrfPartF= 1;
  for (CFFListIterator i= sqrfFactorization; i.hasItem(); i++)
    sqrfPartF *= i.getItem().factor();

  evalSqrfPartF= evaluateAtZero (sqrfPartF);

  CanonicalForm test= evalSqrfPartF.getFirst() (0, 2);

  if (degree (test) != degree (sqrfPartF, 1))
    return 0;

  CFFList sqrfFactors;
  CFList tmp2;
  int k= 0;
  factors= uniFactors;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    CanonicalForm tmp= 1;
    sqrfFactors= sqrFree (i.getItem());

    for (CFFListIterator j= sqrfFactors; j.hasItem(); j++)
    {
      tmp2.append (j.getItem().factor());
      tmp *= j.getItem().factor();
    }
    i.getItem()= tmp/Lc(tmp);
    bufSqrfFactors [k]= sqrfFactors;
  }

  for (int i= 0; i < factors.length() - 1; i++)
  {
    for (int k= i + 1; k < factors.length(); k++)
    {
      gcdFreeBasis (bufSqrfFactors [i], bufSqrfFactors[k]);
    }
  }

  factors= CFList();
  for (int i= 0; i < uniFactors.length(); i++)
  {
    if (i == 0)
    {
      for (CFFListIterator k= bufSqrfFactors [i]; k.hasItem(); k++)
      {
        k.getItem()= CFFactor (k.getItem().factor()/Lc (k.getItem().factor()),
                               k.getItem().exp());
        factors.append (k.getItem().factor());
      }
    }
    else
    {
      for (CFFListIterator k= bufSqrfFactors [i]; k.hasItem(); k++)
      {
        k.getItem()= CFFactor (k.getItem().factor()/Lc (k.getItem().factor()),
                               k.getItem().exp());
        if (!find (factors, k.getItem().factor()))
          factors.append (k.getItem().factor());
      }
    }
  }

  test= prod (factors);
  CanonicalForm tmp= evalSqrfPartF.getFirst() (0,2);
  if (test/Lc (test) != tmp/Lc (tmp))
    return 0;
  else
    return 1;
}

CFList
precomputeLeadingCoeff (const CanonicalForm& LCF, const CFList& LCFFactors,
                        const CFList& evaluation,CFList*& differentSecondVarLCs,
                        int length, Variable& y
                       )
{
  y= Variable (1);
  if (LCF.inCoeffDomain())
  {
    CFList result;
    for (int i= 1; i <= LCFFactors.length() + 1; i++)
      result.append (1);
    return result;
  }

  CFMap N;
  CanonicalForm F= compress (LCF, N);
  if (LCF.isUnivariate())
  {
    CFList result;
    int LCFLevel= LCF.level();
    bool found= false;
    if (LCFLevel == 2)
    {
    //bivariate leading coefficients are already the true leading coefficients
      result= LCFFactors;
      Variable v= Variable (LCF.mvar());
      CanonicalForm bla= 1;
      for (CFListIterator i= result; i.hasItem(); i++)
      {
        i.getItem()= i.getItem() (v+evaluation.getLast(), v);
        bla *= Lc (i.getItem());
      }
      found= true;
    }
    else
    {
      for (int i= 0; i < length; i++)
      {
        for (CFListIterator j= differentSecondVarLCs[i]; j.hasItem(); j++)
        {
          if (j.getItem().level() == LCFLevel)
          {
            found= true;
            break;
          }
        }
        if (found)
        {
          result= differentSecondVarLCs [i];
          break;
        }
      }
      if (!found)
        result= LCFFactors;
    }
    if (found)
      result.insert (Lc (LCF));
    else
      result.append (LCF);
    return result;
  }

  CFList factors= LCFFactors;

  CFMap dummy;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    i.getItem()= compress (i.getItem(), dummy);
    i.getItem()= i.getItem() (Variable (1)+evaluation.getLast(), Variable (1));
  }

  CanonicalForm sqrfPartF;
  CFFList * bufSqrfFactors= new CFFList [factors.length()];
  CFList evalSqrfPartF;
  CanonicalForm bufContent;
  CFList bufFactors;
  //TODO sqrfPartF einmal berechnen nicht ständig
  int pass= testFactors (F, factors, sqrfPartF,
                         bufFactors, bufSqrfFactors, evalSqrfPartF);

  bool foundDifferent= false;
  Variable z;
  Variable x= y;
  int j= 0;
  if (!pass)
  {
    int lev;
    for (int i= 1; i <= LCF.level(); i++)
    {
      if(degree (LCF, i) > 0)
      {
        lev= i - 1;
        break;
      }
    }
    for (int i= 0; i < length; i++)
    {
      if (!differentSecondVarLCs [i].isEmpty())
      {
        bool allConstant= true;
        for (CFListIterator iter= differentSecondVarLCs[i]; iter.hasItem();
             iter++)
        {
          if (!iter.getItem().inCoeffDomain())
          {
            allConstant= false;
            y= Variable (iter.getItem().level());
          }
        }
        if (allConstant)
          continue;

        bufFactors= differentSecondVarLCs [i];
        for (CFListIterator iter= bufFactors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), x, y);
        CanonicalForm bufF= F;
        z= Variable (y.level() - lev);
        bufF= swapvar (bufF, x, z);
        CFList bufBufFactors= bufFactors;
        pass= testFactors (bufF, bufBufFactors, sqrfPartF, bufFactors,
                           bufSqrfFactors, evalSqrfPartF);
        if (pass)
        {
          foundDifferent= true;
          F= bufF;
          CFList l= factors;
          for (CFListIterator iter= l; iter.hasItem(); iter++)
            iter.getItem()= swapvar (iter.getItem(), x, y);
          differentSecondVarLCs [i]= l;
          j= i;
          break;
        }
        if (!pass && i == length - 1)
        {
          CFList result;
          result.append (LCF);
          for (int j= 1; j <= factors.length(); j++)
            result.append (LCF);
          y= Variable (1);
          return result;
        }
      }
    }
  }
  if (!pass)
  {
    CFList result;
    result.append (LCF);
    for (int j= 1; j <= factors.length(); j++)
      result.append (LCF);
    y= Variable (1);
    return result;
  }
  else
    factors= bufFactors;

  int liftBound= degree (sqrfPartF,2) + degree (LC (sqrfPartF, 1), 2) + 1;

  int* liftBounds= liftingBounds (sqrfPartF, liftBound);

  bufFactors= factors;
  factors.insert (LC (evalSqrfPartF.getFirst(), 1));
  CFMatrix M= CFMatrix (liftBound, factors.length() - 1);
  CFArray Pi;
  CFList diophant;
  henselLift12 (evalSqrfPartF.getFirst(), factors, liftBound, Pi, diophant, M,
                false);

  if (sqrfPartF.level() > 2)
    factors= henselLift (evalSqrfPartF, factors, liftBounds,
                         sqrfPartF.level() - 1, false);

  CFList MOD;
  for (int i= 0; i < sqrfPartF.level() - 1; i++)
    MOD.append (power (Variable (i + 2), liftBounds [i]));

  CFList interMedResult= leadingCoeffReconstruction (evalSqrfPartF.getLast(),
                                                     factors, MOD);

  CFList result;
  for (int i= 0; i < LCFFactors.length(); i++)
  {
    CanonicalForm tmp= 1;
    for (CFFListIterator k= bufSqrfFactors[i]; k.hasItem(); k++)
    {
      int pos= findItem (bufFactors, k.getItem().factor());
      if (pos)
        tmp *= power (getItem (interMedResult, pos), k.getItem().exp());
    }
    result.append (tmp);
  }

  for (CFListIterator i= result; i.hasItem(); i++)
  {
    F /= i.getItem();
    if (foundDifferent)
      i.getItem()= swapvar (i.getItem(), x, z);
    i.getItem()= N (i.getItem());
  }

  if (foundDifferent)
  {
    CFList l= differentSecondVarLCs [j];
    for (CFListIterator i= l; i.hasItem(); i++)
      i.getItem()= swapvar (i.getItem(), y, z);
    differentSecondVarLCs [j]= l;
    F= swapvar (F, x, z);
  }

  result.insert (N (F));

  result= distributeContent (result, differentSecondVarLCs, length);

  if (!result.getFirst().inCoeffDomain())
  {
    CFListIterator i= result;
    CanonicalForm tmp;
    if (foundDifferent)
      i.getItem()= swapvar (i.getItem(), Variable (2), y);

    tmp= i.getItem();

    i++;
    for (; i.hasItem(); i++)
    {
      if (foundDifferent)
        i.getItem()= swapvar (i.getItem(), Variable (2), y)*tmp;
      else
        i.getItem() *= tmp;
    }
  }
  else
    y= Variable (1);

  return result;
}


CFList
multiFactorize (const CanonicalForm& F, const Variable& v)
{
  if (F.inCoeffDomain())
    return CFList (F);

  // compress and find main Variable
  CFMap N;
  CanonicalForm A= myCompress (F, N);

  //univariate case
  if (F.isUnivariate())
  {
    CFList result= conv (factorize (F, v));
    if (result.getFirst().inCoeffDomain())
      result.removeFirst();
    return result;
  }

  //bivariate case
  if (A.level() == 2)
  {
    CFList buf= biFactorize (F, v);
    return buf;
  }

  Variable x= Variable (1);
  Variable y= Variable (2);

  // remove content
  CFList contentAi;
  CanonicalForm lcmCont= lcmContent (A, contentAi);
  A /= lcmCont;

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
  contentAFactors= multiFactorize (lcmCont, v);

  // univariate after content removal
  CFList factors;
  if (A.isUnivariate ())
  {
    factors= conv (factorize (A, v));
    append (factors, contentAFactors);
    decompress (factors, N);
    return factors;
  }

  // check main variable
  CFList Aeval, list, evaluation, bufEvaluation, bufAeval;
  int factorNums= 3;
  CanonicalForm bivarEval;
  CFList biFactors, bufBiFactors;
  CanonicalForm evalPoly;
  int lift, bufLift;
  CFList* bufAeval2= new CFList [A.level() - 2];
  CFList* Aeval2= new CFList [A.level() - 2];
  int counter;
  int differentSecondVar= 0;
  CanonicalForm bufA;
  // several bivariate factorizations
  REvaluation E (2, A.level(), IntRandom (25));
  for (int i= 0; i < factorNums; i++)
  {
    counter= 0;
    bufA= A;
    bufAeval= CFList();
    bufEvaluation= evalPoints (bufA, bufAeval, E);
    E.nextpoint();
    evalPoly= 0;

    bivarEval= bufEvaluation.getLast();

    evaluationWRTDifferentSecondVars (bufAeval2, bufEvaluation, A);

    for (int j= 0; j < A.level() - 1; j++)
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
      for (int j= 0; j < A.level() - 2; j++)
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
        for (int j= 0; j < A.level() - 2; j++)
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
  factorizationWRTDifferentSecondVars (A, Aeval2, minFactorsLength, irred, v);

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

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  CFList * oldAeval= new CFList [A.level() - 2];
  for (int i= 0; i < A.level() - 2; i++)
    oldAeval[i]= Aeval2[i];

  getLeadingCoeffs (A, Aeval2, uniFactors, evaluation);

  CFList biFactorsLCs;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
    biFactorsLCs.append (LC (i.getItem(), 1));


  //shifting to zero
  A= shift2Zero (A, Aeval, evaluation);

  CanonicalForm hh= Lc (Aeval.getFirst());

  for (CFListIterator i= Aeval; i.hasItem(); i++)
    i.getItem() /= hh;

  A /= hh;

  Variable w;
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs,
                                          evaluation, Aeval2, A.level() - 2, w);

  if (w.level() != 1)
  {
    A= swapvar (A, y, w);
    for (int i= 0; i < A.level() - 2; i++)
    {
      if (oldAeval[i].isEmpty())
        continue;
      if (oldAeval[i].getFirst().level() == w.level())
      {
        biFactors= CFList();
        for (CFListIterator iter= oldAeval [i]; iter.hasItem(); iter++)
          biFactors.append (swapvar (iter.getItem(), w, y));
      }
    }
    int i= A.level();
    CanonicalForm evalPoint;
    for (CFListIterator iter= evaluation; iter.hasItem(); iter++, i--)
    {
      if (i == w.level())
      {
        evalPoint= iter.getItem();
        iter.getItem()= evaluation.getLast();
        evaluation.removeLast();
        evaluation.append (evalPoint);
        break;
      }
    }
    Aeval= evaluateAtZero (A);
  }

  CFListIterator iter= biFactors;
  for (; iter.hasItem(); iter++)
    iter.getItem()= iter.getItem () (y + evaluation.getLast(), y);

  CanonicalForm oldA= A;
  CFList oldBiFactors= biFactors;
  if (!leadingCoeffs.getFirst().inCoeffDomain())
  {
    CanonicalForm tmp= power (leadingCoeffs.getFirst(), biFactors.length() - 1);
    A *= tmp;
    Aeval= evaluateAtZero (A);
    tmp= leadingCoeffs.getFirst();
    for (int i= A.level(); i > 2; i--)
      tmp= tmp (0, i);
    if (!tmp.inCoeffDomain())
    {
      for (CFListIterator i= biFactors; i.hasItem(); i++)
      {
        i.getItem() *= tmp/LC (i.getItem(), 1);
        i.getItem() /= Lc (i.getItem());
      }
    }
    hh= Lc (Aeval.getFirst());
    for (CFListIterator i= Aeval; i.hasItem(); i++)
      i.getItem() /= hh;

    A /= hh;
  }

  leadingCoeffs.removeFirst();

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [A.level() - 2];
  prepareLeadingCoeffs (leadingCoeffs2, A.level(), leadingCoeffs, biFactors);

  CFArray Pi;
  CFList diophant;
  int* liftBounds= new int [A.level() - 1];
  int liftBoundsLength= A.level() - 1;
  for (int i= 0; i < liftBoundsLength; i++)
    liftBounds [i]= degree (A, i + 2) + 1;

  Aeval.removeFirst();
  bool noOneToOne= false;

  factors= nonMonicHenselLift (Aeval, biFactors, leadingCoeffs2, diophant,
                               Pi, liftBounds, liftBoundsLength, noOneToOne);

  if (!noOneToOne)
  {
    int check= factors.length();
    factors= recoverFactors (A, factors);
    if (check != factors.length())
      noOneToOne= true;
  }
  if (noOneToOne)
  {
    A= oldA;
    Aeval= evaluateAtZero (A);
    biFactors= oldBiFactors;
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
    TIMING_START (fac_hensel_lift);
    CFList liftedFactors= henselLiftAndEarly
                          (A, MOD, liftBounds, earlySuccess, earlyFactors,
                           Aeval, biFactors, evaluation, info);
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");

    TIMING_START (fac_factor_recombination);
    factors= factorRecombination (A, liftedFactors, MOD);
    TIMING_END_AND_PRINT (fac_factor_recombination,
                          "time for factor recombination: ");

    if (earlySuccess)
      factors= Union (factors, earlyFactors);
  }

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

  if (w.level() != 1)
  {
    for (CFListIterator iter= factors; iter.hasItem(); iter++)
      iter.getItem()= swapvar (iter.getItem(), w, y);
  }

  swap (factors, 0, 0, x);
  append (factors, contentAFactors);
  decompress (factors, N);
  if (isOn (SW_RATIONAL))
    normalize (factors);

  delete[] liftBounds;

  return factors;
}
