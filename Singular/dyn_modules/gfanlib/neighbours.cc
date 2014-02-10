#include <utility>
#include <gfanlib/gfanlib_zfan.h>
#include <Singular/lists.h>
#include <bbcone.h>
#include <bbfan.h>
#include <ttinitialReduction.h>
#include <ppinitialReduction.h>
#include <initial.h>
#include <witness.h>
#include <groebnerCone.h>
#include <tropicalCurves.h>

/***
 * Computes a relative interior point and outer normal vector for each facet of zc
 **/
static gfan::ZMatrix facetInteriorPoints(const gfan::ZCone zc)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  // next we iterate over each of the r facets, build the respective cone and add it to the list
  // this is the i=0 case
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());

  // these are the cases i=1,...,r-2
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i-1,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  }

  // this is the i=r-1 case
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r]);
  facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());

  return relativeInteriorPoints;
}

/***
 * Computes a relative interior point and outer normal vector for each facet of zc
 **/
static std::pair<gfan::ZMatrix,gfan::ZMatrix> interiorPointsAndFacetNormals(const gfan::ZCone zc)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  gfan::ZMatrix outerFacetNormals = gfan::ZMatrix(0,c);
  // next we iterate over each of the r facets, build the respective cone and add it to the list
  // this is the i=0 case
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  outerFacetNormals.appendRow(-inequalities[0]);

  // these are the cases i=1,...,r-2
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i-1,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
    outerFacetNormals.appendRow(-inequalities[i]);
  }

  // this is the i=r-1 case
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r]);
  facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  outerFacetNormals.appendRow(-inequalities[r]);

  return std::make_pair(relativeInteriorPoints,outerFacetNormals);
}

groebnerConesData groebnerNeighbors(const groebnerConeData sigma)
{
  gfan::ZCone zc = sigma.getCone();
  std::pair<gfan::ZMatrix, gfan::ZMatrix> facets = interiorPointsAndFacetNormals(zc);
  gfan::ZMatrix interiorPoints = facets.first;
  gfan::ZMatrix facetNormals = facets.second;
  groebnerConesData neighbours;

  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    std::pair<ideal,ring> flipped = flip(sigma.getIdeal(),sigma.getRing(),interiorPoints[i],facetNormals[i]);
    ideal I = flipped.first;
    ring r = flipped.second;
    ppreduceInitially(I,r);
    neighbours.insert(groebnerConeData(I,r));
  }

  return neighbours;
}

groebnerConesData tropicalNeighbors(const groebnerConeData sigma)
{
  gfan::ZCone zc = sigma.getCone();
  gfan::ZMatrix interiorPoints = facetInteriorPoints(zc);
  groebnerConesData neighbours;

  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    gfan::ZVector w = interiorPoints[i];
    ideal I = sigma.getIdeal();
    ring r = sigma.getRing();
    ideal inI = initial(I,r,w);
    gfan::ZFan zf = tropicalCurve(inI,w,r);
    gfan::ZMatrix u = rays(&zf);

    for (int j=0; j<u.getHeight(); j++)
    {
      std::pair<ideal,ring> flipped = flip(I,r,w,u[j]);
      ideal I = flipped.first;
      ring r = flipped.second;
      ppreduceInitially(I,r);
      neighbours.insert(groebnerConeData(I,r));
    }
  }

  return neighbours;
}

// std::set<gfan::ZCone> tropicalNeighbours(const ideal I, const ring r, const gfan::ZVector w)
void tropicalNeighbours(const ideal I, const ring r, const gfan::ZVector w)
{
  ideal inI = initial(I,r,w);
  gfan::ZFan zf = tropicalCurve(inI,w,r);
  gfan::ZMatrix u = rays(&zf);

  // std::set<gfan::ZCone> neighbours;
  for (int j=0; j<u.getHeight(); j++)
  {
    std::pair<ideal,ring> flipped = flip(I,r,w,u[j]);
    ideal I = flipped.first;
    ring r = flipped.second;
    // ppreduceInitially(I,r);
    gfan::ZCone zc = groebnerCone(I,r,1024*w+u[j]);
    zc.canonicalize();
    std::cout << "extreme rays: " << zc.extremeRays() << std::endl;
    // neighbours.insert(zc);
  }

  // return neighbours;
}

BOOLEAN tropicalNeighbours(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  bigintmat* wbim = (bigintmat*) v->Data();
  gfan::ZVector* w = bigintmatToZVector(wbim);
  (void) tropicalNeighbours(I,currRing,*w);
  // std::set<gfan::ZCone> neighbours = tropicalNeighbours(I,currRing,*w);
  // lists L =  (lists)omAllocBin(slists_bin);
  // L->Init(neighbours.size()); int i=-1;
  // for (std::set<gfan::ZCone>::iterator nb=neighbours.begin(); nb!=neighbours.end(); nb++)
  // {
  //   i++; L->m[i].rtyp=coneID; L->m[i].data=(void*) new gfan::ZCone(*nb);
  // }
  id_Delete(&I,currRing);
  delete w;
  // res->rtyp = LIST_CMD;
  // res->data = (void*) L;
  res->rtyp = NONE;
  res->data = NULL;
  return FALSE;
}
