/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqBivar.cc
 *
 * This file provides functions for factorizing a bivariate polynomial over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF, based on "Modern Computer
 * Algebra, Chapter 15" by J. von zur Gathen & J. Gerhard and "Factoring
 * multivariate polynomials over a finite field" by L. Bernardin.
 *
 * ABSTRACT: In contrast to biFactorizer() in facFqFactorice.cc we evaluate and
 * factorize the polynomial in both variables. So far factor recombination is
 * done naive!
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

#include "canonicalform.h"
#include "cf_defs.h"
#include "cf_map_ext.h"
#include "cf_random.h"
#include "facHensel.h"
#include "cf_map.h"
#include "cf_gcd_smallp.h"
#include "facFqBivar.h"


#ifdef HAVE_NTL
#include "NTLconvert.h"

TIMING_DEFINE_PRINT(fac_uni_factorizer);
TIMING_DEFINE_PRINT(fac_hensel_lift);
TIMING_DEFINE_PRINT(fac_factor_recombination);

CanonicalForm prodMod0 (const CFList& L, const CanonicalForm& M)
{
  if (L.isEmpty())
    return 1;
  else if (L.length() == 1)
    return mod (L.getFirst()(0, 1) , M);
  else if (L.length() == 2)
    return mod (L.getFirst()(0, 1)*L.getLast()(0, 1), M);
  else
  {
    int l= L.length()/2;
    CFListIterator i= L;
    CFList tmp1, tmp2;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod0 (tmp1, M);
    buf2= prodMod0 (tmp2, M);
    return mod (buf1*buf2, M);
  }
}

CanonicalForm evalPoint (const CanonicalForm& F, CanonicalForm & eval,
                         const Variable& alpha, CFList& list, const bool& GF,
                         bool& fail)
{
  fail= false;
  Variable x= Variable(2);
  FFRandom genFF;
  GFRandom genGF;
  CanonicalForm random, mipo;
  double bound;
  int p= getCharacteristic ();
  if (alpha.level() != 1)
  {
    mipo= getMipo (alpha);
    int d= degree (mipo);
    bound= ipower (p, d);
  }
  else if (GF)
  {
    int d= getGFDegree();
    bound= ipower (p, d);
  }
  else
    bound= p;

  do
  {
    if (list.length() >= bound)
    {
      fail= true;
      break;
    }
    if (list.isEmpty())
      random= 0;
    else if (GF)
      random= genGF.generate();
    else if (list.length() < p || alpha == x)
      random= genFF.generate();
    else if (alpha != x && list.length() >= p)
    {
      AlgExtRandomF genAlgExt (alpha);
      random= genAlgExt.generate();
    }
    if (find (list, random)) continue;
    eval= F (random, x);
    if (degree (eval) != degree (F, Variable (1)))
    { //leading coeff vanishes
      if (!find (list, random))
        list.append (random);
      continue;
    }
    if (degree (gcd (deriv (eval, eval.mvar()), eval), eval.mvar()) > 0)
    { //evaluated polynomial is not squarefree
      if (!find (list, random))
        list.append (random);
      continue;
    }
  } while (find (list, random));

  return random;
}

