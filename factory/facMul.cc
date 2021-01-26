/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facMul.cc
 *
 * This file implements functions for fast multiplication and division with
 * remainder.
 *
 * Nomenclature rules: kronSub* -> plain Kronecker substitution
 *                     reverseSubst* -> reverse Kronecker substitution
 *                     kronSubRecipro* -> reciprocal Kronecker substitution as
 *                                        described in D. Harvey "Faster
 *                                        polynomial multiplication via
 *                                        multipoint Kronecker substitution"
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "debug.h"

#include "config.h"


#include "canonicalform.h"
#include "facMul.h"
#include "cf_util.h"
#include "cf_iter.h"
#include "cf_algorithm.h"
#include "templates/ftmpl_functions.h"

#ifdef HAVE_NTL
#include <NTL/lzz_pEX.h>
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

// univariate polys
#if defined(HAVE_NTL) || defined(HAVE_FLINT)

#ifdef HAVE_FLINT
void kronSubQa (fmpz_poly_t result, const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  fmpz_poly_init2 (result, d*(degAy + 1));
  _fmpz_poly_set_length (result, d*(degAy + 1));
  CFIterator j;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inBaseDomain())
      convertCF2initFmpz (fmpz_poly_get_coeff_ptr (result, i.exp()*d), i.coeff());
    else
      for (j= i.coeff(); j.hasTerms(); j++)
        convertCF2initFmpz (fmpz_poly_get_coeff_ptr (result, i.exp()*d+j.exp()),
                        j.coeff());
  }
  _fmpz_poly_normalise(result);
}


CanonicalForm
reverseSubstQa (const fmpz_poly_t F, int d, const Variable& x,
                const Variable& alpha, const CanonicalForm& den)
{
  CanonicalForm result= 0;
  int i= 0;
  int degf= fmpz_poly_degree (F);
  int k= 0;
  int degfSubK;
  int repLength;
  fmpq_poly_t buf;
  fmpq_poly_t mipo;
  convertFacCF2Fmpq_poly_t (mipo, getMipo(alpha));
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    fmpq_poly_init2 (buf, repLength);
    _fmpq_poly_set_length (buf, repLength);
    _fmpz_vec_set (buf->coeffs, F->coeffs + k, repLength);
    _fmpq_poly_normalise (buf);
    fmpq_poly_rem (buf, buf, mipo);

    result += convertFmpq_poly_t2FacCF (buf, alpha)*power (x, i);
    fmpq_poly_clear (buf);
    i++;
    k= d*i;
  }
  fmpq_poly_clear (mipo);
  result /= den;
  return result;
}

CanonicalForm
mulFLINTQa (const CanonicalForm& F, const CanonicalForm& G,
            const Variable& alpha)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  CanonicalForm denA= bCommonDen (A);
  CanonicalForm denB= bCommonDen (B);

  A *= denA;
  B *= denB;
  int degAa= degree (A, alpha);
  int degBa= degree (B, alpha);
  int d= degAa + 1 + degBa;

  fmpz_poly_t FLINTA,FLINTB;
  kronSubQa (FLINTA, A, d);
  kronSubQa (FLINTB, B, d);

  fmpz_poly_mul (FLINTA, FLINTA, FLINTB);

  denA *= denB;
  A= reverseSubstQa (FLINTA, d, F.mvar(), alpha, denA);

  fmpz_poly_clear (FLINTA);
  fmpz_poly_clear (FLINTB);
  return A;
}

CanonicalForm
mulFLINTQ (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  CanonicalForm denA= bCommonDen (A);
  CanonicalForm denB= bCommonDen (B);

  A *= denA;
  B *= denB;
  fmpz_poly_t FLINTA,FLINTB;
  convertFacCF2Fmpz_poly_t (FLINTA, A);
  convertFacCF2Fmpz_poly_t (FLINTB, B);
  fmpz_poly_mul (FLINTA, FLINTA, FLINTB);
  denA *= denB;
  A= convertFmpz_poly_t2FacCF (FLINTA, F.mvar());
  A /= denA;
  fmpz_poly_clear (FLINTA);
  fmpz_poly_clear (FLINTB);

  return A;
}

/*CanonicalForm
mulFLINTQ2 (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  fmpq_poly_t FLINTA,FLINTB;
  convertFacCF2Fmpq_poly_t (FLINTA, A);
  convertFacCF2Fmpq_poly_t (FLINTB, B);

  fmpq_poly_mul (FLINTA, FLINTA, FLINTB);
  A= convertFmpq_poly_t2FacCF (FLINTA, F.mvar());
  fmpq_poly_clear (FLINTA);
  fmpq_poly_clear (FLINTB);
  return A;
}*/

CanonicalForm
divFLINTQ (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  fmpq_poly_t FLINTA,FLINTB;
  convertFacCF2Fmpq_poly_t (FLINTA, A);
  convertFacCF2Fmpq_poly_t (FLINTB, B);

  fmpq_poly_div (FLINTA, FLINTA, FLINTB);
  A= convertFmpq_poly_t2FacCF (FLINTA, F.mvar());

  fmpq_poly_clear (FLINTA);
  fmpq_poly_clear (FLINTB);
  return A;
}

CanonicalForm
modFLINTQ (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  fmpq_poly_t FLINTA,FLINTB;
  convertFacCF2Fmpq_poly_t (FLINTA, A);
  convertFacCF2Fmpq_poly_t (FLINTB, B);

  fmpq_poly_rem (FLINTA, FLINTA, FLINTB);
  A= convertFmpq_poly_t2FacCF (FLINTA, F.mvar());

  fmpq_poly_clear (FLINTA);
  fmpq_poly_clear (FLINTB);
  return A;
}

CanonicalForm
mulFLINTQaTrunc (const CanonicalForm& F, const CanonicalForm& G,
                 const Variable& alpha, int m)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  CanonicalForm denA= bCommonDen (A);
  CanonicalForm denB= bCommonDen (B);

  A *= denA;
  B *= denB;

  int degAa= degree (A, alpha);
  int degBa= degree (B, alpha);
  int d= degAa + 1 + degBa;

  fmpz_poly_t FLINTA,FLINTB;
  kronSubQa (FLINTA, A, d);
  kronSubQa (FLINTB, B, d);

  int k= d*m;
  fmpz_poly_mullow (FLINTA, FLINTA, FLINTB, k);

  denA *= denB;
  A= reverseSubstQa (FLINTA, d, F.mvar(), alpha, denA);
  fmpz_poly_clear (FLINTA);
  fmpz_poly_clear (FLINTB);
  return A;
}

CanonicalForm
mulFLINTQTrunc (const CanonicalForm& F, const CanonicalForm& G, int m)
{
  if (F.inCoeffDomain() && G.inCoeffDomain())
    return F*G;
  if (F.inCoeffDomain())
    return mod (F*G, power (G.mvar(), m));
  if (G.inCoeffDomain())
    return mod (F*G, power (F.mvar(), m));
  Variable alpha;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
    return mulFLINTQaTrunc (F, G, alpha, m);

  CanonicalForm A= F;
  CanonicalForm B= G;

  CanonicalForm denA= bCommonDen (A);
  CanonicalForm denB= bCommonDen (B);

  A *= denA;
  B *= denB;
  fmpz_poly_t FLINTA,FLINTB;
  convertFacCF2Fmpz_poly_t (FLINTA, A);
  convertFacCF2Fmpz_poly_t (FLINTB, B);
  fmpz_poly_mullow (FLINTA, FLINTA, FLINTB, m);
  denA *= denB;
  A= convertFmpz_poly_t2FacCF (FLINTA, F.mvar());
  A /= denA;
  fmpz_poly_clear (FLINTA);
  fmpz_poly_clear (FLINTB);

  return A;
}

CanonicalForm uniReverse (const CanonicalForm& F, int d, const Variable& x)
{
  if (d == 0)
    return F;
  if (F.inCoeffDomain())
    return F*power (x,d);
  CanonicalForm result= 0;
  CFIterator i= F;
  while (d - i.exp() < 0)
    i++;

  for (; i.hasTerms() && (d - i.exp() >= 0); i++)
    result += i.coeff()*power (x, d - i.exp());
  return result;
}

CanonicalForm
newtonInverse (const CanonicalForm& F, const int n, const Variable& x)
{
  int l= ilog2(n);

  CanonicalForm g;
  if (F.inCoeffDomain())
    g= F;
  else
    g= F [0];

  if (!F.inCoeffDomain())
    ASSERT (F.mvar() == x, "main variable of F and x differ");
  ASSERT (!g.isZero(), "expected a unit");

  if (!g.isOne())
    g = 1/g;
  CanonicalForm result;
  int exp= 0;
  if (n & 1)
  {
    result= g;
    exp= 1;
  }
  CanonicalForm h;

  for (int i= 1; i <= l; i++)
  {
    h= mulNTL (g, mod (F, power (x, (1 << i))));
    h= mod (h, power (x, (1 << i)) - 1);
    h= div (h, power (x, (1 << (i - 1))));
    g -= power (x, (1 << (i - 1)))*
         mulFLINTQTrunc (g, h, 1 << (i-1));

    if (n & (1 << i))
    {
      if (exp)
      {
        h= mulNTL (result, mod (F, power (x, exp + (1 << i))));
        h= mod (h, power (x, exp + (1 << i)) - 1);
        h= div (h, power (x, exp));
        result -= power(x, exp)*mulFLINTQTrunc (g, h, 1 << i);
        exp += (1 << i);
      }
      else
      {
        exp= (1 << i);
        result= g;
      }
    }
  }

  return result;
}

void
newtonDivrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
              CanonicalForm& R)
{
  ASSERT (F.level() == G.level(), "F and G have different level");
  CanonicalForm A= F;
  CanonicalForm B= G;
  Variable x= A.mvar();
  int degA= degree (A);
  int degB= degree (B);
  int m= degA - degB;

  if (m < 0)
  {
    R= A;
    Q= 0;
    return;
  }

  if (degB <= 1)
    divrem (A, B, Q, R);
  else
  {
    R= uniReverse (A, degA, x);

    CanonicalForm revB= uniReverse (B, degB, x);
    revB= newtonInverse (revB, m + 1, x);
    Q= mulFLINTQTrunc (R, revB, m + 1);
    Q= uniReverse (Q, m, x);

    R= A - mulNTL (Q, B);
  }
}

void
newtonDiv (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q)
{
  ASSERT (F.level() == G.level(), "F and G have different level");
  CanonicalForm A= F;
  CanonicalForm B= G;
  Variable x= A.mvar();
  int degA= degree (A);
  int degB= degree (B);
  int m= degA - degB;

  if (m < 0)
  {
    Q= 0;
    return;
  }

  if (degB <= 1)
    Q= div (A, B);
  else
  {
    CanonicalForm R= uniReverse (A, degA, x);
    CanonicalForm revB= uniReverse (B, degB, x);
    revB= newtonInverse (revB, m + 1, x);
    Q= mulFLINTQTrunc (R, revB, m + 1);
    Q= uniReverse (Q, m, x);
  }
}

#endif

CanonicalForm
mulNTL (const CanonicalForm& F, const CanonicalForm& G, const modpk& b)
{
  if (CFFactory::gettype() == GaloisFieldDomain)
    return F*G;
  if (getCharacteristic() == 0)
  {
    Variable alpha;
    if ((!F.inCoeffDomain() && !G.inCoeffDomain()) &&
        (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha)))
    {
      if (b.getp() != 0)
      {
        CanonicalForm mipo= getMipo (alpha);
        bool is_rat= isOn (SW_RATIONAL);
        if (!is_rat)
          On (SW_RATIONAL);
        mipo *=bCommonDen (mipo);
        if (!is_rat)
          Off (SW_RATIONAL);
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
        fmpz_t FLINTp;
        fmpz_mod_poly_t FLINTmipo;
        fq_ctx_t fq_con;
        fq_poly_t FLINTF, FLINTG;

        fmpz_init (FLINTp);

        convertCF2initFmpz (FLINTp, b.getpk());

        convertFacCF2Fmpz_mod_poly_t (FLINTmipo, mipo, FLINTp);

        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
        fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
        #else
        fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        #endif

        convertFacCF2Fq_poly_t (FLINTF, F, fq_con);
        convertFacCF2Fq_poly_t (FLINTG, G, fq_con);

        fq_poly_mul (FLINTF, FLINTF, FLINTG, fq_con);

        CanonicalForm result= convertFq_poly_t2FacCF (FLINTF, F.mvar(),
                                                      alpha, fq_con);

        fmpz_clear (FLINTp);
        fq_poly_clear (FLINTF, fq_con);
        fq_poly_clear (FLINTG, fq_con);
        fq_ctx_clear (fq_con);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTmipo,fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear(FLINTmipo);
        #endif
        return b (result);
#endif
#ifdef HAVE_NTL
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (mipo));
        ZZ_pE::init (NTLmipo);
        ZZ_pEX NTLg= convertFacCF2NTLZZ_pEX (G, NTLmipo);
        ZZ_pEX NTLf= convertFacCF2NTLZZ_pEX (F, NTLmipo);
        mul (NTLf, NTLf, NTLg);

        return b (convertNTLZZ_pEX2CF (NTLf, F.mvar(), alpha));
#endif
      }
#ifdef HAVE_FLINT
      CanonicalForm result= mulFLINTQa (F, G, alpha);
      return result;
#else
      return F*G;
#endif
    }
    else if (!F.inCoeffDomain() && !G.inCoeffDomain())
    {
#ifdef HAVE_FLINT
      if (b.getp() != 0)
      {
        fmpz_t FLINTpk;
        fmpz_init (FLINTpk);
        convertCF2initFmpz (FLINTpk, b.getpk());
        fmpz_mod_poly_t FLINTF, FLINTG;
        convertFacCF2Fmpz_mod_poly_t (FLINTF, F, FLINTpk);
        convertFacCF2Fmpz_mod_poly_t (FLINTG, G, FLINTpk);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTpk);
        fmpz_mod_poly_mul (FLINTF, FLINTF, FLINTG, fmpz_ctx);
        #else
        fmpz_mod_poly_mul (FLINTF, FLINTF, FLINTG);
        #endif
        CanonicalForm result= convertFmpz_mod_poly_t2FacCF (FLINTF, F.mvar(),b);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTG,fmpz_ctx);
        fmpz_mod_poly_clear (FLINTF,fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTG);
        fmpz_mod_poly_clear (FLINTF);
        #endif
        fmpz_clear (FLINTpk);
        return result;
      }
      return mulFLINTQ (F, G);
