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
#include "facFqBivarUtil.h"
#include "facFqBivar.h"
#include "cfNewtonPolygon.h"

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

  random= 0;
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
    {
      if (list.length() == 1)
        random= getGFGenerator();
      else
        random= genGF.generate();
    }
    else if (list.length() < p || alpha.level() == 1)
      random= genFF.generate();
    else if (alpha != x && list.length() >= p)
    {
      if (list.length() == p)
        random= alpha;
      else
      {
        AlgExtRandomF genAlgExt (alpha);
        random= genAlgExt.generate();
      }
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
extFactorRecombination (CFList& factors, CanonicalForm& F,
                        const CanonicalForm& M, const ExtensionInfo& info,
                        DegreePattern& degs, const CanonicalForm& eval, int s,
                        int thres)
{
  if (factors.length() == 0)
  {
    F= 1;
    return CFList();
  }

  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  int k= info.getGFDegree();

  Variable y= F.mvar();
  CFList source, dest;
  if (degs.getLength() <= 1 || factors.length() == 1)
  {
    CFList result= CFList(mapDown (F(y-eval, y), info, source, dest));
    F= 1;
    return result;
  }

  DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, M) == F " <<
            (LC (F, 1)*prodMod (factors, M) == F));
  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList T, S, Diff;
  T= factors;

  CFList result;
  CanonicalForm buf, buf2;

  buf= F;

  CanonicalForm g, LCBuf= LC (buf, Variable (1));
  int * v= new int [T.length()];
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
  while (T.length() >= 2*s && s <= thres)
  {
    while (nosubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombination)
        {
          T.insert (LCBuf);
          g= prodMod (T, M);
          T.removeFirst();
          g /= content(g);
          g= g (y - eval, y);
          g /= Lc (g);
          appendTestMapDown (result, g, info, source, dest);
          F= 1;
          return result;
        }
        else
        {
          appendMapDown (result, F (y - eval, y), info, source, dest);
          F= 1;
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

            if (!k && beta.level() == 1)
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
              if (!isInExtension (buf2, gamma, k, delta, source, dest))
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
                delete [] v;
                if (recombination)
                {
                  appendTestMapDown (result, buf (y - eval, y), info, source,
                                     dest);
                  F= 1;
                  return result;
                }
                else
                {
                  appendMapDown (result, F (y - eval, y), info, source, dest);
                  F= 1;
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
      delete [] v;
      if (recombination)
      {
        appendTestMapDown (result, buf (y - eval, y), info, source, dest);
        F= 1;
        return result;
      }
      else
      {
        appendMapDown (result, F (y - eval, y), info, source, dest);
        F= 1;
        return result;
      }
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }
  if (T.length() < 2*s)
  {
    appendMapDown (result, F (y - eval, y), info, source, dest);
    F= 1;
    delete [] v;
    return result;
  }

  if (s > thres)
  {
    factors= T;
    F= buf;
    degs= bufDegs1;
  }

  delete [] v;
  return result;
}

/// naive factor recombination as decribed in "Factoring
/// multivariate polynomials over a finite field" by L Bernardin.
CFList
factorRecombination (CFList& factors, CanonicalForm& F,
                     const CanonicalForm& M, DegreePattern& degs, int s,
                     int thres
                    )
{
  if (factors.length() == 0)
  {
    F= 1;
    return CFList ();
  }
  if (degs.getLength() <= 1 || factors.length() == 1)
  {
    CFList result= CFList (F);
    F= 1;
    return result;
  }
  DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, M) == F " <<
            (LC (F, 1)*prodMod (factors, M) == F));
  CFList T, S;

  T= factors;
  CFList result;
  CanonicalForm LCBuf= LC (F, Variable (1));
  CanonicalForm g, buf= F;
  int * v= new int [T.length()];
  for (int i= 0; i < T.length(); i++)
    v[i]= 0;
  bool nosubset= false;
  CFArray TT;
  DegreePattern bufDegs1, bufDegs2;
  bufDegs1= degs;
  int subsetDeg;
  TT= copy (factors);
  bool recombination= false;
  while (T.length() >= 2*s && s <= thres)
  {
    while (nosubset == false)
    {
      if (T.length() == s)
      {
        delete [] v;
        if (recombination)
        {
          T.insert (LCBuf);
          g= prodMod (T, M);
          T.removeFirst();
          result.append (g/content (g, Variable (1)));
          F= 1;
          return result;
        }
        else
        {
          result= CFList (F);
          F= 1;
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
              delete [] v;
              if (recombination)
              {
                result.append (buf);
                F= 1;
                return result;
              }
              else
              {
                result= CFList (F);
                F= 1;
                return result;
              }
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
      delete [] v;
      if (recombination)
      {
        result.append (buf);
        F= 1;
        return result;
      }
      else
      {
        result= CFList (F);
        F= 1;
        return result;
      }
    }
    for (int i= 0; i < T.length(); i++)
      v[i]= 0;
    nosubset= false;
  }
  delete [] v;
  if (T.length() < 2*s)
  {
    result.append (F);
    F= 1;
    return result;
  }

  if (s > thres)
  {
    factors= T;
    F= buf;
    degs= bufDegs1;
  }

  return result;
}

Variable chooseExtension (const Variable & alpha, const Variable& beta, int k)
{
  zz_p::init (getCharacteristic());
  zz_pX NTLIrredpoly;
  int i, m;
  // extension of F_p needed
  if (alpha.level() == 1 && beta.level() == 1 && k == 1)
  {
    i= 1;
    m= 2;
  } //extension of F_p(alpha) needed but want to factorize over F_p
  else if (alpha.level() != 1 && beta.level() == 1 && k == 1)
  {
    i= 1;
    m= degree (getMipo (alpha)) + 1;
  } //extension of F_p(alpha) needed for first time
  else if (alpha.level() != 1 && beta.level() == 1 && k != 1)
  {
    i= 2;
    m= degree (getMipo (alpha));
  }
  else if (alpha.level() != 1 && beta.level() != 1 && k != 1)
  {
    m= degree (getMipo (beta));
    i= degree (getMipo (alpha))/m + 1;
  }
  BuildIrred (NTLIrredpoly, i*m);
  CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
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
  int d= degree (F) + degree (LCBuf);
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
  int d= degree (F) + degree (LCBuf);
  CFList source, dest;
  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
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
            if (!isInExtension (buf2, gamma, k, delta, source, dest))
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

long isReduced (const mat_zz_p& M)
{
  long i, j, nonZero;
  for (i = 1; i <= M.NumRows(); i++)
  {
    nonZero= 0;
    for (j = 1; j <= M.NumCols(); j++)
    {
      if (!IsZero (M (i,j)))
        nonZero++;
    }
    if (nonZero != 1)
      return 0;
  }
  return 1;
}

long isReduced (const mat_zz_pE& M)
{
  long i, j, nonZero;
  for (i = 1; i <= M.NumRows(); i++)
  {
    nonZero= 0;
    for (j = 1; j <= M.NumCols(); j++)
    {
      if (!IsZero (M (i,j)))
        nonZero++;
    }
    if (nonZero != 1)
      return 0;
  }
  return 1;
}

int * extractZeroOneVecs (const mat_zz_p& M)
{
  long i, j;
  bool nonZeroOne= false;
  int * result= new int [M.NumCols()];
  for (i = 1; i <= M.NumCols(); i++)
  {
    for (j = 1; j <= M.NumRows(); j++)
    {
      if (!(IsOne (M (j,i)) || IsZero (M (j,i))))
      {
        nonZeroOne= true;
        break;
      }
    }
    if (!nonZeroOne)
      result [i - 1]= 1;
    else
      result [i - 1]= 0;
    nonZeroOne= false;
  }
  return result;
}

int * extractZeroOneVecs (const mat_zz_pE& M)
{
  long i, j;
  bool nonZeroOne= false;
  int * result= new int [M.NumCols()];
  for (i = 1; i <= M.NumCols(); i++)
  {
    for (j = 1; j <= M.NumRows(); j++)
    {
      if (!(IsOne (M (j,i)) || IsZero (M (j,i))))
      {
        nonZeroOne= true;
        break;
      }
    }
    if (!nonZeroOne)
      result [i - 1]= 1;
    else
      result [i - 1]= 0;
    nonZeroOne= false;
  }
  return result;
}

void
reconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const CFList&
                   factors, const int liftBound, int& factorsFound, int*&
                   factorsFoundIndex, mat_zz_pE& N, bool beenInThres
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm yToL= power (y, liftBound);
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf;
    if (beenInThres)
    {
      int count= 1;
      while (count < i)
      {
        count++;
        iter++;
      }
      buf= iter.getItem();
    }
    else
    {
      buf= 1;
      for (long j= 1; j <= N.NumRows(); j++, iter++)
      {
        if (!IsZero (N (j,i)))
          buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      F /= buf;
      F /= Lc (F);
      reconstructedFactors.append (buf);
    }
    if (degree (F) <= 0)
      return;
    if (factorsFound + 1 == N.NumCols())
    {
      reconstructedFactors.append (F);
      return;
    }
  }
}

void
reconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const CFList&
                   factors, const int liftBound, int& factorsFound, int*&
                   factorsFoundIndex, mat_zz_p& N, bool beenInThres
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm yToL= power (y, liftBound);
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf;
    if (beenInThres)
    {
      int count= 1;
      while (count < i)
      {
        count++;
        iter++;
      }
      buf= iter.getItem();
    }
    else
    {
      buf= 1;
      for (long j= 1; j <= N.NumRows(); j++, iter++)
      {
        if (!IsZero (N (j,i)))
          buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      F /= buf;
      F /= Lc (F);
      reconstructedFactors.append (buf);
    }
    if (degree (F) <= 0)
      return;
    if (factorsFound + 1 == N.NumCols())
    {
      reconstructedFactors.append (F);
      return;
    }
  }
}

CFList
reconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs, int
                precision, const mat_zz_pE& N
               )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm F= G;
  CanonicalForm yToL= power (y, precision);
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    factorsConsidered= CFList();
    for (long j= 1; j <= N.NumRows(); j++, iter++)
    {
      if (!IsZero (N (j,i)))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F))
    {
      F /= buf;
      F /= Lc (F);
      result.append (buf);
      bufFactors= Difference (bufFactors, factorsConsidered);
    }
    if (degree (F) <= 0)
    {
      G= F;
      factors= bufFactors;
      return result;
    }
  }
  G= F;
  factors= bufFactors;
  return result;
}

CFList
monicReconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs,
                     int precision, const mat_zz_pE& N
                    )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm F= G;
  CanonicalForm yToL= power (y, precision);
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    factorsConsidered= CFList();
    for (long j= 1; j <= N.NumRows(); j++, iter++)
    {
      if (!IsZero (N (j,i)))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    CanonicalForm buf2= buf;
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F))
    {
      F /= buf;
      F /= Lc (F);
      result.append (buf2);
      bufFactors= Difference (bufFactors, factorsConsidered);
    }
    if (degree (F) <= 0)
    {
      G= F;
      factors= bufFactors;
      return result;
    }
  }
  G= F;
  factors= bufFactors;
  return result;
}

