#include <callgfanlib_conversion.h>
#include <containsMonomial.h>
#include <tropical.h>
#include <initial.h>
#include <lift.h>
#include <groebnerCone.h>
#include <neighbours.h>
#include <tropicalStrategy.h>
#include <tropicalCurves.h>
#include <bbcone.h>
#include <tropicalVariety.h>


/***
 * checks whether sigma is contained in the tropical variety
 * by testing whether the initial Ideal with respect to the interior point
 * is monomial free.
 **/
static bool checkContainmentInTropicalVariety(const groebnerCone sigma)
{
  gfan::ZVector w = sigma.getInteriorPoint();
  ideal I = sigma.getPolynomialIdeal();
  ring r = sigma.getPolynomialRing();
  ideal inI = sloppyInitial(I,r,w);
  poly s = checkForMonomialViaSuddenSaturation(inI,r);
  id_Delete(&inI,r);
  if (s)
  {
    p_Delete(&s,r);
    return false;
  }
  return true;
}


/***
 * Computes a starting point by traversing the Groebner fan,
 * checking each cone whether it contains a ray in the tropical variety.
 * Returns a point in the tropical variety and a maximal Groebner cone
 * containing the point.
 **/
std::pair<gfan::ZVector,groebnerCone> tropicalStartingDataViaGroebnerFan(const ideal I, const ring r, const tropicalStrategy& currentStrategy)
{
  currentStrategy.reduce(I,r);

  const groebnerCone sigma(I,r,currentStrategy);
  gfan::ZVector startingPoint = sigma.tropicalPoint();
  if (startingPoint.size() > 0)
    return std::make_pair(startingPoint,sigma);
  groebnerCones groebnerFan;
  groebnerCones workingList;
  workingList.insert(sigma);

  while (!workingList.empty())
  {
    const groebnerCone sigma = *(workingList.begin());
    groebnerCones neighbours = sigma.groebnerNeighbours();
    for (groebnerCones::iterator tau = neighbours.begin(); tau!=neighbours.end(); tau++)
    {
      if (groebnerFan.count(*tau) == 0)
      {
        if (workingList.count(*tau) == 0)
        {
          startingPoint = tau->tropicalPoint();
          if (startingPoint.size() > 0)
            return std::make_pair(startingPoint,*tau);
        }
        workingList.insert(*tau);
      }
    }
    groebnerFan.insert(sigma);
    workingList.erase(sigma);
  }
  gfan::ZVector emptyVector = gfan::ZVector(0);
  groebnerCone emptyCone = groebnerCone();
  return std::pair<gfan::ZVector,groebnerCone>(emptyVector,emptyCone);
}

BOOLEAN tropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    tropicalStrategy currentStrategy(I,currRing);
    std::pair<gfan::ZVector,groebnerCone> startingData = tropicalStartingDataViaGroebnerFan(I,currRing,currentStrategy);
    gfan::ZVector startingPoint = startingData.first;
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(startingPoint);
    return FALSE;
  }
  WerrorS("tropicalStartingPoint: unexpected parameters");
  return TRUE;
}

/***
 * returs the lineality space of the Groebner fan
 **/
static gfan::ZCone linealitySpaceOfGroebnerFan(const ideal I, const ring r)
{
  int n = rVar(r);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  int k = idSize(I);
  for (int i=0; i<k; i++)
  {
    poly g = I->m[i];
    p_GetExpV(g,expv,r);
    gfan::ZVector leadexp = intStar2ZVector(n,expv);
    for (pIter(g); g; pIter(g))
    {
      p_GetExpV(g,expv,r);
      equations.appendRow(leadexp-intStar2ZVector(n,expv));
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));
  return gfan::ZCone(gfan::ZMatrix(0,n),equations);
}


/***
 * Computes a starting cone in the tropical variety.
 **/
groebnerCone tropicalStartingCone(const ideal I, const ring r, const tropicalStrategy& currentStrategy)
{
  ring s = rCopy(r);
  int k = idSize(I); ideal inI = idInit(k);
  nMapFunc identityMap = n_SetMap(r->cf,s->cf);
  for (int i=0; i<k; i++)
    inI->m[i] = p_PermPoly(I->m[i],NULL,r,s,identityMap,NULL,0);

  gfan::ZCone zc = linealitySpaceOfGroebnerFan(inI,s);
  gfan::ZVector startingPoint; groebnerCone ambientMaximalCone;
  while (zc.dimension()<currentStrategy.getDimensionOfIdeal())
  {
    std::pair<gfan::ZVector,groebnerCone> startingData = tropicalStartingDataViaGroebnerFan(inI,s,currentStrategy);
    startingPoint = startingData.first;
    ambientMaximalCone = groebnerCone(startingData.second);

    id_Delete(&inI,s); rDelete(s);
    inI = ambientMaximalCone.getPolynomialIdeal();
    s = ambientMaximalCone.getPolynomialRing();

    inI = sloppyInitial(inI,s,startingPoint);
    zc = linealitySpaceOfGroebnerFan(inI,s);
  }

  ideal J = lift(I,r,inI,s);
  groebnerCone tropicalStartingCone(J,inI,s,currentStrategy);
  id_Delete(&inI,s);
  id_Delete(&J,s);

  assume(checkContainmentInTropicalVariety(tropicalStartingCone));
  return tropicalStartingCone;
}


BOOLEAN tropicalStartingCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == NUMBER_CMD))
    {
      number p = (number) v->Data();
      leftv w = v->next;
      if (w==NULL)
      {
        tropicalStrategy valuedCase(I,p,currRing);
        groebnerCone sigma = tropicalStartingCone(I,currRing,valuedCase);
        res->rtyp = coneID;
        res->data = (char*) new gfan::ZCone(sigma.getPolyhedralCone());
        return FALSE;
      }
    }
    else
    {
      if (v==NULL)
      {
        tropicalStrategy nonValuedCase(I,currRing);
        groebnerCone sigma = tropicalStartingCone(I,currRing,nonValuedCase);
        res->rtyp = coneID;
        res->data = (char*) new gfan::ZCone(sigma.getPolyhedralCone());
        return FALSE;
      }
    }
  }
  WerrorS("tropicalStartingCone: unexpected parameters");
  return TRUE;
}
