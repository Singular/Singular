/* emacs edit mode for this file is -*- C++ -*- */
#ifndef CF_HNF_H
#define CF_HNF_H

/*BEGINPUBLIC*/

/**
 *
 * The input matrix A is square matrix of integers
 * output: the Hermite Normal Form of A; that is,
 * the unique m x m matrix whose rows span L, such that
 *
 * - lower triangular,
 * - the diagonal entries are positive,
 * - any entry below the diagonal is a non-negative number
 *   strictly less than the diagonal entry in its column.
 *
 * @note: uses NTL
 *
**/

CFMatrix* FACTORY_PUBLIC cf_HNF(CFMatrix& A);

/**
 * performs LLL reduction.
 *
 * B is an m x n matrix, viewed as m rows of n-vectors.  m may be less
 * than, equal to, or greater than n, and the rows need not be
 * linearly independent.  B is transformed into an LLL-reduced basis,
 * and the return value is the rank r of B.  The first m-r rows of B
 * are zero.
 *
 * More specifically, elementary row transformations are performed on
 * B so that the non-zero rows of new-B form an LLL-reduced basis
 * for the lattice spanned by the rows of old-B.
 * The default reduction parameter is delta=3/4, which means
 * that the squared length of the first non-zero basis vector
 * is no more than 2^{r-1} times that of the shortest vector in
 * the lattice.
 *
 * @note: uses NTL or FLINT
**/

CFMatrix* FACTORY_PUBLIC cf_LLL(CFMatrix& A);

/*ENDPUBLIC*/

#endif
