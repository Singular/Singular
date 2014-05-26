/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorizeUtil.cc
 *
 * This file provides utility functions for multivariate factorization
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "canonicalform.h"
#include "cf_map.h"
#include "cf_algorithm.h"

static inline
void appendSwap (CFList& factors1, const CFList& factors2, const int
                  swapLevel1, const int swapLevel2, const Variable& x)
{
  for (CFListIterator i= factors2; i.hasItem(); i++)
  {
    if (swapLevel1)
    {
      if (swapLevel2)
        factors1.append (swapvar (swapvar (i.getItem(), x,
                         Variable (swapLevel2)), Variable (swapLevel1), x));
      else
        factors1.append (swapvar (i.getItem(), Variable (swapLevel1), x));
    }
    else
    {
      if (swapLevel2)
        factors1.append (swapvar (i.getItem(), x, Variable (swapLevel2)));
      else
        factors1.append (i.getItem());
    }
  }
  return;
}


void swap (CFList& factors, const int swapLevel1, const int swapLevel2, const
           Variable& x)
{
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (swapLevel1)
    {
      if (swapLevel2)
        i.getItem()= swapvar (swapvar (i.getItem(), x, Variable (swapLevel2)),
                              Variable (swapLevel1), x);
      else
        i.getItem()= swapvar (i.getItem(), Variable (swapLevel1), x);
    }
    else
    {
      if (swapLevel2)
        i.getItem()= swapvar (i.getItem(), x, Variable (swapLevel2));
    }
  }
  return;
}

void appendSwapDecompress (CFList& factors1, const CFList& factors2,
                             const CFMap& N, const int swapLevel, const
                             Variable& x)
{
  for (CFListIterator i= factors1; i.hasItem(); i++)
  {
    if (swapLevel)
      i.getItem()= swapvar (i.getItem(), Variable (swapLevel), x);
    i.getItem()= N(i.getItem());
  }
  for (CFListIterator i= factors2; i.hasItem(); i++)
  {
    if (!i.getItem().inCoeffDomain())
      factors1.append (N (i.getItem()));
  }
  return;
}

void appendSwapDecompress (CFList& factors1, const CFList& factors2,
                             const CFMap& N, const int swapLevel1,
                             const int swapLevel2, const Variable& x)
{
  for (CFListIterator i= factors1; i.hasItem(); i++)
  {
    if (swapLevel1)
    {
      if (swapLevel2)
        i.getItem()=
        N (swapvar (swapvar (i.getItem(), Variable (swapLevel2), x), x,
                    Variable (swapLevel1)));
      else
        i.getItem()= N (swapvar (i.getItem(), x, Variable (swapLevel1)));
    }
    else
    {
      if (swapLevel2)
        i.getItem()= N (swapvar (i.getItem(), Variable (swapLevel2), x));
      else
        i.getItem()= N (i.getItem());
    }
  }
  for (CFListIterator i= factors2; i.hasItem(); i++)
  {
    if (!i.getItem().inCoeffDomain())
      factors1.append (N (i.getItem()));
  }
  return;
}

int* liftingBounds (const CanonicalForm& A, const int& bivarLiftBound)
{
  int j= A.level() - 1;
  int* liftBounds= new int [j];
  liftBounds[0]= bivarLiftBound;
  for (int i= 1; i < j; i++)
  {
    liftBounds[i]= degree (A, Variable (i + 2)) + 1 +
                            degree (LC (A, 1), Variable (i + 2));
  }
  return liftBounds;
}

CanonicalForm shift2Zero (const CanonicalForm& F, CFList& Feval, const CFList& evaluation, int l)
{
  CanonicalForm A= F;
  int k= evaluation.length() + l - 1;
  for (CFListIterator i= evaluation; i.hasItem(); i++, k--)
    A= A (Variable (k) + i.getItem(), k);
  CanonicalForm buf= A;
  Feval= CFList();
  Feval.append (buf);
  for (k= A.level(); k > 2; k--)
  {
    buf= mod (buf, Variable (k));
    Feval.insert (buf);
  }
  return A;
}

