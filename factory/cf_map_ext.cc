// -*- c++ -*-
//*****************************************************************************
/** @file cf_map_ext.cc
 *
 * This file implements functions to map between extensions of finite fields
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
 * @date   16.11.2009
**/
//*****************************************************************************


#include "config.h"


#include "cf_assert.h"
#include "debug.h"

#include "canonicalform.h"
#include "cf_util.h"
#include "imm.h"
#include "cf_iter.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

// cyclotomoic polys:
#include "cf_cyclo.h"

#include "cf_map_ext.h"

/// helper function
int findItem (const CFList& list, const CanonicalForm& item)
{
  int result= 1;
  for (CFListIterator i= list; i.hasItem(); i++, result++)
  {
    if (i.getItem() == item)
      return result;
  }
  return 0;
}

/// helper function
CanonicalForm getItem (const CFList& list, const int& pos)
{
  int j= 1;
  if ((pos > 0) && (pos <= list.length()))
  {
    for (CFListIterator i= list; j <= pos; i++, j++)
    {
      if (j == pos)
        return i.getItem();
    }
  }
  return 0;
}

/// \f$ F_{p} (\alpha ) \subset F_{p}(\beta ) \f$ and \f$ \alpha \f$ is a
/// primitive element, returns the image of \f$ \alpha \f$
static inline
CanonicalForm mapUp (const Variable& alpha, const Variable& beta)
{
  int p= getCharacteristic ();
  #ifdef HAVE_FLINT
    // convert mipo1
    nmod_poly_t mipo1;
    convertFacCF2nmod_poly_t(mipo1,getMipo(beta));
    fq_nmod_ctx_t ctx;
    fq_nmod_ctx_init_modulus(ctx,mipo1,"t");
    nmod_poly_clear(mipo1);
    // convert mipo2 (alpah)
    fq_nmod_poly_t mipo2;
    convertFacCF2Fq_nmod_poly_t(mipo2,getMipo(alpha),ctx);
    fq_nmod_poly_factor_t fac;
    fq_nmod_poly_factor_init(fac,ctx);
    fq_nmod_poly_roots(fac, mipo2, 0, ctx);
    // root of first (linear) factor: -absolute Term
    fq_nmod_t r0;
    fq_nmod_init(r0, ctx);
    fq_nmod_poly_get_coeff(r0,fac->poly,0,ctx);
    fq_nmod_neg(r0, r0, ctx);
    // convert
    CanonicalForm r1=convertFq_nmod_t2FacCF(r0,beta);
    // cleanup
    fq_nmod_poly_factor_clear(fac,ctx);
    fq_nmod_clear(r0, ctx);
    fq_nmod_poly_clear(mipo2,ctx);
    fq_nmod_ctx_clear(ctx);
    return r1;
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTL_mipo= convertFacCF2NTLzzpX (getMipo (beta));
  zz_pE::init (NTL_mipo);
  zz_pEX NTL_alpha_mipo= convertFacCF2NTLzz_pEX (getMipo(alpha), NTL_mipo);
  zz_pE root= FindRoot (NTL_alpha_mipo);
  return convertNTLzzpE2CF (root, beta);
  #endif
}


/// the CanonicalForm G is the output of map_up, returns F considered as an
/// element over \f$ F_{p}(\alpha ) \f$, WARNING: make sure coefficients of F
/// are really elements of a subfield of \f$ F_{p}(\beta ) \f$ which is
/// isomorphic to \f$ F_{p}(\alpha ) \f$
static inline
CanonicalForm
mapDown (const CanonicalForm& F, const Variable& alpha, const
          CanonicalForm& G, CFList& source, CFList& dest)
{
  CanonicalForm buf, buf2;
  int counter= 0;
  int pos;
  int p= getCharacteristic();
  int d= degree(getMipo(alpha));
  int bound= ipower(p, d);
  CanonicalForm result= 0;
  CanonicalForm remainder;
  CanonicalForm alpha_power;
  if (degree(F) == 0) return F;
  if (F.level() < 0 && F.isUnivariate())
  {
    buf= F;
    remainder= mod (buf, G);
    ASSERT (remainder.isZero(), "alpha is not primitive");
    pos= findItem (source, buf);
    if (pos == 0)
      source.append (buf);
    buf2= buf;
    while (degree (buf) != 0 && counter < bound)
    {
      buf /= G;
      counter++;
      if (buf == buf2) break;
    }
    ASSERT (counter >= bound, "alpha is not primitive");
    if (pos == 0)
    {
      alpha_power= power (alpha, counter);
      dest.append (alpha_power);
    }
    else
      alpha_power= getItem (dest, pos);
    result = alpha_power;
    return result;
  }
  else
  {
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      buf= mapDown (i.coeff(), alpha, G, source, dest);
      result += buf*power(F.mvar(), i.exp());
    }
    return result;
  }
}

