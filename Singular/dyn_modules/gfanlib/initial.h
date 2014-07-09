#ifndef INITIAL_H
#define INITIAL_H

/***
 * various functions to compute the initial form of polynomials and ideals
 **/

#include <gfanlib/gfanlib_vector.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <Singular/ipid.h>

/***
 * Computes the weighted degree of the leading monomial of p with respect to w
 **/
long wDeg(const poly p, const ring r, const gfan::ZVector w);

/***
 * Returns the first terms of p of same weighted degree under w.
 * Coincides with the initial form of p with respect to w if and only if p was already
 * sorted with respect to w.
 **/
poly sloppyInitial(const poly p, const ring r, const gfan::ZVector w);

/***
 * Runs the above procedure over all generators of an ideal.
 * Coincides with the initial ideal of I with respect to w if and only if
 * the elements of I were already sorted with respect to w and
 * I is a standard basis form with respect to w.
 **/
ideal sloppyInitial(const ideal I, const ring r, const gfan::ZVector w);

poly initial(const poly p, const ring r, const gfan::ZVector w);
ideal initial(const ideal I, const ring r, const gfan::ZVector w);



poly initial(const poly p, const ring r);
ideal initial(const ideal I, const ring r);
BOOLEAN initial(leftv res, leftv args);
#ifndef NDEBUG
BOOLEAN initial0(leftv res, leftv args);
#endif

#endif
