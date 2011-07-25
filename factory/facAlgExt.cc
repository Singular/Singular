// -*- c++ -*-
//*****************************************************************************
/** @file facAlgExt.cc
 *
 * @author Martin Lee
 * @date
 *
 * Univariate factorization over algebraic extension of Q using Trager's
 * algorithm
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "cf_random.h"
#include "cf_algorithm.h"
#include "facFqBivarUtil.h"
#include "facAlgExt.h"
#include "cfModResultant.h"

// squarefree part of F
CanonicalForm
uniSqrfPart (const CanonicalForm& F)
{
  ASSERT (F.isUnivariate(), "univariate input expected");
  ASSERT (getCharacteristic() == 0, "characteristic 0 expected");
  CanonicalForm G= deriv (F, F.mvar());
  G= gcd (F, G);
  return F/G;
}

// i is an integer such that Norm (F (x-i*alpha)) is squarefree
CanonicalForm sqrfNorm (const CanonicalForm& F, const Variable& alpha, int& i)
{
  Variable x= Variable (F.level() + 1);
  Variable y= F.mvar();
  CanonicalForm g= F (x, alpha);
  CanonicalForm mipo= getMipo (alpha);
  mipo= mipo (x, alpha);
  mipo *= bCommonDen (mipo);

  int degg= degree (g);
  int degmipo= degree (mipo);
  CanonicalForm norm;
  if (degg >= 8 || degmipo >= 8)
    norm= resultantZ (g, mipo, x);
  else
    norm= resultant (g, mipo, x);

  i= 0;
  int k;
  if (degree (gcd (deriv (norm, y), norm)) <= 0)
    return norm;
  i= 1;
  do
  {
    k= 1;
    while (k < 3)
    {
      if (k == 1)
      {
        g= F (y - i*alpha, y);
        g *= bCommonDen (g);
        if (degg >= 8 || degmipo >= 8)
          norm= resultantZ (g (x, alpha), mipo, x);
        else
          norm= resultant (g (x, alpha), mipo, x);
      }
      else
      {
        g= F (y + i*alpha, y);
        g *= bCommonDen (g);
        if (degg >= 8 || degmipo >= 8)
          norm= resultantZ (g (x, alpha), mipo, x);
        else
          norm= resultant (g (x, alpha), mipo, x);
      }
      if (degree (gcd (deriv (norm, y), norm)) <= 0)
      {
        if (k == 2)
          i= -i;
        return norm;
      }
      k++;
    }
    i++;
  } while (1);
}

CFList
AlgExtSqrfFactorize (const CanonicalForm& F, const Variable& alpha)
{
  ASSERT (F.isUnivariate(), "univariate input expected");
  ASSERT (getCharacteristic() == 0, "characteristic 0 expected");

  bool save_rat=!isOn (SW_RATIONAL);
  On (SW_RATIONAL);
  CanonicalForm f= F*bCommonDen (F);
  int shift;
  CanonicalForm norm= sqrfNorm (f, alpha, shift);
  ASSERT (degree (norm, alpha) <= 0, "wrong norm computed");
  CFFList normFactors= factorize (norm);
  CFList factors;
  if (normFactors.length() <= 2)
  {
    if (save_rat) Off(SW_RATIONAL);
    return CFList (F);
  }

  normFactors.removeFirst();
  CanonicalForm buf;
  if (shift != 0)
    buf= f (f.mvar() - shift*alpha, f.mvar());
  else
    buf= f;
  CanonicalForm factor;
  for (CFFListIterator i= normFactors; i.hasItem(); i++)
  {
    ASSERT (i.getItem().exp() == 1, "norm not squarefree");
    factor= gcd (buf, i.getItem().factor());
    buf /= factor;
    if (shift != 0)
      factor= factor (f.mvar() + shift*alpha, f.mvar());
    factors.append (factor);
  }
  ASSERT (degree (buf) <= 0, "incomplete factorization");
  if (save_rat) Off(SW_RATIONAL);
  return factors;
}

CFFList
AlgExtFactorize (const CanonicalForm& F, const Variable& alpha)
{
  ASSERT (F.isUnivariate(), "univariate input expected");
  ASSERT (getCharacteristic() == 0, "characteristic 0 expected");


  if (F.inCoeffDomain())
    return CFFList (CFFactor (F, 1));

  CanonicalForm sqrf= uniSqrfPart (F);
  CFList sqrfFactors= AlgExtSqrfFactorize (sqrf, alpha);

  bool save_rat=!isOn (SW_RATIONAL);
  On (SW_RATIONAL);
  CanonicalForm quot, buf= F/Lc (F);
  CFFList factors;
  int multi;
  for (CFListIterator i= sqrfFactors; i.hasItem(); i++)
  {
    multi= 0;
    i.getItem() /= Lc (i.getItem()); //make factors monic
    while (fdivides (i.getItem(), buf, quot))
    {
      buf= quot;
      multi++;
    }
    factors.append (CFFactor (i.getItem(), multi));
  }
  factors.insert (CFFactor (Lc(F), 1));
  ASSERT (degree (buf) <= 0, "bug in AlgExtFactorize");
  if (save_rat) Off(SW_RATIONAL);
  return factors;
}

