/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.cc
 *
 * This file implements functions for factoring a multivariate polynomial over
 * a finite field.
 *
 * ABSTRACT: "Efficient Multivariate Factorization over Finite Fields" by
 * L. Bernardin & M. Monagon. Precomputation of leading coefficients is 
 * described in "Sparse Hensel lifting" by E. Kaltofen
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "facFqFactorizeUtil.h"
#include "facFqFactorize.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_gcd_smallp.h"
#include "cf_map_ext.h"
#include "algext.h"

#ifdef HAVE_NTL
#include <NTL/ZZ_pEX.h>
#include "NTLconvert.h"

TIMING_DEFINE_PRINT(fac_bi_factorizer);
TIMING_DEFINE_PRINT(fac_hensel_lift);
TIMING_DEFINE_PRINT(fac_factor_recombination);

static inline
CanonicalForm
listGCD (const CFList& L);

static inline
CanonicalForm
myContent (const CanonicalForm& F)
{
  CanonicalForm G= swapvar (F, F.mvar(), Variable (1));
  CFList L;
  for (CFIterator i= G; i.hasTerms(); i++)
    L.append (i.coeff());
  if (L.length() == 2)
    return swapvar (gcd (L.getFirst(), L.getLast()), F.mvar(), Variable (1));
  if (L.length() == 1)
    return LC (F, Variable (1));
  return swapvar (listGCD (L), F.mvar(), Variable (1));
}

static inline
CanonicalForm
listGCD (const CFList& L)
{
  if (L.length() == 1)
    return L.getFirst();
  if (L.length() == 2)
    return gcd (L.getFirst(), L.getLast());
  else
  {
    CFList lHi, lLo;
    CanonicalForm resultHi, resultLo;
    int length= L.length()/2;
    int j= 0;
    for (CFListIterator i= L; j < length; i++, j++)
      lHi.append (i.getItem());
    lLo= Difference (L, lHi);
    resultHi= listGCD (lHi);
    resultLo= listGCD (lLo);
    if (resultHi.isOne() || resultLo.isOne())
      return 1;
    return gcd (resultHi, resultLo);
  }
}

static inline
CanonicalForm
myContent (const CanonicalForm& F, const Variable& x)
{
  if (degree (F, x) <= 0)
    return 1;
  CanonicalForm G= F;
  bool swap= false;
  if (x != F.mvar())
  {
    swap= true;
    G= swapvar (F, x, F.mvar());
  }
  CFList L;
  Variable alpha;
  for (CFIterator i= G; i.hasTerms(); i++)
    L.append (i.coeff());
  if (L.length() == 2)
  {
    if (swap)
      return swapvar (gcd (L.getFirst(), L.getLast()), F.mvar(), x);
    else
      return gcd (L.getFirst(), L.getLast());
  }
  if (L.length() == 1)
  {
    return LC (F, x);
  }
  if (swap)
    return swapvar (listGCD (L), F.mvar(), x);
  else
    return listGCD (L);
}

CanonicalForm myCompress (const CanonicalForm& F, CFMap& N)
{
  int n= F.level();
  int * degsf= new int [n + 1];
  int ** swap;
  swap= new int* [n + 1];
  for (int i= 0; i <= n; i++)
  {
    degsf[i]= 0;
    swap [i]= new int [2];
    swap [i] [0]= 0;
    swap [i] [1]= 0;
  }
  int i= 1;
  n= 1;
  degsf= degrees (F, degsf);

  CanonicalForm result= F;
  while ( i <= F.level() )
  {
    while( degsf[i] == 0 ) i++;
    swap[n][0]= i;
    swap[n][1]= degsf[i];
    if (i != n)
      result= swapvar (result, Variable (n), Variable(i));
    n++; i++;
  }

  int buf1, buf2;
  n--;

  for (i= 1; i < n; i++)
  {
    for (int j= 1; j < n - i + 1; j++)
    {
      if (swap[j][1] < swap[j + 1][1])
      {
        buf1= swap [j + 1] [0];
        buf2= swap [j + 1] [1];
        swap[j + 1] [0]= swap[j] [0];
        swap[j + 1] [1]= swap[j] [1];
        swap[j][0]= buf1;
        swap[j][1]= buf2;
        result= swapvar (result, Variable (j + 1), Variable (j));
      }
    }
  }

  for (i= n; i > 0; i--)
  {
    if (i != swap[i] [0])
      N.newpair (Variable (i), Variable (swap[i] [0]));
  }

  for (i= 0; i <= n; i++)
    delete [] swap[i];
  delete [] swap;

  delete [] degsf;

  return result;
}

CFList
extFactorRecombination (const CFList& factors, const CanonicalForm& F,
                        const CFList& M, const ExtensionInfo& info,
                        const CFList& evaluation)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  CFList source, dest;
  if (factors.length() == 1)
  {
    CanonicalForm buf= reverseShift (F, evaluation);
    return CFList (mapDown (buf, info, source, dest));
  }
  if (factors.length() < 1)
    return CFList();

  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList T, S;
  T= factors;

  int s= 1;
  CFList result;
  CanonicalForm buf;

  buf= F;

  CanonicalForm g, LCBuf= LC (buf, Variable (1));
  CanonicalForm buf2;
  int * v= new int [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  TT= copy (factors);
  bool recombination= false;
  bool trueFactor= false;
  while (T.length() >= 2*s)
  {
    while (noSubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombination)
        {
          T.insert (LCBuf);
          g= prodMod (T, M);
          T.removeFirst();
          result.append (g/myContent (g));
          g= reverseShift (g, evaluation);
          g /= Lc (g);
          appendTestMapDown (result, g, info, source, dest);
          return result;
        }
        else
        {
          buf= reverseShift (buf, evaluation);
          return CFList (buf);
        }
      }

      S= subset (v, s, TT, noSubset);
      if (noSubset) break;

      S.insert (LCBuf);
      g= prodMod (S, M);
      S.removeFirst();
      g /= myContent (g);
      if (fdivides (g, buf))
      {
        buf2= reverseShift (g, evaluation);
        buf2 /= Lc (buf2);
        if (!k && beta == Variable (1))
        {
          if (degree (buf2, alpha) < degMipoBeta)
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf /= g;
            LCBuf= LC (buf, Variable (1));
            recombination= true;
            trueFactor= true;
          }
        }
        else
        {
          if (!isInExtension (buf2, gamma, k, delta, source, dest))
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf /= g;
            LCBuf= LC (buf, Variable (1));
            recombination= true;
            trueFactor= true;
          }
        }

        if (trueFactor)
        {
          T= Difference (T, S);

          if (T.length() < 2*s || T.length() == s)
          {
            buf= reverseShift (buf, evaluation);
            buf /= Lc (buf);
            appendTestMapDown (result, buf, info, source, dest);
            delete [] v;
            return result;
          }
          trueFactor= false;
          TT= copy (T);
          indexUpdate (v, s, T.length(), noSubset);
          if (noSubset) break;
        }
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      buf= reverseShift (buf, evaluation);
      appendTestMapDown (result, buf, info, source, dest);
      delete [] v;
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
  {
    buf= reverseShift (F, evaluation);
    appendMapDown (result, buf, info, source, dest);
  }

  delete [] v;
  return result;
}

