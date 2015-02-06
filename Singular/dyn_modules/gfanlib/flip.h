#ifndef GFANLIB_FLIP_H
#define GFANLIB_FLIP_H

#include <utility>
#include <polys/simpleideals.h>
#include <gfanlib/gfanlib_vector.h>

std::pair<ideal,ring> flip(const ideal I, const ring r,
                           const gfan::ZVector interiorPoint,
                           const gfan::ZVector facetNormal,
                           const gfan::ZVector adjustedInteriorPoint,
                           const gfan::ZVector adjustedFacetNormal);

#endif
