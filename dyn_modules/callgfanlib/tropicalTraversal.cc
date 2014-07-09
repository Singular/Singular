#include <groebnerCone.h>

groebnerCones tropicalTraversal(const groebnerCone startingCone)
{
  groebnerCones tropicalVariety;
  groebnerCones workingList;
  workingList.insert(startingCone);
  while(!workingList.empty())
  {
    const groebnerCone sigma=*(workingList.begin());
    const groebnerCones neighbours = sigma.tropicalNeighbours();
    workingList.insert(neighbours.begin(),neighbours.end());
    tropicalVariety.insert(sigma);
    workingList.erase(sigma);
  }
  return tropicalVariety;
}
