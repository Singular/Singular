#ifndef MPR_H
#define MPR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id: mpr_inout.h,v 1.1.1.1 2003-10-06 12:15:58 Singular Exp $ */

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
 * Arguments 4: ideal i, int k, int l, int m
 *   k=0: use sparse resultant matrix of Gelfand, Kapranov and Zelevinsky
 *   k=1: use resultant matrix of Macaulay (k=0 is default)
 *   l>0: defines precision of fractional part if ground field is Q
 *   m=0,1,2: number of iterations for approximation of roots (default=2)
 * Returns a list containing the roots of the system.
 */
BOOLEAN nuUResSolve( leftv res, leftv args );

/** returns module representing the multipolynomial resultant matrix
 * Arguments 2: ideal i, int k
 *   k=0: use sparse resultant matrix of Gelfand, Kapranov and Zelevinsky
 *   k=1: use resultant matrix of Macaulay (k=0 is default)
 */
BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 );

/** find the (complex) roots an univariate polynomial
 * Determines the roots of an univariate polynomial using Laguerres'
 * root-solver. Good for polynomials with low and middle degree (<40).
 * Arguments 3: poly arg1 , int arg2 , int arg3
 *  arg2>0: defines precision of fractional part if ground field is Q
 *  arg3: number of iterations for approximation of roots (default=2)
 * Returns a list of all (complex) roots of the polynomial arg1
 */
BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 );

/**
 * COMPUTE: polynomial p with values given by v at points p1,..,pN derived
 * from p; more precisely: consider p as point in K^n and v as N elements in K,
 * let p1,..,pN be the points in K^n obtained by evaluating all monomials
 * of degree 0,1,...,N at p in lexicographical order, then the procedure
 * computes the polynomial f satisfying f(pi) = v[i]
 * RETURN:  polynomial f of degree d
 */
BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2, leftv arg3 );

/** compute Newton Polytopes of input polynomials
 */
BOOLEAN loNewtonP( leftv res, leftv arg1 );

/** Implementation of the Simplex Algorithm.
 * For args, see class simplex.
 */
BOOLEAN loSimplex( leftv res, leftv args );

#endif

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