CFList
uniFactorizer (const CanonicalForm& A, const Variable& alpha, const bool& GF)
{
  Variable x= A.mvar();
  ASSERT (A.isUnivariate(), "univariate polynomial expected");
  CFFList factorsA;
  ZZ p= to_ZZ (getCharacteristic());
  ZZ_p::init (p);
  if (GF)
  {
    Variable beta= rootOf (gf_mipo);
    int k= getGFDegree();
    char cGFName= gf_name;
    setCharacteristic (getCharacteristic());
    CanonicalForm buf= GF2FalphaRep (A, beta);
    if (getCharacteristic() > 2)
    {
      ZZ_pX NTLMipo= convertFacCF2NTLZZpX (gf_mipo);
      ZZ_pE::init (NTLMipo);
      ZZ_pEX NTLA= convertFacCF2NTLZZ_pEX (buf, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_ZZ_pEX_long NTLFactorsA= CanZass (NTLA);
      ZZ_pE multi= to_ZZ_pE (1);
      factorsA= convertNTLvec_pair_ZZpEX_long2FacCFFList (NTLFactorsA, multi,
                                                         x, beta);
    }
    else
    {
      GF2X NTLMipo= convertFacCF2NTLGF2X (gf_mipo);
      GF2E::init (NTLMipo);
      GF2EX NTLA= convertFacCF2NTLGF2EX (buf, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_GF2EX_long NTLFactorsA= CanZass (NTLA);
      GF2E multi= to_GF2E (1);
      factorsA= convertNTLvec_pair_GF2EX_long2FacCFFList (NTLFactorsA, multi,
                                                           x, beta);
    }
    setCharacteristic (getCharacteristic(), k, cGFName);
    for (CFFListIterator i= factorsA; i.hasItem(); i++)
    {
      buf= i.getItem().factor();
      buf= Falpha2GFRep (buf);
      i.getItem()= CFFactor (buf, i.getItem().exp());
    }
  }
  else if (alpha.level() != 1)
  {
    if (getCharacteristic() > 2)
    {
      ZZ_pX NTLMipo= convertFacCF2NTLZZpX (getMipo (alpha));
      ZZ_pE::init (NTLMipo);
      ZZ_pEX NTLA= convertFacCF2NTLZZ_pEX (A, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_ZZ_pEX_long NTLFactorsA= CanZass (NTLA);
      ZZ_pE multi= to_ZZ_pE (1);
      factorsA= convertNTLvec_pair_ZZpEX_long2FacCFFList (NTLFactorsA, multi,
                                                           x, alpha);
    }
    else
    {
      GF2X NTLMipo= convertFacCF2NTLGF2X (getMipo (alpha));
      GF2E::init (NTLMipo);
      GF2EX NTLA= convertFacCF2NTLGF2EX (A, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_GF2EX_long NTLFactorsA= CanZass (NTLA);
      GF2E multi= to_GF2E (1);
      factorsA= convertNTLvec_pair_GF2EX_long2FacCFFList (NTLFactorsA, multi,
                                                           x, alpha);
    }
  }
  else
  {
    if (getCharacteristic() > 2)
    {
      ZZ_pX NTLA= convertFacCF2NTLZZpX (A);
      MakeMonic (NTLA);
      vec_pair_ZZ_pX_long NTLFactorsA= CanZass (NTLA);
      ZZ_p multi= to_ZZ_p (1);
      factorsA= convertNTLvec_pair_ZZpX_long2FacCFFList (NTLFactorsA, multi,
                                                          x);
    }
    else
    {
      GF2X NTLA= convertFacCF2NTLGF2X (A);
      vec_pair_GF2X_long NTLFactorsA= CanZass (NTLA);
      GF2 multi= to_GF2 (1);
      factorsA= convertNTLvec_pair_GF2X_long2FacCFFList (NTLFactorsA, multi,
                                                          x);
    }
  }
  CFList uniFactors;
  for (CFFListIterator i= factorsA; i.hasItem(); i++)
    uniFactors.append (i.getItem().factor());
  return uniFactors;
}

/// naive factor recombination as decribed in "Factoring
/// multivariate polynomials over a finite field" by L Bernardin.
CFList
extFactorRecombination (const CFList& factors, const CanonicalForm& F,
                        const CanonicalForm& M, const ExtensionInfo& info,
                        const DegreePattern& degs, const CanonicalForm& eval)
{
  if (factors.length() == 0)
    return CFList();

  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();

  Variable y= F.mvar();
  CFList source, dest;
  if (degs.getLength() <= 1 || factors.length() == 1)
    return CFList(mapDown (F(y-eval, y), info, source, dest));

  DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, M) == F " <<
            (LC (F, 1)*prodMod (factors, M) == F));
  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList T, S, Diff;
  T= factors;

  int s= 1;
  CFList result;
  CanonicalForm buf, buf2;
  if (beta != Variable (1))
    buf= mapDown (F, gamma, delta, alpha, source, dest);
  else
    buf= F;

  CanonicalForm g, LCBuf= LC (buf, Variable (1));
  int v [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;

  CFArray TT;
  DegreePattern bufDegs1, bufDegs2;
  bufDegs1= degs;
  int subsetDeg;
  TT= copy (factors);
  bool nosubset= false;
  bool recombination= false;
  bool trueFactor= false;
  while (T.length() >= 2*s)
  {
    while (nosubset == false)
    {
      if (T.length() == s)
      {
        if (recombination)
        {
          T.insert (LCBuf);
          g= prodMod (T, M);
          T.removeFirst();
          g /= content(g);
          g= g (y - eval, y);
          g /= Lc (g);
          appendTestMapDown (result, g, info, source, dest);
          return result;
        }
        else
        {
          appendMapDown (result, F (y - eval, y), info, source, dest);
          return result;
        }
      }
      S= subset (v, s, TT, nosubset);
      if (nosubset) break;
      subsetDeg= subsetDegree (S);
      // skip those combinations that are not possible
      if (!degs.find (subsetDeg))
        continue;
      else
      {
        g= prodMod0 (S, M);
        g= mod (g*LCBuf, M);
        g /= content (g);
        if (fdivides (LC (g), LCBuf))
        {
          S.insert (LCBuf);
          g= prodMod (S, M);
          S.removeFirst();
          g /= content (g, Variable (1));
          if (fdivides (g, buf))
          {
            buf2= g (y - eval, y);
            buf2 /= Lc (buf2);

            if (!k && beta == Variable (1))
            {
              if (degree (buf2, alpha) < degMipoBeta)
              {
                buf /= g;
                LCBuf= LC (buf, Variable (1));
                recombination= true;
                appendTestMapDown (result, buf2, info, source, dest);
                trueFactor= true;
              }
            }
            else
            {
              if (!isInExtension (buf2, delta, k))
              {
                buf /= g;
                LCBuf= LC (buf, Variable (1));
                recombination= true;
                appendTestMapDown (result, buf2, info, source, dest);
                trueFactor= true;
              }
            }
            if (trueFactor)
            {
              T= Difference (T, S);
              // compute new possible degree pattern
              bufDegs2= DegreePattern (T);
              bufDegs1.intersect (bufDegs2);
              bufDegs1.refine ();
              if (T.length() < 2*s || T.length() == s ||
                  bufDegs1.getLength() == 1)
              {
                if (recombination)
                {
                  appendTestMapDown (result, buf (y - eval, y), info, source,
                                     dest);
                  return result;
                }
                else
                {
                  appendMapDown (result, F (y - eval, y), info, source, dest);
                  return result;
                }
              }
              trueFactor= false;
              TT= copy (T);
              indexUpdate (v, s, T.length(), nosubset);
              if (nosubset) break;
            }
          }
        }
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      if (recombination)
      {
        appendTestMapDown (result, buf (y - eval, y), info, source, dest);
        return result;
      }
      else
      {
        appendMapDown (result, F (y - eval, y), info, source, dest);
        return result;
      }
    }
    int v [T.length()];
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }
  if (T.length() < 2*s)
    appendMapDown (result, F (y - eval, y), info, source, dest);

  return result;
}

/// naive factor recombination as decribed in "Factoring
/// multivariate polynomials over a finite field" by L Bernardin.
CFList
factorRecombination (const CFList& factors, const CanonicalForm& F,
                     const CanonicalForm& M, const DegreePattern& degs)
{
  if (factors.length() == 0)
    return CFList ();
  if (degs.getLength() <= 1 || factors.length() == 1)
    return CFList(F);
  DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, M) == F " <<
            (LC (F, 1)*prodMod (factors, M) == F));
  CFList T, S;

  T= factors;
  int s= 1;
  CFList result;
  CanonicalForm LCBuf= LC (F, Variable (1));
  CanonicalForm g, buf= F;
  int v [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool nosubset= false;
  CFArray TT;
  DegreePattern bufDegs1, bufDegs2;
  bufDegs1= degs;
  int subsetDeg;
  TT= copy (factors);
  bool recombination= false;
  while (T.length() >= 2*s)
  {
    while (nosubset == false)
    {
      if (T.length() == s)
      {
        if (recombination)
        {
          T.insert (LCBuf);
          g= prodMod (T, M);
          T.removeFirst();
          result.append (g/content (g, Variable (1)));
          return result;
        }
        else
          return CFList (F);
      }
      S= subset (v, s, TT, nosubset);
      if (nosubset) break;
      subsetDeg= subsetDegree (S);
      // skip those combinations that are not possible
      if (!degs.find (subsetDeg))
        continue;
      else
      {
        g= prodMod0 (S, M);
        g= mod (g*LCBuf, M);
        g /= content (g);
        if (fdivides (LC(g), LCBuf))
        {
          S.insert (LCBuf);
          g= prodMod (S, M);
          S.removeFirst();
          g /= content (g, Variable (1));
          if (fdivides (g, buf))
          {
            recombination= true;
            result.append (g);
            buf /= g;
            LCBuf= LC (buf, Variable(1));
            T= Difference (T, S);

            // compute new possible degree pattern
            bufDegs2= DegreePattern (T);
            bufDegs1.intersect (bufDegs2);
            bufDegs1.refine ();
            if (T.length() < 2*s || T.length() == s ||
                bufDegs1.getLength() == 1)
            {
              if (recombination)
              {
                result.append (buf);
                return result;
              }
              else
                return CFList (F);
            }
            TT= copy (T);
            indexUpdate (v, s, T.length(), nosubset);
            if (nosubset) break;
          }
        }
      }
    }
    s++;
    if (T.length() < 2*s || T.length() == s)
    {
      if (recombination)
      {
        result.append (buf);
        return result;
      }
      else
        return CFList (F);
    }
    int v [T.length()];
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }
  if (T.length() < 2*s)
    result.append (F);

  return result;
}

Variable chooseExtension (const CanonicalForm & A, const Variable & alpha)
{
  int p= getCharacteristic();
  ZZ NTLp= to_ZZ (p);
  ZZ_p::init (NTLp);
  ZZ_pX NTLirredpoly;
  int d= degree (A);
  int m= 1;
  if (alpha != Variable (1))
    m= degree (getMipo (alpha));
  int i= (int) floor((double) d/(double) m) - 1;
  if (i < 2)
    i= 2;
  if (i > 8)
    i= i/4;
  BuildIrred (NTLirredpoly, i*m);
  Variable x= A.mvar();
  CanonicalForm newMipo= convertNTLZZpX2CF (NTLirredpoly, x);
  return rootOf (newMipo);
}

CFList
earlyFactorDetection (CanonicalForm& F, CFList& factors,int& adaptedLiftBound,
                      DegreePattern& degs, bool& success, int deg)
{
  DegreePattern bufDegs1= degs;
  DegreePattern bufDegs2;
  CFList result;
  CFList T= factors;
  CanonicalForm buf= F;
  CanonicalForm LCBuf= LC (buf, Variable (1));
  CanonicalForm g;
  CanonicalForm M= power (F.mvar(), deg);
  adaptedLiftBound= 0;
  int d;
  if (degree (LCBuf) == degree (F))
    d= degree (F);
  else
    d= degree (F) + degree (LCBuf);
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (!bufDegs1.find (degree (i.getItem(), 1)))
      continue;
    else
    {
      g= i.getItem() (0, 1);
      g *= LCBuf;
      g= mod (g, M);
      if (fdivides (LC (g), LCBuf))
      {
        g= mulMod2 (i.getItem(), LCBuf, M);
        g /= content (g, Variable (1));
        if (fdivides (g, buf))
        {
          result.append (g);
          buf /= g;
          d -= degree (g) + degree (LC (g, Variable (1)));
          LCBuf= LC (buf, Variable (1));
          T= Difference (T, CFList (i.getItem()));

          // compute new possible degree pattern
          bufDegs2= DegreePattern (T);
          bufDegs1.intersect (bufDegs2);
          bufDegs1.refine ();
          if (bufDegs1.getLength() <= 1)
          {
            result.append (buf);
            break;
          }
        }
      }
    }
  }
  adaptedLiftBound= d + 1;
  if (d < deg)
  {
    factors= T;
    degs= bufDegs1;
    F= buf;
    success= true;
  }
  if (bufDegs1.getLength() <= 1)
    degs= bufDegs1;
  return result;
}

CFList
extEarlyFactorDetection (CanonicalForm& F, CFList& factors,
                         int& adaptedLiftBound, DegreePattern& degs,
                         bool& success, const ExtensionInfo& info,
                         const CanonicalForm& eval, int deg)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  DegreePattern bufDegs1= degs, bufDegs2;
  CFList result;
  CFList T= factors;
  Variable y= F.mvar();
  CanonicalForm buf= F, LCBuf= LC (buf, Variable (1)), g, buf2;
  CanonicalForm M= power (y, deg);
  adaptedLiftBound= 0;
  bool trueFactor= false;
  int d;
  if (degree (F) == degree (LCBuf))
    d= degree (F);
  else
    d= degree (F) + degree (LCBuf);
  CFList source, dest;
  int degMipoBeta;
  if (!k && beta.level() == 1)
    degMipoBeta= 1;
  else if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));
  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (!bufDegs1.find (degree (i.getItem(), 1)))
      continue;
    else
    {
      g= i.getItem() (0, 1);
      g *= LCBuf;
      g= mod (g, M);
      if (fdivides (LC (g), LCBuf))
      {
        g= mulMod2 (i.getItem(), LCBuf, M);
        g /= content (g, Variable (1));
        if (fdivides (g, buf))
        {
          buf2= g (y - eval, y);
          buf2 /= Lc (buf2);

          if (!k && beta == Variable (1))
          {
            if (degree (buf2, alpha) < degMipoBeta)
            {
              appendTestMapDown (result, buf2, info, source, dest);
              buf /= g;
              d -= degree (g) + degree (LC (g, Variable (1)));
              LCBuf= LC (buf, Variable (1));
              trueFactor= true;
            }
          }
          else
          {
            if (!isInExtension (buf2, delta, k))
            {
              appendTestMapDown (result, buf2, info, source, dest);
              buf /= g;
              d -= degree (g) + degree (LC (g, Variable (1)));
              LCBuf= LC (buf, Variable (1));
              trueFactor= true;
            }
          }
          if (trueFactor)
          {
            T= Difference (T, CFList (i.getItem()));

            // compute new possible degree pattern
            bufDegs2= DegreePattern (T);
            bufDegs1.intersect (bufDegs2);
            bufDegs1.refine ();
            trueFactor= false;
            if (bufDegs1.getLength() <= 1)
            {
              buf= buf (y - eval, y);
              buf /= Lc (buf);
              appendMapDown (result, buf, info, source, dest);
              break;
            }
          }
        }
      }
    }
  }
  adaptedLiftBound= d + 1;
  if (adaptedLiftBound < deg)
  {
    success= true;
    factors= T;
    degs= bufDegs1;
    F= buf;
  }
  if (bufDegs1.getLength() <= 1)
    degs= bufDegs1;

  return result;
}

