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

#ifdef HAVE_NTL
CFList evalPoints (const CanonicalForm& F, CFList& eval, Evaluation& E)
{
  CFList result;
  Variable x= Variable (1);

  bool found= false;
  bool allZero= true;
  bool foundZero= false;
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
      if (!E[i].isZero())
        allZero= false;
      else
        foundZero= true;
      if (!allZero && foundZero)
      {
        result= CFList();
        eval= CFList();
        bad= true;
        foundZero= false;
        break;
      }
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
    CFList buf= biFactorize (F, v);
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
  int factorNums= 1;
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

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  sortByUniFactors (Aeval2, lengthAeval2, uniFactors, evaluation);

  CFList * oldAeval= new CFList [lengthAeval2];
  for (int i= 0; i < lengthAeval2; i++)
    oldAeval[i]= Aeval2[i];

  getLeadingCoeffs (A, Aeval2, uniFactors, evaluation);

  CFList biFactorsLCs;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
    biFactorsLCs.append (LC (i.getItem(), 1));

  Variable w;
  TIMING_START (fac_precompute_leadcoeff);
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs, x,
                                          evaluation, Aeval2, lengthAeval2, w);

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
    for (i= 0; i < lengthAeval2; i++)
    {
      if (oldAeval[i].isEmpty())
        continue;
      if (oldAeval[i].getFirst().level() == w.level())
      {
        CFArray tmp= copy (oldAeval[i]);
        oldAeval[i]= biFactors; 
        for (CFListIterator iter= oldAeval[i]; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), w, y);
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

  CanonicalForm LCmultiplier= leadingCoeffs.getFirst();
  bool LCmultiplierIsConst= LCmultiplier.inCoeffDomain();
  leadingCoeffs.removeFirst();

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [lengthAeval2];
  prepareLeadingCoeffs (leadingCoeffs2, A.level(), leadingCoeffs, biFactors,
                        evaluation);


  Aeval= evaluateAtEval (A, evaluation, 2);

  CanonicalForm hh= 1/Lc (Aeval.getFirst());

  for (iter= Aeval; iter.hasItem(); iter++)
    iter.getItem() *= hh;

  A *= hh;

  CFListIterator iter2;
  CFList bufLeadingCoeffs2= leadingCoeffs2[lengthAeval2-1];
  bufBiFactors= biFactors;
  bufA= A;
  CanonicalForm bufLCmultiplier= LCmultiplier;
  CanonicalForm testVars;
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
      CanonicalForm cont;
      CFList contents, LCs;
      int index=1;
      bool foundTrueMultiplier= false;
      for (iter= factors; iter.hasItem(); iter++, index++)
      {
        cont= content (iter.getItem(), 1);
        cont= gcd (cont , LCmultiplier);
        contents.append (cont);
        if (cont.inCoeffDomain()) // trivial content->LCmultiplier needs to go there
        {
          foundTrueMultiplier= true;
          int index2= 1;
          for (iter2= leadingCoeffs2[lengthAeval2-1]; iter2.hasItem(); iter2++,
                                                                    index2++)
          {
            if (index2 == index)
              continue;
            iter2.getItem() /= LCmultiplier;
          }
          A= oldA;
          leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
          for (int i= lengthAeval2-1; i > -1; i--)
            leadingCoeffs2[i]= CFList();
          prepareLeadingCoeffs (leadingCoeffs2, A.level(), leadingCoeffs,
                                biFactors, evaluation );
          Aeval= evaluateAtEval (A, evaluation, 2);

          hh= 1/Lc (Aeval.getFirst());

          for (iter2= Aeval; iter2.hasItem(); iter2++)
            iter2.getItem() *= hh;

          A *= hh;
          break;
        }
        else
          LCs.append (LC (iter.getItem()/cont, 1));
      }
      if (!foundTrueMultiplier)
      {
        index= 1;
        iter2= factors;
        bool foundMultiplier= false;
        for (iter= contents; iter.hasItem(); iter++, iter2++, index++)
        {
          if (fdivides (iter.getItem(), LCmultiplier))
          {
            if ((LCmultiplier/iter.getItem()).inCoeffDomain() &&
                !isOnlyLeadingCoeff(iter2.getItem())) //content divides LCmultiplier completely and factor consists of more terms than just the leading coeff
            {
              int index2= 1;
              for (CFListIterator iter3= leadingCoeffs2[lengthAeval2-1];
                   iter3.hasItem(); iter3++, index2++)
              {
                if (index2 == index)
                {
                  iter3.getItem() /= LCmultiplier;
                  break;
                }
              }
              A /= LCmultiplier;
              foundMultiplier= true;
              iter.getItem()= 1;
            }
          }
        }
        // coming from above: divide out more LCmultiplier if possible
        if (foundMultiplier)
        {
          foundMultiplier= false;
          index=1;
          iter2= factors;
          for (iter= contents; iter.hasItem(); iter++, iter2++, index++)
          {
            if (!iter.getItem().isOne() &&
                fdivides (iter.getItem(), LCmultiplier))
            {
              if (!isOnlyLeadingCoeff (iter2.getItem())) // factor is more than just leading coeff
              {
                int index2= 1;
                for (iter2= leadingCoeffs2[lengthAeval2-1]; iter2.hasItem();
                     iter2++, index2++)
                {
                  if (index2 == index)
                  {
                    iter2.getItem() /= iter.getItem();
                    foundMultiplier= true;
                    break;
                  }
                }
                A /= iter.getItem();
                LCmultiplier /= iter.getItem();
                iter.getItem()= 1;
              }
              else if (fdivides (getVars (LCmultiplier), testVars))//factor consists of just leading coeff
              {
                Variable xx= Variable (2);
                CanonicalForm vars;
                vars= power (xx, degree (LC (getItem(oldBiFactors, index),1),
                                          xx));
                for (int i= 0; i < lengthAeval2; i++)
                {
                  if (oldAeval[i].isEmpty())
                    continue;
                  xx= oldAeval[i].getFirst().mvar();
                  vars *= power (xx, degree (LC (getItem(oldAeval[i], index),1),
                                             xx));
                }
                if (myGetVars(content(getItem(leadingCoeffs2[lengthAeval2-1],index),1))
                    /myGetVars (LCmultiplier) == vars)
                {
                  int index2= 1;
                  for (iter2= leadingCoeffs2[lengthAeval2-1]; iter2.hasItem();
                       iter2++, index2++)
                  {
                    if (index2 == index)
                    {
                      iter2.getItem() /= LCmultiplier;
                      foundMultiplier= true;
                      break;
                    }
                  }
                  A /= LCmultiplier;
                  iter.getItem()= 1;
                }
              }
            }
          }
        }
        else
        {
          CanonicalForm pLCs= prod (LCs);
          if (fdivides (pLCs, LC (oldA,1)) && (LC(oldA,1)/pLCs).inCoeffDomain()) // check if the product of the lead coeffs of the primitive factors equals the lead coeff of the old A
          {
            A= oldA;
            iter2= leadingCoeffs2[lengthAeval2-1];
            for (iter= contents; iter.hasItem(); iter++, iter2++)
              iter2.getItem() /= iter.getItem();
            foundMultiplier= true;
          }
          if (!foundMultiplier && fdivides (getVars (LCmultiplier), testVars))
          {
            Variable xx;
            CFList vars1;
            CFFList sqrfMultiplier= sqrFree (LCmultiplier);
            if (sqrfMultiplier.getFirst().factor().inCoeffDomain())
              sqrfMultiplier.removeFirst();
            sqrfMultiplier= sortCFFListByNumOfVars (sqrfMultiplier);
            xx= Variable (2);
            for (iter= oldBiFactors; iter.hasItem(); iter++)
              vars1.append (power (xx, degree (LC (iter.getItem(),1), xx)));
            for (int i= 0; i < lengthAeval2; i++)
            {
              if (oldAeval[i].isEmpty())
                continue;
              xx= oldAeval[i].getFirst().mvar();
              iter2= vars1;
              for (iter= oldAeval[i]; iter.hasItem(); iter++, iter2++)
                iter2.getItem() *= power(xx,degree (LC (iter.getItem(),1), xx));
            }
            CanonicalForm tmp;
            iter2= vars1;
            for (iter= leadingCoeffs2[lengthAeval2-1]; iter.hasItem(); iter++,
                                                                    iter2++)
            {
              tmp= iter.getItem()/LCmultiplier;
              for (int i=1; i <= tmp.level(); i++)
              {
                if (degree(tmp,i) > 0 &&
                    (degree(iter2.getItem(),i) > degree (tmp,i)))
                  iter2.getItem() /= power (Variable (i), degree (tmp,i));
              }
            }
            int multi;
            for (CFFListIterator ii= sqrfMultiplier; ii.hasItem(); ii++)
            {
              multi= 0;
              for (iter= vars1; iter.hasItem(); iter++)
              {
                tmp= iter.getItem();
                while (fdivides (myGetVars (ii.getItem().factor()), tmp))
                {
                  multi++;
                  tmp /= myGetVars (ii.getItem().factor());
                }
              }
              if (multi == ii.getItem().exp())
              {
                index= 1;
                for (iter= vars1; iter.hasItem(); iter++, index++)
                {
                  while (fdivides (myGetVars(ii.getItem().factor()),
                                   iter.getItem()
                                  )
                        )
                  {
                    int index2= 1;
                    for (iter2= leadingCoeffs2[lengthAeval2-1]; iter2.hasItem();
                         iter2++, index2++)
                    {
                      if (index2 == index)
                        continue;
                      else
                      {
                        tmp= ii.getItem().factor();
                        iter2.getItem() /= tmp;
                        CFListIterator iter3= evaluation;
                        for (int jj= A.level(); jj > 2; jj--, iter3++)
                          tmp= tmp (iter3.getItem(), jj);
                        if (!tmp.inCoeffDomain())
                        {
                          int index3= 1;
                          for (iter3= biFactors; iter3.hasItem(); iter3++,
                                                                  index3++)
                          {
                            if (index3 == index2)
                            {
                              iter3.getItem() /= tmp;
                              iter3.getItem() /= Lc (iter3.getItem());
                              break;
                            }
                          }
                        }
                        A /= ii.getItem().factor();
                      }
                    }
                    iter.getItem() /= getVars (ii.getItem().factor());
                  }
                }
              }
              else
              {
                index= 1;
                for (iter= vars1; iter.hasItem(); iter++, index++)
                {
                  if (!fdivides (myGetVars (ii.getItem().factor()),
                                 iter.getItem()
                                )
                     )
                  {
                    int index2= 1;
                    for (iter2= leadingCoeffs2[lengthAeval2-1]; iter2.hasItem();
                         iter2++, index2++)
                    {
                      if (index2 == index)
                      {
                        tmp= power (ii.getItem().factor(), ii.getItem().exp());
                        iter2.getItem() /= tmp;
                        A /= tmp;
                        CFListIterator iter3= evaluation;
                        for (int jj= A.level(); jj > 2; jj--, iter3++)
                          tmp= tmp (iter3.getItem(), jj);
                        if (!tmp.inCoeffDomain())
                        {
                          int index3= 1;
                          for (iter3= biFactors; iter3.hasItem(); iter3++,
                                                                  index3++)
                          {
                            if (index3 == index2)
                            {
                              iter3.getItem() /= tmp;
                              iter3.getItem() /= Lc (iter3.getItem());
                              break;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }

        // patch everything together again
        leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
        for (int i= lengthAeval2-1; i > -1; i--)
          leadingCoeffs2[i]= CFList();
        prepareLeadingCoeffs (leadingCoeffs2,A.level(),leadingCoeffs, biFactors,
                              evaluation);
        Aeval= evaluateAtEval (A, evaluation, 2);

        hh= 1/Lc (Aeval.getFirst());

        for (CFListIterator i= Aeval; i.hasItem(); i++)
          i.getItem() *= hh;

        A *= hh;
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
    int index;
    Variable xx;
    CFList vars1;
    CFFList sqrfMultiplier= sqrFree (LCmultiplier);
    if (sqrfMultiplier.getFirst().factor().inCoeffDomain())
      sqrfMultiplier.removeFirst();
    sqrfMultiplier= sortCFFListByNumOfVars (sqrfMultiplier);
    xx= Variable (2);
    for (iter= oldBiFactors; iter.hasItem(); iter++)
      vars1.append (power (xx, degree (LC (iter.getItem(),1), xx)));
    for (int i= 0; i < lengthAeval2; i++)
    {
      if (oldAeval[i].isEmpty())
        continue;
      xx= oldAeval[i].getFirst().mvar();
      iter2= vars1;
      for (iter= oldAeval[i]; iter.hasItem(); iter++, iter2++)
        iter2.getItem() *= power (xx, degree (LC (iter.getItem(),1), xx));
    }
    CanonicalForm tmp;
    iter2= vars1;
    for (iter= leadingCoeffs2[lengthAeval2-1]; iter.hasItem(); iter++, iter2++)
    {
      tmp= iter.getItem()/LCmultiplier;
      for (int i=1; i <= tmp.level(); i++)
      {
        if (degree(tmp,i) > 0 && (degree(iter2.getItem(),i) > degree (tmp,i)))
          iter2.getItem() /= power (Variable (i), degree (tmp,i));
      }
    }
    int multi;
    for (CFFListIterator ii= sqrfMultiplier; ii.hasItem(); ii++)
    {
      multi= 0;
      for (iter= vars1; iter.hasItem(); iter++)
      {
        tmp= iter.getItem();
        while (fdivides (myGetVars (ii.getItem().factor()), tmp))
        {
          multi++;
          tmp /= myGetVars (ii.getItem().factor());
        }
      }
      if (multi == ii.getItem().exp())
      {
        index= 1;
        for (iter= vars1; iter.hasItem(); iter++, index++)
        {
          while (fdivides (myGetVars (ii.getItem().factor()), iter.getItem()))
          {
            int index2= 1;
            for (iter2= leadingCoeffs2[lengthAeval2-1];iter2.hasItem();iter2++,
                                                                      index2++)
            {
              if (index2 == index)
                continue;
              else
              {
                tmp= ii.getItem().factor();
                iter2.getItem() /= tmp;
                CFListIterator iter3= evaluation;
                for (int jj= A.level(); jj > 2; jj--, iter3++)
                  tmp= tmp (iter3.getItem(), jj);
                if (!tmp.inCoeffDomain())
                {
                  int index3= 1;
                  for (iter3= biFactors; iter3.hasItem(); iter3++, index3++)
                  {
                    if (index3 == index2)
                    {
                      iter3.getItem() /= tmp;
                      iter3.getItem() /= Lc (iter3.getItem());
                      break;
                    }
                  }
                }
                A /= ii.getItem().factor();
              }
            }
            iter.getItem() /= getVars (ii.getItem().factor());
          }
        }
      }
      else
      {
        index= 1;
        for (iter= vars1; iter.hasItem(); iter++, index++)
        {
          if (!fdivides (myGetVars (ii.getItem().factor()), iter.getItem()))
          {
            int index2= 1;
            for (iter2= leadingCoeffs2[lengthAeval2-1];iter2.hasItem();iter2++,
                                                                      index2++)
            {
              if (index2 == index)
              {
                tmp= power (ii.getItem().factor(), ii.getItem().exp());
                iter2.getItem() /= tmp;
                A /= tmp;
                CFListIterator iter3= evaluation;
                for (int jj= A.level(); jj > 2; jj--, iter3++)
                  tmp= tmp (iter3.getItem(), jj);
                if (!tmp.inCoeffDomain())
                {
                  int index3= 1;
                  for (iter3= biFactors; iter3.hasItem(); iter3++, index3++)
                  {
                    if (index3 == index2)
                    {
                      iter3.getItem() /= tmp;
                      iter3.getItem() /= Lc (iter3.getItem());
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
    for (int i= lengthAeval2-1; i > -1; i--)
      leadingCoeffs2[i]= CFList();
    prepareLeadingCoeffs (leadingCoeffs2,A.level(),leadingCoeffs, biFactors,
                          evaluation);
    Aeval= evaluateAtEval (A, evaluation, 2);

    hh= 1/Lc (Aeval.getFirst());

    for (CFListIterator i= Aeval; i.hasItem(); i++)
      i.getItem() *= hh;

    A *= hh;

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
