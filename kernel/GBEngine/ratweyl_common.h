#ifndef RATWEYL_COMMON_H
#define RATWEYL_COMMON_H

#include "kernel/structs.h"

#ifdef HAVE_RATWEYL_COMMON
// Return (u,v) with u*p+v*q=0 for coefficient polynomials p and q.
// The input polynomials are consumed.  This fraction-free coefficient
// operation is shared by the historical ratGB engine and rational Weyl
// order-ideal border bases.
ideal ncGCD(poly p, poly q, ring r);
#endif

#endif
