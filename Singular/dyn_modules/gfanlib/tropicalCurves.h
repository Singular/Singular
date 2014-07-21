#ifndef TROPICAL_CURVE_H
#define TROPICAL_CURVE_H

#include <gfanlib/gfanlib_zcone.h>
#include <gfanlib/gfanlib_zfan.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <tropicalStrategy.h>

std::set<gfan::ZCone> tropicalStar(const ideal I, const ring r, const gfan::ZVector u,
                                   const tropicalStrategy& currentStrategy);
std::set<gfan::ZVector> raysOfTropicalStar(ideal I, const ring r, const gfan::ZVector u,
                                           const tropicalStrategy& currentStrategy);

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args);
BOOLEAN tropicalCurve1(leftv res, leftv args);
#endif

#endif
