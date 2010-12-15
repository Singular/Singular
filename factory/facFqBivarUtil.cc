/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqBivarUtil.cc
 *
 * This file provides utility functions for bivariate factorization
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "cf_map.h"
#include "cf_map_ext.h"
#include "templates/ftmpl_functions.h"
#include "ExtensionInfo.h"
#include "cf_algorithm.h"
#include "cf_factory.h"
#include "cf_util.h"
#include "imm.h"
#include "cf_iter.h"
#include "facFqBivarUtil.h"


void append (CFList& factors1, const CFList& factors2)
{
  for (CFListIterator i= factors2; i.hasItem(); i++)
  {
    if (!i.getItem().inCoeffDomain())
      factors1.append (i.getItem());
  }
  return;
}

void decompress (CFList& factors, const CFMap& N)
{
  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= N (i.getItem());
  return;
}

void appendSwapDecompress (CFList& factors1, const CFList& factors2,
                           const CFList& factors3, const bool swap1,
                           const bool swap2, const CFMap& N)
{
  Variable x= Variable (1);
  Variable y= Variable (2);
  for (CFListIterator i= factors1; i.hasItem(); i++)
  {
    if (swap1)
    {
      if (!swap2)
        i.getItem()= swapvar (i.getItem(), x, y);
    }
    else
    {
      if (swap2)
        i.getItem()= swapvar (i.getItem(), y, x);
    }
    i.getItem()= N (i.getItem());
  }
  for (CFListIterator i= factors2; i.hasItem(); i++)
    factors1.append (N (i.getItem()));
  for (CFListIterator i= factors3; i.hasItem(); i++)
    factors1.append (N (i.getItem()));
  return;
}

void swapDecompress (CFList& factors, const bool swap, const CFMap& N)
{
  Variable x= Variable (1);
  Variable y= Variable (2);
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (swap)
      i.getItem()= swapvar (i.getItem(), x, y);
    i.getItem()= N (i.getItem());
  }
  return;
}

static inline
bool GFInExtensionHelper (const CanonicalForm& F, const int number)
{
  if (F.isOne()) return false;
  InternalCF* buf;
  int exp;
  bool result= false;
  if (F.inBaseDomain())
  {
    buf= F.getval();
    exp= imm2int (buf);
    if (exp%number != 0)
      return true;
    else
      return result;
  }
  else
  {
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      result= GFInExtensionHelper (i.coeff(), number);
      if (result == true)
        return result;
    }
  }
  return result;
}

static inline
bool FqInExtensionHelper (const CanonicalForm& F, const CanonicalForm& gamma)
{
  bool result= false;
  if (F.inBaseDomain())
    return result;
  else if (F.inCoeffDomain())
  {
    if (!fdivides (gamma, F))
      return true;
    else
      return result;
  }
  else
  {
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      result= FqInExtensionHelper (i.coeff(), gamma);
      if (result == true)
        return result;
    }
  }
  return result;
}

bool isInExtension (const CanonicalForm& F, const CanonicalForm& gamma,
                    const int k)
{
  bool result;
  if (CFFactory::gettype() == GaloisFieldDomain)
  {
    int p= getCharacteristic();
    int orderFieldExtension= ipower (p, getGFDegree()) - 1;
    int order= ipower (p, k) - 1;
    int number= orderFieldExtension/order;
    result= GFInExtensionHelper (F, number);
    return result;
  }
  else
  {
    result= FqInExtensionHelper (F, gamma);
    return result;
  }
}

CanonicalForm
mapDown (const CanonicalForm& F, const ExtensionInfo& info, CFList& source,
         CFList& dest)
{
  int k= info.getGFDegree();
  Variable beta= info.getAlpha();
  CanonicalForm primElem= info.getGamma();
  CanonicalForm imPrimElem= info.getDelta();
  if (k > 1)
    return GFMapDown (F, k);
  else if (k == 1)
    return F;
  else if (!k && beta == Variable (1))
    return F;
  else if (!k && beta != Variable (1))
    return mapDown (F, primElem, imPrimElem, beta, source, dest);
}