CFList
factorRecombination (const CanonicalForm& F, const CFList& factors,
                     const CFList& M)
{
  if (factors.length() == 1)
    return CFList(F);
  if (factors.length() < 1)
    return CFList();

  CFList T, S;

  T= factors;

  int s= 1;
  CFList result;
  CanonicalForm LCBuf= LC (F, Variable (1));
  CanonicalForm g, buf= F;
  int * v= new int [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  TT= copy (factors);
  Variable y= F.level() - 1;
  bool recombination= false;
  CanonicalForm h;
  while (T.length() >= 2*s)
  {
    while (noSubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombination)
        {
          T.insert (LC (buf));
          g= prodMod (T, M);
          result.append (g/myContent (g));
          return result;
        }
        else
          return CFList (F);
      }
      S= subset (v, s, TT, noSubset);
      if (noSubset) break;
      S.insert (LCBuf);
      g= prodMod (S, M);
      S.removeFirst();
      g /= myContent (g);
      if (fdivides (g, buf))
      {
        recombination= true;
        result.append (g);
        buf /= g;
        LCBuf= LC (buf, Variable(1));
        T= Difference (T, S);
        if (T.length() < 2*s || T.length() == s)
        {
          result.append (buf);
          delete [] v;
          return result;
        }
        TT= copy (T);
        indexUpdate (v, s, T.length(), noSubset);
        if (noSubset) break;
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      result.append (buf);
      delete [] v;
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
    result.append (F);

  delete [] v;
  return result;
}

int
liftBoundAdaption (const CanonicalForm& F, const CFList& factors, bool&
                   success, const int deg, const CFList& MOD, const int bound)
{
  int adaptedLiftBound= 0;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g;
  CFList M= MOD;
  M.append (power (y, deg));
  int d= bound;
  int e= 0;
  int nBuf;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    g= mulMod (i.getItem(), LCBuf, M);
    g /= myContent (g);
    if (fdivides (g, buf))
    {
      nBuf= degree (g, y) + degree (LC (g, 1), y);
      d -= nBuf;
      e= tmax (e, nBuf);
      buf /= g;
      LCBuf= LC (buf, Variable (1));
    }
  }
  adaptedLiftBound= d;

  if (adaptedLiftBound < deg)
  {
    if (adaptedLiftBound < degree (F) + 1)
    {
      if (d == 1)
      {
        if (e + 1 > deg)
        {
          adaptedLiftBound= deg;
          success= false;
        }
        else
        {
          success= true;
          if (e + 1 < degree (F) + 1)
            adaptedLiftBound= deg;
          else
            adaptedLiftBound= e + 1;
        }
      }
      else
      {
        success= true;
        adaptedLiftBound= deg;
      }
    }
    else
    {
      success= true;
    }
  }
  return adaptedLiftBound;
}

int
extLiftBoundAdaption (const CanonicalForm& F, const CFList& factors, bool&
                      success, const ExtensionInfo& info, const CFList& eval,
                      const int deg, const CFList& MOD, const int bound)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  int adaptedLiftBound= 0;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g, gg;
  CFList M= MOD;
  M.append (power (y, deg));
  adaptedLiftBound= 0;
  int d= bound;
  int e= 0;
  int nBuf;
  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList source, dest;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    g= mulMod (i.getItem(), LCBuf, M);
    g /= myContent (g);
    if (fdivides (g, buf))
    {
      gg= reverseShift (g, eval);
      gg /= Lc (gg);
      if (!k && beta == Variable (1))
      {
        if (degree (gg, alpha) < degMipoBeta)
        {
          buf /= g;
          nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, Variable (1));
        }
      }
      else
      {
        if (!isInExtension (gg, gamma, k, delta, source, dest))
        {
          buf /= g;
          nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, Variable (1));
        }
      }
    }
  }
  adaptedLiftBound= d;

  if (adaptedLiftBound < deg)
  {
    if (adaptedLiftBound < degree (F) + 1)
    {
      if (d == 1)
      {
        if (e + 1 > deg)
        {
          adaptedLiftBound= deg;
          success= false;
        }
        else
        {
          success= true;
          if (e + 1 < degree (F) + 1)
            adaptedLiftBound= deg;
          else
            adaptedLiftBound= e + 1;
        }
      }
      else
      {
        success= true;
        adaptedLiftBound= deg;
      }
    }
    else
    {
      success= true;
    }
  }

  return adaptedLiftBound;
}

CFList
earlyFactorDetect (CanonicalForm& F, CFList& factors, int& adaptedLiftBound,
                   bool& success, const int deg, const CFList& MOD,
                   const int bound)
{
  CFList result;
  CFList T= factors;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g;
  CFList M= MOD;
  M.append (power (y, deg));
  adaptedLiftBound= 0;
  int d= bound;
  int e= 0;
  int nBuf;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    g= mulMod (i.getItem(), LCBuf, M);
    g /= myContent (g);
    if (fdivides (g, buf))
    {
      result.append (g);
      nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
      d -= nBuf;
      e= tmax (e, nBuf);
      buf /= g;
      LCBuf= LC (buf, Variable (1));
      T= Difference (T, CFList (i.getItem()));
    }
  }
  adaptedLiftBound= d;

  if (adaptedLiftBound < deg)
  {
    if (adaptedLiftBound < degree (F) + 1)
    {
      if (d == 1)
        adaptedLiftBound= tmin (e + 1, deg);
      else
        adaptedLiftBound= deg;
    }
    factors= T;
    F= buf;
    success= true;
  }
  return result;
}

CFList
extEarlyFactorDetect (CanonicalForm& F, CFList& factors, int& adaptedLiftBound,
                      bool& success, const ExtensionInfo& info, const CFList&
                      eval, const int deg, const CFList& MOD, const int bound)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  CFList result;
  CFList T= factors;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g, gg;
  CFList M= MOD;
  M.append (power (y, deg));
  adaptedLiftBound= 0;
  int d= bound;
  int e= 0;
  int nBuf;
  CFList source, dest;

  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    g= mulMod (i.getItem(), LCBuf, M);
    g /= myContent (g);
    if (fdivides (g, buf))
    {
      gg= reverseShift (g, eval);
      gg /= Lc (gg);
      if (!k && beta == Variable (1))
      {
        if (degree (gg, alpha) < degMipoBeta)
        {
          appendTestMapDown (result, gg, info, source, dest);
          buf /= g;
          nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, Variable (1));
          T= Difference (T, CFList (i.getItem()));
        }
      }
      else
      {
        if (!isInExtension (gg, gamma, k, delta, source, dest))
        {
          appendTestMapDown (result, gg, info, source, dest);
          buf /= g;
          nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, Variable (1));
          T= Difference (T, CFList (i.getItem()));
         }
      }
    }
  }
  adaptedLiftBound= d;

  if (adaptedLiftBound < deg)
  {
    if (adaptedLiftBound < degree (F) + 1)
    {
      if (d == 1)
        adaptedLiftBound= tmin (e + 1, deg);
      else
        adaptedLiftBound= deg;
    }
    success= true;
    factors= T;
    F= buf;
  }
  return result;
}