CFList
extReconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs, int
                   precision, const mat_zz_p& N, const ExtensionInfo& info,
                   const CanonicalForm& evaluation
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  int k= info.getGFDegree();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  CanonicalForm F= G;
  CanonicalForm yToL= power (y, precision);
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  CanonicalForm buf2;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    factorsConsidered= CFList();
    for (long j= 1; j <= N.NumRows(); j++, iter++)
    {
      if (!IsZero (N (j,i)))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    buf2= buf (y-evaluation, y);
    if (!k && beta == Variable (1))
    {
      if (degree (buf2, alpha) < 1)
      {
        if (fdivides (buf, F))
        {
          F /= buf;
          F /= Lc (F);
          result.append (buf2);
          bufFactors= Difference (bufFactors, factorsConsidered);
        }
      }
    }
    else
    {
      CFList source, dest;

      if (!isInExtension (buf2, gamma, k, delta, source, dest))
      {
        if (fdivides (buf, F))
        {
          F /= buf;
          F /= Lc (F);
          result.append (buf2);
          bufFactors= Difference (bufFactors, factorsConsidered);
        }
      }
    }
    if (degree (F) <= 0)
    {
      G= F;
      factors= bufFactors;
      return result;
    }
  }
  G= F;
  factors= bufFactors;
  return result;
}

CFList
reconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs,
                int precision, const mat_zz_p& N)
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm F= G;
  CanonicalForm yToL= power (y, precision);
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    factorsConsidered= CFList();
    for (long j= 1; j <= N.NumRows(); j++, iter++)
    {
      if (!IsZero (N (j,i)))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F))
    {
      F /= buf;
      F /= Lc (F);
      result.append (buf);
      bufFactors= Difference (bufFactors, factorsConsidered);
    }
    if (degree (F) <= 0)
    {
      G= F;
      factors= bufFactors;
      return result;
    }
  }
  G= F;
  factors= bufFactors;
  return result;
}

void
extReconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const
                      CFList& factors, const int liftBound, int& factorsFound,
                      int*& factorsFoundIndex, mat_zz_p& N, bool beenInThres,
                      const ExtensionInfo& info, const CanonicalForm& evaluation
                     )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  int k= info.getGFDegree();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  CanonicalForm yToL= power (y, liftBound);
  CFList source, dest;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    CFListIterator iter= factors;
    CanonicalForm buf;
    CanonicalForm buf2;
    if (beenInThres)
    {
      int count= 1;
      while (count < i)
      {
        count++;
        iter++;
      }
      buf= iter.getItem();
    }
    else
    {
      buf= 1;
      for (long j= 1; j <= N.NumRows(); j++, iter++)
      {
        if (!IsZero (N (j,i)))
          buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    buf2= buf (y - evaluation, y);
    if (!k && beta == Variable (1))
    {
      if (degree (buf2, alpha) < 1)
      {
        if (fdivides (buf, F))
        {
          factorsFoundIndex[i - 1]= 1;
          factorsFound++;
          F /= buf;
          F /= Lc (F);
          buf2= mapDown (buf2, info, source, dest);
          reconstructedFactors.append (buf2);
        }
      }
    }
    else
    {
      CFList source, dest;
      if (!isInExtension (buf2, gamma, k, delta, source, dest))
      {
        if (fdivides (buf, F))
        {
          factorsFoundIndex[i - 1]= 1;
          factorsFound++;
          F /= buf;
          F /= Lc (F);
          buf2= mapDown (buf2, info, source, dest);
          reconstructedFactors.append (buf2);
        }
      }
    }
    if (degree (F) <= 0)
      return;
    if (factorsFound + 1 == N.NumCols())
    {
      CanonicalForm tmp= F (y - evaluation, y);
      tmp= mapDown (tmp, info, source, dest);
      reconstructedFactors.append (tmp);
      return;
    }
  }
}

//over Fp
int
liftAndComputeLattice (const CanonicalForm& F, int* bounds, int sizeBounds, int
                       start, int liftBound, int minBound, CFList& factors,
                       mat_zz_p& NTLN, CFList& diophant, CFMatrix& M, CFArray&
                       Pi, CFArray& bufQ, bool& irreducible
                      )
{
  CanonicalForm LCF= LC (F, 1);
  CFArray *A= new CFArray [factors.length() - 1];
  bool wasInBounds= false;
  bool hitBound= false;
  int l= (minBound+1)*2;
  int stepSize= 2;
  int oldL= l/2;
  bool reduced= false;
  while (l <= liftBound)
  {

    if (start)
    {
      henselLiftResume12 (F, factors, start, l, Pi, diophant, M);
      start= 0;
    }
    else
    {
      if (wasInBounds)
        henselLiftResume12 (F, factors, oldL, l, Pi, diophant, M);
      else
        henselLift12 (F, factors, l, Pi, diophant, M);
    }

    factors.insert (LCF);
    CFListIterator j= factors;
    j++;

    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ[i]);
      }
      else
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i], bufQ[i]);
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN))
        {
          reduced= true;
          break;
        }
      }
    }

    if (irreducible)
      break;
    if (reduced)
      break;
    oldL= l;
    l += stepSize;
    stepSize *= 2;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return l;
}

//over field extension
int
extLiftAndComputeLattice (const CanonicalForm& F, int* bounds, int sizeBounds,
                          int liftBound, int minBound, int start, CFList&
                          factors, mat_zz_p& NTLN, CFList& diophant,
                          CFMatrix& M, CFArray& Pi, CFArray& bufQ, bool&
                          irreducible, const CanonicalForm& evaluation, const
                          ExtensionInfo& info, CFList& source, CFList& dest
                         )
{
  bool GF= (CFFactory::gettype()==GaloisFieldDomain);
  CanonicalForm LCF= LC (F, 1);
  CFArray *A= new CFArray [factors.length() - 1];
  bool wasInBounds= false;
  bool hitBound= false;
  int degMipo;
  Variable alpha;
  alpha= info.getAlpha();
  degMipo= degree (getMipo (alpha));

  Variable gamma= info.getBeta();
  CanonicalForm primElemAlpha= info.getGamma();
  CanonicalForm imPrimElemAlpha= info.getDelta();

  int stepSize= 2;
  int l= ((minBound+1)/degMipo+1)*2;
  l= tmax (l, 2);
  if (start > l)
    l= start;
  int oldL= l/2;
  bool reduced= false;
  while (l <= liftBound)
  {
    if (start)
    {
      henselLiftResume12 (F, factors, start, l, Pi, diophant, M);
      start= 0;
    }
    else
    {
      if (wasInBounds)
        henselLiftResume12 (F, factors, oldL, l, Pi, diophant, M);
      else
        henselLift12 (F, factors, l, Pi, diophant, M);
    }

    Variable y= F.mvar();
    Variable x= Variable (1);

    factors.insert (LCF);

    if (GF)
      setCharacteristic (getCharacteristic());

    CanonicalForm powX= power (y-gamma, l);
    CFMatrix Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {

      CanonicalForm imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      CFIterator iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    mat_zz_p* NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);

    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    CFListIterator j= factors;
    j++;

    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i], bufQ[i]);
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= (l/2)*degMipo)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, (l/2)*degMipo);
        CFMatrix C= CFMatrix (l*degMipo - k, factors.length() - 1);

        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
          {
            if (GF)
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              setCharacteristic (getCharacteristic());
              A[ii] [i]= GF2FalphaRep (A[ii] [i], alpha);
              if (alpha != gamma)
                A [ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                     gamma, source, dest
                                    );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
          }
          writeInMatrix (C, buf, ii + 1, 0);
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN))
        {
          reduced= true;
          break;
        }
      }
    }

    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }
    if (reduced)
      break;
    oldL= l;
    l += stepSize;
    stepSize *= 2;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return l;
}

// over Fq
int
liftAndComputeLattice (const CanonicalForm& F, int* bounds, int sizeBounds,
                       int start, int liftBound, int minBound, CFList& factors,
                       mat_zz_pE& NTLN, CFList& diophant, CFMatrix& M, CFArray&
                       Pi, CFArray& bufQ, bool& irreducible
                      )
{
  CanonicalForm LCF= LC (F, 1);
  CFArray *A= new CFArray [factors.length() - 1];
  bool wasInBounds= false;
  bool hitBound= false;
  int l= (minBound+1)*2;
  int stepSize= 2;
  int oldL= l/2;
  bool reduced= false;
  while (l <= liftBound)
  {
    if (start)
    {
      henselLiftResume12 (F, factors, start, l, Pi, diophant, M);
      start= 0;
    }
    else
    {
      if (wasInBounds)
        henselLiftResume12 (F, factors, oldL, l, Pi, diophant, M);
      else
        henselLift12 (F, factors, l, Pi, diophant, M);
    }

    factors.insert (LCF);
    CFListIterator j= factors;
    j++;

    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (l == (minBound+1)*2)
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ[i]);
      }
      else
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]
                                    );
      }
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }

        mat_zz_pE* NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        mat_zz_pE NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN))
        {
          reduced= true;
          break;
        }
      }
    }

    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }
    if (reduced)
      break;
    oldL= l;
    l += stepSize;
    stepSize *= 2;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return l;
}

int
liftAndComputeLatticeFq2Fp (const CanonicalForm& F, int* bounds, int sizeBounds,
                            int start, int liftBound, int minBound, CFList&
                            factors, mat_zz_p& NTLN, CFList& diophant, CFMatrix&
                            M, CFArray& Pi, CFArray& bufQ, bool& irreducible,
                            const Variable& alpha
                           )
{
  CanonicalForm LCF= LC (F, 1);
  CFArray *A= new CFArray [factors.length() - 1];
  bool wasInBounds= false;
  int l= (minBound+1)*2;
  int oldL= l/2;
  int stepSize= 2;
  bool hitBound= false;
  int extensionDeg= degree (getMipo (alpha));
  bool reduced= false;
  while (l <= liftBound)
  {
    if (start)
    {
      henselLiftResume12 (F, factors, start, l, Pi, diophant, M);
      start= 0;
    }
    else
    {
      if (wasInBounds)
        henselLiftResume12 (F, factors, oldL, l, Pi, diophant, M);
      else
        henselLift12 (F, factors, l, Pi, diophant, M);
    }

    factors.insert (LCF);
    CFListIterator j= factors;
    j++;

    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (l == (minBound+1)*2)
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ[i]);
      }
      else
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]
                                    );
      }
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix ((l - k)*extensionDeg, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k, alpha);
          writeInMatrix (C, buf, ii + 1, 0);
        }

        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN))
        {
          reduced= true;
          break;
        }
      }
    }

    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }
    if (reduced)
      break;
    oldL= l;
    l += stepSize;
    stepSize *= 2;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return l;
}

