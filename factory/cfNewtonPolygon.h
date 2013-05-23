/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfNewtonPolygon.h
 *
 * This file provides functions to compute the Newton polygon of a bivariate
 * polynomial
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef CF_NEWTON_POLYGON_H
#define CF_NEWTON_POLYGON_H

// #include "config.h"

/// compute a polygon
///
/// @return an integer n such that the first n entries of @a points are the
///         vertices of the convex hull of @a points
int polygon (int** points, ///< [in,out] an array of points in the plane
             int sizePoints///< [in] number of elements in @a points
            );

/// compute the Newton polygon of a bivariate polynomial
///
/// @return an array of points in the plane which are the vertices of the Newton
///         polygon of F
int ** newtonPolygon (const CanonicalForm& F,///< [in] a bivariate polynomial
                      int& sizeOfNewtonPoly  ///< [in, out] size of the result
                     );

/// compute the convex hull of the support of two bivariate polynomials
///
/// @return an array of points in the plane which are the vertices of the convex
///         hull of the support of F and G
int ** newtonPolygon (const CanonicalForm& F,///< [in] a bivariate polynomial
                      const CanonicalForm& G,///< [in] a bivariate polynomial
                      int& sizeOfNewtonPoly  ///< [in, out] size of the result
                     );

/// check if @a point is inside a polygon described by points
///
/// @return true if @a point is inside a polygon described by points
bool isInPolygon (int ** points, ///< [in] an array of points in the
                                 ///< plane describing a polygon
                  int sizePoints,///< [in] size of @a points
                  int* point     ///< [in] a point in the plane
                 );

/// get the y-direction slopes of all edges with positive slope in y-direction
/// of a convex polygon with at least one point of the polygon lying on the
/// x-axis and one lying on the y-axis
///
/// @return an array containing the slopes as described above
int* getRightSide (int** polygon,     ///<[in] vertices of a polygon
                   int sizeOfPolygon, ///<[in] number of vertices
                   int& sizeOfOutput  ///<[in,out] size of the output
                  );

/// computes the Newton polygon of F and checks if it satisfies the
/// irreducibility criterion from S.Gao "Absolute irreducibility of polynomials
/// via polytopes", Example 1
///
/// @return true if it satisfies the above criterion, false otherwise
bool
irreducibilityTest (const CanonicalForm& F ///<[in] a bivariate polynomial
                   );

/// absolute irreducibility test as described in "Modular Las Vegas Algorithms
/// for Polynomial Absolute Factorization" by C. Bertone, G. Cheze, A. Galligo
///
/// @return true if F satisfies condition (C) from the above paper and thus
/// is absolutely irreducible, false otherwise
bool
absIrredTest (const CanonicalForm& F ///< [in] a bivariate polynomial
                                     ///< irreducible over ground field
             );

/// modular absolute irreducibility test as described in "Modular Las Vegas
/// Algorithms for Polynomial Absolute Factorization" by C. Bertone, G. Cheze,
/// A. Galligo
///
/// @return true if F is absolutely irreducible, false otherwise
bool
modularIrredTest (const CanonicalForm& F ///< [in] a bivariate polynomial
                                         ///< irreducible over Z
                 );

/// modular absolute irreducibility test with shift as described in "Modular Las
/// Vegas Algorithms for Polynomial Absolute Factorization" by C. Bertone,
/// G. Cheze, A. Galligo
///
/// @return true if F is absolutely irreducible, false otherwise
bool
modularIrredTestWithShift (const CanonicalForm& F ///< [in] a bivariate polynomial
                                                  ///< irreducible over Z
                          );

/// Algorithm 5 as described in Convex-Dense Bivariate Polynomial Factorization
/// by Berthomieu, Lecerf
void convexDense (int** points,  ///< [in, out] a set of points in Z^2, returns
                                 ///< M (points)+A
                  int sizePoints,///< [in] size of points
                  mpz_t*& M,     ///< [in,out] returns an invertible 2x2 matrix
                  mpz_t*& A      ///< [in,out] returns translation
                 );

/// compress a bivariate poly
///
/// @return @a compress returns a compressed bivariate poly
/// @sa convexDense, decompress
CanonicalForm
compress (const CanonicalForm& F, ///< [in] compressed, i.e. F.level()==2,
                                  ///< bivariate poly
          mpz_t*& inverseM,       ///< [in,out] returns the inverse of M,
                                  ///< if computeMA==true, M otherwise
          mpz_t*& A,              ///< [in,out] returns translation
          bool computeMA= true    ///< [in] whether to compute M and A
         );

/// decompress a bivariate poly
///
/// @return @a decompress returns a decompressed bivariate poly
/// @sa convexDense, decompress
CanonicalForm
decompress (const CanonicalForm& F,///< [in] compressed, i.e. F.level()<= 2,
                                   ///< uni- or bivariate poly
            const mpz_t* M,       ///< [in] matrix M obtained from compress
            const mpz_t* A        ///< [in] vector A obtained from compress
           );

#endif

