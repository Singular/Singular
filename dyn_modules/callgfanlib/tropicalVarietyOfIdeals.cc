#include <groebnerCone.h>
#include <startingCone.h>
#include <tropicalTraversal.h>

gfan::ZFan* tropicalVariety(const ideal I, const ring r, const tropicalStrategy currentStrategy)
{
  groebnerCone startingCone = tropicalStartingCone(I,r,currentStrategy);
  groebnerCones tropicalVariety = tropicalTraversal(startingCone);
  return toFanStar(tropicalVariety);
}