CFList
increasePrecision (CanonicalForm& F, CFList& factors, int factorsFound,
                   int oldNumCols, int oldL, int precision
                  )
{
  bool irreducible= false;
  int d;
  int* bounds= computeBounds (F, d);
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }
  int l= tmax (2*(minBound + 1), oldL);
  int oldL2= l/2;
  int stepSize= 2;
  bool useOldQs= false;
  bool hitBound= false;
  while (l <= precision)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          delete [] bounds;
          CanonicalForm G= F;
          F= 1;
          return CFList (G);
        }
      }
    }

    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, NTLN, false
                          );
        F= bufF;
        if (result.length() > 0)
        {
          delete [] factorsFoundIndex;
          delete [] A;
          delete [] bounds;
          return result;
        }
        delete [] factorsFoundIndex;
      }
      else if (l == precision)
      {
        CanonicalForm bufF= F;
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN);
        F= bufF;
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return result;
      }
    }
    oldL2= l;
    l += stepSize;
    stepSize *= 2;
    if (l > precision)
    {
      if (!hitBound)
      {
        l= precision;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] bounds;
  delete [] A;
  return CFList();
}

CFList
increasePrecision (CanonicalForm& F, CFList& factors, int factorsFound,
                   int oldNumCols, int oldL, const Variable& alpha,
                   int precision
                  )
{
  bool irreducible= false;
  int d;
  int* bounds= computeBounds (F, d);
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  mat_zz_pE NTLN;
  ident (NTLN, factors.length());
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }
  int l= tmax (2*(minBound + 1), oldL);
  int oldL2= l/2;
  int stepSize= 2;
  bool useOldQs= false;
  bool hitBound= false;
  while (l <= precision)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_pE* NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        mat_zz_pE NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          delete [] bounds;
          return CFList (F);
        }
      }
    }

    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, NTLN, false);
        if (result.length() == NTLN.NumCols())
        {
          delete [] factorsFoundIndex;
          delete [] A;
          delete [] bounds;
          return result;
        }
        delete [] factorsFoundIndex;
      }
      else if (l == precision)
      {
        CanonicalForm bufF= F;
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN);
        F= bufF;
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return result;
      }
    }
    oldL2= l;
    l += stepSize;
    stepSize *= 2;
    if (l > precision)
    {
      if (!hitBound)
      {
        l= precision;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] bounds;
  delete [] A;
  return CFList();
}

//over field extension
CFList
extIncreasePrecision (CanonicalForm& F, CFList& factors, int factorsFound,
                      int oldNumCols, int oldL, const CanonicalForm& evaluation,
                      const ExtensionInfo& info, CFList& source, CFList& dest,
                      int precision
                     )
{
  bool GF= (CFFactory::gettype()==GaloisFieldDomain);
  int degMipo= degree (getMipo (info.getAlpha()));
  Variable alpha= info.getAlpha();
  bool irreducible= false;
  int d;
  int* bounds= computeBounds (F, d);

  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  zz_p::init (getCharacteristic());
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }
  int l= tmax (oldL, 2*((minBound+1)/degMipo+1));
  int oldL2= l/2;
  int stepSize= 2;
  bool useOldQs= false;
  bool hitBound= false;
  Variable gamma= info.getBeta();
  CanonicalForm primElemAlpha= info.getGamma();
  CanonicalForm imPrimElemAlpha= info.getDelta();
  while (l <= precision)
  {
    CFListIterator j= factors;
    if (GF)
      setCharacteristic (getCharacteristic());
    Variable y= F.mvar();
    CanonicalForm powX= power (y-gamma, l);
    CFMatrix Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {

      CanonicalForm imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      CFIterator iter= imBasis;
      for (; iter.hasTerms(); iter++)
          Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    mat_zz_p* NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= (l/2)*degMipo)
      {
        int k= tmin (bounds [i] + 1, (l/2)*degMipo);
        CFMatrix C= CFMatrix (l*degMipo - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
          {
            if (GF)
            {
              A[ii] [i]= A [ii] [i] (y-evaluation, y);
              setCharacteristic (getCharacteristic());
              A[ii] [i]= GF2FalphaRep (A[ii] [i], alpha);
              if (alpha != gamma)
                A [ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                     gamma, source, dest
                                    );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
          }
          writeInMatrix (C, buf, ii + 1, 0);
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          Variable y= Variable (2);
          CanonicalForm tmp= F (y - evaluation, y);
          CFList source, dest;
          tmp= mapDown (tmp, info, source, dest);
          delete [] A;
          delete [] bounds;
          return CFList (tmp);
        }
      }
    }

    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
      int * factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      int factorsFound2= 0;
      CFList result;
      CanonicalForm bufF= F;
      extReconstructionTry (result, bufF, factors, degree (F)+1, factorsFound2,
                            factorsFoundIndex, NTLN, false, info, evaluation
                           );
      if (result.length() == NTLN.NumCols())
      {
        delete [] factorsFoundIndex;
        delete [] A;
        delete [] bounds;
        return result;
      }
      delete [] factorsFoundIndex;
      }
      else if (l == precision)
      {
        CanonicalForm bufF= F;
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= extReconstruction (bufF, factors, zeroOne, precision,
                                          NTLN, info, evaluation
                                         );
        F= bufF;
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return result;
      }
    }
    oldL2= l;
    l += stepSize;
    stepSize *= 2;
    if (l > precision)
    {
      if (!hitBound)
      {
        hitBound= true;
        l= precision;
      }
      else
        break;
    }
  }
  delete [] bounds;
  delete [] A;
  return CFList();
}

CFList
increasePrecision2 (const CanonicalForm& F, CFList& factors,
                    const Variable& alpha, int precision)
{
  bool irreducible= false;
  int d;
  int* bounds= computeBounds (F, d);
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  zz_p::init (getCharacteristic());
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  zz_pE::init (NTLMipo);
  mat_zz_pE NTLN;
  ident (NTLN, factors.length());
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }
  int l= tmin (2*(minBound + 1), precision);
  int oldL= l/2;
  int stepSize= 2;
  bool useOldQs= false;
  bool hitBound= false;
  Variable y= Variable (2);
  while (l <= precision)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i], bufQ[i]);
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
      {
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
      }
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_pE* NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        mat_zz_pE NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          delete [] bounds;
          return CFList (F);
        }
      }
    }

    if (isReduced (NTLN) || l == precision)
    {
      CanonicalForm bufF= F;
      int * zeroOne= extractZeroOneVecs (NTLN);
      CFList bufFactors= factors;
      CFList result= monicReconstruction (bufF, factors, zeroOne, precision,
                                          NTLN
                                         );
      if (result.length() != NTLN.NumCols() && l != precision)
        factors= bufFactors;
      if (result.length() == NTLN.NumCols())
      {
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return result;
      }
      if (l == precision)
      {
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return Union (result, factors);
      }
    }
    oldL= l;
    l += stepSize;
    stepSize *= 2;
    if (l > precision)
    {
      if (!hitBound)
      {
        l= precision;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] bounds;
  delete [] A;
  return CFList();
}


CFList
increasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int factorsFound,
                        int oldNumCols, int oldL, const Variable& alpha,
                        int precision
                       )
{
  bool irreducible= false;
  int d;
  int* bounds= computeBounds (F, d);
  int extensionDeg= degree (getMipo (alpha));
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }
  int l= tmax (2*(minBound + 1), oldL);
  int oldL2= l/2;
  int stepSize= 2;
  bool useOldQs= false;
  bool hitBound= false;
  while (l <= precision)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix ((l - k)*extensionDeg, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k, alpha);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          delete [] bounds;
          return CFList (F);
        }
      }
    }

    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, NTLN, false
                          );
        if (result.length() == NTLN.NumCols())
        {
          delete [] factorsFoundIndex;
          delete [] A;
          delete [] bounds;
          return result;
        }
        delete [] factorsFoundIndex;
      }
      else if (l == precision)
      {
        CanonicalForm bufF= F;
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN);
        F= bufF;
        delete [] zeroOne;
        delete [] A;
        delete [] bounds;
        return result;
      }
    }
    oldL2= l;
    l += stepSize;
    stepSize *= 2;
    if (l > precision)
    {
      if (!hitBound)
      {
        hitBound= true;
        l= precision;
      }
      else
        break;
    }
  }
  delete [] bounds;
  delete [] A;
  return CFList();
}

CFList
increasePrecision (CanonicalForm& F, CFList& factors, int oldL, int
                   l, int d, int* bounds, CFArray& bufQ, mat_zz_p& NTLN
                  )
{
  CFList result= CFList();
  bool irreducible= false;
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2;
  bool hitBound= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
  {
    ident (NTLN, factors.length());
    bufQ= CFArray (factors.length());
  }
  bool useOldQs= false;
  while (oldL <= l)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        CFMatrix C= CFMatrix (oldL - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          return CFList (F);
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      delete [] A;
      return CFList (F);
    }
    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    CanonicalForm bufF= F;
    CFList bufUniFactors= factors;
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN);
    delete [] zeroOneVecs;
    if (degree (bufF) + 1 + degree (LC (bufF, 1)) < oldL && result.length() > 0)
    {
      F= bufF;
      factors= bufUniFactors;
      delete [] A;
      return result;
    }

    result= CFList();
    oldL2= oldL;
    oldL *= 2;
    if (oldL > l)
    {
      if (!hitBound)
      {
        oldL= l;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return result;
}

CFList
increasePrecision (CanonicalForm& F, CFList& factors, int oldL, int
                   l, int d, int* bounds, CFArray& bufQ, mat_zz_pE& NTLN
                  )
{
  CFList result= CFList();
  bool irreducible= false;
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2;
  bool hitBound= false;
  bool useOldQs= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (oldL <= l)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        CFMatrix C= CFMatrix (oldL - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_pE* NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        mat_zz_pE NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          return CFList (F);
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      delete [] A;
      return CFList (F);
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    CanonicalForm bufF= F;
    CFList bufUniFactors= factors;
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN);
    delete [] zeroOneVecs;
    if (degree (bufF) + 1 + degree (LC (bufF, 1)) < l && result.length() > 0)
    {
      F= bufF;
      factors= bufUniFactors;
      delete [] A;
      return result;
    }

    result= CFList();
    oldL2= oldL;
    oldL *= 2;
    if (oldL > l)
    {
      if (!hitBound)
      {
        oldL= l;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return result;
}

//over field extension
CFList
extIncreasePrecision (CanonicalForm& F, CFList& factors, int oldL, int l, int d,
                      int* bounds, CFArray& bufQ, mat_zz_p& NTLN, const
                      CanonicalForm& evaluation, const ExtensionInfo& info,
                      CFList& source, CFList& dest
                     )
{
  CFList result= CFList();
  bool irreducible= false;
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2; //be careful
  bool hitBound= false;
  bool useOldQs= false;
  bool GF= (CFFactory::gettype()==GaloisFieldDomain);
  int degMipo= degree (getMipo (info.getAlpha()));
  Variable alpha= info.getAlpha();

  Variable gamma= info.getBeta();
  CanonicalForm primElemAlpha= info.getGamma();
  CanonicalForm imPrimElemAlpha= info.getDelta();
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (oldL <= l)
  {
    CFListIterator j= factors;
    if (GF)
      setCharacteristic (getCharacteristic());
    Variable y= F.mvar();
    CanonicalForm powX= power (y-gamma, oldL);
    CFMatrix Mat= CFMatrix (oldL*degMipo, oldL*degMipo);
    for (int i= 0; i < oldL*degMipo; i++)
    {

      CanonicalForm imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      CFIterator iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    mat_zz_p* NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        CFMatrix C= CFMatrix (oldL*degMipo - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
          {
            if (GF)
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              setCharacteristic (getCharacteristic());
              A[ii] [i]= GF2FalphaRep (A[ii] [i], alpha);
              if (alpha != gamma)
                A [ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                     gamma, source, dest
                                    );
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, *NTLMat);
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, *NTLMat);
            }
          }
          writeInMatrix (C, buf, ii + 1, 0);
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          Variable y= Variable (2);
          CanonicalForm tmp= F (y - evaluation, y);
          CFList source, dest;
          tmp= mapDown (tmp, info, source, dest);
          delete [] A;
          return CFList (tmp);
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      Variable y= Variable (2);
      CanonicalForm tmp= F (y - evaluation, y);
      CFList source, dest;
      tmp= mapDown (tmp, info, source, dest);
      delete [] A;
      return CFList (tmp);
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    CanonicalForm bufF= F;
    CFList bufUniFactors= factors;
    result= extReconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN,
                               info, evaluation
                              );
    delete [] zeroOneVecs;
    if (degree (bufF) + 1 + degree (LC (bufF, 1)) < l && result.length() > 0)
    {
      F= bufF;
      factors= bufUniFactors;
      return result;
    }

    if (isReduced (NTLN))
    {
      int factorsFound= 0;
      CanonicalForm bufF= F;
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, NTLN, false, info, evaluation
                           );
      if (NTLN.NumCols() == result.length())
      {
        delete [] A;
        delete [] factorsFoundIndex;
        return result;
      }
    }
    result= CFList();
    oldL2= oldL;
    oldL *= 2;
    if (oldL > l)
    {
      if (!hitBound)
      {
        oldL= l;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return result;
}

CFList
increasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int oldL, int l,
                        int d, int* bounds, CFArray& bufQ, mat_zz_p& NTLN,
                        const Variable& alpha
                       )
{
  CFList result= CFList();
  bool irreducible= false;
  CFArray * A= new CFArray [factors.length()];
  int extensionDeg= degree (getMipo (alpha));
  int oldL2= oldL/2;
  bool hitBound= false;
  bool useOldQs= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
  {
    int minBound= bounds [0];
    for (int i= 1; i < d; i++)
    {
      if (bounds [i] != 0)
        minBound= tmin (minBound, bounds [i]);
    }
    oldL= 2*(minBound+1);
    oldL2= minBound + 1;
    ident (NTLN, factors.length());
  }
  while (oldL <= l)
  {
    CFListIterator j= factors;
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        CFMatrix C= CFMatrix ((oldL - k)*extensionDeg, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k, alpha);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          delete [] A;
          return CFList (F);
        }
      }
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);

    CanonicalForm bufF= F;
    CFList bufUniFactors= factors;
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN);
    delete [] zeroOneVecs;
    if (degree (bufF) + 1 + degree (LC (bufF, 1)) < l && result.length() > 0)
    {
      F= bufF;
      factors= bufUniFactors;
      delete [] A;
      return result;
    }

    result= CFList();
    oldL2= oldL;
    oldL *= 2;
    if (oldL > l)
    {
      if (!hitBound)
      {
        oldL= l;
        hitBound= true;
      }
      else
        break;
    }
  }
  delete [] A;
  return result;
}

