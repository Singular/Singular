#ifndef TROPICAL_CURVE_H
#define TROPICAL_CURVE_H

#include <gfanlib/gfanlib_zcone.h>
#include <gfanlib/gfanlib_zfan.h>
#include <polys/monomials/p_polys.h>
#include <tropicalStrategy.h>
#include <tropicalVarietyOfPolynomials.h>

ZConesSortedByDimension tropicalStar(const ideal I, const ring r, const gfan::ZVector &u,
                                     const tropicalStrategy* currentStrategy);
gfan::ZMatrix raysOfTropicalStar(ideal I, const ring r, const gfan::ZVector &u,
                                 const tropicalStrategy* currentStrategy);

#ifndef NDEBUG
BOOLEAN tropicalStarDebug(leftv res, leftv args);
#endif

#endif
