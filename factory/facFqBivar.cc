/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqBivar.cc
 *
 * This file provides functions for factorizing a bivariate polynomial over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF, based on "Modern Computer
 * Algebra, Chapter 15" by J. von zur Gathen & J. Gerhard and "Factoring
 * multivariate polynomials over a finite field" by L. Bernardin.
 * Factor Recombination is described in "Factoring polynomials over global 
 * fields" by K. Belabas, M. van Hoeij, J. Klueners, A. Steel
 *
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "cf_defs.h"
#include "cf_map_ext.h"
#include "cf_random.h"
#include "facHensel.h"
#include "facMul.h"
#include "cf_map.h"
#include "cf_gcd_smallp.h"
#include "facFqBivarUtil.h"
#include "facFqBivar.h"
#include "cfNewtonPolygon.h"
#include "algext.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

TIMING_DEFINE_PRINT(fac_uni_factorizer)
TIMING_DEFINE_PRINT(fac_hensel_lift12)

CanonicalForm prodMod0 (const CFList& L, const CanonicalForm& M, const modpk& b)
{
  if (L.isEmpty())
    return 1;
  else if (L.length() == 1)
    return mod (L.getFirst()(0, 1) , M);
  else if (L.length() == 2)
    return mod (mulNTL (L.getFirst()(0, 1),L.getLast()(0, 1), b), M);
  else
  {
    int l= L.length()/2;
    CFListIterator i= L;
    CFList tmp1, tmp2;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod0 (tmp1, M, b);
    buf2= prodMod0 (tmp2, M, b);
    return mod (mulNTL (buf1,buf2, b), M);
  }
}

