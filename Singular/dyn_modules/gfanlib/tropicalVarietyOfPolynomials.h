#ifndef GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H
#define GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H

#include <gfanlib/gfanlib.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <set>

#ifndef NDEBUG
#include <Singular/subexpr.h> // for leftv
#include <bbfan.h>            // for fanID
#endif

std::set<gfan::ZCone> fullTropicalVarietyOfPolynomial(const poly &g, const ring &r);
std::set<gfan::ZCone> lowerTropicalVarietyOfPolynomial(const poly &g, const ring &r);

#ifndef NDEBUG
BOOLEAN tropicalVariety00(leftv res, leftv args);
BOOLEAN tropicalVariety01(leftv res, leftv args);
#endif

#endif
