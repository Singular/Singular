/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.cc
 *
 * multivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "cf_algorithm.h"
#include "facFqFactorizeUtil.h"
#include "facFactorize.h"
#include "facFqFactorize.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_gcd_smallp.h"
#include "cf_map_ext.h"
#include "algext.h"
#include "cf_reval.h"
#include "facSparseHensel.h"

TIMING_DEFINE_PRINT(fac_bi_factorize)
TIMING_DEFINE_PRINT(fac_hensel_lift)
TIMING_DEFINE_PRINT(fac_factor_recombination)

#ifdef HAVE_NTL
CFList evalPoints (const CanonicalForm& F, CFList& eval, Evaluation& E)
{
  CFList result;
  Variable x= Variable (1);

  bool found= false;
  CanonicalForm deriv_x, gcd_deriv;
  CFListIterator iter;
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
    iter= eval;
    iter++;
    CanonicalForm contentx= content (iter.getItem(), x);
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
  Variable v;
  CFList factors;
  CanonicalForm LCA= LC (A,1);
  if (!LCA.inCoeffDomain())
  {
    for (int j= 0; j < A.level() - 2; j++)
    {
      if (!Aeval[j].isEmpty() && (degree (LCA, j+3) > 0))
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
      else if (!Aeval[j].isEmpty())
      {
        Aeval[j]=CFList();
      }
    }
  }
  else
  {
    for (int j= 0; j < A.level() - 2; j++)
      Aeval[j]= CFList();
  }
}

