#include <utility>

#include <kernel/kstd1.h>
#include <kernel/ideals.h>
#include <Singular/ipid.h>

#include <libpolys/polys/monomials/p_polys.h>
#include <libpolys/polys/monomials/ring.h>
#include <libpolys/polys/prCopy.h>

#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_matrix.h>

#include <tropicalStrategy.h>
#include <groebnerCone.h>
#include <callgfanlib_conversion.h>
#include <containsMonomial.h>
#include <initial.h>
#include <flip.h>
#include <tropicalCurves.h>
#include <bbcone.h>

static bool checkPolynomialInput(const ideal I, const ring r)
{
  if (r) rTest(r);
  if (I && r) id_Test(I,r);
  return ((!I) || (I && r));
}

static bool checkOrderingAndCone(const ring r, const gfan::ZCone zc)
{
  if (r)
  {
    int n = rVar(r); int* w = r->wvhdl[0];
    gfan::ZVector v = wvhdlEntryToZVector(n,w);
    if (!zc.contains(v))
    {
      std::cout << "ERROR: weight of ordering not inside Groebner cone!" << std::endl
                << "cone: " << std::endl
                << toString(&zc)
                << "weight: " << std::endl
                << v << std::endl;
    }
    return zc.contains(v);
  }
  return (zc.dimension()==0);
}

static bool checkPolyhedralInput(const gfan::ZCone zc, const gfan::ZVector p)
{
  return zc.containsRelatively(p);
}

groebnerCone::groebnerCone():
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  polyhedralCone(gfan::ZCone(0)),
  interiorPoint(gfan::ZVector(0)),
  currentStrategy(NULL)
{
}

groebnerCone::groebnerCone(const ideal I, const ring r, const tropicalStrategy& currentCase):
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  currentStrategy(&currentCase)
{
  assume(checkPolynomialInput(I,r));
  if (I) polynomialIdeal = id_Copy(I,r);
  if (r) polynomialRing = rCopy(r);

  int n = rVar(polynomialRing);
  poly g = NULL;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<idSize(polynomialIdeal); i++)
  {
    g = polynomialIdeal->m[i];
    pGetExpV(g,leadexpv);
    leadexpw = intStar2ZVector(n, leadexpv);
    pIter(g);
    while (g)
    {
      pGetExpV(g,tailexpv);
      tailexpw = intStar2ZVector(n, tailexpv);
      inequalities.appendRow(leadexpw-tailexpw);
      pIter(g);
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  polyhedralCone = gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
  interiorPoint = polyhedralCone.getRelativeInteriorPoint();
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
}

static bool checkOrderingAndWeight(const ideal I, const ring r, const gfan::ZVector w, const tropicalStrategy& currentCase)
{
  groebnerCone sigma(I,r,currentCase);
  gfan::ZCone zc = sigma.getPolyhedralCone();
  return zc.contains(w);
}

groebnerCone::groebnerCone(const ideal I, const ring r, const gfan::ZVector& w, const tropicalStrategy& currentCase):
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  currentStrategy(&currentCase)
{
  assume(checkPolynomialInput(I,r));
  if (I) polynomialIdeal = id_Copy(I,r);
  if (r) polynomialRing = rCopy(r);

  int n = rVar(r);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=0; i<idSize(polynomialIdeal); i++)
  {
    poly g = polynomialIdeal->m[i];
    pGetExpV(g,expv);
    gfan::ZVector leadexpv = intStar2ZVector(n,expv);
    long d = wDeg(g,polynomialRing,w);
    for (pIter(g); g; pIter(g))
    {
      pGetExpV(g,expv); gfan::ZVector tailexpv = intStar2ZVector(n,expv);
      if (wDeg(g,polynomialRing,w)==d)
        equations.appendRow(leadexpv-tailexpv);
      else
      {
        assume(wDeg(g,polynomialRing,w)<d);
        inequalities.appendRow(leadexpv-tailexpv);
      }
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));

  polyhedralCone = gfan::ZCone(inequalities,equations);
  interiorPoint = polyhedralCone.getRelativeInteriorPoint();
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
}

