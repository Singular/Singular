#ifndef KERNEL_GBENGINE_QRING_POWER_H
#define KERNEL_GBENGINE_QRING_POWER_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: polynomial powers reduced modulo a standard basis
*/

#include "polys/monomials/ring.h"

/// Return the normal form of a^e modulo the standard basis G.
/// Intermediate products are reduced to avoid unnecessary growth.
/// This destroys a and currently supports commutative rings only.
poly p_PowerMod(poly a, int e, ideal G, const ring r);

#endif