CFList
furtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList&
                                    factors, int l, int liftBound, int d, int*
                                    bounds, mat_zz_p& NTLN, CFList& diophant,
                                    CFMatrix& M, CFArray& Pi, CFArray& bufQ
                                   )
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  bool hitBound= false;
  int oldL= l;
  int stepSize= 8; //TODO choose better step size?
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l), 2);
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    bufFactors.insert (LCF);
    bufFactors.removeFirst();
    CFListIterator j= bufFactors;
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
      }
    }

    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }

    int * zeroOneVecs= extractZeroOneVecs (NTLN);
    CanonicalForm bufF= F;
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN);
    delete [] zeroOneVecs;
    if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
    {
      F= bufF;
      factors= bufFactors;
      delete [] A;
      return result;
    }

    if (isReduced (NTLN))
    {
      int factorsFound= 0;
      CanonicalForm bufF= F;
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      if (l < liftBound)
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, NTLN, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, false
                          );

      if (NTLN.NumCols() == result.length())
      {
        delete [] A;
        delete [] factorsFoundIndex;
        return result;
      }
      delete [] factorsFoundIndex;
    }
    result= CFList();
    oldL= l;
    stepSize *= 2;
    l += stepSize;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  if (irreducible)
  {
    delete [] A;
    return CFList (F);
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}

//Fq
CFList
furtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList&
                                    factors, int l, int liftBound, int d, int*
                                    bounds, mat_zz_pE& NTLN, CFList& diophant,
                                    CFMatrix& M, CFArray& Pi, CFArray& bufQ
                                   )
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  bool hitBound= false;
  int oldL= l;
  int stepSize= 8; //TODO choose better step size?
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l), 2);
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    CFListIterator j= bufFactors;
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_pE* NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        mat_zz_pE NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }

      int * zeroOneVecs= extractZeroOneVecs (NTLN);
      CanonicalForm bufF= F;
      result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN);
      delete [] zeroOneVecs;
      if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
      {
        F= bufF;
        factors= bufFactors;
        delete [] A;
        return result;
      }

    if (isReduced (NTLN))
    {
      int factorsFound= 0;
      CanonicalForm bufF= F;
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      if (l < liftBound)
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, NTLN, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, false
                          );
      if (NTLN.NumCols() == result.length())
      {
        delete [] A;
        delete [] factorsFoundIndex;
        return result;
      }
      delete [] factorsFoundIndex;
    }
    result= CFList();
    oldL= l;
    stepSize *= 2;
    l += stepSize;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  if (irreducible)
  {
    delete [] A;
    return CFList (F);
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}

//over field extension
CFList
extFurtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList& factors, int l,
                                       int liftBound, int d, int* bounds,
                                       mat_zz_p& NTLN, CFList& diophant,
                                       CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                       const CanonicalForm& evaluation, const
                                       ExtensionInfo& info, CFList& source,
                                       CFList& dest
                                      )
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  bool hitBound= false;
  bool GF= (CFFactory::gettype()==GaloisFieldDomain);
  int degMipo= degree (getMipo (info.getAlpha()));
  Variable alpha= info.getAlpha();
  int oldL= l; //be careful
  int stepSize= 8;
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l),2);
  Variable gamma= info.getBeta();
  CanonicalForm primElemAlpha= info.getGamma();
  CanonicalForm imPrimElemAlpha= info.getDelta();
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);

    if (GF)
      setCharacteristic (getCharacteristic());

    Variable y= F.mvar();
    CanonicalForm powX= power (y-gamma, l);
    CFMatrix Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {

      CanonicalForm imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      CFIterator iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    mat_zz_p* NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);

    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    CFListIterator j= bufFactors;
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l*degMipo - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
          {
            if (GF)
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              setCharacteristic (getCharacteristic());
              A[ii] [i]= GF2FalphaRep (A[ii] [i], alpha);
              if (alpha != gamma)
                A [ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                     gamma, source, dest
                                    );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
            }
          }
          writeInMatrix (C, buf, ii + 1, 0);
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }

      int * zeroOneVecs= extractZeroOneVecs (NTLN);
      CanonicalForm bufF= F;
      result= extReconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN, info,
                                 evaluation
                                );
      delete [] zeroOneVecs;
      if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
      {
        F= bufF;
        factors= bufFactors;
        delete [] A;
        return result;
      }

    if (isReduced (NTLN))
    {
      int factorsFound= 0;
      CanonicalForm bufF= F;
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      if (l < degree (bufF) + 1 + degree (LCF))
        extReconstructionTry (result, bufF, bufFactors, l, factorsFound,
                              factorsFoundIndex, NTLN, false, info, evaluation
                             );
      else
        extReconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                              degree (LCF), factorsFound, factorsFoundIndex,
                              NTLN, false, info, evaluation
                             );
      if (NTLN.NumCols() == result.length())
      {
        delete [] A;
        delete [] factorsFoundIndex;
        return result;
      }
      delete [] factorsFoundIndex;
    }
    result= CFList();
    oldL= l;
    stepSize *= 2;
    l += stepSize;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  if (irreducible)
  {
    delete [] A;
    Variable y= Variable (2);
    CanonicalForm tmp= F (y - evaluation, y);
    CFList source, dest;
    tmp= mapDown (tmp, info, source, dest);
    return CFList (tmp);
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}

CFList
furtherLiftingAndIncreasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int
                                         l, int liftBound, int d, int* bounds,
                                         mat_zz_p& NTLN, CFList& diophant,
                                         CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                         const Variable& alpha
                                        )
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  int extensionDeg= degree (getMipo (alpha));
  bool hitBound= false;
  int oldL= l;
  int stepSize= 8; //TODO choose better step size?
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l), 2);
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    CFListIterator j= bufFactors;
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, oldL, bufQ[i],bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (F, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix ((l - k)*extensionDeg, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
            buf= getCoeffs (A[ii] [i], k, alpha);
          writeInMatrix (C, buf, ii + 1, 0);
        }
        mat_zz_p* NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        mat_zz_p NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      irreducible= true;
      break;
    }

      int * zeroOneVecs= extractZeroOneVecs (NTLN);
      CanonicalForm bufF= F;
      result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN);
      delete [] zeroOneVecs;
      if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
      {
        F= bufF;
        factors= bufFactors;
        delete [] A;
        return result;
      }

    if (isReduced (NTLN))
    {
      int factorsFound= 0;
      CanonicalForm bufF= F;
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
      if (l < degree (bufF) + 1 + degree (LCF))
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, NTLN, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, false
                          );
      if (NTLN.NumCols() == result.length())
      {
        delete [] A;
        delete [] factorsFoundIndex;
        return result;
      }
      delete [] factorsFoundIndex;
    }
    result= CFList();
    oldL= l;
    stepSize *= 2;
    l += stepSize;
    if (l > liftBound)
    {
      if (!hitBound)
      {
        l= liftBound;
        hitBound= true;
      }
      else
        break;
    }
  }
  if (irreducible)
  {
    delete [] A;
    return CFList (F);
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}

void
refineAndRestartLift (const CanonicalForm& F, const mat_zz_p& NTLN, int
                      liftBound, int l, CFList& factors, CFMatrix& M, CFArray&
                      Pi, CFList& diophant
                     )
{
  CFList bufFactors;
  Variable y= Variable (2);
  CanonicalForm LCF= LC (F, 1);
  for (long i= 1; i <= NTLN.NumCols(); i++)
  {
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    for (long j= 1; j <= NTLN.NumRows(); j++, iter++)
    {
      if (!IsZero (NTLN (j,i)))
        buf= mulNTL (buf, mod (iter.getItem(), y));
    }
    bufFactors.append (buf);
  }
  factors= bufFactors;
  M= CFMatrix (liftBound, factors.length());
  Pi= CFArray();
  diophant= CFList();
  factors.insert (LCF);
  henselLift12 (F, factors, l, Pi, diophant, M);
}

