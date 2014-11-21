#include <groebnerCone.h>
#include <startingCone.h>
#include <tropicalTraversal.h>

gfan::ZFan* tropicalVariety(const tropicalStrategy currentStrategy)
{
  groebnerCone startingCone = tropicalStartingCone(currentStrategy);
  groebnerCones tropicalVariety = tropicalTraversal(startingCone);
  return toFanStar(tropicalVariety);
}