CFList
evalPoints (const CanonicalForm& F, CFList & eval, const Variable& alpha,
            CFList& list, const bool& GF, bool& fail)
{
  int k= F.level() - 1;
  Variable x= Variable (1);
  CFList result;
  FFRandom genFF;
  GFRandom genGF;
  int p= getCharacteristic ();
  double bound;
  if (alpha != Variable (1))
  {
    bound= pow ((double) p, (double) degree (getMipo(alpha)));
    bound= pow ((double) bound, (double) k);
  }
  else if (GF)
  {
    bound= pow ((double) p, (double) getGFDegree());
    bound= pow ((double) bound, (double) k);
  }
  else
    bound= pow ((double) p, (double) k);

  CanonicalForm random;
  CanonicalForm deriv_x, gcd_deriv;
  do
  {
    random= 0;
    // possible overflow if list.length() does not fit into a int
    if (list.length() >= bound)
    {
      fail= true;
      break;
    }
    for (int i= 0; i < k; i++)
    {
      if (list.isEmpty())
        result.append (0);
      else if (GF)
      {
        result.append (genGF.generate());
        random += result.getLast()*power (x, i);
      }
      else if (alpha.level() != 1)
      {
        AlgExtRandomF genAlgExt (alpha);
        result.append (genAlgExt.generate());
        random += result.getLast()*power (x, i);
      }
      else
      {
        result.append (genFF.generate());
        random += result.getLast()*power (x, i);
      }
    }
    if (find (list, random))
    {
      result= CFList();
      continue;
    }
    int l= F.level();
    eval.insert (F);
    bool bad= false;
    for (CFListIterator i= result; i.hasItem(); i++, l--)
    {
      eval.insert (eval.getFirst()(i.getItem(), l));
      if (degree (eval.getFirst(), l - 1) != degree (F, l - 1))
      {
        if (!find (list, random))
          list.append (random);
        result= CFList();
        eval= CFList();
        bad= true;
        break;
      }
    }

    if (bad)
      continue;

    if (degree (eval.getFirst()) != degree (F, 1))
    {
      if (!find (list, random))
        list.append (random);
      result= CFList();
      eval= CFList();
      continue;
    }

    deriv_x= deriv (eval.getFirst(), x);
    gcd_deriv= gcd (eval.getFirst(), deriv_x);
    if (degree (gcd_deriv) > 0)
    {
      if (!find (list, random))
        list.append (random);
      result= CFList();
      eval= CFList();
      continue;
    }
    CFListIterator i= eval;
    i++;
    CanonicalForm contentx= content (i.getItem(), x);
    if (degree (contentx) > 0)
    {
      if (!find (list, random))
        list.append (random);
      result= CFList();
      eval= CFList();
      continue;
    }

    if (list.length() >= bound)
    {
      fail= true;
      break;
    }
  } while (find (list, random));

  if (!eval.isEmpty())
    eval.removeFirst();

  return result;
}

static inline
int newMainVariableSearch (CanonicalForm& A, CFList& Aeval, CFList&
                           evaluation, const Variable& alpha, const int lev,
                           CanonicalForm& g
                          )
{
  Variable x= Variable (1);
  CanonicalForm derivI, buf;
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  int swapLevel= 0;
  CFList list;
  bool fail= false;
  buf= A;
  Aeval= CFList();
  evaluation= CFList();
  for (int i= lev; i <= A.level(); i++)
  {
    derivI= deriv (buf, Variable (i));
    if (!derivI.isZero())
    {
      g= gcd (buf, derivI);
      if (degree (g) > 0)
        return -1;

      buf= swapvar (buf, x, Variable (i));
      Aeval= CFList();
      evaluation= CFList();
      fail= false;
      evaluation= evalPoints (buf, Aeval, alpha, list, GF, fail);
      if (!fail)
      {
        A= buf;
        swapLevel= i;
        break;
      }
      else
        buf= A;
    }
  }
  return swapLevel;
}

CanonicalForm lcmContent (const CanonicalForm& A, CFList& contentAi)
{
  int i= A.level();
  contentAi.append (myContent (A, i));
  contentAi.append (myContent (A, i - 1));
  CanonicalForm result= lcm (contentAi.getFirst(), contentAi.getLast());
  for (i= i - 2; i > 0; i--)
  {
    contentAi.append (content (A, i));
    result= lcm (result, contentAi.getLast());
  }
  return result;
}

