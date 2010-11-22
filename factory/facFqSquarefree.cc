/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqSquarefree.cc
 *
 * This file provides functions for squarefrees factorizing over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF, as decribed in "Factoring
 * multivariate polynomials over a finite field" by L. Bernardin
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "canonicalform.h"

#include "cf_gcd_smallp.h"
#include "cf_iter.h"
#include "cf_map.h"
#include "cf_util.h"
#include "cf_factory.h"
#include "facFqSquarefree.h"

static inline
CanonicalForm
pthRoot (const CanonicalForm & F, const int & q)
{
  CanonicalForm A= F;
  int p= getCharacteristic ();
  if (A.inCoeffDomain())
  {
    A= power (A, q/p);
    return A;
  }
  else
  {
    CanonicalForm buf= 0;
    for (CFIterator i= A; i.hasTerms(); i++)
      buf= buf + power(A.mvar(), i.exp()/p)*pthRoot (i.coeff(), q);
    return buf;
  }
}

CanonicalForm
maxpthRoot (const CanonicalForm & F, const int & q, int& l)
{
  CanonicalForm result= F;
  bool derivZero= true;
  l= 0;
  while (derivZero)
  {
    for (int i= 1; i <= result.level(); i++)
    {
      if (!deriv (result, Variable (i)).isZero())
      {
        derivZero= false;
        break;
      }
    }
    if (!derivZero)
      break;
    result= pthRoot (result, q);
    l++;
  }
  return result;
}

static inline
CFFList
sqrfPosDer (const CanonicalForm & F, const Variable & x, const int & k,
            const Variable & alpha, CanonicalForm & c)
{
  Variable buf= alpha;
  CanonicalForm b= deriv (F, x);
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  if (GF)
    c= GCD_GF (F, b);
  else if (GF == false && k != 1)
    c= GCD_Fp_extension (F, b, buf);
  else
    c= GCD_small_p (F, b);
  CanonicalForm w= F/c;
  CanonicalForm v= b/c;
  CanonicalForm u= v - deriv (w, x);
  int j= 1;
  int p= getCharacteristic();
  CanonicalForm g;
  CFFList result;
  while (j < p - 1 && degree(u) >= 0)
  {
    if (GF)
      g= GCD_GF (w, u);
    else if (GF == false && k != 1)
      g= GCD_Fp_extension (w, u, buf);
    else
      g= GCD_small_p (w, u);
    if (degree(g) > 0)
      result.append (CFFactor (g, j));
    w= w/g;
    c= c/w;
    v= u/g;
    u= v - deriv (w, x);
    j++;
  }
  if (degree(w) > 0)
    result.append (CFFactor (w, j));
  return result;
}

