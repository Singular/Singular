// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/
#ifndef LIBPOLYS_POLYS_FLINTCONV_H
#define LIBPOLYS_POLYS_FLINTCONV_H

#include <polys/matpol.h>
#include <coeffs/bigintmat.h>
// #include <polys/monomials/ring.h>


#ifdef HAVE_FLINT

#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_poly_mat.h>

//  Have to define this when the new version of Flint is released
//#define FLINT_VER_2_4_5

#ifdef FLINT_VER_2_4_5
#include <flint/fmpz_lll.h>
#endif



int convFlintISingI (fmpz_t f);
void convSingIFlintI(fmpz_t f, int p);
void convFlintNSingN (mpz_t z, fmpz_t f);
void convSingNFlintN(fmpz_t f, mpz_t z);
#ifdef FLINT_VER_2_4_5
bigintmat*  singflint_LLL(bigintmat* A);
intvec* singflint_LLL(intvec* A);
#endif
#endif


#endif 
// LIBPOLYS_POLYS_FLINTCONV_H