CFList
henselLiftAndEarly (CanonicalForm& A, bool& earlySuccess, CFList&
                    earlyFactors, DegreePattern& degs, int& liftBound,
                    const CFList& uniFactors, const ExtensionInfo& info,
                    const CanonicalForm& eval)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  bool extension= info.isInExtension();

  Variable x= Variable (1);
  Variable y= Variable (2);
  CFArray Pi;
  CFList diophant;
  CFList bufUniFactors= uniFactors;
  bufUniFactors.insert (LC (A, x));
  CFMatrix M= CFMatrix (liftBound, bufUniFactors.length() - 1);
  earlySuccess= false;
  int newLiftBound= 0;
  int smallFactorDeg= 11; //this is a tunable parameter
  if (smallFactorDeg >= liftBound)
    henselLift12 (A, bufUniFactors, liftBound, Pi, diophant, M);
  else if (smallFactorDeg >= degree (A, y) + 1)
  {
    henselLift12 (A, bufUniFactors, degree (A, y) + 1, Pi, diophant, M);
    if (!extension)
      earlyFactors= earlyFactorDetection (A, bufUniFactors, newLiftBound,
                     degs, earlySuccess, degree (A, y) + 1);
    else
      earlyFactors= extEarlyFactorDetection (A, bufUniFactors,
                     newLiftBound, degs, earlySuccess, info, eval,
                     degree (A, y) + 1);
    if (degs.getLength() > 1 && !earlySuccess)
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
    if (!extension)
      earlyFactors= earlyFactorDetection (A, bufUniFactors, newLiftBound,
                                           degs, earlySuccess,
                                           smallFactorDeg);
    else
      earlyFactors= extEarlyFactorDetection (A, bufUniFactors,
                     newLiftBound, degs, earlySuccess,
                     info, eval, smallFactorDeg);
    if (degs.getLength() > 1 && !earlySuccess)
    {
      bufUniFactors.insert (LC (A, x));
      henselLiftResume12 (A, bufUniFactors, smallFactorDeg, degree (A, y)
                          + 1, Pi, diophant, M);
      if (!extension)
        earlyFactors= earlyFactorDetection (A, bufUniFactors, newLiftBound,
                       degs, earlySuccess, degree (A, y) + 1);
      else
        earlyFactors= extEarlyFactorDetection (A, bufUniFactors,
                       newLiftBound, degs, earlySuccess,
                       info, eval, degree(A,y) + 1);
      if (degs.getLength() > 1 && !earlySuccess)
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
  return bufUniFactors;
}

CFList
extBiFactorize (const CanonicalForm& F, const ExtensionInfo& info);

/// bivariate factorization over finite fields as decribed in "Factoring
/// multivariate polynomials over a finite field" by L Bernardin.
CFList
biFactorize (const CanonicalForm& F, const ExtensionInfo& info)
{
  if (F.inCoeffDomain())
    return CFList(F);

  CanonicalForm A= F;
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);

  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();
  bool extension= info.isInExtension();
  if (A.isUnivariate())
  {
    if (extension == false)
      return uniFactorizer (F, alpha, GF);
    else
    {
      CFList source, dest;
      A= mapDown (A, info, source, dest);
      return uniFactorizer (A, beta, GF);
    }
  }

  CFMap N;
  A= compress (A, N);
  Variable y= A.mvar();

  if (y.level() > 2) return CFList (F);
  Variable x= Variable (1);

  //remove and factorize content
  CanonicalForm contentAx= content (A, x);
  CanonicalForm contentAy= content (A);

  A= A/(contentAx*contentAy);
  CFList contentAxFactors, contentAyFactors;

  if (!extension)
  {
    contentAxFactors= uniFactorizer (contentAx, alpha, GF);
    contentAyFactors= uniFactorizer (contentAy, alpha, GF);
  }

  //trivial case
  CFList factors;
  if (A.inCoeffDomain())
  {
    append (factors, contentAxFactors);
    append (factors, contentAyFactors);
    decompress (factors, N);
    return factors;
  }
  else if (A.isUnivariate())
  {
    factors= uniFactorizer (A, alpha, GF);
    append (factors, contentAxFactors);
    append (factors, contentAyFactors);
    decompress (factors, N);
    return factors;
  }

  // check derivatives
  bool derivXZero= false;
  CanonicalForm derivX= deriv (A, x);
  CanonicalForm gcdDerivX;
  if (derivX.isZero())
    derivXZero= true;
  else
  {
    gcdDerivX= gcd (A, derivX);
    if (degree (gcdDerivX) > 0)
    {
      CanonicalForm g= A/gcdDerivX;
      CFList factorsG=
      Union (biFactorize (g, info), biFactorize (gcdDerivX, info));
      append (factorsG, contentAxFactors);
      append (factorsG, contentAyFactors);
      decompress (factorsG, N);
      normalize (factors);
      return factorsG;
    }
  }
  bool derivYZero= false;
  CanonicalForm derivY= deriv (A, y);
  CanonicalForm gcdDerivY;
  if (derivY.isZero())
    derivYZero= true;
  else
  {
    gcdDerivY= gcd (A, derivY);
    if (degree (gcdDerivY) > 0)
    {
      CanonicalForm g= A/gcdDerivY;
      CFList factorsG=
      Union (biFactorize (g, info), biFactorize (gcdDerivY, info));
      append (factorsG, contentAxFactors);
      append (factorsG, contentAyFactors);
      decompress (factorsG, N);
      normalize (factors);
      return factorsG;
    }
  }
  //main variable is chosen s.t. the degree in x is minimal
  bool swap= false;
  if ((degree (A) > degree (A, x)) || derivXZero)
  {
    if (!derivYZero)
    {
      A= swapvar (A, y, x);
      swap= derivXZero;
      derivXZero= derivYZero;
      derivYZero= swap;
      swap= true;
    }
  }

  bool fail;
  CanonicalForm Aeval, evaluation, bufAeval, bufEvaluation, buf;
  CFList uniFactors, list, bufUniFactors;
  DegreePattern degs;
  DegreePattern bufDegs;

  bool fail2= false;
  CanonicalForm Aeval2, evaluation2, bufAeval2, bufEvaluation2;
  CFList bufUniFactors2, list2, uniFactors2;
  DegreePattern degs2;
  DegreePattern bufDegs2;
  bool swap2= false;

  // several univariate factorizations to obtain more information about the
  // degree pattern therefore usually less combinations have to be tried during
  // the recombination process
  int factorNums= 5;
  double logarithm= (double) ilog2 (totaldegree (A));
  logarithm /= log2exp;
  logarithm= ceil (logarithm);
  if (factorNums < (int) logarithm)
    factorNums= (int) logarithm;
  for (int i= 0; i < factorNums; i++)
  {
    bufAeval= A;
    bufEvaluation= evalPoint (A, bufAeval, alpha, list, GF, fail);
    if (!derivXZero && !fail2)
    {
      buf= swapvar (A, x, y);
      bufAeval2= buf;
      bufEvaluation2= evalPoint (buf, bufAeval2, alpha, list2, GF, fail2);
    }
    // first try to change main variable if there is no valid evaluation point
    if (fail && (i == 0))
    {
      if (!derivXZero)
        bufEvaluation= evalPoint (buf, bufAeval, alpha, list, GF, fail);
      else
        fail= true;

      if (!fail)
      {
        A= buf;
        swap2= true;
      }
    }

    // if there is no valid evaluation point pass to a field extension
    if (fail && (i == 0))
    {
      factors= extBiFactorize (A, info);
      appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                            swap, swap2, N);
      normalize (factors);
      return factors;
    }

    // there is at least one valid evaluation point
    // but we do not compute more univariate factorization over an extension
    if (fail && (i != 0))
      break;

    // univariate factorization
    TIMING_START (fac_uni_factorizer);
    bufUniFactors= uniFactorizer (bufAeval, alpha, GF);
    TIMING_END_AND_PRINT (fac_uni_factorizer,
                          "time for univariate factorization: ");
    DEBOUTLN (cerr, "Lc (bufAeval)*prod (bufUniFactors)== bufAeval " <<
              prod (bufUniFactors)*Lc (bufAeval) == bufAeval);

    if (!derivXZero && !fail2)
    {
      TIMING_START (fac_uni_factorizer);
      bufUniFactors2= uniFactorizer (bufAeval2, alpha, GF);
      TIMING_END_AND_PRINT (fac_uni_factorizer,
                            "time for univariate factorization in y: ");
      DEBOUTLN (cerr, "Lc (Aeval2)*prod (uniFactors2)== Aeval2 " <<
                prod (bufUniFactors2)*Lc (bufAeval2) == bufAeval2);
    }

    if (bufUniFactors.length() == 1 ||
        (!fail2 && !derivXZero && (bufUniFactors2.length() == 1)))
    {
      if (extension)
      {
        CFList source, dest;
        ExtensionInfo info2= ExtensionInfo (beta, alpha, delta, gamma, k,
                             info.getGFName(), info.isInExtension());
        appendMapDown (factors, A, info2, source, dest);
      }
      else
        factors.append (A);

      appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                            swap, swap2, N);

      normalize (factors);
      return factors;
    }

    if (i == 0)
    {
      int subCheck= substituteCheck (bufUniFactors);

      if (subCheck > 1)
      {
        CanonicalForm bufA= A;
        subst (bufA, bufA, subCheck, x);
        factors= biFactorize (bufA, info);
        reverseSubst (factors, subCheck, x);
        appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                              swap, swap2, N);
        normalize (factors);
        return factors;
      }

      if (!derivXZero && !fail2)
      {
        subCheck= substituteCheck (bufUniFactors2);
        if (subCheck > 1)
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, y);
          factors= biFactorize (bufA, info);
          reverseSubst (factors, subCheck, x);
          appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                                swap, swap2, N);
          normalize (factors);
          return factors;
        }
      }
    }

    // degree analysis
    bufDegs = DegreePattern (bufUniFactors);
    if (!derivXZero && !fail2)
      bufDegs2= DegreePattern (bufUniFactors2);

    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      uniFactors= bufUniFactors;
      degs= bufDegs;
      if (!derivXZero && !fail2)
      {
        Aeval2= bufAeval2;
        evaluation2= bufEvaluation2;
        uniFactors2= bufUniFactors2;
        degs2= bufDegs2;
      }
    }
    else
    {
      degs.intersect (bufDegs);
      if (!derivXZero && !fail2)
      {
        degs2.intersect (bufDegs2);
        if (bufUniFactors2.length() < uniFactors2.length())
        {
          uniFactors2= bufUniFactors2;
          Aeval2= bufAeval2;
          evaluation2= bufEvaluation2;
        }
      }
      if (bufUniFactors.length() < uniFactors.length())
      {
        uniFactors= bufUniFactors;
        Aeval= bufAeval;
        evaluation= bufEvaluation;
      }
    }
    list.append (bufEvaluation);
    if (!derivXZero && !fail2)
      list2.append (bufEvaluation2);
  }

  if (!derivXZero && !fail2)
  {
    if (uniFactors.length() > uniFactors2.length() ||
        (uniFactors.length() == uniFactors2.length()
         && degs.getLength() > degs2.getLength()))
    {
      degs= degs2;
      uniFactors= uniFactors2;
      evaluation= evaluation2;
      Aeval= Aeval2;
      A= buf;
      swap2= true;
    }
  }

  if (degs.getLength() == 1) // A is irreducible
  {
    if (extension)
    {
      CFList source, dest;
      ExtensionInfo info2= ExtensionInfo (beta, alpha, delta, gamma, k,
                           info.getGFName(), info.isInExtension());
      appendMapDown (factors, A, info2, source, dest);
    }
    else
      factors.append (A);
    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                            swap, swap2, N);
    normalize (factors);
    return factors;
  }

  A= A (y + evaluation, y);

  int liftBound;
  if (degree (A, y) == degree (LC (A, x)))
    liftBound= degree (LC (A, x)) + 1;
  else
    liftBound= degree (A, y) + 1 + degree (LC(A, x));

  DEBOUTLN (cerr, "uniFactors= " << uniFactors);
  bool earlySuccess= false;
  CFList earlyFactors;
  TIMING_START (fac_hensel_lift);
  uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
  TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");
  DEBOUTLN (cerr, "lifted factors= " << uniFactors);

  CanonicalForm MODl= power (y, liftBound);
  TIMING_START (fac_factor_recombination);
  if (!extension)
    factors= factorRecombination (uniFactors, A, MODl, degs);
  else
    factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                     evaluation);
  TIMING_END_AND_PRINT (fac_factor_recombination,
                        "time for factor recombination: ");
  if (earlySuccess)
    factors= Union (earlyFactors, factors);
  else if (!earlySuccess && degs.getLength() == 1)
    factors= earlyFactors;

  if (!extension)
  {
    for (CFListIterator i= factors; i.hasItem(); i++)
      i.getItem()= i.getItem() (y - evaluation, y);
  }

  appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                        swap, swap2, N);
  normalize (factors);

  return factors;
}

