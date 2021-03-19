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
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "facFqFactorizeUtil.h"
#include "facFqFactorize.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_irred.h"
#include "cf_map_ext.h"
#include "facSparseHensel.h"
#include "facMul.h"
#include "cfUnivarGcd.h"

TIMING_DEFINE_PRINT(fac_fq_bi_factorizer)
TIMING_DEFINE_PRINT(fac_fq_hensel_lift)
TIMING_DEFINE_PRINT(fac_fq_factor_recombination)
TIMING_DEFINE_PRINT(fac_fq_shift_to_zero)
TIMING_DEFINE_PRINT(fac_fq_precompute_leadcoeff)
TIMING_DEFINE_PRINT(fac_fq_evaluation)
TIMING_DEFINE_PRINT(fac_fq_recover_factors)
TIMING_DEFINE_PRINT(fac_fq_preprocess_and_content)
TIMING_DEFINE_PRINT(fac_fq_bifactor_total)
TIMING_DEFINE_PRINT(fac_fq_luckswang)
TIMING_DEFINE_PRINT(fac_fq_lcheuristic)
TIMING_DEFINE_PRINT(fac_fq_content)
TIMING_DEFINE_PRINT(fac_fq_check_mainvar)
TIMING_DEFINE_PRINT(fac_fq_compress)


static inline
CanonicalForm
listGCD (const CFList& L);

static inline
CanonicalForm
myContent (const CanonicalForm& F)
{
  Variable x= Variable (1);
  CanonicalForm G= swapvar (F, F.mvar(), x);
  CFList L;
  for (CFIterator i= G; i.hasTerms(); i++)
    L.append (i.coeff());
  if (L.length() == 2)
    return swapvar (gcd (L.getFirst(), L.getLast()), F.mvar(), x);
  if (L.length() == 1)
    return LC (F, x);
  return swapvar (listGCD (L), F.mvar(), x);
}

