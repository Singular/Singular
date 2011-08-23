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
 * @internal
 * @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef CF_NEWTON_POLYGON_H
#define CF_NEWTON_POLYGON_H

// #include "config.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

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

/// check if @a point is inside a polygon described by points
///
/// @return true if @a point is inside a polygon described by points
bool isInPolygon (int ** points, ///< [in] an array of points in the
                                 ///< plane describing a polygon
                  int sizePoints,///< [in] size of @a points
                  int* point     ///< [in] a point in the plane
                 );

#ifdef HAVE_NTL
/// Algorithm 5 as described in Convex-Dense Bivariate Polynomial Factorization
/// by Berthomieu, Lecerf
void convexDense (int** points,  ///< [in, out] a set of points in Z^2, returns
                                 ///< M (points)+A
                  int sizePoints,///< [in] size of points
                  mat_ZZ& M,     ///< [in,out] returns an invertible matrix
                  vec_ZZ& A      ///< [in,out] returns translation
                 );

/// compress a bivariate poly
///
/// @return @a compress returns a compressed bivariate poly
/// @sa convexDense, decompress
CanonicalForm
compress (const CanonicalForm& F, ///< [in] compressed, i.e. F.level()==2,
                                  ///< bivariate poly
          mat_ZZ& inverseM,       ///< [in,out] returns the inverse of M
          vec_ZZ& A               ///< [in,out] returns translation
         );

/// decompress a bivariate poly
///
/// @return @a decompress returns a decompressed bivariate poly
/// @sa convexDense, decompress
CanonicalForm
decompress (const CanonicalForm& F,///< [in] compressed, i.e. F.level()<= 2,
                                   ///< uni- or bivariate poly
            const mat_ZZ& M,       ///< [in] matrix M obtained from compress
            const vec_ZZ& A        ///< [in] vector A obtained from compress
           );
#endif

#endif

