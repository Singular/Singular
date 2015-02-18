#ifndef TROPICAL_TRAVERSAL_H
#define TROPICAL_TRAVERSAL_H

#include <groebnerCone.h>

groebnerCones tropicalTraversalMinimizingFlips(const groebnerCone startingCone);
groebnerCones tropicalTraversal(const groebnerCone startingCone);
groebnerCones groebnerTraversal(const groebnerCone startingCone);

#endif
