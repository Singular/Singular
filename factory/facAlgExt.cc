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
**/
//*****************************************************************************


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "cf_random.h"
#include "cf_algorithm.h"
#include "facFqBivarUtil.h"
#include "facAlgExt.h"
#include "cfModResultant.h"
#include "fac_sqrfree.h"

TIMING_DEFINE_PRINT(fac_alg_resultant)
TIMING_DEFINE_PRINT(fac_alg_norm)
TIMING_DEFINE_PRINT(fac_alg_factor_norm)
TIMING_DEFINE_PRINT(fac_alg_gcd)
TIMING_DEFINE_PRINT(fac_alg_sqrf)
TIMING_DEFINE_PRINT(fac_alg_factor_sqrf)
TIMING_DEFINE_PRINT(fac_alg_time_shift)

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
  TIMING_START (fac_alg_resultant);
  if (degg >= 8 || degmipo >= 8)
    norm= resultantZ (g, mipo, x);
  else
    norm= resultant (g, mipo, x);
  TIMING_END_AND_PRINT (fac_alg_resultant, "time to compute resultant0: ");

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
        TIMING_START (fac_alg_resultant);
        if (degg >= 8 || degmipo >= 8)
          norm= resultantZ (g (x, alpha), mipo, x);
        else
          norm= resultant (g (x, alpha), mipo, x);
        TIMING_END_AND_PRINT (fac_alg_resultant,"time to compute resultant1: ");
      }
      else
      {
        g= F (y + i*alpha, y);
        g *= bCommonDen (g);
        TIMING_START (fac_alg_resultant);
        if (degg >= 8 || degmipo >= 8)
          norm= resultantZ (g (x, alpha), mipo, x);
        else
          norm= resultant (g (x, alpha), mipo, x);
        TIMING_END_AND_PRINT (fac_alg_resultant,"time to compute resultant2: ");
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
  TIMING_START (fac_alg_norm);
  CanonicalForm norm= sqrfNorm (f, alpha, shift);
  TIMING_END_AND_PRINT (fac_alg_norm, "time to compute sqrf norm: ");
  ASSERT (degree (norm, alpha) <= 0, "wrong norm computed");
  TIMING_START (fac_alg_factor_norm);
  bool save_sort= !isOn (SW_USE_NTL_SORT);
  On (SW_USE_NTL_SORT);
  CFFList normFactors= factorize (norm);
  if (save_sort)
    Off (SW_USE_NTL_SORT);
  TIMING_END_AND_PRINT (fac_alg_factor_norm, "time to factor norm: ");
  CFList factors;
  if (normFactors.length() <= 2)
  {
    if (save_rat) Off(SW_RATIONAL);
    return CFList (F);
  }

  normFactors.removeFirst();
  CFFListIterator i= normFactors;
  CanonicalForm buf;
  bool shiftBuf= false;
  if (!(normFactors.length() == 2 && degree (i.getItem().factor()) <= degree (f)))
  {
    TIMING_START (fac_alg_time_shift);
    if (shift != 0)
      buf= f (f.mvar() - shift*alpha, f.mvar());
    else
      buf= f;
    shiftBuf= true;
    TIMING_END_AND_PRINT (fac_alg_time_shift, "time to shift: ");
  }
  else
    buf= f;
  CanonicalForm factor;
  int count= 0;
  for (; i.hasItem(); i++)
  {
    ASSERT (i.getItem().exp() == 1, "norm not squarefree");
    TIMING_START (fac_alg_gcd);
    if (shiftBuf)
      factor= gcd (buf, i.getItem().factor());
    else
    {
      if (shift == 0)
        factor= gcd (buf, i.getItem().factor());
      else
        factor= gcd (buf, i.getItem().factor() (f.mvar() + shift*alpha, f.mvar()));
    }
    buf /= factor;
    if (shiftBuf)
    {
      if (shift != 0)
        factor= factor (f.mvar() + shift*alpha, f.mvar());
    }
    TIMING_END_AND_PRINT (fac_alg_gcd, "time to recover factors: ");
    factors.append (factor);
    count++;
    if (normFactors.length() - 1 == count)
    {
      if (shiftBuf)
        factors.append (buf (f.mvar() + shift*alpha, f.mvar()));
      else
        factors.append (buf);
      buf= 1;
      break;
    }
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

  bool save_rat=!isOn (SW_RATIONAL);
  On (SW_RATIONAL);
  TIMING_START (fac_alg_sqrf);
  CFFList sqrf= sqrFreeZ (F);
  TIMING_END_AND_PRINT (fac_alg_sqrf, "time for sqrf factors in Q(a)[x]: ");
  CFList factorsSqrf;
  CFFList factors;
  CFListIterator j;

  CanonicalForm lcinv;
  for (CFFListIterator i= sqrf; i.hasItem(); i++)
  {
    if (i.getItem().factor().inCoeffDomain()) continue;
    TIMING_START (fac_alg_factor_sqrf);
    factorsSqrf= AlgExtSqrfFactorize (i.getItem().factor(), alpha);
    TIMING_END_AND_PRINT (fac_alg_factor_sqrf,
                          "time to factor sqrf factors in Q(a)[x]: ");
    for (j= factorsSqrf; j.hasItem(); j++)
    {
      lcinv= 1/Lc (j.getItem());
      factors.append (CFFactor (j.getItem()*lcinv, i.getItem().exp()));
    }
  }

  factors.insert (CFFactor (Lc(F), 1));
  if (save_rat) Off(SW_RATIONAL);
  return factors;
}

