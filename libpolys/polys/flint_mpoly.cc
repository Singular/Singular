// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint mpoly
*/

#include "misc/auxiliary.h"
#include "flintconv.h"
#include "flint_mpoly.h"

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20503
#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "polys/monomials/p_polys.h"

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
    //fmpq_clear(c); // LEAK?
    pSetCoeff0(pp,n);
    pNext(pp)=p;
    p=pp;
  }
  fmpq_clear(c);
  p_Test(p,r);
  return p;
}

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  int bits=SI_LOG2(r->bitmask);
  fmpq_mpoly_init3(res,lp*lq,bits,ctx);
  fmpq_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  fmpq_mpoly_clear(res,ctx);
  fmpq_mpoly_clear(pp,ctx);
  fmpq_mpoly_clear(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
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

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  int bits=SI_LOG2(r->bitmask);
  nmod_mpoly_init3(res,lp*lq,bits,ctx);
  nmod_mpoly_mul(res,pp,qq,ctx);
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
  int bits=SI_LOG2(r->bitmask);
  nmod_mpoly_init3(res,si_max(lp,lq),bits,ctx);
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
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  int bits=SI_LOG2(r->bitmask);
  fmpq_mpoly_init3(res,si_max(lp,lq),bits,ctx);
  int ok=fmpq_mpoly_gcd(res,pp,qq,ctx);
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
  fmpq_mpoly_clear(res,ctx);
  fmpq_mpoly_clear(pp,ctx);
  fmpq_mpoly_clear(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  return pres;
}
#endif
#endif