CFList
henselLiftAndEarly (CanonicalForm& A, CFList& MOD, int*& liftBounds, bool&
                    earlySuccess, CFList& earlyFactors, const CFList& Aeval,
                    const CFList& biFactors, const CFList& evaluation,
                    const ExtensionInfo& info)
{
  bool extension= info.isInExtension();
  CFList bufFactors= biFactors;
  bufFactors.insert (LC (Aeval.getFirst(), 1));

  sortList (bufFactors, Variable (1));

  CFList diophant;
  CFArray Pi;
  int smallFactorDeg= 11; //tunable parameter
  CFList result;
  int adaptedLiftBound= 0;
  int liftBound= liftBounds[1];

  earlySuccess= false;
  CFList earlyReconstFactors;
  CFListIterator j= Aeval;
  j++;
  CanonicalForm buf= j.getItem();
  CFMatrix Mat= CFMatrix (liftBound, bufFactors.length() - 1);
  MOD= CFList (power (Variable (2), liftBounds[0]));
  if (smallFactorDeg >= liftBound)
  {
    result= henselLift23 (Aeval, bufFactors, liftBounds, diophant, Pi, Mat);
  }
  else if (smallFactorDeg >= degree (buf) + 1)
  {
    liftBounds[1]= degree (buf) + 1;
    result= henselLift23 (Aeval, bufFactors, liftBounds, diophant, Pi, Mat);
    if (Aeval.length() == 2)
    {
      if (!extension)
        earlyFactors= earlyFactorDetect
                       (buf, result, adaptedLiftBound, earlySuccess,
                        degree (buf) + 1, MOD, liftBound);
      else
        earlyFactors= extEarlyFactorDetect
                       (buf, result, adaptedLiftBound, earlySuccess,
                        info, evaluation, degree
                        (buf) + 1, MOD, liftBound);
    }
    else
    {
      if (!extension)
        adaptedLiftBound= liftBoundAdaption (buf, result, earlySuccess,
                                             degree (buf) + 1, MOD, liftBound);
      else
        adaptedLiftBound= extLiftBoundAdaption (buf, result, earlySuccess, info,
                                                evaluation, degree (buf) + 1,
                                                MOD, liftBound);
    }
    if (!earlySuccess)
    {
      result.insert (LC (buf, 1));
      liftBounds[1]= adaptedLiftBound;
      liftBound= adaptedLiftBound;
      henselLiftResume (buf, result, degree (buf) + 1, liftBound,
                        Pi, diophant, Mat, MOD);
    }
    else
      liftBounds[1]= adaptedLiftBound;
  }
  else if (smallFactorDeg < degree (buf) + 1)
  {
    liftBounds[1]= smallFactorDeg;
    result= henselLift23 (Aeval, bufFactors, liftBounds, diophant, Pi, Mat);
    if (Aeval.length() == 2)
    {
      if (!extension)
        earlyFactors= earlyFactorDetect (buf, result, adaptedLiftBound,
                                         earlySuccess, smallFactorDeg, MOD,
                                         liftBound);
      else
        earlyFactors= extEarlyFactorDetect (buf, result, adaptedLiftBound,
                                            earlySuccess, info, evaluation,
                                            smallFactorDeg, MOD, liftBound);
    }
    else
    {
      if (!extension)
        adaptedLiftBound= liftBoundAdaption (buf, result, earlySuccess,
                                             smallFactorDeg, MOD, liftBound);
      else
        adaptedLiftBound= extLiftBoundAdaption (buf, result, earlySuccess, info,
                                                evaluation, smallFactorDeg, MOD,
                                                liftBound);
    }

    if (!earlySuccess)
    {
      result.insert (LC (buf, 1));
      henselLiftResume (buf, result, smallFactorDeg, degree (buf) + 1,
                        Pi, diophant, Mat, MOD);
      if (Aeval.length() == 2)
      {
         if (!extension)
           earlyFactors= earlyFactorDetect (buf, result, adaptedLiftBound,
                                            earlySuccess, degree (buf) + 1,
                                            MOD, liftBound);
         else
           earlyFactors= extEarlyFactorDetect (buf, result, adaptedLiftBound,
                                               earlySuccess, info, evaluation,
                                               degree (buf) + 1, MOD,
                                               liftBound);
      }
      else
      {
        if (!extension)
          adaptedLiftBound= liftBoundAdaption (buf, result, earlySuccess,
                                               degree (buf) + 1, MOD,liftBound);
        else
          adaptedLiftBound= extLiftBoundAdaption (buf, result, earlySuccess,
                                                  info, evaluation,
                                                  degree (buf) + 1, MOD,
                                                  liftBound);
      }
      if (!earlySuccess)
      {
        result.insert (LC (buf, 1));
        liftBounds[1]= adaptedLiftBound;
        liftBound= adaptedLiftBound;
        henselLiftResume (buf, result, degree (buf) + 1, liftBound,
                          Pi, diophant, Mat, MOD);
      }
      else
        liftBounds[1]= adaptedLiftBound;
    }
    else
      liftBounds[1]= adaptedLiftBound;
  }

  MOD.append (power (Variable (3), liftBounds[1]));

  if (Aeval.length() > 2)
  {
    CFListIterator j= Aeval;
    j++;
    CFList bufEval;
    bufEval.append (j.getItem());
    j++;
    int liftBoundsLength= Aeval.getLast().level() - 1;
    for (int i= 2; i <= liftBoundsLength && j.hasItem(); i++, j++)
    {
      earlySuccess= false;
      result.insert (LC (bufEval.getFirst(), 1));
      bufEval.append (j.getItem());
      liftBound= liftBounds[i];
      Mat= CFMatrix (liftBounds[i], result.length() - 1);

      buf= j.getItem();
      if (smallFactorDeg >= liftBound)
        result= henselLift (bufEval, result, MOD, diophant, Pi, Mat,
                            liftBounds[i -  1], liftBounds[i]);
      else if (smallFactorDeg >= degree (buf) + 1)
      {
        result= henselLift (bufEval, result, MOD, diophant, Pi, Mat,
                            liftBounds[i -  1], degree (buf) + 1);

        if (Aeval.length() == i + 1)
        {
          if (!extension)
            earlyFactors= earlyFactorDetect
                           (buf, result, adaptedLiftBound, earlySuccess,
                            degree (buf) + 1, MOD, liftBound);
          else
            earlyFactors= extEarlyFactorDetect
                           (buf, result, adaptedLiftBound, earlySuccess,
                            info, evaluation, degree (buf) + 1, MOD, liftBound);
        }
        else
        {
          if (!extension)
            adaptedLiftBound= liftBoundAdaption
                                (buf, result, earlySuccess, degree (buf)
                                 + 1,  MOD, liftBound);
          else
            adaptedLiftBound= extLiftBoundAdaption
                                (buf, result, earlySuccess, info, evaluation,
                                 degree (buf) + 1, MOD, liftBound);
        }

        if (!earlySuccess)
        {
          result.insert (LC (buf, 1));
          liftBounds[i]= adaptedLiftBound;
          liftBound= adaptedLiftBound;
          henselLiftResume (buf, result, degree (buf) + 1, liftBound,
                            Pi, diophant, Mat, MOD);
        }
        else
        {
          liftBounds[i]= adaptedLiftBound;
        }
      }
      else if (smallFactorDeg < degree (buf) + 1)
      {
        result= henselLift (bufEval, result, MOD, diophant, Pi, Mat,
                            liftBounds[i -  1], smallFactorDeg);

        if (Aeval.length() == i + 1)
        {
          if (!extension)
            earlyFactors= earlyFactorDetect
                           (buf, result, adaptedLiftBound, earlySuccess,
                            smallFactorDeg, MOD, liftBound);
          else
            earlyFactors= extEarlyFactorDetect
                           (buf, result, adaptedLiftBound, earlySuccess,
                            info, evaluation, smallFactorDeg, MOD, liftBound);
        }
        else
        {
          if (!extension)
            adaptedLiftBound= liftBoundAdaption
                                (buf, result, earlySuccess,
                                 smallFactorDeg, MOD, liftBound);
          else
            adaptedLiftBound= extLiftBoundAdaption
                                (buf, result, earlySuccess, info, evaluation,
                                 smallFactorDeg, MOD, liftBound);
        }

        if (!earlySuccess)
        {
          result.insert (LC (buf, 1));
          henselLiftResume (buf, result, smallFactorDeg,
                            degree (buf) + 1, Pi, diophant, Mat, MOD);
          if (Aeval.length() == i + 1)
          {
            if (!extension)
              earlyFactors= earlyFactorDetect
                             (buf, result, adaptedLiftBound, earlySuccess,
                              degree (buf) +  1,  MOD, liftBound);
            else
              earlyFactors= extEarlyFactorDetect
                             (buf, result, adaptedLiftBound, earlySuccess,
                              info, evaluation, degree (buf) + 1, MOD,
                              liftBound);
          }
          else
          {
            if (!extension)
              adaptedLiftBound= liftBoundAdaption
                                  (buf, result, earlySuccess, degree
                                   (buf) +  1,  MOD, liftBound);
            else
              adaptedLiftBound= extLiftBoundAdaption
                                  (buf, result, earlySuccess, info, evaluation,
                                   degree (buf) + 1,  MOD, liftBound);
          }

          if (!earlySuccess)
          {
            result.insert (LC (buf, 1));
            liftBounds[i]= adaptedLiftBound;
            liftBound= adaptedLiftBound;
            henselLiftResume (buf, result, degree (buf) + 1, liftBound,
                              Pi, diophant, Mat, MOD);
          }
          else
            liftBounds[i]= adaptedLiftBound;
        }
        else
          liftBounds[i]= adaptedLiftBound;
      }
      MOD.append (power (Variable (i + 2), liftBounds[i]));
      bufEval.removeFirst();
    }
    bufFactors= result;
  }
  else
    bufFactors= result;

  if (earlySuccess)
    A= buf;
  return result;
}