#endif
#ifdef HAVE_NTL
      if (b.getp() != 0)
      {
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZX ZZf= convertFacCF2NTLZZX (F);
        ZZX ZZg= convertFacCF2NTLZZX (G);
        ZZ_pX NTLf= to_ZZ_pX (ZZf);
        ZZ_pX NTLg= to_ZZ_pX (ZZg);
        mul (NTLf, NTLf, NTLg);
        return b (convertNTLZZX2CF (to_ZZX (NTLf), F.mvar()));
      }
      return F*G;
#endif
    }
    if (b.getp() != 0)
    {
      if (!F.inBaseDomain() && !G.inBaseDomain())
      {
        if (hasFirstAlgVar (G, alpha) || hasFirstAlgVar (F, alpha))
        {
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
          fmpz_t FLINTp;
          fmpz_mod_poly_t FLINTmipo;
          fq_ctx_t fq_con;

          fmpz_init (FLINTp);
          convertCF2initFmpz (FLINTp, b.getpk());

          convertFacCF2Fmpz_mod_poly_t (FLINTmipo, getMipo (alpha), FLINTp);

          #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
          fmpz_mod_ctx_t fmpz_ctx;
          fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
          fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
          #else
          fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
          #endif

          CanonicalForm result;

          if (F.inCoeffDomain() && !G.inCoeffDomain())
          {
            fq_poly_t FLINTG;
            fmpz_poly_t FLINTF;
            convertFacCF2Fmpz_poly_t (FLINTF, F);
            convertFacCF2Fq_poly_t (FLINTG, G, fq_con);

            fq_poly_scalar_mul_fq (FLINTG, FLINTG, FLINTF, fq_con);

            result= convertFq_poly_t2FacCF (FLINTG, G.mvar(), alpha, fq_con);
            fmpz_poly_clear (FLINTF);
            fq_poly_clear (FLINTG, fq_con);
          }
          else if (!F.inCoeffDomain() && G.inCoeffDomain())
          {
            fq_poly_t FLINTF;
            fmpz_poly_t FLINTG;

            convertFacCF2Fmpz_poly_t (FLINTG, G);
            convertFacCF2Fq_poly_t (FLINTF, F, fq_con);

            fq_poly_scalar_mul_fq (FLINTF, FLINTF, FLINTG, fq_con);

            result= convertFq_poly_t2FacCF (FLINTF, F.mvar(), alpha, fq_con);
            fmpz_poly_clear (FLINTG);
            fq_poly_clear (FLINTF, fq_con);
          }
          else
          {
            fq_t FLINTF, FLINTG;

            convertFacCF2Fq_t (FLINTF, F, fq_con);
            convertFacCF2Fq_t (FLINTG, G, fq_con);

            fq_mul (FLINTF, FLINTF, FLINTG, fq_con);

            result= convertFq_t2FacCF (FLINTF, alpha);
            fq_clear (FLINTF, fq_con);
            fq_clear (FLINTG, fq_con);
          }

          fmpz_clear (FLINTp);
          #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
          fmpz_mod_poly_clear (FLINTmipo,fmpz_ctx);
          fmpz_mod_ctx_clear(fmpz_ctx);
          #else
          fmpz_mod_poly_clear (FLINTmipo);
          #endif
          fq_ctx_clear (fq_con);

          return b (result);
#endif
#ifdef HAVE_NTL
          ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
          ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (alpha)));
          ZZ_pE::init (NTLmipo);

          if (F.inCoeffDomain() && !G.inCoeffDomain())
          {
            ZZ_pEX NTLg= convertFacCF2NTLZZ_pEX (G, NTLmipo);
            ZZ_pX NTLf= convertFacCF2NTLZZpX (F);
            mul (NTLg, to_ZZ_pE (NTLf), NTLg);
            return b (convertNTLZZ_pEX2CF (NTLg, G.mvar(), alpha));
          }
          else if (!F.inCoeffDomain() && G.inCoeffDomain())
          {
            ZZ_pX NTLg= convertFacCF2NTLZZpX (G);
            ZZ_pEX NTLf= convertFacCF2NTLZZ_pEX (F, NTLmipo);
            mul (NTLf, NTLf, to_ZZ_pE (NTLg));
            return b (convertNTLZZ_pEX2CF (NTLf, F.mvar(), alpha));
          }
          else
          {
            ZZ_pX NTLg= convertFacCF2NTLZZpX (G);
            ZZ_pX NTLf= convertFacCF2NTLZZpX (F);
            ZZ_pE result;
            mul (result, to_ZZ_pE (NTLg), to_ZZ_pE (NTLf));
            return b (convertNTLZZpX2CF (rep (result), alpha));
          }
#endif
        }
      }
      return b (F*G);
    }
    return F*G;
  }
  else if (F.inCoeffDomain() || G.inCoeffDomain())
    return F*G;
  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
#ifdef HAVE_NTL
#if (!defined(HAVE_FLINT) ||  __FLINT_RELEASE < 20400)
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
#endif
#endif
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
    if (!getReduce (alpha))
    {
      result= 0;
      for (CFIterator i= F; i.hasTerms(); i++)
        result += i.coeff()*G*power (F.mvar(),i.exp());
      return result;
    }
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
    nmod_poly_t FLINTmipo;
    fq_nmod_ctx_t fq_con;

    nmod_poly_init (FLINTmipo, getCharacteristic());
    convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

    fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

    fq_nmod_poly_t FLINTF, FLINTG;
    convertFacCF2Fq_nmod_poly_t (FLINTF, F, fq_con);
    convertFacCF2Fq_nmod_poly_t (FLINTG, G, fq_con);

    fq_nmod_poly_mul (FLINTF, FLINTF, FLINTG, fq_con);

    result= convertFq_nmod_poly_t2FacCF (FLINTF, F.mvar(), alpha, fq_con);

    fq_nmod_poly_clear (FLINTF, fq_con);
    fq_nmod_poly_clear (FLINTG, fq_con);
    nmod_poly_clear (FLINTmipo);
    fq_nmod_ctx_clear (fq_con);
    return result;
#elif defined(AHVE_NTL)
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    mul (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
    return result;
#endif
  }
  else
  {
#ifdef HAVE_FLINT
    nmod_poly_t FLINTF, FLINTG;
    convertFacCF2nmod_poly_t (FLINTF, F);
    convertFacCF2nmod_poly_t (FLINTG, G);
    nmod_poly_mul (FLINTF, FLINTF, FLINTG);
    result= convertnmod_poly_t2FacCF (FLINTF, F.mvar());
    nmod_poly_clear (FLINTF);
    nmod_poly_clear (FLINTG);
    return result;
#endif
#ifdef HAVE_NTL
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    mul (NTLF, NTLF, NTLG);
    return convertNTLzzpX2CF(NTLF, F.mvar());
#endif
  }
  return F*G;
}

CanonicalForm
modNTL (const CanonicalForm& F, const CanonicalForm& G, const modpk& b)
{
  if (CFFactory::gettype() == GaloisFieldDomain)
    return mod (F, G);
  if (F.inCoeffDomain() && G.isUnivariate() && !G.inCoeffDomain())
  {
    if (b.getp() != 0)
      return b(F);
    return F;
  }
  else if (F.inCoeffDomain() && G.inCoeffDomain())
  {
    if (b.getp() != 0)
      return b(F%G);
    return mod (F, G);
  }
  else if (F.isUnivariate() && G.inCoeffDomain())
  {
    if (b.getp() != 0)
      return b(F%G);
    return mod (F,G);
  }

  if (getCharacteristic() == 0)
  {
    Variable alpha;
    if (!hasFirstAlgVar (F, alpha) && !hasFirstAlgVar (G, alpha))
    {
#ifdef HAVE_FLINT
      if (b.getp() != 0)
      {
        fmpz_t FLINTpk;
        fmpz_init (FLINTpk);
        convertCF2initFmpz (FLINTpk, b.getpk());
        fmpz_mod_poly_t FLINTF, FLINTG;
        convertFacCF2Fmpz_mod_poly_t (FLINTF, F, FLINTpk);
        convertFacCF2Fmpz_mod_poly_t (FLINTG, G, FLINTpk);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTpk);
        fmpz_mod_poly_divrem (FLINTG, FLINTF, FLINTF, FLINTG, fmpz_ctx);
        #else
        fmpz_mod_poly_divrem (FLINTG, FLINTF, FLINTF, FLINTG);
        #endif
        CanonicalForm result= convertFmpz_mod_poly_t2FacCF (FLINTF,F.mvar(),b);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTG, fmpz_ctx);
        fmpz_mod_poly_clear (FLINTF, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTG);
        fmpz_mod_poly_clear (FLINTF);
        #endif
        fmpz_clear (FLINTpk);
        return result;
      }
      return modFLINTQ (F, G);
#else
      if (b.getp() != 0)
      {
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZX ZZf= convertFacCF2NTLZZX (F);
        ZZX ZZg= convertFacCF2NTLZZX (G);
        ZZ_pX NTLf= to_ZZ_pX (ZZf);
        ZZ_pX NTLg= to_ZZ_pX (ZZg);
        rem (NTLf, NTLf, NTLg);
        return b (convertNTLZZX2CF (to_ZZX (NTLf), F.mvar()));
      }
      return mod (F, G);
#endif
    }
    else
    {
      if (b.getp() != 0)
      {
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
        fmpz_t FLINTp;
        fmpz_mod_poly_t FLINTmipo;
        fq_ctx_t fq_con;
        fq_poly_t FLINTF, FLINTG;

        fmpz_init (FLINTp);

        convertCF2initFmpz (FLINTp, b.getpk());

        convertFacCF2Fmpz_mod_poly_t (FLINTmipo, getMipo (alpha), FLINTp);

        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
        fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
        #else
        fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        #endif

        convertFacCF2Fq_poly_t (FLINTF, F, fq_con);
        convertFacCF2Fq_poly_t (FLINTG, G, fq_con);

        fq_poly_rem (FLINTF, FLINTF, FLINTG, fq_con);

        CanonicalForm result= convertFq_poly_t2FacCF (FLINTF, F.mvar(),
                                                      alpha, fq_con);

        fmpz_clear (FLINTp);
        fq_poly_clear (FLINTF, fq_con);
        fq_poly_clear (FLINTG, fq_con);
        fq_ctx_clear (fq_con);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTmipo, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTmipo);
        #endif

        return b(result);
#else
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (alpha)));
        ZZ_pE::init (NTLmipo);
        ZZ_pEX NTLg= convertFacCF2NTLZZ_pEX (G, NTLmipo);
        ZZ_pEX NTLf= convertFacCF2NTLZZ_pEX (F, NTLmipo);
        rem (NTLf, NTLf, NTLg);
        return b (convertNTLZZ_pEX2CF (NTLf, F.mvar(), alpha));
#endif
      }
#ifdef HAVE_FLINT
      CanonicalForm Q, R;
      newtonDivrem (F, G, Q, R);
      return R;
#else
      return mod (F,G);
#endif
    }
  }

  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
#if (!defined(HAVE_FLINT) ||  __FLINT_RELEASE < 20400)
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
#endif
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
    nmod_poly_t FLINTmipo;
    fq_nmod_ctx_t fq_con;

    nmod_poly_init (FLINTmipo, getCharacteristic());
    convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

    fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

    fq_nmod_poly_t FLINTF, FLINTG;
    convertFacCF2Fq_nmod_poly_t (FLINTF, F, fq_con);
    convertFacCF2Fq_nmod_poly_t (FLINTG, G, fq_con);

    fq_nmod_poly_rem (FLINTF, FLINTF, FLINTG, fq_con);

    result= convertFq_nmod_poly_t2FacCF (FLINTF, F.mvar(), alpha, fq_con);

    fq_nmod_poly_clear (FLINTF, fq_con);
    fq_nmod_poly_clear (FLINTG, fq_con);
    nmod_poly_clear (FLINTmipo);
    fq_nmod_ctx_clear (fq_con);
#else
    zz_pX NTLMipo= convertFacCF2NTLzzpX(getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    rem (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
#endif
  }
  else
  {
#ifdef HAVE_FLINT
    nmod_poly_t FLINTF, FLINTG;
    convertFacCF2nmod_poly_t (FLINTF, F);
    convertFacCF2nmod_poly_t (FLINTG, G);
    nmod_poly_divrem (FLINTG, FLINTF, FLINTF, FLINTG);
    result= convertnmod_poly_t2FacCF (FLINTF, F.mvar());
    nmod_poly_clear (FLINTF);
    nmod_poly_clear (FLINTG);
#else
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    rem (NTLF, NTLF, NTLG);
    result= convertNTLzzpX2CF(NTLF, F.mvar());
#endif
  }
  return result;
}