int
testFactors (const CanonicalForm& G, const CFList& uniFactors,
             CanonicalForm& sqrfPartF, CFList& factors,
             CFFList*& bufSqrfFactors, CFList& evalSqrfPartF,
             const CFArray& evalPoint)
{
  CanonicalForm tmp;
  CFListIterator j;
  for (CFListIterator i= uniFactors; i.hasItem(); i++)
  {
    tmp= i.getItem();
    if (i.hasItem())
      i++;
    else
      break;
    for (j= i; j.hasItem(); j++)
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

  evalSqrfPartF= evaluateAtEval (sqrfPartF, evalPoint);

  CanonicalForm test= evalSqrfPartF.getFirst() (evalPoint[0], 2);

  if (degree (test) != degree (sqrfPartF, 1) || test.inCoeffDomain())
    return 0;

  CFFList sqrfFactors;
  CFList tmp2;
  int k= 0;
  factors= uniFactors;
  CFFListIterator iter;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    tmp= 1;
    sqrfFactors= sqrFree (i.getItem());

    for (iter= sqrfFactors; iter.hasItem(); iter++)
    {
      tmp2.append (iter.getItem().factor());
      tmp *= iter.getItem().factor();
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
      for (iter= bufSqrfFactors [i]; iter.hasItem(); iter++)
      {
        if (iter.getItem().factor().inCoeffDomain())
          continue;
        iter.getItem()= CFFactor (iter.getItem().factor()/
                                  Lc (iter.getItem().factor()),
                                  iter.getItem().exp());
        factors.append (iter.getItem().factor());
      }
    }
    else
    {
      for (iter= bufSqrfFactors [i]; iter.hasItem(); iter++)
      {
        if (iter.getItem().factor().inCoeffDomain())
          continue;
        iter.getItem()= CFFactor (iter.getItem().factor()/
                               Lc (iter.getItem().factor()),
                               iter.getItem().exp());
        if (!find (factors, iter.getItem().factor()))
          factors.append (iter.getItem().factor());
      }
    }
  }

  test= prod (factors);
  tmp= evalSqrfPartF.getFirst() (evalPoint[0],2);
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

  CFMap N, M;
  CFArray dummy= CFArray (2);
  dummy [0]= LCF;
  dummy [1]= Variable (2);
  compress (dummy, M, N);
  CanonicalForm F= M (LCF);
  if (LCF.isUnivariate())
  {
    CFList result;
    int LCFLevel= LCF.level();
    bool found= false;
    if (LCFLevel == 2)
    {
    //bivariate leading coefficients are already the true leading coefficients
      result= LCFFactors;
      found= true;
    }
    else
    {
      CFListIterator j;
      for (int i= 0; i < length; i++)
      {
        for (j= differentSecondVarLCs[i]; j.hasItem(); j++)
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

  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= M (i.getItem());

  CanonicalForm sqrfPartF;
  CFFList * bufSqrfFactors= new CFFList [factors.length()];
  CFList evalSqrfPartF, bufFactors;
  CFArray evalPoint= CFArray (evaluation.length() - 1);
  CFArray buf= CFArray (evaluation.length());
  CFArray swap= CFArray (evaluation.length());
  CFListIterator iter= evaluation;
  CanonicalForm vars=getVars (LCF)*Variable (2);
  for (int i= evaluation.length() +1; i > 1; i--, iter++)
  {
    buf[i-2]=iter.getItem();
    if (degree (vars, i) > 0)
      swap[M(Variable (i)).level()-1]=buf[i-2];
  }
  buf= swap;
  for (int i= 0; i < evaluation.length() - 1; i++)
    evalPoint[i]= buf[i+1];

  //TODO sqrfPartF einmal berechnen nicht ständig
  int pass= testFactors (F, factors, sqrfPartF,
                         bufFactors, bufSqrfFactors, evalSqrfPartF, evalPoint);

  bool foundDifferent= false;
  Variable z;
  Variable x= y;
  int j= 0;
  if (!pass)
  {
    int lev= 0;
    CanonicalForm bufF;
    CFList bufBufFactors;
    for (int i= 0; i < length; i++)
    {
      if (!differentSecondVarLCs [i].isEmpty())
      {
        bool allConstant= true;
        for (iter= differentSecondVarLCs[i]; iter.hasItem(); iter++)
        {
          if (!iter.getItem().inCoeffDomain())
          {
            allConstant= false;
            y= Variable (iter.getItem().level());
            lev= M(y).level();
          }
        }
        if (allConstant)
          continue;

        bufFactors= differentSecondVarLCs [i];
        for (iter= bufFactors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), x, y);
        bufF= F;
        z= Variable (lev);
        bufF= swapvar (bufF, x, z);
        bufBufFactors= bufFactors;
        evalPoint= CFArray (evaluation.length() - 1);
        for (int k= 0; k < evaluation.length()-1; k++)
        {
          if (N (Variable (k+1)).level() != y.level())
            evalPoint[k]= buf[k+1];
          else
            evalPoint[k]= buf[0];
        }
        pass= testFactors (bufF, bufBufFactors, sqrfPartF, bufFactors,
                           bufSqrfFactors, evalSqrfPartF, evalPoint);
        if (pass)
        {
          foundDifferent= true;
          F= bufF;
          CFList l= factors;
          for (iter= l; iter.hasItem(); iter++)
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
          delete [] bufSqrfFactors;
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
    delete [] bufSqrfFactors;
    return result;
  }
  else
    factors= bufFactors;


  bufFactors= factors;
  CFList evaluation2;
  for (int i= 0; i < F.level()-1; i++)
    evaluation2.insert (evalPoint[i]);

  CFList interMedResult;
  CanonicalForm oldSqrfPartF= sqrfPartF;
  sqrfPartF= shift2Zero (sqrfPartF, evalSqrfPartF, evaluation2);
  if (factors.length() > 1)
  {
    CanonicalForm LC1= LC (oldSqrfPartF, 1);
    CFList leadingCoeffs;
    for (int i= 0; i < factors.length(); i++)
      leadingCoeffs.append (LC1);

    CFList LC1eval= evaluateAtEval (LC1, evaluation2,2);
    CFList oldFactors= factors;
    for (CFListIterator i= oldFactors; i.hasItem(); i++)
      i.getItem() *= LC1eval.getFirst()/Lc (i.getItem());

    bool success= false;
    CanonicalForm oldSqrfPartFPowLC= oldSqrfPartF*power(LC1,factors.length()-1);
    if (size (oldSqrfPartFPowLC)/getNumVars (oldSqrfPartFPowLC) < 500 &&
        LucksWangSparseHeuristic (oldSqrfPartFPowLC,
                                  oldFactors, 2, leadingCoeffs, factors))
    {
      interMedResult= recoverFactors (oldSqrfPartF, factors);
      if (oldFactors.length() == interMedResult.length())
        success= true;
    }
    if (!success)
    {
      LC1= LC (evalSqrfPartF.getFirst(), 1);

      CFArray leadingCoeffs= CFArray (factors.length());
      for (int i= 0; i < factors.length(); i++)
        leadingCoeffs[i]= LC1;

      for (CFListIterator i= factors; i.hasItem(); i++)
        i.getItem() *= LC1 (0,2)/Lc (i.getItem());
      factors.insert (1);

      CanonicalForm
      newSqrfPartF= evalSqrfPartF.getFirst()*power (LC1, factors.length() - 2);

      int liftBound= degree (newSqrfPartF,2) + 1;

      CFMatrix M= CFMatrix (liftBound, factors.length() - 1);
      CFArray Pi;
      CFList diophant;
      nonMonicHenselLift12 (newSqrfPartF, factors, liftBound, Pi, diophant, M,
                            leadingCoeffs, false);

      if (sqrfPartF.level() > 2)
      {
        int* liftBounds= new int [sqrfPartF.level() - 1];
        liftBounds [0]= liftBound;
        bool noOneToOne= false;
        CFList *leadingCoeffs2= new CFList [sqrfPartF.level()-2];
        LC1= LC (evalSqrfPartF.getLast(), 1);
        CFList LCs;
        for (int i= 0; i < factors.length(); i++)
          LCs.append (LC1);
        leadingCoeffs2 [sqrfPartF.level() - 3]= LCs;
        for (int i= sqrfPartF.level() - 1; i > 2; i--)
        {
          for (CFListIterator j= LCs; j.hasItem(); j++)
            j.getItem()= j.getItem() (0, i + 1);
          leadingCoeffs2 [i - 3]= LCs;
        }
        sqrfPartF *= power (LC1, factors.length()-1);

        int liftBoundsLength= sqrfPartF.level() - 1;
        for (int i= 1; i < liftBoundsLength; i++)
          liftBounds [i]= degree (sqrfPartF, i + 2) + 1;
        evalSqrfPartF= evaluateAtZero (sqrfPartF);
        evalSqrfPartF.removeFirst();
        factors= nonMonicHenselLift (evalSqrfPartF, factors, leadingCoeffs2,
                 diophant, Pi, liftBounds, sqrfPartF.level() - 1, noOneToOne);
        delete [] leadingCoeffs2;
        delete [] liftBounds;
      }
      for (CFListIterator iter= factors; iter.hasItem(); iter++)
        iter.getItem()= reverseShift (iter.getItem(), evaluation2);

      interMedResult=
      recoverFactors (reverseShift(evalSqrfPartF.getLast(),evaluation2),
                      factors);
    }
  }
  else
  {
    CanonicalForm contF=content (oldSqrfPartF,1);
    factors= CFList (oldSqrfPartF/contF);
    interMedResult= recoverFactors (oldSqrfPartF, factors);
  }

  CFList result;
  CFFListIterator k;
  CanonicalForm tmp;
  for (int i= 0; i < LCFFactors.length(); i++)
  {
    tmp= 1;
    for (k= bufSqrfFactors[i]; k.hasItem(); k++)
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

  delete [] bufSqrfFactors;

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
    if (v.level() != 1)
      factors= conv (factorize (A, v));
    else
      factors= conv (factorize (A,true));
    append (factors, contentAFactors);
    decompress (factors, N);
    return factors;
  }

  A *= bCommonDen (A);
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

    TIMING_START (fac_bi_factorize);
    bufBiFactors= ratBiSqrfFactorize (bufAeval.getFirst(), v);
    TIMING_END_AND_PRINT (fac_bi_factorize,
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

  if (differentSecondVar == A.level() - 2 && getNumVars(LC(A,1)) == A.level()-1)
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

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  sortByUniFactors (Aeval2, A.level() - 2, uniFactors, evaluation);

  CFList * oldAeval= new CFList [A.level() - 2];
  for (int i= 0; i < A.level() - 2; i++)
    oldAeval[i]= Aeval2[i];

  getLeadingCoeffs (A, Aeval2, uniFactors, evaluation);

  CFList biFactorsLCs;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
    biFactorsLCs.append (LC (i.getItem(), 1));

  Variable w;
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs,
                                          evaluation, Aeval2, A.level() - 2, w);

  if (w.level() != 1)
  {
    A= swapvar (A, y, w);
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
    for (i= 0; i < A.level() - 2; i++)
    {
      if (oldAeval[i].isEmpty())
        continue;
      if (oldAeval[i].getFirst().level() == w.level())
      {
        CFArray tmp= copy (oldAeval[i]);
        for (int ii= 0; ii < tmp.size(); ii++)
          tmp[ii]= swapvar (tmp[ii], w, y);
        CFArray tmp2= CFArray (tmp.size());
        CanonicalForm buf;
        for (int ii= 0; ii < tmp.size(); ii++)
        {
          buf= tmp[ii] (evaluation.getLast(),y);
          buf /= Lc (buf);
          tmp2[findItem (uniFactors, buf)-1]=tmp[ii];
        }
        biFactors= CFList();
        for (int j= 0; j < tmp2.size(); j++)
          biFactors.append (tmp2[j]);
      }
    }
  }

  CFListIterator iter;
  CanonicalForm oldA= A;
  CFList oldBiFactors= biFactors;
  if (!leadingCoeffs.getFirst().inCoeffDomain())
  {
    CanonicalForm tmp= power (leadingCoeffs.getFirst(), biFactors.length() - 1);
    A *= tmp;
    tmp= leadingCoeffs.getFirst();
    iter= evaluation;
    for (int i= A.level(); i > 2; i--, iter++)
      tmp= tmp (iter.getItem(), i);
    if (!tmp.inCoeffDomain())
    {
      for (CFListIterator i= biFactors; i.hasItem(); i++)
      {
        i.getItem() *= tmp/LC (i.getItem(), 1);
        i.getItem() /= Lc (i.getItem());
      }
    }
  }

  leadingCoeffs.removeFirst();

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [A.level() - 2];
  prepareLeadingCoeffs (leadingCoeffs2, A.level(), leadingCoeffs, biFactors,
                        evaluation);


  Aeval= evaluateAtEval (A, evaluation, 2);

  CanonicalForm hh= Lc (Aeval.getFirst());

  for (CFListIterator i= Aeval; i.hasItem(); i++)
    i.getItem() /= hh;

  A /= hh;

  if (size (A)/getNumVars (A) < 500 &&
      LucksWangSparseHeuristic (A, biFactors, 2, leadingCoeffs2 [A.level() - 3],
                                factors))
  {
    int check= factors.length();
    factors= recoverFactors (A, factors);

    if (check == factors.length())
    {

      if (w.level() != 1)
      {
        for (CFListIterator iter= factors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), w, y);
      }

      appendSwapDecompress (factors, contentAFactors, N, 0, 0, x);
      normalize (factors);
      delete [] Aeval2;
      return factors;
    }
    else
      factors= CFList();
  }


  //shifting to zero
  A= shift2Zero (A, Aeval, evaluation);

  for (iter= biFactors; iter.hasItem(); iter++)
    iter.getItem()= iter.getItem () (y + evaluation.getLast(), y);

  for (int i= 0; i < A.level() - 2; i++)
  {
    if (i != A.level() - 3)
      leadingCoeffs2[i]= CFList();
  }
  for (iter= leadingCoeffs2[A.level() - 3]; iter.hasItem(); iter++)
  {
    iter.getItem()= shift2Zero (iter.getItem(), list, evaluation);
    for (int i= A.level() - 4; i > -1; i--)
    {
      if (i + 1 == A.level() - 3)
        leadingCoeffs2[i].append (iter.getItem() (0, i + 4));
      else
        leadingCoeffs2[i].append (leadingCoeffs2[i+1].getLast() (0, i + 4));
    }
  }

  CFArray Pi;
  CFList diophant;
  int* liftBounds= new int [A.level() - 1];
  int liftBoundsLength= A.level() - 1;
  for (int i= 0; i < liftBoundsLength; i++)
    liftBounds [i]= degree (A, i + 2) + 1;

  Aeval.removeFirst();
  bool noOneToOne= false;

  CFList commonDenominators;
  for (CFListIterator iter=biFactors; iter.hasItem(); iter++)
    commonDenominators.append (bCommonDen (iter.getItem()));
  CanonicalForm tmp1, tmp2, tmp3=1;
  CFListIterator iter1, iter2;
  for (int i= 0; i < A.level() - 2; i++)
  {
    iter2= commonDenominators;
    for (iter1= leadingCoeffs2[i]; iter1.hasItem(); iter1++, iter2++)
    {
      tmp1= bCommonDen (iter1.getItem());
      Off (SW_RATIONAL);
      iter2.getItem()= lcm (iter2.getItem(), tmp1);
      On (SW_RATIONAL);
    }
  }
  tmp1= prod (commonDenominators);
  for (iter1= Aeval; iter1.hasItem(); iter1++)
  {
    tmp2= bCommonDen (iter1.getItem());
    Off (SW_RATIONAL);
    tmp3= lcm (tmp2,tmp3);
    On (SW_RATIONAL);
  }
  CanonicalForm multiplier;
  multiplier= tmp3/tmp1;
  iter2= commonDenominators;
  for (iter1=biFactors; iter1.hasItem(); iter1++, iter2++)
    iter1.getItem() *= iter2.getItem()*multiplier;

  for (iter1= Aeval; iter1.hasItem(); iter1++)
    iter1.getItem() *= tmp3*power (multiplier, biFactors.length() - 1);

  for (int i= 0; i < A.level() - 2; i++)
  {
    iter2= commonDenominators;
    for (iter1= leadingCoeffs2[i]; iter1.hasItem(); iter1++, iter2++)
      iter1.getItem() *= iter2.getItem()*multiplier;
  }


  factors= nonMonicHenselLift (Aeval, biFactors, leadingCoeffs2, diophant,
                               Pi, liftBounds, liftBoundsLength, noOneToOne);

  if (!noOneToOne)
  {
    int check= factors.length();
    A= oldA;
    factors= recoverFactors (A, factors, evaluation);
    if (check != factors.length())
      noOneToOne= true;
  }
  if (noOneToOne)
  {
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

  swap (factors, 0, 0, x);
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
