#ifndef STARTING_CONE_H
#define STARTING_CONE_H

#include <groebnerCone.h>
#include <tropicalStrategy.h>

groebnerCone tropicalStartingCone(const tropicalStrategy& currentCase);

BOOLEAN tropicalStartingPoint(leftv res, leftv args);
BOOLEAN positiveTropicalStartingPoint(leftv res, leftv args);
BOOLEAN nonNegativeTropicalStartingPoint(leftv res, leftv args);
BOOLEAN negativeTropicalStartingPoint(leftv res, leftv args);
BOOLEAN nonPositiveTropicalStartingPoint(leftv res, leftv args);
BOOLEAN tropicalStartingCone(leftv res, leftv args);

#endif
