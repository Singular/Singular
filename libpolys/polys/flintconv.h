// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/


#include <polys/monomials/ring.h>
//#include <factory/factory.h>
#ifdef HAVE_FLINT

#include <flint/fmpz_poly.h>
#include <flint/fmpz_lll.h>
#include <flint/fmpz_poly_mat.h>

#endif
#include <polys/matpol.h>
#include <coeffs/bigintmat.h>

//  Have to define this when the new version of Flint is released
//#define FLINT_VER_2_4_5

#ifdef HAVE_FLINT
#ifdef FLINT_VER_2_4_5
int convFlintISingI (fmpz_t f);
void convSingIFlintI(fmpz_t f, int p);
void convFlintNSingN (mpz_t z, fmpz_t f);
void convSingNFlintN(fmpz_t f, mpz_t z);
bigintmat*  singflint_LLL(bigintmat* A);
intvec* singflint_LLL(intvec* A);
#endif
#endif