void
refineAndRestartLift (const CanonicalForm& F, const mat_zz_pE& NTLN, int
                      liftBound, int l, CFList& factors, CFMatrix& M, CFArray&
                      Pi, CFList& diophant
                     )
{
  CFList bufFactors;
  Variable y= Variable (2);
  CanonicalForm LCF= LC (F, 1);
  for (long i= 1; i <= NTLN.NumCols(); i++)
  {
    CFListIterator iter= factors;
    CanonicalForm buf= 1;
    for (long j= 1; j <= NTLN.NumRows(); j++, iter++)
    {
      if (!IsZero (NTLN (j,i)))
        buf= mulNTL (buf, mod (iter.getItem(), y));
    }
    bufFactors.append (buf);
  }
  factors= bufFactors;
  M= CFMatrix (liftBound, factors.length());
  Pi= CFArray();
  diophant= CFList();
  factors.insert (LCF);
  henselLift12 (F, factors, l, Pi, diophant, M);
}

CFList
earlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_p& N,
                               CanonicalForm& bufF, CFList& factors, int& l,
                               int& factorsFound, bool beenInThres, CFMatrix& M,
                               CFArray& Pi, CFList& diophant
                              )
{
  bufF= F;
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= 11;
  mat_zz_p NTLN= N;
  int * factorsFoundIndex= new int [NTLN.NumCols()];
  for (long i= 0; i < NTLN.NumCols(); i++)
    factorsFoundIndex [i]= 0;

  if (degree (F) + 1 > smallFactorDeg)
  {
    if (l < smallFactorDeg)
    {
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      l= smallFactorDeg;
    }
    reconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                       factorsFoundIndex, NTLN, beenInThres
                      );
    if (result.length() == NTLN.NumCols())
    {
      delete [] factorsFoundIndex;
      return result;
    }
  }

  if (l < degree (bufF)/2+2)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF)/2 + 2, Pi, diophant, M);
    l= degree (bufF)/2 + 2;
  }
  reconstructionTry (result, bufF, factors, degree (bufF)/2 + 2,
                     factorsFound, factorsFoundIndex, NTLN, beenInThres
                    );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }

  if (l < degree (F) + 1)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF) + 1, Pi, diophant, M);
    l= degree (bufF) + 1;
  }
  reconstructionTry (result, bufF, factors, degree (bufF) + 1, factorsFound,
                     factorsFoundIndex, NTLN, beenInThres
                    );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }
  delete [] factorsFoundIndex;
  return result;
}

CFList
earlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_pE& N,
                               CanonicalForm& bufF, CFList& factors, int& l,
                               int& factorsFound, bool beenInThres, CFMatrix& M,
                               CFArray& Pi, CFList& diophant
                              )
{
  bufF= F;
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= 11;
  mat_zz_pE NTLN= N;
  int * factorsFoundIndex= new int [NTLN.NumCols()];
  for (long i= 0; i < NTLN.NumCols(); i++)
    factorsFoundIndex [i]= 0;
  if (degree (F) + 1 > smallFactorDeg)
  {
    if (l < smallFactorDeg)
    {
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      l= smallFactorDeg;
    }
    reconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                       factorsFoundIndex, NTLN, beenInThres
                      );
    if (result.length() == NTLN.NumCols())
    {
      delete [] factorsFoundIndex;
      return result;
    }
  }

  if (l < degree (bufF)/2+2)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF)/2 + 2, Pi, diophant, M);
    l= degree (bufF)/2 + 2;
  }
  reconstructionTry (result, bufF, factors, degree (bufF)/2 + 2,
                     factorsFound, factorsFoundIndex, NTLN, beenInThres
                    );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }

  if (l < degree (F) + 1)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF) + 1, Pi, diophant, M);
    l= degree (bufF) + 1;
  }
  reconstructionTry (result, bufF, factors, degree (bufF) + 1, factorsFound,
                     factorsFoundIndex, NTLN, beenInThres
                    );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }
  delete [] factorsFoundIndex;
  return result;
}

//over field extension
CFList
extEarlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_p& N,
                                  CanonicalForm& bufF, CFList& factors, int& l,
                                  int& factorsFound, bool beenInThres, CFMatrix&
                                  M, CFArray& Pi, CFList& diophant, const
                                  ExtensionInfo& info, const CanonicalForm&
                                  evaluation
                                 )
{
  bufF= F;
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= 11;
  mat_zz_p NTLN= N;
  int * factorsFoundIndex= new int [NTLN.NumCols()];
  for (long i= 0; i < NTLN.NumCols(); i++)
    factorsFoundIndex [i]= 0;

  if (degree (F) + 1 > smallFactorDeg)
  {
    if (l < smallFactorDeg)
    {
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      l= smallFactorDeg;
    }
    extReconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                          factorsFoundIndex, NTLN, beenInThres, info,
                          evaluation
                         );
    if (result.length() == NTLN.NumCols())
    {
      delete [] factorsFoundIndex;
      return result;
    }
  }

  if (l < degree (bufF)/2+2)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF)/2 + 2, Pi, diophant, M);
    l= degree (bufF)/2 + 2;
  }
  extReconstructionTry (result, bufF, factors, degree (bufF)/2 + 2,
                       factorsFound, factorsFoundIndex, NTLN, beenInThres, info,
                       evaluation
                       );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }

  if (l < degree (bufF) + 1)
  {
    factors.insert (LCF);
    henselLiftResume12 (F, factors, l, degree (bufF) + 1, Pi, diophant, M);
    l= degree (bufF) + 1;
  }

  extReconstructionTry (result, bufF, factors, degree (bufF) + 1, factorsFound,
                        factorsFoundIndex, NTLN, beenInThres, info, evaluation
                       );
  if (result.length() == NTLN.NumCols())
  {
    delete [] factorsFoundIndex;
    return result;
  }
  delete [] factorsFoundIndex;
  return result;
}

CFList
sieveSmallFactors (const CanonicalForm& G, CFList& uniFactors, DegreePattern&
                   degPat, CanonicalForm& H, CFList& diophant, CFArray& Pi,
                   CFMatrix& M, bool& success, int d
                  )
{
  CanonicalForm F= G;
  CFList bufUniFactors= uniFactors;
  bufUniFactors.insert (LC (F, 1));
  int smallFactorDeg= d;
  DegreePattern degs= degPat;
  henselLift12 (F, bufUniFactors, smallFactorDeg, Pi, diophant, M);
  int adaptedLiftBound;
  success= false;
  CFList earlyFactors= earlyFactorDetection (F, bufUniFactors, adaptedLiftBound,
                                             degs, success, smallFactorDeg
                                            );
  if (degs.getLength() == 1)
  {
    degPat= degs;
    return earlyFactors;
  }
  if (success)
  {
    H= F;
    return earlyFactors;
  }
  int sizeOldF= size (F);
  CFList result;
  CanonicalForm bufF= F;
  if (earlyFactors.length() > 0)
  {
    for (CFListIterator i= earlyFactors; i.hasItem(); i++)
      bufF /= i.getItem();
  }

  if (size (bufF) < sizeOldF)
  {
    H= bufF;
    success= true;
    return earlyFactors;
  }
  else
  {
    uniFactors= bufUniFactors;
    return CFList();
  }
}

CFList
extSieveSmallFactors (const CanonicalForm& G, CFList& uniFactors, DegreePattern&
                      degPat, CanonicalForm& H, CFList& diophant, CFArray& Pi,
                      CFMatrix& M, bool& success, int d, const CanonicalForm&
                      evaluation, const ExtensionInfo& info
                     )
{
  CanonicalForm F= G;
  CFList bufUniFactors= uniFactors;
  bufUniFactors.insert (LC (F, 1));
  int smallFactorDeg= d;
  DegreePattern degs= degPat;
  henselLift12 (F, bufUniFactors, smallFactorDeg, Pi, diophant, M);
  int adaptedLiftBound;
  success= false;
  CFList earlyFactors= extEarlyFactorDetection (F, bufUniFactors,
                                                adaptedLiftBound, degs, success,
                                                info, evaluation, smallFactorDeg
                                               );
  if (degs.getLength() == 1)
  {
    degPat= degs;
    return earlyFactors;
  }
  if (success)
  {
    H= F;
    return earlyFactors;
  }
  Variable y= F.mvar();
  CanonicalForm shiftedF= F (y - evaluation, y);
  int sizeOldF= size (shiftedF);
  CFList result;
  CanonicalForm bufF= shiftedF;
  if (earlyFactors.length() > 0)
  {
    for (CFListIterator i= earlyFactors; i.hasItem(); i++)
    {
      bufF /= i.getItem();
      result.append (i.getItem());
    }
  }

  if (size (bufF) < sizeOldF)
  {
    H= bufF (y + evaluation, y); //shift back to zero
    success= true;
    return result;
  }
  else
  {
    uniFactors= bufUniFactors;
    return CFList();
  }
}