CanonicalForm reverseShift (const CanonicalForm& F, const CFList& evaluation, int l)
{
  int k= evaluation.length() + l - 1;
  CanonicalForm result= F;
  CFListIterator j= evaluation;
  for (int i= k; j.hasItem() && (i > l - 1); i--, j++)
  {
    if (F.level() < i)
      continue;
    result= result (Variable (i) - j.getItem(), i);
  }
  return result;
}

bool isOnlyLeadingCoeff (const CanonicalForm& F)
{
  return (F-LC (F,1)*power (Variable(1),degree (F,1))).isZero();
}

/// like getVars but including multiplicities
CanonicalForm myGetVars (const CanonicalForm& F)
{
  CanonicalForm result= 1;
  int deg;
  for (int i= 1; i <= F.level(); i++)
  {
    if ((deg= degree (F, i)) > 0)
      result *= power (Variable (i), deg);
  }
  return result;
}

int compareByNumberOfVars (const CFFactor& F, const CFFactor& G)
{
  return getNumVars (F.factor()) < getNumVars (G.factor());
}

CFFList
sortCFFListByNumOfVars (CFFList& F)
{
    F.sort (compareByNumberOfVars);
    CFFList result= F;
    return result;
}

CFList evaluateAtZero (const CanonicalForm& F)
{
  CFList result;
  CanonicalForm buf= F;
  result.insert (buf);
  for (int i= F.level(); i > 2; i--)
  {
    buf= buf (0, i);
    result.insert (buf);
  }
  return result;
}

CFList evaluateAtEval (const CanonicalForm& F, const CFArray& eval)
{
  CFList result;
  CanonicalForm buf= F;
  result.insert (buf);
  int k= eval.size();
  for (int i= 1; i < k; i++)
  {
    buf= buf (eval[i], i + 2);
    result.insert (buf);
  }
  return result;
}

CFList evaluateAtEval (const CanonicalForm& F, const CFList& evaluation, int l)
{
  CFList result;
  CanonicalForm buf= F;
  result.insert (buf);
  int k= evaluation.length() + l - 1;
  CFListIterator j= evaluation;
  for (int i= k; j.hasItem() && i > l; i--, j++)
  {
    if (F.level() < i)
      continue;
    buf= buf (j.getItem(), i);
    result.insert (buf);
  }
  return result;
}


CFList recoverFactors (const CanonicalForm& F, const CFList& factors)
{
  CFList result;
  CanonicalForm tmp, tmp2;
  CanonicalForm G= F;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    tmp= i.getItem()/content (i.getItem(), 1);
    if (fdivides (tmp, G, tmp2))
    {
      G= tmp2;
      result.append (tmp);
    }
  }
  if (result.length() + 1 == factors.length())
    result.append (G/content (G,1));
  return result;
}

CFList recoverFactors (const CanonicalForm& F, const CFList& factors,
                       const CFList& evaluation)
{
  CFList result;
  CanonicalForm tmp, tmp2;
  CanonicalForm G= F;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    tmp= reverseShift (i.getItem(), evaluation, 2);
    tmp /= content (tmp, 1);
    if (fdivides (tmp, G, tmp2))
    {
      G= tmp2;
      result.append (tmp);
    }
  }
  if (result.length() + 1 == factors.length())
    result.append (G/content (G,1));
  return result;
}

CFList recoverFactors (CanonicalForm& F, const CFList& factors, int* index)
{
  CFList result;
  CanonicalForm tmp, tmp2;
  CanonicalForm G= F;
  int j= 0;
  for (CFListIterator i= factors; i.hasItem(); i++, j++)
  {
    if (i.getItem().isZero())
    {
      index[j]= 0;
      continue;
    }
    tmp= i.getItem();
    if (fdivides (tmp, G, tmp2))
    {
      G= tmp2;
      tmp /=content (tmp, 1);
      result.append (tmp);
      index[j]= 1;
    }
    else
      index[j]= 0;
  }
  if (result.length() + 1 == factors.length())
  {
    result.append (G/content (G,1));
    F= G/content (G,1);
  }
  else
    F= G;
  return result;
}
