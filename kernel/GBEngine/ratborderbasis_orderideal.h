#ifndef RATBORDERBASIS_ORDERIDEAL_H
#define RATBORDERBASIS_ORDERIDEAL_H

#include "kernel/structs.h"

#ifdef HAVE_RATGRING
// Compute a rational-Weyl border basis for a caller-supplied differential
// order ideal.  Unlike nc_RationalWeylBorderBasis, this routine does not
// compute a Groebner basis or derive the order ideal from leading terms.
// Instead it uses fraction-free Macaulay prolongations over K[x].
//
// I and orderIdeal are represented in K[x]<D>.  The first
// coefficientVariables variables are the x variables and the remaining
// variables are the differential operators.  maxProlongation bounds the
// differential degree of the left multiples of the generators used in the
// Macaulay spaces.
//
// Returns FALSE on success and TRUE on error.
BOOLEAN nc_RationalWeylBorderBasisForOrderIdeal(
    const ideal I, const ideal orderIdeal, int coefficientVariables,
    int maxProlongation, ideal &borderBasis, const ring r);
#endif

#endif