CanonicalForm
divNTL (const CanonicalForm& F, const CanonicalForm& G, const modpk& b)
{
  if (CFFactory::gettype() == GaloisFieldDomain)
    return div (F, G);
  if (F.inCoeffDomain() && G.isUnivariate() && !G.inCoeffDomain())
  {
    return 0;
  }
  else if (F.inCoeffDomain() && G.inCoeffDomain())
  {
    if (b.getp() != 0)
    {
      if (!F.inBaseDomain() || !G.inBaseDomain())
      {
        Variable alpha;
        hasFirstAlgVar (F, alpha);
        hasFirstAlgVar (G, alpha);
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
        fmpz_t FLINTp;
        fmpz_mod_poly_t FLINTmipo;
        fq_ctx_t fq_con;
        fq_t FLINTF, FLINTG;

        fmpz_init (FLINTp);
        convertCF2initFmpz (FLINTp, b.getpk());

        convertFacCF2Fmpz_mod_poly_t (FLINTmipo, getMipo (alpha), FLINTp);

        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
        fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
        #else
        fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        #endif

        convertFacCF2Fq_t (FLINTF, F, fq_con);
        convertFacCF2Fq_t (FLINTG, G, fq_con);

        fq_inv (FLINTG, FLINTG, fq_con);
        fq_mul (FLINTF, FLINTF, FLINTG, fq_con);

        CanonicalForm result= convertFq_t2FacCF (FLINTF, alpha);

        fmpz_clear (FLINTp);
        fq_clear (FLINTF, fq_con);
        fq_clear (FLINTG, fq_con);
        fq_ctx_clear (fq_con);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTmipo, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTmipo);
        #endif
        return b (result);
#else
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (alpha)));
        ZZ_pE::init (NTLmipo);
        ZZ_pX NTLg= convertFacCF2NTLZZpX (G);
        ZZ_pX NTLf= convertFacCF2NTLZZpX (F);
        ZZ_pE result;
        div (result, to_ZZ_pE (NTLf), to_ZZ_pE (NTLg));
        return b (convertNTLZZpX2CF (rep (result), alpha));
#endif
      }
      return b(div (F,G));
    }
    return div (F, G);
  }
  else if (F.isUnivariate() && G.inCoeffDomain())
  {
    if (b.getp() != 0)
    {
      if (!G.inBaseDomain())
      {
        Variable alpha;
        hasFirstAlgVar (G, alpha);
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
        fmpz_t FLINTp;
        fmpz_mod_poly_t FLINTmipo;
        fq_ctx_t fq_con;
        fq_poly_t FLINTF;
        fq_t FLINTG;

        fmpz_init (FLINTp);
        convertCF2initFmpz (FLINTp, b.getpk());

        convertFacCF2Fmpz_mod_poly_t (FLINTmipo, getMipo (alpha), FLINTp);

        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
        fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
        #else
        fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        #endif

        convertFacCF2Fq_poly_t (FLINTF, F, fq_con);
        convertFacCF2Fq_t (FLINTG, G, fq_con);

        fq_inv (FLINTG, FLINTG, fq_con);
        fq_poly_scalar_mul_fq (FLINTF, FLINTF, FLINTG, fq_con);

        CanonicalForm result= convertFq_poly_t2FacCF (FLINTF, F.mvar(),
                                                      alpha, fq_con);

        fmpz_clear (FLINTp);
        fq_poly_clear (FLINTF, fq_con);
        fq_clear (FLINTG, fq_con);
        fq_ctx_clear (fq_con);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTmipo, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTmipo);
        #endif
        return b (result);
#else
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (alpha)));
        ZZ_pE::init (NTLmipo);
        ZZ_pX NTLg= convertFacCF2NTLZZpX (G);
        ZZ_pEX NTLf= convertFacCF2NTLZZ_pEX (F, NTLmipo);
        div (NTLf, NTLf, to_ZZ_pE (NTLg));
        return b (convertNTLZZ_pEX2CF (NTLf, F.mvar(), alpha));
#endif
      }
      return b(div (F,G));
    }
    return div (F, G);
  }

  if (getCharacteristic() == 0)
  {

    Variable alpha;
    if (!hasFirstAlgVar (F, alpha) && !hasFirstAlgVar (G, alpha))
    {
#ifdef HAVE_FLINT
      if (b.getp() != 0)
      {
        fmpz_t FLINTpk;
        fmpz_init (FLINTpk);
        convertCF2initFmpz (FLINTpk, b.getpk());
        fmpz_mod_poly_t FLINTF, FLINTG;
        convertFacCF2Fmpz_mod_poly_t (FLINTF, F, FLINTpk);
        convertFacCF2Fmpz_mod_poly_t (FLINTG, G, FLINTpk);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTpk);
        fmpz_mod_poly_divrem (FLINTF, FLINTG, FLINTF, FLINTG, fmpz_ctx);
        #else
        fmpz_mod_poly_divrem (FLINTF, FLINTG, FLINTF, FLINTG);
        #endif
        CanonicalForm result= convertFmpz_mod_poly_t2FacCF (FLINTF,F.mvar(),b);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTG, fmpz_ctx);
        fmpz_mod_poly_clear (FLINTF, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTG);
        fmpz_mod_poly_clear (FLINTF);
        #endif
        fmpz_clear (FLINTpk);
        return result;
      }
      return divFLINTQ (F,G);
#else
      if (b.getp() != 0)
      {
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZX ZZf= convertFacCF2NTLZZX (F);
        ZZX ZZg= convertFacCF2NTLZZX (G);
        ZZ_pX NTLf= to_ZZ_pX (ZZf);
        ZZ_pX NTLg= to_ZZ_pX (ZZg);
        div (NTLf, NTLf, NTLg);
        return b (convertNTLZZX2CF (to_ZZX (NTLf), F.mvar()));
      }
      return div (F, G);
#endif
    }
    else
    {
      if (b.getp() != 0)
      {
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
        fmpz_t FLINTp;
        fmpz_mod_poly_t FLINTmipo;
        fq_ctx_t fq_con;
        fq_poly_t FLINTF, FLINTG;

        fmpz_init (FLINTp);
        convertCF2initFmpz (FLINTp, b.getpk());

        convertFacCF2Fmpz_mod_poly_t (FLINTmipo, getMipo (alpha), FLINTp);

        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_ctx_t fmpz_ctx;
        fmpz_mod_ctx_init(fmpz_ctx,FLINTp);
        fq_ctx_init_modulus (fq_con, FLINTmipo, fmpz_ctx, "Z");
        #else
        fq_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        #endif

        convertFacCF2Fq_poly_t (FLINTF, F, fq_con);
        convertFacCF2Fq_poly_t (FLINTG, G, fq_con);

        fq_poly_divrem (FLINTF, FLINTG, FLINTF, FLINTG, fq_con);

        CanonicalForm result= convertFq_poly_t2FacCF (FLINTF, F.mvar(),
                                                      alpha, fq_con);

        fmpz_clear (FLINTp);
        fq_poly_clear (FLINTF, fq_con);
        fq_poly_clear (FLINTG, fq_con);
        fq_ctx_clear (fq_con);
        #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        fmpz_mod_poly_clear (FLINTmipo, fmpz_ctx);
        fmpz_mod_ctx_clear(fmpz_ctx);
        #else
        fmpz_mod_poly_clear (FLINTmipo);
        #endif
        return b (result);
#else
        ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
        ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (alpha)));
        ZZ_pE::init (NTLmipo);
        ZZ_pEX NTLg= convertFacCF2NTLZZ_pEX (G, NTLmipo);
        ZZ_pEX NTLf= convertFacCF2NTLZZ_pEX (F, NTLmipo);
        div (NTLf, NTLf, NTLg);
        return b (convertNTLZZ_pEX2CF (NTLf, F.mvar(), alpha));
#endif
      }
#ifdef HAVE_FLINT
      CanonicalForm Q;
      newtonDiv (F, G, Q);
      return Q;
#else
      return div (F,G);
#endif
    }
  }

  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
#if (!defined(HAVE_FLINT) ||  __FLINT_RELEASE < 20400)
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
#endif
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
    nmod_poly_t FLINTmipo;
    fq_nmod_ctx_t fq_con;

    nmod_poly_init (FLINTmipo, getCharacteristic());
    convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

    fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

    fq_nmod_poly_t FLINTF, FLINTG;
    convertFacCF2Fq_nmod_poly_t (FLINTF, F, fq_con);
    convertFacCF2Fq_nmod_poly_t (FLINTG, G, fq_con);

    fq_nmod_poly_divrem (FLINTF, FLINTG, FLINTF, FLINTG, fq_con);

    result= convertFq_nmod_poly_t2FacCF (FLINTF, F.mvar(), alpha, fq_con);

    fq_nmod_poly_clear (FLINTF, fq_con);
    fq_nmod_poly_clear (FLINTG, fq_con);
    nmod_poly_clear (FLINTmipo);
    fq_nmod_ctx_clear (fq_con);
#else
    zz_pX NTLMipo= convertFacCF2NTLzzpX(getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    div (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
#endif
  }
  else
  {
#ifdef HAVE_FLINT
    nmod_poly_t FLINTF, FLINTG;
    convertFacCF2nmod_poly_t (FLINTF, F);
    convertFacCF2nmod_poly_t (FLINTG, G);
    nmod_poly_div (FLINTF, FLINTF, FLINTG);
    result= convertnmod_poly_t2FacCF (FLINTF, F.mvar());
    nmod_poly_clear (FLINTF);
    nmod_poly_clear (FLINTG);
#else
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    div (NTLF, NTLF, NTLG);
    result= convertNTLzzpX2CF(NTLF, F.mvar());
#endif
  }
  return result;
}

// end univariate polys
//*************************
// bivariate polys

#ifdef HAVE_FLINT
void kronSubFp (nmod_poly_t result, const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  nmod_poly_init2 (result, getCharacteristic(), d*(degAy + 1));
  result->length= d*(degAy + 1);
  flint_mpn_zero (result->coeffs, d*(degAy+1));

  nmod_poly_t buf;

  int k;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    convertFacCF2nmod_poly_t (buf, i.coeff());
    k= i.exp()*d;
    flint_mpn_copyi (result->coeffs+k, buf->coeffs, nmod_poly_length(buf));

    nmod_poly_clear (buf);
  }
  _nmod_poly_normalise (result);
}

#if ( __FLINT_RELEASE >= 20400)
void
kronSubFq (fq_nmod_poly_t result, const CanonicalForm& A, int d,
           const fq_nmod_ctx_t fq_con)
{
  int degAy= degree (A);
  fq_nmod_poly_init2 (result, d*(degAy + 1), fq_con);
  _fq_nmod_poly_set_length (result, d*(degAy + 1), fq_con);
  _fq_nmod_vec_zero (result->coeffs, d*(degAy + 1), fq_con);

  fq_nmod_poly_t buf1;

  nmod_poly_t buf2;

  int k;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      convertFacCF2nmod_poly_t (buf2, i.coeff());
      fq_nmod_poly_init2 (buf1, 1, fq_con);
      fq_nmod_poly_set_coeff (buf1, 0, buf2, fq_con);
      nmod_poly_clear (buf2);
    }
    else
      convertFacCF2Fq_nmod_poly_t (buf1, i.coeff(), fq_con);

    k= i.exp()*d;
    _fq_nmod_vec_set (result->coeffs+k, buf1->coeffs,
                      fq_nmod_poly_length (buf1, fq_con), fq_con);

    fq_nmod_poly_clear (buf1, fq_con);
  }

  _fq_nmod_poly_normalise (result, fq_con);
}
#endif

/*void kronSubQa (fmpq_poly_t result, const CanonicalForm& A, int d1, int d2)
{
  int degAy= degree (A);
  fmpq_poly_init2 (result, d1*(degAy + 1));

  fmpq_poly_t buf;
  fmpq_t coeff;

  int k, l, bufRepLength;
  CFIterator j;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      k= d1*i.exp();
      convertFacCF2Fmpq_poly_t (buf, i.coeff());
      bufRepLength= (int) fmpq_poly_length(buf);
      for (l= 0; l < bufRepLength; l++)
      {
        fmpq_poly_get_coeff_fmpq (coeff, buf, l);
        fmpq_poly_set_coeff_fmpq (result, l + k, coeff);
      }
      fmpq_poly_clear (buf);
    }
    else
    {
      for (j= i.coeff(); j.hasTerms(); j++)
      {
        k= d1*i.exp();
        k += d2*j.exp();
        convertFacCF2Fmpq_poly_t (buf, j.coeff());
        bufRepLength= (int) fmpq_poly_length(buf);
        for (l= 0; l < bufRepLength; l++)
        {
          fmpq_poly_get_coeff_fmpq (coeff, buf, l);
          fmpq_poly_set_coeff_fmpq (result, k + l, coeff);
        }
        fmpq_poly_clear (buf);
      }
    }
  }
  fmpq_clear (coeff);
  _fmpq_poly_normalise (result);
}*/

void kronSubQa (fmpz_poly_t result, const CanonicalForm& A, int d1, int d2)
{
  int degAy= degree (A);
  fmpz_poly_init2 (result, d1*(degAy + 1));
  _fmpz_poly_set_length (result, d1*(degAy + 1));

  fmpz_poly_t buf;

  int k;
  CFIterator j;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      k= d1*i.exp();
      convertFacCF2Fmpz_poly_t (buf, i.coeff());
      _fmpz_vec_set (result->coeffs + k, buf->coeffs, buf->length);
      fmpz_poly_clear (buf);
    }
    else
    {
      for (j= i.coeff(); j.hasTerms(); j++)
      {
        k= d1*i.exp();
        k += d2*j.exp();
        convertFacCF2Fmpz_poly_t (buf, j.coeff());
        _fmpz_vec_set (result->coeffs + k, buf->coeffs, buf->length);
        fmpz_poly_clear (buf);
      }
    }
  }
  _fmpz_poly_normalise (result);
}

