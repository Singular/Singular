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
#include "cf_irred.h"
#include "facFqBivarUtil.h"
#include "facFqBivar.h"
#include "cfNewtonPolygon.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

TIMING_DEFINE_PRINT(fac_fq_uni_factorizer)
TIMING_DEFINE_PRINT(fac_fq_bi_hensel_lift)
TIMING_DEFINE_PRINT(fac_fq_bi_factor_recombination)
TIMING_DEFINE_PRINT(fac_fq_bi_evaluation)
TIMING_DEFINE_PRINT(fac_fq_bi_shift_to_zero)
TIMING_DEFINE_PRINT(fac_fq_logarithmic)
TIMING_DEFINE_PRINT(fac_fq_compute_lattice_lift)
TIMING_DEFINE_PRINT(fac_fq_till_reduced)
TIMING_DEFINE_PRINT(fac_fq_reconstruction)
TIMING_DEFINE_PRINT(fac_fq_lift)
TIMING_DEFINE_PRINT(fac_fq_uni_total)

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

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
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
    bound= pow ((double) p, (double) d);
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
  if (GF)
  {
    int k= getGFDegree();
    char cGFName= gf_name;
    CanonicalForm mipo= gf_mipo;
    setCharacteristic (getCharacteristic());
    Variable beta= rootOf (mipo.mapinto());
    CanonicalForm buf= GF2FalphaRep (A, beta);
#ifdef HAVE_NTL    
    if (getCharacteristic() > 2)
#else
    if (getCharacteristic() > 0)
#endif
    {
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
      nmod_poly_t FLINTmipo, leadingCoeff;
      fq_nmod_ctx_t fq_con;
      fq_nmod_poly_t FLINTA;
      fq_nmod_poly_factor_t FLINTFactorsA;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      convertFacCF2nmod_poly_t (FLINTmipo, mipo.mapinto());

      fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

      convertFacCF2Fq_nmod_poly_t (FLINTA, buf, fq_con);
      fq_nmod_poly_make_monic (FLINTA, FLINTA, fq_con);

      fq_nmod_poly_factor_init (FLINTFactorsA, fq_con);
      nmod_poly_init (leadingCoeff, getCharacteristic());

      fq_nmod_poly_factor (FLINTFactorsA, leadingCoeff, FLINTA, fq_con);

      factorsA= convertFLINTFq_nmod_poly_factor2FacCFFList (FLINTFactorsA, x,
                                                            beta, fq_con);

      fq_nmod_poly_factor_clear (FLINTFactorsA, fq_con);
      fq_nmod_poly_clear (FLINTA, fq_con);
      nmod_poly_clear (FLINTmipo);
      nmod_poly_clear (leadingCoeff);
      fq_nmod_ctx_clear (fq_con);
#else
      if (fac_NTL_char != getCharacteristic())
      {
        fac_NTL_char= getCharacteristic();
        zz_p::init (getCharacteristic());
      }
      zz_pX NTLMipo= convertFacCF2NTLzzpX (mipo.mapinto());
      zz_pE::init (NTLMipo);
      zz_pEX NTLA= convertFacCF2NTLzz_pEX (buf, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_zz_pEX_long NTLFactorsA= CanZass (NTLA);
      zz_pE multi= to_zz_pE (1);
      factorsA= convertNTLvec_pair_zzpEX_long2FacCFFList (NTLFactorsA, multi,
                                                         x, beta);
#endif
    }
#ifdef HAVE_NTL    
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
#endif    
    setCharacteristic (getCharacteristic(), k, cGFName);
    for (CFFListIterator i= factorsA; i.hasItem(); i++)
    {
      buf= i.getItem().factor();
      buf= Falpha2GFRep (buf);
      i.getItem()= CFFactor (buf, i.getItem().exp());
    }
    prune (beta);
  }
  else if (alpha.level() != 1)
  {
#ifdef HAVE_NTL  
    if (getCharacteristic() > 2)
#else
    if (getCharacteristic() > 0)
#endif
    {
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
      nmod_poly_t FLINTmipo, leadingCoeff;
      fq_nmod_ctx_t fq_con;
      fq_nmod_poly_t FLINTA;
      fq_nmod_poly_factor_t FLINTFactorsA;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

      fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

      convertFacCF2Fq_nmod_poly_t (FLINTA, A, fq_con);
      fq_nmod_poly_make_monic (FLINTA, FLINTA, fq_con);

      fq_nmod_poly_factor_init (FLINTFactorsA, fq_con);
      nmod_poly_init (leadingCoeff, getCharacteristic());

      fq_nmod_poly_factor (FLINTFactorsA, leadingCoeff, FLINTA, fq_con);

      factorsA= convertFLINTFq_nmod_poly_factor2FacCFFList (FLINTFactorsA, x,
                                                            alpha, fq_con);

      fq_nmod_poly_factor_clear (FLINTFactorsA, fq_con);
      fq_nmod_poly_clear (FLINTA, fq_con);
      nmod_poly_clear (FLINTmipo);
      nmod_poly_clear (leadingCoeff);
      fq_nmod_ctx_clear (fq_con);
#else
      if (fac_NTL_char != getCharacteristic())
      {
        fac_NTL_char= getCharacteristic();
        zz_p::init (getCharacteristic());
      }
      zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
      zz_pE::init (NTLMipo);
      zz_pEX NTLA= convertFacCF2NTLzz_pEX (A, NTLMipo);
      MakeMonic (NTLA);
      vec_pair_zz_pEX_long NTLFactorsA= CanZass (NTLA);
      zz_pE multi= to_zz_pE (1);
      factorsA= convertNTLvec_pair_zzpEX_long2FacCFFList (NTLFactorsA, multi,
                                                           x, alpha);
#endif
    }
#ifdef HAVE_NTL
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
#endif    
  }
  else
  {
#ifdef HAVE_FLINT
#ifdef HAVE_NTL
    if (degree (A) < 300)
#endif    
    {
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
    }
#ifdef HAVE_NTL
    else
#endif
#endif /* HAVE_FLINT */
#ifdef HAVE_NTL
    if (getCharacteristic() > 2)
    {
      if (fac_NTL_char != getCharacteristic())
      {
        fac_NTL_char= getCharacteristic();
        zz_p::init (getCharacteristic());
      }
      zz_pX NTLA= convertFacCF2NTLzzpX (A);
      MakeMonic (NTLA);
      vec_pair_zz_pX_long NTLFactorsA= CanZass (NTLA);
      zz_p multi= to_zz_p (1);
      factorsA= convertNTLvec_pair_zzpX_long2FacCFFList (NTLFactorsA, multi,
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
  if (F.inCoeffDomain())
    return CFList();

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
                  buf= buf (y-eval,y);
                  buf /= Lc (buf);
                  appendTestMapDown (result, buf, info, source,
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
        buf= buf (y-eval,y);
        buf /= Lc (buf);
        appendTestMapDown (result, buf, info, source, dest);
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
                     const CanonicalForm& N, DegreePattern& degs, const
                     CanonicalForm& eval, int s, int thres, const modpk& b,
                     const CanonicalForm& den
                    )
{
  if (factors.length() == 0)
  {
    F= 1;
    return CFList ();
  }
  if (F.inCoeffDomain())
    return CFList();
  Variable y= Variable (2);
  if (degs.getLength() <= 1 || factors.length() == 1)
  {
    CFList result= CFList (F(y-eval,y));
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
  Variable x= Variable (1);
  CanonicalForm denom= den, denQuot;
  CanonicalForm LCBuf= LC (F, x)*denom;
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
  bool isRat= (isOn (SW_RATIONAL) && getCharacteristic() == 0) ||
               getCharacteristic() > 0;
  if (!isRat)
    On (SW_RATIONAL);
  CanonicalForm buf0= mulNTL (buf (0, x), LCBuf);
  if (!isRat)
    Off (SW_RATIONAL);
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
          g /= content (g,x);
          result.append (g(y-eval,y));
          F= 1;
          return result;
        }
        else
        {
          result= CFList (F(y-eval,y));
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
          if (!isRat)
          {
            On (SW_RATIONAL);
            if (!Lc (g).inBaseDomain())
              g /= Lc (g);
            g *= bCommonDen (g);
            Off (SW_RATIONAL);
            g /= icontent (g);
            On (SW_RATIONAL);
          }
          if (fdivides (g, buf, quot))
          {
            denom *= abs (lc (g));
            recombination= true;
            result.append (g (y-eval,y));
            if (b.getp() != 0)
            {
              denQuot= bCommonDen (quot);
              buf= quot*denQuot;
              Off (SW_RATIONAL);
              denom /= gcd (denom, denQuot);
              On (SW_RATIONAL);
            }
            else
              buf= quot;
            LCBuf= LC (buf, x)*denom;
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
                result.append (buf (y-eval,y));
                F= 1;
                return result;
              }
              else
              {
                result= CFList (F (y-eval,y));
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
        result.append (buf(y-eval,y));
        F= 1;
        return result;
      }
      else
      {
        result= CFList (F(y-eval,y));
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
    result.append (F(y-eval,y));
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
  #if defined(HAVE_FLINT)
  nmod_poly_t Irredpoly;
  nmod_poly_init(Irredpoly,getCharacteristic());
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLIrredpoly;
  #endif
  int i=1, m= 2;
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
  #if defined(HAVE_FLINT)
  nmod_poly_randtest_monic_irreducible(Irredpoly,FLINTrandom,i*m+1);
  CanonicalForm newMipo= convertnmod_poly_t2FacCF(Irredpoly,Variable (1));
  #elif defined(HAVE_NTL)
  BuildIrred (NTLIrredpoly, i*m);
  CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
  #endif
  return rootOf (newMipo);
}

void
earlyFactorDetection (CFList& reconstructedFactors, CanonicalForm& F, CFList&
                      factors, int& adaptedLiftBound, int*& factorsFoundIndex,
                      DegreePattern& degs, bool& success, int deg, const
                      CanonicalForm& eval, const modpk& b, CanonicalForm& den)
{
  DegreePattern bufDegs1= degs;
  DegreePattern bufDegs2;
  CFList T= factors;
  CanonicalForm buf= F;
  Variable x= Variable (1);
  Variable y= Variable (2);
  CanonicalForm g, quot;
  CanonicalForm M= power (F.mvar(), deg);
  adaptedLiftBound= 0;
  int d= degree (F), l= 0;
  bool isRat= (isOn (SW_RATIONAL) && getCharacteristic() == 0) ||
               getCharacteristic() > 0;
  if (!isRat)
    On (SW_RATIONAL);
  if (b.getp() != 0)
    buf *= bCommonDen (buf);
  CanonicalForm LCBuf= LC (buf, x)*den;
  CanonicalForm buf0= mulNTL (buf (0,x), LCBuf);
  CanonicalForm buf1= mulNTL (buf (1,x), LCBuf);
  if (!isRat)
    Off (SW_RATIONAL);
  CanonicalForm test0, test1;
  CanonicalForm denQuot;

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
          if (!isRat)
          {
            On (SW_RATIONAL);
            if (!Lc (g).inBaseDomain())
              g /= Lc (g);
            g *= bCommonDen (g);
            Off (SW_RATIONAL);
            g /= icontent (g);
            On (SW_RATIONAL);
          }
          if (fdivides (g, buf, quot))
          {
            den *= abs (lc (g));
            reconstructedFactors.append (g (y-eval,y));
            factorsFoundIndex[l]= 1;
            if (b.getp() != 0)
            {
              denQuot= bCommonDen (quot);
              buf= quot*denQuot;
              Off (SW_RATIONAL);
              den /= gcd (den, denQuot);
              On (SW_RATIONAL);
            }
            else
              buf= quot;
            d -= degree (g);
            LCBuf= LC (buf, x)*den;
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
              if (!buf.inCoeffDomain())
              {
                reconstructedFactors.append (buf (y-eval,y));
                F= 1;
              }
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
earlyFactorDetection (CFList& reconstructedFactors, CanonicalForm& F, CFList&
                      factors, int& adaptedLiftBound, int*& factorsFoundIndex,
                      DegreePattern& degs, bool& success, int deg, const
                      CanonicalForm& eval, const modpk& b)
{
  CanonicalForm den= 1;
  earlyFactorDetection (reconstructedFactors, F, factors, adaptedLiftBound,
                        factorsFoundIndex, degs, success, deg, eval, b, den);
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
            if (!buf.inCoeffDomain())
            {
              buf= buf (y - eval, y);
              buf /= Lc (buf);
              appendMapDown (reconstructedFactors, buf, info, source, dest);
              F= 1;
            }
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

#ifdef HAVE_NTL // henselLift12
CFList
henselLiftAndEarly (CanonicalForm& A, bool& earlySuccess, CFList&
                    earlyFactors, DegreePattern& degs, int& liftBound,
                    const CFList& uniFactors, const ExtensionInfo& info,
                    const CanonicalForm& eval,modpk& b, CanonicalForm& den)
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
  On (SW_RATIONAL);
  CanonicalForm bufA= A;
  if (!Lc (A).inBaseDomain())
  {
    bufA /= Lc (A);
    CanonicalForm denBufA= bCommonDen (bufA);
    bufA *= denBufA;
    Off (SW_RATIONAL);
    den /= gcd (den, denBufA);
  }
  else
  {
    bufA= A;
    Off (SW_RATIONAL);
    den /= gcd (den, Lc (A));
  }
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
                              smallFactorDeg, eval, b, den);
      else
        earlyFactorDetection(earlyFactors, bufA, bufBufUniFactors, newLiftBound,
                             factorsFoundIndex, degs, earlySuccess,
                             smallFactorDeg, eval, b, den);
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
                                liftPre[sizeOfLiftPre-1] + 1, eval, b, den);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[sizeOfLiftPre-1] + 1, eval, b, den);
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
                                liftPre[i-1] + 1, eval, b, den);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess,
                                liftPre[i-1] + 1, eval, b, den);
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
                            smallFactorDeg, eval, b, den);
      else
      earlyFactorDetection (earlyFactors, bufA, bufBufUniFactors, newLiftBound,
                            factorsFoundIndex, degs, earlySuccess,
                            smallFactorDeg, eval, b, den);
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
                              factorsFoundIndex, degs, earlySuccess, dummy,eval,
                              b, den);
        else
        earlyFactorDetection (earlyFactors, bufA,bufBufUniFactors, newLiftBound,
                              factorsFoundIndex, degs, earlySuccess, dummy,eval,
                              b, den);
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
                                factorsFoundIndex, degs, earlySuccess, dummy,
                                eval, b, den);
          else
          earlyFactorDetection (earlyFactors,bufA,bufBufUniFactors,newLiftBound,
                                factorsFoundIndex, degs, earlySuccess, dummy,
                                eval, b, den);
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
#endif

#ifdef HAVE_NTL // henselLiftAndEarly
CFList
henselLiftAndEarly (CanonicalForm& A, bool& earlySuccess, CFList&
                    earlyFactors, DegreePattern& degs, int& liftBound,
                    const CFList& uniFactors, const ExtensionInfo& info,
                    const CanonicalForm& eval)
{
  modpk dummy= modpk();
  CanonicalForm den= 1;
  return henselLiftAndEarly (A, earlySuccess, earlyFactors, degs, liftBound,
                             uniFactors, info, eval, dummy, den);
}
#endif

#ifndef HAVE_FLINT
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
#endif

#ifdef HAVE_FLINT
long isReduced (const nmod_mat_t M)
{
  long i, j, nonZero;
  for (i = 1; i <= nmod_mat_nrows(M); i++)
  {
    nonZero= 0;
    for (j = 1; j <= nmod_mat_ncols (M); j++)
    {
      if (!(nmod_mat_entry (M, i-1, j-1)==0))
        nonZero++;
    }
    if (nonZero != 1)
      return 0;
  }
  return 1;
}
#endif
  
#ifdef HAVE_NTL // mat_zz_pE
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
#endif

#ifndef HAVE_FLINT
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
#endif

#ifdef HAVE_FLINT
int * extractZeroOneVecs (const nmod_mat_t M)
{
  long i, j;
  bool nonZeroOne= false;
  int * result= new int [nmod_mat_ncols (M)];
  for (i = 0; i < nmod_mat_ncols (M); i++)
  {
    for (j = 0; j < nmod_mat_nrows (M); j++)
    {
      if (!((nmod_mat_entry (M, j, i) == 1) || (nmod_mat_entry (M, j,i) == 0)))
      {
        nonZeroOne= true;
        break;
      }
    }
    if (!nonZeroOne)
      result [i]= 1;
    else
      result [i]= 0;
    nonZeroOne= false;
  }
  return result;
}
#endif

#ifdef HAVE_NTL // mat_zz_pE
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
#endif

#ifdef HAVE_NTL // mat_zz_pE
void
reconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const CFList&
                   factors, const int liftBound, int& factorsFound, int*&
                   factorsFoundIndex, mat_zz_pE& N, const CanonicalForm& eval,
                   bool beenInThres
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm yToL= power (y, liftBound);
  CanonicalForm bufF= F (y-eval, y);
  if (factors.length() == 2)
  {
    CanonicalForm tmp1, tmp2, tmp3;
    tmp1= factors.getFirst();
    tmp2= factors.getLast();
    tmp1= mulMod2 (tmp1, LC (F,x), yToL);
    tmp1 /= content (tmp1, x);
    tmp1= tmp1 (y-eval, y);
    tmp2= mulMod2 (tmp2, LC (F,x), yToL);
    tmp2 /= content (tmp2, x);
    tmp2= tmp2 (y-eval, y);
    tmp3 = tmp1*tmp2;
    if (tmp3/Lc (tmp3) == bufF/Lc (bufF))
    {
      factorsFound++;
      F= 1;
      reconstructedFactors.append (tmp1);
      reconstructedFactors.append (tmp2);
      return;
    }
  }
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf= buf (y-eval,y);
    if (fdivides (buf, bufF, quot))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      bufF= quot;
      bufF /= Lc (bufF);
      reconstructedFactors.append (buf);
    }
    if (degree (bufF) <= 0)
      return;
    if (factorsFound + 1 == N.NumCols())
    {
      reconstructedFactors.append (bufF);
      F= 1;
      return;
    }
  }
  if (reconstructedFactors.length() != 0)
    F= bufF (y+eval,y);
}
#endif

