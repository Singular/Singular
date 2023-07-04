// emacs edit mode for this file is -*- C++ -*-
#ifndef LIBPOLYS_POLYS_FLINT_MPOLY_H
#define LIBPOLYS_POLYS_FLINT_MPOLY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint mpoly
*/

#ifdef HAVE_FLINT
#include <flint/flint.h>
#include "polys/monomials/monomials.h"

#if __FLINT_RELEASE >= 20503
#include <flint/fmpq.h>
#include <flint/fmpq_poly.h>
#include <flint/fmpq_mpoly.h>
#include <flint/nmod_mpoly.h>

BOOLEAN convSingRFlintR(fmpq_mpoly_ctx_t ctx, const ring r);
void convSingPFlintMP(fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, poly p, int lp,const ring r);
poly convFlintMPSingP(fmpq_mpoly_t p, fmpq_mpoly_ctx_t ctx, const ring r);

BOOLEAN convSingRFlintR(nmod_mpoly_ctx_t ctx, const ring r);
void convSingPFlintMP(nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, poly p, int lp,const ring r);
poly convFlintMPSingP(nmod_mpoly_t p, nmod_mpoly_ctx_t ctx, const ring r);

BOOLEAN convSingRFlintR(fmpz_mpoly_ctx_t ctx, const ring r);
void convSingPFlintMP(fmpz_mpoly_t res, fmpq_mpoly_ctx_t ctx, poly p, int lp,const ring r);
poly convFlintMPSingP(fmpz_mpoly_t p, fmpq_mpoly_ctx_t ctx, const ring r);

poly Flint_Mult_MP(poly p,int lp,poly q,int lq,fmpq_mpoly_ctx_t ctx, const ring r);
poly Flint_Mult_MP(poly p,int lp,poly q,int lq,nmod_mpoly_ctx_t ctx, const ring r);
poly Flint_Mult_MP(poly p,int lp,poly q,int lq,fmpz_mpoly_ctx_t ctx, const ring r);
poly Flint_Divide_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r);
poly Flint_Divide_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r);
poly Flint_GCD_MP(poly p,int lp,poly q,int lq,fmpq_mpoly_ctx_t ctx,const ring r);
poly Flint_GCD_MP(poly p,int lp,poly q,int lq,nmod_mpoly_ctx_t ctx,const ring r);
poly Flint_GCD_MP(poly p,int lp,poly q,int lq,fmpz_mpoly_ctx_t ctx,const ring r);

#endif
#endif
#endif
// LIBPOLYS_POLYS_FLINTCONV_H
