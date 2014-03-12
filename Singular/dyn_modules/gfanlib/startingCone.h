#ifndef STARTING_CONE_H
#define STARTING_CONE_H

#include <groebnerCone.h>
#include <tropicalStrategy.h>

gfan::ZVector containsTropicalPoint(const groebnerConeData sigma);
gfan::ZVector tropicalStartingPoint(ideal I, ring r);
gfan::ZCone tropicalStartingCone(const ideal I, const ring r, const tropicalStrategy currentCase);

BOOLEAN tropicalStartingPoint0(leftv res, leftv args);
BOOLEAN tropicalStartingPoint1(leftv res, leftv args);

BOOLEAN tropicalStartingCone0(leftv res, leftv args);
BOOLEAN tropicalStartingCone1(leftv res, leftv args);

#endif