#ifndef HAVE_FLINT
void
reconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const CFList&
                   factors, const int liftBound, int& factorsFound, int*&
                   factorsFoundIndex, mat_zz_p& N, const CanonicalForm& eval,
                   bool beenInThres
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm yToL= power (y, liftBound);
  CanonicalForm bufF= F (y-eval, y);
  if (factors.length() == 2)
  {
    CanonicalForm tmp1, tmp2, tmp3;
    tmp1= factors.getFirst();
    tmp2= factors.getLast();
    tmp1= mulMod2 (tmp1, LC (F,x), yToL);
    tmp1 /= content (tmp1, x);
    tmp1= tmp1 (y-eval, y);
    tmp2= mulMod2 (tmp2, LC (F,x), yToL);
    tmp2 /= content (tmp2, x);
    tmp2= tmp2 (y-eval,y);
    tmp3 = tmp1*tmp2;
    if (tmp3/Lc (tmp3) == bufF/Lc (bufF))
    {
      factorsFound++;
      F= 1;
      reconstructedFactors.append (tmp1);
      reconstructedFactors.append (tmp2);
      return;
    }
  }
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf= buf (y-eval,y);
    if (fdivides (buf, bufF, quot))
    {
      factorsFoundIndex[i - 1]= 1;
      factorsFound++;
      bufF= quot;
      bufF /= Lc (bufF);
      reconstructedFactors.append (buf);
    }
    if (degree (bufF) <= 0)
      return;
    if (factorsFound + 1 == N.NumCols())
    {
      reconstructedFactors.append (bufF);
      F=1;
      return;
    }
  }
  if (reconstructedFactors.length() != 0)
    F= bufF (y+eval,y);
}
#endif

