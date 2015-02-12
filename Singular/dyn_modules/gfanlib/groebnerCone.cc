#include <utility>

#include <kernel/GBEngine/kstd1.h>
#include <kernel/ideals.h>
#include <Singular/ipid.h>

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>
#include <polys/prCopy.h>

#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_matrix.h>

#include <initial.h>
#include <tropicalStrategy.h>
#include <groebnerCone.h>
#include <callgfanlib_conversion.h>
#include <containsMonomial.h>
#include <initial.h>
// #include <flip.h>
#include <tropicalCurves.h>
#include <bbcone.h>

#ifndef NDEBUG
static bool checkPolynomialInput(const ideal I, const ring r)
{
  if (r) rTest(r);
  if (I && r) id_Test(I,r);
  return ((!I) || (I && r));
}

static bool checkOrderingAndCone(const ring r, const gfan::ZCone zc)
{
  return true;
  if (r)
  {
    int n = rVar(r); int* w = r->wvhdl[0];
    gfan::ZVector v = wvhdlEntryToZVector(n,w);
    if (r->order[0]==ringorder_ws)
      v = gfan::Integer((long)-1)*v;
    if (!zc.contains(v))
    {
      std::cout << "ERROR: weight of ordering not inside Groebner cone!" << std::endl
                << "cone: " << std::endl
                << toString(&zc)
                << "weight: " << std::endl
                << v << std::endl;
      return false;
    }
    return true;
  }
  return (zc.dimension()==0);
}

static bool checkPolyhedralInput(const gfan::ZCone zc, const gfan::ZVector p)
{
  return zc.containsRelatively(p);
}

#if 0 /*unused*/
static bool checkOrderingAndWeight(const ideal I, const ring r, const gfan::ZVector w, const tropicalStrategy& currentCase)
{
  groebnerCone sigma(I,r,currentCase);
  gfan::ZCone zc = sigma.getPolyhedralCone();
  return zc.contains(w);
}
#endif

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
  if (polyhedralCone.containsRelatively(interiorPoint))
  {
    std::cout << "ERROR: interiorPoint is contained in the interior of the maximal Groebner cone!" << std::endl
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
  gfan::ZCone dual = polyhedralCone.dualCone();
  if(dual.containsRelatively(facetNormal))
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
#endif //NDEBUG

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
  if (r) polynomialRing = rCopy(r);
  if (I)
  {
    polynomialIdeal = id_Copy(I,r);
    currentCase.pReduce(polynomialIdeal,polynomialRing);
    currentCase.reduce(polynomialIdeal,polynomialRing);
  }

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
    if (g)
    {
      p_GetExpV(g,leadexpv,r);
      leadexpw = expvToZVector(n, leadexpv);
      pIter(g);
      while (g)
      {
        p_GetExpV(g,tailexpv,r);
        tailexpw = expvToZVector(n, tailexpv);
        inequalities.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  // if (currentStrategy->restrictToLowerHalfSpace())
  // {
  //   gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
  //   lowerHalfSpaceCondition[0] = -1;
  //   inequalities.appendRow(lowerHalfSpaceCondition);
  // }

  polyhedralCone = gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
  polyhedralCone.canonicalize();
  interiorPoint = polyhedralCone.getRelativeInteriorPoint();
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
}

groebnerCone::groebnerCone(const ideal I, const ring r, const gfan::ZVector& w, const tropicalStrategy& currentCase):
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  currentStrategy(&currentCase)
{
  assume(checkPolynomialInput(I,r));
  if (r) polynomialRing = rCopy(r);
  if (I)
  {
    polynomialIdeal = id_Copy(I,r);
    currentCase.pReduce(polynomialIdeal,polynomialRing);
    currentCase.reduce(polynomialIdeal,polynomialRing);
  }

  int n = rVar(polynomialRing);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=0; i<idSize(polynomialIdeal); i++)
  {
    poly g = polynomialIdeal->m[i];
    if (g)
    {
      p_GetExpV(g,expv,polynomialRing);
      gfan::ZVector leadexpv = intStar2ZVector(n,expv);
      long d = wDeg(g,polynomialRing,w);
      for (pIter(g); g; pIter(g))
      {
        p_GetExpV(g,expv,polynomialRing);
        gfan::ZVector tailexpv = intStar2ZVector(n,expv);
        if (wDeg(g,polynomialRing,w)==d)
          equations.appendRow(leadexpv-tailexpv);
        else
        {
          assume(wDeg(g,polynomialRing,w)<d);
          inequalities.appendRow(leadexpv-tailexpv);
        }
      }
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));
  // if (currentStrategy->restrictToLowerHalfSpace())
  // {
  //   gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
  //   lowerHalfSpaceCondition[0] = -1;
  //   inequalities.appendRow(lowerHalfSpaceCondition);
  // }

  polyhedralCone = gfan::ZCone(inequalities,equations);
  polyhedralCone.canonicalize();
  interiorPoint = polyhedralCone.getRelativeInteriorPoint();
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
}

/***
 * Computes the groebner cone of I around u+e*w for e>0 sufficiently small.
 * Assumes that this cone is a face of the maximal Groenbner cone given by the ordering of r.
 **/
groebnerCone::groebnerCone(const ideal I, const ring r, const gfan::ZVector& u, const gfan::ZVector& w, const tropicalStrategy& currentCase):
  polynomialIdeal(NULL),
  polynomialRing(NULL),
  currentStrategy(&currentCase)
{
  assume(checkPolynomialInput(I,r));
  if (r) polynomialRing = rCopy(r);
  if (I)
  {
    polynomialIdeal = id_Copy(I,r);
    currentCase.pReduce(polynomialIdeal,polynomialRing);
    currentCase.reduce(polynomialIdeal,polynomialRing);
  }

  int n = rVar(polynomialRing);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=0; i<idSize(polynomialIdeal); i++)
  {
    poly g = polynomialIdeal->m[i];
    if (g)
    {
      p_GetExpV(g,expv,polynomialRing);
      gfan::ZVector leadexpv = intStar2ZVector(n,expv);
      long d1 = wDeg(g,polynomialRing,u);
      long d2 = wDeg(g,polynomialRing,w);
      for (pIter(g); g; pIter(g))
      {
        p_GetExpV(g,expv,polynomialRing);
        gfan::ZVector tailexpv = intStar2ZVector(n,expv);
        if (wDeg(g,polynomialRing,u)==d1 && wDeg(g,polynomialRing,w)==d2)
          equations.appendRow(leadexpv-tailexpv);
        else
        {
          assume(wDeg(g,polynomialRing,u)<d1 || wDeg(g,polynomialRing,w)<d2);
          inequalities.appendRow(leadexpv-tailexpv);
        }
      }
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));
  // if (currentStrategy->restrictToLowerHalfSpace())
  // {
  //   gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
  //   lowerHalfSpaceCondition[0] = -1;
  //   inequalities.appendRow(lowerHalfSpaceCondition);
  // }

  polyhedralCone = gfan::ZCone(inequalities,equations);
  polyhedralCone.canonicalize();
  interiorPoint = polyhedralCone.getRelativeInteriorPoint();
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
}