CanonicalForm evalPoint (const CanonicalForm& F, CanonicalForm & eval,
                         const Variable& alpha, CFList& list, const bool& GF,
                         bool& fail)
{
  fail= false;
  Variable x= Variable(2);
  Variable y= Variable(1);
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
    if (degree (eval) != degree (F, y))
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
  if (A.inCoeffDomain())
    return CFList();
  ASSERT (A.isUnivariate(),
          "univariate polynomial expected or constant expected");
  CFFList factorsA;
  ZZ p= to_ZZ (getCharacteristic());
  ZZ_p::init (p);
  if (GF)
  {
    int k= getGFDegree();
    char cGFName= gf_name;
    CanonicalForm mipo= gf_mipo;
    setCharacteristic (getCharacteristic());
    Variable beta= rootOf (mipo.mapinto());
    CanonicalForm buf= GF2FalphaRep (A, beta);
    if (getCharacteristic() > 2)
    {
      ZZ_pX NTLMipo= convertFacCF2NTLZZpX (mipo.mapinto());
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
      GF2X NTLMipo= convertFacCF2NTLGF2X (mipo.mapinto());
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
#ifdef HAVE_FLINT
    nmod_poly_t FLINTA;
    convertFacCF2nmod_poly_t (FLINTA, A);
    nmod_poly_factor_t result;
    nmod_poly_factor_init (result);
    mp_limb_t leadingCoeff= nmod_poly_factor (result, FLINTA);
    factorsA= convertFLINTnmod_poly_factor2FacCFFList (result, leadingCoeff, x);
    if (factorsA.getFirst().factor().inCoeffDomain())
      factorsA.removeFirst();
    nmod_poly_factor_clear (result);
    nmod_poly_clear (FLINTA);
#else
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
#endif
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
                        const CanonicalForm& N, const ExtensionInfo& info,
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

  CanonicalForm M= N;
  int l= degree (N);
  Variable y= F.mvar();
  Variable x= Variable (1);
  CFList source, dest;
  if (degs.getLength() <= 1 || factors.length() == 1)
  {
    CFList result= CFList(mapDown (F(y-eval, y), info, source, dest));
    F= 1;
    return result;
  }

  DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, M) == F " <<
            (mod (LC (F, 1)*prodMod (factors, M), M)/Lc (mod (LC (F, 1)*prodMod (factors, M), M)) == F/Lc (F)));
  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));

  CFList T, S, Diff;
  T= factors;

  CFList result;
  CanonicalForm buf, buf2, quot;

  buf= F;

  CanonicalForm g, LCBuf= LC (buf, x);
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
  CanonicalForm test;
  CanonicalForm buf0= buf (0, x)*LCBuf;
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
        test= prodMod0 (S, M);
        test *= LCBuf;
        test = mod (test, M);
        if (fdivides (test, buf0))
        {
          S.insert (LCBuf);
          g= prodMod (S, M);
          S.removeFirst();
          g /= content (g, x);
          if (fdivides (g, buf, quot))
          {
            buf2= g (y - eval, y);
            buf2 /= Lc (buf2);

            if (!k && beta.level() == 1)
            {
              if (degree (buf2, alpha) < degMipoBeta)
              {
                buf= quot;
                LCBuf= LC (buf, x);
                recombination= true;
                appendTestMapDown (result, buf2, info, source, dest);
                trueFactor= true;
              }
            }
            else
            {
              if (!isInExtension (buf2, gamma, k, delta, source, dest))
              {
                buf= quot;
                LCBuf= LC (buf, x);
                recombination= true;
                appendTestMapDown (result, buf2, info, source, dest);
                trueFactor= true;
              }
            }
            if (trueFactor)
            {
              T= Difference (T, S);
              l -= degree (g);
              M= power (y, l);
              buf0= buf (0, x)*LCBuf;

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
                     const CanonicalForm& N, DegreePattern& degs, int s,
                     int thres, const modpk& b
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
#ifdef DEBUGOUTPUT
  if (b.getp() == 0)
    DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, N) == F " <<
              (mod (LC (F, 1)*prodMod (factors, N),N)/Lc (mod (LC (F, 1)*prodMod (factors, N),N)) == F/Lc(F)));
  else
    DEBOUTLN (cerr, "LC (F, 1)*prodMod (factors, N) == F " <<
              (mod (b(LC (F, 1)*prodMod (factors, N)),N)/Lc (mod (b(LC (F, 1)*prodMod (factors, N)),N)) == F/Lc(F)));
#endif
  CFList T, S;

  CanonicalForm M= N;
  int l= degree (N);
  T= factors;
  CFList result;
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (F, x);
  CanonicalForm g, quot, buf= F;
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
  CanonicalForm test;
  bool isRat= (isOn (SW_RATIONAL) && getCharacteristic() == 0) || getCharacteristic() > 0;
  if (!isRat)
    On (SW_RATIONAL);
  CanonicalForm buf0= mulNTL (buf (0, x), LCBuf);
  if (!isRat)
    Off (SW_RATIONAL);
  buf0= buf(0,x)*LCBuf;
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
          if (b.getp() != 0)
            g= b(g);
          T.removeFirst();
          result.append (g/content (g, x));
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
        if (!isRat)
          On (SW_RATIONAL);
        test= prodMod0 (S, M);
        if (!isRat)
        {
          test *= bCommonDen (test);
          Off (SW_RATIONAL);
        }
        test= mulNTL (test, LCBuf, b);
        test= mod (test, M);
        if (uniFdivides (test, buf0))
        {
          if (!isRat)
            On (SW_RATIONAL);
          S.insert (LCBuf);
          g= prodMod (S, M);
          S.removeFirst();
          if (!isRat)
          {
            g *= bCommonDen(g);
            Off (SW_RATIONAL);
          }
          if (b.getp() != 0)
            g= b(g);
          if (!isRat)
            On (SW_RATIONAL);
          g /= content (g, x);
          if (fdivides (g, buf, quot))
          {
            recombination= true;
            result.append (g);
            buf= quot;
            LCBuf= LC (buf, x);
            T= Difference (T, S);
            l -= degree (g);
            M= power (y, l);
            buf0= mulNTL (buf (0, x), LCBuf);
            if (!isRat)
              Off (SW_RATIONAL);
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
          if (!isRat)
            Off (SW_RATIONAL);
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

void
earlyFactorDetection (CFList& reconstructedFactors, CanonicalForm& F, CFList&
                      factors, int& adaptedLiftBound, int*& factorsFoundIndex,
                      DegreePattern& degs, bool& success, int deg,
                      const modpk& b)
{
  DegreePattern bufDegs1= degs;
  DegreePattern bufDegs2;
  CFList T= factors;
  CanonicalForm buf= F;
  Variable x= Variable (1);
  CanonicalForm LCBuf= LC (buf, x);
  CanonicalForm g, quot;
  CanonicalForm M= power (F.mvar(), deg);
  adaptedLiftBound= 0;
  int d= degree (F), l= 0;
  bool isRat= (isOn (SW_RATIONAL) && getCharacteristic() == 0) || getCharacteristic() > 0;
  if (!isRat)
    On (SW_RATIONAL);
  CanonicalForm buf0= mulNTL (buf (0,x), LCBuf);
  CanonicalForm buf1= mulNTL (buf (1,x), LCBuf);
  if (!isRat)
    Off (SW_RATIONAL);
  CanonicalForm test0, test1;

  for (CFListIterator i= factors; i.hasItem(); i++, l++)
  {
    if (!bufDegs1.find (degree (i.getItem(), 1)) || factorsFoundIndex[l] == 1)
      continue;
    else
    {
      test1= mod (mulNTL (i.getItem() (1,x), LCBuf, b), M);
      if (uniFdivides (test1, buf1))
      {
        test0= mod (mulNTL (i.getItem() (0,x), LCBuf, b), M);
        if (uniFdivides (test0, buf0))
        {
          if (!isRat)
            On (SW_RATIONAL);
          g= mulMod2 (i.getItem(), LCBuf, M);
          if (!isRat)
          {
            g *= bCommonDen(g);
            Off (SW_RATIONAL);
          }
          if (b.getp() != 0)
            g= b(g);
          if (!isRat)
            On (SW_RATIONAL);
          g /= content (g, x);
          if (fdivides (g, buf, quot))
          {
            reconstructedFactors.append (g);
            factorsFoundIndex[l]= 1;
            buf= quot;
            d -= degree (g);
            LCBuf= LC (buf, x);
            buf0= mulNTL (buf (0,x), LCBuf);
            buf1= mulNTL (buf (1,x), LCBuf);
            if (!isRat)
              Off (SW_RATIONAL);
            T= Difference (T, CFList (i.getItem()));
            F= buf;

            // compute new possible degree pattern
            bufDegs2= DegreePattern (T);
            bufDegs1.intersect (bufDegs2);
            bufDegs1.refine ();
            if (bufDegs1.getLength() <= 1)
            {
              reconstructedFactors.append (buf);
              break;
            }
          }
          if (!isRat)
            Off (SW_RATIONAL);
        }
      }
    }
  }
  adaptedLiftBound= d + 1;
  if (adaptedLiftBound < deg)
  {
    degs= bufDegs1;
    success= true;
  }
  if (bufDegs1.getLength() <= 1)
    degs= bufDegs1;
}

void
extEarlyFactorDetection (CFList& reconstructedFactors, CanonicalForm& F, CFList&
                         factors,int& adaptedLiftBound, int*& factorsFoundIndex,
                         DegreePattern& degs, bool& success, const
                         ExtensionInfo& info, const CanonicalForm& eval, int deg
                        )
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
  Variable x= Variable (1);
  CanonicalForm buf= F, LCBuf= LC (buf, x), g, buf2;
  CanonicalForm M= power (y, deg);
  adaptedLiftBound= 0;
  bool trueFactor= false;
  int d= degree (F), l= 0;
  CFList source, dest;
  int degMipoBeta= 1;
  if (!k && beta.level() != 1)
    degMipoBeta= degree (getMipo (beta));
  CanonicalForm quot;
  for (CFListIterator i= factors; i.hasItem(); i++, l++)
  {
    if (!bufDegs1.find (degree (i.getItem(), 1)) || factorsFoundIndex[l] == 1)
      continue;
    else
    {
      g= mulMod2 (i.getItem(), LCBuf, M);
      g /= content (g, x);
      if (fdivides (g, buf, quot))
      {
        buf2= g (y - eval, y);
        buf2 /= Lc (buf2);

        if (!k && beta == x)
        {
          if (degree (buf2, alpha) < degMipoBeta)
          {
            appendTestMapDown (reconstructedFactors, buf2, info, source, dest);
            factorsFoundIndex[l]= 1;
            buf= quot;
            d -= degree (g);
            LCBuf= LC (buf, x);
            trueFactor= true;
          }
        }
        else
        {
          if (!isInExtension (buf2, gamma, k, delta, source, dest))
          {
            appendTestMapDown (reconstructedFactors, buf2, info, source, dest);
            factorsFoundIndex[l]= 1;
            buf= quot;
            d -= degree (g);
            LCBuf= LC (buf, x);
            trueFactor= true;
          }
        }
        if (trueFactor)
        {
          T= Difference (T, CFList (i.getItem()));
          F= buf;

          // compute new possible degree pattern
          bufDegs2= DegreePattern (T);
          bufDegs1.intersect (bufDegs2);
          bufDegs1.refine ();
          trueFactor= false;
          if (bufDegs1.getLength() <= 1)
          {
            buf= buf (y - eval, y);
            buf /= Lc (buf);
            appendMapDown (reconstructedFactors, buf, info, source, dest);
            break;
          }
        }
      }
    }
  }
  adaptedLiftBound= d + 1;
  if (adaptedLiftBound < deg)
  {
    degs= bufDegs1;
    success= true;
  }
  if (bufDegs1.getLength() <= 1)
    degs= bufDegs1;
}

int*
getCombinations (int * rightSide, int sizeOfRightSide, int& sizeOfOutput,
                 int degreeLC)
{
  Variable x= Variable (1);
  int p= getCharacteristic();
  int d= getGFDegree();
  char cGFName= gf_name;
  setCharacteristic(0);
  CanonicalForm buf= 1;
  for (int i= 0; i < sizeOfRightSide; i++)
    buf *= (power (x, rightSide [i]) + 1);

  int j= 0;
  for (CFIterator i= buf; i.hasTerms(); i++, j++)
  {
    if (i.exp() < degreeLC)
    {
      j++;
      break;
    }
  }

  ASSERT ( j > 1, "j > 1 expected" );

  int* result = new int  [j - 1];
  sizeOfOutput= j - 1;

  int i= 0;
  for (CFIterator m = buf; i < j - 1; i++, m++)
    result [i]= m.exp();

  if (d > 1)
    setCharacteristic (p, d, cGFName);
  else
    setCharacteristic (p);
  return result;
}

int *
getLiftPrecisions (const CanonicalForm& F, int& sizeOfOutput, int degreeLC)
{
  int sizeOfNewtonPoly;
  int ** newtonPolyg= newtonPolygon (F, sizeOfNewtonPoly);
  int sizeOfRightSide;
  int * rightSide= getRightSide(newtonPolyg, sizeOfNewtonPoly, sizeOfRightSide);
  int * result= getCombinations(rightSide, sizeOfRightSide, sizeOfOutput,
                                degreeLC);
  delete [] rightSide;
  for (int i= 0; i < sizeOfNewtonPoly; i++)
    delete [] newtonPolyg[i];
  delete [] newtonPolyg;
  return result;
}

void
deleteFactors (CFList& factors, int* factorsFoundIndex)
{
  CFList result;
  int i= 0;
  for (CFListIterator iter= factors; iter.hasItem(); iter++, i++)
  {
    if (factorsFoundIndex[i] == 1)
      continue;
    else
      result.append (iter.getItem());
  }
  factors= result;
}

CFList
henselLiftAndEarly (CanonicalForm& A, bool& earlySuccess, CFList&
                    earlyFactors, DegreePattern& degs, int& liftBound,
                    const CFList& uniFactors, const ExtensionInfo& info,
                    const CanonicalForm& eval, modpk& b)
{
  Variable alpha= info.getAlpha();
  Variable beta= info.getBeta();
  CanonicalForm gamma= info.getGamma();
  CanonicalForm delta= info.getDelta();
  bool extension= info.isInExtension();

  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (A, sizeOfLiftPre, degree (LC (A, 1), 2));

  Variable x= Variable (1);
  Variable y= Variable (2);
  CFArray Pi;
  CFList diophant;
  CFList bufUniFactors= uniFactors;
  CanonicalForm bufA= A;
  CanonicalForm lcA0= 0;
  bool mipoHasDen= false;
  if (getCharacteristic() == 0 && b.getp() != 0)
  {
    if (alpha.level() == 1)
    {
      lcA0= lc (A (0, 2));
      A *= b.inverse (lcA0);
      A= b (A);
      for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
        i.getItem()= b (i.getItem()*b.inverse (lc (i.getItem())));
    }
    else
    {
      lcA0= Lc (A (0,2));
      On (SW_RATIONAL);
      mipoHasDen= !bCommonDen(getMipo(alpha)).isOne();
      Off (SW_RATIONAL);
      CanonicalForm lcA0inverse= b.inverse (lcA0);
      A *= lcA0inverse;
      A= b (A);
      // Lc of bufUniFactors is in Z
      for (CFListIterator i= bufUniFactors; i.hasItem(); i++)
        i.getItem()= b (i.getItem()*b.inverse (lc (i.getItem())));
    }
  }
  bufUniFactors.insert (LC (A, x));
  CFMatrix M= CFMatrix (liftBound, bufUniFactors.length() - 1);
  earlySuccess= false;
  int newLiftBound= 0;

  int smallFactorDeg= tmin (11, liftPre [sizeOfLiftPre- 1] + 1);//this is a tunable parameter
  int dummy;
  int * factorsFoundIndex= new int [uniFactors.length()];
  for (int i= 0; i < uniFactors.length(); i++)
    factorsFoundIndex [i]= 0;

  CFList bufBufUniFactors;
  Variable v= alpha;
  if (smallFactorDeg >= liftBound || degree (A,y) <= 4)
    henselLift12 (A, bufUniFactors, liftBound, Pi, diophant, M, b, true);
  else if (sizeOfLiftPre > 1 && sizeOfLiftPre < 30)
  {
    henselLift12 (A, bufUniFactors, smallFactorDeg, Pi, diophant, M, b, true);
    if (mipoHasDen)
    {
      for (CFListIterator iter= bufUniFactors; iter.hasItem(); iter++)
        if (hasFirstAlgVar (iter.getItem(), v))
          break;
      if (v != alpha)
      {
        bufBufUniFactors= bufUniFactors;
        for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
          iter.getItem()= replacevar (iter.getItem(), v, alpha);
        A= replacevar (A, alpha, v);
      }
    }

    if (!extension)
    {
      if (v==alpha)
        earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                              factorsFoundIndex, degs, earlySuccess,
                              smallFactorDeg, b);
      else
        earlyFactorDetection(earlyFactors, bufA, bufBufUniFactors, newLiftBound,
                             factorsFoundIndex, degs, earlySuccess,
                             smallFactorDeg, b);
    }
    else
      extEarlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                               factorsFoundIndex, degs, earlySuccess, info,
                               eval, smallFactorDeg);
    if (degs.getLength() > 1 && !earlySuccess &&
        smallFactorDeg != liftPre [sizeOfLiftPre-1] + 1)
    {
      if (newLiftBound >= liftPre[sizeOfLiftPre-1]+1)
      {
        bufUniFactors.insert (LC (A, x));
        henselLiftResume12 (A, bufUniFactors, smallFactorDeg,
                            liftPre[sizeOfLiftPre-1] + 1, Pi, diophant, M, b);
        if (v!=alpha)
        {
          bufBufUniFactors= bufUniFactors;
          for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
            iter.getItem()= replacevar (iter.getItem(), v, alpha);
        }
        if (!extension)
        {
          if (v==alpha)
          earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[sizeOfLiftPre-1] + 1, b);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[sizeOfLiftPre-1] + 1, b);
        }
        else
          extEarlyFactorDetection (earlyFactors,bufA,bufUniFactors,newLiftBound,
                                   factorsFoundIndex, degs, earlySuccess, info,
                                   eval, liftPre[sizeOfLiftPre-1] + 1);
      }
    }
    else if (earlySuccess)
      liftBound= newLiftBound;

    int i= sizeOfLiftPre - 1;
    while (degs.getLength() > 1 && !earlySuccess && i - 1 >= 0)
    {
      if (newLiftBound >= liftPre[i] + 1)
      {
        bufUniFactors.insert (LC (A, x));
        henselLiftResume12 (A, bufUniFactors, liftPre[i] + 1,
                            liftPre[i-1] + 1, Pi, diophant, M, b);
        if (v!=alpha)
        {
          bufBufUniFactors= bufUniFactors;
          for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
            iter.getItem()= replacevar (iter.getItem(), v, alpha);
        }
        if (!extension)
        {
          if (v==alpha)
          earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[i-1] + 1, b);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[i-1] + 1, b);
        }
        else
          extEarlyFactorDetection (earlyFactors,bufA,bufUniFactors,newLiftBound,
                                   factorsFoundIndex, degs, earlySuccess, info,
                                   eval, liftPre[i-1] + 1);
      }
      else
      {
        liftBound= newLiftBound;
        break;
      }
      i--;
    }
    if (earlySuccess)
      liftBound= newLiftBound;
    //after here all factors are lifted to liftPre[sizeOfLiftPre-1]
  }
  else
  {
    henselLift12 (A, bufUniFactors, smallFactorDeg, Pi, diophant, M, b, true);
    if (mipoHasDen)
    {
      for (CFListIterator iter= bufUniFactors; iter.hasItem(); iter++)
        if (hasFirstAlgVar (iter.getItem(), v))
          break;
      if (v != alpha)
      {
        bufBufUniFactors= bufUniFactors;
        for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
          iter.getItem()= replacevar (iter.getItem(), v, alpha);
        A= replacevar (A, alpha, v);
      }
    }
    if (!extension)
    {
      if (v==alpha)
      earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                            factorsFoundIndex, degs, earlySuccess,
                            smallFactorDeg, b);
      else
      earlyFactorDetection (earlyFactors, bufA, bufBufUniFactors, newLiftBound,
                            factorsFoundIndex, degs, earlySuccess,
                            smallFactorDeg, b);
    }
    else
      extEarlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                               factorsFoundIndex, degs, earlySuccess, info,
                               eval, smallFactorDeg);
    int i= 1;
    while ((degree (A,y)/4)*i + 4 <= smallFactorDeg)
      i++;
    dummy= tmin (degree (A,y)+1, (degree (A,y)/4)*i+4);
    if (degs.getLength() > 1 && !earlySuccess && dummy > smallFactorDeg)
    {
      bufUniFactors.insert (LC (A, x));
      henselLiftResume12 (A, bufUniFactors, smallFactorDeg,
                          dummy, Pi, diophant, M, b);
      if (v!=alpha)
      {
        bufBufUniFactors= bufUniFactors;
        for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
          iter.getItem()= replacevar (iter.getItem(), v, alpha);
      }
      if (!extension)
      {
        if (v==alpha)
        earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                              factorsFoundIndex, degs, earlySuccess, dummy, b);
        else
        earlyFactorDetection (earlyFactors, bufA,bufBufUniFactors, newLiftBound,
                              factorsFoundIndex, degs, earlySuccess, dummy, b);
      }
      else
        extEarlyFactorDetection (earlyFactors, bufA,bufUniFactors, newLiftBound,
                                 factorsFoundIndex, degs, earlySuccess, info,
                                 eval, dummy);
    }
    while (degs.getLength() > 1 && !earlySuccess && i < 4)
    {
      if (newLiftBound >= dummy)
      {
        bufUniFactors.insert (LC (A, x));
        dummy= tmin (degree (A,y)+1, (degree (A,y)/4)*(i+1)+4);
        henselLiftResume12 (A, bufUniFactors, (degree (A,y)/4)*i + 4,
                            dummy, Pi, diophant, M, b);
        if (v!=alpha)
        {
          bufBufUniFactors= bufUniFactors;
          for (CFListIterator iter= bufBufUniFactors; iter.hasItem(); iter++)
            iter.getItem()= replacevar (iter.getItem(), v, alpha);
        }
        if (!extension)
        {
          if (v==alpha)
          earlyFactorDetection (earlyFactors, bufA, bufUniFactors, newLiftBound,
                                factorsFoundIndex, degs, earlySuccess, dummy,b);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess, dummy,b);
        }
        else
          extEarlyFactorDetection (earlyFactors,bufA,bufUniFactors,newLiftBound,
                                   factorsFoundIndex, degs, earlySuccess, info,
                                   eval, dummy);
      }
      else
      {
        liftBound= newLiftBound;
        break;
      }
      i++;
    }
    if (earlySuccess)
      liftBound= newLiftBound;
  }

  A= bufA;
  if (earlyFactors.length() > 0 && degs.getLength() > 1)
  {
    liftBound= degree (A,y) + 1;
    earlySuccess= true;
    deleteFactors (bufUniFactors, factorsFoundIndex);
  }

  delete [] factorsFoundIndex;
  delete [] liftPre;

  return bufUniFactors;
}