void appendTestMapDown (CFList& factors, const CanonicalForm& f,
                        const ExtensionInfo& info, CFList& source, CFList& dest)
{
  int k= info.getGFDegree();
  Variable beta= info.getBeta();
  Variable alpha= info.getAlpha();
  CanonicalForm delta= info.getDelta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm g= f;
  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));
  if (k > 1)
  {
    if (!isInExtension (g, delta, k))
    {
      g= GFMapDown (g, k);
      factors.append (g);
    }
  }
  else if (k == 1)
  {
    if (!isInExtension (g, delta, k));
      factors.append (g);
  }
  else if (!k && beta == Variable (1))
  {
    if (degree (g, alpha) < degMipoBeta)
      factors.append (g);
  }
  else if (!k && beta != Variable (1))
  {
    if (!isInExtension (g, delta, k))
    {
      g= mapDown (g, gamma, delta, alpha, source, dest);
      factors.append (g);
    }
  }
  return;
}

void
appendMapDown (CFList& factors, const CanonicalForm& g,
               const ExtensionInfo& info, CFList& source, CFList& dest)
{
  int k= info.getGFDegree();
  Variable beta= info.getBeta();
  Variable alpha= info.getAlpha();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  if (k > 1)
    factors.append (GFMapDown (g, k));
  else if (k == 1)
    factors.append (g);
  else if (!k && beta == Variable (1))
    factors.append (g);
  else if (!k && beta != Variable (1))
    factors.append (mapDown (g, gamma, delta, alpha, source, dest));
  return;
}

void normalize (CFList& factors)
{
  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem() /= Lc(i.getItem());
  return;
}

CFList subset (int index [], const int& s, const CFArray& elements,
               bool& noSubset)
{
  int r= elements.size();
  int i= 0;
  CFList result;
  noSubset= false;
  if (index[s - 1] == 0)
  {
    while (i < s)
    {
      index[i]= i + 1;
      result.append (elements[i]);
      i++;
    }
    return result;
  }
  int buf;
  int k;
  bool found= false;
  if (index[s - 1] == r)
  {
    if (index[0] == r - s + 1)
    {
      noSubset= true;
      return result;
    }
    else {
      while (found == false)
      {
        if (index[s - 2 - i] < r - i - 1)
          found= true;
        i++;
      }
      buf= index[s - i - 1];
      k= 0;
      while (s - i - 1 + k < s)
      {
        index[s - i - 1 + k]= buf + k + 1;
        k++;
      }
    }
    for (int j= 0; j < s; j++)
      result.append (elements[index[j] - 1]);
    return result;
  }
  else
  {
    index[s - 1] += 1;
    for (int j= 0; j < s; j++)
      result.append (elements[index[j] - 1]);
    return result;
  }
}

CFArray copy (const CFList& list)
{
  CFArray array= CFArray (list.length());
  int j= 0;
  for (CFListIterator i= list; i.hasItem(); i++, j++)
    array[j]= i.getItem();
  return array;
}

void indexUpdate (int index [], const int& subsetSize, const int& setSize,
                   bool& noSubset)
{
  noSubset= false;
  if (subsetSize > setSize)
  {
    noSubset= true;
    return;
  }
  int v [setSize];
  for (int i= 0; i < setSize; i++)
    v[i]= index[i];
  if (subsetSize == 1)
  {
    v[0]= v[0] - 1;
    if (v[0] >= setSize)
    {
      noSubset= true;
      return;
    }
  }
  else
  {
    if (v[subsetSize - 1] - v[0] + 1 == subsetSize && v[0] > 1)
    {
      if (v[0] + subsetSize - 1 > setSize)
      {
        noSubset= true;
        return;
      }
      v[0]= v[0] - 1;
      for (int i= 1; i < subsetSize - 1; i++)
        v[i]= v[i - 1] + 1;
      v[subsetSize - 1]= v[subsetSize - 2];
    }
    else
    {
      if (v[0] + subsetSize - 1 > setSize)
      {
        noSubset= true;
        return;
      }
      for (int i= 1; i < subsetSize - 1; i++)
        v[i]= v[i - 1] + 1;
      v[subsetSize - 1]= v[subsetSize - 2];
    }
  }
  for (int i= 0; i < setSize; i++)
    index[i]= v[i];
  return;
}

int subsetDegree (const CFList& S)
{
  int result= 0;
  for (CFListIterator i= S; i.hasItem(); i++)
    result += degree (i.getItem(), Variable (1));
  return result;
}

CFFList multiplicity (CanonicalForm& F, const CFList& factors)
{
  if (F.inCoeffDomain())
    return CFFList (CFFactor (F, 1));
  CFFList result;
  int multi= 0;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    while (fdivides (i.getItem(), F))
    {
      multi++;
      F /= i.getItem();
    }
    if (multi > 0)
      result.append (CFFactor (i.getItem(), multi));
    multi= 0;
  }
  return result;
}

