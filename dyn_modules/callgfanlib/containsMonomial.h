#ifndef CONTAINS_MONOMIAL_H
#define CONTAINS_MONOMIAL_H

#include <libpolys/polys/simpleideals.h>
#include <gfanlib/gfanlib_vector.h>

poly containsMonomial(const ideal &I, const gfan::ZVector &w);
BOOLEAN containsMonomial(leftv res, leftv args);

#endif