CFList
leadingCoeffReconstruction (const CanonicalForm& F, const CFList& factors,
                            const CFList& M)
{
  CanonicalForm buf= F;
  CanonicalForm LCBuf= LC (buf, 1);

  CFList result;

  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    CanonicalForm tmp= i.getItem();
    tmp= mulMod (tmp, LCBuf, M);
    tmp= tmp/content (tmp, 1);
    if (fdivides (tmp, buf))
    {
      buf /= tmp;
      result.append (tmp);
      LCBuf= LC (buf, 1);
    }
    else //no one-to-one correspondence
      return CFList();
  }

  return result;
}

void
gcdFreeBasis (CFFList& factors1, CFFList& factors2)
{
  CanonicalForm g;
  int k= factors1.length();
  int l= factors2.length();
  int n= 1;
  int m;
  for (CFFListIterator i= factors1; (i.hasItem() && n < k); i++, n++)
  {
    m= 1;
    for (CFFListIterator j= factors2; (j.hasItem() && m < l); j++, m++)
    {
      CanonicalForm g= gcd (i.getItem().factor(), j.getItem().factor());
      if (degree (g) > 0)
      {
        j.getItem()= CFFactor (j.getItem().factor()/g, j.getItem().exp());
        i.getItem()= CFFactor (i.getItem().factor()/g, i.getItem().exp());
        factors1.append (CFFactor (g, i.getItem().exp()));
        factors2.append (CFFactor (g, j.getItem().exp()));
      }
    }
  }
}