void
kronSubReciproFp (nmod_poly_t subA1, nmod_poly_t subA2, const CanonicalForm& A,
                  int d)
{
  int degAy= degree (A);
  mp_limb_t ninv= n_preinvert_limb (getCharacteristic());
  nmod_poly_init2_preinv (subA1, getCharacteristic(), ninv, d*(degAy + 2));
  nmod_poly_init2_preinv (subA2, getCharacteristic(), ninv, d*(degAy + 2));

  nmod_poly_t buf;

  int k, kk, j, bufRepLength;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    convertFacCF2nmod_poly_t (buf, i.coeff());

    k= i.exp()*d;
    kk= (degAy - i.exp())*d;
    bufRepLength= (int) nmod_poly_length (buf);
    for (j= 0; j < bufRepLength; j++)
    {
      nmod_poly_set_coeff_ui (subA1, j + k,
                              n_addmod (nmod_poly_get_coeff_ui (subA1, j+k),
                                        nmod_poly_get_coeff_ui (buf, j),
                                        getCharacteristic()
                                       )
                             );
      nmod_poly_set_coeff_ui (subA2, j + kk,
                              n_addmod (nmod_poly_get_coeff_ui (subA2, j + kk),
                                        nmod_poly_get_coeff_ui (buf, j),
                                        getCharacteristic()
                                       )
                             );
    }
    nmod_poly_clear (buf);
  }
  _nmod_poly_normalise (subA1);
  _nmod_poly_normalise (subA2);
}

#if ( __FLINT_RELEASE >= 20400)
void
kronSubReciproFq (fq_nmod_poly_t subA1, fq_nmod_poly_t subA2,
                  const CanonicalForm& A, int d, const fq_nmod_ctx_t fq_con)
{
  int degAy= degree (A);
  fq_nmod_poly_init2 (subA1, d*(degAy + 2), fq_con);
  fq_nmod_poly_init2 (subA2, d*(degAy + 2), fq_con);

  _fq_nmod_poly_set_length (subA1, d*(degAy + 2), fq_con);
  _fq_nmod_vec_zero (subA1->coeffs, d*(degAy + 2), fq_con);

  _fq_nmod_poly_set_length (subA2, d*(degAy + 2), fq_con);
  _fq_nmod_vec_zero (subA2->coeffs, d*(degAy + 2), fq_con);

  fq_nmod_poly_t buf1;

  nmod_poly_t buf2;

  int k, kk;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      convertFacCF2nmod_poly_t (buf2, i.coeff());
      fq_nmod_poly_init2 (buf1, 1, fq_con);
      fq_nmod_poly_set_coeff (buf1, 0, buf2, fq_con);
      nmod_poly_clear (buf2);
    }
    else
      convertFacCF2Fq_nmod_poly_t (buf1, i.coeff(), fq_con);

    k= i.exp()*d;
    kk= (degAy - i.exp())*d;
    _fq_nmod_vec_add (subA1->coeffs+k, subA1->coeffs+k, buf1->coeffs,
                      fq_nmod_poly_length(buf1, fq_con), fq_con);
    _fq_nmod_vec_add (subA2->coeffs+kk, subA2->coeffs+kk, buf1->coeffs,
                      fq_nmod_poly_length(buf1, fq_con), fq_con);

    fq_nmod_poly_clear (buf1, fq_con);
  }
  _fq_nmod_poly_normalise (subA1, fq_con);
  _fq_nmod_poly_normalise (subA2, fq_con);
}
#endif

void
kronSubReciproQ (fmpz_poly_t subA1, fmpz_poly_t subA2, const CanonicalForm& A,
                 int d)
{
  int degAy= degree (A);
  fmpz_poly_init2 (subA1, d*(degAy + 2));
  fmpz_poly_init2 (subA2, d*(degAy + 2));

  fmpz_poly_t buf;

  int k, kk;
  for (CFIterator i= A; i.hasTerms(); i++)
  {
    convertFacCF2Fmpz_poly_t (buf, i.coeff());

    k= i.exp()*d;
    kk= (degAy - i.exp())*d;
    _fmpz_vec_add (subA1->coeffs+k, subA1->coeffs + k, buf->coeffs, buf->length);
    _fmpz_vec_add (subA2->coeffs+kk, subA2->coeffs + kk, buf->coeffs, buf->length);
    fmpz_poly_clear (buf);
  }

  _fmpz_poly_normalise (subA1);
  _fmpz_poly_normalise (subA2);
}

CanonicalForm reverseSubstQ (const fmpz_poly_t F, int d)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fmpz_poly_t buf;
  CanonicalForm result= 0;
  int i= 0;
  int degf= fmpz_poly_degree(F);
  int k= 0;
  int degfSubK, repLength;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    fmpz_poly_init2 (buf, repLength);
    _fmpz_poly_set_length (buf, repLength);
    _fmpz_vec_set (buf->coeffs, F->coeffs+k, repLength);
    _fmpz_poly_normalise (buf);

    result += convertFmpz_poly_t2FacCF (buf, x)*power (y, i);
    i++;
    k= d*i;
    fmpz_poly_clear (buf);
  }

  return result;
}

/*CanonicalForm
reverseSubstQa (const fmpq_poly_t F, int d1, int d2, const Variable& alpha,
                const fmpq_poly_t mipo)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fmpq_poly_t f;
  fmpq_poly_init (f);
  fmpq_poly_set (f, F);

  fmpq_poly_t buf;
  CanonicalForm result= 0, result2;
  int i= 0;
  int degf= fmpq_poly_degree(f);
  int k= 0;
  int degfSubK;
  int repLength;
  fmpq_t coeff;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d1)
      repLength= d1;
    else
      repLength= degfSubK + 1;

    fmpq_init (coeff);
    int j= 0;
    int l;
    result2= 0;
    while (j*d2 < repLength)
    {
      fmpq_poly_init2 (buf, d2);
      for (l= 0; l < d2; l++)
      {
        fmpq_poly_get_coeff_fmpq (coeff, f, k + j*d2 + l);
        fmpq_poly_set_coeff_fmpq (buf, l, coeff);
      }
      _fmpq_poly_normalise (buf);
      fmpq_poly_rem (buf, buf, mipo);
      result2 += convertFmpq_poly_t2FacCF (buf, alpha)*power (x, j);
      j++;
      fmpq_poly_clear (buf);
    }
    if (repLength - j*d2 != 0 && j*d2 - repLength < d2)
    {
      j--;
      repLength -= j*d2;
      fmpq_poly_init2 (buf, repLength);
      j++;
      for (l= 0; l < repLength; l++)
      {
        fmpq_poly_get_coeff_fmpq (coeff, f, k + j*d2 + l);
        fmpq_poly_set_coeff_fmpq (buf, l, coeff);
      }
      _fmpq_poly_normalise (buf);
      fmpq_poly_rem (buf, buf, mipo);
      result2 += convertFmpq_poly_t2FacCF (buf, alpha)*power (x, j);
      fmpq_poly_clear (buf);
    }
    fmpq_clear (coeff);

    result += result2*power (y, i);
    i++;
    k= d1*i;
  }

  fmpq_poly_clear (f);
  return result;
}*/

CanonicalForm
reverseSubstQa (const fmpz_poly_t F, int d1, int d2, const Variable& alpha,
                const fmpq_poly_t mipo)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fmpq_poly_t buf;
  CanonicalForm result= 0, result2;
  int i= 0;
  int degf= fmpz_poly_degree(F);
  int k= 0;
  int degfSubK;
  int repLength;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d1)
      repLength= d1;
    else
      repLength= degfSubK + 1;

    int j= 0;
    result2= 0;
    while (j*d2 < repLength)
    {
      fmpq_poly_init2 (buf, d2);
      _fmpq_poly_set_length (buf, d2);
      _fmpz_vec_set (buf->coeffs, F->coeffs + k + j*d2, d2);
      _fmpq_poly_normalise (buf);
      fmpq_poly_rem (buf, buf, mipo);
      result2 += convertFmpq_poly_t2FacCF (buf, alpha)*power (x, j);
      j++;
      fmpq_poly_clear (buf);
    }
    if (repLength - j*d2 != 0 && j*d2 - repLength < d2)
    {
      j--;
      repLength -= j*d2;
      fmpq_poly_init2 (buf, repLength);
      _fmpq_poly_set_length (buf, repLength);
      j++;
      _fmpz_vec_set (buf->coeffs, F->coeffs + k + j*d2, repLength);
      _fmpq_poly_normalise (buf);
      fmpq_poly_rem (buf, buf, mipo);
      result2 += convertFmpq_poly_t2FacCF (buf, alpha)*power (x, j);
      fmpq_poly_clear (buf);
    }

    result += result2*power (y, i);
    i++;
    k= d1*i;
  }

  return result;
}

CanonicalForm
reverseSubstReciproFp (const nmod_poly_t F, const nmod_poly_t G, int d, int k)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  nmod_poly_t f, g;
  mp_limb_t ninv= n_preinvert_limb (getCharacteristic());
  nmod_poly_init_preinv (f, getCharacteristic(), ninv);
  nmod_poly_init_preinv (g, getCharacteristic(), ninv);
  nmod_poly_set (f, F);
  nmod_poly_set (g, G);
  int degf= nmod_poly_degree(f);
  int degg= nmod_poly_degree(g);


  nmod_poly_t buf1,buf2, buf3;

  if (nmod_poly_length (f) < (long) d*(k+1)) //zero padding
    nmod_poly_fit_length (f,(long)d*(k+1));

  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2, repLengthBuf1, ind, tmp;
  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    nmod_poly_init2_preinv (buf1, getCharacteristic(), ninv, repLengthBuf1);

    for (ind= 0; ind < repLengthBuf1; ind++)
      nmod_poly_set_coeff_ui (buf1, ind, nmod_poly_get_coeff_ui (f, ind+lf));
    _nmod_poly_normalise (buf1);

    repLengthBuf1= nmod_poly_length (buf1);

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    nmod_poly_init2_preinv (buf2, getCharacteristic(), ninv, repLengthBuf2);
    for (ind= 0; ind < repLengthBuf2; ind++)
      nmod_poly_set_coeff_ui (buf2, ind, nmod_poly_get_coeff_ui (g, ind + lg));

    _nmod_poly_normalise (buf2);
    repLengthBuf2= nmod_poly_length (buf2);

    nmod_poly_init2_preinv (buf3, getCharacteristic(), ninv, repLengthBuf2 + d);
    for (ind= 0; ind < repLengthBuf1; ind++)
      nmod_poly_set_coeff_ui (buf3, ind, nmod_poly_get_coeff_ui (buf1, ind));
    for (ind= repLengthBuf1; ind < d; ind++)
      nmod_poly_set_coeff_ui (buf3, ind, 0);
    for (ind= 0; ind < repLengthBuf2; ind++)
      nmod_poly_set_coeff_ui (buf3, ind+d, nmod_poly_get_coeff_ui (buf2, ind));
    _nmod_poly_normalise (buf3);

    result += convertnmod_poly_t2FacCF (buf3, x)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (ind= 0; ind < tmp; ind++)
        nmod_poly_set_coeff_ui (g, ind + lg,
                                n_submod (nmod_poly_get_coeff_ui (g, ind + lg),
                                          nmod_poly_get_coeff_ui (buf1, ind),
                                          getCharacteristic()
                                         )
                               );
    }
    if (lg < 0)
    {
      nmod_poly_clear (buf1);
      nmod_poly_clear (buf2);
      nmod_poly_clear (buf3);
      break;
    }
    if (degfSubLf >= 0)
    {
      for (ind= 0; ind < repLengthBuf2; ind++)
        nmod_poly_set_coeff_ui (f, ind + lf,
                                n_submod (nmod_poly_get_coeff_ui (f, ind + lf),
                                          nmod_poly_get_coeff_ui (buf2, ind),
                                          getCharacteristic()
                                         )
                               );
    }
    nmod_poly_clear (buf1);
    nmod_poly_clear (buf2);
    nmod_poly_clear (buf3);
  }

  nmod_poly_clear (f);
  nmod_poly_clear (g);

  return result;
}

#if ( __FLINT_RELEASE >= 20400)
CanonicalForm
reverseSubstReciproFq (const fq_nmod_poly_t F, const fq_nmod_poly_t G, int d,
                       int k, const Variable& alpha, const fq_nmod_ctx_t fq_con)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fq_nmod_poly_t f, g;
  int degf= fq_nmod_poly_degree(F, fq_con);
  int degg= fq_nmod_poly_degree(G, fq_con);

  fq_nmod_poly_t buf1,buf2, buf3;

  fq_nmod_poly_init (f, fq_con);
  fq_nmod_poly_init (g, fq_con);
  fq_nmod_poly_set (f, F, fq_con);
  fq_nmod_poly_set (g, G, fq_con);
  if (fq_nmod_poly_length (f, fq_con) < (long) d*(k + 1)) //zero padding
    fq_nmod_poly_fit_length (f, (long) d*(k + 1), fq_con);

  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2, repLengthBuf1, tmp;
  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    fq_nmod_poly_init2 (buf1, repLengthBuf1, fq_con);
    _fq_nmod_poly_set_length (buf1, repLengthBuf1, fq_con);

    _fq_nmod_vec_set (buf1->coeffs, f->coeffs + lf, repLengthBuf1, fq_con);
    _fq_nmod_poly_normalise (buf1, fq_con);

    repLengthBuf1= fq_nmod_poly_length (buf1, fq_con);

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    fq_nmod_poly_init2 (buf2, repLengthBuf2, fq_con);
    _fq_nmod_poly_set_length (buf2, repLengthBuf2, fq_con);
    _fq_nmod_vec_set (buf2->coeffs, g->coeffs + lg, repLengthBuf2, fq_con);

    _fq_nmod_poly_normalise (buf2, fq_con);
    repLengthBuf2= fq_nmod_poly_length (buf2, fq_con);

    fq_nmod_poly_init2 (buf3, repLengthBuf2 + d, fq_con);
    _fq_nmod_poly_set_length (buf3, repLengthBuf2 + d, fq_con);
    _fq_nmod_vec_set (buf3->coeffs, buf1->coeffs, repLengthBuf1, fq_con);
    _fq_nmod_vec_set (buf3->coeffs + d, buf2->coeffs, repLengthBuf2, fq_con);

    _fq_nmod_poly_normalise (buf3, fq_con);

    result += convertFq_nmod_poly_t2FacCF (buf3, x, alpha, fq_con)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k - i);
    deggSubLg= degg - lg;

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      tmp= tmin (repLengthBuf1, deggSubLg + 1);
      _fq_nmod_vec_sub (g->coeffs + lg, g->coeffs + lg, buf1-> coeffs,
                        tmp, fq_con);
    }
    if (lg < 0)
    {
      fq_nmod_poly_clear (buf1, fq_con);
      fq_nmod_poly_clear (buf2, fq_con);
      fq_nmod_poly_clear (buf3, fq_con);
      break;
    }
    if (degfSubLf >= 0)
      _fq_nmod_vec_sub (f->coeffs + lf, f->coeffs + lf, buf2->coeffs,
                        repLengthBuf2, fq_con);
    fq_nmod_poly_clear (buf1, fq_con);
    fq_nmod_poly_clear (buf2, fq_con);
    fq_nmod_poly_clear (buf3, fq_con);
  }

  fq_nmod_poly_clear (f, fq_con);
  fq_nmod_poly_clear (g, fq_con);

  return result;
}
#endif