groebnerCone::groebnerCone(const ideal I, const ideal inI, const ring r, const tropicalStrategy& currentCase):
  polynomialIdeal(id_Copy(I,r)),
  polynomialRing(rCopy(r)),
  currentStrategy(&currentCase)
{
  assume(checkPolynomialInput(I,r));
  assume(checkPolynomialInput(inI,r));

  currentCase.pReduce(polynomialIdeal,polynomialRing);
  currentCase.reduce(polynomialIdeal,polynomialRing);

  int n = rVar(r);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=0; i<idSize(inI); i++)
  {
    poly g = inI->m[i];
    if (g)
    {
      p_GetExpV(g,expv,r);
      gfan::ZVector leadexpv = intStar2ZVector(n,expv);
      for (pIter(g); g; pIter(g))
      {
        p_GetExpV(g,expv,r);
        gfan::ZVector tailexpv = intStar2ZVector(n,expv);
        equations.appendRow(leadexpv-tailexpv);
      }
    }
  }
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<idSize(polynomialIdeal); i++)
  {
    poly g = polynomialIdeal->m[i];
    if (g)
    {
      p_GetExpV(g,expv,r);
      gfan::ZVector leadexpv = intStar2ZVector(n,expv);
      for (pIter(g); g; pIter(g))
      {
        p_GetExpV(g,expv,r);
        gfan::ZVector tailexpv = intStar2ZVector(n,expv);
        inequalities.appendRow(leadexpv-tailexpv);
      }
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));
  if (currentStrategy->restrictToLowerHalfSpace())
  {
    gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
    lowerHalfSpaceCondition[0] = -1;
    inequalities.appendRow(lowerHalfSpaceCondition);
  }

  polyhedralCone = gfan::ZCone(inequalities,equations);
  polyhedralCone.canonicalize();
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

/**
 * Returns true if Groebner cone contains w, false otherwise
 */
bool groebnerCone::contains(const gfan::ZVector &w) const
{
  return polyhedralCone.contains(w);
}


/***
 * Returns a point in the tropical variety, if the groebnerCone contains one.
 * Returns an empty vector otherwise.
 **/
gfan::ZVector groebnerCone::tropicalPoint() const
{
  assume(checkOrderingAndCone(polynomialRing,polyhedralCone));
  ideal I = polynomialIdeal;
  ring r = polynomialRing;

  gfan::ZCone coneToCheck = polyhedralCone;
  gfan::ZMatrix R = coneToCheck.extremeRays();
  for (int i=0; i<R.getHeight(); i++)
  {
    assume(!currentStrategy->restrictToLowerHalfSpace() || R[i][0].sign()<=0);
    if (currentStrategy->restrictToLowerHalfSpace() && R[i][0].sign()==0)
      continue;
    std::pair<poly,int> s = currentStrategy->checkInitialIdealForMonomial(I,r,R[i]);
    if (s.first==NULL)
    {
      if (s.second<0)
        // if monomial was initialized, delete it
        p_Delete(&s.first,r);
      return R[i];
    }
  }
  return gfan::ZVector();
}

