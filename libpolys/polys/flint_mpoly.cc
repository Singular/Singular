// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint mpoly
*/

#include "misc/auxiliary.h"

#ifdef HAVE_FLINT
#include "flintconv.h"
#include "flint_mpoly.h"

#if __FLINT_RELEASE >= 20503
#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "polys/monomials/p_polys.h"
#define TRANSEXT_PRIVATES
#include "polys/ext_fields/transext.h"

#include <vector>

/****** ring conversion ******/

BOOLEAN convSingRFlintR(fmpq_mpoly_ctx_t ctx, const ring r)
{
  if (rRing_ord_pure_dp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_DEGREVLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_DEGLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_LEX);
    return FALSE;
  }
  return TRUE;
}

BOOLEAN convSingRFlintR(nmod_mpoly_ctx_t ctx, const ring r)
{
  if (rRing_ord_pure_dp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_DEGREVLEX,r->cf->ch);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_DEGLEX,r->cf->ch);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_LEX,r->cf->ch);
    return FALSE;
  }
  return TRUE;
}

BOOLEAN convSingRFlintR(fmpz_mpoly_ctx_t ctx, const ring r)
{
  if (rRing_ord_pure_dp(r))
  {
    fmpz_mpoly_ctx_init(ctx,r->N,ORD_DEGREVLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    fmpz_mpoly_ctx_init(ctx,r->N,ORD_DEGLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    fmpz_mpoly_ctx_init(ctx,r->N,ORD_LEX);
    return FALSE;
  }
  return TRUE;
}

#if __FLINT_RELEASE >= 20800
BOOLEAN Flint_Factorize_MP_is_supported(const ring r)
{
  if ((r == NULL) || (r->cf == NULL)) return FALSE;
  if (!rField_is_Zp_long(r)) return FALSE;
  if ((r->cf->modExponent != 1) || (r->cf->modNumber == NULL)) return FALSE;
  return (r->OrdSgn == 1);
}

BOOLEAN convSingRFlintR(fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  if (!Flint_Factorize_MP_is_supported(r)) return TRUE;

  fmpz_t p;
  fmpz_init(p);
  fmpz_set_mpz(p, r->cf->modNumber);

  if (rRing_ord_pure_dp(r))
  {
    fmpz_mod_mpoly_ctx_init(ctx, r->N, ORD_DEGREVLEX, p);
    fmpz_clear(p);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    fmpz_mod_mpoly_ctx_init(ctx, r->N, ORD_DEGLEX, p);
    fmpz_clear(p);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    fmpz_mod_mpoly_ctx_init(ctx, r->N, ORD_LEX, p);
    fmpz_clear(p);
    return FALSE;
  }
  else if (r->OrdSgn == 1)
  {
    fmpz_mod_mpoly_ctx_init(ctx, r->N, ORD_DEGREVLEX, p);
    fmpz_clear(p);
    return FALSE;
  }

  fmpz_clear(p);
  return TRUE;
}
#endif

/******** polynomial conversion ***********/

// memory allocation is not thread safe; singular polynomials must be constructed in serial

/*
    We agree the that result of a singular -> fmpq_mpoly conversion is
    readonly. This restricts the usage of the result in flint functions to
    const arguments. However, the real readonly conversion is currently only
    implemented in the threaded conversion below since it requires a scan of
    all coefficients anyways. The _fmpq_mpoly_clear_readonly_sing needs to
    be provided for a consistent interface in the polynomial operations.
*/
static void _fmpq_mpoly_clear_readonly_sing(fmpq_mpoly_t a, fmpq_mpoly_ctx_t ctx)
{
    fmpq_mpoly_clear(a, ctx);
}

void convSingPFlintMP(fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
  fmpq_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpq_t c;
    convSingNFlintN_QQ(c,n);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    fmpq_mpoly_push_term_fmpq_ui(res, c, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    fmpq_mpoly_push_term_fmpq_ui(res, c, &(exp[1]), ctx);
    #endif
    fmpq_clear(c);
    pIter(p);
  }
  fmpq_mpoly_reduce(res, ctx); // extra step for QQ ensures res has content canonically factored out
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

poly convFlintMPSingP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, const ring r)
{
  int d=fmpq_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc0((r->N+1)*sizeof(ulong));
  fmpq_t c;
  fmpq_init(c);
  for(int i=d; i>=0; i--)
  {
    fmpq_mpoly_get_term_coeff_fmpq(c,f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    fmpq_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    fmpq_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    number n=convFlintNSingN_QQ(c,r->cf);
    pSetCoeff0(pp,n);
    pNext(pp)=p;
    p=pp;
  }
  fmpq_clear(c);
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p_Test(p,r);
  return p;
}

void convSingPFlintMP(fmpz_mpoly_t res, fmpz_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
  fmpz_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpz_t c;
    convSingNFlintN(c,n);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    fmpz_mpoly_push_term_fmpz_ui(res, c, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    fmpz_mpoly_push_term_fmpz_ui(res, c, &(exp[1]), ctx);
    #endif
    fmpz_clear(c);
    pIter(p);
  }
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

poly convFlintMPSingP(fmpz_mpoly_t f, fmpz_mpoly_ctx_t ctx, const ring r)
{
  int d=fmpz_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc0((r->N+1)*sizeof(ulong));
  fmpz_t c;
  fmpz_init(c);
  for(int i=d; i>=0; i--)
  {
    fmpz_mpoly_get_term_coeff_fmpz(c,f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    fmpz_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    fmpz_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    number n=convFlintNSingN(c,r->cf);
    pSetCoeff0(pp,n);
    pNext(pp)=p;
    p=pp;
  }
  fmpz_clear(c);
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p_Test(p,r);
  return p;
}

poly convFlintMPSingP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, const ring r)
{
  int d=nmod_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc0((r->N+1)*sizeof(ulong));
  for(int i=d; i>=0; i--)
  {
    ulong c=nmod_mpoly_get_term_coeff_ui(f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    nmod_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    nmod_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    pSetCoeff0(pp,(number)c);
    pNext(pp)=p;
    p=pp;
  }
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p_Test(p,r);
  return p;
}

void convSingPFlintMP(nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, poly p, int lp,const ring r)
{
  nmod_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    nmod_mpoly_push_term_ui_ui(res, (ulong)n, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    nmod_mpoly_push_term_ui_ui(res, (ulong)n, &(exp[1]), ctx);
    #endif
    pIter(p);
  }
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

#if __FLINT_RELEASE >= 20800
void convSingPFlintMP(fmpz_mod_mpoly_t res, fmpz_mod_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
  fmpz_mod_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  fmpz_t c;
  fmpz_init(c);
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpz_set_mpz(c, (mpz_ptr)n);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    fmpz_mod_mpoly_push_term_fmpz_ui(res, c, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    fmpz_mod_mpoly_push_term_fmpz_ui(res, c, &(exp[1]), ctx);
    #endif
    pIter(p);
  }
  fmpz_clear(c);
  fmpz_mod_mpoly_sort_terms(res, ctx);
  fmpz_mod_mpoly_combine_like_terms(res, ctx);
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

poly convFlintMPSingP(fmpz_mod_mpoly_t f, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  slong d=fmpz_mod_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc0((r->N+1)*sizeof(ulong));
  fmpz_t c;
  fmpz_init(c);
  for(slong i=d; i>=0; i--)
  {
    fmpz_mod_mpoly_get_term_coeff_fmpz(c,f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    fmpz_mod_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    fmpz_mod_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    number n=convFlintNSingN(c,r->cf);
    pSetCoeff0(pp,n);
    pNext(pp)=p;
    p=pp;
  }
  fmpz_clear(c);
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p=p_SortMerge(p,r);
  p_Test(p,r);
  return p;
}

static ideal Flint_FmpzModFactor_MP_to_Sing(fmpz_mod_mpoly_factor_t fac,
                                            intvec ** v,
                                            int with_exps,
                                            BOOLEAN include_constant,
                                            const ring r,
                                            fmpz_mod_mpoly_ctx_t ctx)
{
  slong flen=fmpz_mod_mpoly_factor_length(fac, ctx);
  BOOLEAN want_exps=((with_exps==0) || (with_exps==2));
  int len=(int)flen + (include_constant ? 1 : 0);
  if (len<=0) len=1;

  ideal res=idInit(len,1);
  if (want_exps)
  {
    *v=new intvec(len);
  }

  int j=0;
  if (include_constant)
  {
    fmpz_t c;
    fmpz_init(c);
    fmpz_mod_mpoly_factor_get_constant_fmpz(c, fac, ctx);
    res->m[j]=p_NSet(convFlintNSingN(c, r->cf), r);
    fmpz_clear(c);
    if (want_exps) (**v)[j]=1;
    j++;
  }

  fmpz_mod_mpoly_t base;
  fmpz_mod_mpoly_init(base, ctx);
  for (slong i=0; i<flen; i++, j++)
  {
    fmpz_mod_mpoly_factor_get_base(base, fac, i, ctx);
    res->m[j]=convFlintMPSingP(base, ctx, r);
    if (want_exps) (**v)[j]=(int)fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx);
  }
  fmpz_mod_mpoly_clear(base, ctx);

  if (res->m[0]==NULL)
  {
    res->m[0]=p_One(r);
    if (want_exps) (**v)[0]=1;
  }
  return res;
}

static ideal Flint_FmpzModSqrfree_MP_to_Sing(fmpz_mod_mpoly_factor_t fac,
                                             intvec ** v,
                                             int with_exps,
                                             BOOLEAN include_constant,
                                             const ring r,
                                             fmpz_mod_mpoly_ctx_t ctx)
{
  slong flen=fmpz_mod_mpoly_factor_length(fac, ctx);
  BOOLEAN want_exps=((with_exps==0) || (with_exps==2));

  fmpz_mod_mpoly_factor_sort(fac, ctx);

  slong groups=0;
  slong prev_exp=0;
  for (slong i=0; i<flen; i++)
  {
    slong e=fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx);
    if ((i==0) || (e!=prev_exp)) groups++;
    prev_exp=e;
  }

  int len=(int)groups + (include_constant ? 1 : 0);
  if (len<=0) len=1;

  ideal res=idInit(len,1);
  if (want_exps)
  {
    *v=new intvec(len);
  }

  int j=0;
  if (include_constant)
  {
    fmpz_t c;
    fmpz_init(c);
    fmpz_mod_mpoly_factor_get_constant_fmpz(c, fac, ctx);
    res->m[j]=p_NSet(convFlintNSingN(c, r->cf), r);
    fmpz_clear(c);
    if (want_exps) (**v)[j]=1;
    j++;
  }

  fmpz_mod_mpoly_t acc, base, tmp;
  fmpz_mod_mpoly_init(acc, ctx);
  fmpz_mod_mpoly_init(base, ctx);
  fmpz_mod_mpoly_init(tmp, ctx);

  for (slong i=0; i<flen; )
  {
    slong e=fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx);
    fmpz_mod_mpoly_factor_get_base(acc, fac, i, ctx);
    i++;

    while (i<flen && fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx)==e)
    {
      fmpz_mod_mpoly_factor_get_base(base, fac, i, ctx);
      fmpz_mod_mpoly_mul(tmp, acc, base, ctx);
      fmpz_mod_mpoly_swap(acc, tmp, ctx);
      i++;
    }

    res->m[j]=convFlintMPSingP(acc, ctx, r);
    if (want_exps) (**v)[j]=(int)e;
    j++;
  }

  fmpz_mod_mpoly_clear(tmp, ctx);
  fmpz_mod_mpoly_clear(base, ctx);
  fmpz_mod_mpoly_clear(acc, ctx);

  if (res->m[0]==NULL)
  {
    res->m[0]=p_One(r);
    if (want_exps) (**v)[0]=1;
  }
  return res;
}

poly Flint_Divide_MP(poly p, int lp, poly q, int lq, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  fmpz_mod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  fmpz_mod_mpoly_init(res,ctx);
  int ok=fmpz_mod_mpoly_divides(res,pp,qq,ctx);
  poly pres=NULL;
  if (ok)
  {
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  fmpz_mod_mpoly_clear(res,ctx);
  fmpz_mod_mpoly_clear(pp,ctx);
  fmpz_mod_mpoly_clear(qq,ctx);
  fmpz_mod_mpoly_ctx_clear(ctx);
  return pres;
}

poly Flint_GCD_MP(poly p, int lp, poly q, int lq, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  fmpz_mod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  fmpz_mod_mpoly_init(res,ctx);
  int ok=fmpz_mod_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    pres=convFlintMPSingP(res,ctx,r);
    p_Norm(pres,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  fmpz_mod_mpoly_clear(res,ctx);
  fmpz_mod_mpoly_clear(pp,ctx);
  fmpz_mod_mpoly_clear(qq,ctx);
  fmpz_mod_mpoly_ctx_clear(ctx);
  return pres;
}

ideal Flint_Factorize_MP(poly p, int lp, intvec ** v, int with_exps, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  fmpz_mod_mpoly_t pp;
  fmpz_mod_mpoly_factor_t fac;
  ideal res=NULL;

  convSingPFlintMP(pp, ctx, p, lp, r);
  fmpz_mod_mpoly_factor_init(fac, ctx);

  if (fmpz_mod_mpoly_factor(fac, pp, ctx))
  {
    res=Flint_FmpzModFactor_MP_to_Sing(fac, v, with_exps, with_exps==0, r, ctx);
  }

  fmpz_mod_mpoly_factor_clear(fac, ctx);
  fmpz_mod_mpoly_clear(pp, ctx);
  fmpz_mod_mpoly_ctx_clear(ctx);
  return res;
}

ideal Flint_Sqrfree_MP(poly p, int lp, intvec ** v, int with_exps, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  fmpz_mod_mpoly_t pp;
  fmpz_mod_mpoly_factor_t fac;
  ideal res=NULL;

  convSingPFlintMP(pp, ctx, p, lp, r);
  fmpz_mod_mpoly_factor_init(fac, ctx);

  if (fmpz_mod_mpoly_factor_squarefree(fac, pp, ctx))
  {
    res=Flint_FmpzModSqrfree_MP_to_Sing(fac, v, with_exps, (with_exps==0) || (with_exps==3), r, ctx);
  }

  fmpz_mod_mpoly_factor_clear(fac, ctx);
  fmpz_mod_mpoly_clear(pp, ctx);
  fmpz_mod_mpoly_ctx_clear(ctx);
  return res;
}

BOOLEAN Flint_Factorize_TransExt_MP_is_supported(const ring r)
{
  if ((r == NULL) || (r->cf == NULL) || (r->cf->extRing == NULL)) return FALSE;
  if (r->cf->extRing->qideal != NULL) return FALSE;

  const ring er = r->cf->extRing;
  if ((er == NULL) || (er->cf == NULL)) return FALSE;
  if (!rField_is_Zp_long(er)) return FALSE;
  if ((er->cf->modExponent != 1) || (er->cf->modNumber == NULL)) return FALSE;

  return ((r->OrdSgn == 1) && (er->OrdSgn == 1));
}

static BOOLEAN convSingTrRFlintR(fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  if (!Flint_Factorize_TransExt_MP_is_supported(r)) return TRUE;

  fmpz_t p;
  fmpz_init(p);
  fmpz_set_mpz(p, r->cf->extRing->cf->modNumber);
  fmpz_mod_mpoly_ctx_init(ctx, rPar(r) + rVar(r), ORD_DEGREVLEX, p);
  fmpz_clear(p);
  return FALSE;
}

static BOOLEAN convSingTrPFlintMP(fmpz_mod_mpoly_t res, fmpz_mod_mpoly_ctx_t ctx,
                                  poly p, const ring r)
{
  const ring er = r->cf->extRing;
  const int np = rPar(r);
  const int nv = rVar(r);
  const int nt = np + nv;

  fmpz_mod_mpoly_init(res, ctx);

  ulong* exp=(ulong*)omAlloc0(nt*sizeof(ulong));
  fmpz_t c, d, dinv, modp;
  fmpz_init(c);
  fmpz_init(d);
  fmpz_init(dinv);
  fmpz_init(modp);
  fmpz_set_mpz(modp, er->cf->modNumber);

  while (p != NULL)
  {
    fraction fr = (fraction)pGetCoeff(p);
    if (!IS0(fr))
    {
      BOOLEAN have_dinv = FALSE;
      poly den = DEN(fr);
      if (den != NULL)
      {
        if (!p_IsConstant(den, er))
        {
          omFreeSize(exp, nt*sizeof(ulong));
          fmpz_clear(modp);
          fmpz_clear(dinv);
          fmpz_clear(d);
          fmpz_clear(c);
          fmpz_mod_mpoly_clear(res, ctx);
          return TRUE;
        }
        fmpz_set_mpz(d, (mpz_ptr)pGetCoeff(den));
        if (!fmpz_invmod(dinv, d, modp))
        {
          omFreeSize(exp, nt*sizeof(ulong));
          fmpz_clear(modp);
          fmpz_clear(dinv);
          fmpz_clear(d);
          fmpz_clear(c);
          fmpz_mod_mpoly_clear(res, ctx);
          return TRUE;
        }
        have_dinv = TRUE;
      }

      for (poly cp = NUM(fr); cp != NULL; pIter(cp))
      {
        for (int i=0; i<nt; i++) exp[i]=0;
        for (int i=1; i<=np; i++) exp[i-1]=p_GetExp(cp, i, er);
        for (int i=1; i<=nv; i++) exp[np+i-1]=p_GetExp(p, i, r);

        fmpz_set_mpz(c, (mpz_ptr)pGetCoeff(cp));
        if (have_dinv) fmpz_mul(c, c, dinv);
        fmpz_mod_mpoly_push_term_fmpz_ui(res, c, exp, ctx);
      }
    }
    pIter(p);
  }

  fmpz_mod_mpoly_sort_terms(res, ctx);
  fmpz_mod_mpoly_combine_like_terms(res, ctx);

  omFreeSize(exp, nt*sizeof(ulong));
  fmpz_clear(modp);
  fmpz_clear(dinv);
  fmpz_clear(d);
  fmpz_clear(c);
  return FALSE;
}

static poly convFlintMPSingTrP(fmpz_mod_mpoly_t f, fmpz_mod_mpoly_ctx_t ctx, const ring r)
{
  const ring er = r->cf->extRing;
  const int np = rPar(r);
  const int nv = rVar(r);
  const int nt = np + nv;

  slong d=fmpz_mod_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc0(nt*sizeof(ulong));
  fmpz_t c;
  fmpz_init(c);

  for (slong i=d; i>=0; i--)
  {
    fmpz_mod_mpoly_get_term_coeff_fmpz(c, f, i, ctx);
    fmpz_mod_mpoly_get_term_exp_ui(exp, f, i, ctx);

    poly cp=p_Init(er);
    for (int j=1; j<=np; j++) p_SetExp(cp, j, exp[j-1], er);
    p_Setm(cp, er);
    pSetCoeff0(cp, convFlintNSingN(c, er->cf));

    poly pp=p_Init(r);
    for (int j=1; j<=nv; j++) p_SetExp(pp, j, exp[np+j-1], r);
    p_Setm(pp, r);
    pSetCoeff0(pp, ntInit(cp, r->cf));

    pNext(pp)=p;
    p=pp;
  }

  fmpz_clear(c);
  omFreeSize(exp, nt*sizeof(ulong));
  p=p_SortAdd(p, r);
  p_Test(p, r);
  return p;
}

static poly Flint_TransExt_Constant_MP_to_Sing(fmpz_t c, const ring r)
{
  const ring er = r->cf->extRing;
  poly cp=p_NSet(convFlintNSingN(c, er->cf), er);
  return p_NSet(ntInit(cp, r->cf), r);
}

static void Flint_TransExt_Append_Factor(std::vector<poly>& factors,
                                         std::vector<int>& exps,
                                         poly fp,
                                         int e,
                                         poly& unit,
                                         BOOLEAN include_constant,
                                         const ring r)
{
  if (fp == NULL) return;

  if (p_IsConstant(fp, r))
  {
    if (include_constant)
    {
      for (int i=0; i<e; i++)
      {
        unit=p_Mult_q(unit, p_Copy(fp, r), r);
      }
    }
    p_Delete(&fp, r);
  }
  else
  {
    factors.push_back(fp);
    exps.push_back(e);
  }
}

static ideal Flint_TransExt_Factors_MP_to_Sing(fmpz_mod_mpoly_factor_t fac,
                                               intvec ** v,
                                               int with_exps,
                                               BOOLEAN include_constant,
                                               const ring r,
                                               fmpz_mod_mpoly_ctx_t ctx)
{
  fmpz_mod_mpoly_factor_sort(fac, ctx);

  std::vector<poly> factors;
  std::vector<int> exps;
  poly unit=p_One(r);

  fmpz_t c;
  fmpz_init(c);
  fmpz_mod_mpoly_factor_get_constant_fmpz(c, fac, ctx);
  poly cpoly=Flint_TransExt_Constant_MP_to_Sing(c, r);
  unit=p_Mult_q(unit, cpoly, r);
  fmpz_clear(c);

  fmpz_mod_mpoly_t base;
  fmpz_mod_mpoly_init(base, ctx);

  const slong flen=fmpz_mod_mpoly_factor_length(fac, ctx);
  for (slong i=0; i<flen; i++)
  {
    fmpz_mod_mpoly_factor_get_base(base, fac, i, ctx);
    poly fp=convFlintMPSingTrP(base, ctx, r);
    int e=(int)fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx);
    Flint_TransExt_Append_Factor(factors, exps, fp, e, unit, include_constant, r);
  }

  fmpz_mod_mpoly_clear(base, ctx);

  const BOOLEAN want_exps=((with_exps==0) || (with_exps==2));
  int len=(int)factors.size() + (include_constant ? 1 : 0);
  if (len<=0) len=1;

  ideal res=idInit(len, 1);
  if (want_exps)
  {
    *v=new intvec(len);
  }

  int j=0;
  if (include_constant)
  {
    res->m[j]=unit;
    if (want_exps) (**v)[j]=1;
    j++;
  }
  else
  {
    p_Delete(&unit, r);
  }

  for (size_t i=0; i<factors.size(); i++, j++)
  {
    res->m[j]=factors[i];
    if (want_exps) (**v)[j]=exps[i];
  }

  if (res->m[0]==NULL)
  {
    res->m[0]=p_One(r);
    if (want_exps) (**v)[0]=1;
  }

  return res;
}

static ideal Flint_TransExt_Sqrfree_MP_to_Sing(fmpz_mod_mpoly_factor_t fac,
                                               intvec ** v,
                                               int with_exps,
                                               BOOLEAN include_constant,
                                               const ring r,
                                               fmpz_mod_mpoly_ctx_t ctx)
{
  fmpz_mod_mpoly_factor_sort(fac, ctx);

  std::vector<poly> factors;
  std::vector<int> exps;
  poly unit=p_One(r);

  fmpz_t c;
  fmpz_init(c);
  fmpz_mod_mpoly_factor_get_constant_fmpz(c, fac, ctx);
  poly cpoly=Flint_TransExt_Constant_MP_to_Sing(c, r);
  unit=p_Mult_q(unit, cpoly, r);
  fmpz_clear(c);

  fmpz_mod_mpoly_t acc, base, tmp;
  fmpz_mod_mpoly_init(acc, ctx);
  fmpz_mod_mpoly_init(base, ctx);
  fmpz_mod_mpoly_init(tmp, ctx);

  const slong flen=fmpz_mod_mpoly_factor_length(fac, ctx);
  for (slong i=0; i<flen; )
  {
    slong e=fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx);
    fmpz_mod_mpoly_factor_get_base(acc, fac, i, ctx);
    i++;

    while (i<flen && fmpz_mod_mpoly_factor_get_exp_si(fac, i, ctx)==e)
    {
      fmpz_mod_mpoly_factor_get_base(base, fac, i, ctx);
      fmpz_mod_mpoly_mul(tmp, acc, base, ctx);
      fmpz_mod_mpoly_swap(acc, tmp, ctx);
      i++;
    }

    poly fp=convFlintMPSingTrP(acc, ctx, r);
    Flint_TransExt_Append_Factor(factors, exps, fp, (int)e, unit, include_constant, r);
  }

  fmpz_mod_mpoly_clear(tmp, ctx);
  fmpz_mod_mpoly_clear(base, ctx);
  fmpz_mod_mpoly_clear(acc, ctx);

  const BOOLEAN want_exps=((with_exps==0) || (with_exps==2));
  int len=(int)factors.size() + (include_constant ? 1 : 0);
  if (len<=0) len=1;

  ideal res=idInit(len, 1);
  if (want_exps)
  {
    *v=new intvec(len);
  }

  int j=0;
  if (include_constant)
  {
    res->m[j]=unit;
    if (want_exps) (**v)[j]=1;
    j++;
  }
  else
  {
    p_Delete(&unit, r);
  }

  for (size_t i=0; i<factors.size(); i++, j++)
  {
    res->m[j]=factors[i];
    if (want_exps) (**v)[j]=exps[i];
  }

  if (res->m[0]==NULL)
  {
    res->m[0]=p_One(r);
    if (want_exps) (**v)[0]=1;
  }

  return res;
}

ideal Flint_Factorize_TransExt_MP(poly p, intvec ** v, int with_exps, const ring r)
{
  fmpz_mod_mpoly_ctx_t ctx;
  if (convSingTrRFlintR(ctx, r)) return NULL;

  fmpz_mod_mpoly_t pp;
  fmpz_mod_mpoly_factor_t fac;
  ideal res=NULL;

  if (!convSingTrPFlintMP(pp, ctx, p, r))
  {
    fmpz_mod_mpoly_factor_init(fac, ctx);
    if (fmpz_mod_mpoly_factor(fac, pp, ctx))
    {
      res=Flint_TransExt_Factors_MP_to_Sing(fac, v, with_exps, with_exps==0, r, ctx);
    }
    fmpz_mod_mpoly_factor_clear(fac, ctx);
    fmpz_mod_mpoly_clear(pp, ctx);
  }

  fmpz_mod_mpoly_ctx_clear(ctx);
  return res;
}

ideal Flint_Sqrfree_TransExt_MP(poly p, intvec ** v, int with_exps, const ring r)
{
  fmpz_mod_mpoly_ctx_t ctx;
  if (convSingTrRFlintR(ctx, r)) return NULL;

  fmpz_mod_mpoly_t pp;
  fmpz_mod_mpoly_factor_t fac;
  ideal res=NULL;

  if (!convSingTrPFlintMP(pp, ctx, p, r))
  {
    fmpz_mod_mpoly_factor_init(fac, ctx);
    if (fmpz_mod_mpoly_factor_squarefree(fac, pp, ctx))
    {
      res=Flint_TransExt_Sqrfree_MP_to_Sing(fac, v, with_exps,
                                            (with_exps==0) || (with_exps==3),
                                            r, ctx);
    }
    fmpz_mod_mpoly_factor_clear(fac, ctx);
    fmpz_mod_mpoly_clear(pp, ctx);
  }

  fmpz_mod_mpoly_ctx_clear(ctx);
  return res;
}
#endif

/****** polynomial operations ***********/

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  fmpq_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  nmod_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, fmpz_mpoly_ctx_t ctx, const ring r)
{
  fmpz_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpz_mpoly_init(res,ctx);
  fmpz_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  fmpz_mpoly_clear(res,ctx);
  fmpz_mpoly_clear(pp,ctx);
  fmpz_mpoly_clear(qq,ctx);
  fmpz_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

// Zero will be returned if the division is not exact
poly Flint_Divide_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  fmpq_mpoly_divides(res,pp,qq,ctx);
  poly pres = convFlintMPSingP(res,ctx,r);
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_Divide_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  nmod_mpoly_divides(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_GCD_MP(poly p,int lp,poly q,int lq,nmod_mpoly_ctx_t ctx,const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  int ok=nmod_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  return pres;
}

poly Flint_GCD_MP(poly p,int lp,poly q,int lq,fmpq_mpoly_ctx_t ctx,const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  int ok=fmpq_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    // Flint normalizes the gcd to be monic.
    // Singular wants a gcd defined over ZZ that is primitive and has a positive leading coeff.
    //if (!fmpq_mpoly_is_zero(res, ctx))
    {
      fmpq_t content;
      fmpq_init(content);
      fmpq_mpoly_content(content, res, ctx);
      fmpq_mpoly_scalar_div_fmpq(res, res, content, ctx);
      fmpq_clear(content);
    }
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  return pres;
}

poly Flint_GCD_MP(poly p,int lp,poly q,int lq,fmpz_mpoly_ctx_t ctx,const ring r)
{
  fmpz_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  fmpz_mpoly_init(res,ctx);
  int ok=fmpz_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    // Singular wants a gcd defined over ZZ that is primitive and has a positive leading coeff.
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  fmpz_mpoly_clear(res,ctx);
  fmpz_mpoly_clear(pp,ctx);
  fmpz_mpoly_clear(qq,ctx);
  fmpz_mpoly_ctx_clear(ctx);
  return pres;
}

#endif
#endif
