/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorizeUtil.cc
 *
 * This file provides utility functions for multivariate factorization
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "canonicalform.h"
#include "cf_map.h"

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

CanonicalForm shift2Zero (const CanonicalForm& F, CFList& Feval, const CFList& evaluation)
{
  CanonicalForm A= F;
  int k= A.level();
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

CanonicalForm reverseShift (const CanonicalForm& F, const CFList& evaluation)
{
  int l= evaluation.length() + 1;
  CanonicalForm result= F;
  CFListIterator j= evaluation;
  for (int i= l; i > 1; i--, j++)
  {
    if (F.level() < i)
      continue;
    result= result (Variable (i) - j.getItem(), i);
  }
  return result;
}