#ifdef HAVE_FLINT
void
reconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const CFList&
                   factors, const int liftBound, int& factorsFound, int*&
                   factorsFoundIndex, nmod_mat_t N, const CanonicalForm& eval,
                   bool beenInThres
                  )
{
  Variable y= Variable (2);
  Variable x= Variable (1);
  CanonicalForm yToL= power (y, liftBound);
  CanonicalForm bufF= F (y-eval, y);
  if (factors.length() == 2)
  {
    CanonicalForm tmp1, tmp2, tmp3;
    tmp1= factors.getFirst();
    tmp2= factors.getLast();
    tmp1= mulMod2 (tmp1, LC (F,x), yToL);
    tmp1 /= content (tmp1, x);
    tmp1= tmp1 (y-eval, y);
    tmp2= mulMod2 (tmp2, LC (F,x), yToL);
    tmp2 /= content (tmp2, x);
    tmp2= tmp2 (y-eval, y);
    tmp3 = tmp1*tmp2;
    if (tmp3/Lc (tmp3) == bufF/Lc (bufF))
    {
      factorsFound++;
      F= 1;
      reconstructedFactors.append (tmp1);
      reconstructedFactors.append (tmp2);
      return;
    }
  }
  CanonicalForm quot, buf;
  CFListIterator iter;
  for (long i= 0; i < nmod_mat_ncols (N); i++)
  {
    if (factorsFoundIndex [i] == 1)
      continue;
    iter= factors;
    if (beenInThres)
    {
      int count= 0;
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
      for (long j= 0; j < nmod_mat_nrows (N); j++, iter++)
      {
        if (!(nmod_mat_entry (N, j, i) == 0))
          buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf= buf (y-eval,y);
    if (fdivides (buf, bufF, quot))
    {
      factorsFoundIndex[i]= 1;
      factorsFound++;
      bufF= quot;
      bufF /= Lc (bufF);
      reconstructedFactors.append (buf);
    }
    if (degree (F) <= 0)
      return;
    if (factorsFound + 1 == nmod_mat_ncols (N))
    {
      F= 1;
      reconstructedFactors.append (bufF);
      return;
    }
  }
  if (reconstructedFactors.length() != 0)
    F= bufF (y+eval,y);
}
#endif

#ifdef HAVE_NTL //mat_zz_pE
CFList
reconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs, int
                precision, const mat_zz_pE& N, const CanonicalForm& eval
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F, quot))
    {
      F= quot;
      F /= Lc (F);
      result.append (buf (y-eval,y));
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
#endif

#ifdef HAVE_NTL // mat_zz_pE
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
    buf= mulMod2 (buf, LC (F,x), yToL);
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
#endif

#ifndef HAVE_FLINT
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf2= buf (y-evaluation, y);
    buf2 /= Lc (buf2);
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
#endif

#ifdef HAVE_FLINT
CFList
extReconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs, int
                   precision, const nmod_mat_t N, const ExtensionInfo& info,
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
  for (long i= 0; i < nmod_mat_ncols(N); i++)
  {
    if (zeroOneVecs [i] == 0)
      continue;
    iter= factors;
    buf= 1;
    factorsConsidered= CFList();
    for (long j= 0; j < nmod_mat_nrows(N); j++, iter++)
    {
      if (!(nmod_mat_entry (N, j, i) == 0))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf2= buf (y-evaluation, y);
    buf2 /= Lc (buf2);
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
#endif

#ifndef HAVE_FLINT
CFList
reconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs,
                int precision, const mat_zz_p& N, const CanonicalForm& eval)
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F, quot))
    {
      F= quot;
      F /= Lc (F);
      result.append (buf (y-eval,y));
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
#endif

#ifdef HAVE_FLINT
CFList
reconstruction (CanonicalForm& G, CFList& factors, int* zeroOneVecs,
                int precision, const nmod_mat_t N, const CanonicalForm& eval)
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
  for (long i= 0; i < nmod_mat_ncols (N); i++)
  {
    if (zeroOneVecs [i] == 0)
      continue;
    iter= factors;
    buf= 1;
    factorsConsidered= CFList();
    for (long j= 0; j < nmod_mat_nrows (N); j++, iter++)
    {
      if (!(nmod_mat_entry (N, j, i) == 0))
      {
        factorsConsidered.append (iter.getItem());
        buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    if (fdivides (buf, F, quot))
    {
      F= quot;
      F /= Lc (F);
      result.append (buf (y-eval,y));
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
#endif

#ifndef HAVE_FLINT
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
  if (factors.length() == 2)
  {
    CanonicalForm tmp1, tmp2, tmp3;
    tmp1= factors.getFirst();
    tmp2= factors.getLast();
    tmp1= mulMod2 (tmp1, LC (F,x), yToL);
    tmp1 /= content (tmp1, x);
    tmp2= mulMod2 (tmp2, LC (F,x), yToL);
    tmp2 /= content (tmp2, x);
    tmp3 = tmp1*tmp2;
    if (tmp3/Lc (tmp3) == F/Lc (F))
    {
      tmp1= tmp1 (y - evaluation, y);
      tmp2= tmp2 (y - evaluation, y);
      tmp1 /= Lc (tmp1);
      tmp2 /= Lc (tmp2);
      if (!k && beta == x && degree (tmp2, alpha) < 1 &&
          degree (tmp1, alpha) < 1)
      {
        factorsFound++;
        F= 1;
        tmp1= mapDown (tmp1, info, source, dest);
        tmp2= mapDown (tmp2, info, source, dest);
        reconstructedFactors.append (tmp1);
        reconstructedFactors.append (tmp2);
        return;
      }
      else if (!isInExtension (tmp2, gamma, k, delta, source, dest) &&
               !isInExtension (tmp1, gamma, k, delta, source, dest))
      {
        factorsFound++;
        F= 1;
        tmp1= mapDown (tmp1, info, source, dest);
        tmp2= mapDown (tmp2, info, source, dest);
        reconstructedFactors.append (tmp1);
        reconstructedFactors.append (tmp2);
        return;
      }
    }
  }
  CanonicalForm quot, buf, buf2;
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
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf2= buf (y - evaluation, y);
    buf2 /= Lc (buf2);
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
#endif

#ifdef HAVE_FLINT
void
extReconstructionTry (CFList& reconstructedFactors, CanonicalForm& F, const
                      CFList& factors, const int liftBound, int& factorsFound,
                      int*& factorsFoundIndex, nmod_mat_t N, bool beenInThres,
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
  if (factors.length() == 2)
  {
    CanonicalForm tmp1, tmp2, tmp3;
    tmp1= factors.getFirst();
    tmp2= factors.getLast();
    tmp1= mulMod2 (tmp1, LC (F,x), yToL);
    tmp1 /= content (tmp1, x);
    tmp2= mulMod2 (tmp2, LC (F,x), yToL);
    tmp2 /= content (tmp2, x);
    tmp3 = tmp1*tmp2;
    if (tmp3/Lc (tmp3) == F/Lc (F))
    {
      tmp1= tmp1 (y - evaluation, y);
      tmp2= tmp2 (y - evaluation, y);
      tmp1 /= Lc (tmp1);
      tmp2 /= Lc (tmp2);
      if (!k && beta == x && degree (tmp2, alpha) < 1 &&
          degree (tmp1, alpha) < 1)
      {
        factorsFound++;
        F= 1;
        tmp1= mapDown (tmp1, info, source, dest);
        tmp2= mapDown (tmp2, info, source, dest);
        reconstructedFactors.append (tmp1);
        reconstructedFactors.append (tmp2);
        return;
      }
      else if (!isInExtension (tmp2, gamma, k, delta, source, dest) &&
               !isInExtension (tmp1, gamma, k, delta, source, dest))
      {
        factorsFound++;
        F= 1;
        tmp1= mapDown (tmp1, info, source, dest);
        tmp2= mapDown (tmp2, info, source, dest);
        reconstructedFactors.append (tmp1);
        reconstructedFactors.append (tmp2);
        return;
      }
    }
  }
  CanonicalForm quot, buf, buf2;
  CFListIterator iter;
  for (long i= 0; i < nmod_mat_ncols (N); i++)
  {
    if (factorsFoundIndex [i] == 1)
      continue;
    iter= factors;
    if (beenInThres)
    {
      int count= 0;
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
      for (long j= 0; j < nmod_mat_nrows (N); j++, iter++)
      {
        if (!(nmod_mat_entry (N, j, i) == 0))
          buf= mulMod2 (buf, iter.getItem(), yToL);
      }
    }
    buf= mulMod2 (buf, LC (F,x), yToL);
    buf /= content (buf, x);
    buf2= buf (y - evaluation, y);
    buf2 /= Lc (buf2);
    if (!k && beta == x)
    {
      if (degree (buf2, alpha) < 1)
      {
        if (fdivides (buf, F, quot))
        {
          factorsFoundIndex[i]= 1;
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
          factorsFoundIndex[i]= 1;
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
    if (factorsFound + 1 == nmod_mat_nrows (N))
    {
      CanonicalForm tmp= F (y - evaluation, y);
      tmp= mapDown (tmp, info, source, dest);
      reconstructedFactors.append (tmp);
      return;
    }
  }
}
#endif

#ifndef HAVE_FLINT
#ifdef HAVE_NTL // logarithmicDerivative
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
    TIMING_START (fac_fq_compute_lattice_lift);
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
    TIMING_END_AND_PRINT (fac_fq_compute_lattice_lift,
                          "time to lift in compute lattice: ");

    factors.insert (LCF);
    j= factors;
    j++;

    truncF= mod (F, power (y, l));
    TIMING_START (fac_fq_logarithmic);
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    TIMING_END_AND_PRINT (fac_fq_logarithmic,
                          "time to compute logarithmic derivative: ");

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
        delete NTLC;

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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif
#endif

#ifdef HAVE_FLINT
#ifdef HAVE_NTL // henselLift12
int
liftAndComputeLattice (const CanonicalForm& F, int* bounds, int sizeBounds, int
                       start, int liftBound, int minBound, CFList& factors,
                       nmod_mat_t FLINTN, CFList& diophant, CFMatrix& M,CFArray&
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
  long rank;
  nmod_mat_t FLINTK, FLINTC, null;
  CFMatrix C;
  CFArray buf;
  CFListIterator j;
  CanonicalForm truncF;
  Variable y= F.mvar();
  while (l <= liftBound)
  {
    TIMING_START (fac_fq_compute_lattice_lift);
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
    TIMING_END_AND_PRINT (fac_fq_compute_lattice_lift,
                          "time to lift in compute lattice: ");

    factors.insert (LCF);
    j= factors;
    j++;

    truncF= mod (F, power (y, l));
    TIMING_START (fac_fq_logarithmic);
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    TIMING_END_AND_PRINT (fac_fq_logarithmic,
                          "time to compute logarithmic derivative: ");

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

        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
        if (nmod_mat_ncols (FLINTN) == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (FLINTN) && l > (minBound+1)*2)
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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif
#endif

#ifndef HAVE_FLINT
#ifdef HAVE_NTL //logarithmicDerivative
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
    TIMING_START (fac_fq_compute_lattice_lift);
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
    TIMING_END_AND_PRINT (fac_fq_compute_lattice_lift,
                          "time to lift in compute lattice: ");

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
    TIMING_START (fac_fq_logarithmic);
    for (int i= 0; i < factors.length() - 1; i++, j++)
    {
      if (!wasInBounds)
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, bufQ[i]);
      else
        A[i]= logarithmicDerivative (truncF, j.getItem(), l, oldL, bufQ[i],
                                     bufQ[i]);
    }
    TIMING_END_AND_PRINT (fac_fq_logarithmic,
                          "time to compute logarithmic derivative: ");

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
        delete NTLC;

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

    delete NTLMat;

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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif
#endif

#ifdef HAVE_FLINT
#ifdef HAVE_NTL // henselLift12
//over field extension
int
extLiftAndComputeLattice (const CanonicalForm& F, int* bounds, int sizeBounds,
                          int liftBound, int minBound, int start, CFList&
                          factors, nmod_mat_t FLINTN, CFList& diophant,
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
  Variable y= F.mvar();
  Variable x= Variable (1);
  CanonicalForm powX, imBasis, truncF;
  CFMatrix Mat, C;
  CFArray buf;
  CFIterator iter;
  long rank;
  nmod_mat_t FLINTMat, FLINTMatInv, FLINTC, FLINTK, null;
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

    convertFacCFMatrix2nmod_mat_t (FLINTMat, Mat);
    nmod_mat_init (FLINTMatInv, nmod_mat_nrows (FLINTMat),
                   nmod_mat_nrows (FLINTMat), getCharacteristic());
    nmod_mat_inv (FLINTMatInv, FLINTMat);

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
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
            }
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

        if (nmod_mat_ncols (FLINTN) == 1)
        {
          irreducible= true;
          break;
        }
        if (isReduced (FLINTN))
        {
          reduced= true;
          break;
        }
      }
    }

    nmod_mat_clear (FLINTMat);
    nmod_mat_clear (FLINTMatInv);

    if (nmod_mat_ncols (FLINTN) == 1)
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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif
#endif

// over Fq
#ifdef HAVE_NTL
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
    TIMING_START (fac_fq_compute_lattice_lift);
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
    TIMING_END_AND_PRINT (fac_fq_compute_lattice_lift,
                          "time to lift in compute lattice: ");

    factors.insert (LCF);
    j= factors;
    j++;

    truncF= mod (F, power (y,l));
    TIMING_START (fac_fq_logarithmic);
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
    TIMING_END_AND_PRINT (fac_fq_logarithmic,
                          "time to compute logarithmic derivative: ");

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
        delete NTLC;

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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif

#ifdef HAVE_NTL // henselLift12
#ifdef HAVE_FLINT
int
liftAndComputeLatticeFq2Fp (const CanonicalForm& F, int* bounds, int sizeBounds,
                            int start, int liftBound, int minBound, CFList&
                            factors, nmod_mat_t FLINTN, CFList& diophant,
                            CFMatrix& M, CFArray& Pi, CFArray& bufQ, bool&
                            irreducible, const Variable& alpha
                           )
#else
int
liftAndComputeLatticeFq2Fp (const CanonicalForm& F, int* bounds, int sizeBounds,
                            int start, int liftBound, int minBound, CFList&
                            factors, mat_zz_p& NTLN, CFList& diophant, CFMatrix&
                            M, CFArray& Pi, CFArray& bufQ, bool& irreducible,
                            const Variable& alpha
                           )
#endif
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
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
  Variable y= F.mvar();
  CanonicalForm truncF;
  while (l <= liftBound)
  {
    TIMING_START (fac_fq_compute_lattice_lift);
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
    TIMING_END_AND_PRINT (fac_fq_compute_lattice_lift,
                          "time to lift in compute lattice: ");

    factors.insert (LCF);
    j= factors;
    j++;

    truncF= mod (F, power (y,l));
    TIMING_START (fac_fq_logarithmic);
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
    TIMING_END_AND_PRINT (fac_fq_logarithmic,
                          "time to compute logarithmic derivative: ");

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

#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif

#ifdef HAVE_FLINT
        if (nmod_mat_nrows (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          irreducible= true;
          break;
        }
#ifdef HAVE_FLINT
        if (isReduced (FLINTN) && l > (minBound+1)*2)
#else
        if (isReduced (NTLN) && l > (minBound+1)*2)
#endif
        {
          reduced= true;
          break;
        }
      }
    }

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    if (NTLN.NumCols() == 1)
#endif
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
  if (!wasInBounds)
  {
    if (start)
      henselLiftResume12 (F, factors, start, degree (F) + 1, Pi, diophant, M);
    else
      henselLift12 (F, factors, degree (F) + 1, Pi, diophant, M);
    factors.insert (LCF);
  }
  return l;
}
#endif

#ifdef HAVE_NTL // logarithmicDerivative
CFList
increasePrecision (CanonicalForm& F, CFList& factors, int factorsFound,
                   int oldNumCols, int oldL, int precision,
                   const CanonicalForm& eval
                  )
{
  int d;
  bool isIrreducible= false;
  int* bounds= computeBounds (F, d, isIrreducible);
  Variable y= F.mvar();
  if (isIrreducible)
  {
    delete [] bounds;
    CanonicalForm G= F;
    F= 1;
    return CFList (G (y-eval, y));
  }
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init (FLINTN,factors.length(),factors.length(), getCharacteristic());
  for (long i=factors.length()-1; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
#endif
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
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
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
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
        {
          nmod_mat_clear (FLINTN);
#else
        if (NTLN.NumCols() == 1)
        {
#endif
          delete [] A;
          delete [] bounds;
          CanonicalForm G= F;
          F= 1;
          return CFList (G (y-eval,y));
        }
      }
    }

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) < oldNumCols - factorsFound)
    {
      if (isReduced (FLINTN))
      {
        int * factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
        for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
#endif
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
#ifdef HAVE_FLINT
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, FLINTN, eval, false
                          );
        if (result.length() == nmod_mat_ncols (FLINTN))
        {
          nmod_mat_clear (FLINTN);
#else
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, NTLN, eval, false
                          );
        if (result.length() == NTLN.NumCols())
        {
#endif
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
#ifdef HAVE_FLINT
        int * zeroOne= extractZeroOneVecs (FLINTN);
        CFList result= reconstruction (bufF,factors,zeroOne,precision,FLINTN, eval);
        nmod_mat_clear (FLINTN);
#else
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN, eval);
#endif
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
#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif
  delete [] bounds;
  delete [] A;
  return CFList();
}
#endif

#ifdef HAVE_NTL // mat_zz_pE
CFList
increasePrecision (CanonicalForm& F, CFList& factors, int factorsFound,
                   int oldNumCols, int oldL, const Variable&,
                   int precision, const CanonicalForm& eval
                  )
{
  int d;
  bool isIrreducible= false;
  Variable y= F.mvar();
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    CanonicalForm G= F;
    F= 1;
    return CFList (G (y-eval,y));
  }
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
        delete NTLC;
        if (NTLN.NumCols() == 1)
        {
          delete [] A;
          delete [] bounds;
          CanonicalForm G= F;
          F= 1;
          return CFList (G (y-eval,y));
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
                           factorsFoundIndex, NTLN, eval, false);
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
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN, eval);
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
#endif

#ifdef HAVE_NTL // logarithmicDerivative
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
  bool isIrreducible= false;
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    Variable y= Variable (2);
    CanonicalForm tmp= F (y - evaluation, y);
    CFList source, dest;
    tmp= mapDown (tmp, info, source, dest);
    F= 1;
    return CFList (tmp);
  }

  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init (FLINTN,factors.length(),factors.length(), getCharacteristic());
  for (long i=factors.length()-1; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
#endif
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
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTMat, FLINTMatInv, FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLMat,*NTLC, NTLK;
#endif
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

#ifdef HAVE_FLINT
    convertFacCFMatrix2nmod_mat_t (FLINTMat, Mat);
    nmod_mat_init (FLINTMatInv, nmod_mat_nrows (FLINTMat),
                   nmod_mat_nrows (FLINTMat), getCharacteristic());
    nmod_mat_inv (FLINTMatInv, FLINTMat);
#else
    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
#endif

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
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
#endif
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
#endif
            }
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
        {
          nmod_mat_clear (FLINTMat);
          nmod_mat_clear (FLINTMatInv);
          nmod_mat_clear (FLINTN);
#else
        if (NTLN.NumCols() == 1)
        {
          delete NTLMat;
#endif
          Variable y= Variable (2);
          CanonicalForm tmp= F (y - evaluation, y);
          CFList source, dest;
          tmp= mapDown (tmp, info, source, dest);
          delete [] A;
          delete [] bounds;
          F= 1;
          return CFList (tmp);
        }
      }
    }

#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTMat);
    nmod_mat_clear (FLINTMatInv);
#else
    delete NTLMat;
#endif

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) < oldNumCols - factorsFound)
    {
      if (isReduced (FLINTN))
      {
        int * factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
        for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
#endif
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
#ifdef HAVE_FLINT
        extReconstructionTry (result, bufF, factors,degree (F)+1, factorsFound2,
                              factorsFoundIndex, FLINTN, false, info, evaluation
                             );
        if (result.length() == nmod_mat_ncols (FLINTN))
        {
          nmod_mat_clear (FLINTN);
#else
        extReconstructionTry (result, bufF, factors,degree (F)+1, factorsFound2,
                              factorsFoundIndex, NTLN, false, info, evaluation
                             );
        if (result.length() == NTLN.NumCols())
        {
#endif
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
#ifdef HAVE_FLINT
        int * zeroOne= extractZeroOneVecs (FLINTN);
        CFList result= extReconstruction (bufF, factors, zeroOne, precision,
                                          FLINTN, info, evaluation
                                         );
        nmod_mat_clear (FLINTN);
#else
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= extReconstruction (bufF, factors, zeroOne, precision,
                                          NTLN, info, evaluation
                                         );
#endif
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

#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif
  delete [] bounds;
  delete [] A;
  return CFList();
}
#endif

#ifdef HAVE_NTL // mat_zz_pE
CFList
increasePrecision2 (const CanonicalForm& F, CFList& factors,
                    const Variable& alpha, int precision)
{
  int d;
  bool isIrreducible= false;
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    return CFList (F);
  }
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
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
        delete NTLC;

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
      delete [] zeroOne;
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
#endif

#ifdef HAVE_NTL // logarithmicDerivative
CFList
increasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int factorsFound,
                        int oldNumCols, int oldL, const Variable& alpha,
                        int precision, const CanonicalForm& eval
                       )
{
  int d;
  bool isIrreducible= false;
  Variable y= F.mvar();
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    CanonicalForm G= F;
    F= 1;
    return CFList (G (y-eval,y));
  }
  int extensionDeg= degree (getMipo (alpha));
  CFArray * A= new CFArray [factors.length()];
  CFArray bufQ= CFArray (factors.length());
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init (FLINTN,factors.length(),factors.length(), getCharacteristic());
  for (long i=factors.length()-1; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  mat_zz_p NTLN;
  ident (NTLN, factors.length());
#endif
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
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
  CFArray buf;
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
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
        {
          nmod_mat_clear (FLINTN);
#else
        if (NTLN.NumCols() == 1)
        {
#endif
          delete [] A;
          delete [] bounds;
          CanonicalForm G= F;
          F= 1;
          return CFList (G (y-eval,y));
        }
      }
    }

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) < oldNumCols - factorsFound)
    {
      if (isReduced (FLINTN))
      {
        int * factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
        for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
    if (NTLN.NumCols() < oldNumCols - factorsFound)
    {
      if (isReduced (NTLN))
      {
        int * factorsFoundIndex= new int [NTLN.NumCols()];
        for (long i= 0; i < NTLN.NumCols(); i++)
#endif
          factorsFoundIndex[i]= 0;
        int factorsFound2= 0;
        CFList result;
        CanonicalForm bufF= F;
#ifdef HAVE_FLINT
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, FLINTN, eval, false
                          );
        if (result.length() == nmod_mat_ncols (FLINTN))
        {
          nmod_mat_clear (FLINTN);
#else
        reconstructionTry (result, bufF, factors, degree (F) + 1, factorsFound2,
                           factorsFoundIndex, NTLN, eval, false
                          );
        if (result.length() == NTLN.NumCols())
        {
#endif
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
#ifdef HAVE_FLINT
        int * zeroOne= extractZeroOneVecs (FLINTN);
        CFList result= reconstruction (bufF,factors,zeroOne,precision,FLINTN, eval);
        nmod_mat_clear (FLINTN);
#else
        int * zeroOne= extractZeroOneVecs (NTLN);
        CFList result= reconstruction (bufF, factors, zeroOne, precision, NTLN, eval);
#endif
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
#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif
  delete [] bounds;
  delete [] A;
  return CFList();
}
#endif

#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
increasePrecision (CanonicalForm& F, CFList& factors, int oldL, int
                   l, int d, int* bounds, CFArray& bufQ, nmod_mat_t FLINTN,
                   const CanonicalForm& eval
                  )
#else
CFList
increasePrecision (CanonicalForm& F, CFList& factors, int oldL, int
                   l, int d, int* bounds, CFArray& bufQ, mat_zz_p& NTLN,
                   const CanonicalForm& eval
                  )
#endif
{
  CFList result= CFList();
  CFArray * A= new CFArray [factors.length()];
  int oldL2= oldL/2;
  bool hitBound= false;
#ifdef HAVE_FLINT
  if (nmod_mat_nrows (FLINTN) != factors.length()) //refined factors
  {
    nmod_mat_clear (FLINTN);
    nmod_mat_init(FLINTN,factors.length(),factors.length(),getCharacteristic());
    for (long i=factors.length()-1; i >= 0; i--)
      nmod_mat_entry (FLINTN, i, i)= 1;
    bufQ= CFArray (factors.length());
  }
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
  {
    ident (NTLN, factors.length());
    bufQ= CFArray (factors.length());
  }
#endif
  bool useOldQs= false;
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
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
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          delete [] A;
          return CFList (F (y-eval,y));
        }
      }
    }
#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    if (NTLN.NumCols() == 1)
#endif
    {
      delete [] A;
      return CFList (F (y-eval,y));
    }
    int * zeroOneVecs;
#ifdef HAVE_FLINT
    zeroOneVecs= extractZeroOneVecs (FLINTN);
#else
    zeroOneVecs= extractZeroOneVecs (NTLN);
#endif
    bufF= F;
    bufUniFactors= factors;
#ifdef HAVE_FLINT
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, FLINTN, eval);
#else
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN, eval);
#endif
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
#endif