CFList
henselLiftAndLatticeRecombi (const CanonicalForm& G, const CFList& uniFactors,
                             const Variable& alpha, const DegreePattern& degPat
                            )
{
  DegreePattern degs= degPat;
  CanonicalForm F= G;
  CanonicalForm LCF= LC (F, 1);
  Variable y= F.mvar();
  Variable x= Variable (1);
  int d;
  int* bounds= computeBounds (F, d);

  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  CFList bufUniFactors= uniFactors;
  CFArray Pi;
  CFList diophant;
  int liftBound= 2*totaldegree (F) - 1;
  CFMatrix M= CFMatrix (liftBound, bufUniFactors.length());

  CFList smallFactors;
  CanonicalForm H;
  bool success;
  smallFactors= sieveSmallFactors (F, bufUniFactors, degs, H, diophant, Pi, M,
                                   success, 2*(minBound + 1)
                                  );

  if (smallFactors.length() > 0)
  {
    if (smallFactors.length() == 1)
    {
      if (smallFactors.getFirst() == F)
      {
        delete [] bounds;
        return CFList (G);
      }
    }
    if (degs.getLength() <= 1)
    {
      delete [] bounds;
      return smallFactors;
    }
  }

  int index;
  for (CFListIterator i= smallFactors; i.hasItem(); i++)
  {
    index= 1;
    CanonicalForm tmp1, tmp2;
    tmp1= mod (i.getItem(),y);
    tmp1 /= Lc (tmp1);
    for (CFListIterator j= bufUniFactors; j.hasItem(); j++, index++)
    {
      tmp2= mod (j.getItem(), y);
      tmp2 /= Lc (tmp2);
      if (tmp1 == tmp2)
      {
        index++;
        j.remove(index);
        break;
      }
    }
  }

  if (bufUniFactors.isEmpty())
  {
    delete [] bounds;
    return smallFactors;
  }

  if (success)
  {
    F= H;
    delete [] bounds;
    bounds= computeBounds (F, d);
    LCF= LC (F, 1);

    minBound= bounds[0];
    for (int i= 1; i < d; i++)
    {
      if (bounds[i] != 0)
        minBound= tmin (minBound, bounds[i]);
    }
    Pi= CFArray();
    diophant= CFList();
    liftBound= 2*totaldegree (F) - 1;
    M= CFMatrix (liftBound, bufUniFactors.length());
    DegreePattern bufDegs= DegreePattern (bufUniFactors);
    degs.intersect (bufDegs);
    degs.refine();
    if (degs.getLength() <= 1)
    {
      smallFactors.append (F);
      delete [] bounds;
      return smallFactors;
    }
  }

  bool reduceFq2Fp= (degree (F) > getCharacteristic());
  bufUniFactors.insert (LCF);
  int l= 1;

  zz_p::init (getCharacteristic());
  mat_zz_p NTLN;
  if (alpha.level() != 1)
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);
  }
  mat_zz_pE NTLNe;
  if (alpha.level() == 1)
    ident (NTLN, bufUniFactors.length() - 1);
  else
  {
    if (reduceFq2Fp)
      ident (NTLN, bufUniFactors.length() - 1);
    else
      ident (NTLNe, bufUniFactors.length() - 1);
  }
  bool irreducible= false;
  CFArray bufQ= CFArray (bufUniFactors.length() - 1);

  int oldL;
  if (success)
  {
    int start= 0;
    if (alpha.level() == 1)
      oldL= liftAndComputeLattice (F, bounds, d, start, liftBound, minBound,
                                   bufUniFactors, NTLN, diophant, M, Pi, bufQ,
                                   irreducible
                                  );
    else
    {
      if (reduceFq2Fp)
        oldL= liftAndComputeLatticeFq2Fp (F, bounds, d, start, liftBound,
                                          minBound, bufUniFactors, NTLN,
                                          diophant, M, Pi, bufQ, irreducible,
                                          alpha
                                         );
      else
        oldL= liftAndComputeLattice (F, bounds, d, start, liftBound, minBound,
                                    bufUniFactors, NTLNe, diophant, M, Pi, bufQ,
                                    irreducible
                                    );
    }
  }
  else
  {
    if (alpha.level() == 1)
      oldL= liftAndComputeLattice (F, bounds, d, 2*(minBound + 1), liftBound,
                                   minBound, bufUniFactors, NTLN, diophant, M,
                                   Pi, bufQ, irreducible
                                  );
    else
    {
      if (reduceFq2Fp)
        oldL= liftAndComputeLatticeFq2Fp (F, bounds, d, 2*(minBound + 1),
                                          liftBound, minBound, bufUniFactors,
                                          NTLN, diophant, M, Pi, bufQ,
                                          irreducible, alpha
                                         );
      else
        oldL= liftAndComputeLattice (F, bounds, d, 2*(minBound + 1), liftBound,
                                     minBound, bufUniFactors, NTLNe, diophant,
                                     M, Pi, bufQ, irreducible
                                    );
    }
  }

  bufUniFactors.removeFirst();
  if (oldL > liftBound)
  {
    delete [] bounds;
    return Union (smallFactors,
                  factorRecombination (bufUniFactors, F,
                                       power (y, degree (F) + 1 + degree (LCF)),
                                       degs, 1, bufUniFactors.length()/2
                                      )
                 );
  }

  l= oldL;
  if (irreducible)
  {
    delete [] bounds;
    return Union (CFList (F), smallFactors);
  }

  CanonicalForm yToL= power (y,l);

  CFList result;
  if (l >= degree (F) + 1)
  {
    int * factorsFoundIndex;
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
    }
    else
    {
      factorsFoundIndex= new int [NTLNe.NumCols()];
      for (long i= 0; i < NTLNe.NumCols(); i++)
        factorsFoundIndex[i]= 0;
    }
    int factorsFound= 0;
    CanonicalForm bufF= F;
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                         factorsFound, factorsFoundIndex, NTLN, false
                        );
    else
        reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                           factorsFound, factorsFoundIndex, NTLNe, false
                          );
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      if (result.length() == NTLN.NumCols())
      {
        delete [] factorsFoundIndex;
        delete [] bounds;
        return Union (result, smallFactors);
      }
    }
    else
    {
      if (result.length() == NTLNe.NumCols())
      {
        delete [] factorsFoundIndex;
        delete [] bounds;
        return Union (result, smallFactors);
      }
    }
    delete [] factorsFoundIndex;
  }
  if (l >= liftBound)
  {
    int * factorsFoundIndex;
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
    }
    else
    {
      factorsFoundIndex= new int [NTLNe.NumCols()];
      for (long i= 0; i < NTLNe.NumCols(); i++)
        factorsFoundIndex[i]= 0;
    }
    CanonicalForm bufF= F;
    int factorsFound= 0;
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1 + degree
                         (LCF), factorsFound, factorsFoundIndex, NTLN, false
                        );
    else
      reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1 + degree
                         (LCF), factorsFound, factorsFoundIndex, NTLNe, false
                        );
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      if (result.length() == NTLN.NumCols())
      {
        delete [] factorsFoundIndex;
        delete [] bounds;
        return Union (result, smallFactors);
      }
    }
    else
    {
      if (result.length() == NTLNe.NumCols())
      {
        delete [] factorsFoundIndex;
        delete [] bounds;
        return Union (result, smallFactors);
      }
    }
    delete [] factorsFoundIndex;
  }

  result= CFList();
  bool beenInThres= false;
  int thres= 100;
  if (l <= thres)
  {
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      if (NTLN.NumCols() < bufUniFactors.length())
      {
        refineAndRestartLift (F, NTLN, liftBound, l, bufUniFactors, M, Pi,
                              diophant
                             );
        beenInThres= true;
      }
    }
    else
    {
      if (NTLNe.NumCols() < bufUniFactors.length())
      {
        refineAndRestartLift (F, NTLNe, liftBound, l, bufUniFactors, M, Pi,
                              diophant
                             );
        beenInThres= true;
      }
    }
  }

  CanonicalForm bufF= F;
  int factorsFound= 0;
  if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
  {
    result= earlyReconstructionAndLifting (F, NTLN, bufF, bufUniFactors, l,
                                           factorsFound, beenInThres, M, Pi,
                                           diophant
                                          );

    if (result.length() == NTLN.NumCols())
    {
      delete [] bounds;
      return Union (result, smallFactors);
    }
  }
  else
  {
    result= earlyReconstructionAndLifting (F, NTLNe, bufF, bufUniFactors, l,
                                           factorsFound, beenInThres, M, Pi,
                                           diophant
                                          );

    if (result.length() == NTLNe.NumCols())
    {
      delete [] bounds;
      return Union (result, smallFactors);
    }
  }

  if (result.length() > 0)
  {
    if (beenInThres)
    {
    int index;
    for (CFListIterator i= result; i.hasItem(); i++)
    {
      index= 1;
      CanonicalForm tmp1, tmp2;
      tmp1= mod (i.getItem(), y);
      tmp1 /= Lc (tmp1);
      for (CFListIterator j= bufUniFactors; j.hasItem(); j++, index++)
      {
        tmp2= mod (j.getItem(), y);
        tmp2 /= Lc (tmp2);
        if (tmp1 == tmp2)
        {
          index++;
          j.remove(index);
          break;
        }
      }
    }
    }
    else
    {
    int * zeroOne= extractZeroOneVecs (NTLN);
    CFList bufBufUniFactors= bufUniFactors;
    for (int i= 0; i < NTLN.NumCols(); i++)
    {
      if (zeroOne [i] == 0)
        continue;
      CFListIterator iter= bufUniFactors;
      CanonicalForm buf= 1;
      CFList factorsConsidered;
      for (int j= 0; j < NTLN.NumRows(); j++, iter++)
      {
        if (!IsZero (NTLN (j + 1,i + 1)))
        {
          factorsConsidered.append (iter.getItem());
          buf *= mod (iter.getItem(), y);
        }
      }
      buf /= Lc (buf);
      for (CFListIterator iter2= result; iter2.hasItem(); iter2++)
      {
        CanonicalForm tmp= mod (iter2.getItem(), y);
        tmp /= Lc (tmp);
        if (tmp == buf)
        {
          bufBufUniFactors= Difference (bufBufUniFactors, factorsConsidered);
          break;
        }
      }
    }
    bufUniFactors= bufBufUniFactors;
    delete [] zeroOne;
    }

    int oldNumCols;
    CFList resultBufF;
    irreducible= false;

    if (alpha.level() == 1)
    {
      oldNumCols= NTLN.NumCols();
      resultBufF= increasePrecision (bufF, bufUniFactors, factorsFound,
                                     oldNumCols, oldL, l
                                    );
    }
    else
    {
      if (reduceFq2Fp)
      {
        oldNumCols= NTLN.NumCols();

        resultBufF= increasePrecisionFq2Fp (bufF, bufUniFactors, factorsFound,
                                            oldNumCols, oldL, alpha, l
                                           );
      }
      else
      {
        oldNumCols= NTLNe.NumCols();

        resultBufF= increasePrecision (bufF, bufUniFactors, factorsFound,
                                       oldNumCols, oldL,  alpha, l
                                      );
      }
    }

    if (bufUniFactors.isEmpty() || degree (bufF) <= 0)
    {
      delete [] bounds;
      result= Union (resultBufF, result);
      return Union (result, smallFactors);
    }

    for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
      i.getItem()= mod (i.getItem(), y);

    result= Union (result, resultBufF);
    result= Union (result, smallFactors);
    delete [] bounds;
    DegreePattern bufDegs= DegreePattern (bufUniFactors);
    degs.intersect (bufDegs);
    degs.refine();
    if (degs.getLength() == 1 || bufUniFactors.length() == 1)
    {
      result.append (bufF);
      return result;
    }
    return Union (result, henselLiftAndLatticeRecombi (bufF, bufUniFactors,
                                                       alpha, degs
                                                      )
                 );
  }

  if (l < liftBound)
  {
    if (alpha.level() == 1)
    {
        result=increasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                  NTLN
                                 );
    }
    else
    {
      if (reduceFq2Fp)
      {
          result=increasePrecisionFq2Fp (F, bufUniFactors, oldL, l, d, bounds,
                                         bufQ, NTLN, alpha
                                        );
      }
      else
      {
          result=increasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                    NTLNe
                                   );
      }
    }
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
      if (result.length()== NTLN.NumCols())
      {
        delete [] bounds;
        result= Union (result, smallFactors);
        return result;
      }
    }
    else
    {
      if (result.length()== NTLNe.NumCols())
      {
        delete [] bounds;
        result= Union (result, smallFactors);
        return result;
      }
    }

    if (result.isEmpty())
    {
      if (alpha.level() == 1)
        result= furtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                    liftBound, d, bounds, NTLN,
                                                    diophant, M, Pi, bufQ
                                                   );
      else
      {
        if (reduceFq2Fp)
          result= furtherLiftingAndIncreasePrecisionFq2Fp (F,bufUniFactors, l,
                                                           liftBound, d, bounds,
                                                           NTLN, diophant, M,
                                                           Pi, bufQ, alpha
                                                          );
        else
          result= furtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                      liftBound, d, bounds,
                                                      NTLNe, diophant, M,
                                                      Pi, bufQ
                                                     );
      }

      if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      {
        if (result.length() == NTLN.NumCols())
        {
          delete [] bounds;
          result= Union (result, smallFactors);
          return result;
        }
      }
      else
      {
        if (result.length() == NTLNe.NumCols())
        {
          delete [] bounds;
          result= Union (result, smallFactors);
          return result;
        }
      }
    }
  }

  DEBOUTLN (cerr, "lattice recombination failed");

  DegreePattern bufDegs= DegreePattern (bufUniFactors);
  degs.intersect (bufDegs);
  degs.refine();

  delete [] bounds;
  bounds= computeBounds (F, d);
  minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  if (minBound > 16 || result.length() == 0)
  {
    result= Union (result, smallFactors);
    CanonicalForm MODl= power (y, degree (F) + 1 + degree (LC (F, 1)));
    delete [] bounds;
    return Union (result, factorRecombination (bufUniFactors, F, MODl, degs, 1,
                                               bufUniFactors.length()/2
                                              )
                 );
  }
  else
  {
    result= Union (result, smallFactors);
    for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
      i.getItem()= mod (i.getItem(), y);
    delete [] bounds;
    return Union (result, henselLiftAndLatticeRecombi (F, bufUniFactors, alpha,
                                                       degs
                                                      )
                 );
  }
}

