#ifndef GFANLIB_GROEBNERFAN_H
#define GFANLIB_GROEBNERFAN_H

#include <gfanlib/gfanlib_zfan.h>

gfan::ZFan* groebnerFanOfPolynomial(poly g, ring r, bool onlyLowerHalfSpace=false);
BOOLEAN groebnerFan(leftv res, leftv args);

#endif