groebnerCone::groebnerCone(const groebnerCone &sigma):
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  polyhedralCone(gfan::ZCone(sigma.getPolyhedralCone())),
  interiorPoint(gfan::ZVector(sigma.getInteriorPoint())),
  currentStrategy(sigma.getTropicalStrategy())
{
  assume(checkPolynomialInput(sigma.getPolynomialIdeal(),sigma.getPolynomialRing()));
  assume(checkOrderingAndCone(sigma.getPolynomialRing(),sigma.getPolyhedralCone()));
  assume(checkPolyhedralInput(sigma.getPolyhedralCone(),sigma.getInteriorPoint()));
  if (sigma.getPolynomialIdeal()) polynomialIdeal = id_Copy(sigma.getPolynomialIdeal(),sigma.getPolynomialRing());
  if (sigma.getPolynomialRing()) polynomialRing = rCopy(sigma.getPolynomialRing());
}

groebnerCone::~groebnerCone()
{
  assume(checkPolynomialInput(polynomialIdeal,polynomialRing));
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
  assume(checkPolyhedralInput(polyhedralCone,interiorPoint));
  if (polynomialIdeal) id_Delete(&polynomialIdeal,polynomialRing);
  if (polynomialRing) rDelete(polynomialRing);
}

groebnerCone& groebnerCone::operator=(const groebnerCone& sigma)
{
  assume(checkPolynomialInput(sigma.getPolynomialIdeal(),sigma.getPolynomialRing()));
  assume(checkOrderingAndCone(sigma.getPolynomialRing(),sigma.getPolyhedralCone()));
  assume(checkPolyhedralInput(sigma.getPolyhedralCone(),sigma.getInteriorPoint()));
  if (sigma.getPolynomialIdeal()) polynomialIdeal = id_Copy(sigma.getPolynomialIdeal(),sigma.getPolynomialRing());
  if (sigma.getPolynomialRing()) polynomialRing = rCopy(sigma.getPolynomialRing());
  polyhedralCone = sigma.getPolyhedralCone();
  interiorPoint = sigma.getInteriorPoint();
  currentStrategy = sigma.getTropicalStrategy();
  return *this;
}


/***
 * Returns a point in the tropical variety, if the groebnerCone contains one.
 * Returns an empty vector otherwise.
 **/
gfan::ZVector groebnerCone::tropicalPoint() const
{
  ideal I = polynomialIdeal;
  ring r = polynomialRing;
  gfan::ZCone zc = polyhedralCone;
  gfan::ZMatrix R = zc.extremeRays();
  assume(checkOrderingAndCone(r,zc));
  for (int i=0; i<R.getHeight(); i++)
  {
    ideal inI = initial(I,r,R[i]);
    poly s = checkForMonomialViaSuddenSaturation(inI,r);
    id_Delete(&inI,r);
    if (s == NULL)
    {
      p_Delete(&s,r);
      return R[i];
    }
    p_Delete(&s,r);
  }
  return gfan::ZVector();
}

