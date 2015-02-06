#ifndef CONTAINS_MONOMIAL_H
#define CONTAINS_MONOMIAL_H

#include <libpolys/polys/simpleideals.h>
#include <Singular/subexpr.h>
#include <gfanlib/gfanlib_vector.h>

poly checkForMonomialViaSuddenSaturation(const ideal I, const ring r);
BOOLEAN checkForMonomial(leftv res, leftv args);

#endif
