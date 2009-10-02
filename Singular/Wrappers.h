#ifndef WRAPPERS_H
#define WRAPPERS_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include "polys.h"
#include "PrettyPrinter.h"

void testWrappers (bool detailedOutput);

typedef poly SingularPoly;
typedef ring SingularRing;

extern PrettyPrinter prpr;   // for pretty printing

// some enums used for test prior
// to static casts; see code
#define UNSPECIFIED_POLY_TYPE 0
#define CANONICAL_POLY_TYPE 1

#endif // HAVE_WRAPPERS

#endif
/* WRAPPERS_H */
