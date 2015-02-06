#ifndef WITNESS_H
#define WITNESS_H

#include <polys/monomials/monomials.h>
#include <polys/simpleideals.h>

/**
 * Computes a division discarding remainder of f with respect to G.
 * Given f a polynomial and G={g1,...,gk} a set of polynomials in r,
 * returns a matrix Q=(q1,...,qk) over r such that
 *   f = q1*g1+...+qk*gk+s
 * is a determinate division with remainder s.
 */
matrix divisionDiscardingRemainder(const poly f, const ideal G, const ring r);

/**
 * Computes a division discarding remainder of F with respect to G.
 * Given F={f1,...,fl} and G={g1,...,gk} two sets of polynomials in r,
 * returns a matrix Q=(qij) i=1,..,k j=1,...,l over r such that
 *   fj = q1j*g1+...+qkj*gk+sj
 * is a determinate division with remainder sj for all j=1,...,l.
 */
matrix divisionDiscardingRemainder(const ideal F, const ideal G, const ring r);

/**
 * Let w be the uppermost weight vector in the matrix defining the ordering on r.
 * Let I be a Groebner basis of an ideal in r, inI its initial form with respect w.
 * Given an w-homogeneous element m of inI, computes a witness g of m in I,
 * i.e. g in I such that in_w(g)=m.
 */
poly witness(const poly m, const ideal I, const ideal inI, const ring r);

/**
 * Computes witnesses in J for inI
 * Given inI={h1,...,hl} and J={g1,...,gk} two sets of polynomials in r,
 * returns a set I={f1,...,fl} of <g1,...,gk> such that
 *   in_w(fj)=hj for all j=1,...,l,
 * where w denotes the uppoermost weight vector in the matrix defining the ordering on r.
 * Assumes that hj is an element of <in_w(g1),...,in_w(gk)>
 */
ideal witness(const ideal inI, const ideal J, const ring r);

#ifndef NDEBUG
#include <Singular/ipid.h>
BOOLEAN dwrDebug(leftv res, leftv args);
BOOLEAN witnessDebug(leftv res, leftv args);
#endif

#endif
