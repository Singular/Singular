#ifndef STARTING_CONE_H
#define STARTING_CONE_H

#include <groebnerCone.h>
#include <tropicalStrategy.h>

groebnerCone tropicalStartingCone(const ideal I, const ring r, const tropicalStrategy& currentCase);

BOOLEAN tropicalStartingPoint(leftv res, leftv args);
BOOLEAN tropicalStartingCone(leftv res, leftv args);

#endif
