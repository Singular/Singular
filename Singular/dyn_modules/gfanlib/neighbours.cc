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


/***
 * Assuming that r has a ordering whose maximal Groebner cone contains w,
 * i.e. assuming that terms of highest w-degree can be found at the beginning of g,
 * computes the Groebner cone of an ideal I in r containing w relatively.
 **/
gfan::ZCone sloppyGroebnerCone(const poly g, const ring r, const gfan::ZVector w)
{
  int n = r->N;
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);

  /* read off the leading exponent of g and compute its weighted degree */
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  p_GetExpV(g,expv,r);
  gfan::ZVector leadexp = intStar2ZVector(n,expv);
  long d = wDeg(g,r,w);

  /* run through the next terms of same weighted degree and construct the equations */
  poly h=g->next;
  for (; h && wDeg(h,r,w)==d; pIter(h))
  {
    p_GetExpV(h,expv,r);
    equations.appendRow(leadexp-intStar2ZVector(n,expv));
  }

  /* run through the remaining terms and construct the inequalities */
  for (; h; pIter(h))
  {
    p_GetExpV(h,expv,r);
    inequalities.appendRow(leadexp-intStar2ZVector(n,expv));
  }

  omFreeSize(expv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,equations);
}
gfan::ZCone sloppyGroebnerCone(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I);
  gfan::ZCone zc = gfan::ZCone(r->N);
  for (int i=0; i<k; i++)
    zc = intersection(zc,sloppyGroebnerCone(I->m[i],r,w));
  return zc;
}


groebnerCones groebnerNeighbours(const groebnerCone sigma, const tropicalStrategy currentCase)
{
  bool (*reduce)(ideal I, ring r, number p);
  reduce = currentCase.reduce;
  number p = currentCase.uniformizingParameter;

  gfan::ZCone zc = sigma.getPolyhedralCone();
  std::pair<gfan::ZMatrix, gfan::ZMatrix> facets = interiorPointsAndFacetNormals(zc);
  gfan::ZMatrix interiorPoints = facets.first;
  gfan::ZMatrix facetNormals = facets.second;

  groebnerCones neighbours;
  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    std::pair<ideal,ring> flipped = flip(sigma.getPolynomialIdeal(),sigma.getPolynomialRing(),interiorPoints[i],facetNormals[i],currentCase);
    ideal I = flipped.first;
    ring r = flipped.second;
    reduce(I,r,p);
    gfan::ZCone zc = sloppyGroebnerCone(I,r,facetNormals[i]);
    gfan::ZVector p = zc.getRelativeInteriorPoint();
    neighbours.insert(groebnerCone(I,r,zc,p));
  }

  return neighbours;
}

groebnerCones tropicalNeighbours(const groebnerCone sigma, const tropicalStrategy currentCase)
{
  gfan::ZCone zc = sigma.getPolyhedralCone();
  int d = zc.dimension();
  gfan::ZMatrix interiorPoints = facetInteriorPoints(zc);
  groebnerCones neighbours;
  bool (*red)(ideal I, ring r, number p);
  red = currentCase.reduce;
  number p = currentCase.uniformizingParameter;

  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    gfan::ZVector w = interiorPoints[i];
    ideal I = sigma.getPolynomialIdeal();
    ring r = sigma.getPolynomialRing();
    ideal inI = initial(I,r,w);
    std::set<gfan::ZCone> C = tropicalCurve(inI,r,currentCase,d-1);
    std::set<gfan::ZVector> setOfRays = rays(C);

    for (std::set<gfan::ZVector>::iterator ray = setOfRays.begin(); ray!=setOfRays.end(); ray++)
    {
      std::pair<ideal,ring> flipped = flip(I,r,w,*ray,currentCase);
      ideal I = flipped.first;
      ring r = flipped.second;
      red(I,r,p);
      gfan::ZCone c = sloppyGroebnerCone(I,r,*ray);
      gfan::ZVector p = c.getRelativeInteriorPoint();
      neighbours.insert(groebnerCone(I,r,c,p));
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
