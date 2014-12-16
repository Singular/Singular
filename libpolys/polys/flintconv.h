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
 * @note the code is garded by the undefined macro FLINT_VER_2_4_5
 * In its current form it will never become an official part.
 * (conversion routines may be moved to other files/directories, etc.)
 *
 * ABSTRACT: Conversion to/from FLINT, and access to some FLINT-routines
 *
 * REQUIREMENTS:
 * - agreement about the interface to LLL
 * - SINGULAR_4_1
 * - FLINT providing LLL
 * (none of the above is currently true, but all of them is required)
 *
 **/

//  Have to define this when this code shall be used:
//#define FLINT_VER_2_4_5
#ifdef FLINT_VER_2_4_5
#include <polys/matpol.h>
#include <coeffs/bigintmat.h>
// #include <polys/monomials/ring.h>


#ifdef HAVE_FLINT
#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_poly_mat.h>
#include <flint/fmpz_lll.h>

int convFlintISingI (fmpz_t f);
void convSingIFlintI(fmpz_t f, int p);
void convFlintNSingN (mpz_t z, fmpz_t f);
void convSingNFlintN(fmpz_t f, mpz_t z);
bigintmat*  singflint_LLL(bigintmat* A, bigintmat* T);
intvec* singflint_LLL(intvec* A, intvec* T);
#endif

#endif
#endif
// LIBPOLYS_POLYS_FLINTCONV_H
