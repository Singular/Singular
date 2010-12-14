/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqFactorize.cc
 *
 * This file implements functions for factoring a multivariate polynomial over
 * a finite field.
 *
 * ABSTRACT: "Efficient Multivariate Factorization over Finite Fields" by
 * L. Bernardin & M. Monagon.
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>

#include "assert.h"
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
  bool algExt= hasFirstAlgVar (G, alpha);
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

static inline
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
monicFactorRecombi (const CFList& factors,const CanonicalForm& F, const
                    CanonicalForm& M, const DegreePattern& degs)
{
  if (degs.getLength() == 1)
    return CFList(F);

  CFList T, S;

  T= factors;

  int s= 1;
  CFList result;
  CanonicalForm buf= F;
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g, gg;
  int v [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  int subsetDeg;
  DegreePattern bufDegs1= degs, bufDegs2;
  TT= copy (factors);
  while (T.length() >= 2*s)
  {
    while (noSubset == false)
    {
      if (T.length() == s)
      {
        result.append (prodMod (T, M));
        return result;
      }
      S= subset (v, s, TT, noSubset);
      if (noSubset) break;
      subsetDeg= subsetDegree (S);
      if (!degs.find (subsetDeg))
        continue;
      else
      {
        g= prodMod0 (S, M);
        gg= mod (g*LCBuf, M); //univariate polys
        gg /= content (gg);
        if (fdivides (LC (gg), LCBuf))
        {
          g= prodMod (S, M);
          gg= mulMod2 (LCBuf, g, M);
          gg /= content (gg, Variable (1));
          if (fdivides (gg, buf))
          {
            result.append (g);
            buf /= gg;
            LCBuf= LC (buf, Variable(1));
            T= Difference (T, S);
            // compute new possible degree pattern
            bufDegs2= DegreePattern (T);
            bufDegs1.intersect (bufDegs2);
            bufDegs1.refine ();
            if (T.length() < 2*s || T.length() == s ||
                bufDegs1.getLength() == 1)
            {
              result.append (prodMod (T, M));
              return result;
            }
            TT= copy (T);
            indexUpdate (v, s, T.length(), noSubset);
            if (noSubset) break;
          }
        }
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      result.append (prodMod (T, M));
      return result;
    }
    int v [T.length()];
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
    result.append (prodMod (T, M));

  return result;
}

CFList
earlyMonicFactorDetect (CanonicalForm& F, CFList& factors,
                        int& adaptedLiftBound, DegreePattern& degs,
                        bool& success, int deg, const int bound)
{
  DegreePattern bufDegs1= degs;
  DegreePattern bufDegs2;
  CFList result;
  CFList T= factors;
  CanonicalForm buf= F;
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g, gg;
  CanonicalForm M= power (F.mvar(), deg);
  int d= bound;
  int e= 0;
  adaptedLiftBound= 0;
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (!bufDegs1.find (degree (i.getItem(), 1)))
      continue;
    else
    {
      gg= i.getItem() (0, 1);
      gg *= LCBuf;
      gg= mod (gg, M);
      if (fdivides (LC (gg), LCBuf))
      {
        g= i.getItem();
        gg= mulMod2 (g, LCBuf, M);
        gg /= content (gg, Variable (1));
        if (fdivides (gg, buf))
        {
          result.append (g);
          CanonicalForm bufbuf= buf;
          buf /= gg;
          d -= degree (gg) + degree (LC (gg, 1));
          e= tmax (e, degree (gg) + degree (LC (gg, 1)));
          LCBuf= LC (buf, Variable (1));
          T= Difference (T, CFList (i.getItem()));

          // compute new possible degree pattern
          bufDegs2= DegreePattern (T);
          bufDegs1.intersect (bufDegs2);
          bufDegs1.refine ();
          if (bufDegs1.getLength() <= 1)
          {
            result.append (prodMod (T, M));
            break;
          }
        }
      }
    }
  }
  adaptedLiftBound= d;

  if (adaptedLiftBound < deg)
  {
    factors= T;
    degs= bufDegs1;
    if (adaptedLiftBound < degree (F) + 1)
    {
      if (d == 1)
      {
        if (e + 1 > deg)
        {
          success= false;
          adaptedLiftBound= deg;
        }
        else
        {
          F= buf;
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
        F= buf;
        adaptedLiftBound= deg;
      }
    }
    else
    {
      F= buf;
      success= true;
    }
  }
  if (bufDegs1.getLength() <= 1)
  {
    degs= bufDegs1;
    if (!success)
      adaptedLiftBound= deg;
  }

  return result;
}

CFList biFactorizer (const CanonicalForm& F, const Variable& alpha,
                     CanonicalForm& bivarEval, int& liftBound)
{
  CanonicalForm A= F;
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);

  if (A.isUnivariate())
    return uniFactorizer (F, alpha, GF);


  CFMap N;
  A= compress (A, N);
  Variable y= A.mvar();
  A /= Lc(A);

  if (y.level() > 2) return CFList (F);
  Variable x= Variable (1);

  //remove content and factorize content
  CanonicalForm contentAy= content (A, y);
  CanonicalForm contentAx= content (A, x);
  A= A/(contentAy*contentAx);
  CFList contentAyFactors= uniFactorizer (contentAy, alpha, GF);

  //trivial case
  CFList factors;
  if (A.inCoeffDomain())
  {
    append (factors, contentAyFactors);
    decompress (factors, N);
    bivarEval= N (y - bivarEval);
    return factors;
  }
  else if (A.isUnivariate())
  {
    if (A.mvar() == x)
      factors= uniFactorizer (A, alpha, GF);
    append (factors, contentAyFactors);
    decompress (factors, N);
    bivarEval= N (y - bivarEval);
    return factors;
  }
  bool fail;
  CanonicalForm Aeval, evaluation, bufAeval, bufEvaluation, buf;
  CFList uniFactors, list, bufUniFactors;
  DegreePattern degs;
  DegreePattern bufDegs;

  int factorNums= 5;
  double logarithm= (double) ilog2 (totaldegree (A));
  logarithm /= log2exp;
  logarithm= ceil (logarithm);
  if (factorNums < (int) logarithm)
    factorNums= (int) logarithm;
  for (int i= 0; i < factorNums; i++)
  {
    if (i == 0)
      Aeval= A (bivarEval, y);
    else if (i > 0 && contentAx.inCoeffDomain())
    {
      Aeval= A;
      evaluation= evalPoint (A, Aeval, alpha, list, GF, fail);
    }

    if (fail && (i != 0))
      break;

    // univariate factorization
    uniFactors= uniFactorizer (Aeval, alpha, GF);

    if (uniFactors.length() == 1)
    {
      append (factors, contentAyFactors);
      if (contentAyFactors.isEmpty())
        factors.append (F/lc(F));
      else
      {
        A= A (y - bivarEval, y);
        A= A/lc (A);
        if (!LC(A, 1).isOne())
        {
          CanonicalForm s,t;
          (void) extgcd (LC (A, 1), power (y, liftBound), s, t);
          A *= s;
          A= mod (A, power (y, liftBound));
        }
        factors.append (A);
      }
      decompress (factors, N);
      bivarEval= N (bivarEval);
      return factors;
    }

    // degree analysis
    degs= DegreePattern (uniFactors);

    if (i == 0)
    {
      bufAeval= Aeval;
      bufEvaluation= bivarEval;
      bufUniFactors= uniFactors;
      bufDegs= degs;
      if (!contentAx.inCoeffDomain())
        break;
    }
    else
    {
      bufDegs.intersect (degs);
      if (uniFactors.length() < bufUniFactors.length())
      {
        bufUniFactors= uniFactors;
        bufAeval= Aeval;
        bufEvaluation= evaluation;
      }
    }

    if (bufDegs.getLength() == 1)
    {
      append (factors, contentAyFactors);
      if (contentAyFactors.isEmpty())
        factors.append (F/lc(F));
      else
      {
        A= A (y - bivarEval, y);
        A= A/lc (A);
        if (!LC(A, 1).isOne())
        {
          CanonicalForm s,t;
          (void) extgcd (LC (A, 1), power (y, liftBound), s, t);
          A *= s;
          A= mod (A, power (y, liftBound));
        }
        factors.append (A);
      }
      decompress (factors, N);
      bivarEval= N (bivarEval);
      return factors;
    }
    list.append (evaluation);
  }

  bivarEval= y - bufEvaluation;
  A= A (y + bufEvaluation, y);
  bufUniFactors.insert (LC (A, x));

  // Hensel lifting
  CFList diophant;
  CFMatrix M= CFMatrix (liftBound, bufUniFactors.length() - 1);
  CFArray Pi;
  bool earlySuccess= false;
  int newLiftBound= 0;
  CFList earlyFactors;
  int smallFactorDeg= 11; //tunable parameter
  if (smallFactorDeg >= liftBound)
    henselLift12 (A, bufUniFactors, liftBound, Pi, diophant, M);
  else if (smallFactorDeg >= degree (A, y) + 1)
  {
    henselLift12 (A, bufUniFactors, degree (A, y) + 1, Pi, diophant, M);
    earlyFactors= earlyMonicFactorDetect (A, bufUniFactors, newLiftBound,
                                           bufDegs, earlySuccess,
                                           degree (A, y) + 1, liftBound);
    if (bufDegs.getLength() > 1 && !earlySuccess)
    {
      if (newLiftBound > degree (A, y) + 1)
      {
        liftBound= newLiftBound;
        bufUniFactors.insert (LC(A, x));
        henselLiftResume12 (A, bufUniFactors, degree (A, y) + 1, liftBound,
                            Pi, diophant, M);
      }
    }
    else if (earlySuccess)
      liftBound= newLiftBound;
  }
  else if (smallFactorDeg < degree (A, y) + 1)
  {
    henselLift12 (A, bufUniFactors, smallFactorDeg, Pi, diophant, M);
    earlyFactors= earlyMonicFactorDetect (A, bufUniFactors, newLiftBound,
                                           bufDegs, earlySuccess,
                                           smallFactorDeg, liftBound);
    if (bufDegs.getLength() > 1 && !earlySuccess)
    {
      bufUniFactors.insert (LC (A, x));
      henselLiftResume12 (A, bufUniFactors, smallFactorDeg, degree (A, y) +
                          1, Pi, diophant, M);
      earlyFactors= earlyMonicFactorDetect (A, bufUniFactors, newLiftBound,
                                             bufDegs, earlySuccess,
                                             degree (A, y) + 1, liftBound);
      if (bufDegs.getLength() > 1 && !earlySuccess)
      {
        if (newLiftBound > degree (A, y) + 1)
        {
          if (newLiftBound < newLiftBound)
            liftBound= newLiftBound;
          bufUniFactors.insert (LC(A, x));
          henselLiftResume12 (A, bufUniFactors, degree (A, y) + 1, liftBound,
                              Pi, diophant, M);
        }
      }
      else if (earlySuccess)
        liftBound= newLiftBound;
    }
    else if (earlySuccess)
      liftBound= newLiftBound;
  }

  if (newLiftBound > 0)
    liftBound= newLiftBound;

  CanonicalForm MODl= power (y, liftBound);

  if (bufDegs.getLength() > 1)
    factors= monicFactorRecombi (bufUniFactors, A, MODl, bufDegs);

  if (earlySuccess)
    factors= Union (earlyFactors, factors);
  else if (!earlySuccess && bufDegs.getLength() == 1)
    factors= earlyFactors;

  decompressAppend (factors, contentAyFactors, N);

  bivarEval= N (bivarEval);

  return factors;
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

  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList T, S;
  T= factors;

  int s= 1;
  CFList result;
  CanonicalForm buf;
  if (beta != Variable (1))
    buf= mapDown (F, gamma, delta, alpha, source, dest);
  else
    buf= F;

  CanonicalForm g, LCBuf= LC (buf, Variable (1));
  CanonicalForm buf2;
  int v [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  int subsetDeg;
  TT= copy (factors);
  bool recombination= false;
  while (T.length() >= 2*s)
  {
    while (noSubset == false)
    {
      if (T.length() == s)
      {
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
        return result;
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
          }
        }
        else
        {
          if (!isInExtension (buf2, delta, k))
          {
            appendTestMapDown (result, buf2, info, source, dest);
            buf /= g;
            LCBuf= LC (buf, Variable (1));
            recombination= true;
          }
        }
        T= Difference (T, S);

        if (T.length() < 2*s || T.length() == s)
        {
          buf= reverseShift (buf, evaluation);
          buf /= Lc (buf);
          appendTestMapDown (result, buf, info, source, dest);
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
      buf= reverseShift (buf, evaluation);
      appendTestMapDown (result, buf, info, source, dest);
      return result;
    }
    int v [T.length()];
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
  {
    buf= reverseShift (F, evaluation);
    appendMapDown (result, buf, info, source, dest);
  }

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
  int v [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool noSubset= false;
  CFArray TT;
  int subsetDeg;
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
      return result;
    }
    int v [T.length()];
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    noSubset= false;
  }
  if (T.length() < 2*s)
    result.append (F);

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
  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
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
          buf /= g;
          nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
          d -= nBuf;
          e= tmax (e, nBuf);
          LCBuf= LC (buf, Variable (1));
        }
      }
      else
      {
        if (!isInExtension (gg, delta, k))
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

  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
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
            }
          }
          else
          {
            if (!isInExtension (gg, delta, k))
            {
              appendTestMapDown (result, gg, info, source, dest);
              buf /= g;
              nBuf= degree (g, y) + degree (LC (g, Variable (1)), y);
              d -= nBuf;
              e= tmax (e, nBuf);
              LCBuf= LC (buf, Variable (1));
            }
          }
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
    for (CFListIterator i= result; i.hasItem(); i++, l--)
      eval.insert (eval.getFirst()(i.getItem(), l));

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
                           evaluation, const Variable& alpha, const int lev)
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