#ifdef HAVE_NTL // mat_zz_pE
CFList
increasePrecision (CanonicalForm& F, CFList& factors, int oldL, int
                   l, int d, int* bounds, CFArray& bufQ, mat_zz_pE& NTLN,
                   const CanonicalForm& eval
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
        delete NTLC;

        if (NTLN.NumCols() == 1)
        {
          delete [] A;
          return CFList (F (y-eval,y));
        }
      }
    }
    if (NTLN.NumCols() == 1)
    {
      delete [] A;
      return CFList (F (y-eval,y));
    }

    int * zeroOneVecs;
    zeroOneVecs= extractZeroOneVecs (NTLN);
    bufF= F;
    bufUniFactors= factors;
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN, eval);
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
#endif

//over field extension
#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
extIncreasePrecision (CanonicalForm& F, CFList& factors, int oldL, int l, int d,
                      int* bounds, CFArray& bufQ, nmod_mat_t FLINTN, const
                      CanonicalForm& evaluation, const ExtensionInfo& info,
                      CFList& source, CFList& dest
                     )
#else
CFList
extIncreasePrecision (CanonicalForm& F, CFList& factors, int oldL, int l, int d,
                      int* bounds, CFArray& bufQ, mat_zz_p& NTLN, const
                      CanonicalForm& evaluation, const ExtensionInfo& info,
                      CFList& source, CFList& dest
                     )
#endif
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
#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
  nmod_mat_init (FLINTN,factors.length(),factors.length(), getCharacteristic());
  for (long i=factors.length()-1; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
#endif
  Variable y= F.mvar();
  CFListIterator j;
  CanonicalForm powX, imBasis, bufF, truncF;
  CFMatrix Mat, C;
  CFIterator iter;
  CFArray buf;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTMat, FLINTMatInv, FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK, *NTLMat;
#endif
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

#ifdef HAVE_FLINT
    convertFacCFMatrix2nmod_mat_t (FLINTMat, Mat);
    nmod_mat_init (FLINTMatInv, nmod_mat_nrows (FLINTMat),
                   nmod_mat_nrows (FLINTMat), getCharacteristic());
    nmod_mat_inv (FLINTMatInv, FLINTMat);
#else
    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
#endif

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
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, *NTLMat);
#endif
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, oldL, degMipo, gamma, 0, *NTLMat);
#endif
            }
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
        {
          nmod_mat_clear (FLINTMat);
          nmod_mat_clear (FLINTMatInv);
#else
        if (NTLN.NumCols() == 1)
        {
          delete NTLMat;
#endif
          Variable y= Variable (2);
          CanonicalForm tmp= F (y - evaluation, y);
          CFList source, dest;
          tmp= mapDown (tmp, info, source, dest);
          delete [] A;
          return CFList (tmp);
        }
      }
    }

#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTMat);
    nmod_mat_clear (FLINTMatInv);
#else
    delete NTLMat;
#endif

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    if (NTLN.NumCols() == 1)
#endif
    {
      Variable y= Variable (2);
      CanonicalForm tmp= F (y - evaluation, y);
      CFList source, dest;
      tmp= mapDown (tmp, info, source, dest);
      delete [] A;
      return CFList (tmp);
    }

    int * zeroOneVecs;
    bufF= F;
    bufUniFactors= factors;
#ifdef HAVE_FLINT
    zeroOneVecs= extractZeroOneVecs (FLINTN);
    result= extReconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, FLINTN,
                               info, evaluation
                              );
#else
    zeroOneVecs= extractZeroOneVecs (NTLN);
    result= extReconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN,
                               info, evaluation
                              );
#endif
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
#endif

#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
increasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int oldL, int l,
                        int d, int* bounds, CFArray& bufQ, nmod_mat_t FLINTN,
                        const Variable& alpha, const CanonicalForm& eval
                       )
#else
CFList
increasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int oldL, int l,
                        int d, int* bounds, CFArray& bufQ, mat_zz_p& NTLN,
                        const Variable& alpha, const CanonicalForm& eval
                       )
#endif
{
  CFList result= CFList();
  CFArray * A= new CFArray [factors.length()];
  int extensionDeg= degree (getMipo (alpha));
  int oldL2= oldL/2;
  bool hitBound= false;
  bool useOldQs= false;
#ifdef HAVE_FLINT
  if (nmod_mat_nrows (FLINTN) != factors.length()) //refined factors
  {
    nmod_mat_clear (FLINTN);
    nmod_mat_init(FLINTN,factors.length(),factors.length(),getCharacteristic());
    for (long i=factors.length()-1; i >= 0; i--)
      nmod_mat_entry (FLINTN, i, i)= 1;
  }
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
#endif
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
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
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          delete [] A;
          return CFList (F(y-eval,y));
        }
      }
    }

    int * zeroOneVecs;
#ifdef HAVE_FLINT
    zeroOneVecs= extractZeroOneVecs (FLINTN);
#else
    zeroOneVecs= extractZeroOneVecs (NTLN);
#endif

    bufF= F;
    bufUniFactors= factors;
#ifdef HAVE_FLINT
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, FLINTN, eval);
#else
    result= reconstruction (bufF, bufUniFactors, zeroOneVecs, oldL, NTLN, eval);
#endif
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
#endif

#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
furtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList&
                                    factors, int l, int liftBound, int d, int*
                                    bounds, nmod_mat_t FLINTN, CFList& diophant,
                                    CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                    const CanonicalForm& eval
                                   )
#else
CFList
furtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList&
                                    factors, int l, int liftBound, int d, int*
                                    bounds, mat_zz_p& NTLN, CFList& diophant,
                                    CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                    const CanonicalForm& eval
                                   )
#endif
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFList bufBufFactors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  bool hitBound= false;
  int oldL= l;
  int stepSize= 8; //TODO choose better step size?
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l), 2);
#ifdef HAVE_FLINT
  if (nmod_mat_nrows (FLINTN) != factors.length()) //refined factors
  {
    nmod_mat_clear (FLINTN);
    nmod_mat_init(FLINTN,factors.length(),factors.length(),getCharacteristic());
    for (long i=factors.length()-1; i >= 0; i--)
      nmod_mat_entry (FLINTN, i, i)= 1;
  }
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
#endif
  CFListIterator j;
  CFMatrix C;
  CFArray buf;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
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
        C= CFMatrix (l - k, bufFactors.length());
        for (int ii= 0; ii < bufFactors.length(); ii++)
        {
          if (A[ii].size() - 1 >= i)
          {
            buf= getCoeffs (A[ii] [i], k);
            writeInMatrix (C, buf, ii + 1, 0);
          }
        }
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          irreducible= true;
          break;
        }
      }
    }

#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    if (NTLN.NumCols() == 1)
#endif
    {
      irreducible= true;
      break;
    }

#ifdef HAVE_FLINT
    int * zeroOneVecs= extractZeroOneVecs (FLINTN);
#else
    int * zeroOneVecs= extractZeroOneVecs (NTLN);
#endif
    bufF= F;
    bufBufFactors= bufFactors;
#ifdef HAVE_FLINT
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, FLINTN, eval);
#else
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN, eval);
#endif
    delete [] zeroOneVecs;
    if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
    {
      F= bufF;
      factors= bufFactors;
      delete [] A;
      return result;
    }
    else
    {
      bufF= F;
      bufFactors= bufBufFactors;
    }

#ifdef HAVE_FLINT
    if (isReduced (FLINTN))
#else
    if (isReduced (NTLN))
#endif
    {
      int factorsFound= 0;
      bufF= F;
#ifdef HAVE_FLINT
      int* factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
      for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
#endif
        factorsFoundIndex[i]= 0;
#ifdef HAVE_FLINT
      if (l < liftBound)
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, FLINTN, eval, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           FLINTN, eval, false
                          );

      if (nmod_mat_ncols (FLINTN) == result.length())
#else
      if (l < liftBound)
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, NTLN, eval, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, eval, false
                          );

      if (NTLN.NumCols() == result.length())
#endif
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
    return CFList (F (y-eval,y));
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}
#endif

//Fq
#ifdef HAVE_NTL
CFList
furtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList&
                                    factors, int l, int liftBound, int d, int*
                                    bounds, mat_zz_pE& NTLN, CFList& diophant,
                                    CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                    const CanonicalForm& eval
                                   )
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFList bufBufFactors;
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
        delete NTLC;
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
    bufBufFactors= bufFactors;
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN, eval);
    delete [] zeroOneVecs;
    if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
    {
      F= bufF;
      factors= bufFactors;
      delete [] A;
      return result;
    }
    else
    {
      bufF= F;
      bufFactors= bufBufFactors;
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
                           factorsFoundIndex, NTLN, eval, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, eval, false
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
    return CFList (F (y-eval,y));
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}
#endif

//over field extension
#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
extFurtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList& factors, int l,
                                       int liftBound, int d, int* bounds,
                                       nmod_mat_t FLINTN, CFList& diophant,
                                       CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                       const CanonicalForm& evaluation, const
                                       ExtensionInfo& info, CFList& source,
                                       CFList& dest
                                      )
#else
CFList
extFurtherLiftingAndIncreasePrecision (CanonicalForm& F, CFList& factors, int l,
                                       int liftBound, int d, int* bounds,
                                       mat_zz_p& NTLN, CFList& diophant,
                                       CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                       const CanonicalForm& evaluation, const
                                       ExtensionInfo& info, CFList& source,
                                       CFList& dest
                                      )
#endif
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFList bufBufFactors;
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
#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
  nmod_mat_init (FLINTN,factors.length(),factors.length(), getCharacteristic());
  for (long i=factors.length()-1; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
#endif
  Variable y= F.mvar();
  CanonicalForm powX, imBasis, bufF, truncF;
  CFMatrix Mat, C;
  CFIterator iter;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTMat, FLINTMatInv, FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLMat,*NTLC, NTLK;
#endif
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

#ifdef HAVE_FLINT
    convertFacCFMatrix2nmod_mat_t (FLINTMat, Mat);
    nmod_mat_init (FLINTMatInv, nmod_mat_nrows (FLINTMat),
                   nmod_mat_nrows (FLINTMat), getCharacteristic());
    nmod_mat_inv (FLINTMatInv, FLINTMat);
#else
    NTLMat= convertFacCFMatrix2NTLmat_zz_p (Mat);
    *NTLMat= inv (*NTLMat);
#endif

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
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
#endif
            }
            else
            {
              A [ii] [i]= A [ii] [i] (y-evaluation, y);
              if (alpha != gamma)
                A[ii] [i]= mapDown (A[ii] [i], imPrimElemAlpha, primElemAlpha,
                                    gamma, source, dest
                                   );
#ifdef HAVE_FLINT
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, FLINTMatInv);
#else
              buf= getCoeffs (A[ii] [i], k, l, degMipo, gamma, 0, *NTLMat);