ExtensionInfo
init4ext (const ExtensionInfo& info, const CanonicalForm& evaluation,
          int& degMipo
         )
{
  bool GF= (CFFactory::gettype() == GaloisFieldDomain);
  Variable alpha= info.getAlpha();
  if (GF)
  {
    degMipo= getGFDegree();
    CanonicalForm GFMipo= gf_mipo;
    setCharacteristic (getCharacteristic());
    GFMipo.mapinto();
    alpha= rootOf (GFMipo);
    setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
  }
  else
  {
    alpha= info.getAlpha();
    degMipo= degree (getMipo (alpha));
  }

  Variable gamma;
  CanonicalForm primElemAlpha, imPrimElemAlpha;
  if ((!GF && evaluation != alpha) || (GF && evaluation != getGFGenerator()))
  {
    CanonicalForm bufEvaluation;
    if (GF)
    {
      setCharacteristic (getCharacteristic());
      bufEvaluation= GF2FalphaRep (evaluation, alpha);
    }
    else
      bufEvaluation= evaluation;
    CanonicalForm mipo= findMinPoly (bufEvaluation, alpha);
    gamma= rootOf (mipo);
    Variable V_buf;
    bool fail= false;
    primElemAlpha= primitiveElement (alpha, V_buf, fail);
    imPrimElemAlpha= map (primElemAlpha, alpha, bufEvaluation, gamma);

    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
  }
  else
    gamma= alpha;
  ExtensionInfo info2= ExtensionInfo (alpha, gamma, primElemAlpha,
                                      imPrimElemAlpha, 1, info.getGFName(), true
                                     );

  return info2;
}