CanonicalForm
reverseSubstReciproQ (const fmpz_poly_t F, const fmpz_poly_t G, int d, int k)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fmpz_poly_t f, g;
  fmpz_poly_init (f);
  fmpz_poly_init (g);
  fmpz_poly_set (f, F);
  fmpz_poly_set (g, G);
  int degf= fmpz_poly_degree(f);
  int degg= fmpz_poly_degree(g);

  fmpz_poly_t buf1,buf2, buf3;

  if (fmpz_poly_length (f) < (long) d*(k+1)) //zero padding
    fmpz_poly_fit_length (f,(long)d*(k+1));

  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2, repLengthBuf1, ind, tmp;
  fmpz_t tmp1, tmp2;
  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;

    fmpz_poly_init2 (buf1, repLengthBuf1);

    for (ind= 0; ind < repLengthBuf1; ind++)
    {
      fmpz_poly_get_coeff_fmpz (tmp1, f, ind + lf);
      fmpz_poly_set_coeff_fmpz (buf1, ind, tmp1);
    }
    _fmpz_poly_normalise (buf1);

    repLengthBuf1= fmpz_poly_length (buf1);

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    fmpz_poly_init2 (buf2, repLengthBuf2);

    for (ind= 0; ind < repLengthBuf2; ind++)
    {
      fmpz_poly_get_coeff_fmpz (tmp1, g, ind + lg);
      fmpz_poly_set_coeff_fmpz (buf2, ind, tmp1);
    }

    _fmpz_poly_normalise (buf2);
    repLengthBuf2= fmpz_poly_length (buf2);

    fmpz_poly_init2 (buf3, repLengthBuf2 + d);
    for (ind= 0; ind < repLengthBuf1; ind++)
    {
      fmpz_poly_get_coeff_fmpz (tmp1, buf1, ind);
      fmpz_poly_set_coeff_fmpz (buf3, ind, tmp1);
    }
    for (ind= repLengthBuf1; ind < d; ind++)
      fmpz_poly_set_coeff_ui (buf3, ind, 0);
    for (ind= 0; ind < repLengthBuf2; ind++)
    {
      fmpz_poly_get_coeff_fmpz (tmp1, buf2, ind);
      fmpz_poly_set_coeff_fmpz (buf3, ind + d, tmp1);
    }
    _fmpz_poly_normalise (buf3);

    result += convertFmpz_poly_t2FacCF (buf3, x)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (ind= 0; ind < tmp; ind++)
      {
        fmpz_poly_get_coeff_fmpz (tmp1, g, ind + lg);
        fmpz_poly_get_coeff_fmpz (tmp2, buf1, ind);
        fmpz_sub (tmp1, tmp1, tmp2);
        fmpz_poly_set_coeff_fmpz (g, ind + lg, tmp1);
      }
    }
    if (lg < 0)
    {
      fmpz_poly_clear (buf1);
      fmpz_poly_clear (buf2);
      fmpz_poly_clear (buf3);
      break;
    }
    if (degfSubLf >= 0)
    {
      for (ind= 0; ind < repLengthBuf2; ind++)
      {
        fmpz_poly_get_coeff_fmpz (tmp1, f, ind + lf);
        fmpz_poly_get_coeff_fmpz (tmp2, buf2, ind);
        fmpz_sub (tmp1, tmp1, tmp2);
        fmpz_poly_set_coeff_fmpz (f, ind + lf, tmp1);
      }
    }
    fmpz_poly_clear (buf1);
    fmpz_poly_clear (buf2);
    fmpz_poly_clear (buf3);
  }

  fmpz_poly_clear (f);
  fmpz_poly_clear (g);
  fmpz_clear (tmp1);
  fmpz_clear (tmp2);

  return result;
}

#if ( __FLINT_RELEASE >= 20400)
CanonicalForm
reverseSubstFq (const fq_nmod_poly_t F, int d, const Variable& alpha,
                const fq_nmod_ctx_t fq_con)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  fq_nmod_poly_t buf;
  CanonicalForm result= 0;
  int i= 0;
  int degf= fq_nmod_poly_degree(F, fq_con);
  int k= 0;
  int degfSubK, repLength;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    fq_nmod_poly_init2 (buf, repLength, fq_con);
    _fq_nmod_poly_set_length (buf, repLength, fq_con);
    _fq_nmod_vec_set (buf->coeffs, F->coeffs+k, repLength, fq_con);
    _fq_nmod_poly_normalise (buf, fq_con);

    result += convertFq_nmod_poly_t2FacCF (buf, x, alpha, fq_con)*power (y, i);
    i++;
    k= d*i;
    fq_nmod_poly_clear (buf, fq_con);
  }

  return result;
}
#endif

CanonicalForm reverseSubstFp (const nmod_poly_t F, int d)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  mp_limb_t ninv= n_preinvert_limb (getCharacteristic());

  nmod_poly_t buf;
  CanonicalForm result= 0;
  int i= 0;
  int degf= nmod_poly_degree(F);
  int k= 0;
  int degfSubK, repLength, j;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    nmod_poly_init2_preinv (buf, getCharacteristic(), ninv, repLength);
    for (j= 0; j < repLength; j++)
      nmod_poly_set_coeff_ui (buf, j, nmod_poly_get_coeff_ui (F, j + k));
    _nmod_poly_normalise (buf);

    result += convertnmod_poly_t2FacCF (buf, x)*power (y, i);
    i++;
    k= d*i;
    nmod_poly_clear (buf);
  }

  return result;
}

CanonicalForm
mulMod2FLINTFpReci (const CanonicalForm& F, const CanonicalForm& G, const
                    CanonicalForm& M)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  nmod_poly_t F1, F2;
  kronSubReciproFp (F1, F2, F, d1);

  nmod_poly_t G1, G2;
  kronSubReciproFp (G1, G2, G, d1);

  int k= d1*degree (M);
  nmod_poly_mullow (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);;
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);

  int b= nmod_poly_degree (F2) + nmod_poly_degree (G2) - k - degtailF - degtailG
         + d1*(2+taildegF + taildegG);
  nmod_poly_mulhigh (F2, F2, G2, b);
  nmod_poly_shift_right (F2, F2, b);
  int d2= tmax (nmod_poly_degree (F2)/d1, nmod_poly_degree (F1)/d1);


  CanonicalForm result= reverseSubstReciproFp (F1, F2, d1, d2);

  nmod_poly_clear (F1);
  nmod_poly_clear (F2);
  nmod_poly_clear (G1);
  nmod_poly_clear (G2);
  return result;
}

CanonicalForm
mulMod2FLINTFp (const CanonicalForm& F, const CanonicalForm& G, const
                CanonicalForm& M)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  int degAx= degree (A, 1);
  int degAy= degree (A, 2);
  int degBx= degree (B, 1);
  int degBy= degree (B, 2);
  int d1= degAx + 1 + degBx;
  int d2= tmax (degAy, degBy);

  if (d1 > 128 && d2 > 160 && (degAy == degBy) && (2*degAy > degree (M)))
    return mulMod2FLINTFpReci (A, B, M);

  nmod_poly_t FLINTA, FLINTB;
  kronSubFp (FLINTA, A, d1);
  kronSubFp (FLINTB, B, d1);

  int k= d1*degree (M);
  nmod_poly_mullow (FLINTA, FLINTA, FLINTB, (long) k);

  A= reverseSubstFp (FLINTA, d1);

  nmod_poly_clear (FLINTA);
  nmod_poly_clear (FLINTB);
  return A;
}

#if ( __FLINT_RELEASE >= 20400)
CanonicalForm
mulMod2FLINTFqReci (const CanonicalForm& F, const CanonicalForm& G, const
                    CanonicalForm& M, const Variable& alpha,
                    const fq_nmod_ctx_t fq_con)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  fq_nmod_poly_t F1, F2;
  kronSubReciproFq (F1, F2, F, d1, fq_con);

  fq_nmod_poly_t G1, G2;
  kronSubReciproFq (G1, G2, G, d1, fq_con);

  int k= d1*degree (M);
  fq_nmod_poly_mullow (F1, F1, G1, (long) k, fq_con);

  int degtailF= degree (tailcoeff (F), 1);
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);

  int b= k + degtailF + degtailG - d1*(2+taildegF + taildegG);

  fq_nmod_poly_reverse (F2, F2, fq_nmod_poly_length (F2, fq_con), fq_con);
  fq_nmod_poly_reverse (G2, G2, fq_nmod_poly_length (G2, fq_con), fq_con);
  fq_nmod_poly_mullow (F2, F2, G2, b+1, fq_con);
  fq_nmod_poly_reverse (F2, F2, b+1, fq_con);

  int d2= tmax (fq_nmod_poly_degree (F2, fq_con)/d1,
                fq_nmod_poly_degree (F1, fq_con)/d1);

  CanonicalForm result= reverseSubstReciproFq (F1, F2, d1, d2, alpha, fq_con);

  fq_nmod_poly_clear (F1, fq_con);
  fq_nmod_poly_clear (F2, fq_con);
  fq_nmod_poly_clear (G1, fq_con);
  fq_nmod_poly_clear (G2, fq_con);
  return result;
}

CanonicalForm
mulMod2FLINTFq (const CanonicalForm& F, const CanonicalForm& G, const
                CanonicalForm& M, const Variable& alpha,
                const fq_nmod_ctx_t fq_con)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  int degAx= degree (A, 1);
  int degAy= degree (A, 2);
  int degBx= degree (B, 1);
  int degBy= degree (B, 2);
  int d1= degAx + 1 + degBx;
  int d2= tmax (degAy, degBy);

  if (d1 > 128 && d2 > 160 && (degAy == degBy) && (2*degAy > degree (M)))
    return mulMod2FLINTFqReci (A, B, M, alpha, fq_con);

  fq_nmod_poly_t FLINTA, FLINTB;
  kronSubFq (FLINTA, A, d1, fq_con);
  kronSubFq (FLINTB, B, d1, fq_con);

  int k= d1*degree (M);
  fq_nmod_poly_mullow (FLINTA, FLINTA, FLINTB, (long) k, fq_con);

  A= reverseSubstFq (FLINTA, d1, alpha, fq_con);

  fq_nmod_poly_clear (FLINTA, fq_con);
  fq_nmod_poly_clear (FLINTB, fq_con);
  return A;
}
#endif

CanonicalForm
mulMod2FLINTQReci (const CanonicalForm& F, const CanonicalForm& G, const
                    CanonicalForm& M)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  fmpz_poly_t F1, F2;
  kronSubReciproQ (F1, F2, F, d1);

  fmpz_poly_t G1, G2;
  kronSubReciproQ (G1, G2, G, d1);

  int k= d1*degree (M);
  fmpz_poly_mullow (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);;
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);

  int b= fmpz_poly_degree (F2) + fmpz_poly_degree (G2) - k - degtailF - degtailG
         + d1*(2+taildegF + taildegG);
  fmpz_poly_mulhigh_n (F2, F2, G2, b);
  fmpz_poly_shift_right (F2, F2, b);
  int d2= tmax (fmpz_poly_degree (F2)/d1, fmpz_poly_degree (F1)/d1);

  CanonicalForm result= reverseSubstReciproQ (F1, F2, d1, d2);

  fmpz_poly_clear (F1);
  fmpz_poly_clear (F2);
  fmpz_poly_clear (G1);
  fmpz_poly_clear (G2);
  return result;
}

CanonicalForm
mulMod2FLINTQ (const CanonicalForm& F, const CanonicalForm& G, const
                CanonicalForm& M)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  int degAx= degree (A, 1);
  int degBx= degree (B, 1);
  int d1= degAx + 1 + degBx;

  CanonicalForm f= bCommonDen (F);
  CanonicalForm g= bCommonDen (G);
  A *= f;
  B *= g;

  fmpz_poly_t FLINTA, FLINTB;
  kronSubQa (FLINTA, A, d1);
  kronSubQa (FLINTB, B, d1);
  int k= d1*degree (M);

  fmpz_poly_mullow (FLINTA, FLINTA, FLINTB, (long) k);
  A= reverseSubstQ (FLINTA, d1);
  fmpz_poly_clear (FLINTA);
  fmpz_poly_clear (FLINTB);
  return A/(f*g);
}

/*CanonicalForm
mulMod2FLINTQa (const CanonicalForm& F, const CanonicalForm& G,
                const CanonicalForm& M)
{
  Variable a;
  if (!hasFirstAlgVar (F,a) && !hasFirstAlgVar (G, a))
    return mulMod2FLINTQ (F, G, M);
  CanonicalForm A= F;

  int degFx= degree (F, 1);
  int degFa= degree (F, a);
  int degGx= degree (G, 1);
  int degGa= degree (G, a);

  int d2= degFa+degGa+1;
  int d1= degFx + 1 + degGx;
  d1 *= d2;

  fmpq_poly_t FLINTF, FLINTG;
  kronSubQa (FLINTF, F, d1, d2);
  kronSubQa (FLINTG, G, d1, d2);

  fmpq_poly_mullow (FLINTF, FLINTF, FLINTG, d1*degree (M));

  fmpq_poly_t mipo;
  convertFacCF2Fmpq_poly_t (mipo, getMipo (a));
  CanonicalForm result= reverseSubstQa (FLINTF, d1, d2, a, mipo);
  fmpq_poly_clear (FLINTF);
  fmpq_poly_clear (FLINTG);
  return result;
}*/

