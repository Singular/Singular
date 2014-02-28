#ifndef WITNESS_H
#define WITNESS_H

#include <utility>
#include <kernel/polys.h>
#include <Singular/ipid.h>
#include <gfanlib/gfanlib_vector.h>

poly witness(const poly m, const ideal I, const ideal inI, const ring r);
std::pair<ideal,ring> flip(const ideal I, const ring r, const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal);

#ifndef NDEBUG
BOOLEAN dwr0(leftv res, leftv args);
BOOLEAN witness0(leftv res, leftv args);
#endif

#endif