CFList
extBiFactorize (const CanonicalForm& F, const ExtensionInfo& info)
{

  CanonicalForm A= F;
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  int k= info.getGFDegree();
  char cGFName= info.getGFName();
  CanonicalForm delta= info.getDelta();

  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  Variable x= Variable (1);
  CFList factors;
  if (!GF && alpha == x)  // we are in F_p
  {
    bool extension= true;
    int p= getCharacteristic();
    if (p*p < (1<<16)) // pass to GF if possible
    {
      setCharacteristic (getCharacteristic(), 2, 'Z');
      A= A.mapinto();
      ExtensionInfo info= ExtensionInfo (extension);
      factors= biFactorize (A, info);

      Variable vBuf= rootOf (gf_mipo);
      setCharacteristic (getCharacteristic());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
    }
    else // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (3, Variable (1));
      Variable v= rootOf (mipo);
      ExtensionInfo info= ExtensionInfo (v);
      factors= biFactorize (A, info);
    }
    return factors;
  }
  else if (!GF && (alpha != x)) // we are in F_p(\alpha)
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
      if (ipower (p, extensionDeg) < (1<<16))
      // pass to GF(p^k+1)
      {
        setCharacteristic (p);
        Variable vBuf= rootOf (gf_mipo);
        A= GF2FalphaRep (A, vBuf);
        setCharacteristic (p, extensionDeg, 'Z');
        ExtensionInfo info= ExtensionInfo (extension);
        factors= biFactorize (A.mapinto(), info);
      }
      else // not able to pass to another GF, pass to F_p(\alpha)
      {
        setCharacteristic (p);
        Variable vBuf= rootOf (gf_mipo);
        A= GF2FalphaRep (A, vBuf);
        Variable v= chooseExtension (A, beta);
        ExtensionInfo info= ExtensionInfo (v, extension);
        factors= biFactorize (A, info);
      }
    }
    else // need factorization over GF (p^k)
    {
      if (ipower (p, 2*extensionDeg) < (1<<16))
      // pass to GF (p^2k)
      {
        setCharacteristic (p, 2*extensionDeg, 'Z');
        ExtensionInfo info= ExtensionInfo (k, cGFName, extension);
        factors= biFactorize (GFMapUp (A, extensionDeg), info);
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
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (primElem, vBuf, v2);

        CFList source, dest;
        CanonicalForm bufA= mapUp (A, v1, v2, primElem, imPrimElem,
                                     source, dest);
        ExtensionInfo info= ExtensionInfo (v2, v1, imPrimElem, primElem);
        factors= biFactorize (bufA, info);
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