static inline
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
  int newLiftBound= 0;
  int adaptedLiftBound= 0;
  int liftBound= liftBounds[1];

  earlySuccess= false;
  bool earlyReconst= false;
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
  int k= info.getGFDegree();
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
        A= swapvar (A, x, z);
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
  // several bivariate factorizations
  for (int i= 0; i < factorNums; i++)
  {
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

      swapLevel2= newMainVariableSearch (A, Aeval, evaluation, alpha, level);

      if (!swapLevel2) // need to pass to an extension
      {
        factors= extFactorize (A, info);
        appendSwapDecompress (factors, contentAFactors, N, swapLevel, x);
        normalize (factors);
        return factors;
      }
      else
      {
        fail= false;
        bufAeval= Aeval;
        bufA= A;
        bufEvaluation= evaluation;
      }
    }
    else if (fail && (i > 0))
      break;

    bivarEval= bufEvaluation.getLast();
    bufEvaluation.removeLast();

    bufLift= degree (A, y) + 1 + degree (LC(A, x), y);

    TIMING_START (fac_bi_factorizer);
    bufBiFactors= biFactorizer (bufAeval.getFirst(), alpha, bivarEval, bufLift);
    TIMING_END_AND_PRINT (fac_bi_factorizer,
                          "time for bivariate factorization: ");

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
      return factors;
    }

    bufEvaluation.append (-bivarEval[0]);
    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      biFactors= bufBiFactors;
      lift= bufLift;
    }
    else
    {
      if (bufBiFactors.length() < biFactors.length() ||
          ((bufLift < lift) && (bufBiFactors.length() == biFactors.length())))
      {
        Aeval= bufAeval;
        evaluation= bufEvaluation;
        biFactors= bufBiFactors;
        lift= bufLift;
      }
    }
    int k= 0;
    for (CFListIterator j= bufEvaluation; j.hasItem(); j++, k++)
      evalPoly += j.getItem()*power (x, k);
    list.append (evalPoly);
  }

  //shifting to zero
  A= shift2Zero (A, Aeval, evaluation);

  int* liftBounds;
  int liftBoundsLength= F.level() - 1;
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
      Variable v= chooseExtension (A, beta);
      ExtensionInfo info= ExtensionInfo (v, extension);
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
        Variable v= chooseExtension (A, alpha);
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
        Variable v= chooseExtension (A, alpha);
        CanonicalForm primElem, imPrimElem;
        bool primFail= false;
        Variable vBuf;
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (delta, beta, v); //oder mapPrimElem (primElem, vBuf, v);

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
        Variable v= chooseExtension (A, beta);
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
        Variable v2= chooseExtension (A, v1);
        CanonicalForm primElem, imPrimElem;
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v1, vBuf, primFail);
        if (primFail)
        {
          ; //ERROR
        }
        else
        {
          imPrimElem= mapPrimElem (primElem, vBuf, v2);
        }
        CFList source, dest;
        CanonicalForm bufA= mapUp (A, alpha, beta, primElem, imPrimElem,
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

