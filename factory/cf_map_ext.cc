// -*- c++ -*-
//*****************************************************************************
/** @file cf_map_ext.cc
 *
 * @author Martin Lee
 * @date   16.11.2009
 *
 * This file implements functions to map between extensions of finite fields
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
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

#ifdef HAVE_NTL
/// \f$ F_{p} (\alpha ) \subset F_{p}(\beta ) \f$ and \f$ \alpha \f$ is a
/// primitive element, returns the image of \f$ \alpha \f$
static inline
CanonicalForm mapUp (const Variable& alpha, const Variable& beta)
{
  int p= getCharacteristic ();
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
}

#endif

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

/// changes representation by primitive element to representation by residue
/// classes modulo a Conway polynomial
CanonicalForm GF2FalphaRep (const CanonicalForm& F, const Variable& alpha)
{
  Variable beta= rootOf (gf_mipo);
  return GF2FalphaHelper (F, beta) (alpha, beta);
}

/// change representation by residue classes modulo a Conway polynomial
/// to representation by primitive element
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

/// maps a polynomial over \f$ GF(p^{k}) \f$ to a polynomial over
/// \f$ GF(p^{d}) \f$ , d needs to be a multiple of k
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

/// maps a polynomial over \f$ GF(p^{d}) \f$ to a polynomial over
/// \f$ GF(p^{k})\f$ , d needs to be a multiple of k
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

#ifdef HAVE_NTL
/// determine a primitive element of \f$ F_{p} (\alpha ) \f$,
/// \f$ \beta \f$ is a primitive element of a field which is isomorphic to
/// \f$ F_{p}(\alpha ) \f$
CanonicalForm
primitiveElement (const Variable& alpha, Variable& beta, bool fail)
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
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTL_mipo;
  CanonicalForm mipo2;
  primitive= false;
  fail= false;
  do
  {
    BuildIrred (NTL_mipo, d);
    mipo2= convertNTLzzpX2CF (NTL_mipo, Variable (1));
    beta= rootOf (mipo2);
    primitive= isPrimitive (beta, fail);
    if (primitive)
      break;
    if (fail)
      return 0;
  } while (1);
  zz_pX alpha_mipo= convertFacCF2NTLzzpX (mipo);
  zz_pE::init (alpha_mipo);
  zz_pEX NTL_beta_mipo= to_zz_pEX (NTL_mipo);
  zz_pE root= FindRoot (NTL_beta_mipo);
  return convertNTLzzpE2CF (root, alpha);
}
#endif

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

#ifdef HAVE_NTL
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
  }
}

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
}

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
