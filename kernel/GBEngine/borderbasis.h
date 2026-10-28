#ifndef BORDERBASIS_H
#define BORDERBASIS_H

#include "kernel/structs.h"

// Compute the border basis belonging to the standard monomials of G.
// G must be a standard basis in a commutative polynomial ring.
// Returns FALSE on success and TRUE on error.
BOOLEAN kBorderBasisFromStandardBasis(const ideal G, ideal &orderIdeal,
                                      ideal &borderBasis, const ring r);

#endif
