#include <groebnerCone.h>
#include <startingCone.h>
#include <tropicalTraversal.h>

gfan::ZFan* tropicalVariety(const tropicalStrategy currentStrategy)
{
  ideal I = currentStrategy.getStartingIdeal();
  ring r = currentStrategy.getStartingRing();
  groebnerCone startingCone = tropicalStartingCone(I,r,currentStrategy);
  groebnerCones tropicalVariety = tropicalTraversal(startingCone);
  return toFanStar(tropicalVariety);
}