#endif
            }
            writeInMatrix (C, buf, ii + 1, 0);
          }
          if (GF)
            setCharacteristic (getCharacteristic(), degMipo, info.getGFName());
        }

        if (GF)
          setCharacteristic(getCharacteristic());

#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif

        if (GF)
          setCharacteristic (getCharacteristic(), degMipo, info.getGFName());

#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          irreducible= true;
          break;
        }
      }
    }

#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTMat);
    nmod_mat_clear (FLINTMatInv);
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    delete NTLMat;
    if (NTLN.NumCols() == 1)
#endif
    {
      irreducible= true;
      break;
    }

    bufF= F;
    bufBufFactors= bufFactors;
#ifdef HAVE_FLINT
    int * zeroOneVecs= extractZeroOneVecs (FLINTN);
    result= extReconstruction (bufF, bufFactors, zeroOneVecs, l, FLINTN, info,
                               evaluation
                              );
#else
    int * zeroOneVecs= extractZeroOneVecs (NTLN);
    result= extReconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN, info,
                               evaluation
                              );
#endif
    delete [] zeroOneVecs;
    if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
    {
      F= bufF;
      factors= bufFactors;
      delete [] A;
      return result;
    }
    else
    {
      bufF= F;
      bufFactors= bufBufFactors;
    }

#ifdef HAVE_FLINT
    if (isReduced (FLINTN))
#else
    if (isReduced (NTLN))
#endif
    {
      int factorsFound= 0;
      bufF= F;
#ifdef HAVE_FLINT
      int* factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
      for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
#endif
        factorsFoundIndex[i]= 0;
#ifdef HAVE_FLINT
      if (l < degree (bufF) + 1 + degree (LCF))
        extReconstructionTry (result, bufF, bufFactors, l, factorsFound,
                              factorsFoundIndex, FLINTN, false, info, evaluation
                             );
      else
        extReconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                              degree (LCF), factorsFound, factorsFoundIndex,
                              FLINTN, false, info, evaluation
                             );
      if (nmod_mat_ncols (FLINTN) == result.length())
#else
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
#endif
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
#endif

#ifdef HAVE_NTL // logarithmicDerivative
#ifdef HAVE_FLINT
CFList
furtherLiftingAndIncreasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int
                                         l, int liftBound, int d, int* bounds,
                                         nmod_mat_t FLINTN, CFList& diophant,
                                         CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                         const Variable& alpha,
                                         const CanonicalForm& eval
                                        )
#else
CFList
furtherLiftingAndIncreasePrecisionFq2Fp (CanonicalForm& F, CFList& factors, int
                                         l, int liftBound, int d, int* bounds,
                                         mat_zz_p& NTLN, CFList& diophant,
                                         CFMatrix& M, CFArray& Pi, CFArray& bufQ,
                                         const Variable& alpha,
                                         const CanonicalForm& eval
                                        )
#endif
{
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  bool irreducible= false;
  CFList bufFactors= factors;
  CFList bufBufFactors;
  CFArray *A = new CFArray [bufFactors.length()];
  bool useOldQs= false;
  int extensionDeg= degree (getMipo (alpha));
  bool hitBound= false;
  int oldL= l;
  int stepSize= 8; //TODO choose better step size?
  l += tmax (tmin (8, degree (F) + 1 + degree (LC (F, 1))-l), 2);
#ifdef HAVE_FLINT
  if (nmod_mat_nrows (FLINTN) != factors.length()) //refined factors
  {
    nmod_mat_clear (FLINTN);
    nmod_mat_init(FLINTN,factors.length(),factors.length(),getCharacteristic());
    for (long i=factors.length()-1; i >= 0; i--)
      nmod_mat_entry (FLINTN, i, i)= 1;
  }
#else
  if (NTLN.NumRows() != factors.length()) //refined factors
    ident (NTLN, factors.length());
#endif
  CFListIterator j;
  CFMatrix C;
#ifdef HAVE_FLINT
  long rank;
  nmod_mat_t FLINTC, FLINTK, null;
#else
  mat_zz_p* NTLC, NTLK;
#endif
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
#ifdef HAVE_FLINT
        convertFacCFMatrix2nmod_mat_t (FLINTC, C);
        nmod_mat_init (FLINTK, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTN),
                       getCharacteristic());
        nmod_mat_mul (FLINTK, FLINTC, FLINTN);
        nmod_mat_init (null, nmod_mat_ncols (FLINTK), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        rank= nmod_mat_nullspace (null, FLINTK);
        nmod_mat_clear (FLINTK);
        nmod_mat_window_init (FLINTK, null, 0, 0, nmod_mat_nrows(null), rank);
        nmod_mat_clear (FLINTC);
        nmod_mat_init_set (FLINTC, FLINTN);
        nmod_mat_clear (FLINTN);
        nmod_mat_init (FLINTN, nmod_mat_nrows (FLINTC), nmod_mat_ncols (FLINTK),
                       getCharacteristic());
        nmod_mat_mul (FLINTN, FLINTC, FLINTK); //no aliasing allowed!!

        nmod_mat_clear (FLINTC);
        nmod_mat_window_clear (FLINTK);
        nmod_mat_clear (null);
#else
        NTLC= convertFacCFMatrix2NTLmat_zz_p(C);
        NTLK= (*NTLC)*NTLN;
        transpose (NTLK, NTLK);
        kernel (NTLK, NTLK);
        transpose (NTLK, NTLK);
        NTLN *= NTLK;
        delete NTLC;
#endif
#ifdef HAVE_FLINT
        if (nmod_mat_ncols (FLINTN) == 1)
#else
        if (NTLN.NumCols() == 1)
#endif
        {
          irreducible= true;
          break;
        }
      }
    }
#ifdef HAVE_FLINT
    if (nmod_mat_ncols (FLINTN) == 1)
#else
    if (NTLN.NumCols() == 1)
#endif
    {
      irreducible= true;
      break;
    }

#ifdef HAVE_FLINT
    int * zeroOneVecs= extractZeroOneVecs (FLINTN);
#else
    int * zeroOneVecs= extractZeroOneVecs (NTLN);
#endif
    CanonicalForm bufF= F;
    bufBufFactors= bufFactors;
#ifdef HAVE_FLINT
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, FLINTN, eval);
#else
    result= reconstruction (bufF, bufFactors, zeroOneVecs, l, NTLN,  eval);
#endif
    delete [] zeroOneVecs;
    if (result.length() > 0 && degree (bufF) + 1 + degree (LC (bufF, 1)) <= l)
    {
      F= bufF;
      factors= bufFactors;
      delete [] A;
      return result;
    }
    else
    {
      bufF= F;
      bufFactors= bufBufFactors;
    }

#ifdef HAVE_FLINT
    if (isReduced (FLINTN))
#else
    if (isReduced (NTLN))
#endif
    {
      int factorsFound= 0;
      bufF= F;
#ifdef HAVE_FLINT
      int* factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
      for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      int* factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
#endif
        factorsFoundIndex[i]= 0;
#ifdef HAVE_FLINT
      if (l < degree (bufF) + 1 + degree (LCF))
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, FLINTN, eval, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           FLINTN, eval, false
                          );
      if (nmod_mat_ncols (FLINTN) == result.length())
#else
      if (l < degree (bufF) + 1 + degree (LCF))
        reconstructionTry (result, bufF, bufFactors, l, factorsFound,
                           factorsFoundIndex, NTLN, eval, false
                          );
      else
        reconstructionTry (result, bufF, bufFactors, degree (bufF) + 1 +
                           degree (LCF), factorsFound, factorsFoundIndex,
                           NTLN, eval, false
                          );
      if (NTLN.NumCols() == result.length())
#endif
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
    return CFList (F (y-eval,y));
  }
  delete [] A;
  factors= bufFactors;
  return CFList();
}
#endif

#ifndef HAVE_FLINT
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
#endif

