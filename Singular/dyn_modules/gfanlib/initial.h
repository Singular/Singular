#ifndef INITIAL_H
#define INITIAL_H

/**
 * various functions to compute the initial form of polynomials and ideals
 */
#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_matrix.h>
#include <libpolys/polys/monomials/p_polys.h>

/**
 * Returns the weighted degree of the leading term of p with respect to w
 */
long wDeg(const poly p, const ring r, const gfan::ZVector w);

/**
 * Returns the weighted multidegree of the leading term of p with respect to (w,W).
 * The weighted multidegree is a vector of length one higher than the column vectors of W.
 * The first entry is the weighted degree with respect to w
 * and the i+1st entry is the weighted degree with respect to the i-th row vector of W.
 */
gfan::ZVector WDeg(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W);

/**
 * Returns the initial form of p with respect to w
 */
poly initial(const poly p, const ring r, const gfan::ZVector w);

/**
 * Returns the initial form of I with respect to w
 */
ideal initial(const ideal I, const ring r, const gfan::ZVector w);

/**
 * Returns the initial form of p with respect to w and W
 */
poly initial(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W);

/**
 * Returns the initial form of I with respect to w and W
 */
ideal initial(const ideal I, const ring r, const gfan::ZVector w, const gfan::ZMatrix W);

/**
 * Stores the initial form of *pStar with respect to w in pStar
 */
void initial(poly* pStar, const ring r, const gfan::ZVector w);

/**
 * Stores the initial form of *IStar with respect to w in IStar
 */
void initial(ideal* IStar, const ring r, const gfan::ZVector w);

/**
 * Stores the initial form of *pStar with respect to w and W in pStar
 */
void initial(poly* pStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W);

/**
 * Stores the initial form of *IStar with respect to w and W in IStar
 */
void initial(ideal* IStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W);

/* throwaway code */
/* #ifndef NDEBUG */
/* BOOLEAN initial0(leftv res, leftv args); */
/* #endif */
/* /\*** */
/*  * Returns the first terms of p of same weighted degree under w. */
/*  * Coincides with the initial form of p with respect to w if and only if p was already */
/*  * sorted with respect to w. */
/*  **\/ */
/* poly sloppyInitial(const poly p, const ring r, const gfan::ZVector w); */

/* /\*** */
/*  * Runs the above procedure over all generators of an ideal. */
/*  * Coincides with the initial ideal of I with respect to w if and only if */
/*  * the elements of I were already sorted with respect to w and */
/*  * I is a standard basis form with respect to w. */
/*  **\/ */
/* ideal sloppyInitial(const ideal I, const ring r, const gfan::ZVector w); */
/* poly initial(const poly p, const ring r, const gfan::ZMatrix W); */
/* ideal initial(const ideal I, const ring r, const gfan::ZMatrix W); */
/* poly initial(const poly p, const ring r); */
/* ideal initial(const ideal I, const ring r); */
/* BOOLEAN initial(leftv res, leftv args); */

#endif