CanonicalForm
mulMod2FLINTQa (const CanonicalForm& F, const CanonicalForm& G,
                const CanonicalForm& M)
{
  Variable a;
  if (!hasFirstAlgVar (F,a) && !hasFirstAlgVar (G, a))
    return mulMod2FLINTQ (F, G, M);
  CanonicalForm A= F, B= G;

  int degFx= degree (F, 1);
  int degFa= degree (F, a);
  int degGx= degree (G, 1);
  int degGa= degree (G, a);

  int d2= degFa+degGa+1;
  int d1= degFx + 1 + degGx;
  d1 *= d2;

  CanonicalForm f= bCommonDen (F);
  CanonicalForm g= bCommonDen (G);
  A *= f;
  B *= g;

  fmpz_poly_t FLINTF, FLINTG;
  kronSubQa (FLINTF, A, d1, d2);
  kronSubQa (FLINTG, B, d1, d2);

  fmpz_poly_mullow (FLINTF, FLINTF, FLINTG, d1*degree (M));

  fmpq_poly_t mipo;
  convertFacCF2Fmpq_poly_t (mipo, getMipo (a));
  A= reverseSubstQa (FLINTF, d1, d2, a, mipo);
  fmpz_poly_clear (FLINTF);
  fmpz_poly_clear (FLINTG);
  return A/(f*g);
}

#endif

#ifndef HAVE_FLINT
zz_pX kronSubFp (const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  zz_pX result;
  result.rep.SetLength (d*(degAy + 1));

  zz_p *resultp;
  resultp= result.rep.elts();
  zz_pX buf;
  zz_p *bufp;
  int j, k, bufRepLength;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
      buf= convertFacCF2NTLzzpX (i.coeff());
    else
      buf= convertFacCF2NTLzzpX (i.coeff());

    k= i.exp()*d;
    bufp= buf.rep.elts();
    bufRepLength= (int) buf.rep.length();
    for (j= 0; j < bufRepLength; j++)
      resultp [j + k]= bufp [j];
  }
  result.normalize();

  return result;
}
#endif

#if (!(HAVE_FLINT &&  __FLINT_RELEASE >= 20400))
zz_pEX kronSubFq (const CanonicalForm& A, int d, const Variable& alpha)
{
  int degAy= degree (A);
  zz_pEX result;
  result.rep.SetLength (d*(degAy + 1));

  Variable v;
  zz_pE *resultp;
  resultp= result.rep.elts();
  zz_pEX buf1;
  zz_pE *buf1p;
  zz_pX buf2;
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  int j, k, buf1RepLength;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      buf2= convertFacCF2NTLzzpX (i.coeff());
      buf1= to_zz_pEX (to_zz_pE (buf2));
    }
    else
      buf1= convertFacCF2NTLzz_pEX (i.coeff(), NTLMipo);

    k= i.exp()*d;
    buf1p= buf1.rep.elts();
    buf1RepLength= (int) buf1.rep.length();
    for (j= 0; j < buf1RepLength; j++)
      resultp [j + k]= buf1p [j];
  }
  result.normalize();

  return result;
}

void
kronSubReciproFq (zz_pEX& subA1, zz_pEX& subA2,const CanonicalForm& A, int d,
                  const Variable& alpha)
{
  int degAy= degree (A);
  subA1.rep.SetLength ((long) d*(degAy + 2));
  subA2.rep.SetLength ((long) d*(degAy + 2));

  Variable v;
  zz_pE *subA1p;
  zz_pE *subA2p;
  subA1p= subA1.rep.elts();
  subA2p= subA2.rep.elts();
  zz_pEX buf;
  zz_pE *bufp;
  zz_pX buf2;
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  int j, k, kk, bufRepLength;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      buf2= convertFacCF2NTLzzpX (i.coeff());
      buf= to_zz_pEX (to_zz_pE (buf2));
    }
    else
      buf= convertFacCF2NTLzz_pEX (i.coeff(), NTLMipo);

    k= i.exp()*d;
    kk= (degAy - i.exp())*d;
    bufp= buf.rep.elts();
    bufRepLength= (int) buf.rep.length();
    for (j= 0; j < bufRepLength; j++)
    {
      subA1p [j + k] += bufp [j];
      subA2p [j + kk] += bufp [j];
    }
  }
  subA1.normalize();
  subA2.normalize();
}
#endif

#ifndef HAVE_FLINT
void
kronSubReciproFp (zz_pX& subA1, zz_pX& subA2, const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  subA1.rep.SetLength ((long) d*(degAy + 2));
  subA2.rep.SetLength ((long) d*(degAy + 2));

  zz_p *subA1p;
  zz_p *subA2p;
  subA1p= subA1.rep.elts();
  subA2p= subA2.rep.elts();
  zz_pX buf;
  zz_p *bufp;
  int j, k, kk, bufRepLength;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    buf= convertFacCF2NTLzzpX (i.coeff());

    k= i.exp()*d;
    kk= (degAy - i.exp())*d;
    bufp= buf.rep.elts();
    bufRepLength= (int) buf.rep.length();
    for (j= 0; j < bufRepLength; j++)
    {
      subA1p [j + k] += bufp [j];
      subA2p [j + kk] += bufp [j];
    }
  }
  subA1.normalize();
  subA2.normalize();
}
#endif

#if (!(HAVE_FLINT &&  __FLINT_RELEASE >= 20400))
CanonicalForm
reverseSubstReciproFq (const zz_pEX& F, const zz_pEX& G, int d, int k,
                       const Variable& alpha)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pEX f= F;
  zz_pEX g= G;
  int degf= deg(f);
  int degg= deg(g);

  zz_pEX buf1;
  zz_pEX buf2;
  zz_pEX buf3;

  zz_pE *buf1p;
  zz_pE *buf2p;
  zz_pE *buf3p;
  if (f.rep.length() < (long) d*(k+1)) //zero padding
    f.rep.SetLength ((long)d*(k+1));

  zz_pE *gp= g.rep.elts();
  zz_pE *fp= f.rep.elts();
  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2, repLengthBuf1, ind, tmp;
  zz_pE zzpEZero= zz_pE();

  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    buf1.rep.SetLength((long) repLengthBuf1);

    buf1p= buf1.rep.elts();
    for (ind= 0; ind < repLengthBuf1; ind++)
      buf1p [ind]= fp [ind + lf];
    buf1.normalize();

    repLengthBuf1= buf1.rep.length();

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    buf2.rep.SetLength ((long) repLengthBuf2);
    buf2p= buf2.rep.elts();
    for (ind= 0; ind < repLengthBuf2; ind++)
      buf2p [ind]= gp [ind + lg];
    buf2.normalize();

    repLengthBuf2= buf2.rep.length();

    buf3.rep.SetLength((long) repLengthBuf2 + d);
    buf3p= buf3.rep.elts();
    buf2p= buf2.rep.elts();
    buf1p= buf1.rep.elts();
    for (ind= 0; ind < repLengthBuf1; ind++)
      buf3p [ind]= buf1p [ind];
    for (ind= repLengthBuf1; ind < d; ind++)
      buf3p [ind]= zzpEZero;
    for (ind= 0; ind < repLengthBuf2; ind++)
      buf3p [ind + d]= buf2p [ind];
    buf3.normalize();

    result += convertNTLzz_pEX2CF (buf3, x, alpha)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    buf1p= buf1.rep.elts();

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (ind= 0; ind < tmp; ind++)
        gp [ind + lg] -= buf1p [ind];
    }

    if (lg < 0)
      break;

    buf2p= buf2.rep.elts();
    if (degfSubLf >= 0)
    {
      for (ind= 0; ind < repLengthBuf2; ind++)
        fp [ind + lf] -= buf2p [ind];
    }
  }

  return result;
}
#endif

#ifndef HAVE_FLINT
CanonicalForm
reverseSubstReciproFp (const zz_pX& F, const zz_pX& G, int d, int k)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pX f= F;
  zz_pX g= G;
  int degf= deg(f);
  int degg= deg(g);

  zz_pX buf1;
  zz_pX buf2;
  zz_pX buf3;

  zz_p *buf1p;
  zz_p *buf2p;
  zz_p *buf3p;

  if (f.rep.length() < (long) d*(k+1)) //zero padding
    f.rep.SetLength ((long)d*(k+1));

  zz_p *gp= g.rep.elts();
  zz_p *fp= f.rep.elts();
  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2, repLengthBuf1, ind, tmp;
  zz_p zzpZero= zz_p();
  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    buf1.rep.SetLength((long) repLengthBuf1);

    buf1p= buf1.rep.elts();
    for (ind= 0; ind < repLengthBuf1; ind++)
      buf1p [ind]= fp [ind + lf];
    buf1.normalize();

    repLengthBuf1= buf1.rep.length();

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    buf2.rep.SetLength ((long) repLengthBuf2);
    buf2p= buf2.rep.elts();
    for (ind= 0; ind < repLengthBuf2; ind++)
      buf2p [ind]= gp [ind + lg];

    buf2.normalize();

    repLengthBuf2= buf2.rep.length();


    buf3.rep.SetLength((long) repLengthBuf2 + d);
    buf3p= buf3.rep.elts();
    buf2p= buf2.rep.elts();
    buf1p= buf1.rep.elts();
    for (ind= 0; ind < repLengthBuf1; ind++)
      buf3p [ind]= buf1p [ind];
    for (ind= repLengthBuf1; ind < d; ind++)
      buf3p [ind]= zzpZero;
    for (ind= 0; ind < repLengthBuf2; ind++)
      buf3p [ind + d]= buf2p [ind];
    buf3.normalize();

    result += convertNTLzzpX2CF (buf3, x)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    buf1p= buf1.rep.elts();

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (ind= 0; ind < tmp; ind++)
        gp [ind + lg] -= buf1p [ind];
    }
    if (lg < 0)
      break;

    buf2p= buf2.rep.elts();
    if (degfSubLf >= 0)
    {
      for (ind= 0; ind < repLengthBuf2; ind++)
        fp [ind + lf] -= buf2p [ind];
    }
  }

  return result;
}
#endif

#if (!(HAVE_FLINT &&  __FLINT_RELEASE >= 20400))
CanonicalForm reverseSubstFq (const zz_pEX& F, int d, const Variable& alpha)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pEX f= F;
  zz_pE *fp= f.rep.elts();

  zz_pEX buf;
  zz_pE *bufp;
  CanonicalForm result= 0;
  int i= 0;
  int degf= deg(f);
  int k= 0;
  int degfSubK, repLength, j;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    buf.rep.SetLength ((long) repLength);
    bufp= buf.rep.elts();
    for (j= 0; j < repLength; j++)
      bufp [j]= fp [j + k];
    buf.normalize();

    result += convertNTLzz_pEX2CF (buf, x, alpha)*power (y, i);
    i++;
    k= d*i;
  }

  return result;
}
#endif

#ifndef HAVE_FLINT
CanonicalForm reverseSubstFp (const zz_pX& F, int d)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pX f= F;
  zz_p *fp= f.rep.elts();

  zz_pX buf;
  zz_p *bufp;
  CanonicalForm result= 0;
  int i= 0;
  int degf= deg(f);
  int k= 0;
  int degfSubK, repLength, j;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    buf.rep.SetLength ((long) repLength);
    bufp= buf.rep.elts();
    for (j= 0; j < repLength; j++)
      bufp [j]= fp [j + k];
    buf.normalize();

    result += convertNTLzzpX2CF (buf, x)*power (y, i);
    i++;
    k= d*i;
  }

  return result;
}

// assumes input to be reduced mod M and to be an element of Fp
CanonicalForm
mulMod2NTLFpReci (const CanonicalForm& F, const CanonicalForm& G, const
                  CanonicalForm& M)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  zz_pX F1, F2;
  kronSubReciproFp (F1, F2, F, d1);
  zz_pX G1, G2;
  kronSubReciproFp (G1, G2, G, d1);

  int k= d1*degree (M);
  MulTrunc (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);
  int b= k + degtailF + degtailG - d1*(2+taildegF+taildegG);

  reverse (F2, F2);
  reverse (G2, G2);
  MulTrunc (F2, F2, G2, b + 1);
  reverse (F2, F2, b);

  int d2= tmax (deg (F2)/d1, deg (F1)/d1);
  return reverseSubstReciproFp (F1, F2, d1, d2);
}

//Kronecker substitution
CanonicalForm
mulMod2NTLFp (const CanonicalForm& F, const CanonicalForm& G, const
              CanonicalForm& M)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  int degAx= degree (A, 1);
  int degAy= degree (A, 2);
  int degBx= degree (B, 1);
  int degBy= degree (B, 2);
  int d1= degAx + 1 + degBx;
  int d2= tmax (degAy, degBy);

  if (d1 > 128 && d2 > 160 && (degAy == degBy) && (2*degAy > degree (M)))
    return mulMod2NTLFpReci (A, B, M);

  zz_pX NTLA= kronSubFp (A, d1);
  zz_pX NTLB= kronSubFp (B, d1);

  int k= d1*degree (M);
  MulTrunc (NTLA, NTLA, NTLB, (long) k);

  A= reverseSubstFp (NTLA, d1);

  return A;
}
#endif

