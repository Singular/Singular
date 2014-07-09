#ifndef TROPICAL_CURVE_H
#define TROPICAL_CURVE_H

#include <gfanlib/gfanlib_zcone.h>
#include <gfanlib/gfanlib_zfan.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <tropicalStrategy.h>

std::set<gfan::ZCone> tropicalCurve(const ideal I, const ring r, const tropicalStrategy& currentStrategy);
std::set<gfan::ZVector> raysOfTropicalCurve(ideal I, const ring r, const tropicalStrategy& currentStrategy);

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args);
BOOLEAN tropicalCurve1(leftv res, leftv args);
#endif

#endif
