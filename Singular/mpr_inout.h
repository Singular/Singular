#ifndef MPR_H
#define MPR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_inout.h,v 1.2 1999-06-28 16:06:28 Singular Exp $ */

/*
* ABSTRACT - multipolynomial resultants - interface to Singular
*
*/

#define DEFAULT_DIGITS 30

#define MPR_DENSE  1
#define MPR_SPARSE 2

/** solve a multipolynomial system using the u-resultant
 * Input ideal must be 0-dimensional and pVariables == IDELEMS(ideal).
 * Resultant method can be MPR_DENSE, which uses Macaulay Resultant (good for
 * dense homogeneous polynoms) or MPR_SPARSE, which uses Sparse Resultant
 * (Gelfand, Kapranov, Zelevinsky).
 * If interpolate == true then the determinant of the u-resultant will be
 * numerically interpolatet using a Vandermonde System.
 * Otherwise, the Sparse Bareiss will be used (faster!).
 * Returns a list containing the roots of the system.
 */
BOOLEAN nuUResSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 );

/** build resultant matrix from ideal
 * Make sure that IDELEMS(ideal) == pVariables+1.
 */
BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 );

/** find the (complex) roots an univariate polynomial
 * Determines the roots of an univariate polynomial using Laguerres'
 * root-solver. Good for polynomials with low and middle degree (<40).
 * Returns a list containing the roots of the polynomial.
 */
BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2 );

/**
 */
BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2, leftv arg3 );

#endif

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