/// helper function
static inline
CanonicalForm GF2FalphaHelper (const CanonicalForm& F, const Variable& alpha)
{
  if (F.isZero())
    return 0;
  int exp;
  CanonicalForm result= 0;
  InternalCF* buf;
  if (F.inBaseDomain())
  {
    if (F.isOne()) return 1;
    buf= F.getval();
    exp= imm2int(buf);
    result= power (alpha, exp).mapinto();
    return result;
  }
  for (CFIterator i= F; i.hasTerms(); i++)
    result += GF2FalphaHelper (i.coeff(), alpha)*power (F.mvar(), i.exp());
  return result;
}

CanonicalForm GF2FalphaRep (const CanonicalForm& F, const Variable& alpha)
{
  Variable beta= rootOf (gf_mipo);
  CanonicalForm result= GF2FalphaHelper (F, beta) (alpha, beta);
  prune (beta);
  return result;
}

CanonicalForm Falpha2GFRep (const CanonicalForm& F)
{
  CanonicalForm result= 0;
  InternalCF* buf;

  if (F.inCoeffDomain())
  {
    if (F.inBaseDomain())
      return F.mapinto();
    else
    {
      for (CFIterator i= F; i.hasTerms(); i++)
      {
        buf= int2imm_gf (i.exp());
        result += i.coeff().mapinto()*CanonicalForm (buf);
      }
    }
    return result;
  }
  for (CFIterator i= F; i.hasTerms(); i++)
    result += Falpha2GFRep (i.coeff())*power (F.mvar(), i.exp());
  return result;
}

/// GF_map_up helper
static inline
CanonicalForm GFPowUp (const CanonicalForm & F, int k)
{
  if (F.isOne()) return F;
  CanonicalForm result= 0;
  if (F.inBaseDomain())
    return power(F, k);
  for (CFIterator i= F; i.hasTerms(); i++)
    result += GFPowUp (i.coeff(), k)*power (F.mvar(), i.exp());
  return result;
}

CanonicalForm GFMapUp (const CanonicalForm & F, int k)
{
  int d= getGFDegree();
  ASSERT (d%k == 0, "multiple of GF degree expected");
  int p= getCharacteristic();
  int ext_field_size= ipower (p, d);
  int field_size= ipower ( p, k);
  int diff= (ext_field_size - 1)/(field_size - 1);
  return GFPowUp (F, diff);
}

/// GFMapDown helper
static inline
CanonicalForm GFPowDown (const CanonicalForm & F, int k)
{
  if (F.isOne()) return F;
  CanonicalForm result= 0;
  int exp;
  InternalCF* buf;
  if (F.inBaseDomain())
  {
    buf= F.getval();
    exp= imm2int (buf);
    if ((exp % k) == 0)
      exp= exp/k;
    else
      return -1;

    buf= int2imm_gf (exp);
    return CanonicalForm (buf);
  }
  for (CFIterator i= F; i.hasTerms(); i++)
    result += GFPowDown (i.coeff(), k)*power (F.mvar(), i.exp());
  return result;
}