#if (!(HAVE_FLINT &&  __FLINT_RELEASE >= 20400))
// assumes input to be reduced mod M and to be an element of Fq not Fp
CanonicalForm
mulMod2NTLFqReci (const CanonicalForm& F, const CanonicalForm& G, const
                  CanonicalForm& M, const Variable& alpha)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  zz_pEX F1, F2;
  kronSubReciproFq (F1, F2, F, d1, alpha);
  zz_pEX G1, G2;
  kronSubReciproFq (G1, G2, G, d1, alpha);

  int k= d1*degree (M);
  MulTrunc (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);
  int b= k + degtailF + degtailG - d1*(2+taildegF+taildegG);

  reverse (F2, F2);
  reverse (G2, G2);
  MulTrunc (F2, F2, G2, b + 1);
  reverse (F2, F2, b);

  int d2= tmax (deg (F2)/d1, deg (F1)/d1);
  return reverseSubstReciproFq (F1, F2, d1, d2, alpha);
}
#endif

#ifdef HAVE_FLINT
CanonicalForm
mulMod2FLINTFp (const CanonicalForm& F, const CanonicalForm& G, const
                CanonicalForm& M);
#endif

CanonicalForm
mulMod2NTLFq (const CanonicalForm& F, const CanonicalForm& G, const
              CanonicalForm& M)
{
  Variable alpha;
  CanonicalForm A= F;
  CanonicalForm B= G;

  if (hasFirstAlgVar (A, alpha) || hasFirstAlgVar (B, alpha))
  {
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
    nmod_poly_t FLINTmipo;
    convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

    fq_nmod_ctx_t fq_con;
    fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

    A= mulMod2FLINTFq (A, B, M, alpha, fq_con);
    nmod_poly_clear (FLINTmipo);
    fq_nmod_ctx_clear (fq_con);
#else
    int degAx= degree (A, 1);
    int degAy= degree (A, 2);
    int degBx= degree (B, 1);
    int degBy= degree (B, 2);
    int d1= degAx + degBx + 1;
    int d2= tmax (degAy, degBy);
    if (fac_NTL_char != getCharacteristic())
    {
      fac_NTL_char= getCharacteristic();
      zz_p::init (getCharacteristic());
    }
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);

    int degMipo= degree (getMipo (alpha));
    if ((d1 > 128/degMipo) && (d2 > 160/degMipo) && (degAy == degBy) &&
        (2*degAy > degree (M)))
      return mulMod2NTLFqReci (A, B, M, alpha);

    zz_pEX NTLA= kronSubFq (A, d1, alpha);
    zz_pEX NTLB= kronSubFq (B, d1, alpha);

    int k= d1*degree (M);

    MulTrunc (NTLA, NTLA, NTLB, (long) k);

    A= reverseSubstFq (NTLA, d1, alpha);
#endif
  }
  else
  {
#ifdef HAVE_FLINT
    A= mulMod2FLINTFp (A, B, M);
#else
    A= mulMod2NTLFp (A, B, M);
#endif
  }
  return A;
}

CanonicalForm mulMod2 (const CanonicalForm& A, const CanonicalForm& B,
                       const CanonicalForm& M)
{
  if (A.isZero() || B.isZero())
    return 0;

  ASSERT (M.isUnivariate(), "M must be univariate");

  CanonicalForm F= mod (A, M);
  CanonicalForm G= mod (B, M);
  if (F.inCoeffDomain())
    return G*F;
  if (G.inCoeffDomain())
    return F*G;

  Variable y= M.mvar();
  int degF= degree (F, y);
  int degG= degree (G, y);

  if ((degF < 1 && degG < 1) && (F.isUnivariate() && G.isUnivariate()) &&
      (F.level() == G.level()))
  {
    CanonicalForm result= mulNTL (F, G);
    return mod (result, M);
  }
  else if (degF <= 1 && degG <= 1)
  {
    CanonicalForm result= F*G;
    return mod (result, M);
  }

  int sizeF= size (F);
  int sizeG= size (G);

  int fallBackToNaive= 50;
  if (sizeF < fallBackToNaive || sizeG < fallBackToNaive)
  {
    if (sizeF < sizeG)
      return mod (G*F, M);
    else
      return mod (F*G, M);
  }

#ifdef HAVE_FLINT
  if (getCharacteristic() == 0)
    return mulMod2FLINTQa (F, G, M);
#endif

  if (getCharacteristic() > 0 && CFFactory::gettype() != GaloisFieldDomain &&
      (((degF-degG) < 50 && degF > degG) || ((degG-degF) < 50 && degF <= degG)))
    return mulMod2NTLFq (F, G, M);

  int m= (int) ceil (degree (M)/2.0);
  if (degF >= m || degG >= m)
  {
    CanonicalForm MLo= power (y, m);
    CanonicalForm MHi= power (y, degree (M) - m);
    CanonicalForm F0= mod (F, MLo);
    CanonicalForm F1= div (F, MLo);
    CanonicalForm G0= mod (G, MLo);
    CanonicalForm G1= div (G, MLo);
    CanonicalForm F0G1= mulMod2 (F0, G1, MHi);
    CanonicalForm F1G0= mulMod2 (F1, G0, MHi);
    CanonicalForm F0G0= mulMod2 (F0, G0, M);
    return F0G0 + MLo*(F0G1 + F1G0);
  }
  else
  {
    m= (int) ceil (tmax (degF, degG)/2.0);
    CanonicalForm yToM= power (y, m);
    CanonicalForm F0= mod (F, yToM);
    CanonicalForm F1= div (F, yToM);
    CanonicalForm G0= mod (G, yToM);
    CanonicalForm G1= div (G, yToM);
    CanonicalForm H00= mulMod2 (F0, G0, M);
    CanonicalForm H11= mulMod2 (F1, G1, M);
    CanonicalForm H01= mulMod2 (F0 + F1, G0 + G1, M);
    return H11*yToM*yToM + (H01 - H11 - H00)*yToM + H00;
  }
  DEBOUTLN (cerr, "fatal end in mulMod2");
}

// end bivariate polys
//**********************
// multivariate polys

CanonicalForm mod (const CanonicalForm& F, const CFList& M)
{
  CanonicalForm A= F;
  for (CFListIterator i= M; i.hasItem(); i++)
    A= mod (A, i.getItem());
  return A;
}

CanonicalForm mulMod (const CanonicalForm& A, const CanonicalForm& B,
                      const CFList& MOD)
{
  if (A.isZero() || B.isZero())
    return 0;

  if (MOD.length() == 1)
    return mulMod2 (A, B, MOD.getLast());

  CanonicalForm M= MOD.getLast();
  CanonicalForm F= mod (A, M);
  CanonicalForm G= mod (B, M);
  if (F.inCoeffDomain())
    return G*F;
  if (G.inCoeffDomain())
    return F*G;

  int sizeF= size (F);
  int sizeG= size (G);

  if (sizeF / MOD.length() < 100 || sizeG / MOD.length() < 100)
  {
    if (sizeF < sizeG)
      return mod (G*F, MOD);
    else
      return mod (F*G, MOD);
  }

  Variable y= M.mvar();
  int degF= degree (F, y);
  int degG= degree (G, y);

  if ((degF <= 1 && F.level() <= M.level()) &&
      (degG <= 1 && G.level() <= M.level()))
  {
    CFList buf= MOD;
    buf.removeLast();
    if (degF == 1 && degG == 1)
    {
      CanonicalForm F0= mod (F, y);
      CanonicalForm F1= div (F, y);
      CanonicalForm G0= mod (G, y);
      CanonicalForm G1= div (G, y);
      if (degree (M) > 2)
      {
        CanonicalForm H00= mulMod (F0, G0, buf);
        CanonicalForm H11= mulMod (F1, G1, buf);
        CanonicalForm H01= mulMod (F0 + F1, G0 + G1, buf);
        return H11*y*y + (H01 - H00 - H11)*y + H00;
      }
      else //here degree (M) == 2
      {
        buf.append (y);
        CanonicalForm F0G1= mulMod (F0, G1, buf);
        CanonicalForm F1G0= mulMod (F1, G0, buf);
        CanonicalForm F0G0= mulMod (F0, G0, MOD);
        CanonicalForm result= F0G0 + y*(F0G1 + F1G0);
        return result;
      }
    }
    else if (degF == 1 && degG == 0)
      return mulMod (div (F, y), G, buf)*y + mulMod (mod (F, y), G, buf);
    else if (degF == 0 && degG == 1)
      return mulMod (div (G, y), F, buf)*y + mulMod (mod (G, y), F, buf);
    else
      return mulMod (F, G, buf);
  }
  int m= (int) ceil (degree (M)/2.0);
  if (degF >= m || degG >= m)
  {
    CanonicalForm MLo= power (y, m);
    CanonicalForm MHi= power (y, degree (M) - m);
    CanonicalForm F0= mod (F, MLo);
    CanonicalForm F1= div (F, MLo);
    CanonicalForm G0= mod (G, MLo);
    CanonicalForm G1= div (G, MLo);
    CFList buf= MOD;
    buf.removeLast();
    buf.append (MHi);
    CanonicalForm F0G1= mulMod (F0, G1, buf);
    CanonicalForm F1G0= mulMod (F1, G0, buf);
    CanonicalForm F0G0= mulMod (F0, G0, MOD);
    return F0G0 + MLo*(F0G1 + F1G0);
  }
  else
  {
    m= (tmax(degF, degG)+1)/2;
    CanonicalForm yToM= power (y, m);
    CanonicalForm F0= mod (F, yToM);
    CanonicalForm F1= div (F, yToM);
    CanonicalForm G0= mod (G, yToM);
    CanonicalForm G1= div (G, yToM);
    CanonicalForm H00= mulMod (F0, G0, MOD);
    CanonicalForm H11= mulMod (F1, G1, MOD);
    CanonicalForm H01= mulMod (F0 + F1, G0 + G1, MOD);
    return H11*yToM*yToM + (H01 - H11 - H00)*yToM + H00;
  }
  DEBOUTLN (cerr, "fatal end in mulMod");
}

CanonicalForm prodMod (const CFList& L, const CanonicalForm& M)
{
  if (L.isEmpty())
    return 1;
  int l= L.length();
  if (l == 1)
    return mod (L.getFirst(), M);
  else if (l == 2) {
    CanonicalForm result= mulMod2 (L.getFirst(), L.getLast(), M);
    return result;
  }
  else
  {
    l /= 2;
    CFList tmp1, tmp2;
    CFListIterator i= L;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod (tmp1, M);
    buf2= prodMod (tmp2, M);
    CanonicalForm result= mulMod2 (buf1, buf2, M);
    return result;
  }
}

CanonicalForm prodMod (const CFList& L, const CFList& M)
{
  if (L.isEmpty())
    return 1;
  else if (L.length() == 1)
    return L.getFirst();
  else if (L.length() == 2)
    return mulMod (L.getFirst(), L.getLast(), M);
  else
  {
    int l= L.length()/2;
    CFListIterator i= L;
    CFList tmp1, tmp2;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod (tmp1, M);
    buf2= prodMod (tmp2, M);
    return mulMod (buf1, buf2, M);
  }
}

// end multivariate polys
//***************************
// division

CanonicalForm reverse (const CanonicalForm& F, int d)
{
  if (d == 0)
    return F;
  CanonicalForm A= F;
  Variable y= Variable (2);
  Variable x= Variable (1);
  if (degree (A, x) > 0)
  {
    A= swapvar (A, x, y);
    CanonicalForm result= 0;
    CFIterator i= A;
    while (d - i.exp() < 0)
      i++;

    for (; i.hasTerms() && (d - i.exp() >= 0); i++)
      result += swapvar (i.coeff(),x,y)*power (x, d - i.exp());
    return result;
  }
  else
    return A*power (x, d);
}

CanonicalForm
newtonInverse (const CanonicalForm& F, const int n, const CanonicalForm& M)
{
  int l= ilog2(n);

  CanonicalForm g= mod (F, M)[0] [0];

  ASSERT (!g.isZero(), "expected a unit");

  Variable alpha;

  if (!g.isOne())
    g = 1/g;
  Variable x= Variable (1);
  CanonicalForm result;
  int exp= 0;
  if (n & 1)
  {
    result= g;
    exp= 1;
  }
  CanonicalForm h;

  for (int i= 1; i <= l; i++)
  {
    h= mulMod2 (g, mod (F, power (x, (1 << i))), M);
    h= mod (h, power (x, (1 << i)) - 1);
    h= div (h, power (x, (1 << (i - 1))));
    h= mod (h, M);
    g -= power (x, (1 << (i - 1)))*
         mod (mulMod2 (g, h, M), power (x, (1 << (i - 1))));

    if (n & (1 << i))
    {
      if (exp)
      {
        h= mulMod2 (result, mod (F, power (x, exp + (1 << i))), M);
        h= mod (h, power (x, exp + (1 << i)) - 1);
        h= div (h, power (x, exp));
        h= mod (h, M);
        result -= power(x, exp)*mod (mulMod2 (g, h, M),
                                       power (x, (1 << i)));
        exp += (1 << i);
      }
      else
      {
        exp= (1 << i);
        result= g;
      }
    }
  }

  return result;
}