static inline
CanonicalForm
listGCD (const CFList& L)
{
  if (L.length() == 0)
    return 0;
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
  int * degsf= NEW_ARRAY(int,n + 1);
  int ** swap= new int* [n + 1];
  for (int i= 0; i <= n; i++)
  {
    degsf[i]= 0;
    swap [i]= new int [3];
    swap [i] [0]= 0;
    swap [i] [1]= 0;
    swap [i] [2]= 0;
  }
  int i= 1;
  n= 1;
  degsf= degrees (F, degsf);

  CanonicalForm result= F;
  while ( i <= F.level() )
  {
    while( degsf[i] == 0 ) i++;
    swap[n][0]= i;
    swap[n][1]= size (LC (F,i));
    swap[n][2]= degsf [i];
    if (i != n)
      result= swapvar (result, Variable (n), Variable(i));
    n++; i++;
  }

  int buf1, buf2, buf3;
  n--;

  for (i= 1; i < n; i++)
  {
    for (int j= 1; j < n - i + 1; j++)
    {
      if (swap[j][1] > swap[j + 1][1])
      {
        buf1= swap [j + 1] [0];
        buf2= swap [j + 1] [1];
        buf3= swap [j + 1] [2];
        swap[j + 1] [0]= swap[j] [0];
        swap[j + 1] [1]= swap[j] [1];
        swap[j + 1] [2]= swap[j] [2];
        swap[j][0]= buf1;
        swap[j][1]= buf2;
        swap[j][2]= buf3;
        result= swapvar (result, Variable (j + 1), Variable (j));
      }
      else if (swap[j][1] == swap[j + 1][1] && swap[j][2] < swap[j + 1][2])
      {
        buf1= swap [j + 1] [0];
        buf2= swap [j + 1] [1];
        buf3= swap [j + 1] [2];
        swap[j + 1] [0]= swap[j] [0];
        swap[j + 1] [1]= swap[j] [1];
        swap[j + 1] [2]= swap[j] [2];
        swap[j][0]= buf1;
        swap[j][1]= buf2;
        swap[j][2]= buf3;
        result= swapvar (result, Variable (j + 1), Variable (j));
      }
    }
  }

  for (i= n; i > 0; i--)
  {
    if (i != swap[i] [0])
      N.newpair (Variable (i), Variable (swap[i] [0]));
  }

  for (i= F.level(); i >=0 ; i--)
    delete [] swap[i];
  delete [] swap;

  DELETE_ARRAY(degsf);

  return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
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

  Variable x= Variable (1);
  CanonicalForm g, LCBuf= LC (buf, x);
  CanonicalForm buf2, quot;
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
      if (fdivides (g, buf, quot))
      {
        buf2= reverseShift (g, evaluation);
        buf2 /= Lc (buf2);
        if (!k && beta == x)
        {
          if (degree (buf2, alpha) < degMipoBeta)
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf= quot;
            LCBuf= LC (buf, x);
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
            LCBuf= LC (buf, x);
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
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
  CanonicalForm h, quot;
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
      if (fdivides (g, buf, quot))
      {
        recombination= true;
        result.append (g);
        buf= quot;
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
int
liftBoundAdaption (const CanonicalForm& F, const CFList& factors, bool&
                   success, const int deg, const CFList& MOD, const int bound)
{
  int adaptedLiftBound= 0;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (buf, x);
  CanonicalForm g, quot;
  CFList M= MOD;
  M.append (power (y, deg));
  int d= bound;
  int e= 0;
  int nBuf;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    g= mulMod (i.getItem(), LCBuf, M);
    g /= myContent (g);
    if (fdivides (g, buf, quot))
    {
      nBuf= degree (g, y) + degree (LC (g, 1), y);
      d -= nBuf;
      e= tmax (e, nBuf);
      buf= quot;
      LCBuf= LC (buf, x);
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
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
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (buf, x);
  CanonicalForm g, gg, quot;
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
    if (fdivides (g, buf, quot))
    {
      gg= reverseShift (g, eval);
      gg /= Lc (gg);
      if (!k && beta == x)
      {
        if (degree (gg, alpha) < degMipoBeta)
        {
          buf= quot;
          nBuf= degree (g, y) + degree (LC (g, x), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, x);
        }
      }
      else
      {
        if (!isInExtension (gg, gamma, k, delta, source, dest))
        {
          buf= quot;
          nBuf= degree (g, y) + degree (LC (g, x), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, x);
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
CFList
earlyFactorDetect (CanonicalForm& F, CFList& factors, int& adaptedLiftBound,
                   bool& success, const int deg, const CFList& MOD,
                   const int bound)
{
  CFList result;
  CFList T= factors;
  CanonicalForm buf= F;
  Variable y= F.mvar();
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (buf, x);
  CanonicalForm g, quot;
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
    if (fdivides (g, buf, quot))
    {
      result.append (g);
      nBuf= degree (g, y) + degree (LC (g, x), y);
      d -= nBuf;
      e= tmax (e, nBuf);
      buf= quot;
      LCBuf= LC (buf, x);
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
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
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (buf, x);
  CanonicalForm g, gg, quot;
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
    if (fdivides (g, buf, quot))
    {
      gg= reverseShift (g, eval);
      gg /= Lc (gg);
      if (!k && beta == x)
      {
        if (degree (gg, alpha) < degMipoBeta)
        {
          appendTestMapDown (result, gg, info, source, dest);
          buf= quot;
          nBuf= degree (g, y) + degree (LC (g, x), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, x);
          T= Difference (T, CFList (i.getItem()));
        }
      }
      else
      {
        if (!isInExtension (gg, gamma, k, delta, source, dest))
        {
          appendTestMapDown (result, gg, info, source, dest);
          buf= quot;
          nBuf= degree (g, y) + degree (LC (g, x), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, x);
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
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
#define CHAR_THRESHOLD 8
CFList
evalPoints (const CanonicalForm& F, CFList & eval, const Variable& alpha,
            CFList& list, const bool& GF, bool& fail)
{
  int k= F.level() - 1;
  Variable x= Variable (1);
  CanonicalForm LCF=LC (F, x);
  CFList LCFeval;

  CFList result;
  FFRandom genFF;
  GFRandom genGF;
  int p= getCharacteristic ();
  if (p < CHAR_THRESHOLD)
  {
    if (!GF && alpha.level() == 1)
    {
      fail= true;
      return CFList();
    }
    else if (!GF && alpha.level() != 1)
    {
      if ((p == 2 && degree (getMipo (alpha)) < 6) ||
          (p == 3 && degree (getMipo (alpha)) < 4) ||
          (p == 5 && degree (getMipo (alpha)) < 3))
      {
        fail= true;
        return CFList();
      }
    }
  }
  double bound;
  if (alpha != x)
  {
    bound= pow ((double) p, (double) degree (getMipo(alpha)));
    bound *= (double) k;
  }
  else if (GF)
  {
    bound= pow ((double) p, (double) getGFDegree());
    bound *= (double) k;
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
    LCFeval.insert (LCF);
    bool bad= false;
    for (CFListIterator i= result; i.hasItem(); i++, l--)
    {
      eval.insert (eval.getFirst()(i.getItem(), l));
      LCFeval.insert (LCFeval.getFirst()(i.getItem(), l));
      if (degree (eval.getFirst(), l - 1) != degree (F, l - 1))
      {
        if (!find (list, random))
          list.append (random);
        result= CFList();
        eval= CFList();
        LCFeval= CFList();
        bad= true;
        break;
      }
      if ((l != 2) && (degree (LCFeval.getFirst(), l-1) != degree (LCF, l-1)))
      {
        if (!find (list, random))
          list.append (random);
        result= CFList();
        eval= CFList();
        LCFeval= CFList();
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
      LCFeval= CFList();
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
      LCFeval= CFList();
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
      LCFeval= CFList();
      eval= CFList();
      continue;
    }

    contentx= content (i.getItem());
    if (degree (contentx) > 0)
    {
      if (!find (list, random))
        list.append (random);
      result= CFList();
      LCFeval= CFList();
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
#endif

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
  CanonicalForm buf= A;
  contentAi.append (content (buf, i));
  buf /= contentAi.getLast();
  contentAi.append (content (buf, i - 1));
  CanonicalForm result= lcm (contentAi.getFirst(), contentAi.getLast());
  for (i= i - 2; i > 0; i--)
  {
    contentAi.append (content (buf, i));
    buf /= contentAi.getLast();
    result= lcm (result, contentAi.getLast());
  }
  return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // henselLift23
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
#endif

void
gcdFreeBasis (CFFList& factors1, CFFList& factors2)
{
  CanonicalForm g;
  int k= factors1.length();
  int l= factors2.length();
  int n= 0;
  int m;
  CFFListIterator j;
  for (CFFListIterator i= factors1; (n < k && i.hasItem()); i++, n++)
  {
    m= 0;
    for (j= factors2; (m < l && j.hasItem()); j++, m++)
    {
      g= gcd (i.getItem().factor(), j.getItem().factor());
      if (degree (g,1) > 0)
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
        for (CFListIterator iter2= differentSecondVarFactors[i];iter2.hasItem();
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

  Variable v;
  CFListIterator iter1, iter2;
  CanonicalForm tmp, g;
  CFList multiplier;
  for (int i= 0; i < length; i++)
  {
    if (differentSecondVarFactors[i].isEmpty())
      continue;
    iter1= l;
    iter1++;

    tmp= 1;
    for (iter2= differentSecondVarFactors[i]; iter2.hasItem();
         iter2++, iter1++)
    {
      if (iter2.getItem().inCoeffDomain())
      {
        multiplier.append (1);
        continue;
      }
      v= iter2.getItem().mvar();
      if (degree (iter2.getItem()) == degree (iter1.getItem(),v))
      {
        multiplier.append (1);
        continue;
      }
      g= gcd (iter2.getItem(), content);
      if (!g.inCoeffDomain())
      {
        tmp *= g;
        multiplier.append (g);
      }
      else
        multiplier.append (1);
    }
    if (!tmp.isOne() && fdivides (tmp, content))
    {
      iter1= l;
      iter1++;
      content /= tmp;
      for (iter2= multiplier; iter2.hasItem(); iter1++, iter2++)
        iter1.getItem() *= iter2.getItem();
    }
    multiplier= CFList();
  }

  l.removeFirst();
  l.insert (content);
  return l;
}

int
testFactors (const CanonicalForm& G, const CFList& uniFactors,
             const Variable& alpha, CanonicalForm& sqrfPartF, CFList& factors,
             CFFList*& bufSqrfFactors, CFList& evalSqrfPartF,
             const CFArray& evalPoint)
{
  CanonicalForm F= G;
  CFFList sqrfFactorization;
  if (getCharacteristic() > 0)
    sqrfFactorization= squarefreeFactorization (F, alpha);
  else
    sqrfFactorization= sqrFree (F);

  sqrfPartF= 1;
  for (CFFListIterator i= sqrfFactorization; i.hasItem(); i++)
    sqrfPartF *= i.getItem().factor();

  evalSqrfPartF= evaluateAtEval (sqrfPartF, evalPoint);

  CanonicalForm test= evalSqrfPartF.getFirst() (evalPoint[0], 2);

  if (degree (test) != degree (sqrfPartF, 1) || test.inCoeffDomain())
    return 0;

  CFFList sqrfFactors;
  CanonicalForm tmp;
  CFList tmp2;
  int k= 0;
  factors= uniFactors;
  CFFListIterator iter;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    tmp= 1;
    if (getCharacteristic() > 0)
      sqrfFactors= squarefreeFactorization (i.getItem(), alpha);
    else
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
    for (k= i + 1; k < factors.length(); k++)
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

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // nonMonicHenselLift12
CFList
precomputeLeadingCoeff (const CanonicalForm& LCF, const CFList& LCFFactors,
                        const Variable& alpha, const CFList& evaluation,
                        CFList* & differentSecondVarLCs, int lSecondVarLCs,
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
      for (int i= 0; i < lSecondVarLCs; i++)
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
      result.insert (LCF);

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

  int pass= testFactors (F, factors, alpha, sqrfPartF,
                         bufFactors, bufSqrfFactors, evalSqrfPartF, evalPoint);

  bool foundDifferent= false;
  Variable z, x= y;
  int j= 0;
  if (!pass)
  {
    int lev= 0;
    // LCF is non-constant here
    CFList bufBufFactors;
    CanonicalForm bufF;
    for (int i= 0; i < lSecondVarLCs; i++)
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
        for (int k= 1; k < evaluation.length(); k++)
        {
          if (N (Variable (k+1)).level() != y.level())
            evalPoint[k-1]= buf[k];
          else
            evalPoint[k-1]= buf[0];
        }
        pass= testFactors (bufF, bufBufFactors, alpha, sqrfPartF, bufFactors,
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
        if (!pass && i == lSecondVarLCs - 1)
        {
          CFList result;
          result.append (LCF);
          for (int j= 1; j <= factors.length(); j++)
            result.append (1);
          result= distributeContent (result, differentSecondVarLCs, lSecondVarLCs);
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
      result.append (1);
    result= distributeContent (result, differentSecondVarLCs, lSecondVarLCs);
    y= Variable (1);
    delete [] bufSqrfFactors;
    return result;
  }
  else
    factors= bufFactors;

  bufFactors= factors;

  CFMap MM, NN;
  dummy [0]= sqrfPartF;
  dummy [1]= 1;
  compress (dummy, MM, NN);
  sqrfPartF= MM (sqrfPartF);
  CanonicalForm varsSqrfPartF= getVars (sqrfPartF);
  for (CFListIterator iter= factors; iter.hasItem(); iter++)
    iter.getItem()= MM (iter.getItem());

  CFList evaluation2;
  for (int i= 2; i <= varsSqrfPartF.level(); i++)
    evaluation2.insert (evalPoint[NN (Variable (i)).level()-2]);

  CFList interMedResult;
  CanonicalForm oldSqrfPartF= sqrfPartF;
  sqrfPartF= shift2Zero (sqrfPartF, evalSqrfPartF, evaluation2);
  if (factors.length() > 1)
  {
    CanonicalForm LC1= LC (oldSqrfPartF, 1);
    CFList leadingCoeffs;
    for (int i= 0; i < factors.length(); i++)
      leadingCoeffs.append (LC1);

    CFList LC1eval= evaluateAtEval (LC1, evaluation2, 2);
    CFList oldFactors= factors;
    for (CFListIterator i= oldFactors; i.hasItem(); i++)
      i.getItem() *= LC1eval.getFirst()/Lc (i.getItem());

    bool success= false;
    CanonicalForm oldSqrfPartFPowLC= oldSqrfPartF*power(LC1,factors.length()-1);
    CFList heuResult;
    if (size (oldSqrfPartFPowLC)/getNumVars (oldSqrfPartFPowLC) < 500 &&
        LucksWangSparseHeuristic (oldSqrfPartFPowLC,
                                  oldFactors, 2, leadingCoeffs, heuResult))
    {
      interMedResult= recoverFactors (oldSqrfPartF, heuResult);
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
        for (int i= 0; i < liftBoundsLength; i++)
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

  for (CFListIterator iter= interMedResult; iter.hasItem(); iter++)
    iter.getItem()= NN (iter.getItem());

  CFList result;
  CFFListIterator k;
  for (int i= 0; i < LCFFactors.length(); i++)
  {
    CanonicalForm tmp= 1;
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

  result= distributeContent (result, differentSecondVarLCs, lSecondVarLCs);

  if (!result.getFirst().inCoeffDomain())
  {
    // prepare input for recursion
    if (foundDifferent)
    {
      for (CFListIterator i= result; i.hasItem(); i++)
        i.getItem()= swapvar (i.getItem(), Variable (2), y);
      CFList l= differentSecondVarLCs [j];
      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= swapvar (i.getItem(), y, z);
      differentSecondVarLCs [j]= l;
    }

    F= result.getFirst();
    int level= 0;
    if (foundDifferent)
    {
      level= y.level() - 2;
      for (int i= y.level(); i > 1; i--)
      {
        if (degree (F,i) > 0)
        {
          if (y.level() == 3)
            level= 0;
          else
            level= i-3;
        }
      }
    }
lcretry:
    if (lSecondVarLCs - level > 0)
    {
      CFList evaluation2= evaluation;
      int j= lSecondVarLCs+2;
      CanonicalForm swap;
      CFListIterator i;
      for (i= evaluation2; i.hasItem(); i++, j--)
      {
        if (j==y.level())
        {
          swap= i.getItem();
          i.getItem()= evaluation2.getLast();
          evaluation2.removeLast();
          evaluation2.append (swap);
        }
      }

      CFList newLCs= differentSecondVarLCs[level];
      if (newLCs.isEmpty())
      {
        if (degree (F, level+3) > 0)
        {
          delete [] bufSqrfFactors;
          return result; //TODO handle this case
        }
        level=level+1;
        goto lcretry;
      }
      i= newLCs;
      CFListIterator iter= result;
      iter++;
      CanonicalForm quot;
      for (;iter.hasItem(); iter++, i++)
      {
        swap= iter.getItem();
        if (degree (swap, level+3) > 0)
        {
          int count= evaluation.length()+1;
          for (CFListIterator iter2= evaluation2; iter2.hasItem(); iter2++,
                                                                    count--)
          {
            if (count != level+3)
              swap= swap (iter2.getItem(), count);
          }
          if (fdivides (swap, i.getItem(), quot))
            i.getItem()= quot;
        }
      }
      CFList * differentSecondVarLCs2= new CFList [lSecondVarLCs - level - 1];
      for (int j= level+1; j < lSecondVarLCs; j++)
      {
        if (degree (F, j+3) > 0)
        {
          if (!differentSecondVarLCs[j].isEmpty())
          {
            differentSecondVarLCs2[j - level - 1]= differentSecondVarLCs[j];
            i= differentSecondVarLCs2[j-level - 1];
            iter=result;
            iter++;
            for (;iter.hasItem(); iter++, i++)
            {
              swap= iter.getItem();
              if (degree (swap, j+3) > 0)
              {
                int count= evaluation.length()+1;
                for (CFListIterator iter2= evaluation2; iter2.hasItem();iter2++,
                                                                        count--)
                {
                  if (count != j+3)
                    swap= swap (iter2.getItem(), count);
                }
                if (fdivides (swap, i.getItem(), quot))
                  i.getItem()= quot;
              }
            }
          }
        }
      }

      for (int j= 0; j < level+1; j++)
        evaluation2.removeLast();
      Variable dummyvar= Variable (1);

      CanonicalForm newLCF= result.getFirst();
      newLCF=swapvar (newLCF, Variable (2), Variable (level+3));
      for (i=newLCs; i.hasItem(); i++)
        i.getItem()= swapvar (i.getItem(), Variable (2), Variable (level+3));
      for (int j= 1; j < lSecondVarLCs-level;j++)
      {
        for (i= differentSecondVarLCs2[j-1]; i.hasItem(); i++)
          i.getItem()= swapvar (i.getItem(), Variable (2+j),
                                             Variable (level+3+j));
        newLCF= swapvar (newLCF, Variable (2+j), Variable (level+3+j));
      }

      CFList recursiveResult=
      precomputeLeadingCoeff (newLCF, newLCs, alpha, evaluation2,
                              differentSecondVarLCs2, lSecondVarLCs - level - 1,
                              dummyvar);

      if (dummyvar.level() != 1)
      {
        for (i= recursiveResult; i.hasItem(); i++)
          i.getItem()= swapvar (i.getItem(), Variable (2), dummyvar);
      }
      for (i= recursiveResult; i.hasItem(); i++)
      {
        for (int j= lSecondVarLCs-level-1; j > 0; j--)
          i.getItem()=swapvar (i.getItem(), Variable (2+j),
                                      Variable (level+3+j));
        i.getItem()= swapvar (i.getItem(), Variable (2), Variable (level+3));
      }

      if (recursiveResult.getFirst() == result.getFirst())
      {
        delete [] bufSqrfFactors;
        delete [] differentSecondVarLCs2;
        return result;
      }
      else
      {
        iter=recursiveResult;
        i= result;
        i.getItem()= iter.getItem();
        i++;
        iter++;
        for (; i.hasItem(); i++, iter++)
          i.getItem() *= iter.getItem();
        delete [] differentSecondVarLCs2;
      }
    }
  }
  else
    y= Variable (1);

  delete [] bufSqrfFactors;

  return result;
}
#endif

void
evaluationWRTDifferentSecondVars (CFList*& Aeval, const CFList& evaluation,
                                  const CanonicalForm& A)
{
  CanonicalForm tmp;
  CFList tmp2;
  CFListIterator iter;
  bool preserveDegree= true;
  Variable x= Variable (1);
  int j, degAi, degA1= degree (A,1);
  for (int i= A.level(); i > 2; i--)
  {
    tmp= A;
    tmp2= CFList();
    iter= evaluation;
    preserveDegree= true;
    degAi= degree (A,i);
    for (j= A.level(); j > 1; j--, iter++)
    {
      if (j == i)
        continue;
      else
      {
        tmp= tmp (iter.getItem(), j);
        tmp2.insert (tmp);
        if ((degree (tmp, i) != degAi) ||
            (degree (tmp, 1) != degA1))
        {
          preserveDegree= false;
          break;
        }
      }
    }
    if (!content(tmp,1).inCoeffDomain())
      preserveDegree= false;
    if (!content(tmp).inCoeffDomain())
      preserveDegree= false;
    if (!(gcd (deriv (tmp,x), tmp)).inCoeffDomain())
      preserveDegree= false;
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

void
checkHelper (const CanonicalForm& f1, CFList& factors1, CFList& factors2,
             CFList& l1, CFList& l2)
{
  CanonicalForm g1= f1, g2;
  CFListIterator iter1= factors1, iter2= factors2;
  for (; iter1.hasItem(); iter1++, iter2++)
  {
    g2= gcd (g1, iter1.getItem());
    if (!g2.inCoeffDomain())
    {
      l1.append (iter1.getItem());
      l2.append (iter2.getItem());
      g1 /= g2;
    }
  }
  factors1= Difference (factors1, l1);
  factors2= Difference (factors2, l2);
}

/// check if univariate factors @a factors2 of @a factors3 coincide with
/// univariate factors of @a factors1 and recombine if necessary.
/// The recombined factors of @a factors1 are returned and @a factors3 is
/// recombined accordingly.
CFList
checkOneToOne (const CFList& factors1, const CFList& factors2, CFList& factors3,
               const CanonicalForm& evalPoint, const Variable& x)
{
  CFList uniFactorsOfFactors1;
  CFList result, result2;
  CFList bad1= factors2;
  CFListIterator iter, iter2, iter3;
  CanonicalForm tmp;
  int pos;

  for (iter= factors1; iter.hasItem(); iter++)
  {
    tmp= iter.getItem() (evalPoint, x);
    tmp /= Lc (tmp);
    if ((pos= findItem (factors2, tmp)))
    {
      result2.append (getItem (factors3, pos));
      result.append (iter.getItem());
      bad1= Difference (bad1, CFList (tmp));
    }
    else
      uniFactorsOfFactors1.append (tmp);
  }

  CFList bad2, bad3;
  bad2= Difference (factors1, result);
  bad3= Difference (factors3, result2);
  CFList tmp2, tmp3;
  CanonicalForm g1, g2, g3, g4;

  while (!uniFactorsOfFactors1.isEmpty())
  {
    tmp= uniFactorsOfFactors1.getFirst();
    checkHelper (tmp, bad1, bad3, tmp2, tmp3);
    g1= prod (tmp2);
    g2= prod (tmp3);
    tmp2= CFList();
    tmp3= CFList();
    checkHelper (g1, uniFactorsOfFactors1, bad2, tmp2, tmp3);
    g3= prod (tmp2);
    g4= prod (tmp3);
    tmp2= CFList();
    tmp3= CFList();
    do
    {
      checkHelper (g3, bad1, bad3, tmp2, tmp3);
      g1 *= prod (tmp2);
      g2 *= prod (tmp3);
      tmp2= CFList();
      tmp3= CFList();
      checkHelper (g1, uniFactorsOfFactors1, bad2, tmp2, tmp3);
      g3 *= prod (tmp2);
      g4 *= prod (tmp3);
      tmp2= CFList();
      tmp3= CFList();
    } while (!bad2.isEmpty() && !bad3.isEmpty());
    result.append (g4);
    result2.append (g2);
  }

  if (factors3.length() != result2.length())
    factors3= result2;
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
  int recombinations= 0;
  while (T.length() >= 2*s && s <= thres)
  {
    while (nosubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombinations == factors2.length() - 1)
          result.append (prod (T));
        else
          result= Union (result, T);
        return result;
      }
      S= subset (v, s, TT, nosubset);
      if (nosubset) break;
      buf= prodEval (S, evalPoint, x);
      buf /= Lc (buf);
      if (find (factors2, buf))
      {
        recombinations++;
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
      if (recombinations == factors2.length() - 1)
        result.append (prod (T));
      else
        result= Union (result, T);
      delete [] v;
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }

  delete [] v;
  if (T.length() < 2*s)
  {
    result= Union (result, T);
    return result;
  }

  return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
#ifdef HAVE_NTL // GFBiSqrfFactorize
void
factorizationWRTDifferentSecondVars (const CanonicalForm& A, CFList*& Aeval,
                                     const ExtensionInfo& info,
                                     int& minFactorsLength, bool& irred)
{
  Variable x= Variable (1);
  minFactorsLength= 0;
  irred= false;
  CFList factors;
  Variable v;
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      v= Variable (Aeval[j].getFirst().level());
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
#endif
#endif

CFList conv (const CFArray & A)
{
  CFList result;
  for (int i= A.max(); i >= A.min(); i--)
    result.insert (A[i]);
  return result;
}


void getLeadingCoeffs (const CanonicalForm& A, CFList*& Aeval
                      )
{
  CFListIterator iter;
  CFList LCs;
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      LCs= CFList();
      for (iter= Aeval[j]; iter.hasItem(); iter++)
        LCs.append (LC (iter.getItem(), 1));
      //normalize (LCs);
      Aeval[j]= LCs;
    }
  }
}

void sortByUniFactors (CFList*& Aeval, int AevalLength,
                       CFList& uniFactors, CFList& biFactors,
                       const CFList& evaluation
                      )
{
  CanonicalForm evalPoint;
  int i;
  CFListIterator iter, iter2;
  Variable v;
  CFList LCs, buf;
  CFArray l;
  int pos, index, checklength;
  bool leaveLoop=false;
recurse:
  for (int j= 0; j < AevalLength; j++)
  {
    if (!Aeval[j].isEmpty())
    {
      i= evaluation.length() + 1;
      for (iter= evaluation; iter.hasItem(); iter++, i--)
      {
        for (iter2= Aeval[j]; iter2.hasItem(); iter2++)
        {
          if (i == iter2.getItem().level())
          {
            evalPoint= iter.getItem();
            leaveLoop= true;
            break;
          }
        }
        if (leaveLoop)
        {
          leaveLoop= false;
          break;
        }
      }

      v= Variable (i);
      if (Aeval[j].length() > uniFactors.length())
        Aeval[j]= recombination (Aeval[j], uniFactors, 1,
                                 Aeval[j].length() - uniFactors.length() + 1,
                                 evalPoint, v);

      checklength= biFactors.length();
      Aeval[j]= checkOneToOne (Aeval[j], uniFactors, biFactors, evalPoint, v);
      if (checklength > biFactors.length())
      {
        uniFactors= buildUniFactors (biFactors, evaluation.getLast(),
                                     Variable (2));
        goto recurse;
      }

      buf= buildUniFactors (Aeval[j], evalPoint, v);
      l= CFArray (uniFactors.length());
      index= 1;
      for (iter= buf; iter.hasItem(); iter++, index++)
      {
        pos= findItem (uniFactors, iter.getItem());
        if (pos)
          l[pos-1]= getItem (Aeval[j], index);
      }
      buf= conv (l);
      Aeval [j]= buf;

      buf= buildUniFactors (Aeval[j], evalPoint, v);
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
  CFListIterator iter, iter2;
  CanonicalForm evalPoint;
  int i;
  Variable v;
  Variable y= Variable (2);
  CFList list;
  bool leaveLoop= false;
  for (int j= 0; j < A.level() - 2; j++)
  {
    if (Aeval[j].length() == minFactorsLength)
    {
      i= A.level();

      for (iter= evaluation; iter.hasItem(); iter++, i--)
      {
        for (iter2= Aeval[j]; iter2.hasItem(); iter2++)
        {
          if (i == iter2.getItem().level())
          {
            evalPoint= iter.getItem();
            leaveLoop= true;
            break;
          }
        }
        if (leaveLoop)
        {
          leaveLoop= false;
          break;
        }
      }

      v= Variable (i);
      list= buildUniFactors (Aeval[j], evalPoint, v);

      biFactors= recombination (biFactors, list, 1,
                                biFactors.length() - list.length() + 1,
                                evaluation.getLast(), y);
      return;
    }
  }
}

void
prepareLeadingCoeffs (CFList*& LCs, CanonicalForm& A, CFList& Aeval, int n,
                      const CFList& leadingCoeffs, const CFList& biFactors,
                      const CFList& evaluation)
{
  CFList l= leadingCoeffs;
  LCs [n-3]= l;
  CFListIterator j;
  CFListIterator iter= evaluation;
  for (int i= n - 1; i > 2; i--, iter++)
  {
    for (j= l; j.hasItem(); j++)
      j.getItem()= j.getItem() (iter.getItem(), i + 1);
    LCs [i - 3]= l;
  }
  l= LCs [0];
  for (CFListIterator i= l; i.hasItem(); i++)
    i.getItem()= i.getItem() (iter.getItem(), 3);
  CFListIterator ii= biFactors;
  CFList normalizeFactor;
  for (CFListIterator i= l; i.hasItem(); i++, ii++)
    normalizeFactor.append (Lc (LC (ii.getItem(), 1))/Lc (i.getItem()));
  for (int i= 0; i < n-2; i++)
  {
    ii= normalizeFactor;
    for (j= LCs [i]; j.hasItem(); j++, ii++)
      j.getItem() *= ii.getItem();
  }

  Aeval= evaluateAtEval (A, evaluation, 2);

  CanonicalForm hh= 1/Lc (Aeval.getFirst());

  for (iter= Aeval; iter.hasItem(); iter++)
    iter.getItem() *= hh;

  A *= hh;
}

CFList
extNonMonicFactorRecombination (const CFList& factors, const CanonicalForm& F,
                                const ExtensionInfo& info)
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
  CanonicalForm quot, buf= F;

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
          g /= myContent (g);
          g /= Lc (g);
          appendTestMapDown (result, g, info, source, dest);
          return result;
        }
        else
          return CFList (buf/myContent(buf));
      }

      S= subset (v, s, TT, noSubset);
      if (noSubset) break;

      g= prod (S);
      g /= myContent (g);
      if (fdivides (g, buf, quot))
      {
        buf2= g;
        buf2 /= Lc (buf2);
        if (!k && beta.level() == 1)
        {
          if (degree (buf2, alpha) < degMipoBeta)
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf= quot;
            recombination= true;
            trueFactor= true;
          }
        }
        else
        {
          if (!isInExtension (buf2, gamma, k, delta, source, dest))
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf= quot;
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
            buf /= myContent (buf);
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
      buf /= myContent (buf);
      buf /= Lc (buf);
      appendTestMapDown (result, buf, info, source, dest);
      return result;
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
  {
    buf= F/myContent (F);
    buf /= Lc (buf);
    appendMapDown (result, buf, info, source, dest);
  }

  delete [] v;
  return result;
}

void
changeSecondVariable (CanonicalForm& A, CFList& biFactors, CFList& evaluation,
                      CFList*& oldAeval, int lengthAeval2,
                      const CFList& uniFactors, const Variable& w)
{
  Variable y= Variable (2);
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

void
distributeLCmultiplier (CanonicalForm& A, CFList& leadingCoeffs,
                        CFList& biFactors, const CFList& evaluation,
                        const CanonicalForm& LCmultipler)
{
  CanonicalForm tmp= power (LCmultipler, biFactors.length() - 1);
  A *= tmp;
  tmp= LCmultipler;
  CFListIterator iter= leadingCoeffs;
  for (;iter.hasItem(); iter++)
    iter.getItem() *= LCmultipler;
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

void
LCHeuristic (CanonicalForm& A, const CanonicalForm& LCmultiplier,
             CFList& biFactors, CFList*& leadingCoeffs, const CFList* oldAeval,
             int lengthAeval, const CFList& evaluation,
             const CFList& oldBiFactors)
{
  CFListIterator iter, iter2;
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
  for (int i= 0; i < lengthAeval; i++)
  {
    if (oldAeval[i].isEmpty())
      continue;
    xx= oldAeval[i].getFirst().mvar();
    iter2= vars1;
    for (iter= oldAeval[i]; iter.hasItem(); iter++, iter2++)
      iter2.getItem() *= power (xx, degree (LC (iter.getItem(),1), xx));
  }
  CanonicalForm tmp, quot1, quot2, quot3;
  iter2= vars1;
  for (iter= leadingCoeffs[lengthAeval-1]; iter.hasItem(); iter++, iter2++)
  {
    tmp= iter.getItem()/LCmultiplier;
    for (int i=1; i <= tmp.level(); i++)
    {
      if (degree (tmp,i) > 0 && (degree (iter2.getItem(),i) > degree (tmp,i)))
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
          for (iter2= leadingCoeffs[lengthAeval-1]; iter2.hasItem();iter2++,
                                                                    index2++)
          {
            if (index2 == index)
              continue;
            else
            {
              tmp= ii.getItem().factor();
              if (fdivides (tmp, iter2.getItem(), quot1))
              {
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
                      if (fdivides (tmp, iter3.getItem(), quot2))
                      {
                        if (fdivides (ii.getItem().factor(), A, quot3))
                        {
                          A               = quot3;
                          iter2.getItem() = quot2;
                          iter3.getItem() = quot3;
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
          for (iter2= leadingCoeffs[lengthAeval-1];iter2.hasItem();iter2++,
                                                                    index2++)
          {
            if (index2 == index)
            {
              tmp= power (ii.getItem().factor(), ii.getItem().exp());
              if (fdivides (tmp, A, quot1))
              {
                if (fdivides (tmp, iter2.getItem()))
                {
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
                        if (fdivides (tmp, iter3.getItem(), quot3))
                        {
                          A               =  quot1;
                          iter2.getItem() =  quot2;
                          iter3.getItem() =  quot3;
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
  }
}

void
LCHeuristicCheck (const CFList& LCs, const CFList& contents, CanonicalForm& A,
                  const CanonicalForm& oldA, CFList& leadingCoeffs,
                  bool& foundTrueMultiplier)
{
  CanonicalForm pLCs= prod (LCs);
  if (fdivides (pLCs, LC (oldA,1)) && (LC(oldA,1)/pLCs).inCoeffDomain()) // check if the product of the lead coeffs of the primitive factors equals the lead coeff of the old A
  {
    A= oldA;
    CFListIterator iter2= leadingCoeffs;
    for (CFListIterator iter= contents; iter.hasItem(); iter++, iter2++)
      iter2.getItem() /= iter.getItem();
    foundTrueMultiplier= true;
  }
}

void
LCHeuristic2 (const CanonicalForm& LCmultiplier, const CFList& factors,
              CFList& leadingCoeffs, CFList& contents, CFList& LCs,
              bool& foundTrueMultiplier)
{
  CanonicalForm cont;
  int index= 1;
  CFListIterator iter2;
  for (CFListIterator iter= factors; iter.hasItem(); iter++, index++)
  {
    cont= content (iter.getItem(), 1);
    cont= gcd (cont, LCmultiplier);
    contents.append (cont);
    if (cont.inCoeffDomain()) // trivial content->LCmultiplier needs to go there
    {
      foundTrueMultiplier= true;
      int index2= 1;
      for (iter2= leadingCoeffs; iter2.hasItem(); iter2++, index2++)
      {
        if (index2 == index)
          continue;
        iter2.getItem() /= LCmultiplier;
      }
      break;
    }
    else
      LCs.append (LC (iter.getItem()/cont, 1));
  }
}

void
LCHeuristic3 (const CanonicalForm& LCmultiplier, const CFList& factors,
              const CFList& oldBiFactors, const CFList& contents,
              const CFList* oldAeval, CanonicalForm& A, CFList*& leadingCoeffs,
              int lengthAeval, bool& foundMultiplier)
{
  int index= 1;
  CFListIterator iter, iter2= factors;
  for (iter= contents; iter.hasItem(); iter++, iter2++, index++)
  {
    if (fdivides (iter.getItem(), LCmultiplier))
    {
      if ((LCmultiplier/iter.getItem()).inCoeffDomain() &&
          !isOnlyLeadingCoeff(iter2.getItem())) //content divides LCmultiplier completely and factor consists of more terms than just the leading coeff
      {
        Variable xx= Variable (2);
        CanonicalForm vars;
        vars= power (xx, degree (LC (getItem(oldBiFactors, index),1),
                                  xx));
        for (int i= 0; i < lengthAeval; i++)
        {
          if (oldAeval[i].isEmpty())
            continue;
          xx= oldAeval[i].getFirst().mvar();
          vars *= power (xx, degree (LC (getItem(oldAeval[i], index),1),
                                      xx));
        }
        if (vars.level() <= 2)
        {
          int index2= 1;
          for (CFListIterator iter3= leadingCoeffs[lengthAeval-1];
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
  }
}

void
LCHeuristic4 (const CFList& oldBiFactors, const CFList* oldAeval,
              const CFList& contents, const CFList& factors,
              const CanonicalForm& testVars, int lengthAeval,
              CFList*& leadingCoeffs, CanonicalForm& A,
              CanonicalForm& LCmultiplier, bool& foundMultiplier)
{
  int index=1;
  CFListIterator iter, iter2= factors;
  for (iter= contents; iter.hasItem(); iter++, iter2++, index++)
  {
    if (!iter.getItem().isOne() &&
        fdivides (iter.getItem(), LCmultiplier))
    {
      if (!isOnlyLeadingCoeff (iter2.getItem())) // factor is more than just leading coeff
      {
        int index2= 1;
        for (iter2= leadingCoeffs[lengthAeval-1]; iter2.hasItem();
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
        for (int i= 0; i < lengthAeval; i++)
        {
          if (oldAeval[i].isEmpty())
            continue;
          xx= oldAeval[i].getFirst().mvar();
          vars *= power (xx, degree (LC (getItem(oldAeval[i], index),1),
                                      xx));
        }
        if (myGetVars(content(getItem(leadingCoeffs[lengthAeval-1],index),1))
            /myGetVars (LCmultiplier) == vars)
        {
          int index2= 1;
          for (iter2= leadingCoeffs[lengthAeval-1]; iter2.hasItem();
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

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
#ifdef HAVE_NTL // biSqrfFactorizeHelper
CFList
extFactorize (const CanonicalForm& F, const ExtensionInfo& info);

CFList
multiFactorize (const CanonicalForm& F, const ExtensionInfo& info)
{
  if (F.inCoeffDomain())
    return CFList (F);

  TIMING_START (fac_fq_preprocess_and_content);
  // compress and find main Variable
  CFMap N;
  TIMING_START (fac_fq_compress)
  CanonicalForm A= myCompress (F, N);
  TIMING_END_AND_PRINT (fac_fq_compress, "time to compress poly over Fq: ")

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
    CFList buf= biSqrfFactorizeHelper (F, info);
    if (buf.getFirst().inCoeffDomain())
      buf.removeFirst();
    return buf;
  }

  Variable x= Variable (1);
  Variable y= Variable (2);

  // remove content
  TIMING_START (fac_fq_content);
  CFList contentAi;
  CanonicalForm lcmCont= lcmContent (A, contentAi);
  A /= lcmCont;
  TIMING_END_AND_PRINT (fac_fq_content, "time to extract content over Fq: ");

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
    if (!extension)
      normalize (contentAFactors);
    return contentAFactors;
  }

  // factorize content
  TIMING_START (fac_fq_content);
  contentAFactors= multiFactorize (lcmCont, info);
  TIMING_END_AND_PRINT (fac_fq_content, "time to factor content over Fq: ");

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
  TIMING_START (fac_fq_check_mainvar);
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
      gcdDerivZ= gcd (bufA, derivZ);
      if (degree (gcdDerivZ) > 0 && !derivZ.isZero())
      {
        CanonicalForm g= bufA/gcdDerivZ;
        CFList factorsG=
        Union (multiFactorize (g, info),
               multiFactorize (gcdDerivZ, info));
        appendSwapDecompress (factorsG, contentAFactors, N, swapLevel, x);
        if (!extension)
          normalize (factorsG);
        return factorsG;
      }
      else
      {
        if (swapLevel == 1)
        {
          swapLevel= i;
          bufA= swapvar (A, x, z);
        }
        A= bufA;
        break;
      }
    }
  }
  TIMING_END_AND_PRINT (fac_fq_check_mainvar,
                        "time to check main var over Fq: ");
  TIMING_END_AND_PRINT (fac_fq_preprocess_and_content,
                       "time to preprocess poly and extract content over Fq: ");

  CFList Aeval, list, evaluation, bufEvaluation, bufAeval;
  bool fail= false;
  int swapLevel2= 0;
  //int level;
  int factorNums= 3;
  CFList biFactors, bufBiFactors;
  CanonicalForm evalPoly;
  int lift, bufLift, lengthAeval2= A.level()-2;
  double logarithm= (double) ilog2 (totaldegree (A));
  logarithm /= log2exp;
  logarithm= ceil (logarithm);
  if (factorNums < (int) logarithm)
    factorNums= (int) logarithm;
  CFList* bufAeval2= new CFList [lengthAeval2];
  CFList* Aeval2= new CFList [lengthAeval2];
  int counter;
  int differentSecondVar= 0;
  // several bivariate factorizations
  TIMING_START (fac_fq_bifactor_total);
  for (int i= 0; i < factorNums; i++)
  {
    counter= 0;
    bufA= A;
    bufAeval= CFList();
    TIMING_START (fac_fq_evaluation);
    bufEvaluation= evalPoints (bufA, bufAeval, alpha, list, GF, fail);
    TIMING_END_AND_PRINT (fac_fq_evaluation,
                          "time to find evaluation point over Fq: ");
    evalPoly= 0;

    if (fail && (i == 0))
    {
      /*if (!swapLevel) //uncomment to reenable search for new main variable
        level= 2;
      else
        level= swapLevel + 1;*/

      //CanonicalForm g;
      //swapLevel2= newMainVariableSearch (A, Aeval, evaluation, alpha, level, g);

      /*if (!swapLevel2) // need to pass to an extension
      {*/
        factors= extFactorize (A, info);
        appendSwapDecompress (factors, contentAFactors, N, swapLevel, x);
        normalize (factors);
        delete [] bufAeval2;
        delete [] Aeval2;
        return factors;
      /*}
      else
      {
        if (swapLevel2 == -1)
        {
          CFList factorsG=
          Union (multiFactorize (g, info),
                 multiFactorize (A/g, info));
          appendSwapDecompress (factorsG, contentAFactors, N, swapLevel, x);
          if (!extension)
            normalize (factorsG);
          delete [] bufAeval2;
          delete [] Aeval2;
          return factorsG;
        }
        fail= false;
        bufAeval= Aeval;
        bufA= A;
        bufEvaluation= evaluation;
      }*/ //end uncomment
    }
    else if (fail && (i > 0))
      break;

    TIMING_START (fac_fq_evaluation);
    evaluationWRTDifferentSecondVars (bufAeval2, bufEvaluation, A);
    TIMING_END_AND_PRINT (fac_fq_evaluation,
                          "time for evaluation wrt diff second vars over Fq: ");

    for (int j= 0; j < lengthAeval2; j++)
    {
      if (!bufAeval2[j].isEmpty())
        counter++;
    }

    bufLift= degree (A, y) + 1 + degree (LC(A, x), y);

    TIMING_START (fac_fq_bi_factorizer);
    if (!GF && alpha.level() == 1)
      bufBiFactors= FpBiSqrfFactorize (bufAeval.getFirst());
    else if (GF)
      bufBiFactors= GFBiSqrfFactorize (bufAeval.getFirst());
    else
      bufBiFactors= FqBiSqrfFactorize (bufAeval.getFirst(), alpha);
    TIMING_END_AND_PRINT (fac_fq_bi_factorizer,
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
      if (!extension)
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
  TIMING_START (fac_fq_bi_factorizer);
  factorizationWRTDifferentSecondVars (A, Aeval2, info, minFactorsLength,irred);
  TIMING_END_AND_PRINT (fac_fq_bi_factorizer,
             "time for bivariate factorization wrt diff second vars over Fq: ");

  TIMING_END_AND_PRINT (fac_fq_bifactor_total,
                        "total time for eval and bivar factors over Fq: ");
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
    if (!extension)
      normalize (factors);
    delete [] Aeval2;
    return factors;
  }

  if (minFactorsLength == 0)
    minFactorsLength= biFactors.length();
  else if (biFactors.length() > minFactorsLength)
    refineBiFactors (A, biFactors, Aeval2, evaluation, minFactorsLength);
  minFactorsLength= tmin (minFactorsLength, biFactors.length());

  CFList uniFactors= buildUniFactors (biFactors, evaluation.getLast(), y);

  sortByUniFactors (Aeval2, lengthAeval2, uniFactors, biFactors, evaluation);

  minFactorsLength= tmin (minFactorsLength, biFactors.length());

  if (minFactorsLength == 1)
  {
    if (extension)
    {
      CFList source, dest;
      A= mapDown (A, info, source, dest);
    }
    factors.append (A);
    appendSwapDecompress (factors, contentAFactors, N, swapLevel,
                          swapLevel2, x);
    if (!extension)
      normalize (factors);
    delete [] Aeval2;
    return factors;
  }

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
        if (extension)
          factors= extNonMonicFactorRecombination (factors, A, info);

        appendSwapDecompress (factors, contentAFactors, N, swapLevel,
                              swapLevel2, x);
        if (!extension)
          normalize (factors);
        delete [] Aeval2;
        return factors;
      }
      else
        factors= CFList();
      //TODO case where factors.length() > 0
    }
  }

  CFList * oldAeval= new CFList [lengthAeval2]; //TODO use bufAeval2 for this
  for (int i= 0; i < lengthAeval2; i++)
    oldAeval[i]= Aeval2[i];

  getLeadingCoeffs (A, Aeval2);

  CFList biFactorsLCs;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
    biFactorsLCs.append (LC (i.getItem(), 1));

  Variable v;
  TIMING_START (fac_fq_precompute_leadcoeff);
  CFList leadingCoeffs= precomputeLeadingCoeff (LC (A, 1), biFactorsLCs, alpha,
                                          evaluation, Aeval2, lengthAeval2, v);

  if (v.level() != 1)
    changeSecondVariable (A, biFactors, evaluation, oldAeval, lengthAeval2,
                          uniFactors, v);

  CanonicalForm oldA= A;
  CFList oldBiFactors= biFactors;

  CanonicalForm LCmultiplier= leadingCoeffs.getFirst();
  bool LCmultiplierIsConst= LCmultiplier.inCoeffDomain();
  leadingCoeffs.removeFirst();

  if (!LCmultiplierIsConst)
    distributeLCmultiplier (A, leadingCoeffs, biFactors, evaluation, LCmultiplier);

  //prepare leading coefficients
  CFList* leadingCoeffs2= new CFList [lengthAeval2];
  prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(), leadingCoeffs,
                        biFactors, evaluation);

  CFListIterator iter;
  CFList bufLeadingCoeffs2= leadingCoeffs2[lengthAeval2-1];
  bufBiFactors= biFactors;
  bufA= A;
  CanonicalForm testVars, bufLCmultiplier= LCmultiplier;
  if (!LCmultiplierIsConst)
  {
    testVars= Variable (2);
    for (int i= 0; i < lengthAeval2; i++)
    {
      if (!oldAeval[i].isEmpty())
        testVars *= oldAeval[i].getFirst().mvar();
    }
  }
  TIMING_END_AND_PRINT(fac_fq_precompute_leadcoeff,
                       "time to precompute LC over Fq: ");

  TIMING_START (fac_fq_luckswang);
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
      if (extension)
        factors= extNonMonicFactorRecombination (factors, bufA, info);

      if (v.level() != 1)
      {
        for (iter= factors; iter.hasItem(); iter++)
          iter.getItem()= swapvar (iter.getItem(), v, y);
      }

      appendSwapDecompress (factors, contentAFactors, N, swapLevel,
                            swapLevel2, x);
      if (!extension)
        normalize (factors);
      delete [] index;
      delete [] Aeval2;
      TIMING_END_AND_PRINT (fac_fq_luckswang,
                            "time for successful LucksWang over Fq: ");
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
      CFList contents, LCs;
      bool foundTrueMultiplier= false;
      LCHeuristic2 (LCmultiplier, factors, leadingCoeffs2[lengthAeval2-1],
                    contents, LCs, foundTrueMultiplier);
      if (foundTrueMultiplier)
      {
          A= oldA;
          leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
          for (int i= lengthAeval2-1; i > -1; i--)
            leadingCoeffs2[i]= CFList();
          prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),
                                leadingCoeffs, biFactors, evaluation);
      }
      else
      {
        bool foundMultiplier= false;
        LCHeuristic3 (LCmultiplier, factors, oldBiFactors, contents, oldAeval,
                      A, leadingCoeffs2, lengthAeval2, foundMultiplier);

        // coming from above: divide out more LCmultiplier if possible
        if (foundMultiplier)
        {
          foundMultiplier= false;
          LCHeuristic4 (oldBiFactors, oldAeval, contents, factors, testVars,
                        lengthAeval2, leadingCoeffs2, A, LCmultiplier,
                        foundMultiplier);
        }
        else
        {
          LCHeuristicCheck (LCs, contents, A, oldA,
                            leadingCoeffs2[lengthAeval2-1], foundMultiplier);
          if (!foundMultiplier && fdivides (getVars (LCmultiplier), testVars))
          {
            LCHeuristic (A, LCmultiplier, biFactors, leadingCoeffs2, oldAeval,
                         lengthAeval2, evaluation, oldBiFactors);
          }
        }

        // patch everything together again
        leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
        for (int i= lengthAeval2-1; i > -1; i--)
          leadingCoeffs2[i]= CFList();
        prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),leadingCoeffs,
                              biFactors, evaluation);
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
  TIMING_END_AND_PRINT (fac_fq_luckswang, "time for LucksWang over Fq: ");

  TIMING_START (fac_fq_lcheuristic);
  if (!LCheuristic && !LCmultiplierIsConst && bufFactors.isEmpty()
      && fdivides (getVars (LCmultiplier), testVars))
  {
    LCheuristic= true;
    LCHeuristic (A, LCmultiplier, biFactors, leadingCoeffs2, oldAeval,
                 lengthAeval2, evaluation, oldBiFactors);

    leadingCoeffs= leadingCoeffs2[lengthAeval2-1];
    for (int i= lengthAeval2-1; i > -1; i--)
      leadingCoeffs2[i]= CFList();
    prepareLeadingCoeffs (leadingCoeffs2, A, Aeval, A.level(),leadingCoeffs,
                          biFactors, evaluation);

    if (!fdivides (LC (oldA,1),prod (leadingCoeffs2[lengthAeval2-1])))
    {
      LCheuristic= false;
      A= bufA;
      biFactors= bufBiFactors;
      leadingCoeffs2[lengthAeval2-1]= bufLeadingCoeffs2;
      LCmultiplier= bufLCmultiplier;
    }
  }
  TIMING_END_AND_PRINT (fac_fq_lcheuristic, "time for Lc heuristic over Fq: ");

tryAgainWithoutHeu:
  TIMING_START (fac_fq_shift_to_zero);
  A= shift2Zero (A, Aeval, evaluation);

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
  TIMING_END_AND_PRINT (fac_fq_shift_to_zero,
                        "time to shift evaluation point to zero: ");

  CFArray Pi;
  CFList diophant;
  int* liftBounds= new int [A.level() - 1];
  int liftBoundsLength= A.level() - 1;
  for (int i= 0; i < liftBoundsLength; i++)
    liftBounds [i]= degree (A, i + 2) + 1;

  Aeval.removeFirst();
  bool noOneToOne= false;
  TIMING_START (fac_fq_hensel_lift);
  factors= nonMonicHenselLift (Aeval, biFactors, leadingCoeffs2, diophant,
                               Pi, liftBounds, liftBoundsLength, noOneToOne);
  TIMING_END_AND_PRINT (fac_fq_hensel_lift,
                        "time for non monic hensel lifting over Fq: ");

  if (!noOneToOne)
  {
    int check= factors.length();
    A= oldA;
    TIMING_START (fac_fq_recover_factors);
    factors= recoverFactors (A, factors, evaluation);
    TIMING_END_AND_PRINT (fac_fq_recover_factors,
                          "time to recover factors over Fq: ");
    if (check != factors.length())
      noOneToOne= true;
    else
      factors= Union (factors, bufFactors);

    if (extension && !noOneToOne)
      factors= extNonMonicFactorRecombination (factors, A, info);
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
      lift= tmax (lift,
                  degree (i.getItem(), 2) + degree (LC (i.getItem(), 1)) + 1);

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
    CFList earlyFactors, liftedFactors;
    TIMING_START (fac_fq_hensel_lift);
    liftedFactors= henselLiftAndEarly
                   (A, MOD, liftBounds, earlySuccess, earlyFactors,
                    Aeval, biFactors, evaluation, info);
    TIMING_END_AND_PRINT (fac_fq_hensel_lift,
                          "time for hensel lifting over Fq: ");

    if (!extension)
    {
      TIMING_START (fac_fq_factor_recombination);
      factors= factorRecombination (A, liftedFactors, MOD);
      TIMING_END_AND_PRINT (fac_fq_factor_recombination,
                            "time for factor recombination: ");
    }
    else
    {
      TIMING_START (fac_fq_factor_recombination);
      factors= extFactorRecombination (liftedFactors, A, MOD, info, evaluation);
      TIMING_END_AND_PRINT (fac_fq_factor_recombination,
                            "time for factor recombination: ");
    }

    if (earlySuccess)
      factors= Union (factors, earlyFactors);
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
  }

  if (v.level() != 1)
  {
    for (CFListIterator iter= factors; iter.hasItem(); iter++)
      iter.getItem()= swapvar (iter.getItem(), v, y);
  }

  swap (factors, swapLevel, swapLevel2, x);
  append (factors, contentAFactors);
  decompress (factors, N);
  if (!extension)
    normalize (factors);

  delete[] liftBounds;

  return factors;
}
#endif

/// multivariate factorization over an extension of the initial field
#ifdef HAVE_NTL // multiFactorize
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
    if (p < 7)
    {
      if (p == 2)
        setCharacteristic (getCharacteristic(), 6, 'Z');
      else if (p == 3)
        setCharacteristic (getCharacteristic(), 4, 'Z');
      else if (p == 5)
        setCharacteristic (getCharacteristic(), 3, 'Z');
      ExtensionInfo info= ExtensionInfo (extension);
      A= A.mapinto();
      factors= multiFactorize (A, info);

      CanonicalForm mipo= gf_mipo;
      setCharacteristic (getCharacteristic());
      Variable vBuf= rootOf (mipo.mapinto());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
      prune (vBuf);
    }
    else if (p >= 7 && p*p < (1<<16)) // pass to GF if possible
    {
      setCharacteristic (getCharacteristic(), 2, 'Z');
      ExtensionInfo info= ExtensionInfo (extension);
      A= A.mapinto();
      factors= multiFactorize (A, info);

      CanonicalForm mipo= gf_mipo;
      setCharacteristic (getCharacteristic());
      Variable vBuf= rootOf (mipo.mapinto());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
      prune (vBuf);
    }
    else  // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (2, w);
      Variable v= rootOf (mipo);
      ExtensionInfo info= ExtensionInfo (v);
      factors= multiFactorize (A, info);
      prune (v);
    }
    return factors;
  }
  else if (!GF && (alpha != w)) // we are in F_p(\alpha)
  {
    if (k == 1) // need factorization over F_p
    {
      int extDeg= degree (getMipo (alpha));
      extDeg++;
      CanonicalForm mipo= randomIrredpoly (extDeg, w);
      Variable v= rootOf (mipo);
      ExtensionInfo info= ExtensionInfo (v);
      factors= multiFactorize (A, info);
      prune (v);
    }
    else
    {
      if (beta == w)
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
          imPrimElem= mapPrimElem (primElem, alpha, v);

        CFList source, dest;
        CanonicalForm bufA= mapUp (A, alpha, v, primElem, imPrimElem,
                                   source, dest);
        ExtensionInfo info= ExtensionInfo (v, alpha, imPrimElem, primElem);
        factors= multiFactorize (bufA, info);
        prune (v);
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
        factors= multiFactorize (bufA, info);
        prune (v);
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
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable vBuf= rootOf (mipo.mapinto());
        A= GF2FalphaRep (A, vBuf);
        setCharacteristic (p, extensionDeg, 'Z');
        ExtensionInfo info= ExtensionInfo (extension);
        factors= multiFactorize (A.mapinto(), info);
        prune (vBuf);
      }
      else // not able to pass to another GF, pass to F_p(\alpha)
      {
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable vBuf= rootOf (mipo.mapinto());
        A= GF2FalphaRep (A, vBuf);
        Variable v= chooseExtension (vBuf, beta, k);
        ExtensionInfo info= ExtensionInfo (v, extension);
        factors= multiFactorize (A, info);
        prune (vBuf);
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
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable v1= rootOf (mipo.mapinto());
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
        prune (v1);
      }
    }
    return factors;
  }
}
#endif
#endif
