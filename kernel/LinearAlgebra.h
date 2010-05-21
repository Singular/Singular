#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: LineareAlgebra.h$ */
/*
 * ABSTRACT: linear algebra over any SINGULAR-supported field
 *
 * Use 'K' in all method documentations to denote the field.
 * All matrices herein are assumed to have 'number' entries,
 * representing elements of K.
 * Use 'r' and 'c' as canonical row and column indices to ease
 * readability of the code.
 */

#include <structs.h>

/**
  * LU-decomposition of a given (m x n)-matrix.
  *
  * Given an (m x n) matrix A, the method computes its LU-decomposition,
  * that is, it computes matrices P, L, and U such that
  * - A = P * L * U,
  * - P is an (m x m) permutation matrix, i.e., its row/columns are the
  *   standard basis vectors of K^m
  * - L is an (m x m) matrix in lower triangular form,
  * - U is an (m x n) matrix in upper row echelon form
  * From these conditions, it easily follows that also
  * P * A = L * U, as P is self-inverse.
  * The method assumes that U contains the matrix A for which the
  * decomposition is being sought.
  * The method will modify all agument matrices so that they will
  * finally contain the matrices P, L, and U with the above properties.
  *
  * @param pMat afterwards the row permutation matrix P
  * @param lMat afterwards the lower triangular matrix L
  * @param uMat afterwards the upper row echelon matrix U, initially A
  */
void luDecomp(matrix &pMat, matrix &lMat, matrix uMat);

/**
  * Finds the best pivot element in some part of a given matrix.
  *
  * Given any matrix A with valid row indices r1..r2 and valid column
  * indices c1..c2, this method finds the best pivot element for
  * continuing Gauss elimination in A. "Best" here means best according
  * to the implemented pivotStrategy; see there.
  * In case all elements in A[r1..r2, c1..c2] are zero, the method returns
  * false, otherwise true.
  *
  * @param aMat any matrix
  * @param r1 the lower row index, determines the relevant part of aMat
  * @param r2 the upper row index, determines the relevant part of aMat
  * @param c1 the lower column index, determines the relevant part of aMat
  * @param c2 the upper column index, determines the relevant part of aMat
  * @param bestR afterwards address of the row index of the best pivot element
  * @param bestC afterwards address of the col index of the best pivot element
  * @return false if all relevant matrix entries are zero, false otherwise
  */
bool pivot(const matrix aMat, const int r1, const int r2, const int c1,
           const int c2, int* bestR, int* bestC);

/**
  * Computes an estimate for how well a given non-zero number can serve as
  * pivot element for the next Gauss elimination step.
  *
  * This method expects a non-zero number. The following rules must be obeyed:
  * - return a non-negative int
  * - the higher the value, the better will n serve as pivot element
  *
  * @param n a number representing a non-zero element of the ground field K
  * @return estimate for how well n will serve as pivot element
  */
int pivotStrategy(const number n);

/**
  * Computes the inverse of a quadratic matrix.
  *
  * This method expects a quadratic matrix, that is, it must have as many rows
  * as columns. Inversion of the first argument is attempted via the
  * LU-decomposition. There are two cases:
  * 1) The matrix is not invertible. Then the method returns false, and the
  *    content of iMat is useless.
  * 2) The matrix is invertible. Then the method returns true, and the
  *    content of iMat is the inverse of aMat.
  *
  * @param aMat the quadratic matrix to be inverted
  * @param afterwards iMat the inverse of aMat in case that aMat is invertible
  * @return true iff aMat is invertible, false otherwise
  */
bool luInverse(const matrix aMat, matrix &iMat);

/**
  * Computes the inverse of a quadratic matrix in upper right row echelon form.
  *
  * This method expects a quadratic matrix, that is, it must have as many rows
  * as columns. Moreover, uMat[i,j] = 0, at least for all i > j.
  * If the argument diagonalIsOne is true, then we know additionally, that
  * uMat[i,i] = 1, for all i. In this case uMat is invertible. If diagonalIsOne
  * is false, we do not know anything about the diagonal entries. (Note that
  * they may still all be 1.)
  * In general, there are two cases:
  * 1) The matrix is not invertible. Then the method returns false, and the
  *    content of iMat is useless.
  * 2) The matrix is invertible. Then the method returns true, and the
  *    content of iMat is the inverse of aMat.
  *
  * @param uMat a quadratic matrix in upper right row echelon form
  * @param iMat afterwards the inverse of uMat in case that uMat is invertible
  * @param diagonalIsOne if true all diagonal entries of uMat are 1
  * @return true iff uMat is invertible, false otherwise
  */
bool upperRightTriangleInverse(const matrix uMat, matrix &iMat,
                               bool diagonalIsOne);

/**
  * Computes the inverse of a quadratic matrix in lower left row echelon form.
  *
  * This method expects a quadratic matrix, that is, it must have as many rows
  * as columns. Moreover, lMat[i,j] = 0, at least for all j > i.
  * If the argument diagonalIsOne is true, then we know additionally, that
  * lMat[i,i] = 1, for all i. In this case lMat is invertible. If diagonalIsOne
  * is false, we do not know anything about the diagonal entries. (Note that
  * they may still all be 1.)
  * In general, there are two cases:
  * 1) The matrix is not invertible. Then the method returns false, and the
  *    content of iMat is useless.
  * 2) The matrix is invertible. Then the method returns true, and the
  *    content of iMat is the inverse of aMat.
  *
  * @param lMat a quadratic matrix in lower left row echelon form
  * @param iMat afterwards the inverse of lMat in case that lMat is invertible
  * @param diagonalIsOne if true all diagonal entries of lMat are 1
  * @return true iff lMat is invertible, false otherwise
  */
bool lowerLeftTriangleInverse(const matrix uMat, matrix &iMat,
                              bool diagonalIsOne);

/**
  * Computes the inverse of a quadratic matrix which is given by its LU-
  * decomposition.
  *
  * With A denoting the matrix to be inverted, the method expects the
  * LU-decomposition of A, that is, pMat * A = lMat * uMat, where
  * the argument matrices have the appropriate proteries.
  * Furthermore, uMat is expected to be quadratic. Then A^(-1) is computed
  * as A^(-1) = uMat^(-1) * lMat^(-1) * pMat, since pMat is self-inverse.
  * This will work if and only if uMat is invertible, as lMat and pMat are
  * in any case invertible.
  *
  * There are two cases:
  * 1) A is not invertible. Then the method returns false, and the
  *    content of iMat is useless.
  * 2) A is invertible. Then the method returns true, and the content
  *    of iMat is the inverse of A.
  *
  * @param pMat the permutation matrix of the LU-decomposition of A
  * @param lMat the lower triangle matrix of the LU-decomposition of A
  * @param uMat the upper row echelon matrix of the LU-decomposition of A
  * @param iMat afterwards the inverse of A in case that A is invertible
  * @return true iff A is invertible, false otherwise
  */
bool luInverseFromLUDecomp(const matrix pMat, const matrix lMat,
                           const matrix uMat, matrix &iMat);

#endif
/* LINEAR_ALGEBRA_H */
