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
 * This restriction may become obsolete in the future.
 *
 * @author Frank Seelisch
 *
 *
 **/
/*****************************************************************************/

#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

// include basic SINGULAR structures
#include "kernel/structs.h"

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
 * From these conditions, it follows immediately that also A = P * L * U,
 * since P is self-inverse.<br>
 *
 * The method will modify all argument matrices except aMat, so that
 * they will finally contain the matrices P, L, and U as specified
 * above.
 **/
void luDecomp(
       const matrix aMat, /**< [in]  the initial matrix A,          */
       matrix &pMat,      /**< [out] the row permutation matrix P   */
       matrix &lMat,      /**< [out] the lower triangular matrix L  */
       matrix &uMat,      /**< [out] the upper row echelon matrix U */
       const ring r= currRing       /**< [in] current ring */
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
               number n,    /**< [in] a non-zero matrix entry */
               const ring r= currRing /**< [in] current ring */
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
           int* bestC,        /**< [out] address of column index of
                                         best pivot element             */
           const ring r= currRing       /**< [in] current ring */
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
               matrix &iMat,      /**< [out] inverse of aMat if
                                             invertible            */
               const ring r=currRing /**< [in] current ring */
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
       bool diagonalIsOne,/**< [in]  if true, then all diagonal
                                     entries of uMat are 1         */
       const ring r= currRing /**< [in] current ring */
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
       matrix &iMat,      /**< [out] inverse of A if invertible   */
       const ring r= currRing
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
       const bool isRowEchelon,/**< [in]  if true then aMat is assumed to be
                                          already in row echelon form */
       const ring r= currRing
          );

/**
 * Solves the linear system A * x = b, where A is an (m x n)-matrix
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
 * 2) solves the simple system L * y = b', and then
 * 3) solves the simple system U * x = y.
 * Note that steps 1) and 2) will always work, as L is in any case
 * invertible. Moreover, y is uniquely determined. Step 3) will only
 * work if and only if y is in the column span of U. In that case,
 * there may be infinitely many solutions.
 * Thus, there are three cases:<br>
 * 1) There is no solution. Then the method returns false, and &xVec
 *    as well as &H remain unchanged.<br>
 * 2) There is a unique solution. Then the method returns true and
 *    H is the 1x1 matrix with zero entry.<br>
 * 3) There are infinitely many solutions. Then the method returns
 *    true and some solution of the given original linear system.
 *    Furthermore, the columns of H span the solution space of the
 *    homogeneous linear system. The dimension of the solution
 *    space is then the number of columns of H.
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
       matrix &H          /**< [out] matrix with columns spanning
                                     homogeneous solution space   */
                          );

/**
 * Solves the linear system A * x = b, where A is an (m x n)-matrix
 * which is given by its LDU-decomposition.
 *
 * The method expects the LDU-decomposition of A, that is,
 * pMat * A = lMat * dMat^(-1) * uMat, where the argument matrices have
 * the appropriate proteries; see method
 * 'lduDecomp(const matrix aMat, matrix &pMat, matrix &lMat,
 * matrix &dMat, matrix &uMat, poly &l, poly &u, poly &lTimesU)'.<br>
 * Instead of trying to invert A and return x = A^(-1)*b, this
 * method
 * 1) computes b' = l * pMat * b,
 * 2) solves the simple system L * y = b',
 * 3) computes y' = u * dMat * y,
 * 4) solves the simple system U * y'' = y',
 * 5) computes x = 1/(lTimesU) * y''.
 * Note that steps 1), 2) and 3) will always work, as L is in any case
 * invertible. Moreover, y and thus y' are uniquely determined.
 * Step 4) will only work if and only if y' is in the column span of U.
 * In that case, there may be infinitely many solutions.
 * In contrast to luSolveViaLUDecomp, this algorithm guarantees that
 * all divisions which have to be performed in steps 2) and 4) will
 * work without remainder. This is due to properties of the given LDU-
 * decomposition. Only the final step 5) performs a division of a vector
 * by a member of the ground field. (Suppose the ground field is Q or
 * some rational function field. Then, if A contains only integers or
 * polynomials, respectively, then steps 1) - 4) will keep all data
 * integer or polynomial, respectively. This may speed up computations
 * considerably.)
 * For the outcome, there are three cases:<br>
 * 1) There is no solution. Then the method returns false, and &xVec
 *    as well as &H remain unchanged.<br>
 * 2) There is a unique solution. Then the method returns true and
 *    H is the 1x1 matrix with zero entry.<br>
 * 3) There are infinitely many solutions. Then the method returns
 *    true and some solution of the given original linear system.
 *    Furthermore, the columns of H span the solution space of the
 *    homogeneous linear system. The dimension of the solution
 *    space is then the number of columns of H.
 *
 * @return true if there is at least one solution, false otherwise
 **/