bool groebnerCone::checkFlipConeInput(const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const
{
  /* check first whether interiorPoint lies on the boundary of the cone */
  if (!polyhedralCone.contains(interiorPoint))
  {
    std::cout << "ERROR: interiorPoint is not contained in the Groebner cone!" << std::endl
              << "cone: " << std::endl
              << toString(&polyhedralCone)
              << "interiorPoint:" << std::endl
              << interiorPoint << std::endl;
    return false;
  }
  gfan::ZCone hopefullyAFacet = polyhedralCone.faceContaining(interiorPoint);
  if (hopefullyAFacet.dimension()!=(polyhedralCone.dimension()-1))
  {
    std::cout << "ERROR: interiorPoint is not contained in the interior of a facet!" << std::endl
              << "cone: " << std::endl
              << toString(&polyhedralCone)
              << "interiorPoint:" << std::endl
              << interiorPoint << std::endl;
    return false;
  }
  /* check whether facet normal points outwards */
  gfan::ZMatrix halfSpaceInequality(0,facetNormal.size());
  halfSpaceInequality.appendRow(facetNormal);
  gfan::ZCone halfSpace = gfan::ZCone(halfSpaceInequality,gfan::ZMatrix(0,facetNormal.size()));
  hopefullyAFacet = intersection(polyhedralCone, halfSpace);
  if (hopefullyAFacet.dimension()!=(polyhedralCone.dimension()-1))
  {
    std::cout << "ERROR: facetNormal is not pointing outwards!" << std::endl
              << "cone: " << std::endl
              << toString(&polyhedralCone)
              << "facetNormal:" << std::endl
              << facetNormal << std::endl;
    return false;
  }
  return true;
}

/***
 * Given an interior point on the facet and the outer normal factor on the facet,
 * returns the adjacent groebnerCone sharing that facet
 **/
groebnerCone groebnerCone::flipCone(const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const
{
  assume(this->checkFlipConeInput(interiorPoint,facetNormal));
  /* Note: the polynomial ring created will have a weighted ordering with respect to interiorPoint
   *   and with a weighted ordering with respect to facetNormal as tiebreaker.
   *   Hence it is sufficient to compute the initial form with respect to facetNormal,
   *   to obtain an initial form with respect to interiorPoint+e*facetNormal,
   *   for e>0 sufficiently small */
  std::pair<ideal,ring> flipped = flip(polynomialIdeal,polynomialRing,interiorPoint,facetNormal,*currentStrategy);
  assume(checkPolynomialInput(flipped.first,flipped.second));
  groebnerCone flippedCone(flipped.first, flipped.second, facetNormal, *currentStrategy);
  return flippedCone;
}


/***
 * Computes a relative interior point and an outer normal vector for each facet of zc
 **/
static std::pair<gfan::ZMatrix,gfan::ZMatrix> interiorPointsAndNormalsOfFacets(const gfan::ZCone zc)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  /* our cone has r facets, if r==0 return empty matrices */
  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  gfan::ZMatrix outerFacetNormals = gfan::ZMatrix(0,c);
  if (r==0)
    return std::make_pair(relativeInteriorPoints,outerFacetNormals);

  /* next we iterate over each of the r facets,
   * build the respective cone and add it to the list
   * this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  assume(zc.contains(relativeInteriorPoints[0]) && !zc.containsRelatively(relativeInteriorPoints[0]));
  outerFacetNormals.appendRow(-inequalities[0]);

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
    assume(zc.contains(relativeInteriorPoints[i]) && !zc.containsRelatively(relativeInteriorPoints[i]));
    outerFacetNormals.appendRow(-inequalities[i]);
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  assume(zc.contains(relativeInteriorPoints[r-1]) && !zc.containsRelatively(relativeInteriorPoints[r-1]));
  outerFacetNormals.appendRow(-inequalities[r-1]);

  return std::make_pair(relativeInteriorPoints,outerFacetNormals);
}


/***
 * Returns a complete list of neighboring Groebner cones.
 **/
groebnerCones groebnerCone::groebnerNeighbours() const
{
  std::pair<gfan::ZMatrix, gfan::ZMatrix> facetsData = interiorPointsAndNormalsOfFacets(polyhedralCone);
  gfan::ZMatrix interiorPoints = facetsData.first;
  gfan::ZMatrix facetNormals = facetsData.second;

  groebnerCones neighbours;
  for (int i=0; i<interiorPoints.getHeight(); i++)
    neighbours.insert(this->flipCone(interiorPoints[i],facetNormals[i]));

  return neighbours;
}


/***
 * Computes a relative interior point for each facet of zc
 **/
static gfan::ZMatrix interiorPointsOfFacets(const gfan::ZCone zc)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  /* our cone has r facets, if r==0 return empty matrices */
  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  if (r==0) return relativeInteriorPoints;

  /* next we iterate over each of the r facets,
   * build the respective cone and add it to the list
   * this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i-1,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  facet = gfan::ZCone(newInequalities,newEquations);
  relativeInteriorPoints.appendRow(facet.getRelativeInteriorPoint());

  return relativeInteriorPoints;
}


/***
 * Returns a complete list of neighboring Groebner cones in the tropical variety.
 **/
groebnerCones groebnerCone::tropicalNeighbours() const
{
  gfan::ZMatrix interiorPoints = interiorPointsOfFacets(polyhedralCone);
  groebnerCones neighbours;
  for (int i=0; i<interiorPoints.getHeight(); i++)
  {
    ideal initialIdeal = initial(polynomialIdeal,polynomialRing,interiorPoints[i]);
    std::set<gfan::ZVector> rays = raysOfTropicalCurve(initialIdeal,polynomialRing,*currentStrategy);
    groebnerCones neighbours;
    for (std::set<gfan::ZVector>::iterator ray = rays.begin(); ray!=rays.end(); ray++)
      neighbours.insert(this->flipCone(interiorPoints[i],*ray));
  }
  return neighbours;
}


gfan::ZFan* toFanStar(groebnerCones setOfCones)
{
  if (setOfCones.size() > 0)
  {
    groebnerCones::iterator sigma = setOfCones.begin();
    gfan::ZFan* zf = new gfan::ZFan(sigma->getPolyhedralCone().ambientDimension());
    for (; sigma!=setOfCones.end(); sigma++)
      zf->insert(sigma->getPolyhedralCone());
    return zf;
  }
  else
    return new gfan::ZFan(gfan::ZFan::fullFan(currRing->N));
}
