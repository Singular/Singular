#ifndef CALLGFANLIB_NEIGHBOURS_H
#define CALLGFANLIB_NEIGHBOURS_H

#include <groebnerCone.h>
#include <tropicalStrategy.h>

gfan::ZCone sloppyGroebnerCone(const ideal I, const ring r, const gfan::ZVector w);
groebnerCones groebnerNeighbours(const groebnerCone sigma, const tropicalStrategy currentCase);
groebnerCones tropicalNeighbours(const groebnerCone sigma, const tropicalStrategy currentCase);
BOOLEAN tropicalNeighbours(leftv res, leftv args);

#endif