CanonicalForm GFMapDown (const CanonicalForm & F, int k)
{
  int d= getGFDegree();
  ASSERT (d % k == 0, "multiple of GF degree expected");
  int p= getCharacteristic();
  int ext_field_size= ipower (p, d);
  int field_size= ipower ( p, k);
  int diff= (ext_field_size - 1)/(field_size - 1);
  return GFPowDown (F, diff);
}

/// map F in \f$ F_{p} (\alpha ) \f$ which is generated by G into some
/// \f$ F_{p}(\beta ) \f$ which is generated by H
static inline
CanonicalForm mapUp (const CanonicalForm& F, const CanonicalForm& G,
                      const Variable& alpha, const CanonicalForm& H,
                      CFList& source, CFList& dest)
{
  CanonicalForm buf, buf2;
  int counter= 0;
  int pos;
  int p= getCharacteristic();
  int d= degree (getMipo(alpha));
  int bound= ipower(p, d);
  CanonicalForm result= 0;
  CanonicalForm remainder;
  CanonicalForm H_power;
  if (degree(F) <= 0) return F;
  if (F.level() < 0 && F.isUnivariate())
  {
    buf= F;
    remainder= mod (buf, G);
    ASSERT (remainder.isZero(), "alpha is not primitive");
    pos= findItem (source, buf);
    if (pos == 0)
      source.append (buf);
    buf2= buf;
    while (degree (buf) != 0 && counter < bound)
    {
      buf /= G;
      counter++;
      if (buf == buf2) break;
    }
    ASSERT (counter <= bound, "alpha is not primitive");
    if (pos == 0)
    {
      H_power= buf*power (H, counter);
      dest.append (H_power);
    }
    else
      H_power= getItem (dest, pos);
    result = H_power;
    return result;
  }
  else
  {
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      buf= mapUp (i.coeff(), G, alpha, H, source, dest);
      result += buf*power(F.mvar(), i.exp());
    }
    return result;
  }
}

CanonicalForm
primitiveElement (const Variable& alpha, Variable& beta, bool& fail)
{
  bool primitive= false;
  fail= false;
  primitive= isPrimitive (alpha, fail);
  if (fail)
    return 0;
  if (primitive)
  {
    beta= alpha;
    return alpha;
  }
  CanonicalForm mipo= getMipo (alpha);
  int d= degree (mipo);
  int p= getCharacteristic ();
  #ifdef HAVE_FLINT
  nmod_poly_t FLINT_mipo;
  nmod_poly_init(FLINT_mipo,p);
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTL_mipo;
  #endif
  CanonicalForm mipo2;
  primitive= false;
  fail= false;
  bool initialized= false;
  do
  {
    #ifdef HAVE_FLINT
    nmod_poly_randtest_monic_irreducible(FLINT_mipo, FLINTrandom, d+1);
    mipo2=convertnmod_poly_t2FacCF(FLINT_mipo,Variable(1));
    #elif defined(HAVE_NTL)
    BuildIrred (NTL_mipo, d);
    mipo2= convertNTLzzpX2CF (NTL_mipo, Variable (1));
    #endif
    if (!initialized)
      beta= rootOf (mipo2);
    else
      setMipo (beta, mipo2);
    primitive= isPrimitive (beta, fail);
    if (primitive)
      break;
    if (fail)
      return 0;
  } while (1);
  #ifdef HAVE_FLINT
  nmod_poly_clear(FLINT_mipo);
  // convert alpha_mipo
  nmod_poly_t alpha_mipo;
  convertFacCF2nmod_poly_t(alpha_mipo,mipo);
  fq_nmod_ctx_t ctx;
  fq_nmod_ctx_init_modulus(ctx,alpha_mipo,"t");
  nmod_poly_clear(alpha_mipo);
  // convert beta_mipo (mipo2)
  fq_nmod_poly_t FLINT_beta_mipo;
  convertFacCF2Fq_nmod_poly_t(FLINT_beta_mipo,mipo2,ctx);
  fq_nmod_poly_factor_t fac;
  fq_nmod_poly_factor_init(fac,ctx);
  fq_nmod_poly_roots(fac, FLINT_beta_mipo, 0, ctx);
  // root of first (linear) factor: -absolute Term
  fq_nmod_t r0;
  fq_nmod_init(r0, ctx);
  fq_nmod_poly_get_coeff(r0,fac->poly,0,ctx);
  fq_nmod_neg(r0, r0, ctx);
  // convert
  CanonicalForm r1=convertFq_nmod_t2FacCF(r0,alpha);
  // cleanup
  fq_nmod_poly_factor_clear(fac,ctx);
  fq_nmod_clear(r0, ctx);
  fq_nmod_poly_clear(FLINT_beta_mipo,ctx);
  fq_nmod_ctx_clear(ctx);
  return r1;
  #elif defined(NTL)
  zz_pX alpha_mipo= convertFacCF2NTLzzpX (mipo);
  zz_pE::init (alpha_mipo);
  zz_pEX NTL_beta_mipo= to_zz_pEX (NTL_mipo);
  zz_pE root= FindRoot (NTL_beta_mipo);
  return convertNTLzzpE2CF (root, alpha);
  #endif
}

