#include <bbcone.h>
#include <groebnerCone.h>
#include <tropicalCurves.h>

std::vector<bool> checkNecessaryTropicalFlips(const groebnerCones &tropicalVariety, const groebnerCones &workingList,
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

        std::vector<bool> needToFlip = checkNecessaryTropicalFlips(tropicalVariety,workingList,interiorPoint,normalVectors);
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
    if (printlevel > 0)
      Print("cones finished: %lu   cones in working list: %lu\n",
      (unsigned long)tropicalVariety.size(), (unsigned long)workingList.size());
  }
  return tropicalVariety;
}


std::vector<bool> checkNecessaryGroebnerFlips(const groebnerCones &groebnerFan, const groebnerCones &workingList,
                                              const gfan::ZMatrix &interiorPoints)
{
  int k = interiorPoints.getHeight();
  std::vector<bool> needToFlip(k,true);

  for (groebnerCones::iterator sigma = groebnerFan.begin(); sigma!=groebnerFan.end(); sigma++)
  {
    for (int i=0; i<k; i++)
    {
      if (needToFlip[i] && sigma->contains(interiorPoints[i]))
        needToFlip[i] = false;
    }
  }

  for (groebnerCones::iterator sigma = workingList.begin(); sigma!=workingList.end(); sigma++)
  {
    for (int i=0; i<k; i++)
    {
      if (needToFlip[i] && sigma->contains(interiorPoints[i]))
        needToFlip[i] = false;
    }
  }

  return needToFlip;
}


groebnerCones groebnerTraversal(const groebnerCone startingCone)
{
  const tropicalStrategy* currentStrategy = startingCone.getTropicalStrategy();

  groebnerCones groebnerFan;
  groebnerCones workingList;
  workingList.insert(startingCone);
  std::set<gfan::ZVector> finishedInteriorPoints;
  bool onlyLowerHalfSpace = !currentStrategy->isValuationTrivial();

  while(!workingList.empty())
  {
    /**
     * Pick a maximal Groebner cone from the working list
     * and compute interior points on its facets as well as outer facet normals
     */
    groebnerCone sigma=*(workingList.begin());
    workingList.erase(workingList.begin());

    std::pair<gfan::ZMatrix,gfan::ZMatrix> interiorPointsAndOuterFacetNormals = interiorPointsAndNormalsOfFacets(sigma.getPolyhedralCone(), finishedInteriorPoints, onlyLowerHalfSpace);
    gfan::ZMatrix interiorPoints = interiorPointsAndOuterFacetNormals.first;
    gfan::ZMatrix outerFacetNormals = interiorPointsAndOuterFacetNormals.second;
    std::vector<bool> needToFlip = checkNecessaryGroebnerFlips(groebnerFan,workingList, interiorPoints);

    for (int i=0; i<interiorPoints.getHeight(); i++)
    {
      gfan::ZVector interiorPoint = interiorPoints[i];

      if (needToFlip[i]==true)
      {
        groebnerCone neighbour = sigma.flipCone(interiorPoint,outerFacetNormals[i]);
        workingList.insert(neighbour);
      }
      finishedInteriorPoints.insert(interiorPoints[i]);
    }

    sigma.deletePolynomialData();
    groebnerFan.insert(sigma);
    if (printlevel > 0)
      Print("cones finished: %lu   cones in working list: %lu\n",
      (unsigned long)groebnerFan.size(), (unsigned long)workingList.size());
  }
  return groebnerFan;
}
