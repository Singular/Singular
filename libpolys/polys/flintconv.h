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

#if __FLINT_RELEASE >= 20503
#include "polys/matpol.h"
#include "coeffs/bigintmat.h"
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpq_poly.h>
#include <flint/fmpz_poly_mat.h>
#include <flint/fmpz_lll.h>

int convFlintISingI (fmpz_t f);
void convSingIFlintI(fmpz_t f, int p);
void convFlintNSingN (mpz_t z, fmpz_t f);
void convSingNFlintN(fmpz_t f, mpz_t z);
void convSingNFlintN(fmpz_t f, number n);
void convSingNFlintN(fmpq_t f, number n, const coeffs cf);
void convSingNFlintNN(fmpq_t re, fmpq_t im, number n, const coeffs cf);
number convFlintNSingN (fmpz_t f);
number convFlintNSingN (fmpq_t f, const coeffs cf);
void convSingPFlintP(fmpq_poly_t res, poly p, const ring r);
void convSingImPFlintP(fmpq_poly_t res, poly p, const ring r);
poly convFlintPSingP(fmpq_poly_t f, const ring r);

bigintmat*  singflint_LLL(bigintmat* A, bigintmat* T);
intvec* singflint_LLL(intvec* A, intvec* T);
#endif
#endif
#endif
// LIBPOLYS_POLYS_FLINTCONV_H
