#include <utility>
#include <gfanlib/gfanlib_zfan.h>
#include <Singular/lists.h>
#include <callgfanlib_conversion.h>
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
  if (r==0) return gfan::ZMatrix(0,c);

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
  newEquations.appendRow(inequalities[r-1]);
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
  if (r<=1)
    return std::make_pair(relativeInteriorPoints,outerFacetNormals);
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
  newEquations.appendRow(inequalities[r-1]);
  facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  outerFacetNormals.appendRow(-inequalities[r-1]);

  return std::make_pair(relativeInteriorPoints,outerFacetNormals);
}

setOfGroebnerConeData groebnerNeighbours(const groebnerConeData sigma, const tropicalStrategy currentCase)
{
  bool (*reduce)(ideal I, ring r);
  reduce = currentCase.reduce;

  gfan::ZCone zc = sigma.getCone();
  std::pair<gfan::ZMatrix, gfan::ZMatrix> facets = interiorPointsAndFacetNormals(zc);
  gfan::ZMatrix interiorPoints = facets.first;
  gfan::ZMatrix facetNormals = facets.second;
  setOfGroebnerConeData neighbours;

  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    std::pair<ideal,ring> flipped = flip(sigma.getIdeal(),sigma.getRing(),interiorPoints[i],facetNormals[i],currentCase);
    ideal I = flipped.first;
    ring r = flipped.second;
    reduce(I,r);
    gfan::ZCone c = sloppyGroebnerCone(I,r,facetNormals[i]);
    gfan::ZVector p = c.getRelativeInteriorPoint();
    neighbours.insert(groebnerConeData(I,r,c,p));
  }

  return neighbours;
}

setOfGroebnerConeData tropicalNeighbours(const groebnerConeData sigma, const tropicalStrategy currentCase)
{
  gfan::ZCone zc = sigma.getCone();
  int d = zc.dimension();
  gfan::ZMatrix interiorPoints = facetInteriorPoints(zc);
  setOfGroebnerConeData neighbours;
  bool (*red)(ideal I, ring r);
  red = currentCase.reduce;

  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    gfan::ZVector w = interiorPoints[i];
    ideal I = sigma.getIdeal();
    ring r = sigma.getRing();
    ideal inI = initial(I,r,w);
    std::set<gfan::ZCone> C = tropicalCurve(inI,r,d-1,currentCase);
    std::set<gfan::ZVector> setOfRays = rays(C);

    for (std::set<gfan::ZVector>::iterator ray = setOfRays.begin(); ray!=setOfRays.end(); ray++)
    {
      std::pair<ideal,ring> flipped = flip(I,r,w,*ray,currentCase);
      ideal I = flipped.first;
      ring r = flipped.second;
      red(I,r);
      gfan::ZCone c = sloppyGroebnerCone(I,r,*ray);
      gfan::ZVector p = c.getRelativeInteriorPoint();
      neighbours.insert(groebnerConeData(I,r,c,p));
    }
  }

  return neighbours;
}

BOOLEAN tropicalNeighbours(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  bigintmat* wbim = (bigintmat*) v->Data();
  gfan::ZVector* w = bigintmatToZVector(wbim);
  // (void) tropicalNeighbours(I,currRing,*w);
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