CFList
extHenselLiftAndLatticeRecombi(const CanonicalForm& G, const CFList& uniFactors,
                               const ExtensionInfo& extInfo, const
                               DegreePattern& degPat, const CanonicalForm& eval
                              )
{
  CanonicalForm evaluation= eval;
  ExtensionInfo info= extInfo;
  Variable alpha;
  DegreePattern degs= degPat;
  CanonicalForm F= G;
  Variable x= Variable (1);
  Variable y= F.mvar();
  CFList bufUniFactors= uniFactors;


  int degMipo;
  ExtensionInfo info2= init4ext (info, evaluation, degMipo);

  CFList source, dest;
  CanonicalForm LCF= LC (F, 1);

  int d;
  int* bounds= computeBounds (F, d);
  int minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }


  CFArray Pi;
  CFList diophant;
  int liftBound= tmax ((2*totaldegree (F) - 1)/degMipo + 1, degree (F) + 1 +
                       degree (LC (F, 1)));
  CFMatrix M= CFMatrix (liftBound, bufUniFactors.length());

  CFList smallFactors;
  CanonicalForm H;
  bool success;
  smallFactors= extSieveSmallFactors (F, bufUniFactors, degs, H, diophant, Pi,
                                      M, success, minBound + 1, evaluation, info
                                     );

  if (smallFactors.length() > 0)
  {
    if (smallFactors.length() == 1)
    {
      if (smallFactors.getFirst() == F)
      {
        delete [] bounds;
        CFList source, dest;
        CanonicalForm tmp= G (y - evaluation, y);
        tmp= mapDown (tmp, info, source, dest);
        return CFList (tmp);
      }
    }
    if (degs.getLength() <= 1)
    {
      delete [] bounds;
      return smallFactors;
    }
  }

  int index;
  for (CFListIterator i= smallFactors; i.hasItem(); i++)
  {
    index= 1;
    CanonicalForm tmp1, tmp2;
    tmp1= mod (i.getItem(), y - evaluation);
    tmp1 /= Lc (tmp1);
    for (CFListIterator j= bufUniFactors; j.hasItem(); j++, index++)
    {
      tmp2= mod (j.getItem(), y);
      tmp2 /= Lc (tmp2);
      if (tmp1 == tmp2)
      {
        index++;
        j.remove(index);
        break;
      }
    }
  }

  if (bufUniFactors.isEmpty())
  {
    delete [] bounds;
    return smallFactors;
  }

  if (success)
  {
    F= H;
    delete [] bounds;
    bounds= computeBounds (F, d);
    LCF= LC (F, 1);

    minBound= bounds[0];
    for (int i= 1; i < d; i++)
    {
      if (bounds[i] != 0)
        minBound= tmin (minBound, bounds[i]);
    }
    Pi= CFArray();
    diophant= CFList();
    liftBound=tmax ((2*totaldegree (F) - 1)/degMipo + 1, degree (F) + 1 +
                    degree (LC (F, 1)));
    M= CFMatrix (liftBound, bufUniFactors.length());
    DegreePattern bufDegs= DegreePattern (bufUniFactors);
    degs.intersect (bufDegs);
    degs.refine();
    if (degs.getLength() <= 1)
    {
      delete [] bounds;
      CFList source, dest;
      CanonicalForm tmp= F (y - evaluation, y);
      tmp= mapDown (tmp, info, source, dest);
      smallFactors.append (tmp);
      return smallFactors;
    }
  }

  bufUniFactors.insert (LCF);
  int l= 1;

  zz_p::init (getCharacteristic());
  zz_pX NTLMipo;
  mat_zz_p NTLN;

  ident (NTLN, bufUniFactors.length() - 1);
  bool irreducible= false;
  CFArray bufQ= CFArray (bufUniFactors.length() - 1);

  int oldL;
  if (success)
  {
    int start= 0;
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound, start,
                                    bufUniFactors, NTLN, diophant, M, Pi, bufQ,
                                    irreducible, evaluation, info2, source, dest
                                   );
  }
  else
  {
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound,
                                    minBound + 1, bufUniFactors, NTLN, diophant,
                                    M, Pi, bufQ, irreducible, evaluation, info2,
                                    source, dest
                                   );
  }

  bufUniFactors.removeFirst();
  if (oldL > liftBound)
  {
    delete [] bounds;
    return Union (smallFactors, extFactorRecombination
                                (bufUniFactors, F,
                                 power (y, degree (F) + 1 + degree (LCF)),info,
                                 degs, evaluation, 1, bufUniFactors.length()/2
                                )
                 );
  }

  l= oldL;
  if (irreducible)
  {
    delete [] bounds;
    CFList source, dest;
    CanonicalForm tmp= F (y - evaluation, y);
    tmp= mapDown (tmp, info, source, dest);
    return Union (CFList (tmp), smallFactors);
  }

  CanonicalForm yToL= power (y,l);

  CFList result;
  if (l >= degree (F) + 1)
  {
    int * factorsFoundIndex;

    factorsFoundIndex= new int [NTLN.NumCols()];
    for (long i= 0; i < NTLN.NumCols(); i++)
      factorsFoundIndex[i]= 0;

    int factorsFound= 0;
    CanonicalForm bufF= F;

    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                          factorsFound, factorsFoundIndex, NTLN, false, info,
                          evaluation
                         );

    if (result.length() == NTLN.NumCols())
    {
      delete [] factorsFoundIndex;
      delete [] bounds;
      return Union (result, smallFactors);
    }

    delete [] factorsFoundIndex;
  }
  if (l >= liftBound)
  {
    int * factorsFoundIndex;
      factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
        factorsFoundIndex[i]= 0;
    CanonicalForm bufF= F;
    int factorsFound= 0;

    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1 + degree
                          (LCF), factorsFound, factorsFoundIndex, NTLN, false,
                          info, evaluation
                         );

    if (result.length() == NTLN.NumCols())
    {
      delete [] factorsFoundIndex;
      delete [] bounds;
      return Union (result, smallFactors);
    }
    delete [] factorsFoundIndex;
  }

  result= CFList();
  bool beenInThres= false;
  int thres= 100;
  if (l <= thres && bufUniFactors.length() > NTLN.NumCols())
  {
    refineAndRestartLift (F, NTLN, 2*totaldegree (F)-1, l, bufUniFactors, M, Pi,
                         diophant
                        );
    beenInThres= true;
  }


  CanonicalForm bufF= F;
  int factorsFound= 0;

  result= extEarlyReconstructionAndLifting (F, NTLN, bufF, bufUniFactors, l,
                                            factorsFound, beenInThres, M, Pi,
                                            diophant, info, evaluation
                                           );

  if (result.length() == NTLN.NumCols())
  {
    delete [] bounds;
    return Union (result, smallFactors);
  }

  if (result.length() > 0)
  {
   if (beenInThres)
   {
      int index;
      for (CFListIterator i= result; i.hasItem(); i++)
      {
        index= 1;
        CanonicalForm tmp1, tmp2;
        tmp1= mod (i.getItem(), y-evaluation);
        tmp1 /= Lc (tmp1);
        for (CFListIterator j= bufUniFactors; j.hasItem(); j++, index++)
        {
          tmp2= mod (j.getItem(), y);
          tmp2 /= Lc (tmp2);
          if (tmp1 == tmp2)
          {
            index++;
            j.remove(index);
            break;
          }
        }
      }
    }
    else
    {
      int * zeroOne= extractZeroOneVecs (NTLN);
      CFList bufBufUniFactors= bufUniFactors;
      for (int i= 0; i < NTLN.NumCols(); i++)
      {
        if (zeroOne [i] == 0)
          continue;
        CFListIterator iter= bufUniFactors;
        CanonicalForm buf= 1;
        CFList factorsConsidered;
        for (int j= 0; j < NTLN.NumRows(); j++, iter++)
        {
          if (!IsZero (NTLN (j + 1,i + 1)))
          {
            factorsConsidered.append (iter.getItem());
            buf *= mod (iter.getItem(), y);
          }
        }
        buf /= Lc (buf);
        for (CFListIterator iter2= result; iter2.hasItem(); iter2++)
        {
          CanonicalForm tmp= mod (iter2.getItem(), y - evaluation);
          tmp /= Lc (tmp);
          if (tmp == buf)
          {
            bufBufUniFactors= Difference (bufBufUniFactors, factorsConsidered);
            break;
          }
        }
      }
      bufUniFactors= bufBufUniFactors;
      delete [] zeroOne;
    }

    int oldNumCols;
    CFList resultBufF;
    irreducible= false;

    oldNumCols= NTLN.NumCols();
    resultBufF= extIncreasePrecision (bufF, bufUniFactors, factorsFound,
                                      oldNumCols, oldL, evaluation, info2,
                                      source, dest, l
                                     );

    if (bufUniFactors.isEmpty() || degree (bufF) <= 0)
    {
      delete [] bounds;
      result= Union (resultBufF, result);
      return Union (result, smallFactors);
    }

    for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
      i.getItem()= mod (i.getItem(), y);

    delete [] bounds;
    CFList bufResult;
    DegreePattern bufDegs= DegreePattern (bufUniFactors);
    degs.intersect (bufDegs);
    degs.refine();
    result= Union (result, smallFactors);
    if (degs.getLength() == 1 || bufUniFactors.length() == 1)
    {
      result.append (bufF);
      return result;
    }
    return Union (result, extHenselLiftAndLatticeRecombi (bufF, bufUniFactors,
                                                          info, degs, evaluation
                                                         )
                 );
  }

  if (l/degMipo < liftBound)
  {
    result=extIncreasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                 NTLN, evaluation, info2, source, dest
                                );

    if (result.length()== NTLN.NumCols())
    {
      delete [] bounds;
      result= Union (result, smallFactors);
      return result;
    }

    if (result.isEmpty())
    {
      result= extFurtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                     liftBound, d, bounds, NTLN,
                                                     diophant, M, Pi, bufQ,
                                                     evaluation, info2, source,
                                                     dest
                                                    );
      if (result.length()== NTLN.NumCols())
      {
        delete [] bounds;
        result= Union (result, smallFactors);
        return result;
      }
    }
  }

  DEBOUTLN (cerr, "lattice recombination failed");

  DegreePattern bufDegs= DegreePattern (bufUniFactors);
  degs.intersect (bufDegs);
  degs.refine();

  delete [] bounds;
  bounds= computeBounds (F, d);
  minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  if (minBound > 16 || result.length() == 0)
  {
    result= Union (result, smallFactors);
    CanonicalForm MODl= power (y, degree (F) + 1 + degree (LC (F, 1)));
    delete [] bounds;
    return Union (result, extFactorRecombination (bufUniFactors, F, MODl, info,
                                                  degs, evaluation, 1,
                                                  bufUniFactors.length()/2
                                                 )
                 );
  }
  else
  {
    result= Union (result, smallFactors);
    for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
      i.getItem()= mod (i.getItem(), y);
    delete [] bounds;
    return Union (result, extHenselLiftAndLatticeRecombi (F, bufUniFactors,
                                                          info, degs, evaluation
                                                         )
                 );
  }
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

  //check trivial case
  if (degree (A) == 1 || degree (A, 1) == 1)
  {
    factors.append (A);

    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                          false, false, N);

    normalize (factors);
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

  bool fail= false;
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
  int factorNums= 3;
  int subCheck1= substituteCheck (A, x);
  int subCheck2= substituteCheck (A, y);
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
      if (!derivXZero && !fail2)
      {
        bufEvaluation= bufEvaluation2;
        int dummy= subCheck2;
        subCheck2= subCheck1;
        subCheck1= dummy;
        A= buf;
        bufAeval= bufAeval2;
        swap2= true;
        fail= false;
      }
      else
        fail= true;
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
      if (subCheck1 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors);

        if (subCheck > 1 && (subCheck1%subCheck == 0))
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
      }

      if (!derivXZero && !fail2 && subCheck2 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors2);

        if (subCheck > 1 && (subCheck2%subCheck == 0))
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, y);
          factors= biFactorize (bufA, info);
          reverseSubst (factors, subCheck, y);
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
        if (!evaluation.isZero())
        {
          uniFactors= bufUniFactors;
          Aeval= bufAeval;
          evaluation= bufEvaluation;
        }
      }
    }
    list.append (bufEvaluation);
    if (!derivXZero && !fail2)
      list2.append (bufEvaluation2);
  }

  if (!derivXZero && !fail2)
  {
    if (!evaluation.isZero() && (uniFactors.length() > uniFactors2.length() ||
        (uniFactors.length() == uniFactors2.length()
         && degs.getLength() > degs2.getLength())))
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
      appendMapDown (factors, A, info, source, dest);
    }
    else
      factors.append (A);
    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                            swap, swap2, N);
    normalize (factors);
    return factors;
  }

  A= A (y + evaluation, y);

  int liftBound= degree (A, y) + 1 + degree (LC(A, x));

  int boundsLength;
  int * bounds= computeBounds (A (y - evaluation, y), boundsLength);
  int minBound= bounds[0];
  for (int i= 1; i < boundsLength; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  int degMipo= 1;
  if (extension && alpha.level() != 1 && k==1)
    degMipo= degree (getMipo (alpha));

  DEBOUTLN (cerr, "uniFactors= " << uniFactors);

  if ((GF && !extension) || (GF && extension && k != 1))
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_hensel_lift);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);

    CanonicalForm MODl= power (y, liftBound);

    if (extension)
      factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                       evaluation, 1, uniFactors.length()/2);
    else
      factors= factorRecombination (uniFactors, A, MODl, degs, 1,
                                    uniFactors.length()/2);

    if (earlySuccess)
      factors= Union (earlyFactors, factors);
    else if (!earlySuccess && degs.getLength() == 1)
      factors= earlyFactors;
  }
  else if (degree (A) > 4 && beta.level() == 1 && (2*minBound)/degMipo < 32)
  {
    TIMING_START (fac_hensel_lift);
    if (extension)
    {
      CFList lll= extHenselLiftAndLatticeRecombi (A, uniFactors, info, degs,
                                                  evaluation
                                                 );
      factors= Union (lll, factors);
    }
    else if (alpha.level() == 1 && !GF)
    {
      CFList lll= henselLiftAndLatticeRecombi (A, uniFactors, alpha, degs);
      factors= Union (lll, factors);
    }
    else if (!extension && (alpha != Variable (1) || GF))
    {
      CFList lll= henselLiftAndLatticeRecombi (A, uniFactors, alpha, degs);
      factors= Union (lll, factors);
    }
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);
  }
  else
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_hensel_lift);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_hensel_lift, "time for hensel lifting: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);

    CanonicalForm MODl= power (y, liftBound);
    if (!extension)
    {
      factors= factorRecombination (uniFactors, A, MODl, degs, 1, 3);

      int oldUniFactorsLength= uniFactors.length();
      if (degree (A) > 0)
      {
        CFList tmp;
        if (alpha.level() == 1)
          tmp= increasePrecision (A, uniFactors, 0, uniFactors.length(), 1,
                                  liftBound
                                 );
        else
        {
          if (degree (A) > getCharacteristic())
            tmp= increasePrecisionFq2Fp (A, uniFactors, 0, uniFactors.length(),
                                         1, alpha, liftBound
                                        );
          else
            tmp= increasePrecision (A, uniFactors, 0, uniFactors.length(), 1,
                                    alpha, liftBound
                                   );
        }
        factors= Union (factors, tmp);
        if (tmp.length() == 0 || (tmp.length() > 0 && uniFactors.length() != 0
                                  && uniFactors.length() != oldUniFactorsLength)
           )
        {
          DegreePattern bufDegs= DegreePattern (uniFactors);
          degs.intersect (bufDegs);
          degs.refine ();
          factors= Union (factors, factorRecombination (uniFactors, A, MODl,
                                                        degs, 4,
                                                        uniFactors.length()/2
                                                       )
                         );
        }
      }
    }
    else
    {
      if (beta.level() != 1 || k > 1)
      {
        if (k > 1)
        {
          factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                           evaluation, 1, uniFactors.length()/2
                                          );
        }
        else
        {
          factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                           evaluation, 1, 3
                                          );
          if (degree (A) > 0)
          {
            CFList tmp= increasePrecision2 (A, uniFactors, alpha, liftBound);
            DegreePattern bufDegs= DegreePattern (tmp);
            degs.intersect (bufDegs);
            degs.refine ();
            factors= Union (factors, extFactorRecombination (tmp, A, MODl, info,
                                                             degs, evaluation,
                                                             1, tmp.length()/2
                                                            )
                           );
          }
        }
      }
      else
      {
        factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                         evaluation, 1, 3
                                        );
        int oldUniFactorsLength= uniFactors.length();
        if (degree (A) > 0)
        {
          int degMipo;
          ExtensionInfo info2= init4ext (info, evaluation, degMipo);

          CFList source, dest;
          CFList tmp= extIncreasePrecision (A, uniFactors, 0,
                                            uniFactors.length(), 1, evaluation,
                                            info2, source, dest, liftBound
                                           );
          factors= Union (factors, tmp);
          if (tmp.length() == 0 || (tmp.length() > 0 && uniFactors.length() != 0
                                  && uniFactors.length() != oldUniFactorsLength)
             )
          {
            DegreePattern bufDegs= DegreePattern (uniFactors);
            degs.intersect (bufDegs);
            degs.refine ();
            factors= Union (factors,extFactorRecombination (uniFactors, A, MODl,
                                                        info, degs, evaluation,
                                                        4, uniFactors.length()/2
                                                            )
                           );
          }
        }
      }
    }

    if (earlySuccess)
      factors= Union (earlyFactors, factors);
    else if (!earlySuccess && degs.getLength() == 1)
      factors= earlyFactors;
  }
  delete [] bounds;
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
      ExtensionInfo info2= ExtensionInfo (extension);
      factors= biFactorize (A, info2);

      Variable vBuf= rootOf (gf_mipo);
      setCharacteristic (getCharacteristic());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
    }
    else // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (2, Variable (1));
      Variable v= rootOf (mipo);
      ExtensionInfo info2= ExtensionInfo (v);
      factors= biFactorize (A, info2);
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
      ExtensionInfo info2= ExtensionInfo (v);
      factors= biFactorize (A, info2);
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
          imPrimElem= mapPrimElem (primElem, alpha, v);

        CFList source, dest;
        CanonicalForm bufA= mapUp (A, alpha, v, primElem, imPrimElem,
                                   source, dest);
        ExtensionInfo info2= ExtensionInfo (v, alpha, imPrimElem, primElem);
        factors= biFactorize (bufA, info2);
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
        ExtensionInfo info2= ExtensionInfo (v, beta, imPrimElem, delta);
        factors= biFactorize (bufA, info2);
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
        ExtensionInfo info2= ExtensionInfo (extension);
        factors= biFactorize (A.mapinto(), info2);
      }
      else // not able to pass to another GF, pass to F_p(\alpha)
      {
        setCharacteristic (p);
        Variable vBuf= rootOf (gf_mipo);
        A= GF2FalphaRep (A, vBuf);
        Variable v= chooseExtension (vBuf, beta, k);
        ExtensionInfo info2= ExtensionInfo (v, extension);
        factors= biFactorize (A, info2);
      }
    }
    else // need factorization over GF (p^k)
    {
      if (ipower (p, 2*extensionDeg) < (1<<16))
      // pass to GF (p^2k)
      {
        setCharacteristic (p, 2*extensionDeg, 'Z');
        ExtensionInfo info2= ExtensionInfo (k, cGFName, extension);
        factors= biFactorize (GFMapUp (A, extensionDeg), info2);
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
        primElem= primitiveElement (v1, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElem= mapPrimElem (primElem, v1, v2);

        CFList source, dest;
        CanonicalForm bufA= mapUp (A, v1, v2, primElem, imPrimElem,
                                     source, dest);
        ExtensionInfo info2= ExtensionInfo (v2, v1, imPrimElem, primElem);
        factors= biFactorize (bufA, info2);
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


