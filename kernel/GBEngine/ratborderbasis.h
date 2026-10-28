#ifndef RATBORDERBASIS_H
#define RATBORDERBASIS_H

#include "kernel/structs.h"

#ifdef HAVE_RATGRING
// Compute the border basis of a finite-rank left ideal in a rational Weyl
// algebra.  The first coefficientVariables variables form K[x], and the
// remaining variables are the differential operators.  G must be a rational
// Groebner basis represented fraction-free in the polynomial Weyl algebra.
//
// Each returned border polynomial has the form
//   a(x) * b - sum_t c_t(x) * t,
// and represents the monic rational relation obtained after division by the
// nonzero polynomial a(x).
//
// Returns FALSE on success and TRUE on error.
BOOLEAN nc_RationalWeylBorderBasis(const ideal G, int coefficientVariables,
                                   ideal &orderIdeal, ideal &borderBasis,
                                   const ring r);
#endif

#endif
