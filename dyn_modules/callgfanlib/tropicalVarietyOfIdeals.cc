#include <startingCone.h>
#include <neighbours.h>

setOfGroebnerConeData fullTropicalVariety(const ideal I, const ring r)
{
  groebnerConeData currentCone = groebnerConeData(I,r,tropicalStartingCone(I,r,nonValuedCase));
  setOfGroebnerConeData tropicalVariety;
  setOfGroebnerConeData workingList;
  workingList.insert(currentCone);
  while (!workingList.empty())
  {
    currentCone = *workingList.begin();
    tropicalVariety.insert(currentCone);
    workingList.erase(workingList.begin());

    setOfGroebnerConeData neighbours = tropicalNeighbours(currentCone,nonValuedCase);
    for (setOfGroebnerConeData::iterator it=neighbours.begin(); it!=neighbours.end(); it++)
      if (!tropicalVariety.count(*it)) workingList.insert(*it);
  }
  return tropicalVariety;
}

setOfGroebnerConeData lowerTropicalVariety(const ideal I, const ring r)
{
  groebnerConeData currentCone = groebnerConeData(I,r,tropicalStartingCone(I,r,valuedCase));
  setOfGroebnerConeData tropicalVariety;
  setOfGroebnerConeData workingList;
  workingList.insert(currentCone);
  while (!workingList.empty())
  {
    currentCone = *workingList.begin();
    tropicalVariety.insert(currentCone);
    workingList.erase(workingList.begin());

    setOfGroebnerConeData neighbours = tropicalNeighbours(currentCone,valuedCase);
    for (setOfGroebnerConeData::iterator it=neighbours.begin(); it!=neighbours.end(); it++)
      if (!tropicalVariety.count(*it)) workingList.insert(*it);
  }
  return tropicalVariety;
}
