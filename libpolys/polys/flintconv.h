// emacs edit mode for this file is -*- C++ -*-
#ifndef LIBPOLYS_POLYS_FLINTCONV_H
#define LIBPOLYS_POLYS_FLINTCONV_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/
/** @file flintconv.h
 *
 * This file is work in progress and currently not part of the official Singular
 *
 * @note the code is garded by the version test __FLINT_RELEASE >= 20503 (>=2.5.3)
 * In its current form it will never become an official part.
 * (conversion routines may be moved to other files/directories, etc.)
 *
 * ABSTRACT: Conversion to/from FLINT, and access to some FLINT-routines
 *
 * REQUIREMENTS:
 * - agreement about the interface to LLL
 * - FLINT providing LLL
 * (none of the above is currently true, but all of them is required)
 *
 **/

#ifdef HAVE_FLINT
#include <flint/flint.h>

#include "polys/matpol.h"
#include "coeffs/bigintmat.h"
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpq_mat.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpq_poly.h>
#include <flint/fmpz_poly_mat.h>
#if __FLINT_RELEASE >= 20500
#include <flint/fmpz_lll.h>
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_nmod.h>
#include <flint/fq_nmod_poly.h>
#include <flint/fq_nmod_mat.h>
#endif

int convFlintISingI (fmpz_t f);
void convSingIFlintI(fmpz_t f, int p);
void convFlintNSingN (mpz_t z, fmpz_t f);
void convSingNFlintN(fmpz_t f, mpz_t z);
void convSingNFlintN(fmpz_t f, number n);
void convSingNFlintN_QQ(fmpq_t f, number n);
void convSingNFlintN(fmpq_t f, number n, const coeffs cf);
void convSingNFlintNN(fmpq_t re, fmpq_t im, number n, const coeffs cf);
number convFlintNSingN (fmpz_t f);
number convFlintNSingN (fmpq_t f, const coeffs cf);
number convFlintNSingN (fmpz_t f, const coeffs cf);
number convFlintNSingN_QQ(fmpq_t f, const coeffs cf);
void convSingPFlintP(fmpq_poly_t res, poly p, const ring r);
void convSingImPFlintP(fmpq_poly_t res, poly p, const ring r);
poly convFlintPSingP(fmpq_poly_t f, const ring r);

bigintmat*  singflint_LLL(bigintmat* A, bigintmat* T);
intvec* singflint_LLL(intvec* A, intvec* T);
void convSingMFlintNmod_mat(matrix m, nmod_mat_t M, const ring r);
matrix convFlintNmod_matSingM(nmod_mat_t m, const ring r);

matrix singflint_rref(matrix  m, const ring R);
ideal  singflint_rref(ideal  m, const ring R);
matrix singflint_kernel(matrix  m, const ring R);
ideal  singflint_kernel(ideal  m, const ring R);
#if __FLINT_RELEASE >= 20500
void convSingPFlintnmod_poly_t(nmod_poly_t result, const poly p, const ring r);
void convSingMFlintFq_nmod_mat(matrix m, fq_nmod_mat_t M, const fq_nmod_ctx_t fq_con, const ring r);
poly convFlintFq_nmodSingP(const fq_nmod_t Fp, const fq_nmod_ctx_t ctx, const ring r);
matrix convFlintFq_nmod_matSingM(fq_nmod_mat_t m, const fq_nmod_ctx_t fq_con, const ring r);
#endif
#endif
#endif
// LIBPOLYS_POLYS_FLINTCONV_H