#ifdef HAVE_FLINT
#ifdef HAVE_NTL // henselLift12
void
refineAndRestartLift (const CanonicalForm& F, const nmod_mat_t FLINTN, int
                      liftBound, int l, CFList& factors, CFMatrix& M, CFArray&
                      Pi, CFList& diophant
                     )
{
  CFList bufFactors;
  Variable y= Variable (2);
  CanonicalForm LCF= LC (F, 1);
  CFListIterator iter;
  CanonicalForm buf;
  for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
  {
    iter= factors;
    buf= 1;
    for (long j= 0; j < nmod_mat_nrows (FLINTN); j++, iter++)
    {
      if (!(nmod_mat_entry (FLINTN,j,i) == 0))
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
#endif
#endif

#ifdef HAVE_NTL // mat_zz_pE
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
#endif

#ifdef HAVE_FLINT
CFList
earlyReconstructionAndLifting (const CanonicalForm& F, const nmod_mat_t N,
                               CanonicalForm& bufF, CFList& factors, int& l,
                               int& factorsFound, bool beenInThres, CFMatrix& M,
                               CFArray& Pi, CFList& diophant, bool symmetric,
                               const CanonicalForm& evaluation
                              )
#else
CFList
earlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_p& N,
                               CanonicalForm& bufF, CFList& factors, int& l,
                               int& factorsFound, bool beenInThres, CFMatrix& M,
                               CFArray& Pi, CFList& diophant, bool symmetric,
                               const CanonicalForm& evaluation
                              )
#endif
{
  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (F, sizeOfLiftPre, degree (LC (F, 1), 2));

  Variable y= F.mvar();
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= tmin (11, liftPre [sizeOfLiftPre- 1] + 1);
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init_set (FLINTN, N);
  int * factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
  for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
  mat_zz_p NTLN= N;
  int * factorsFoundIndex= new int [NTLN.NumCols()];
  for (long i= 0; i < NTLN.NumCols(); i++)
#endif
    factorsFoundIndex [i]= 0;

  if (degree (F) + 1 > smallFactorDeg)
  {
    if (l < smallFactorDeg)
    {
      TIMING_START (fac_fq_lift);
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction0: ");
      l= smallFactorDeg;
    }
#ifdef HAVE_FLINT
    TIMING_START (fac_fq_reconstruction);
    reconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                       factorsFoundIndex, FLINTN, evaluation, beenInThres
                      );
    TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct0: ");
    if (result.length() == nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    TIMING_START (fac_fq_reconstruction);
    reconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                       factorsFoundIndex, NTLN, evaluation, beenInThres
                      );
    TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct0: ");
    if (result.length() == NTLN.NumCols())
    {
#endif
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
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction1: ");
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
#ifdef HAVE_FLINT
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, FLINTN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct1: ");
      if (result.length() == nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct1: ");
      if (result.length() == NTLN.NumCols())
      {
#endif
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
    while (((degree (F,y)/4)*i+1) + 4 <= smallFactorDeg)
      i++;
    while (i < 5)
    {
      dummy= tmin (degree (F,y)+1, ((degree (F,y)/4)+1)*i+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction2: ");
        l= dummy;
        if (i == 1 && degree (F)%4==0 && symmetric && factors.length() == 2 &&
            LC (F,1).inCoeffDomain() &&
           (degree (factors.getFirst(), 1) == degree (factors.getLast(),1)))
        {
          Variable x= Variable (1);
          CanonicalForm g, h, gg, hh, multiplier1, multiplier2, check1, check2;
          int m= degree (F)/4+1;
          g= factors.getFirst();
          h= factors.getLast();
          g= mod (g, power (y,m));
          h= mod (h, power (y,m));
          g= g (y-evaluation, y);
          h= h (y-evaluation, y);
          gg= mod (swapvar (g,x,y),power (x,m));
          gg= gg (y + evaluation, y);
          multiplier1= factors.getLast()[m-1][0]/gg[m-1][0];
          gg= div (gg, power (y,m));
          gg= gg*power (y,m);
          hh= mod (swapvar (h,x,y),power (x,m));
          hh= hh (y + evaluation, y);
          multiplier2= factors.getFirst()[m-1][0]/hh[m-1][0];
          hh= div (hh, power (y,m));
          hh= hh*power (y,m);
          gg= multiplier1*gg+mod (factors.getLast(), power (y,m));
          hh= multiplier2*hh+mod (factors.getFirst(), power (y,m));
          check1= gg (y-evaluation,y);
          check2= hh (y-evaluation,y);
          CanonicalForm oldcheck1= check1;
          check1= swapvar (check1, x, y);
          if (check1/Lc (check1) == check2/Lc (check2))
          {
#ifdef HAVE_FLINT
            nmod_mat_clear (FLINTN);
#endif
            result.append (oldcheck1);
            result.append (check2);
            delete [] liftPre;
            delete [] factorsFoundIndex;
            return result;
          }
        }
      }
      else
      {
        i++;
        if (i < 5)
          continue;
      }
#ifdef HAVE_FLINT
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, FLINTN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct2: ");
      if (result.length() == nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct2: ");
      if (result.length() == NTLN.NumCols())
      {
#endif
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i++;
    }
  }

#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif
  delete [] liftPre;
  delete [] factorsFoundIndex;
  return result;
}

#ifdef HAVE_NTL // mat_zz_pE
CFList
earlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_pE& N,
                               CanonicalForm& bufF, CFList& factors, int& l,
                               int& factorsFound, bool beenInThres, CFMatrix& M,
                               CFArray& Pi, CFList& diophant, bool symmetric,
                               const CanonicalForm& evaluation
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
      TIMING_START (fac_fq_lift);
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction0: ");
      l= smallFactorDeg;
    }
    TIMING_START (fac_fq_reconstruction);
    reconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                       factorsFoundIndex, NTLN, evaluation, beenInThres
                      );
    TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct0: ");
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
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction1: ");
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct1: ");
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
    while ((degree (F,y)/4+1)*i + 4 <= smallFactorDeg)
      i++;
    while (i < 5)
    {
      dummy= tmin (degree (F,y)+1, (degree (F,y)/4+1)*i+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction2: ");
        l= dummy;
        if (i == 1 && degree (F)%4==0 && symmetric && factors.length() == 2 &&
            LC (F,1).inCoeffDomain() &&
           (degree (factors.getFirst(), 1) == degree (factors.getLast(),1)))
        {
          Variable x= Variable (1);
          CanonicalForm g, h, gg, hh, multiplier1, multiplier2, check1, check2;
          int m= degree (F)/4+1;
          g= factors.getFirst();
          h= factors.getLast();
          g= mod (g, power (y,m));
          h= mod (h, power (y,m));
          g= g (y-evaluation, y);
          h= h (y-evaluation, y);
          gg= mod (swapvar (g,x,y),power (x,m));
          gg= gg (y + evaluation, y);
          multiplier1= factors.getLast()[m-1][0]/gg[m-1][0];
          gg= div (gg, power (y,m));
          gg= gg*power (y,m);
          hh= mod (swapvar (h,x,y),power (x,m));
          hh= hh (y + evaluation, y);
          multiplier2= factors.getFirst()[m-1][0]/hh[m-1][0];
          hh= div (hh, power (y,m));
          hh= hh*power (y,m);
          gg= multiplier1*gg+mod (factors.getLast(), power (y,m));
          hh= multiplier2*hh+mod (factors.getFirst(), power (y,m));
          check1= gg (y-evaluation,y);
          check2= hh (y-evaluation,y);
          CanonicalForm oldcheck1= check1;
          check1= swapvar (check1, x, y);
          if (check1/Lc (check1) == check2/Lc (check2))
          {
            result.append (oldcheck1);
            result.append (check2);
            delete [] liftPre;
            delete [] factorsFoundIndex;
            return result;
          }
        }
      }
      else
      {
        i++;
        if (i < 5)
          continue;
      }
      TIMING_START (fac_fq_reconstruction);
      reconstructionTry (result, bufF, factors, l, factorsFound,
                         factorsFoundIndex, NTLN, evaluation, beenInThres
                        );
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct2: ");
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
#endif

//over field extension
#ifdef HAVE_FLINT
CFList
extEarlyReconstructionAndLifting (const CanonicalForm& F, const nmod_mat_t N,
                                  CanonicalForm& bufF, CFList& factors, int& l,
                                  int& factorsFound, bool beenInThres, CFMatrix&
                                  M, CFArray& Pi, CFList& diophant, const
                                  ExtensionInfo& info, const CanonicalForm&
                                  evaluation
                                 )
#else
CFList
extEarlyReconstructionAndLifting (const CanonicalForm& F, const mat_zz_p& N,
                                  CanonicalForm& bufF, CFList& factors, int& l,
                                  int& factorsFound, bool beenInThres, CFMatrix&
                                  M, CFArray& Pi, CFList& diophant, const
                                  ExtensionInfo& info, const CanonicalForm&
                                  evaluation
                                 )
#endif
{
  int sizeOfLiftPre;
  int * liftPre= getLiftPrecisions (F, sizeOfLiftPre, degree (LC (F, 1), 2));
  Variable y= F.mvar();
  factorsFound= 0;
  CanonicalForm LCF= LC (F, 1);
  CFList result;
  int smallFactorDeg= 11;
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init_set (FLINTN, N);
  int * factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
  for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
  mat_zz_p NTLN= N;
  int * factorsFoundIndex= new int [NTLN.NumCols()];
  for (long i= 0; i < NTLN.NumCols(); i++)
#endif
    factorsFoundIndex [i]= 0;

  if (degree (F) + 1 > smallFactorDeg)
  {
    if (l < smallFactorDeg)
    {
      TIMING_START (fac_fq_lift);
      factors.insert (LCF);
      henselLiftResume12 (F, factors, l, smallFactorDeg, Pi, diophant, M);
      TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction0: ");
      l= smallFactorDeg;
    }
    TIMING_START (fac_fq_reconstruction);
#ifdef HAVE_FLINT
    extReconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                          factorsFoundIndex, FLINTN, beenInThres, info,
                          evaluation
                      );
#else
    extReconstructionTry (result, bufF, factors, smallFactorDeg, factorsFound,
                          factorsFoundIndex, NTLN, beenInThres, info,
                          evaluation
                      );
#endif
    TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct0: ");
#ifdef HAVE_FLINT
    if (result.length() == nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    if (result.length() == NTLN.NumCols())
    {
#endif
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
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, liftPre[i-1] + 1, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction1: ");
        l= liftPre[i-1] + 1;
      }
      else
      {
        i--;
        if (i != 0)
          continue;
      }
      TIMING_START (fac_fq_reconstruction);
#ifdef HAVE_FLINT
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, FLINTN, beenInThres, info,
                            evaluation
                           );
#else
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, NTLN, beenInThres, info,
                            evaluation
                           );
#endif
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct1: ");
#ifdef HAVE_FLINT
      if (result.length() == nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      if (result.length() == NTLN.NumCols())
      {
#endif
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
    while ((degree (F,y)/4+1)*i + 4 <= smallFactorDeg)
      i++;
    while (i < 5)
    {
      dummy= tmin (degree (F,y)+1, (degree (F,y)/4+1)*i+4);
      if (l < dummy)
      {
        factors.insert (LCF);
        TIMING_START (fac_fq_lift);
        henselLiftResume12 (F, factors, l, dummy, Pi, diophant, M);
        TIMING_END_AND_PRINT (fac_fq_lift, "time to lift in reconstruction2: ");
        l= dummy;
      }
      else
      {
        i++;
        if (i < 5)
          continue;
      }
      TIMING_START (fac_fq_reconstruction);
#ifdef HAVE_FLINT
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, FLINTN, beenInThres, info,
                            evaluation
                           );
#else
      extReconstructionTry (result, bufF, factors, l, factorsFound,
                            factorsFoundIndex, NTLN, beenInThres, info,
                            evaluation
                           );
#endif
      TIMING_END_AND_PRINT (fac_fq_reconstruction, "time to reconstruct2: ");
#ifdef HAVE_FLINT
      if (result.length() == nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      if (result.length() == NTLN.NumCols())
      {
#endif
        delete [] liftPre;
        delete [] factorsFoundIndex;
        return result;
      }
      i++;
    }
  }

#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif
  delete [] liftPre;
  delete [] factorsFoundIndex;
  return result;
}

#ifdef HAVE_NTL // henselLift12
CFList
sieveSmallFactors (const CanonicalForm& G, CFList& uniFactors, DegreePattern&
                   degPat, CanonicalForm& H, CFList& diophant, CFArray& Pi,
                   CFMatrix& M, bool& success, int d, const CanonicalForm& eval
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
                        factorsFoundIndex, degs, success, smallFactorDeg, eval);
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
#endif

#ifdef HAVE_NTL // henselLift12
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
#endif

#ifdef HAVE_NTL // matrix Fq
CFList
henselLiftAndLatticeRecombi (const CanonicalForm& G, const CFList& uniFactors,
                             const Variable& alpha, const DegreePattern& degPat,
                             bool symmetric, const CanonicalForm& eval
                            )
{
  DegreePattern degs= degPat;
  CanonicalForm F= G;
  CanonicalForm LCF= LC (F, 1);
  Variable y= F.mvar();
  Variable x= Variable (1);
  int d;
  bool isIrreducible= false;
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    return CFList (G);
  }
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
  bool success= false;
  smallFactors= sieveSmallFactors (F, bufUniFactors, degs, H, diophant, Pi, M,
                                   success, minBound + 1, eval
                                  );

  if (smallFactors.length() > 0)
  {
    if (smallFactors.length() == 1)
    {
      if (smallFactors.getFirst() == F)
      {
        delete [] bounds;
        return CFList (G (y-eval,y));
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
    tmp1= mod (i.getItem(),y-eval);
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
    bounds= computeBounds (F, d, isIrreducible);
    if (isIrreducible)
    {
      smallFactors.append (F (y-eval,y));
      delete [] bounds;
      return smallFactors;
    }
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
      smallFactors.append (F (y-eval,y));
      delete [] bounds;
      return smallFactors;
    }
  }

  bool reduceFq2Fp= (degree (F) > getCharacteristic());
  bufUniFactors.insert (LCF);
  int l= 1;

#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
#endif

  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  mat_zz_p NTLN;

  if (alpha.level() != 1)
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);
  }
  mat_zz_pE NTLNe;

  if (alpha.level() == 1)
  {
#ifdef HAVE_FLINT
    nmod_mat_init (FLINTN, bufUniFactors.length()-1, bufUniFactors.length()-1, getCharacteristic());
    for (long i= bufUniFactors.length()-2; i >= 0; i--)
      nmod_mat_entry (FLINTN, i, i)= 1;
#else
    ident (NTLN, bufUniFactors.length() - 1);
#endif
  }
  else
  {
    if (reduceFq2Fp)
#ifdef HAVE_FLINT
    {
      nmod_mat_init (FLINTN, bufUniFactors.length()-1, bufUniFactors.length()-1, getCharacteristic());
      for (long i= bufUniFactors.length()-2; i >= 0; i--)
        nmod_mat_entry (FLINTN, i, i)= 1;
    }
#else
      ident (NTLN, bufUniFactors.length() - 1);
#endif
    else
      ident (NTLNe, bufUniFactors.length() - 1);
  }
  bool irreducible= false;
  CFArray bufQ= CFArray (bufUniFactors.length() - 1);

  int oldL;
  TIMING_START (fac_fq_till_reduced);
  if (success)
  {
    int start= 0;
    if (alpha.level() == 1)
      oldL= liftAndComputeLattice (F, bounds, d, start, liftBound, minBound,
#ifdef HAVE_FLINT
                                   bufUniFactors, FLINTN, diophant, M, Pi, bufQ,
#else
                                   bufUniFactors, NTLN, diophant, M, Pi, bufQ,
#endif
                                   irreducible
                                  );
    else
    {
      if (reduceFq2Fp)
        oldL= liftAndComputeLatticeFq2Fp (F, bounds, d, start, liftBound,
#ifdef HAVE_FLINT
                                          minBound, bufUniFactors, FLINTN,
#else
                                          minBound, bufUniFactors, NTLN,
#endif
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
    {
      oldL= liftAndComputeLattice (F, bounds, d, minBound + 1, liftBound,
#ifdef HAVE_FLINT
                                   minBound, bufUniFactors, FLINTN, diophant, M,
#else
                                   minBound, bufUniFactors, NTLN, diophant, M,
#endif
                                   Pi, bufQ, irreducible
                                  );
    }
    else
    {
      if (reduceFq2Fp)
        oldL= liftAndComputeLatticeFq2Fp (F, bounds, d, minBound + 1,
                                          liftBound, minBound, bufUniFactors,
#ifdef HAVE_FLINT
                                          FLINTN, diophant, M, Pi, bufQ,
#else
                                          NTLN, diophant, M, Pi, bufQ,
#endif
                                          irreducible, alpha
                                         );
      else
        oldL= liftAndComputeLattice (F, bounds, d, minBound + 1, liftBound,
                                     minBound, bufUniFactors, NTLNe, diophant,
                                     M, Pi, bufQ, irreducible
                                    );
    }
  }

  TIMING_END_AND_PRINT (fac_fq_till_reduced,
                        "time to compute a reduced lattice: ");
  bufUniFactors.removeFirst();
  if (oldL > liftBound)
  {
#ifdef HAVE_FLINT
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      nmod_mat_clear (FLINTN);
#endif
    delete [] bounds;
    return Union (smallFactors,
                  factorRecombination (bufUniFactors, F,
                                       power (y, degree (F) + 1),
                                       degs, eval, 1, bufUniFactors.length()/2
                                      )
                 );
  }

  l= oldL;
  if (irreducible)
  {
#ifdef HAVE_FLINT
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      nmod_mat_clear (FLINTN);
#endif
    delete [] bounds;
    return Union (CFList (F(y-eval,y)), smallFactors);
  }

  CanonicalForm yToL= power (y,l);

  CFList result;
  if (l >= degree (F) + 1)
  {
    int * factorsFoundIndex;
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
#ifdef HAVE_FLINT
      factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
      for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
#endif
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
#ifdef HAVE_FLINT
                         factorsFound, factorsFoundIndex, FLINTN, eval, false
#else
                         factorsFound, factorsFoundIndex, NTLN, eval, false
#endif
                        );
    else
        reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                           factorsFound, factorsFoundIndex, NTLNe, eval, false
                          );
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
#ifdef HAVE_FLINT
      if (result.length() == nmod_mat_ncols (FLINTN))
      {
        if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
          nmod_mat_clear (FLINTN);
#else
      if (result.length() == NTLN.NumCols())
      {
#endif
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
#ifdef HAVE_FLINT
      factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
      for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      factorsFoundIndex= new int [NTLN.NumCols()];
      for (long i= 0; i < NTLN.NumCols(); i++)
#endif
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
      reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
#ifdef HAVE_FLINT
                         factorsFound, factorsFoundIndex, FLINTN, eval, false
#else
                         factorsFound, factorsFoundIndex, NTLN, eval, false
#endif
                        );
    else
      reconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                         factorsFound, factorsFoundIndex, NTLNe, eval, false
                        );
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
#ifdef HAVE_FLINT
      if (result.length() == nmod_mat_ncols(FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      if (result.length() == NTLN.NumCols())
      {
#endif
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
#ifdef HAVE_FLINT
      if (nmod_mat_ncols (FLINTN) < bufUniFactors.length())
      {
        refineAndRestartLift (F, FLINTN, liftBound, l, bufUniFactors, M, Pi,
#else
      if (NTLN.NumCols() < bufUniFactors.length())
      {
        refineAndRestartLift (F, NTLN, liftBound, l, bufUniFactors, M, Pi,
#endif
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
#ifdef HAVE_FLINT
    result= earlyReconstructionAndLifting (F, FLINTN, bufF, bufUniFactors, l,
#else
    result= earlyReconstructionAndLifting (F, NTLN, bufF, bufUniFactors, l,
#endif
                                           factorsFound, beenInThres, M, Pi,
                                           diophant, symmetric, eval
                                          );

#ifdef HAVE_FLINT
    if (result.length() == nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    if (result.length() == NTLN.NumCols())
    {
#endif
      delete [] bounds;
      return Union (result, smallFactors);
    }
  }
  else
  {
    result= earlyReconstructionAndLifting (F, NTLNe, bufF, bufUniFactors, l,
                                           factorsFound, beenInThres, M, Pi,
                                           diophant, symmetric, eval
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
        tmp1= mod (i.getItem(), y-eval);
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
      int * zeroOne;
      long numCols, numRows;
      if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      {
#ifdef HAVE_FLINT
        numCols= nmod_mat_ncols (FLINTN);
        numRows= nmod_mat_nrows (FLINTN);
        zeroOne= extractZeroOneVecs (FLINTN);
#else
        numCols= NTLN.NumCols();
        numRows= NTLN.NumRows();
        zeroOne= extractZeroOneVecs (NTLN);
#endif
      }
      else
      {
        numCols= NTLNe.NumCols();
        numRows= NTLNe.NumRows();
        zeroOne= extractZeroOneVecs (NTLNe);
      }
      CFList bufBufUniFactors= bufUniFactors;
      CFListIterator iter, iter2;
      CanonicalForm buf;
      CFList factorsConsidered;
      CanonicalForm tmp;
      for (int i= 0; i < numCols; i++)
      {
        if (zeroOne [i] == 0)
          continue;
        iter= bufUniFactors;
        buf= 1;
        factorsConsidered= CFList();
        for (int j= 0; j < numRows; j++, iter++)
        {
          if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
          {
#ifdef HAVE_FLINT
            if (!(nmod_mat_entry (FLINTN, j,i) == 0))
#else
            if (!IsZero (NTLN (j + 1,i + 1)))
#endif
            {
              factorsConsidered.append (iter.getItem());
              buf *= mod (iter.getItem(), y);
            }
          }
          else
          {
            if (!IsZero (NTLNe (j + 1,i + 1)))
            {
              factorsConsidered.append (iter.getItem());
              buf *= mod (iter.getItem(), y);
            }
          }
        }
        buf /= Lc (buf);
        for (iter2= result; iter2.hasItem(); iter2++)
        {
          tmp= mod (iter2.getItem(), y-eval);
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
#ifdef HAVE_FLINT
      oldNumCols= nmod_mat_ncols (FLINTN);
#else
      oldNumCols= NTLN.NumCols();
#endif
      resultBufF= increasePrecision (bufF, bufUniFactors, factorsFound,
                                     oldNumCols, oldL, l, eval
                                    );
    }
    else
    {
      if (reduceFq2Fp)
      {
#ifdef HAVE_FLINT
        oldNumCols= nmod_mat_ncols (FLINTN);
#else
        oldNumCols= NTLN.NumCols();
#endif

        resultBufF= increasePrecisionFq2Fp (bufF, bufUniFactors, factorsFound,
                                            oldNumCols, oldL, alpha, l, eval
                                           );
      }
      else
      {
        oldNumCols= NTLNe.NumCols();

        resultBufF= increasePrecision (bufF, bufUniFactors, factorsFound,
                                       oldNumCols, oldL, alpha, l, eval
                                      );
      }
    }

    if (bufUniFactors.isEmpty() || degree (bufF) <= 0)
    {
#ifdef HAVE_FLINT
      if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
        nmod_mat_clear (FLINTN);
#endif
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
#ifdef HAVE_FLINT
      if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
        nmod_mat_clear (FLINTN);
#endif
      result.append (bufF (y-eval,y));
      return result;
    }
#ifdef HAVE_FLINT
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      nmod_mat_clear (FLINTN);
#endif
    return Union (result, henselLiftAndLatticeRecombi (bufF, bufUniFactors,
                                                       alpha, degs, symmetric,
                                                       eval
                                                      )
                 );
  }

  if (l < liftBound)
  {
    if (alpha.level() == 1)
    {
        result=increasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
#ifdef HAVE_FLINT
                                  FLINTN, eval
#else
                                  NTLN, eval
#endif
                                 );
    }
    else
    {
      if (reduceFq2Fp)
      {
          result=increasePrecisionFq2Fp (F, bufUniFactors, oldL, l, d, bounds,
#ifdef HAVE_FLINT
                                         bufQ, FLINTN, alpha, eval
#else
                                         bufQ, NTLN, alpha, eval
#endif
                                        );
      }
      else
      {
          result=increasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                    NTLNe, eval
                                   );
      }
    }
    if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    {
#ifdef HAVE_FLINT
      if (result.length()== nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      if (result.length()== NTLN.NumCols())
      {
#endif
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
#ifdef HAVE_FLINT
                                                    liftBound,d,bounds,FLINTN,
#else
                                                    liftBound, d, bounds, NTLN,
#endif
                                                    diophant, M, Pi, bufQ, eval
                                                   );
      else
      {
        if (reduceFq2Fp)
          result= furtherLiftingAndIncreasePrecisionFq2Fp (F,bufUniFactors, l,
                                                           liftBound, d, bounds,
#ifdef HAVE_FLINT
                                                           FLINTN, diophant, M,
#else
                                                           NTLN, diophant, M,
#endif
                                                           Pi, bufQ, alpha, eval
                                                          );
        else
          result= furtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                      liftBound, d, bounds,
                                                      NTLNe, diophant, M,
                                                      Pi, bufQ, eval
                                                     );
      }

      if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
      {
#ifdef HAVE_FLINT
        if (result.length() == nmod_mat_ncols (FLINTN))
        {
          nmod_mat_clear (FLINTN);
#else
        if (result.length() == NTLN.NumCols())
        {
#endif
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
  bounds= computeBounds (F, d, isIrreducible);
#ifdef HAVE_FLINT
  if (alpha.level() == 1 || (alpha.level() != 1 && reduceFq2Fp))
    nmod_mat_clear (FLINTN);
#endif
  if (isIrreducible)
  {
    delete [] bounds;
    result= Union (result, smallFactors);
    result.append (F (y-eval,y));
    return result;
  }
  minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  if (minBound > 16 || result.length() == 0)
  {
    result= Union (result, smallFactors);
    CanonicalForm MODl= power (y, degree (F) + 1);
    delete [] bounds;
    return Union (result, factorRecombination (bufUniFactors, F, MODl, degs,
                                               eval, 1, bufUniFactors.length()/2
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
                                                       degs,symmetric, eval
                                                      )
                 );
  }
}
#endif

#ifdef HAVE_NTL //primitiveElement
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
#endif

#ifdef HAVE_NTL // init4ext
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
  bool isIrreducible= false;
  int* bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    CFList source, dest;
    CanonicalForm tmp= G (y - evaluation, y);
    tmp= mapDown (tmp, info, source, dest);
    return CFList (tmp);
  }
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
  bool success= false;
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
    F= H/Lc(H);
    delete [] bounds;
    bounds= computeBounds (F, d, isIrreducible);
    if (isIrreducible)
    {
      delete [] bounds;
      CFList source, dest;
      CanonicalForm tmp= F (y - evaluation, y);
      tmp= mapDown (tmp, info, source, dest);
      smallFactors.append (tmp);
      return smallFactors;
    }
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

#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  nmod_mat_init (FLINTN, bufUniFactors.length()-1, bufUniFactors.length()-1,
                 getCharacteristic());
  for (long i= bufUniFactors.length()-2; i >= 0; i--)
    nmod_mat_entry (FLINTN, i, i)= 1;
#else
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLMipo;
  mat_zz_p NTLN;

  ident (NTLN, bufUniFactors.length() - 1);
#endif
  bool irreducible= false;
  CFArray bufQ= CFArray (bufUniFactors.length() - 1);

  int oldL;
  TIMING_START (fac_fq_till_reduced);
  if (success)
  {
    int start= 0;
#ifdef HAVE_FLINT
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound, start,
                                    bufUniFactors, FLINTN, diophant,M, Pi, bufQ,
                                    irreducible, evaluation, info2, source, dest
                                   );
#else
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound, start,
                                    bufUniFactors, NTLN, diophant, M, Pi, bufQ,
                                    irreducible, evaluation, info2, source, dest
                                   );
#endif
  }
  else
  {
#ifdef HAVE_FLINT
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound,
                                    minBound+1, bufUniFactors, FLINTN, diophant,
                                    M, Pi, bufQ, irreducible, evaluation, info2,
                                    source, dest
                                   );
#else
    oldL= extLiftAndComputeLattice (F, bounds, d, liftBound, minBound,
                                    minBound + 1, bufUniFactors, NTLN, diophant,
                                    M, Pi, bufQ, irreducible, evaluation, info2,
                                    source, dest
                                   );
#endif
  }
  TIMING_END_AND_PRINT (fac_fq_till_reduced,
                        "time to compute a reduced lattice: ");

  bufUniFactors.removeFirst();
  if (oldL > liftBound)
  {
#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTN);
#endif
    delete [] bounds;
    return Union (smallFactors, extFactorRecombination
                                (bufUniFactors, F,
                                 power (y, degree (F) + 1),info,
                                 degs, evaluation, 1, bufUniFactors.length()/2
                                )
                 );
  }

  l= oldL;
  if (irreducible)
  {
#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTN);
#endif
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

#ifdef HAVE_FLINT
    factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
    for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
    factorsFoundIndex= new int [NTLN.NumCols()];
    for (long i= 0; i < NTLN.NumCols(); i++)
#endif
      factorsFoundIndex[i]= 0;

    int factorsFound= 0;
    CanonicalForm bufF= F;

#ifdef HAVE_FLINT
    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                          factorsFound, factorsFoundIndex, FLINTN, false, info,
                          evaluation
                         );

    if (result.length() == nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                          factorsFound, factorsFoundIndex, NTLN, false, info,
                          evaluation
                         );

    if (result.length() == NTLN.NumCols())
    {
#endif
      delete [] factorsFoundIndex;
      delete [] bounds;
      return Union (result, smallFactors);
    }

    delete [] factorsFoundIndex;
  }
  if (l >= liftBound)
  {
    int * factorsFoundIndex;
#ifdef HAVE_FLINT
    factorsFoundIndex= new int [nmod_mat_ncols (FLINTN)];
    for (long i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
    factorsFoundIndex= new int [NTLN.NumCols()];
    for (long i= 0; i < NTLN.NumCols(); i++)
#endif
      factorsFoundIndex[i]= 0;
    CanonicalForm bufF= F;
    int factorsFound= 0;

#ifdef HAVE_FLINT
    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                          factorsFound, factorsFoundIndex, FLINTN, false,
                          info, evaluation
                         );

    if (result.length() == nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    extReconstructionTry (result, bufF, bufUniFactors, degree (F) + 1,
                          factorsFound, factorsFoundIndex, NTLN, false,
                          info, evaluation
                         );

    if (result.length() == NTLN.NumCols())
    {
#endif
      delete [] factorsFoundIndex;
      delete [] bounds;
      return Union (result, smallFactors);
    }
    delete [] factorsFoundIndex;
  }

  result= CFList();
  bool beenInThres= false;
  int thres= 100;
#ifdef HAVE_FLINT
  if (l <= thres && bufUniFactors.length() > nmod_mat_ncols (FLINTN))
  {
    refineAndRestartLift (F, FLINTN, 2*totaldegree (F)-1, l, bufUniFactors, M, Pi,
                         diophant
                        );
#else
  if (l <= thres && bufUniFactors.length() > NTLN.NumCols())
  {
    refineAndRestartLift (F, NTLN, 2*totaldegree (F)-1, l, bufUniFactors, M, Pi,
                         diophant
                        );
#endif
    beenInThres= true;
  }


  CanonicalForm bufF= F;
  int factorsFound= 0;

#ifdef HAVE_FLINT
  result= extEarlyReconstructionAndLifting (F, FLINTN, bufF, bufUniFactors, l,
                                            factorsFound, beenInThres, M, Pi,
                                            diophant, info, evaluation
                                           );

  if (result.length() == nmod_mat_ncols (FLINTN))
  {
    nmod_mat_clear (FLINTN);
#else
  result= extEarlyReconstructionAndLifting (F, NTLN, bufF, bufUniFactors, l,
                                            factorsFound, beenInThres, M, Pi,
                                            diophant, info, evaluation
                                           );

  if (result.length() == NTLN.NumCols())
  {
#endif
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
#ifdef HAVE_FLINT
      int * zeroOne= extractZeroOneVecs (FLINTN);
#else
      int * zeroOne= extractZeroOneVecs (NTLN);
#endif
      CFList bufBufUniFactors= bufUniFactors;
      CFListIterator iter, iter2;
      CanonicalForm buf;
      CFList factorsConsidered;
#ifdef HAVE_FLINT
      for (int i= 0; i < nmod_mat_ncols (FLINTN); i++)
#else
      for (int i= 0; i < NTLN.NumCols(); i++)
#endif
      {
        if (zeroOne [i] == 0)
          continue;
        iter= bufUniFactors;
        buf= 1;
        factorsConsidered= CFList();
#ifdef HAVE_FLINT
        for (int j= 0; j < nmod_mat_nrows (FLINTN); j++, iter++)
        {
          if (!(nmod_mat_entry (FLINTN, j, i) == 0))
#else
        for (int j= 0; j < NTLN.NumRows(); j++, iter++)
        {
          if (!IsZero (NTLN (j + 1,i + 1)))
#endif
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

#ifdef HAVE_FLINT //TODO
    oldNumCols= nmod_mat_ncols (FLINTN);
    resultBufF= extIncreasePrecision (bufF, bufUniFactors, factorsFound,
                                      oldNumCols, oldL, evaluation, info2,
                                      source, dest, l
                                     );
    nmod_mat_clear (FLINTN);
#else
    oldNumCols= NTLN.NumCols();
    resultBufF= extIncreasePrecision (bufF, bufUniFactors, factorsFound,
                                      oldNumCols, oldL, evaluation, info2,
                                      source, dest, l
                                     );
#endif
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
      CFList source, dest;
      CanonicalForm tmp= bufF (y - evaluation, y);
      tmp= mapDown (tmp, info, source, dest);
      result.append (tmp);
      return result;
    }
    return Union (result, extHenselLiftAndLatticeRecombi (bufF, bufUniFactors,
                                                          info, degs, evaluation
                                                         )
                 );
  }

  if (l/degMipo < liftBound)
  {
#ifdef HAVE_FLINT
    result=extIncreasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                 FLINTN, evaluation, info2, source, dest
                                );

    if (result.length()== nmod_mat_ncols (FLINTN))
    {
      nmod_mat_clear (FLINTN);
#else
    result=extIncreasePrecision (F, bufUniFactors, oldL, l, d, bounds, bufQ,
                                 NTLN, evaluation, info2, source, dest
                                );

    if (result.length()== NTLN.NumCols())
    {
#endif
      delete [] bounds;
      result= Union (result, smallFactors);
      return result;
    }

    if (result.isEmpty())
    {
#ifdef HAVE_FLINT
      result= extFurtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                     liftBound, d,bounds,FLINTN,
                                                     diophant, M, Pi, bufQ,
                                                     evaluation, info2, source,
                                                     dest
                                                    );
      if (result.length()== nmod_mat_ncols (FLINTN))
      {
        nmod_mat_clear (FLINTN);
#else
      result= extFurtherLiftingAndIncreasePrecision (F,bufUniFactors, l,
                                                     liftBound, d, bounds, NTLN,
                                                     diophant, M, Pi, bufQ,
                                                     evaluation, info2, source,
                                                     dest
                                                    );
      if (result.length()== NTLN.NumCols())
      {
#endif
        delete [] bounds;
        result= Union (result, smallFactors);
        return result;
      }
    }
  }

#ifdef HAVE_FLINT
  nmod_mat_clear (FLINTN);
#endif

  DEBOUTLN (cerr, "lattice recombination failed");

  DegreePattern bufDegs= DegreePattern (bufUniFactors);
  degs.intersect (bufDegs);
  degs.refine();

  delete [] bounds;
  bounds= computeBounds (F, d, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    CFList source, dest;
    CanonicalForm tmp= F (y - evaluation, y);
    tmp= mapDown (tmp, info, source, dest);
    smallFactors.append (tmp);
    result= Union (result, smallFactors);
    return result;
  }
  minBound= bounds[0];
  for (int i= 1; i < d; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  if (minBound > 16 || result.length() == 0)
  {
    result= Union (result, smallFactors);
    CanonicalForm MODl= power (y, degree (F) + 1);
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
#endif

#ifdef HAVE_NTL // henselLiftAndLatticeRecombi
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
      (size (A) == 2 && igcd (degree (A), degree (A,1))==1))
  {
    factors.append (A);

    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                          false, false, N);

    if (!extension)
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
      if (!extension)
        normalize (factorsG);
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
      if (!extension)
        normalize (factorsG);
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

  int boundsLength;
  bool isIrreducible= false;
  int * bounds= computeBounds (A, boundsLength, isIrreducible);
  if (isIrreducible)
  {
    delete [] bounds;
    factors.append (A);

    appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                          swap, false, N);

    if (!extension)
      normalize (factors);
    return factors;
  }

  int minBound= bounds[0];
  for (int i= 1; i < boundsLength; i++)
  {
    if (bounds[i] != 0)
      minBound= tmin (minBound, bounds[i]);
  }

  int boundsLength2;
  int * bounds2= computeBoundsWrtDiffMainvar (A, boundsLength2, isIrreducible);
  int minBound2= bounds2[0];
  for (int i= 1; i < boundsLength2; i++)
  {
    if (bounds2[i] != 0)
      minBound2= tmin (minBound2, bounds2[i]);
  }


  bool fail= false;
  CanonicalForm Aeval, evaluation, bufAeval, bufEvaluation, buf, tmp;
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
  bool symmetric= false;

  TIMING_START (fac_fq_uni_total);
  for (int i= 0; i < factorNums; i++)
  {
    bufAeval= A;
    TIMING_START (fac_fq_bi_evaluation);
    bufEvaluation= evalPoint (A, bufAeval, alpha, list, GF, fail);
    TIMING_END_AND_PRINT (fac_fq_bi_evaluation, "time to find eval point: ");
    if (!derivXZero && !fail2 && !symmetric)
    {
      if (i == 0)
      {
        buf= swapvar (A, x, y);
        symmetric= (A/Lc (A) == buf/Lc (buf));
      }
      bufAeval2= buf;
      TIMING_START (fac_fq_bi_evaluation);
      bufEvaluation2= evalPoint (buf, bufAeval2, alpha, list2, GF, fail2);
      TIMING_END_AND_PRINT (fac_fq_bi_evaluation,
                            "time to find eval point wrt y: ");
    }
    // first try to change main variable if there is no valid evaluation point
    if (fail && (i == 0))
    {
      if (!derivXZero && !fail2 && !symmetric)
      {
        bufEvaluation= bufEvaluation2;
        int dummy= subCheck2;
        subCheck2= subCheck1;
        subCheck1= dummy;
        tmp= A;
        A= buf;
        buf= tmp;
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
      delete [] bounds;
      delete [] bounds2;
      return factors;
    }

    // there is at least one valid evaluation point
    // but we do not compute more univariate factorization over an extension
    if (fail && (i != 0))
      break;

    // univariate factorization
    TIMING_START (fac_fq_uni_factorizer);
    bufUniFactors= uniFactorizer (bufAeval, alpha, GF);
    TIMING_END_AND_PRINT (fac_fq_uni_factorizer,
                          "time for univariate factorization over Fq: ");
    DEBOUTLN (cerr, "Lc (bufAeval)*prod (bufUniFactors)== bufAeval " <<
              (prod (bufUniFactors)*Lc (bufAeval) == bufAeval));

    if (!derivXZero && !fail2 && !symmetric)
    {
      TIMING_START (fac_fq_uni_factorizer);
      bufUniFactors2= uniFactorizer (bufAeval2, alpha, GF);
      TIMING_END_AND_PRINT (fac_fq_uni_factorizer,
                            "time for univariate factorization in y over Fq: ");
      DEBOUTLN (cerr, "Lc (bufAeval2)*prod (bufUniFactors2)== bufAeval2 " <<
                (prod (bufUniFactors2)*Lc (bufAeval2) == bufAeval2));
    }

    if (bufUniFactors.length() == 1 ||
        (!fail2 && !derivXZero && !symmetric && (bufUniFactors2.length() == 1)))
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

      if (!extension)
        normalize (factors);
      delete [] bounds;
      delete [] bounds2;
      return factors;
    }

    if (i == 0 && !extension)
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
          if (!extension)
            normalize (factors);
          delete [] bounds;
          delete [] bounds2;
          return factors;
        }
      }

      if (!derivXZero && !fail2 && !symmetric && subCheck2 > 0)
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
          if (!extension)
            normalize (factors);
          delete [] bounds;
          delete [] bounds2;
          return factors;
        }
      }
    }

    // degree analysis
    bufDegs = DegreePattern (bufUniFactors);
    if (!derivXZero && !fail2 && !symmetric)
      bufDegs2= DegreePattern (bufUniFactors2);

    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      uniFactors= bufUniFactors;
      degs= bufDegs;
      if (!derivXZero && !fail2 && !symmetric)
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
      if (!derivXZero && !fail2 && !symmetric)
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
    if (!derivXZero && !fail2 && !symmetric)
      list2.append (bufEvaluation2);
  }
  TIMING_END_AND_PRINT (fac_fq_uni_total,
                        "total time for univariate factorizations: ");

  if (!derivXZero && !fail2 && !symmetric)
  {
    if ((uniFactors.length() > uniFactors2.length() && minBound2 <= minBound)||
        (uniFactors.length() == uniFactors2.length()
         && degs.getLength() > degs2.getLength() && minBound2 <= minBound))
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
    if (!extension)
      normalize (factors);
    delete [] bounds;
    delete [] bounds2;
    return factors;
  }

  int liftBound= degree (A, y) + 1;

  if (swap2)
  {
    delete [] bounds;
    bounds= bounds2;
    minBound= minBound2;
  }

  TIMING_START (fac_fq_bi_shift_to_zero);
  A= A (y + evaluation, y);
  TIMING_END_AND_PRINT (fac_fq_bi_shift_to_zero,
                        "time to shift eval to zero: ");

  int degMipo= 1;
  if (extension && alpha.level() != 1 && k==1)
    degMipo= degree (getMipo (alpha));

  DEBOUTLN (cerr, "uniFactors= " << uniFactors);

  if ((GF && !extension) || (GF && extension && k != 1))
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_fq_bi_hensel_lift);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_fq_bi_hensel_lift,
                          "time for bivariate hensel lifting over Fq: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);

    CanonicalForm MODl= power (y, liftBound);

    TIMING_START (fac_fq_bi_factor_recombination);
    if (extension)
      factors= extFactorRecombination (uniFactors, A, MODl, info, degs,
                                       evaluation, 1, uniFactors.length()/2);
    else
      factors= factorRecombination (uniFactors, A, MODl, degs, evaluation, 1,
                                    uniFactors.length()/2);
    TIMING_END_AND_PRINT (fac_fq_bi_factor_recombination,
                          "time for naive bivariate factor recombi over Fq: ");

    if (earlySuccess)
      factors= Union (earlyFactors, factors);
    else if (!earlySuccess && degs.getLength() == 1)
      factors= earlyFactors;
  }
  else if (degree (A) > 4 && beta.level() == 1 && (2*minBound)/degMipo < 32)
  {
    TIMING_START (fac_fq_bi_hensel_lift);
    if (extension)
    {
      CFList lll= extHenselLiftAndLatticeRecombi (A, uniFactors, info, degs,
                                                  evaluation
                                                 );
      factors= Union (lll, factors);
    }
    else if (alpha.level() == 1 && !GF)
    {
      CFList lll= henselLiftAndLatticeRecombi (A, uniFactors, alpha, degs,
                                               symmetric, evaluation);
      factors= Union (lll, factors);
    }
    else if (!extension && (alpha != x || GF))
    {
      CFList lll= henselLiftAndLatticeRecombi (A, uniFactors, alpha, degs,
                                               symmetric, evaluation);
      factors= Union (lll, factors);
    }
    TIMING_END_AND_PRINT (fac_fq_bi_hensel_lift,
                          "time to bivar lift and LLL recombi over Fq: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);
  }
  else
  {
    bool earlySuccess= false;
    CFList earlyFactors;
    TIMING_START (fac_fq_bi_hensel_lift);
    uniFactors= henselLiftAndEarly
               (A, earlySuccess, earlyFactors, degs, liftBound,
                uniFactors, info, evaluation);
    TIMING_END_AND_PRINT (fac_fq_bi_hensel_lift,
                          "time for bivar hensel lifting over Fq: ");
    DEBOUTLN (cerr, "lifted factors= " << uniFactors);

    CanonicalForm MODl= power (y, liftBound);
    if (!extension)
    {
      TIMING_START (fac_fq_bi_factor_recombination);
      factors= factorRecombination (uniFactors, A, MODl, degs, evaluation, 1, 3);
      TIMING_END_AND_PRINT (fac_fq_bi_factor_recombination,
                            "time for small subset naive recombi over Fq: ");

      int oldUniFactorsLength= uniFactors.length();
      if (degree (A) > 0)
      {
        CFList tmp;
        TIMING_START (fac_fq_bi_hensel_lift);
        if (alpha.level() == 1)
          tmp= increasePrecision (A, uniFactors, 0, uniFactors.length(), 1,
                                  liftBound, evaluation
                                 );
        else
        {
          if (degree (A) > getCharacteristic())
            tmp= increasePrecisionFq2Fp (A, uniFactors, 0, uniFactors.length(),
                                         1, alpha, liftBound, evaluation
                                        );
          else
            tmp= increasePrecision (A, uniFactors, 0, uniFactors.length(), 1,
                                    alpha, liftBound, evaluation
                                   );
        }
        TIMING_END_AND_PRINT (fac_fq_bi_hensel_lift,
                              "time to increase precision: ");
        factors= Union (factors, tmp);
        if (tmp.length() == 0 || (tmp.length() > 0 && uniFactors.length() != 0
                                  && uniFactors.length() != oldUniFactorsLength)
           )
        {
          DegreePattern bufDegs= DegreePattern (uniFactors);
          degs.intersect (bufDegs);
          degs.refine ();
          factors= Union (factors, factorRecombination (uniFactors, A, MODl,
                                                        degs, evaluation, 4,
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

  if (!swap2)
    delete [] bounds2;
  delete [] bounds;

  appendSwapDecompress (factors, contentAxFactors, contentAyFactors,
                        swap, swap2, N);
  if (!extension)
    normalize (factors);

  return factors;
}
#endif

#ifdef HAVE_NTL // primitiveElement
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
      prune (vBuf);
    }
    else // not able to pass to GF, pass to F_p(\alpha)
    {
      CanonicalForm mipo= randomIrredpoly (2, x);
      Variable v= rootOf (mipo);
      ExtensionInfo info2= ExtensionInfo (v);
      factors= biFactorize (A, info2);
      prune (v);
    }
    return factors;
  }
  else if (!GF && (alpha != x)) // we are in F_p(\alpha)
  {
    if (k == 1) // need factorization over F_p
    {
      int extDeg= degree (getMipo (alpha));
      extDeg++;
      CanonicalForm mipo= randomIrredpoly (extDeg, x);
      Variable v= rootOf (mipo);
      ExtensionInfo info2= ExtensionInfo (v);
      factors= biFactorize (A, info2);
      prune (v);
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
        ExtensionInfo info2= ExtensionInfo (v, beta, imPrimElem, delta);
        factors= biFactorize (bufA, info2);
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
        prune (vBuf);
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
        prune (vBuf);
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
        prune (v1);
      }
    }
    return factors;
  }
}
#endif
#endif
