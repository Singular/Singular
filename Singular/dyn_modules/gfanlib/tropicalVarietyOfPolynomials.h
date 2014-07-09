#ifndef GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H
#define GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H

#include <gfanlib/gfanlib.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <set>

#include <tropicalStrategy.h>

#ifndef NDEBUG
#include <Singular/subexpr.h> // for leftv
#include <bbfan.h>            // for fanID
#endif

std::set<gfan::ZCone> tropicalVariety(const poly g, const ring r, const tropicalStrategy &currentCase);

#ifndef NDEBUG
BOOLEAN tropicalVariety00(leftv res, leftv args);
BOOLEAN tropicalVariety01(leftv res, leftv args);
#endif

#endif
