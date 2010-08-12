/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_hnf.cc - HNF of NTL
//
// Header file: cf_hnf.h
//
//}}}

#include <config.h>

#ifdef HAVE_NTL
#include "NTLconvert.h"
#include "canonicalform.h"
#include "cf_defs.h"
#include "cf_hnf.h"
#include <NTL/mat_ZZ.h>
#include <NTL/HNF.h>

// The input matrix A is an n x m matrix of rank m (so n >= m), and D
// is a multiple of the determinant of the lattice L spanned by the
// rows of A.  W is computed as the Hermite Normal Form of A; that is,
// W is the unique m x m matrix whose rows span L, such that
//
// - W is lower triangular,
// - the diagonal entries are positive,
// - any entry below the diagonal is a non-negative number
//   strictly less than the diagonal entry in its column.
//
// via ntl
CFMatrix* cf_HNF(CFMatrix& A)
{
  mat_ZZ *AA=convertFacCFMatrix2NTLmat_ZZ(A);
  ZZ DD=convertFacCF2NTLZZ(determinant(A,A.rows()));
  mat_ZZ WW;
  HNF(WW,*AA,DD);
  delete AA;
  return convertNTLmat_ZZ2FacCFMatrix(WW);
}
#endif