CFList
henselLiftAndEarly (CanonicalForm& A, bool& earlySuccess, CFList&
                    earlyFactors, DegreePattern& degs, int& liftBound,
                    const CFList& uniFactors, const ExtensionInfo& info,
                    const CanonicalForm& eval)
{
  modpk dummy= modpk();
  return henselLiftAndEarly (A, earlySuccess, earlyFactors, degs, liftBound,
                             uniFactors, info, eval, dummy);
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
  CanonicalForm quot, buf;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    iter= factors;
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
    if (fdivides (buf, F, quot))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      F= quot;
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
  CanonicalForm quot, buf;
  CanonicalForm yToL= power (y, liftBound);
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    iter= factors;
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
    if (fdivides (buf, F, quot))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      F= quot;
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
  CanonicalForm quot, buf;
  CFList result, factorsConsidered;
  CFList bufFactors= factors;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    iter= factors;
    buf= 1;
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
    if (fdivides (buf, F, quot))
    {
      F= quot;
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
  CanonicalForm quot, buf, buf2;
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    iter= factors;
    buf= 1;
    factorsConsidered= CFList();
    for (long j= 1; j <= N.NumRows(); j++, iter++)
    {
      if (!IsZero (N (j,i)))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf2= buf;
    buf *= LC (F, x);
    buf= mod (buf, yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F, quot))
    {
      F= quot;
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
  CanonicalForm buf2, quot, buf;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    iter= factors;
    buf= 1;
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
    if (!k && beta == x)
    {
      if (degree (buf2, alpha) < 1)
      {
        if (fdivides (buf, F, quot))
        {
          F= quot;
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
        if (fdivides (buf, F, quot))
        {
          F= quot;
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
  CanonicalForm quot, buf;
  CFList result;
  CFList bufFactors= factors;
  CFList factorsConsidered;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (zeroOneVecs [i - 1] == 0)
      continue;
    iter= factors;
    buf= 1;
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
    if (fdivides (buf, F, quot))
    {
      F= quot;
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
  CanonicalForm quot, buf, buf2;
  CFList source, dest;
  CFListIterator iter;
  for (long i= 1; i <= N.NumCols(); i++)
  {
    if (factorsFoundIndex [i - 1] == 1)
      continue;
    iter= factors;
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
    if (!k && beta == x)
    {
      if (degree (buf2, alpha) < 1)
      {
        if (fdivides (buf, F, quot))
        {
          factorsFoundIndex[i - 1]= 1;
          factorsFound++;
          F= quot;
          F /= Lc (F);
          buf2= mapDown (buf2, info, source, dest);
          reconstructedFactors.append (buf2);
        }
      }
    }
    else
    {
      if (!isInExtension (buf2, gamma, k, delta, source, dest))
      {
        if (fdivides (buf, F, quot))
        {
          factorsFoundIndex[i - 1]= 1;
          factorsFound++;
          F= quot;
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
  mat_zz_p NTLK, *NTLC;
  CFMatrix C;
  CFArray buf;
  CFListIterator j;
  CanonicalForm truncF;
  Variable y= F.mvar();
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
    j= factors;
    j++;

    truncF= mod (F, power (y, l));
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l - k, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN) && l > (minBound+1)*2)
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
  int startl= l;
  int oldL= l/2;
  bool reduced= false;
  Variable y= F.mvar();
  Variable x= Variable (1);
  CanonicalForm powX, imBasis, truncF;
  CFMatrix Mat, C;
  CFArray buf;
  CFIterator iter;
  mat_zz_p* NTLMat, *NTLC, NTLK;
  CFListIterator j;
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

    if (GF)
      setCharacteristic (getCharacteristic());

    powX= power (y-gamma, l);
    Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {
      imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);

    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    j= factors;
    j++;

    truncF= mod (F, power (y, l));
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }

    for (int i= 0; i < sizeBounds; i++)
    {
      if (bounds [i] + 1 <= (l/2)*degMipo)
      {
        wasInBounds= true;
        int k= tmin (bounds [i] + 1, (l/2)*degMipo);
        C= CFMatrix (l*degMipo - k, factors.length() - 1);

        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
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
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
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
        if (isReduced (NTLN) && l > startl)
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
  CFListIterator j;
  mat_zz_pE* NTLC, NTLK;
  CFArray buf;
  CFMatrix C;
  Variable y= F.mvar();
  CanonicalForm truncF;
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
    j= factors;
    j++;

    truncF= mod (F, power (y,l));
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (l == (minBound+1)*2)
      {
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      }
      else
      {
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
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
        C= CFMatrix (l - k, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {

          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }

        NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN) && l > (minBound+1)*2)
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
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  Variable y= F.mvar();
  CanonicalForm truncF;
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
    j= factors;
    j++;

    truncF= mod (F, power (y,l));
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (l == (minBound+1)*2)
      {
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      }
      else
      {
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
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
        C= CFMatrix ((l - k)*extensionDeg, factors.length() - 1);
        for (int ii= 0; ii < factors.length() - 1; ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k, alpha);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }

        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (NTLN.NumCols() == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (NTLN) && l > (minBound+1)*2)
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
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  Variable y= F.mvar();
  CanonicalForm truncF;
  while (l <= precision)
  {
    j= factors;
    truncF= mod (F, power (y,l));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
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
        if (result.length() == NTLN.NumCols())
        {
          delete [] factorsFoundIndex;
          delete [] A;
          delete [] bounds;
          F= 1;
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
                   int oldNumCols, int oldL, const Variable&,
                   int precision
                  )
{
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
  CFListIterator j;
  CFMatrix C;
  mat_zz_pE* NTLC, NTLK;
  CFArray buf;
  Variable y= F.mvar();
  CanonicalForm truncF;
  while (l <= precision)
  {
    j= factors;
    truncF= mod (F, power (y,l));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
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
          F= 1;
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
  CFListIterator j;
  Variable y= F.mvar();
  CanonicalForm powX, imBasis, truncF;
  CFMatrix Mat, C;
  CFIterator iter;
  mat_zz_p* NTLMat,*NTLC, NTLK;
  CFArray buf;
  while (l <= precision)
  {
    j= factors;
    if (GF)
      setCharacteristic (getCharacteristic());
    powX= power (y-gamma, l);
    Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {
      imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      iter= imBasis;
      for (; iter.hasTerms(); iter++)
          Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= (l/2)*degMipo)
      {
        int k= tmin (bounds [i] + 1, (l/2)*degMipo);
        C= CFMatrix (l*degMipo - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
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
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
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
        extReconstructionTry (result, bufF, factors,degree (F)+1, factorsFound2,
                              factorsFoundIndex, NTLN, false, info, evaluation
                             );
        if (result.length() == NTLN.NumCols())
        {
          delete [] factorsFoundIndex;
          delete [] A;
          delete [] bounds;
          F= 1;
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
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_pE* NTLC, NTLK;
  Variable y= F.mvar();
  CanonicalForm truncF;
  while (l <= precision)
  {
    j= factors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i], bufQ[i]);
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
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
  CFListIterator j;
  CFMatrix C;
  mat_zz_p* NTLC, NTLK;
  CFArray buf;
  Variable y= F.mvar();
  CanonicalForm truncF;
  while (l <= precision)
  {
    j= factors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    useOldQs= true;
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix ((l - k)*extensionDeg, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k, alpha);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
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
          F= 1;
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
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2;
  bool hitBound= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
  {
    ident (NTLN, factors.length());
    bufQ= CFArray (factors.length());
  }
  bool useOldQs= false;
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  CFList bufUniFactors;
  Variable y= F.mvar();
  while (oldL <= l)
  {
    j= factors;
    truncF= mod (F, power (y, oldL));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        C= CFMatrix (oldL - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          delete [] A;
          return CFList (F);
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      delete [] A;
      return CFList (F);
    }
    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    bufF= F;
    bufUniFactors= factors;
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
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2;
  bool hitBound= false;
  bool useOldQs= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_pE* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  CFList bufUniFactors;
  Variable y= F.mvar();
  while (oldL <= l)
  {
    j= factors;
    truncF= mod (F, power (y, oldL));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        C= CFMatrix (oldL - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          delete [] A;
          return CFList (F);
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      delete [] A;
      return CFList (F);
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    bufF= F;
    bufUniFactors= factors;
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
  Variable y= F.mvar();
  CFListIterator j;
  CanonicalForm powX, imBasis, bufF, truncF;
  CFMatrix Mat, C;
  CFIterator iter;
  mat_zz_p* NTLMat;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  CFList bufUniFactors;
  while (oldL <= l)
  {
    j= factors;
    if (GF)
      setCharacteristic (getCharacteristic());

    powX= power (y-gamma, oldL);
    Mat= CFMatrix (oldL*degMipo, oldL*degMipo);
    for (int i= 0; i < oldL*degMipo; i++)
    {
      imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    truncF= mod (F, power (y, oldL));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        C= CFMatrix (oldL*degMipo - k, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
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
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (NTLN.NumCols() == 1)
        {
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
      Variable y= Variable (2);
      CanonicalForm tmp= F (y - evaluation, y);
      CFList source, dest;
      tmp= mapDown (tmp, info, source, dest);
      delete [] A;
      return CFList (tmp);
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    bufF= F;
    bufUniFactors= factors;
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
  CFArray * A= new CFArray [factors.length()];
  int extensionDeg= degree (getMipo (alpha));
  int oldL2= oldL/2;
  bool hitBound= false;
  bool useOldQs= false;
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  CFList bufUniFactors;
  Variable y= F.mvar();
  while (oldL <= l)
  {
    j= factors;
    truncF= mod (F, power (y, oldL));
    if (useOldQs)
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, oldL2, bufQ[i],
                                     bufQ[i]
                                    );
    }
    else
    {
      for (int i= 0; i < factors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), oldL, bufQ [i]);
    }
    useOldQs= true;

    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= oldL/2)
      {
        int k= tmin (bounds [i] + 1, oldL/2);
        C= CFMatrix ((oldL - k)*extensionDeg, factors.length());
        for (int ii= 0; ii < factors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k, alpha);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        if (NTLN.NumCols() == 1)
        {
          delete [] A;
          return CFList (F);
        }
      }
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);

    bufF= F;
    bufUniFactors= factors;
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
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
  mat_zz_p* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  Variable y= F.mvar();
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    bufFactors.insert (LCF);
    bufFactors.removeFirst();
    j= bufFactors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
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
    bufF= F;
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
      bufF= F;
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
  CFListIterator j;
  CFArray buf;
  mat_zz_pE* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  Variable y= F.mvar();
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    j= bufFactors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        CFMatrix C= CFMatrix (l - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_pE(C);
        NTLK= (*NTLC)*NTLN;
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
    bufF= F;
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
      bufF= F;
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
  Variable y= F.mvar();
  CanonicalForm powX, imBasis, bufF, truncF;
  CFMatrix Mat, C;
  CFIterator iter;
  mat_zz_p* NTLMat,*NTLC, NTLK;
  CFListIterator j;
  CFArray buf;
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);

    if (GF)
      setCharacteristic (getCharacteristic());

    powX= power (y-gamma, l);
    Mat= CFMatrix (l*degMipo, l*degMipo);
    for (int i= 0; i < l*degMipo; i++)
    {

      imBasis= mod (power (y, i), powX);
      imBasis= imBasis (power (y, degMipo), y);
      imBasis= imBasis (y, gamma);
      iter= imBasis;
      for (; iter.hasTerms(); iter++)
        Mat (iter.exp()+ 1, i+1)= iter.coeff();
    }

    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);

    if (GF)
      setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

    j= bufFactors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix (l*degMipo - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
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
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
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
    bufF= F;
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
      bufF= F;
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
  CFListIterator j;
  CFMatrix C;
  mat_zz_p* NTLC, NTLK;
  CanonicalForm bufF, truncF;
  Variable y= F.mvar();
  while (l <= liftBound)
  {
    bufFactors.insert (LCF);
    henselLiftResume12 (F, bufFactors, oldL, l, Pi, diophant, M);
    j= bufFactors;
    truncF= mod (F, power (y, l));
    if (useOldQs)
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    else
    {
      for (int i= 0; i < bufFactors.length(); i++, j++)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ [i]);
    }
    for (int i= 0; i < d; i++)
    {
      if (bounds [i] + 1 <= l/2)
      {
        int k= tmin (bounds [i] + 1, l/2);
        C= CFMatrix ((l - k)*extensionDeg, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          CFArray buf;
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k, alpha);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
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
      bufF= F;
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
  CFListIterator iter;
  CanonicalForm buf;
  for (long i= 1; i <= NTLN.NumCols(); i++)
  {
    iter= factors;
    buf= 1;
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
  CFListIterator iter;
  CanonicalForm buf;
  for (long i= 1; i <= NTLN.NumCols(); i++)
  {
    iter= factors;
    buf= 1;
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
  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (F, sizeOfLiftPre, degree (LC (F, 1), 2));

  Variable y= F.mvar();
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= tmin (11, liftPre [sizeOfLiftPre- 1] + 1);
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
      delete [] liftPre;
      delete [] factorsFoundIndex;
      return result;
    }
  }

  int i= sizeOfLiftPre - 1;
  int dummy= 1;
  if (sizeOfLiftPre > 1 && sizeOfLiftPre < 30)
  {
    while (i > 0)
    {
      if (l < liftPre[i-1] + 1)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, beenInThres
                        );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i--;
    }
  }
  else
  {
    i= 1;
    while ((degree (F,y)/4)*i + 4 <= smallFactorDeg)
      i++;
    while (i < 4)
    {
      dummy= tmin (degree (F,y)+1, (degree (F,y)/4)*(i+1)+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        l= dummy;
      }
      else
      {
        i++;
        if (i < 4)
          continue;
      }
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, beenInThres
                        );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i++;
    }
  }

  delete [] liftPre;
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
  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (F, sizeOfLiftPre, degree (LC (F, 1), 2));
  Variable y= F.mvar();
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
      delete [] liftPre;
      delete [] factorsFoundIndex;
      return result;
    }
  }

  int i= sizeOfLiftPre - 1;
  int dummy= 1;
  if (sizeOfLiftPre > 1 && sizeOfLiftPre < 30)
  {
    while (i > 0)
    {
      if (l < liftPre[i-1] + 1)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, beenInThres
                        );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i--;
    }
  }
  else
  {
    i= 1;
    while ((degree (F,y)/4)*i + 4 <= smallFactorDeg)
      i++;
    while (i < 4)
    {
      dummy= tmin (degree (F,y)+1, (degree (F,y)/4)*(i+1)+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        l= dummy;
      }
      else
      {
        i++;
        if (i < 4)
          continue;
      }
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, beenInThres
                        );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i++;
    }
  }

  delete [] liftPre;
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
  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (F, sizeOfLiftPre, degree (LC (F, 1), 2));
  Variable y= F.mvar();
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
      delete [] liftPre;
      delete [] factorsFoundIndex;
      return result;
    }
  }

  int i= sizeOfLiftPre - 1;
  int dummy= 1;
  if (sizeOfLiftPre > 1 && sizeOfLiftPre < 30)
  {
    while (i > 0)
    {
      if (l < liftPre[i-1] + 1)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, NTLN, beenInThres, info,
                            evaluation
                           );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i--;
    }
  }
  else
  {
    i= 1;
    while ((degree (F,y)/4)*i + 4 <= smallFactorDeg)
      i++;
    while (i < 4)
    {
      dummy= tmin (degree (F,y)+1, (degree (F,y)/4)*(i+1)+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        l= dummy;
      }
      else
      {
        i++;
        if (i < 4)
          continue;
      }
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, NTLN, beenInThres, info,
                            evaluation
                           );
      if (result.length() == NTLN.NumCols())
      {
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i++;
    }
  }

  delete [] liftPre;
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
  int * factorsFoundIndex= new int [uniFactors.length()];
  for (int i= 0; i < uniFactors.length(); i++)
    factorsFoundIndex [i]= 0;
  CFList earlyFactors;
  earlyFactorDetection (earlyFactors, F, bufUniFactors, adaptedLiftBound,
                        factorsFoundIndex, degs, success, smallFactorDeg);
  delete [] factorsFoundIndex;
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
  int sizeOldF= size (G);
  if (size (F) < sizeOldF)
  {
    H= F;
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
  int * factorsFoundIndex= new int [uniFactors.length()];
  for (int i= 0; i < uniFactors.length(); i++)
    factorsFoundIndex [i]= 0;
  CFList earlyFactors;
  extEarlyFactorDetection (earlyFactors, F, bufUniFactors, adaptedLiftBound,
                           factorsFoundIndex, degs, success, info, evaluation,
                           smallFactorDeg);
  delete [] factorsFoundIndex;
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
  CanonicalForm shiftedF= G (y - evaluation, y);
  int sizeOldF= size (shiftedF);
  if (size (F) < sizeOldF)
  {
    H= F (y + evaluation, y); //shift back to zero
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
                                   success, minBound + 1
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
  CanonicalForm tmp1, tmp2;
  for (CFListIterator i= smallFactors; i.hasItem(); i++)
  {
    index= 1;
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
      oldL= liftAndComputeLattice (F, bounds, d, minBound + 1, liftBound,
                                   minBound, bufUniFactors, NTLN, diophant, M,
                                   Pi, bufQ, irreducible
                                  );
    else
    {
      if (reduceFq2Fp)
        oldL= liftAndComputeLatticeFq2Fp (F, bounds, d, minBound + 1,
                                          liftBound, minBound, bufUniFactors,
                                          NTLN, diophant, M, Pi, bufQ,
                                          irreducible, alpha
                                         );
      else
        oldL= liftAndComputeLattice (F, bounds, d, minBound + 1, liftBound,
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
      CFListIterator iter, iter2;
      CanonicalForm buf;
      CFList factorsConsidered;
      CanonicalForm tmp;
      for (int i= 0; i < NTLN.NumCols(); i++)
      {
        if (zeroOne [i] == 0)
          continue;
        iter= bufUniFactors;
        buf= 1;
        factorsConsidered= CFList();
        for (int j= 0; j < NTLN.NumRows(); j++, iter++)
        {
          if (!IsZero (NTLN (j + 1,i + 1)))
          {
            factorsConsidered.append (iter.getItem());
            buf *= mod (iter.getItem(), y);
          }
        }
        buf /= Lc (buf);
        for (iter2= result; iter2.hasItem(); iter2++)
        {
          tmp= mod (iter2.getItem(), y);
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
  CanonicalForm tmp1, tmp2;
  for (CFListIterator i= smallFactors; i.hasItem(); i++)
  {
    index= 1;
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
      CFListIterator iter, iter2;
      CanonicalForm buf;
      CFList factorsConsidered;
      for (int i= 0; i < NTLN.NumCols(); i++)
      {
        if (zeroOne [i] == 0)
          continue;
        iter= bufUniFactors;
        buf= 1;
        factorsConsidered= CFList();
        for (int j= 0; j < NTLN.NumRows(); j++, iter++)
        {
          if (!IsZero (NTLN (j + 1,i + 1)))
          {
            factorsConsidered.append (iter.getItem());
            buf *= mod (iter.getItem(), y);
          }
        }
        buf /= Lc (buf);
        for (iter2= result; iter2.hasItem(); iter2++)
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
  if (degree (A) == 1 || degree (A, 1) == 1 || 
      (size (A) == 2 && gcd (degree (A), degree (A,1)).isOne()))
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
              (prod (bufUniFactors)*Lc (bufAeval) == bufAeval));

    if (!derivXZero && !fail2)
    {
      TIMING_START (fac_uni_factorizer);
      bufUniFactors2= uniFactorizer (bufAeval2, alpha, GF);
      TIMING_END_AND_PRINT (fac_uni_factorizer,
                            "time for univariate factorization in y: ");
      DEBOUTLN (cerr, "Lc (bufAeval2)*prod (bufUniFactors2)== bufAeval2 " <<
                (prod (bufUniFactors2)*Lc (bufAeval2) == bufAeval2));
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
      appendMapDown (factors, A, info, source, dest);
    }
    else
      factors.append (A);
    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                            swap, swap2, N);
    normalize (factors);
    return factors;
  }

  int liftBound= degree (A, y) + 1;

  int boundsLength;
  int * bounds= computeBounds (A, boundsLength);
  int minBound= bounds[0];
  for (int i= 1; i < boundsLength; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  A= A (y + evaluation, y);

  int degMipo= 1;
  if (extension && alpha.level() != 1 && k==1)
    degMipo= degree (getMipo (alpha));

  DEBOUTLN (cerr, "uniFactors= " << uniFactors);

  if ((GF && !extension) || (GF && extension && k != 1))
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_hensel_lift12);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_hensel_lift12, "time for hensel lifting: ");
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
    TIMING_START (fac_hensel_lift12);
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
    else if (!extension && (alpha != x || GF))
    {
      CFList lll= henselLiftAndLatticeRecombi (A, uniFactors, alpha, degs);
      factors= Union (lll, factors);
    }
    TIMING_END_AND_PRINT (fac_hensel_lift12, "time for hensel lifting: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);
  }
  else
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_hensel_lift12);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_hensel_lift12, "time for hensel lifting: ");
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

      CanonicalForm mipo= gf_mipo;
      setCharacteristic (getCharacteristic());
      Variable vBuf= rootOf (mipo.mapinto());
      for (CFListIterator j= factors; j.hasItem(); j++)
        j.getItem()= GF2FalphaRep (j.getItem(), vBuf);
    }
    else // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (2, x);
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
      CanonicalForm mipo= randomIrredpoly (extDeg + 1, x);
      Variable v= rootOf (mipo);
      ExtensionInfo info2= ExtensionInfo (v);
      factors= biFactorize (A, info2);
    }
    else
    {
      if (beta == x)
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
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable vBuf= rootOf (mipo.mapinto());
        A= GF2FalphaRep (A, vBuf);
        setCharacteristic (p, extensionDeg, 'Z');
        ExtensionInfo info2= ExtensionInfo (extension);
        factors= biFactorize (A.mapinto(), info2);
      }
      else // not able to pass to another GF, pass to F_p(\alpha)
      {
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable vBuf= rootOf (mipo.mapinto());
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
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable v1= rootOf (mipo.mapinto());
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


