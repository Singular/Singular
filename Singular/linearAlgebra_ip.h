#ifndef LINEARALGEBRA_H
#define LINEARALGEBRA_H
#include "Singular/lists.h"
#include "kernel/linear_algebra/linearAlgebra.h"

/**
 * Computes all eigenvalues of a given real quadratic matrix with
 * multiplicities.
 *
 * The method assumes that the current ground field is the complex numbers.
 * Computations are based on the QR double shift algorithm involving
 * Hessenberg form and householder transformations.
 * If the algorithm works, then it returns a list with two entries which
 * are again lists of the same size:
 * _[1][i] is the i-th mutually distinct eigenvalue that was found,
 * _[2][i] is the (int) multiplicity of _[1][i].
 * If the algorithm does not work (due to an ill-posed matrix), a list with
 * the single entry (int)0 is returned.
 * 'tol1' is used for detection of deflation in the actual qr double shift
 * algorithm.
 * 'tol2' is used for ending Heron's iteration whenever square roots
 * are being computed.
 * 'tol3' is used to distinguish between distinct eigenvalues: When
 * the Euclidean distance between two computed eigenvalues is less then
 * tol3, then they will be regarded equal, resulting in a higher
 * multiplicity of the corresponding eigenvalue.
 *
 * @return a list with one entry (int)0, or two entries which are again lists
 **/
lists qrDoubleShift(
       const matrix A,     /**< [in]  the quadratic matrix         */
       const number tol1,  /**< [in]  tolerance for deflation      */
       const number tol2,  /**< [in]  tolerance for square roots   */
       const number tol3,   /**< [in]  tolerance for distinguishing
                                      eigenvalues                  */
       const ring r= currRing
                   );
#endif
