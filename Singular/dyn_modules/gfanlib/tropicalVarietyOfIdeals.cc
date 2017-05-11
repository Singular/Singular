#include "groebnerCone.h"
#include "startingCone.h"
#include "tropicalTraversal.h"

gfan::ZFan* tropicalVariety(const tropicalStrategy currentStrategy)
{
  tropicalStartingPoints = gfan::ZMatrix(0,rVar(currentStrategy.getStartingRing()));
  groebnerCone startingCone = tropicalStartingCone(currentStrategy);
  groebnerCones tropicalVariety = tropicalTraversalMinimizingFlips(startingCone);
  return toFanStar(tropicalVariety);
}