bool luSolveViaLDUDecomp(
       const matrix pMat,  /**< [in]  permutation matrix of an LDU-
                                      decomposition                     */
       const matrix lMat,  /**< [in]  lower left matrix of an LDU-
                                      decomposition                     */
       const matrix dMat,  /**< [in]  diagonal matrix of an LDU-
                                      decomposition                     */
       const matrix uMat,  /**< [in]  upper right matrix of an LDU-
                                      decomposition                     */
       const poly l,       /**< [in]  pivot product l of an LDU decomp. */
       const poly u,       /**< [in]  pivot product u of an LDU decomp. */
       const poly lTimesU, /**< [in]  product of l and u                */
       const matrix bVec,  /**< [in]  right-hand side of the linear
                                      system to be solved               */
       matrix &xVec,       /**< [out] solution of A*x = b               */
       matrix &H           /**< [out] matrix with columns spanning
                                      homogeneous solution space        */
                          );

/**
 * Creates a new matrix which is the (nxn) unit matrix, and returns true
 * in case of success.
 *
 * The method will be successful whenever n >= 1. In this case and only then
 * true will be returned and the new (nxn) unit matrix will reside inside
 * the second argument.
 *
 * @return true iff the (nxn) unit matrix could be build
 **/
bool unitMatrix(
       const int n,     /**< [in]  size of the matrix */
       matrix &unitMat,  /**< [out] the new (nxn) unit matrix */
       const ring r= currRing /** [in] current ring */
               );

/**
 * Creates a new matrix which is a submatrix of the first argument, and
 * returns true in case of success.
 *
 * The method will be successful whenever rowIndex1 <= rowIndex2 and
 * colIndex1 <= colIndex2. In this case and only then true will be
 * returned and the last argument will afterwards contain a copy of the
 * respective submatrix of the first argument.
 * Note that this method may also be used to copy an entire matrix.
 *
 * @return true iff the submatrix could be build
 **/
bool subMatrix(
       const matrix aMat,    /**< [in]  the input matrix */
       const int rowIndex1,  /**< [in]  lower row index */
       const int rowIndex2,  /**< [in]  higher row index */
       const int colIndex1,  /**< [in]  lower column index */
       const int colIndex2,  /**< [in]  higher column index */
       matrix &subMat        /**< [out] the new submatrix */
              );

/**
 * Swaps two rows of a given matrix and thereby modifies it.
 *
 * The method expects two valid, distinct row indices, i.e. in
 * [1..n], where n is the number of rows in aMat.
 **/
void swapRows(
       int row1,     /**< [in]     index of first row to swap */
       int row2,     /**< [in]     index of second row to swap */
       matrix& aMat  /**< [in/out] matrix subject to swapping */
             );

/**
 * Swaps two columns of a given matrix and thereby modifies it.
 *
 * The method expects two valid, distinct column indices, i.e. in
 * [1..n], where n is the number of columns in aMat.
 **/
void swapColumns(
       int column1,  /**< [in]     index of first column to swap */
       int column2,  /**< [in]     index of second column to swap */
       matrix& aMat  /**< [in/out] matrix subject to swapping */
             );

/**
 * Creates a new matrix which contains the first argument in the top left
 * corner, and the second in the bottom right.
 *
 * All other entries of the resulting matrix which will be created in the
 * third argument, are zero.
 **/
