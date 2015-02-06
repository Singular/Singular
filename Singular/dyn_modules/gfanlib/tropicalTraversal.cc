#include <bbcone.h>
#include <groebnerCone.h>
#include <tropicalCurves.h>

std::vector<bool> checkNecessaryFlips(const groebnerCones &tropicalVariety, const groebnerCones &workingList,
                                      const gfan::ZVector &interiorPoint, const gfan::ZMatrix &normalVectors)
{
  int k = normalVectors.getHeight();
  std::vector<bool> needToFlip(k,true);

  int n = normalVectors.getWidth();
  gfan::ZMatrix testVectors(k,n);
  gfan::ZVector bigInteriorPoint = 1000*interiorPoint;
  for (int i=0; i<k; i++)
    testVectors[i] = bigInteriorPoint+normalVectors[i];

  for (groebnerCones::iterator sigma = tropicalVariety.begin(); sigma!=tropicalVariety.end(); sigma++)
  {
    if (sigma->contains(interiorPoint))
    {
      for (int i=0; i<k; i++)
      {
        if (needToFlip[i] && sigma->contains(testVectors[i]))
          needToFlip[i] = false;
      }
    }
  }

  for (groebnerCones::iterator sigma = workingList.begin(); sigma!=workingList.end(); sigma++)
  {
    if (sigma->contains(interiorPoint))
    {
      for (int i=0; i<k; i++)
      {
        if (needToFlip[i] && sigma->contains(testVectors[i]))
          needToFlip[i] = false;
      }
    }
  }

  return needToFlip;
}

groebnerCones tropicalTraversalMinimizingFlips(const groebnerCone startingCone)
{
  groebnerCones tropicalVariety;
  groebnerCones workingList;
  workingList.insert(startingCone);
  const tropicalStrategy* currentStrategy=startingCone.getTropicalStrategy();
  std::set<gfan::ZVector> finishedInteriorPoints;
  while(!workingList.empty())
  {
    /**
     * Pick an element the working list and compute interior points on its facets
     */
    groebnerCone sigma=*(workingList.begin());
    gfan::ZMatrix interiorPoints = interiorPointsOfFacets(sigma.getPolyhedralCone(),finishedInteriorPoints);

    for (int i=0; i<interiorPoints.getHeight(); i++)
    {
      /**
       * For each interior point, compute the rays of the tropical star in that point
       */
      gfan::ZVector interiorPoint = interiorPoints[i];
      if (!(currentStrategy->restrictToLowerHalfSpace() && interiorPoint[0].sign()==0))
      {
        ideal inI = initial(sigma.getPolynomialIdeal(),sigma.getPolynomialRing(),interiorPoint);
        gfan::ZMatrix normalVectors = raysOfTropicalStar(inI,
                                                         sigma.getPolynomialRing(),
                                                         interiorPoint,
                                                         sigma.getTropicalStrategy());
        id_Delete(&inI,sigma.getPolynomialRing());

        std::vector<bool> needToFlip = checkNecessaryFlips(tropicalVariety,workingList,interiorPoint,normalVectors);
        for (int j=0; j<normalVectors.getHeight(); j++)
        {
          if (needToFlip[j])
          {
            groebnerCone neighbour = sigma.flipCone(interiorPoint,normalVectors[j]);
            workingList.insert(neighbour);
          }
        }
      }
      finishedInteriorPoints.insert(interiorPoint);
    }

    sigma.deletePolynomialData();
    workingList.erase(sigma);
    tropicalVariety.insert(sigma);
    // std::cout << "tropicalVariety.size():" << tropicalVariety.size() << std::endl;
    // std::cout << "workingList.size():" << workingList.size() << std::endl;
  }
  return tropicalVariety;
}

groebnerCones tropicalTraversal(const groebnerCone startingCone)
{
  groebnerCones tropicalVariety;
  groebnerCones workingList;
  workingList.insert(startingCone);
  // std::cout << "starting traversal" << std::endl;
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
    // std::cout << "tropicalVariety.size():" << tropicalVariety.size() << std::endl;
    // std::cout << "workingList.size():" << workingList.size() << std::endl;
  }
  return tropicalVariety;
}
