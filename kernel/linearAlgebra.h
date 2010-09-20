/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file lineareAlgebra.h
 * 
 * This file provides basic linear algebra functionality.
 *
 * ABSTRACT: This file provides basic algorithms from linear algebra over
 * any SINGULAR-supported field.
 * For the time being, the procedures defined in this file expect matrices
 * containing objects of the SINGULAR type 'number'. This means that, when
 * 'currentRing' represents some polynomial ring K[x_1, x_2, ..., x_n], then
 * the entries of the matrices are 'numbers' representing elements of K (and
 * NOT 'polys' in K[x_1, x_2, ..., x_n]).
 * This restriction may become obselete in the future.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

// include basic SINGULAR structures
#include <kernel/structs.h>

/**
 * LU-decomposition of a given (m x n)-matrix.
 *
 * Given an (m x n) matrix A, the method computes its LU-decomposition,
 * that is, it computes matrices P, L, and U such that<br>
 * - P * A = L * U,<br>
 * - P is an (m x m) permutation matrix, i.e., its row/columns form the
 *   standard basis of K^m,<br>
 * - L is an (m x m) matrix in lower triangular form with all diagonal
 *   entries equal to 1,<br>
 * - U is an (m x n) matrix in upper row echelon form.<br>
 * From these conditions, it easily follows that also A = P * L * U,
 * since P is self-inverse.<br>
 * The method will modify all argument matrices except aMat, so that
 * they will finally contain the matrices P, L, and U as specified
 * above.
 **/
void luDecomp(
       const matrix aMat, /**< [in]  the initial matrix A,          */
       matrix &pMat,      /**< [out] the row permutation matrix P   */
       matrix &lMat,      /**< [out] the lower triangular matrix L  */
       matrix &uMat       /**< [out] the upper row echelon matrix U */
             );

/**
 * Returns a pivot score for any non-zero matrix entry.
 *
 * The smaller the score the better will n serve as a pivot element
 * in subsequent Gauss elimination steps.
 *
 * @return the pivot score of n
 **/
int pivotScore(
               number n  /**< [in] a non-zero matrix entry */
              );

/**
 * Finds the best pivot element in some part of a given matrix.
 *
 * Given any matrix A with valid row indices r1..r2 and valid column
 * indices c1..c2, this method finds the best pivot element for
 * subsequent Gauss elimination steps in A[r1..r2, c1..c2]. "Best"
 * here means best with respect to the implementation of the method
 * 'pivotScore(number n)'.<br>
 * In the case that all elements in A[r1..r2, c1..c2] are zero, the
 * method returns false, otherwise true.
 *
 * @return false if all relevant matrix entries are zero, true otherwise
 * @sa pivotScore(number n)
 **/
bool pivot(
           const matrix aMat, /**< [in]  any matrix with number entries */
           const int r1,      /**< [in]  lower row index                */
           const int r2,      /**< [in]  upper row index                */
           const int c1,      /**< [in]  lower column index             */
           const int c2,      /**< [in]  upper column index             */
           int* bestR,        /**< [out] address of row index of best
                                         pivot element                  */
           int* bestC         /**< [out] address of column index of
                                         best pivot element             */
          );

/**
 * Computes the inverse of a given (n x n)-matrix.
 *
 * This method expects an (n x n)-matrix, that is, it must have as many
 * rows as columns. Inversion of the first argument is attempted via the
 * LU-decomposition. There are two cases:<br>
 * 1) The matrix is not invertible. Then the method returns false, and
 *    &iMat remains unchanged.<br>
 * 2) The matrix is invertible. Then the method returns true, and the
 *    content of iMat is the inverse of aMat.
 *
 * @return true iff aMat is invertible, false otherwise
 * @sa luInverseFromLUDecomp(const matrix pMat, const matrix lMat,
 *                           const matrix uMat, matrix &iMat)
 **/
bool luInverse(
               const matrix aMat, /**< [in]  matrix to be inverted */
               matrix &iMat       /**< [out] inverse of aMat if
                                             invertible            */
              );

/**
 * Computes the inverse of a given (n x n)-matrix in upper right
 * triangular form.
 *
 * This method expects a quadratic matrix, that is, it must have as
 * many rows as columns. Moreover, uMat[i, j] = 0, at least for all
 * i > j, that is, u is in upper right triangular form.<br>
 * If the argument diagonalIsOne is true, then we know additionally,
 * that uMat[i, i] = 1, for all i. In this case uMat is invertible.
 * Contrariwise, if diagonalIsOne is false, we do not know anything
 * about the diagonal entries. (Note that they may still all be
 * 1.)<br>
 * In general, there are two cases:<br>
 * 1) The matrix is not invertible. Then the method returns false,
 *    and &iMat remains unchanged.<br>
 * 2) The matrix is invertible. Then the method returns true, and
 *    the content of iMat is the inverse of uMat.
 *
 * @return true iff uMat is invertible, false otherwise
 **/
bool upperRightTriangleInverse(
       const matrix uMat, /**< [in]  (n x n)-matrix in upper right
                                     triangular form               */
       matrix &iMat,      /**< [out] inverse of uMat if invertible */
       bool diagonalIsOne /**< [in]  if true, then all diagonal
                                     entries of uMat are 1         */
                              );

