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
    for (groebnerCones::iterator tau = neighbours.begin(); tau!=neighbours.end(); tau++)
    {
      if (tropicalVariety.count(*tau)==0)
        workingList.insert(*tau);
    }
    tropicalVariety.insert(sigma);
    workingList.erase(sigma);
    std::cout << "tropicalVariety.size():" << tropicalVariety.size() << std::endl;
    std::cout << "workingList.size():" << workingList.size() << std::endl;
  }
  return tropicalVariety;
}
