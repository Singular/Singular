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
#include "cfNewtonPolygon.h"
#include "facHensel.h"


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
}

void decompress (CFFList& factors, const CFMap& N)
{
  for (CFFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= CFFactor (N (i.getItem().factor()), i.getItem().exp());
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
bool FqInExtensionHelper (const CanonicalForm& F, const CanonicalForm& gamma,
                          const CanonicalForm& delta, CFList& source,
                          CFList& dest)
{
  bool result= false;
  if (F.inBaseDomain())
    return result;
  else if (F.inCoeffDomain())
  {
    if (!fdivides (gamma, F))
      return true;
    else
    {
      int pos= findItem (source, F);
      if (pos > 0)
        return false;
      Variable a;
      hasFirstAlgVar (F, a);
      int bound= ipower (getCharacteristic(), degree (getMipo (a)));
      CanonicalForm buf= 1;
      for (int i= 1; i < bound; i++)
      {
        buf *= gamma;
        if (buf == F)
        {
          source.append (buf);
          dest.append (power (delta, i));
          return false;
        }
      }
      return true;
    }
  }
  else
  {
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      result= FqInExtensionHelper (i.coeff(), gamma, delta, source, dest);
      if (result == true)
        return result;
    }
  }
  return result;
}

bool isInExtension (const CanonicalForm& F, const CanonicalForm& gamma,
                    const int k, const CanonicalForm& delta,
                    CFList& source, CFList& dest)
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
    result= FqInExtensionHelper (F, gamma, delta, source, dest);
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
  if (/*k==0 &&*/ beta == Variable (1))
    return F;
  else /*if (k==0 && beta != Variable (1))*/
    return mapDown (F, imPrimElem, primElem, beta, source, dest);
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
    if (!isInExtension (g, gamma, k, delta, source, dest))
    {
      g= GFMapDown (g, k);
      factors.append (g);
    }
  }
  else if (k == 1)
  {
    if (!isInExtension (g, gamma, k, delta, source, dest));
      factors.append (g);
  }
  else if (!k && beta == Variable (1))
  {
    if (degree (g, alpha) < degMipoBeta)
      factors.append (g);
  }
  else if (!k && beta != Variable (1))
  {
    if (!isInExtension (g, gamma, k, delta, source, dest))
    {
      g= mapDown (g, delta, gamma, alpha, source, dest);
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
    factors.append (mapDown (g, delta, gamma, alpha, source, dest));
  return;
}

void normalize (CFList& factors)
{
  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem() /= Lc(i.getItem());
  return;
}

void normalize (CFFList& factors)
{
  for (CFFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= CFFactor (i.getItem().factor()/Lc(i.getItem().factor()),
                           i.getItem().exp());
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
  int * v= new int [setSize];
  for (int i= 0; i < setSize; i++)
    v[i]= index[i];
  if (subsetSize == 1)
  {
    v[0]= v[0] - 1;
    if (v[0] >= setSize)
    {
      noSubset= true;
      delete [] v;
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
        delete [] v;
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
        delete [] v;
        return;
      }
      for (int i= 1; i < subsetSize - 1; i++)
        v[i]= v[i - 1] + 1;
      v[subsetSize - 1]= v[subsetSize - 2];
    }
  }
  for (int i= 0; i < setSize; i++)
    index[i]= v[i];
  delete [] v;
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

CFArray
logarithmicDerivative (const CanonicalForm& F, const CanonicalForm& G, int l,
                       CanonicalForm& Q
                      )
{
  Variable x= Variable (2);
  Variable y= Variable (1);
  CanonicalForm xToL= power (x, l);
  CanonicalForm q,r;
  CanonicalForm logDeriv;

  q= newtonDiv (F, G, xToL);

  logDeriv= mulMod2 (q, deriv (G, y), xToL);

  logDeriv= swapvar (logDeriv, x, y);
  int j= degree (logDeriv) + 1;
  CFArray result= CFArray (j);
  for (CFIterator i= logDeriv; i.hasTerms() && !logDeriv.isZero(); i++)
  {
    if (i.exp() == j - 1)
    {
      result [j - 1]= swapvar (i.coeff(), x, y);
      j--;
    }
    else
    {
      for (; i.exp() < j - 1; j--)
        result [j - 1]= 0;
      result [j - 1]= swapvar (i.coeff(), x, y);
      j--;
    }
  }
  for (; j > 0; j--)
    result [j - 1]= 0;
  Q= q;
  return result;
}

CFArray
logarithmicDerivative (const CanonicalForm& F, const CanonicalForm& G, int l,
                       int oldL, const CanonicalForm& oldQ, CanonicalForm& Q
                      )
{
  Variable x= Variable (2);
  Variable y= Variable (1);
  CanonicalForm xToL= power (x, l);
  CanonicalForm xToOldL= power (x, oldL);
  CanonicalForm xToLOldL= power (x, l-oldL);
  CanonicalForm q,r;
  CanonicalForm logDeriv;

  CanonicalForm bufF= mod (F, xToL);
  CanonicalForm oldF= mulMod2 (G, oldQ, xToL);
  bufF -= oldF;
  bufF= div (bufF, xToOldL);

  q= newtonDiv (bufF, G, xToLOldL);
  q *= xToOldL;
  q += oldQ;

  logDeriv= mulMod2 (q, deriv (G, y), xToL);

  logDeriv= swapvar (logDeriv, x, y);
  int j= degree (logDeriv) + 1;
  CFArray result= CFArray (j);
  for (CFIterator i= logDeriv; i.hasTerms() && !logDeriv.isZero(); i++)
  {
    if (i.exp() == j - 1)
    {
      result [j - 1]= swapvar (i.coeff(), x, y);
      j--;
    }
    else
    {
      for (; i.exp() < j - 1; j--)
        result [j - 1]= 0;
      result [j - 1]= swapvar (i.coeff(), x, y);
      j--;
    }
  }
  for (; j > 0; j--)
    result [j - 1]= 0;
  Q= q;
  return result;
}

void
writeInMatrix (CFMatrix& M, const CFArray& A, const int column,
               const int startIndex
              )
{
  ASSERT (A.size () - startIndex > 0, "wrong starting index");
  ASSERT (A.size () - startIndex == M.rows(), "wrong starting index");
  ASSERT (column > 0 && column <= M.columns(), "wrong column");
  if (A.size() - startIndex <= 0) return;
  int j= 1;
  for (int i= startIndex; i < A.size(); i++, j++)
    M (j, column)= A [i];
}

CFArray getCoeffs (const CanonicalForm& F, const int k)
{
  ASSERT (F.isUnivariate(), "univariate input expected");
  if (degree (F, 2) < k)
    return CFArray();

  CFArray result= CFArray (degree (F) - k + 1);
  CFIterator j= F;
  for (int i= degree (F); i >= k; i--)
  {
    if (j.exp() == i)
    {
      result [i - k]= j.coeff();
      j++;
      if (!j.hasTerms())
        return result;
    }
    else
      result[i - k]= 0;
  }
  return result;
}

CFArray getCoeffs (const CanonicalForm& F, const int k, const Variable& alpha)
{
  ASSERT (F.isUnivariate(), "univariate input expected");
  if (degree (F, 2) < k)
    return CFArray ();

  int d= degree (getMipo (alpha));
  CFArray result= CFArray ((degree (F) - k + 1)*d);
  CFIterator j= F;
  CanonicalForm buf;
  CFIterator iter;
  for (int i= degree (F); i >= k; i--)
  {
    if (j.exp() == i)
    {
      iter= j.coeff();
      for (int l= degree (j.coeff(), alpha); l >= 0; l--)
      {
        if (iter.exp() == l)
        {
          result [(i - k)*d + l]= iter.coeff();
          iter++;
          if (!iter.hasTerms())
            break;
        }
      }
      j++;
      if (!j.hasTerms())
        return result;
    }
    else
    {
      for (int l= 0; l < d; l++)
        result[(i - k)*d + l]= 0;
    }
  }
  return result;
}

#ifdef HAVE_NTL
CFArray
getCoeffs (const CanonicalForm& G, const int k, const int l, const int degMipo,
           const Variable& alpha, const CanonicalForm& evaluation,
           const mat_zz_p& M)
{
  ASSERT (G.isUnivariate(), "univariate input expected");
  CanonicalForm F= G (G.mvar() - evaluation, G.mvar());
  if (F.isZero())
    return CFArray ();

  Variable y= Variable (2);
  F= F (power (y, degMipo), y);
  F= F (y, alpha);
  zz_pX NTLF= convertFacCF2NTLzzpX (F);
  NTLF.rep.SetLength (l*degMipo);
  NTLF.rep= M*NTLF.rep;
  NTLF.normalize();
  F= convertNTLzzpX2CF (NTLF, y);

  if (degree (F, 2) < k)
    return CFArray();

  CFArray result= CFArray (degree (F) - k + 1);

  CFIterator j= F;
  for (int i= degree (F); i >= k; i--)
  {
    if (j.exp() == i)
    {
      result [i - k]= j.coeff();
      j++;
      if (!j.hasTerms())
        return result;
    }
    else
      result[i - k]= 0;
  }
  return result;
}
#endif

int * computeBounds (const CanonicalForm& F, int& n)
{
  n= degree (F, 1);
  int* result= new int [n];
  int sizeOfNewtonPolygon;
  int** newtonPolyg= newtonPolygon (F, sizeOfNewtonPolygon);

  int minXIndex= 0, minYIndex= 0, maxXIndex= 0, maxYIndex= 0;
  int minX, minY, maxX, maxY;
  minX= newtonPolyg [0] [0];
  minY= newtonPolyg [0] [1];
  maxX= minX;
  maxY= minY;
  for (int i= 1; i < sizeOfNewtonPolygon; i++)
  {
    if (minX > newtonPolyg [i] [0])
    {
      minX= newtonPolyg [i] [0];
      minXIndex= i;
    }
    if (maxX < newtonPolyg [i] [0])
    {
      maxX= newtonPolyg [i] [0];
      maxXIndex= i;
    }
    if (minY > newtonPolyg [i] [1])
    {
      minY= newtonPolyg [i] [1];
      minYIndex= i;
    }
    if (maxY < newtonPolyg [i] [1])
    {
      maxY= newtonPolyg [i] [1];
      maxYIndex= i;
    }
  }

  int k= maxX;
  for (int i= 0; i < n; i++)
  {
    if (i + 1 > maxY || i + 1 < minY)
    {
      result [i]= 0;
      continue;
    }
    int* point= new int [2];
    point [0]= k;
    point [1]= i + 1;
    while (!isInPolygon (newtonPolyg, sizeOfNewtonPolygon, point) && k > 0)
    {
      k--;
      point [0]= k;
    }
    result [i]= k;
    k= maxX;
    delete [] point;
  }

  return result;
}

int
substituteCheck (const CanonicalForm& F, const Variable& x)
{
  if (F.inCoeffDomain())
    return 0;
  if (degree (F, x) < 0)
    return 0;
  CanonicalForm f= swapvar (F, F.mvar(), x);
  int sizef= 0;
  for (CFIterator i= f; i.hasTerms(); i++, sizef++)
  {
    if (i.exp() == 1)
      return 0;
  }
  int * expf= new int [sizef];
  int j= 0;
  for (CFIterator i= f; i.hasTerms(); i++, j++)
    expf [j]= i.exp();

  int indf= sizef - 1;
  if (expf[indf] == 0)
    indf--;

  int result= expf[indf];
  for (int i= indf - 1; i >= 0; i--)
  {
    if (expf [i]%result != 0)
    {
      delete [] expf;
      return 0;
    }
  }

  delete [] expf;
  return result;
}

static int
substituteCheck (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() || G.inCoeffDomain())
    return 0;
  Variable x= Variable (1);
  if (degree (F, x) <= 1 || degree (G, x) <= 1)
    return 0;
  CanonicalForm f= swapvar (F, F.mvar(), x);
  CanonicalForm g= swapvar (G, G.mvar(), x);
  int sizef= 0;
  int sizeg= 0;
  for (CFIterator i= f; i.hasTerms(); i++, sizef++)
  {
    if (i.exp() == 1)
      return 0;
  }
  for (CFIterator i= g; i.hasTerms(); i++, sizeg++)
  {
    if (i.exp() == 1)
      return 0;
  }
  int * expf= new int [sizef];
  int * expg= new int [sizeg];
  int j= 0;
  for (CFIterator i= f; i.hasTerms(); i++, j++)
  {
    expf [j]= i.exp();
  }
  j= 0;
  for (CFIterator i= g; i.hasTerms(); i++, j++)
  {
    expg [j]= i.exp();
  }

  int indf= sizef - 1;
  int indg= sizeg - 1;
  if (expf[indf] == 0)
    indf--;
  if (expg[indg] == 0)
    indg--;

  if ((expg[indg]%expf [indf] != 0 && expf[indf]%expg[indg] != 0) ||
      (expg[indg] == 1 && expf[indf] == 1))
  {
    delete [] expg;
    delete [] expf;
    return 0;
  }

  int result;
  if (expg [indg]%expf [indf] == 0)
    result= expf[indf];
  else
    result= expg[indg];
  for (int i= indf - 1; i >= 0; i--)
  {
    if (expf [i]%result != 0)
    {
      delete [] expf;
      delete [] expg;
      return 0;
    }
  }

  for (int i= indg - 1; i >= 0; i--)
  {
    if (expg [i]%result != 0)
    {
      delete [] expf;
      delete [] expg;
      return 0;
    }
  }

  delete [] expg;
  delete [] expf;
  return result;
}

int recSubstituteCheck (const CanonicalForm& F, const int d)
{
  if (F.inCoeffDomain())
    return 0;
  Variable x= Variable (1);
  if (degree (F, x) <= 1)
    return 0;
  CanonicalForm f= swapvar (F, F.mvar(), x);
  int sizef= 0;
  for (CFIterator i= f; i.hasTerms(); i++, sizef++)
  {
    if (i.exp() == 1)
      return 0;
  }
  int * expf= new int [sizef];
  int j= 0;
  for (CFIterator i= f; i.hasTerms(); i++, j++)
  {
    expf [j]= i.exp();
  }

  int indf= sizef - 1;
  if (expf[indf] == 0)
    indf--;

  if ((d%expf [indf] != 0 && expf[indf]%d != 0) || (expf[indf] == 1))
  {
    delete [] expf;
    return 0;
  }

  int result;
  if (d%expf [indf] == 0)
    result= expf[indf];
  else
    result= d;
  for (int i= indf - 1; i >= 0; i--)
  {
    if (expf [i]%result != 0)
    {
      delete [] expf;
      return 0;
    }
  }

  delete [] expf;
  return result;
}

int substituteCheck (const CFList& L)
{
  ASSERT (L.length() > 1, "expected a list of at least two elements");
  if (L.length() < 2)
    return 0;
  CFListIterator i= L;
  i++;
  int result= substituteCheck (L.getFirst(), i.getItem());
  if (result <= 1)
    return result;
  i++;
  for (;i.hasItem(); i++)
  {
    result= recSubstituteCheck (i.getItem(), result);
    if (result <= 1)
      return result;
  }
  return result;
}

void
subst (const CanonicalForm& F, CanonicalForm& A, const int d, const Variable& x)
{
  if (d <= 1)
  {
    A= F;
    return;
  }
  if (degree (F, x) <= 0)
  {
    A= F;
    return;
  }
  CanonicalForm C= 0;
  CanonicalForm f= swapvar (F, x, F.mvar());
  for (CFIterator i= f; i.hasTerms(); i++)
    C += i.coeff()*power (f.mvar(), i.exp()/ d);
  A= swapvar (C, x, F.mvar());
}

CanonicalForm
reverseSubst (const CanonicalForm& F, const int d, const Variable& x)
{
  if (d <= 1)
    return F;
  if (degree (F, x) <= 0)
    return F;
  CanonicalForm f= swapvar (F, x, F.mvar());
  CanonicalForm result= 0;
  for (CFIterator i= f; i.hasTerms(); i++)
    result += i.coeff()*power (f.mvar(), d*i.exp());
  return swapvar (result, x, F.mvar());
}

void
reverseSubst (CFList& L, const int d, const Variable& x)
{
  for (CFListIterator i= L; i.hasItem(); i++)
    i.getItem()= reverseSubst (i.getItem(), d, x);
}