/**
 * Given an interior point on the facet and the outer normal factor on the facet,
 * returns the adjacent groebnerCone sharing that facet
 */
groebnerCone groebnerCone::flipCone(const gfan::ZVector &interiorPoint, const gfan::ZVector &facetNormal) const
{
  assume(this->checkFlipConeInput(interiorPoint,facetNormal));
  /* Note: the polynomial ring created will have a weighted ordering with respect to interiorPoint
   *   and with a weighted ordering with respect to facetNormal as tiebreaker.
   *   Hence it is sufficient to compute the initial form with respect to facetNormal,
   *   to obtain an initial form with respect to interiorPoint+e*facetNormal,
   *   for e>0 sufficiently small */
  std::pair<ideal,ring> flipped = currentStrategy->computeFlip(polynomialIdeal,polynomialRing,interiorPoint,facetNormal);
  assume(checkPolynomialInput(flipped.first,flipped.second));
  groebnerCone flippedCone(flipped.first, flipped.second, interiorPoint, facetNormal, *currentStrategy);
  id_Delete(&flipped.first,flipped.second);
  rDelete(flipped.second);
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
  {
    gfan::ZVector w = interiorPoints[i];
    gfan::ZVector v = facetNormals[i];
    if (currentStrategy->restrictToLowerHalfSpace())
    {
      assume(w[0].sign()<=0);
      if (w[0].sign()==0 && v[0].sign()>0)
        continue;
    }
    neighbours.insert(flipCone(interiorPoints[i],facetNormals[i]));
  }
  return neighbours;
}


bool groebnerCone::pointsOutwards(const gfan::ZVector w) const
{
  gfan::ZCone dual = polyhedralCone.dualCone();
  return (!dual.contains(w));
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
    if (!(currentStrategy->restrictToLowerHalfSpace() && interiorPoints[i][0].sign()==0))
    {
      ideal initialIdeal = initial(polynomialIdeal,polynomialRing,interiorPoints[i]);
      gfan::ZMatrix ray = raysOfTropicalStar(initialIdeal,polynomialRing,interiorPoints[i],currentStrategy);
      for (int j=0; j<ray.getHeight(); j++)
        if (pointsOutwards(ray[j]))
        {
          groebnerCone neighbour = flipCone(interiorPoints[i],ray[j]);
          neighbours.insert(neighbour);
        }
      id_Delete(&initialIdeal,polynomialRing);
    }
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
    {
      gfan::ZCone zc = sigma->getPolyhedralCone();
      assume(isCompatible(zf,&zc));
      zf->insert(zc);
    }
    return zf;
  }
  else
    return new gfan::ZFan(gfan::ZFan::fullFan(currRing->N));
}


#ifndef NDEBUG

BOOLEAN flipConeDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      leftv w = v->next;
      if ((w!=NULL) && (w->Typ()==BIGINTMAT_CMD))
      {
        leftv x = w->next;
        if ((x!=NULL) && (x->Typ()==BIGINTMAT_CMD))
        {
          omUpdateInfo();
          Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);

          ideal I = (ideal) u->CopyD();
          number p = (number) v->CopyD();
          bigintmat* interiorPoint0 = (bigintmat*) w->CopyD();
          bigintmat* facetNormal0 = (bigintmat*) x->CopyD();
          tropicalStrategy debug = tropicalStrategy::debugStrategy(I,p,currRing);

          gfan::ZVector* interiorPoint = bigintmatToZVector(interiorPoint0);
          gfan::ZVector* facetNormal = bigintmatToZVector(facetNormal0);

          groebnerCone sigma(I,currRing,debug);
          groebnerCone theta = sigma.flipCone(*interiorPoint,*facetNormal);

          id_Delete(&I,currRing);
          n_Delete(&p,currRing->cf);
          delete interiorPoint0;
          delete facetNormal0;
          delete interiorPoint;
          delete facetNormal;

          res->rtyp = NONE;
          res->data = NULL;
          return FALSE;
        }
      }
    }
  }
  WerrorS("computeFlipDebug: unexpected parameters");
  return TRUE;
}

BOOLEAN groebnerNeighboursDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);

      ideal I = (ideal) u->CopyD();
      number p = (number) v->CopyD();

      tropicalStrategy debug = tropicalStrategy::debugStrategy(I,p,currRing);
      groebnerCone sigma(I,currRing,debug);
      groebnerCones neighbours = sigma.groebnerNeighbours();

      id_Delete(&I,currRing);
      n_Delete(&p,currRing->cf);
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }
  }
  WerrorS("computeFlipDebug: unexpected parameters");
  return TRUE;
}

BOOLEAN tropicalNeighboursDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);

      ideal I = (ideal) u->CopyD();
      number p = (number) v->CopyD();

      tropicalStrategy debug = tropicalStrategy::debugStrategy(I,p,currRing);
      groebnerCone sigma(I,currRing,debug);
      groebnerCones neighbours = sigma.groebnerNeighbours();

      id_Delete(&I,currRing);
      n_Delete(&p,currRing->cf);
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }
  }
  WerrorS("computeFlipDebug: unexpected parameters");
  return TRUE;
}
#endif
