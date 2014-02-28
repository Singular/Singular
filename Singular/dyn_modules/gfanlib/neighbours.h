#ifndef CALLGFANLIB_NEIGHBOURS_H
#define CALLGFANLIB_NEIGHBOURS_H

#include <groebnerCone.h>
#include <tropicalStrategy.h>

setOfGroebnerConeData groebnerNeighbours(const groebnerConeData sigma, const tropicalStrategy currentCase);
BOOLEAN tropicalNeighbours(leftv res, leftv args);

#endif
