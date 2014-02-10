#ifndef TROPICAL_CURVE_H
#define TROPICAL_CURVE_H

#include <gfanlib/gfanlib_zcone.h>
#include <gfanlib/gfanlib_zfan.h>
#include <libpolys/polys/monomials/p_polys.h>

gfan::ZFan tropicalCurve(const ideal I, const gfan::ZVector w, const ring r);

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args);
BOOLEAN tropicalCurve1(leftv res, leftv args);
BOOLEAN tropicalCurve2(leftv res, leftv args);
#endif

#endif
