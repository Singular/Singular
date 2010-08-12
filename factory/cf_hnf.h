/* emacs edit mode for this file is -*- C++ -*- */
#ifndef CF_HNF_H
#define CF_HNF_H

/*BEGINPUBLIC*/

/* $Id$ */

//{{{ docu
//
// The input matrix A is square matrix of integers
// output: the Hermite Normal Form of A; that is,
// the unique m x m matrix whose rows span L, such that
//
// - lower triangular,
// - the diagonal entries are positive,
// - any entry below the diagonal is a non-negative number
//   strictly less than the diagonal entry in its column.
//
// via NTL:HNF
//
//}}}

CFMatrix* cf_HNF(CFMatrix& A);

/*ENDPUBLIC*/

#endif