void matrixBlock(
       const matrix aMat,    /**< [in]  the top left input matrix */
       const matrix bMat,    /**< [in]  the bottom right input matrix */
       matrix &block         /**< [out] the new block matrix */
                );

/**
 * Computes the characteristic polynomial from a quadratic (2x2) matrix
 * and returns true in case of success.
 *
 * The method will be successful whenever the input matrix is a (2x2) matrix.
 * In this case, the resulting polynomial will be a univariate polynomial in
 * the first ring variable of degree 2 and it will reside in the second
 * argument.
 * The method assumes that the matrix entries are all numbers, i.e., elements
 * from the ground field/ring.
 *
 * @return true iff the input matrix was (2x2)
 **/
bool charPoly(
       const matrix aMat,    /**< [in]  the input matrix */
       poly &charPoly        /**< [out] the characteristic polynomial */
             );

/**
 * Computes the square root of a non-negative real number and returns
 * it as a new number.
 *
 * The method assumes that the current ground field is the complex
 * numbers, and that the argument has imaginary part zero.
 * If the real part is negative, then false is returned, otherwise true.
 * The square root will be computed in the last argument by Heron's
 * iteration formula with the first argument as the starting value. The
 * iteration will stop as soon as two successive values (in the resulting
 * sequence) differ by no more than the given tolerance, which is assumed
 * to be a positive real number.
 *
 * @return the square root of the non-negative argument number
 **/
bool realSqrt(
       const number n,          /**< [in]  the input number */
       const number tolerance,  /**< [in]  accuracy of iteration */
       number &root             /**< [out] the root of n */
             );

/**
 * Computes the Hessenberg form of a given square matrix.
 *
 * The method assumes that all matrix entries are numbers coming from some
 * subfield of the reals..
 * Afterwards, the following conditions will hold:
 * 1) hessenbergMat = pMat * aMat * pMat^{-1},
 * 2) hessenbergMat is in Hessenberg form.
 * During the algorithm, pMat will be constructed as the product of self-
 * inverse matrices.
 * The algorithm relies on computing square roots of floating point numbers.
 * These will be combuted by Heron's iteration formula, with iteration
 * stopping when two successive approximations of the square root differ by
 * no more than the given tolerance, which is assumed to be a positive real
 * number.
 **/
void hessenberg(
       const matrix aMat,      /**< [in]  the square input matrix */
       matrix &pMat,           /**< [out] the transformation matrix */
       matrix &hessenbergMat,  /**< [out] the Hessenberg form of aMat */
       const number tolerance, /**< [in]  accuracy */
       const ring r
               );

/**
 * Returns all solutions of a quadratic polynomial relation with real
 * coefficients.
 *
 * The method assumes that the polynomial is univariate in the first
 * ring variable, and that the current ground field is the complex numbers.
 * The polynomial has degree <= 2. Thus, there may be
 * a) infinitely many zeros, when p == 0; then -1 is returned;
 * b) no zero, when p == constant <> 0; then 0 is returned;
 * c) one zero, when p is linear; then 1 is returned;
 * d) one double zero; then 2 is returned;
 * e) two distinct zeros; then 3 is returned;
 * In the case e), s1 and s2 will contain the two distinct solutions.
 * In cases c) and d) s1 will contain the single/double solution.
 *
 * @return the number of distinct zeros
 **/
int quadraticSolve(
       const poly p,           /**< [in]  the polynomial        */
       number &s1,             /**< [out] first zero, if any    */
       number &s2,             /**< [out] second zero, if any   */
       const number tolerance  /**< [in] accuracy               */
                  );