CFList
distributeContent (const CFList& L, const CFList* differentSecondVarFactors,
                   int length
                  )
{
  CFList l= L;
  CanonicalForm content= l.getFirst();

  if (content.inCoeffDomain())
    return l;

  if (l.length() == 1)
  {
    CFList result;
    for (int i= 0; i < length; i++)
    {
      if (differentSecondVarFactors[i].isEmpty())
        continue;
      if (result.isEmpty())
      {
        result= differentSecondVarFactors[i];
        for (CFListIterator iter= result; iter.hasItem(); iter++)
          content /= iter.getItem();
      }
      else
      {
        CFListIterator iter1= result;
        for (CFListIterator iter2= differentSecondVarFactors[i]; iter2.hasItem();
             iter2++, iter1++)
        {
          iter1.getItem() *= iter2.getItem();
          content /= iter2.getItem();
        }
      }
    }
    result.insert (content);
    return result;
  }

  for (int i= 0; i < length; i++)
  {
    if (differentSecondVarFactors[i].isEmpty())
      continue;
    CFListIterator iter1= l;
    iter1++;

    Variable v= Variable (i + 3);
    for (CFListIterator iter2= differentSecondVarFactors[i]; iter2.hasItem();
         iter2++, iter1++)
    {
      if (degree (iter2.getItem(),v) == degree (iter1.getItem(),v))
        continue;
      CanonicalForm tmp= iter1.getItem();
      for (int j= tmp.level(); j > 1; j--)
      {
        if (j == i + 3)
          continue;
        tmp= tmp (0, j);
      }
      CanonicalForm g= gcd (iter2.getItem(), content);
      if (degree (g) > 0)
      {
        iter2.getItem() /= tmp;
        content /= g;
        iter1.getItem() *= g;
      }
    }
  }

  l.removeFirst();
  l.insert (content);
  return l;
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

int
testFactors (const CanonicalForm& G, const CFList& uniFactors,
             const Variable& alpha, CanonicalForm& sqrfPartF, CFList& factors,
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
  CFFList sqrfFactorization= squarefreeFactorization (F, alpha);

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
    sqrfFactors= squarefreeFactorization (i.getItem(), alpha);

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
                        const Variable& alpha, const CFList& evaluation,
                        CFList* & differentSecondVarLCs, int length,
                        Variable& y
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
    i.getItem()= i.getItem() (Variable (1) + evaluation.getLast(), Variable (1));
  }

  CanonicalForm sqrfPartF;
  CFFList * bufSqrfFactors= new CFFList [factors.length()];
  CFList evalSqrfPartF;
  CanonicalForm bufContent;
  CFList bufFactors;
  int pass= testFactors (F, factors, alpha, sqrfPartF,
                         bufFactors, bufSqrfFactors, evalSqrfPartF);

  bool foundDifferent= false;
  Variable z;
  Variable x= y;
  int j= 0;
  if (!pass)
  {
    int lev;
    // LCF is non-constant here
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
        pass= testFactors (bufF, bufBufFactors, alpha, sqrfPartF, bufFactors,
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
          for (int k= 1; k <= factors.length(); k++)
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
    for (int k= 1; k <= factors.length(); k++)
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
  henselLift12 (evalSqrfPartF.getFirst(), factors, liftBound, Pi, diophant, M, false);

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

void
evaluationWRTDifferentSecondVars (CFList*& Aeval, const CFList& evaluation,
                                  const CanonicalForm& A)
{
  for (int i= A.level(); i > 2; i--)
  {
    CanonicalForm tmp= A;
    CFList tmp2= CFList();
    CFListIterator iter= evaluation;
    bool preserveDegree= true;
    for (int j= A.level(); j > 1; j--, iter++)
    {
      if (j == i)
        continue;
      else
      {
        tmp= tmp (iter.getItem(), j);
        tmp2.insert (tmp);
        if ((degree (tmp, i) != degree (A, i)) ||
            (degree (tmp, 1) != degree (A, 1)))
        {
          preserveDegree= false;
          break;
        }
        if (!content(tmp).inCoeffDomain() || !content(tmp,1).inCoeffDomain())
        {
          preserveDegree= false;
          break;
        }
      }
    }
    if (preserveDegree)
      Aeval [i - 3]= tmp2;
    else
      Aeval [i - 3]= CFList();
  }
}

static inline
CanonicalForm prodEval (const CFList& l, const CanonicalForm& evalPoint,
                        const Variable& v)
{
  CanonicalForm result= 1;
  for (CFListIterator i= l; i.hasItem(); i++)
    result *= i.getItem() (evalPoint, v);
  return result;
}

//recombine bivariate factors in case one bivariate factorization yields less
// factors than the other
CFList
recombination (const CFList& factors1, const CFList& factors2, int s, int thres,
               const CanonicalForm& evalPoint, const Variable& x)
{
  CFList T, S;

  T= factors1;
  CFList result;
  CanonicalForm buf;
  int * v= new int [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool nosubset= false;
  CFArray TT;
  TT= copy (factors1);
  while (T.length() >= 2*s && s <= thres)
  {
    while (nosubset == false) 
    {
      if (T.length() == s) 
      {
        delete [] v;
        result.append (prod (T));
        return result;
      }
      S= subset (v, s, TT, nosubset);
      if (nosubset) break;
      buf= prodEval (S, evalPoint, x);
      buf /= Lc (buf);
      if (find (factors2, buf))
      {
        T= Difference (T, S);
        result.append (prod (S));
        TT= copy (T);
        indexUpdate (v, s, T.length(), nosubset);
        if (nosubset) break;
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s) 
    {
      delete [] v;
      result.append (prod (T));
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }

  delete [] v;
  if (T.length() < 2*s)
  {
    result.append (prod (T));
    return result;
  }

  return result;
}

void
factorizationWRTDifferentSecondVars (const CanonicalForm& A, CFList*& Aeval,
                                     const ExtensionInfo& info,
                                     int& minFactorsLength, bool& irred)
{
  Variable x= Variable (1);
  minFactorsLength= 0;
  irred= false;
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      Variable v= Variable (Aeval[j].getFirst().level());

      CFList factors;
      if (CFFactory::gettype() == GaloisFieldDomain)
        factors= GFBiSqrfFactorize (Aeval[j].getFirst());
      else if (info.getAlpha().level() == 1)
        factors= FpBiSqrfFactorize (Aeval[j].getFirst());
      else
        factors= FqBiSqrfFactorize (Aeval[j].getFirst(), info.getAlpha());

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

void getLeadingCoeffs (const CanonicalForm& A, CFList*& Aeval,
                       const CFList& uniFactors, const CFList& evaluation
                      )
{
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      int i= A.level();
      CanonicalForm evalPoint;
      for (CFListIterator iter= evaluation; iter.hasItem(); iter++, i--)
      {
        if (i == Aeval[j].getFirst().level())
        {
          evalPoint= iter.getItem();
          break;
        }
      }

      Variable v= Variable (i);
      if (Aeval[j].length() > uniFactors.length())
        Aeval[j]= recombination (Aeval[j], uniFactors, 1,
                                 Aeval[j].length() - uniFactors.length() + 1,
                                 evalPoint, v);

      CFList l;
      CanonicalForm buf;
      for (CFListIterator iter1= uniFactors; iter1.hasItem(); iter1++)
      {
        for (CFListIterator iter2= Aeval[j]; iter2.hasItem(); iter2++)
        {
          buf= mod (iter2.getItem(), v - evalPoint);
          buf /= Lc (buf);
          if (iter1.getItem() == buf)
          {
            l.append (iter2.getItem());
            break;
          }
        }
      }
      Aeval [j]= l;

      CFList LCs;
      for (CFListIterator iter= Aeval[j]; iter.hasItem(); iter++)
        LCs.append (LC (iter.getItem() (v + evalPoint, v), 1));
      normalize (LCs);
      Aeval[j]= LCs;
    }
  }
}

CFList
buildUniFactors (const CFList& biFactors, const CanonicalForm& evalPoint,
                 const Variable& y)
{
  CFList result;
  CanonicalForm tmp;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
  {
    tmp= mod (i.getItem(), y - evalPoint);
    tmp /= Lc (tmp);
    result.append (tmp);
  }
  return result;
}

void refineBiFactors (const CanonicalForm& A, CFList& biFactors,
                      CFList* const& Aeval, const CFList& evaluation,
                      int minFactorsLength)
{
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (Aeval[j].length() == minFactorsLength)
    {
      int i= A.level();
      CanonicalForm evalPoint;
      for (CFListIterator iter= evaluation; iter.hasItem(); iter++, i--)
      {
        if (i == Aeval[j].getFirst().level())
        {
          evalPoint= iter.getItem();
          break;
        }
      }

      Variable v= Variable (i);
      CFList list= buildUniFactors (Aeval[j], evalPoint, v);

      Variable y= Variable (2);
      biFactors= recombination (biFactors, list, 1,
                                biFactors.length() - list.length() + 1,
                                evaluation.getLast(), y);
      return;
    }
  }
}

void prepareLeadingCoeffs (CFList*& LCs, int n, const CFList& leadingCoeffs,
                           const CFList& biFactors)
{
  CFList l= leadingCoeffs;
  LCs [n-3]= l;
  for (int i= n - 1; i > 2; i--)
  {
    for (CFListIterator j= l; j.hasItem(); j++)
      j.getItem()= j.getItem() (0, i + 1);
    LCs [i - 3]= l;
  }
  l= LCs [0];
  for (CFListIterator i= l; i.hasItem(); i++)
    i.getItem()= i.getItem() (0, 3);
  CFListIterator ii= biFactors;
  CFList normalizeFactor;
  for (CFListIterator i= l; i.hasItem(); i++, ii++)
    normalizeFactor.append (Lc (LC (ii.getItem(), 1))/Lc (i.getItem()));
  for (int i= 0; i < n-2; i++)
  {
    ii= normalizeFactor;
    for (CFListIterator j= LCs [i]; j.hasItem(); j++, ii++)
      j.getItem() *= ii.getItem();
  }
}

CFList recoverFactors (const CanonicalForm& F, const CFList& factors)
{
  CFList result;
  CanonicalForm tmp;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    tmp= i.getItem() / content (i.getItem(), 1);
    if (fdivides (tmp, F))
      result.append (tmp);
  }
  return result;
}

CFList
extNonMonicFactorRecombination (const CFList& factors, const CanonicalForm& F,
                                const ExtensionInfo& info,
                                const CFList& evaluation)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  CFList source, dest;

  int degMipoBeta= 1;
  if (!k && beta != Variable(1))
    degMipoBeta= degree (getMipo (beta));

  CFList T, S;
  T= factors;
  int s= 1;
  CFList result;
  CanonicalForm buf= F;

  CanonicalForm g;
  CanonicalForm buf2;
  int * v= new int [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  TT= copy (factors);
  bool recombination= false;
  bool trueFactor= false;
  while (T.length() >= 2*s)
  {
    while (noSubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombination)
        {
          g= prod (T);
          T.removeFirst();
          result.append (g/myContent (g));
          g= reverseShift (g, evaluation);
          g /= Lc (g);
          appendTestMapDown (result, g, info, source, dest);
          return result;
        }
        else
        {
          buf= reverseShift (buf, evaluation);
          return CFList (buf);
        }
      }

      S= subset (v, s, TT, noSubset);
      if (noSubset) break;

      g= prod (S);
      g /= myContent (g);
      if (fdivides (g, buf))
      {
        buf2= reverseShift (g, evaluation);
        buf2 /= Lc (buf2);
        if (!k && beta == Variable (1))
        {
          if (degree (buf2, alpha) < degMipoBeta)
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf /= g;
            recombination= true;
            trueFactor= true;
          }
        }
        else
        {
          if (!isInExtension (buf2, gamma, k, delta, source, dest))
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf /= g;
            recombination= true;
            trueFactor= true;
          }
        }
        if (trueFactor)
        {
          T= Difference (T, S);

          if (T.length() < 2*s || T.length() == s)
          {
            delete [] v;
            buf= reverseShift (buf, evaluation);
            buf /= Lc (buf);
            appendTestMapDown (result, buf, info, source, dest);
            return result;
          }
          trueFactor= false;
          TT= copy (T);
          indexUpdate (v, s, T.length(), noSubset);
          if (noSubset) break;
        }
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      delete [] v;
      buf= reverseShift (buf, evaluation);
      appendTestMapDown (result, buf, info, source, dest);
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
  {
    buf= reverseShift (F, evaluation);
    appendMapDown (result, buf, info, source, dest);
  }

  delete [] v;
  return result;
}

CFList
extFactorize (const CanonicalForm& F, const ExtensionInfo& info);

CFList
multiFactorize (const CanonicalForm& F, const ExtensionInfo& info)
{

  if (F.inCoeffDomain())
    return CFList (F);

  // compress and find main Variable
  CFMap N;
  CanonicalForm A= myCompress (F, N);

  A /= Lc (A); // make monic

  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  bool extension= info.isInExtension();
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  //univariate case
  if (F.isUnivariate())
  {
    if (extension == false)
      return uniFactorizer (F, alpha, GF);
    else
    {
      CFList source, dest;
      A= mapDown (F, info, source, dest);
      return uniFactorizer (A, beta, GF);
    }
  }

  //bivariate case
  if (A.level() == 2)
  {
    CFList buf= biFactorize (F, info);
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
        Union (multiFactorize (lcmCont, info),
               multiFactorize (i.getItem(), info));
        break;
      }
    }
    decompress (contentAFactors, N);
    normalize (contentAFactors);
    return contentAFactors;
  }

  // factorize content
  contentAFactors= multiFactorize (lcmCont, info);

  // univariate after content removal
  CFList factors;
  if (A.isUnivariate ())
  {
    factors= uniFactorizer (A, alpha, GF);
    append (factors, contentAFactors);
    decompress (factors, N);
    return factors;
  }

  // check main variable
  int swapLevel= 0;
  CanonicalForm derivZ;
  CanonicalForm gcdDerivZ;
  CanonicalForm bufA= A;
  Variable z;
  for (int i= 1; i <= A.level(); i++)
  {
    z= Variable (i);
    derivZ= deriv (bufA, z);
    if (derivZ.isZero())
    {
      if (i == 1)
        swapLevel= 1;
      else
        continue;
    }
    else
    {
      if (swapLevel == 1)
      {
        swapLevel= i;
        bufA= swapvar (A, x, z);
      }
      gcdDerivZ= gcd (bufA, derivZ);
      if (degree (gcdDerivZ) > 0 && !derivZ.isZero())
      {
        CanonicalForm g= bufA/gcdDerivZ;
        CFList factorsG=
        Union (multiFactorize (g, info),
               multiFactorize (gcdDerivZ, info));
        appendSwapDecompress (factorsG, contentAFactors, N, swapLevel, x);
        normalize (factorsG);
        return factorsG;
      }
      else
      {
        A= bufA;
        break;
      }
    }
  }


  CFList Aeval, list, evaluation, bufEvaluation, bufAeval;
  bool fail= false;
  int swapLevel2= 0;
  int level;
  int factorNums= 3;
  CanonicalForm bivarEval;
  CFList biFactors, bufBiFactors;
  CanonicalForm evalPoly;
  int lift, bufLift;
  double logarithm= (double) ilog2 (totaldegree (A));
  logarithm /= log2exp;
  logarithm= ceil (logarithm);
  if (factorNums < (int) logarithm)
    factorNums= (int) logarithm;
  CFList* bufAeval2= new CFList [A.level() - 2];
  CFList* Aeval2= new CFList [A.level() - 2];
  int counter;
  int differentSecondVar= 0;
  // several bivariate factorizations
  for (int i= 0; i < factorNums; i++)
  {
    counter= 0;
    bufA= A;
    bufAeval= CFList();
    bufEvaluation= evalPoints (bufA, bufAeval, alpha, list, GF, fail);
    evalPoly= 0;

    if (fail && (i == 0))
    {
      if (!swapLevel)
        level= 2;
      else
        level= swapLevel + 1;

      CanonicalForm g;
      swapLevel2= newMainVariableSearch (A, Aeval, evaluation, alpha, level, g);

      if (!swapLevel2) // need to pass to an extension
      {
        factors= extFactorize (A, info);
        appendSwapDecompress (factors, contentAFactors, N, swapLevel, x);
        normalize (factors);
        delete [] bufAeval2;
        delete [] Aeval2;
        return factors;
      }
      else
      {
        if (swapLevel2 == -1)
        {
          CFList factorsG=
          Union (multiFactorize (g, info),
                 multiFactorize (A/g, info));
          appendSwapDecompress (factorsG, contentAFactors, N, swapLevel, x);
          normalize (factorsG);
          delete [] bufAeval2;
          delete [] Aeval2;
          return factorsG;
        }
        fail= false;
        bufAeval= Aeval;
        bufA= A;
        bufEvaluation= evaluation;
      }
    }
    else if (fail && (i > 0))
      break;

    bivarEval= bufEvaluation.getLast();

    evaluationWRTDifferentSecondVars (bufAeval2, bufEvaluation, A);

    for (int j= 0; j < A.level() - 1; j++)
    {
      if (!bufAeval2[j].isEmpty())
        counter++;
    }

    bufLift= degree (A, y) + 1 + degree (LC(A, x), y);

    TIMING_START (fac_bi_factorizer);
    if (!GF && alpha.level() == 1)
      bufBiFactors= FpBiSqrfFactorize (bufAeval.getFirst());
    else if (GF)
      bufBiFactors= GFBiSqrfFactorize (bufAeval.getFirst());
    else
      bufBiFactors= FqBiSqrfFactorize (bufAeval.getFirst(), alpha);
    TIMING_END_AND_PRINT (fac_bi_factorizer,
                          "time for bivariate factorization: ");
    bufBiFactors.removeFirst();

    if (bufBiFactors.length() == 1)
    {
      if (extension)
      {
        CFList source, dest;
        A= mapDown (A, info, source, dest);
      }
      factors.append (A);
      appendSwapDecompress (factors, contentAFactors, N, swapLevel,
                            swapLevel2, x);
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
  factorizationWRTDifferentSecondVars (A, Aeval2, info, minFactorsLength, irred);

  if (irred)
  {
    if (extension)
    {
      CFList source, dest;
      A= mapDown (A, info, source, dest);
    }
    factors.append (A);
    appendSwapDecompress (factors, contentAFactors, N, swapLevel,
                          swapLevel2, x);
    normalize (factors);
    delete [] Aeval2;
    return factors;
  }

  if (minFactorsLength == 0)
    minFactorsLength= biFactors.length();
  else if (biFactors.length() > minFactorsLength)
    refineBiFactors (A, biFactors, Aeval2, evaluation, minFactorsLength);

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  CFList * oldAeval= new CFList [A.level() - 2]; //TODO use bufAeval2 for this
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

  Variable v;
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs, alpha,
                                          evaluation, Aeval2, A.level() - 2, v);

  if (v.level() != 1)
  {
    A= swapvar (A, y, v);
    for (int i= 0; i < A.level() - 2; i++)
    {
      if (oldAeval[i].isEmpty())
        continue;
      if (oldAeval[i].getFirst().level() == v.level())
      {
        biFactors= CFList();
        for (CFListIterator iter= oldAeval [i]; iter.hasItem(); iter++)
          biFactors.append (swapvar (iter.getItem(), v, y));
      }
    }
    int i= A.level();
    CanonicalForm evalPoint;
    for (CFListIterator iter= evaluation; iter.hasItem(); iter++, i--)
    {
      if (i == v.level())
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

    if (extension && !noOneToOne)
      factors= extNonMonicFactorRecombination (factors, oldA, info, evaluation);
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
      lift= tmax (lift, degree (i.getItem(), 2) + degree (LC (i.getItem(), 1)) + 1);

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
    TIMING_START (fac_hensel_lift);
    CFList liftedFactors= henselLiftAndEarly
                          (A, MOD, liftBounds, earlySuccess, earlyFactors,
                           Aeval, biFactors, evaluation, info);
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");

    if (!extension)
    {
      TIMING_START (fac_factor_recombination);
      factors= factorRecombination (A, liftedFactors, MOD);
      TIMING_END_AND_PRINT (fac_factor_recombination,
                            "time for factor recombination: ");
    }
    else
    {
      TIMING_START (fac_factor_recombination);
      factors= extFactorRecombination (liftedFactors, A, MOD, info, evaluation);
      TIMING_END_AND_PRINT (fac_factor_recombination,
                            "time for factor recombination: ");
    }

    if (earlySuccess)
      factors= Union (factors, earlyFactors);
  }

  if (!extension)
  {
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

  if (v.level() != 1)
  {
    for (CFListIterator iter= factors; iter.hasItem(); iter++)
      iter.getItem()= swapvar (iter.getItem(), v, y);
  }

  swap (factors, swapLevel, swapLevel2, x);
  append (factors, contentAFactors);
  decompress (factors, N);
  normalize (factors);

  delete[] liftBounds;

  return factors;
}

/// multivariate factorization over an extension of the initial field
CFList
extFactorize (const CanonicalForm& F, const ExtensionInfo& info)
{
  CanonicalForm A= F;

  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  int k= info.getGFDegree();
  char cGFName= info.getGFName();
  CanonicalForm delta= info.getDelta();
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  Variable w= Variable (1);

  CFList factors;
  if (!GF && alpha == w)  // we are in F_p
  {
    CFList factors;
    bool extension= true;
    int p= getCharacteristic();
    if (p*p < (1<<16)) // pass to GF if possible
    {
      setCharacteristic (getCharacteristic(), 2, 'Z');
      ExtensionInfo info= ExtensionInfo (extension);
      A= A.mapinto();
      factors= multiFactorize (A, info);

      Variable vBuf= rootOf (gf_mipo);
      setCharacteristic (getCharacteristic());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
    }
    else  // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (2, Variable (1));
      Variable v= rootOf (mipo);
      ExtensionInfo info= ExtensionInfo (v);
      factors= multiFactorize (A, info);
    }
    return factors;
  }
  else if (!GF && (alpha != w)) // we are in F_p(\alpha)
  {
    if (k == 1) // need factorization over F_p
    {
      int extDeg= degree (getMipo (alpha));
      extDeg++;
      CanonicalForm mipo= randomIrredpoly (extDeg + 1, Variable (1));
      Variable v= rootOf (mipo);
      ExtensionInfo info= ExtensionInfo (v);
      factors= biFactorize (A, info);
    }
    else
    {
      if (beta == Variable (1))
      {
        Variable v= chooseExtension (alpha, beta, k);
        CanonicalForm primElem, imPrimElem;
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (alpha, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (primElem, vBuf, v);

        CFList source, dest;
        CanonicalForm bufA= mapUp (A, alpha, v, primElem, imPrimElem,
                                   source, dest);
        ExtensionInfo info= ExtensionInfo (v, alpha, imPrimElem, primElem);
        factors= biFactorize (bufA, info);
      }
      else
      {
        Variable v= chooseExtension (alpha, beta, k);
        CanonicalForm primElem, imPrimElem;
        bool primFail= false;
        Variable vBuf;
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (delta, beta, v);

        CFList source, dest;
        CanonicalForm bufA= mapDown (A, info, source, dest);
        source= CFList();
        dest= CFList();
        bufA= mapUp (bufA, beta, v, delta, imPrimElem, source, dest);
        ExtensionInfo info= ExtensionInfo (v, beta, imPrimElem, delta);
        factors= biFactorize (bufA, info);
      }
    }
    return factors;
  }
  else // we are in GF (p^k)
  {
    int p= getCharacteristic();
    int extensionDeg= getGFDegree();
    bool extension= true;
    if (k == 1) // need factorization over F_p
    {
      extensionDeg++;
      if (pow ((double) p, (double) extensionDeg) < (1<<16))
      // pass to GF(p^k+1)
      {
        setCharacteristic (p);
        Variable vBuf= rootOf (gf_mipo);
        A= GF2FalphaRep (A, vBuf);
        setCharacteristic (p, extensionDeg, 'Z');
        ExtensionInfo info= ExtensionInfo (extension);
        factors= multiFactorize (A.mapinto(), info);
      }
      else // not able to pass to another GF, pass to F_p(\alpha)
      {
        setCharacteristic (p);
        Variable vBuf= rootOf (gf_mipo);
        A= GF2FalphaRep (A, vBuf);
        Variable v= chooseExtension (vBuf, beta, k);
        ExtensionInfo info= ExtensionInfo (v, extension);
        factors= multiFactorize (A, info);
      }
    }
    else // need factorization over GF (p^k)
    {
      if (pow ((double) p, (double) 2*extensionDeg) < (1<<16))
      // pass to GF(p^2k)
      {
        setCharacteristic (p, 2*extensionDeg, 'Z');
        ExtensionInfo info= ExtensionInfo (k, cGFName, extension);
        factors= multiFactorize (GFMapUp (A, extensionDeg), info);
        setCharacteristic (p, extensionDeg, cGFName);
      }
      else // not able to pass to GF (p^2k), pass to F_p (\alpha)
      {
        setCharacteristic (p);
        Variable v1= rootOf (gf_mipo);
        A= GF2FalphaRep (A, v1);
        Variable v2= chooseExtension (v1, v1, k);
        CanonicalForm primElem, imPrimElem;
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v1, v1, primFail);
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (primElem, v1, v2);
        CFList source, dest;
        CanonicalForm bufA= mapUp (A, v1, v2, primElem, imPrimElem,
                                     source, dest);
        ExtensionInfo info= ExtensionInfo (v2, v1, imPrimElem, primElem);
        factors= multiFactorize (bufA, info);
        setCharacteristic (p, k, cGFName);
        for (CFListIterator i= factors; i.hasItem(); i++)
          i.getItem()= Falpha2GFRep (i.getItem());
      }
    }
    return factors;
  }
}

#endif
/* HAVE_NTL */

