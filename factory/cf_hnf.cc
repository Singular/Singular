/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_hnf.cc
 *
 * HNF/LLL of NTL
 *
 * Header file: cf_hnf.h
 *
**/


#include "config.h"

#include "canonicalform.h"
#include "cf_defs.h"
#include "cf_hnf.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#include <NTL/mat_ZZ.h>
#include <NTL/HNF.h>
#include <NTL/LLL.h>
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

/**
 * The input matrix A is an n x m matrix of rank m (so n >= m), and D
 * is a multiple of the determinant of the lattice L spanned by the
 * rows of A.  W is computed as the Hermite Normal Form of A; that is,
 * W is the unique m x m matrix whose rows span L, such that
 *
 * - W is lower triangular,
 * - the diagonal entries are positive,
 * - any entry below the diagonal is a non-negative number
 *   strictly less than the diagonal entry in its column.
 *
**/
CFMatrix* cf_HNF(CFMatrix& A)
{
#ifdef HAVE_FLINT
  fmpz_mat_t FLINTM;
  convertFacCFMatrix2Fmpz_mat_t(FLINTM,A);
  fmpz_mat_hnf(FLINTM,FLINTM);
  CFMatrix *r=convertFmpz_mat_t2FacCFMatrix(FLINTM);
  fmpz_mat_clear(FLINTM);
  return r;
#elif defined(HAVE_NTL)
  mat_ZZ *AA=convertFacCFMatrix2NTLmat_ZZ(A);
  ZZ DD=convertFacCF2NTLZZ(determinant(A,A.rows()));
  mat_ZZ WW;
  HNF(WW,*AA,DD);
  delete AA;
  return convertNTLmat_ZZ2FacCFMatrix(WW);
#endif
}

CFMatrix* cf_LLL(CFMatrix& A)
{
#ifdef HAVE_FLINT
  fmpz_mat_t FLINTM;
  convertFacCFMatrix2Fmpz_mat_t(FLINTM,A);
  fmpq_t delta,eta;
  fmpq_init(delta); fmpq_set_si(delta,1,1);
  fmpq_init(eta); fmpq_set_si(eta,3,4);
  fmpz_mat_lll_storjohann(FLINTM,delta,eta);
  CFMatrix *r=convertFmpz_mat_t2FacCFMatrix(FLINTM);
  fmpz_mat_clear(FLINTM);
  return r;
#elif defined(HAVE_NTL)
  mat_ZZ *AA=convertFacCFMatrix2NTLmat_ZZ(A);
  ZZ det2;
  LLL(det2,*AA,0L);
  CFMatrix *r= convertNTLmat_ZZ2FacCFMatrix(*AA);
  delete AA;
  return r;
#endif
}