CanonicalForm
newtonDiv (const CanonicalForm& F, const CanonicalForm& G, const CanonicalForm&
           M)
{
  ASSERT (getCharacteristic() > 0, "positive characteristic expected");

  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);

  Variable x= Variable (1);
  int degA= degree (A, x);
  int degB= degree (B, x);
  int m= degA - degB;
  if (m < 0)
    return 0;

  Variable v;
  CanonicalForm Q;
  if (degB < 1 || CFFactory::gettype() == GaloisFieldDomain)
  {
    CanonicalForm R;
    divrem2 (A, B, Q, R, M);
  }
  else
  {
    if (hasFirstAlgVar (A, v) || hasFirstAlgVar (B, v))
    {
      CanonicalForm R= reverse (A, degA);
      CanonicalForm revB= reverse (B, degB);
      revB= newtonInverse (revB, m + 1, M);
      Q= mulMod2 (R, revB, M);
      Q= mod (Q, power (x, m + 1));
      Q= reverse (Q, m);
    }
    else
    {
      Variable y= Variable (2);
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
      nmod_poly_t FLINTmipo;
      fq_nmod_ctx_t fq_con;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      convertFacCF2nmod_poly_t (FLINTmipo, M);

      fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");


      fq_nmod_poly_t FLINTA, FLINTB;
      convertFacCF2Fq_nmod_poly_t (FLINTA, swapvar (A, x, y), fq_con);
      convertFacCF2Fq_nmod_poly_t (FLINTB, swapvar (B, x, y), fq_con);

      fq_nmod_poly_divrem (FLINTA, FLINTB, FLINTA, FLINTB, fq_con);

      Q= convertFq_nmod_poly_t2FacCF (FLINTA, x, y, fq_con);

      fq_nmod_poly_clear (FLINTA, fq_con);
      fq_nmod_poly_clear (FLINTB, fq_con);
      nmod_poly_clear (FLINTmipo);
      fq_nmod_ctx_clear (fq_con);
#else
      bool zz_pEbak= zz_pE::initialized();
      zz_pEBak bak;
      if (zz_pEbak)
        bak.save();
      zz_pX mipo= convertFacCF2NTLzzpX (M);
      zz_pEX NTLA, NTLB;
      NTLA= convertFacCF2NTLzz_pEX (swapvar (A, x, y), mipo);
      NTLB= convertFacCF2NTLzz_pEX (swapvar (B, x, y), mipo);
      div (NTLA, NTLA, NTLB);
      Q= convertNTLzz_pEX2CF (NTLA, x, y);
      if (zz_pEbak)
        bak.restore();
#endif
    }
  }

  return Q;
}

void
newtonDivrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
              CanonicalForm& R, const CanonicalForm& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degA= degree (A, x);
  int degB= degree (B, x);
  int m= degA - degB;

  if (m < 0)
  {
    R= A;
    Q= 0;
    return;
  }

  Variable v;
  if (degB <= 1 || CFFactory::gettype() == GaloisFieldDomain)
  {
     divrem2 (A, B, Q, R, M);
  }
  else
  {
    if (hasFirstAlgVar (A, v) || hasFirstAlgVar (B, v))
    {
      R= reverse (A, degA);

      CanonicalForm revB= reverse (B, degB);
      revB= newtonInverse (revB, m + 1, M);
      Q= mulMod2 (R, revB, M);

      Q= mod (Q, power (x, m + 1));
      Q= reverse (Q, m);

      R= A - mulMod2 (Q, B, M);
    }
    else
    {
      Variable y= Variable (2);
#if (HAVE_FLINT &&  __FLINT_RELEASE >= 20400)
      nmod_poly_t FLINTmipo;
      fq_nmod_ctx_t fq_con;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      convertFacCF2nmod_poly_t (FLINTmipo, M);

      fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

      fq_nmod_poly_t FLINTA, FLINTB;
      convertFacCF2Fq_nmod_poly_t (FLINTA, swapvar (A, x, y), fq_con);
      convertFacCF2Fq_nmod_poly_t (FLINTB, swapvar (B, x, y), fq_con);

      fq_nmod_poly_divrem (FLINTA, FLINTB, FLINTA, FLINTB, fq_con);

      Q= convertFq_nmod_poly_t2FacCF (FLINTA, x, y, fq_con);
      R= convertFq_nmod_poly_t2FacCF (FLINTB, x, y, fq_con);

      fq_nmod_poly_clear (FLINTA, fq_con);
      fq_nmod_poly_clear (FLINTB, fq_con);
      nmod_poly_clear (FLINTmipo);
      fq_nmod_ctx_clear (fq_con);
#else
      zz_pX mipo= convertFacCF2NTLzzpX (M);
      zz_pEX NTLA, NTLB;
      NTLA= convertFacCF2NTLzz_pEX (swapvar (A, x, y), mipo);
      NTLB= convertFacCF2NTLzz_pEX (swapvar (B, x, y), mipo);
      zz_pEX NTLQ, NTLR;
      DivRem (NTLQ, NTLR, NTLA, NTLB);
      Q= convertNTLzz_pEX2CF (NTLQ, x, y);
      R= convertNTLzz_pEX2CF (NTLR, x, y);
#endif
    }
  }
}

static inline
CFList split (const CanonicalForm& F, const int m, const Variable& x)
{
  CanonicalForm A= F;
  CanonicalForm buf= 0;
  bool swap= false;
  if (degree (A, x) <= 0)
    return CFList(A);
  else if (x.level() != A.level())
  {
    swap= true;
    A= swapvar (A, x, A.mvar());
  }

  int j= (int) floor ((double) degree (A)/ m);
  CFList result;
  CFIterator i= A;
  for (; j >= 0; j--)
  {
    while (i.hasTerms() && i.exp() - j*m >= 0)
    {
      if (swap)
        buf += i.coeff()*power (A.mvar(), i.exp() - j*m);
      else
        buf += i.coeff()*power (x, i.exp() - j*m);
      i++;
    }
    if (swap)
      result.append (swapvar (buf, x, F.mvar()));
    else
      result.append (buf);
    buf= 0;
  }
  return result;
}

static inline
void divrem32 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M);

static inline
void divrem21 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degB= degree (B, x);
  int degA= degree (A, x);
  if (degA < degB)
  {
    Q= 0;
    R= A;
    return;
  }
  if (degB < 1)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, M);
    R= mod (R, M);
    return;
  }
  int m= (int) ceil ((double) (degB + 1)/2.0) + 1;
  ASSERT (4*m >= degA, "expected degree (F, 1) < 2*degree (G, 1)");
  CFList splitA= split (A, m, x);
  if (splitA.length() == 3)
    splitA.insert (0);
  if (splitA.length() == 2)
  {
    splitA.insert (0);
    splitA.insert (0);
  }
  if (splitA.length() == 1)
  {
    splitA.insert (0);
    splitA.insert (0);
    splitA.insert (0);
  }

  CanonicalForm xToM= power (x, m);

  CFListIterator i= splitA;
  CanonicalForm H= i.getItem();
  i++;
  H *= xToM;
  H += i.getItem();
  i++;
  H *= xToM;
  H += i.getItem();
  i++;

  divrem32 (H, B, Q, R, M);

  CFList splitR= split (R, m, x);
  if (splitR.length() == 1)
    splitR.insert (0);

  H= splitR.getFirst();
  H *= xToM;
  H += splitR.getLast();
  H *= xToM;
  H += i.getItem();

  CanonicalForm bufQ;
  divrem32 (H, B, bufQ, R, M);

  Q *= xToM;
  Q += bufQ;
  return;
}

static inline
void divrem32 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degB= degree (B, x);
  int degA= degree (A, x);
  if (degA < degB)
  {
    Q= 0;
    R= A;
    return;
  }
  if (degB < 1)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, M);
    R= mod (R, M);
    return;
  }
  int m= (int) ceil ((double) (degB + 1)/ 2.0);
  ASSERT (3*m > degA, "expected degree (F, 1) < 3*degree (G, 1)");
  CFList splitA= split (A, m, x);
  CFList splitB= split (B, m, x);

  if (splitA.length() == 2)
  {
    splitA.insert (0);
  }
  if (splitA.length() == 1)
  {
    splitA.insert (0);
    splitA.insert (0);
  }
  CanonicalForm xToM= power (x, m);

  CanonicalForm H;
  CFListIterator i= splitA;
  i++;

  if (degree (splitA.getFirst(), x) < degree (splitB.getFirst(), x))
  {
    H= splitA.getFirst()*xToM + i.getItem();
    divrem21 (H, splitB.getFirst(), Q, R, M);
  }
  else
  {
    R= splitA.getFirst()*xToM + i.getItem() + splitB.getFirst() -
       splitB.getFirst()*xToM;
    Q= xToM - 1;
  }

  H= mulMod (Q, splitB.getLast(), M);

  R= R*xToM + splitA.getLast() - H;

  while (degree (R, x) >= degB)
  {
    xToM= power (x, degree (R, x) - degB);
    Q += LC (R, x)*xToM;
    R -= mulMod (LC (R, x), B, M)*xToM;
    Q= mod (Q, M);
    R= mod (R, M);
  }

  return;
}

void divrem2 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
              CanonicalForm& R, const CanonicalForm& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);

  if (B.inCoeffDomain())
  {
    divrem (A, B, Q, R);
    return;
  }
  if (A.inCoeffDomain() && !B.inCoeffDomain())
  {
    Q= 0;
    R= A;
    return;
  }

  if (B.level() < A.level())
  {
    divrem (A, B, Q, R);
    return;
  }
  if (A.level() > B.level())
  {
    R= A;
    Q= 0;
    return;
  }
  if (B.level() == 1 && B.isUnivariate())
  {
    divrem (A, B, Q, R);
    return;
  }

  Variable x= Variable (1);
  int degB= degree (B, x);
  if (degB > degree (A, x))
  {
    Q= 0;
    R= A;
    return;
  }

  CFList splitA= split (A, degB, x);

  CanonicalForm xToDegB= power (x, degB);
  CanonicalForm H, bufQ;
  Q= 0;
  CFListIterator i= splitA;
  H= i.getItem()*xToDegB;
  i++;
  H += i.getItem();
  CFList buf;
  while (i.hasItem())
  {
    buf= CFList (M);
    divrem21 (H, B, bufQ, R, buf);
    i++;
    if (i.hasItem())
      H= R*xToDegB + i.getItem();
    Q *= xToDegB;
    Q += bufQ;
  }
  return;
}

void divrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
             CanonicalForm& R, const CFList& MOD)
{
  CanonicalForm A= mod (F, MOD);
  CanonicalForm B= mod (G, MOD);
  Variable x= Variable (1);
  int degB= degree (B, x);
  if (degB > degree (A, x))
  {
    Q= 0;
    R= A;
    return;
  }

  if (degB <= 0)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, MOD);
    R= mod (R, MOD);
    return;
  }
  CFList splitA= split (A, degB, x);

  CanonicalForm xToDegB= power (x, degB);
  CanonicalForm H, bufQ;
  Q= 0;
  CFListIterator i= splitA;
  H= i.getItem()*xToDegB;
  i++;
  H += i.getItem();
  while (i.hasItem())
  {
    divrem21 (H, B, bufQ, R, MOD);
    i++;
    if (i.hasItem())
      H= R*xToDegB + i.getItem();
    Q *= xToDegB;
    Q += bufQ;
  }
  return;
}

bool
uniFdivides (const CanonicalForm& A, const CanonicalForm& B)
{
  if (B.isZero())
    return true;
  if (A.isZero())
    return false;
  if (CFFactory::gettype() == GaloisFieldDomain)
    return fdivides (A, B);
  int p= getCharacteristic();
  if (A.inCoeffDomain() || B.inCoeffDomain())
  {
    if (A.inCoeffDomain())
      return true;
    else
      return false;
  }
  if (p > 0)
  {
#if (!defined(HAVE_FLINT) ||  __FLINT_RELEASE < 20400)
    if (fac_NTL_char != p)
    {
      fac_NTL_char= p;
      zz_p::init (p);
    }
#endif
    Variable alpha;
    if (hasFirstAlgVar (A, alpha) || hasFirstAlgVar (B, alpha))
    {
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
      nmod_poly_t FLINTmipo;
      fq_nmod_ctx_t fq_con;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

      fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

      fq_nmod_poly_t FLINTA, FLINTB;
      convertFacCF2Fq_nmod_poly_t (FLINTA, A, fq_con);
      convertFacCF2Fq_nmod_poly_t (FLINTB, B, fq_con);
      int result= fq_nmod_poly_divides (FLINTA, FLINTB, FLINTA, fq_con);
      fq_nmod_poly_clear (FLINTA, fq_con);
      fq_nmod_poly_clear (FLINTB, fq_con);
      nmod_poly_clear (FLINTmipo);
      fq_nmod_ctx_clear (fq_con);
      return result;
#else
      zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
      zz_pE::init (NTLMipo);
      zz_pEX NTLA= convertFacCF2NTLzz_pEX (A, NTLMipo);
      zz_pEX NTLB= convertFacCF2NTLzz_pEX (B, NTLMipo);
      return divide (NTLB, NTLA);
#endif
    }
#ifdef HAVE_FLINT
    nmod_poly_t FLINTA, FLINTB;
    convertFacCF2nmod_poly_t (FLINTA, A);
    convertFacCF2nmod_poly_t (FLINTB, B);
    nmod_poly_divrem (FLINTB, FLINTA, FLINTB, FLINTA);
    bool result= nmod_poly_is_zero (FLINTA);
    nmod_poly_clear (FLINTA);
    nmod_poly_clear (FLINTB);
    return result;
#else
    zz_pX NTLA= convertFacCF2NTLzzpX (A);
    zz_pX NTLB= convertFacCF2NTLzzpX (B);
    return divide (NTLB, NTLA);
#endif
  }
#ifdef HAVE_FLINT
  Variable alpha;
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat)
    On (SW_RATIONAL);
  if (!hasFirstAlgVar (A, alpha) && !hasFirstAlgVar (B, alpha))
  {
    fmpq_poly_t FLINTA,FLINTB;
    convertFacCF2Fmpq_poly_t (FLINTA, A);
    convertFacCF2Fmpq_poly_t (FLINTB, B);
    fmpq_poly_rem (FLINTA, FLINTB, FLINTA);
    bool result= fmpq_poly_is_zero (FLINTA);
    fmpq_poly_clear (FLINTA);
    fmpq_poly_clear (FLINTB);
    if (!isRat)
      Off (SW_RATIONAL);
    return result;
  }
  CanonicalForm Q, R;
  newtonDivrem (B, A, Q, R);
  if (!isRat)
    Off (SW_RATIONAL);
  return R.isZero();
#else
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat)
    On (SW_RATIONAL);
  bool result= fdivides (A, B);
  if (!isRat)
    Off (SW_RATIONAL);
  return result; //maybe NTL?
#endif
}

// end division

#else
CanonicalForm
mulNTL (const CanonicalForm& F, const CanonicalForm& G, const modpk& b)
{ return F*G; }
#endif
