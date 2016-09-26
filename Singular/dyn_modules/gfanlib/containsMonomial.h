#ifndef CONTAINS_MONOMIAL_H
#define CONTAINS_MONOMIAL_H

#include <polys/simpleideals.h>
#include <Singular/subexpr.h>
#include <gfanlib/gfanlib_vector.h>

poly checkForMonomialViaSuddenSaturation(const ideal I, const ring r);
poly searchForMonomialViaStepwiseSaturation(const ideal I, const ring r, const gfan::ZVector w);
BOOLEAN checkForMonomial(leftv res, leftv args);
BOOLEAN searchForMonomialViaStepwiseSaturation(leftv res, leftv args);

#endif