CanonicalForm
mapDown (const CanonicalForm& F, const CanonicalForm& prim_elem, const
          CanonicalForm& im_prim_elem, const Variable& alpha, CFList& source,
          CFList& dest)
{
  return mapUp (F, im_prim_elem, alpha, prim_elem, dest, source);
}

CanonicalForm
mapUp (const CanonicalForm& F, const Variable& alpha, const Variable& /*beta*/,
        const CanonicalForm& prim_elem, const CanonicalForm& im_prim_elem,
        CFList& source, CFList& dest)
{
  if (prim_elem == alpha)
    return F (im_prim_elem, alpha);
  return mapUp (F, prim_elem, alpha, im_prim_elem, source, dest);
}

#ifdef HAVE_NTL // findMinPoly
CanonicalForm
mapPrimElem (const CanonicalForm& primElem, const Variable& alpha,
             const Variable& beta)
{
  if (primElem == alpha)
    return mapUp (alpha, beta);
  else
  {
    CanonicalForm primElemMipo= findMinPoly (primElem, alpha);
    int p= getCharacteristic ();
    #ifdef HAVE_FLINT
    // convert mipo1
    nmod_poly_t mipo1;
    convertFacCF2nmod_poly_t(mipo1,getMipo(beta));
    fq_nmod_ctx_t ctx;
    fq_nmod_ctx_init_modulus(ctx,mipo1,"t");
    nmod_poly_clear(mipo1);
    // convert mipo2 (primElemMipo)
    fq_nmod_poly_t mipo2;
    convertFacCF2Fq_nmod_poly_t(mipo2,primElemMipo,ctx);
    fq_nmod_poly_factor_t fac;
    fq_nmod_poly_factor_init(fac,ctx);
    fq_nmod_poly_roots(fac, mipo2, 0, ctx);
    // root of first (linear) factor: -absolute Term
    fq_nmod_t r0;
    fq_nmod_init(r0, ctx);
    fq_nmod_poly_get_coeff(r0,fac->poly,0,ctx);
    fq_nmod_neg(r0, r0, ctx);
    // convert
    CanonicalForm r1=convertFq_nmod_t2FacCF(r0,beta);
    // cleanup
    fq_nmod_poly_factor_clear(fac,ctx);
    fq_nmod_clear(r0, ctx);
    fq_nmod_poly_clear(mipo2,ctx);
    fq_nmod_ctx_clear(ctx);
    return r1;
    #elif defined(HAVE_NTL)
    if (fac_NTL_char != p)
    {
      fac_NTL_char= p;
      zz_p::init (p);
    }
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (beta));
    zz_pE::init (NTLMipo);
    zz_pEX NTLPrimElemMipo= convertFacCF2NTLzz_pEX (primElemMipo, NTLMipo);
    zz_pE root= FindRoot (NTLPrimElemMipo);
    return convertNTLzzpE2CF (root, beta);
    #endif
  }
}
#endif