/**
 * Computes a factorization of a polynomial h(x, y) in K[[x]][y] up to a
 * certain degree in x, whenever a factorization of h(0, y) is given.
 *
 * The algorithm is based on Hensel's lemma: Let h(x, y) denote a monic
 * polynomial in y of degree m + n with coefficients in K[[x]]. Suppose there
 * are two monic factors f_0(y) (of degree n) and g_0(y) of degree (m) such
 * that h(0, y) = f_0(y) * g_0(y) and <f_0, g_0> = K[y]. Fix an integer d >= 0.
 * Then there are monic polynomials in y with coefficients in K[[x]], namely
 * f(x, y) of degree n and g(x, y) of degree m such that
 *    h(x, y) = f(x, y) * g(x, y) modulo <x^(d+1)>   (*).
 *
 * This implementation expects h, f0, g0, and d as described and computes the
 * factors f and g. Effectively, h will be given as an element of K[x, y] since
 * all terms of h with x-degree larger than d can be ignored due to (*).
 * The method expects the ground ring to contain at least two variables; then
 * x is the first ring variable, specified by the input index xIndex, and y the
 * second one, specified by yIndex.
 *
 * This code was placed here since the algorithm works by successively solving
 * d linear equation systems. It is hence an application of other methods
 * defined in this h-file and its corresponding cc-file.
 *
 **/
void henselFactors(
       const int xIndex,  /**< [in]  index of x in {1, ..., nvars(basering)} */
       const int yIndex,  /**< [in]  index of y in {1, ..., nvars(basering)} */
       const poly h,      /**< [in]  the polynomial h(x, y) as above         */
       const poly f0,     /**< [in]  the first univariate factor of h(0, y)  */
       const poly g0,     /**< [in]  the second univariate factor of h(0, y) */
       const int d,       /**< [in]  the degree bound, d >= 0                */
       poly &f,           /**< [out] the first factor of h(x, y)             */
       poly &g            /**< [out] the second factor of h(x, y)            */
                              );

/**
 * LU-decomposition of a given (m x n)-matrix with performing only those
 * divisions that yield zero remainders.
 *
 * Given an (m x n) matrix A, the method computes its LDU-decomposition,
 * that is, it computes matrices P, L, D, and U such that<br>
 * - P * A = L * D^(-1) * U,<br>
 * - P is an (m x m) permutation matrix, i.e., its row/columns form the
 *   standard basis of K^m,<br>
 * - L is an (m x m) matrix in lower triangular form of full rank,<br>
 * - D is an (m x m) diagonal matrix with full rank, and<br>
 * - U is an (m x n) matrix in upper row echelon form.<br>
 * From these conditions, it follows immediately that also
 * A = P * L * D^(-1) * U, since P is self-inverse.<br>
 *
 * In contrast to luDecomp, this method only performs those divisions which
 * yield zero remainders. Hence, when e.g. computing over a rational function
 * field and starting with polynomial entries only (or over Q and starting
 * with integer entries only), then any newly computed matrix entry will again
 * be polynomial (or integer).
 *
 * The method will modify all argument matrices except aMat, so that
 * they will finally contain the matrices P, L, D, and U as specified
 * above. Moreover, in order to further speed up subsequent calls of
 * luSolveViaLDUDecomp, the two denominators and their product will also be
 * returned.
 **/
void lduDecomp(
       const matrix aMat, /**< [in]  the initial matrix A,          */
       matrix &pMat,      /**< [out] the row permutation matrix P   */
       matrix &lMat,      /**< [out] the lower triangular matrix L  */
       matrix &dMat,      /**< [out] the diagonal matrix D          */
       matrix &uMat,      /**< [out] the upper row echelon matrix U */
       poly &l,           /**< [out] the product of pivots of L     */
       poly &u,           /**< [out] the product of pivots of U     */
       poly &lTimesU      /**< [out] the product of l and u         */
             );

/* helper for qrDoubleShift */
bool qrDS( const int n, matrix* queue, int& queueL, number* eigenValues,
       int& eigenValuesL, const number tol1, const number tol2, const ring R);

/**
 * Tries to find the number n in the array nn[0..nnLength-1].
 **/
int similar(
       const number* nn,       /**< [in] array of numbers to look-up */
       const int nnLength,     /**< [in] length of nn                */
       const number n,         /**< [in] number to loop-up in nn     */
       const number tolerance  /**< [in] tolerance for comparison    */
           );
#endif
/* LINEAR_ALGEBRA_H */
