#ifndef FLIP_H
#define FLIP_H

#include <utility>
#include <libpolys/polys/simpleideals.h>

#include <gfanlib/gfanlib_vector.h>
#include <tropicalStrategy.h>

std::pair<ideal,ring> flip(const ideal I, const ring r, const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal, const tropicalStrategy& currentCase);

#endif
