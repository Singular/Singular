#ifndef INITIAL_H
#define INITIAL_H

#include <gfanlib/gfanlib_vector.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <Singular/ipid.h>

long wDeg(const poly p, const ring r, const gfan::ZVector w);
poly initial(const poly p, const ring r, const gfan::ZVector w);
ideal initial(const ideal I, const ring r, const gfan::ZVector w);

poly initial(const poly p, const ring r);
ideal initial(const ideal I, const ring r);
BOOLEAN initial(leftv res, leftv args);
#ifndef NDEBUG
BOOLEAN initial0(leftv res, leftv args);
#endif

#endif