/**
 * Computes the inverse of a given (n x n)-matrix in lower left
 * triangular form.
 *
 * This method expects an (n x n)-matrix, that is, it must have as
 * many rows as columns. Moreover, lMat[i,j] = 0, at least for all
 * j > i, that ism lMat is in lower left triangular form.<br>
 * If the argument diagonalIsOne is true, then we know additionally,
 * that lMat[i, i] = 1, for all i. In this case lMat is invertible.
 * Contrariwise, if diagonalIsOne is false, we do not know anything
 * about the diagonal entries. (Note that they may still all be
 * 1.)<br>
 * In general, there are two cases:<br>
 * 1) The matrix is not invertible. Then the method returns false,
 *    and &iMat remains unchanged.<br>
 * 2) The matrix is invertible. Then the method returns true, and
 *    the content of iMat is the inverse of lMat.
 *
 * @return true iff lMat is invertible, false otherwise
 **/
bool lowerLeftTriangleInverse(
       const matrix lMat, /**< [in]  (n x n)-matrix in lower left
                                     triangular form               */
       matrix &iMat,      /**< [out] inverse of lMat if invertible */
       bool diagonalIsOne /**< [in]  if true, then all diagonal
                                     entries of lMat are 1         */
                              );

/**
 * Computes the inverse of an (n x n)-matrix which is given by its LU-
 * decomposition.
 *
 * With A denoting the matrix to be inverted, the method expects the
 * LU-decomposition of A, that is, pMat * A = lMat * uMat, where
 * the argument matrices have the appropriate proteries; see method
 * 'luDecomp(const matrix aMat, matrix &pMat, matrix &lMat,
 * matrix &uMat)'.<br>
 * Furthermore, uMat is expected to be an (n x n)-matrix. Then A^(-1)
 * is computed according to A^(-1) = uMat^(-1) * lMat^(-1) * pMat,
 * since pMat is self-inverse. This will work if and only if uMat is
 * invertible, because lMat and pMat are in any case invertible.<br>
 * In general, there are two cases:<br>
 * 1) uMat and hence A is not invertible. Then the method returns
 *    false, and &iMat remains unchanged.<br>
 * 2) uMat and hence A is invertible. Then the method returns true,
 *    and the content of iMat is the inverse of A.
 *
 * @return true if A is invertible, false otherwise
 * @sa luInverse(const matrix aMat, matrix &iMat)
 **/
bool luInverseFromLUDecomp(
       const matrix pMat, /**< [in]  permutation matrix of an LU-
                                     decomposition                */
       const matrix lMat, /**< [in]  lower left matrix of an LU-
                                     decomposition                */
       const matrix uMat, /**< [in]  upper right matrix of an LU-
                                     decomposition                */
       matrix &iMat       /**< [out] inverse of A if invertible   */
                          );

/**
 * Computes the rank of a given (m x n)-matrix.
 *
 * The matrix may already be given in row echelon form, e.g., when
 * the user has before called luDecomp and passes the upper triangular
 * matrix U to luRank. In this case, the second argument can be set to
 * true in order to pass this piece of information to the method.
 * Otherwise, luDecomp will be called first to compute the matrix U.
 * The rank is then read off the matrix U.
 *
 * @return the rank as an int
 * @sa luDecomp(const matrix aMat, matrix &pMat, matrix &lMat, matrix &uMat)
 **/
int luRank(
       const matrix aMat,      /**< [in]  input matrix */
       const bool isRowEchelon /**< [in]  if true then aMat is assumed to be
                                          already in row echelon form */
          );

/**
 * Solves the linear system A*x = b, where A is an (n x n)-matrix
 * which is given by its LU-decomposition.
 *
 * The method expects the LU-decomposition of A, that is,
 * pMat * A = lMat * uMat, where the argument matrices have the
 * appropriate proteries; see method
 * 'luDecomp(const matrix aMat, matrix &pMat, matrix &lMat,
 * matrix &uMat)'.<br>
 * Instead of trying to invert A and return x = A^(-1)*b, this
 * method
 * 1) computes b' = pMat * b,
 * 2) solves the easy system L * y = b', and then
 * 3) solves the easy system U * x = y.
 * Note that steps 1) and 2) will always work, as L is in any case
 * invertible. Moreover, y is uniquely determined. Step 3) will only
 * work if and only if y is in the column span of U. In that case,
 * there may be infinitely many solutions.
 * Thus, there are three cases:<br>
 * 1) There is no solution. Then the method returns false, and &xVec
      as well as &dim remain unchanged.<br>
 * 2) There is a unique solution. Then the method returns true and
 *    the dimension of the affine solution space is zero.<br>
 * 3) There are infinitely many solutions. Then the method returns
 *    true and some solution. Furthermore, the dimension of the
      affine solution space is set accordingly.
 *
 * @return true if there is at least one solution, false otherwise
 **/
bool luSolveViaLUDecomp(
       const matrix pMat, /**< [in]  permutation matrix of an LU-
                                     decomposition                */
       const matrix lMat, /**< [in]  lower left matrix of an LU-
                                     decomposition                */
       const matrix uMat, /**< [in]  upper right matrix of an LU-
                                     decomposition                */
       const matrix bVec, /**< [in]  right-hand side of the linear
                                     system to be solved          */
       matrix &xVec,      /**< [out] solution of A*x = b          */
       int* dim           /**< [out] dimension of affine solution
                                     space                        */
                          );

#endif
/* LINEAR_ALGEBRA_H */