CanonicalForm
map (const CanonicalForm& primElem, const Variable& alpha,
     const CanonicalForm& F, const Variable& beta)
{
  CanonicalForm G= F;
  int order= 0;
  while (!G.isOne())
  {
    G /= primElem;
    order++;
  }
  int p= getCharacteristic ();
  #ifdef HAVE_FLINT
  // convert mipo
  nmod_poly_t mipo1;
  convertFacCF2nmod_poly_t(mipo1,getMipo(beta));
  fq_nmod_ctx_t ctx;
  fq_nmod_ctx_init_modulus(ctx,mipo1,"t");
  nmod_poly_clear(mipo1);
  // convert mipo2 (alpha)
  fq_nmod_poly_t mipo2;
  convertFacCF2Fq_nmod_poly_t(mipo2,getMipo(alpha),ctx);
  fq_nmod_poly_factor_t fac;
  fq_nmod_poly_factor_init(fac,ctx);
  fq_nmod_poly_roots(fac, mipo2, 0, ctx);
  // roots in fac, #=fac->num
  int ind=-1;
  fq_nmod_t r0,FLINTbeta;
  fq_nmod_init(r0, ctx);
  fq_nmod_init(FLINTbeta, ctx);
  convertFacCF2Fq_nmod_t(FLINTbeta,beta,ctx);
  fmpz_t FLINTorder;
  fmpz_set_si(FLINTorder,order);
  for(int i=0;i< fac->num;i++)
  {
    // get the root (-abs.term of linear factor)
    fq_nmod_poly_get_coeff(r0,fac->poly+i,0,ctx);
    fq_nmod_neg(r0,r0,ctx);
    // r^order
    fq_nmod_pow(r0,r0,FLINTorder,ctx);
    // ==beta?
    if (fq_nmod_equal(r0,FLINTbeta,ctx))
    {
       ind=i;
       break;
    }
  }
  fmpz_clear(FLINTorder);
  // convert
  fq_nmod_poly_get_coeff(r0,fac->poly+ind,0,ctx);
  fq_nmod_neg(r0,r0,ctx);
  CanonicalForm r1=convertFq_nmod_t2FacCF(r0,beta);
  // cleanup
  fq_nmod_poly_factor_clear(fac,ctx);
  fq_nmod_clear(r0, ctx);
  fq_nmod_clear(FLINTbeta,ctx);
  fq_nmod_poly_clear(mipo2,ctx);
  fq_nmod_ctx_clear(ctx);
  return r1;
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTL_mipo= convertFacCF2NTLzzpX (getMipo (beta));
  zz_pE::init (NTL_mipo);
  zz_pEX NTL_alpha_mipo= convertFacCF2NTLzz_pEX (getMipo(alpha), NTL_mipo);
  zz_pE NTLBeta= to_zz_pE (convertFacCF2NTLzzpX (beta));
  vec_zz_pE roots= FindRoots (NTL_alpha_mipo);
  long ind=-1;
  for (long i= 0; i < roots.length(); i++)
  {
    if (power (roots [i], order)== NTLBeta)
    {
      ind= i;
      break;
    }
  }
  return (convertNTLzzpE2CF (roots[ind], beta));
  #endif
}

#ifdef HAVE_NTL
CanonicalForm
findMinPoly (const CanonicalForm& F, const Variable& alpha)
{
  ASSERT (F.isUnivariate() && F.mvar()==alpha,"expected element of F_p(alpha)");

  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLF= convertFacCF2NTLzzpX (F);
  int d= degree (getMipo (alpha));

  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo(alpha));
  zz_pE::init (NTLMipo);
  vec_zz_p pows;
  pows.SetLength (2*d);

  zz_pE powNTLF;
  set (powNTLF);
  zz_pE NTLFE= to_zz_pE (NTLF);
  zz_pX buf;
  for (int i= 0; i < 2*d; i++)
  {
    buf= rep (powNTLF);
    buf.rep.SetLength (d);
    pows [i]= buf.rep[0];
    powNTLF *= NTLFE;
  }

  zz_pX NTLMinPoly;
  MinPolySeq (NTLMinPoly, pows, d);

  return convertNTLzzpX2CF (NTLMinPoly, Variable (1));
}

#endif