CFFList
squarefreeFactorization (const CanonicalForm & F, const Variable & alpha)
{
  int p= getCharacteristic();
  CanonicalForm A= F;
  CFMap M;
  A= compress (A, M);
  Variable x= A.mvar();
  int l= x.level();
  int k;
  bool GF= false;
  if (CFFactory::gettype() == GaloisFieldDomain)
  {
    k= getGFDegree();
    GF= true;
  }
  if (alpha.level() != 1)
    k= degree (getMipo (alpha));
  else
    k= 1;
  Variable buf, buf2;
  CanonicalForm tmp;

  CFFList tmp1, tmp2;
  bool found;
  for (int i= l; i > 0; i--)
  {
    buf= Variable (i);
    if (degree (deriv (A, buf)) >= 0)
    {
      if (GF)
        tmp1= sqrfPosDer (A, buf, k, alpha, tmp);
      else if (GF == false && alpha.level() != 1)
        tmp1= sqrfPosDer (A, buf, k, alpha, tmp);
      else
        tmp1= sqrfPosDer (A, buf, 1, alpha, tmp);
      A= tmp;
      for (CFFListIterator j= tmp1; j.hasItem(); j++)
      {
        found= false;
        CFFListIterator k= tmp2;
        if (!k.hasItem()) tmp2.append (j.getItem());
        else
        {
          for (; k.hasItem(); k++)
          {
            if (k.getItem().exp() == j.getItem().exp())
            {
              k.getItem()= CFFactor (k.getItem().factor()*j.getItem().factor(),
                                     j.getItem().exp());
              found= true;
            }
          }
          if (found == false)
            tmp2.append(j.getItem());
        }
      }
    }
  }

  bool degcheck= false;;
  for (int i= l; i > 0; i--)
  if (degree (A, Variable(i)) >= p)
    degcheck= true;

  if (degcheck == false && tmp1.isEmpty() && tmp2.isEmpty())
    return CFFList (CFFactor (F/Lc(F), 1));

  CanonicalForm buffer= pthRoot (A, ipower (p, k));

  tmp1= squarefreeFactorization (buffer, alpha);

  CFFList result;
  buf= alpha;
  for (CFFListIterator i= tmp2; i.hasItem(); i++)
  {
    for (CFFListIterator j= tmp1; j.hasItem(); j++)
    {
      if (GF)
        tmp= GCD_GF (i.getItem().factor(), j.getItem().factor());
      else if (GF == false && buf == Variable (1))
        tmp= GCD_small_p (i.getItem().factor(), j.getItem().factor());
      else
        tmp= GCD_Fp_extension (i.getItem().factor(),j.getItem().factor(),buf);
      i.getItem()= CFFactor (i.getItem().factor()/tmp, i.getItem().exp());
      j.getItem()= CFFactor (j.getItem().factor()/tmp, j.getItem().exp());
      if (degree (tmp) > 0 && tmp.level() > 0)
        result.append (CFFactor (M (tmp),
                       j.getItem().exp()*p + i.getItem().exp()));
    }
  }
  for (CFFListIterator i= tmp2; i.hasItem(); i++)
    if (degree (i.getItem().factor()) > 0 && i.getItem().factor().level() >= 0)
      result.append (CFFactor (M (i.getItem().factor()), i.getItem().exp()));
  for (CFFListIterator j= tmp1; j.hasItem(); j++)
    if (degree (j.getItem().factor()) > 0 && j.getItem().factor().level() >= 0)
      result.append (CFFactor (M (j.getItem().factor()), j.getItem().exp()*p));
  return result;
}

CanonicalForm
sqrfPart (const CanonicalForm& F, CanonicalForm& pthPower,
          const Variable& alpha)
{
  if (F.inCoeffDomain())
  {
    pthPower= 1;
    return F;
  }
  CFMap M;
  CanonicalForm A= compress (F, M);
  Variable vBuf= alpha;
  CanonicalForm w, v, b;
  pthPower= 1;
  CanonicalForm result;
  int i= 1;
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  bool allZero= true;
  for (; i <= A.level(); i++)
  {
    if (!deriv (A, Variable (i)).isZero())
    {
      allZero= false;
      break;
    }
  }
  if (allZero)
  {
    pthPower= F;
    return 1;
  }
  if (GF)
    w= GCD_GF (A, deriv (A, Variable (i)));
  else if (GF == false && alpha == Variable (1))
    w= GCD_small_p (A, deriv (A, Variable (i)));
  else
    w= GCD_Fp_extension (A, deriv (A, Variable (i)), vBuf);

  b= A/w;
  result= b;
  if (degree (w) < 1)
    return M (result);
  i++;
  for (; i <= A.level(); i++)
  {
    if (!deriv (w, Variable (i)).isZero())
    {
      b= w;
      if (GF)
        w= GCD_GF (w, deriv (w, Variable (i)));
      else if (GF == false && vBuf == Variable (1))
        w= GCD_small_p (w, deriv (w, Variable (i)));
      else
        w= GCD_Fp_extension (w, deriv (w, Variable (i)), vBuf);
      b /= w;
      if (degree (b) < 1)
        break;
      CanonicalForm g;
      if (GF)
        g= GCD_GF (b, result);
      else if (GF == false && vBuf == Variable (1))
        g= GCD_small_p (b, result);
      else
        g= GCD_Fp_extension (b, result, vBuf);
      if (degree (g) > 0)
        result *= b/g;
      if (degree (g) <= 0)
        result *= b;
    }
  }
  result= M (result);
  return result;
}

