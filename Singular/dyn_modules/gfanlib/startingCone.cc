#include <callgfanlib_conversion.h>
#include <containsMonomial.h>
#include <tropical.h>
#include <initial.h>
#include <lift.h>
#include <groebnerCone.h>
#include <tropicalStrategy.h>
#include <tropicalCurves.h>
#include <bbcone.h>
#include <tropicalVarietyOfPolynomials.h>
#include <tropicalVariety.h>
#include <tropicalStrategy.h>


groebnerCone groebnerStartingCone(const tropicalStrategy& currentStrategy)
{
  groebnerCone sigma(currentStrategy.getStartingIdeal(), currentStrategy.getStartingRing(), currentStrategy);
  return sigma;
}


/**
 * Computes a starting point outside the lineatliy space by traversing the Groebner fan,
 * checking each cone whether it contains a ray in the tropical variety.
 * Returns a point in the tropical variety and a maximal Groebner cone containing the point.
 **/
std::pair<gfan::ZVector,groebnerCone> tropicalStartingPoint(const ideal I, const ring r, const tropicalStrategy& currentStrategy)
{
  // start by computing a maximal Groebner cone and
  // check whether one of its rays lies in the tropical variety
  const groebnerCone sigma(I,r,currentStrategy);
  gfan::ZVector startingPoint = sigma.tropicalPoint();
  if (startingPoint.size() > 0)
    return std::make_pair(startingPoint,sigma);

  // if not, traverse the groebnerFan and until such a cone is found
  // and return the maximal cone together with a point in its ray
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

  // return some trivial output, if such a cone cannot be found
  gfan::ZVector emptyVector = gfan::ZVector(0);
  groebnerCone emptyCone = groebnerCone();
  return std::pair<gfan::ZVector,groebnerCone>(emptyVector,emptyCone);
}


/**
 * Computes a starting point outside the lineatliy space by traversing the Groebner fan,
 * checking each cone whether it contains a ray in the tropical variety.
 * Returns a point in the tropical variety and a maximal Groebner cone containing the point.
 **/
std::pair<gfan::ZVector,groebnerCone> tropicalStartingDataViaGroebnerFan(const ideal I, const ring r, const tropicalStrategy& currentStrategy)
{
  // start by computing a maximal Groebner cone and
  // check whether one of its rays lies in the tropical variety
  const groebnerCone sigma(I,r,currentStrategy);
  gfan::ZVector startingPoint = sigma.tropicalPoint();
  if (startingPoint.size() > 0)
    return std::make_pair(startingPoint,sigma);

  // if not, traverse the groebnerFan and until such a cone is found
  // and return the maximal cone together with a point in its ray
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

  // return some trivial output, if such a cone cannot be found
  gfan::ZVector emptyVector = gfan::ZVector(0);
  groebnerCone emptyCone = groebnerCone();
  return std::pair<gfan::ZVector,groebnerCone>(emptyVector,emptyCone);
}

BOOLEAN positiveTropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    if (idSize(I)==1)
    {
      tropicalStrategy currentStrategy(I,currRing);
      poly g = I->m[0];
      std::set<gfan::ZCone> Tg = tropicalVariety(g,currRing,&currentStrategy);
      for (std::set<gfan::ZCone>::iterator zc=Tg.begin(); zc!=Tg.end(); zc++)
      {
        gfan::ZMatrix ray = zc->extremeRays();
        for (int i=0; i<ray.getHeight(); i++)
        {
          if (ray[i].isPositive())
          {
            res->rtyp = BIGINTMAT_CMD;
            res->data = (void*) zVectorToBigintmat(ray[i]);
            return FALSE;
          }
        }
      }
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(gfan::ZVector(0));
      return FALSE;
    }
    WerrorS("positiveTropicalStartingPoint: ideal not principal");
    return TRUE;
  }
  WerrorS("positiveTropicalStartingPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN nonNegativeTropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    if (idSize(I)==1)
    {
      tropicalStrategy currentStrategy(I,currRing);
      poly g = I->m[0];
      std::set<gfan::ZCone> Tg = tropicalVariety(g,currRing,&currentStrategy);
      for (std::set<gfan::ZCone>::iterator zc=Tg.begin(); zc!=Tg.end(); zc++)
      {
        gfan::ZMatrix ray = zc->extremeRays();
        for (int i=0; i<ray.getHeight(); i++)
        {
          if (ray[i].isNonNegative())
          {
            res->rtyp = BIGINTMAT_CMD;
            res->data = (void*) zVectorToBigintmat(ray[i]);
            return FALSE;
          }
        }
      }
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(gfan::ZVector(0));
      return FALSE;
    }
    WerrorS("nonNegativeTropicalStartingPoint: ideal not principal");
    return TRUE;
  }
  WerrorS("nonNegativeTropicalStartingPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN negativeTropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    if (idSize(I)==1)
    {
      tropicalStrategy currentStrategy(I,currRing);
      poly g = I->m[0];
      std::set<gfan::ZCone> Tg = tropicalVariety(g,currRing,&currentStrategy);
      for (std::set<gfan::ZCone>::iterator zc=Tg.begin(); zc!=Tg.end(); zc++)
      {
        gfan::ZMatrix ray = zc->extremeRays();
        for (int i=0; i<ray.getHeight(); i++)
        {
          gfan::ZVector negatedRay = gfan::Integer(-1)*ray[i];
          if (negatedRay.isPositive())
          {
            res->rtyp = BIGINTMAT_CMD;
            res->data = (void*) zVectorToBigintmat(ray[i]);
            return FALSE;
          }
        }
      }
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(gfan::ZVector(0));
      return FALSE;
    }
    WerrorS("negativeTropicalStartingPoint: ideal not principal");
    return TRUE;
  }
  WerrorS("negativeTropicalStartingPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN nonPositiveTropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    if (idSize(I)==1)
    {
      tropicalStrategy currentStrategy(I,currRing);
      poly g = I->m[0];
      std::set<gfan::ZCone> Tg = tropicalVariety(g,currRing,&currentStrategy);
      for (std::set<gfan::ZCone>::iterator zc=Tg.begin(); zc!=Tg.end(); zc++)
      {
        gfan::ZMatrix ray = zc->extremeRays();
        for (int i=0; i<ray.getHeight(); i++)
        {
          gfan::ZVector negatedRay = gfan::Integer(-1)*ray[i];
          if (negatedRay.isNonNegative())
          {
            res->rtyp = BIGINTMAT_CMD;
            res->data = (void*) zVectorToBigintmat(ray[i]);
            return FALSE;
          }
        }
      }
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(gfan::ZVector(0));
      return FALSE;
    }
    WerrorS("nonPositiveTropicalStartingPoint: ideal not principal");
    return TRUE;
  }
  WerrorS("nonPositiveTropicalStartingPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN tropicalStartingPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    tropicalStrategy currentStrategy(I,currRing);
    if (idSize(I)==1)
    {
      poly g = I->m[0];
      std::set<gfan::ZCone> Tg = tropicalVariety(g,currRing,&currentStrategy);
      if (Tg.empty())
      {
        res->rtyp = BIGINTMAT_CMD;
        res->data = (void*) zVectorToBigintmat(gfan::ZVector(0));
        return FALSE;
      }
      gfan::ZCone C = *(Tg.begin());
      gfan::ZMatrix rays = C.extremeRays();
      if (rays.getHeight()==0)
      {
        gfan::ZMatrix lin = C.generatorsOfLinealitySpace();
        res->rtyp = BIGINTMAT_CMD;
        res->data = (void*) zVectorToBigintmat(lin[0]);
        return FALSE;
      }
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(rays[0]);
      return FALSE;
    }
    gfan::ZCone C0 = currentStrategy.getHomogeneitySpace();
    if (C0.dimension()==currentStrategy.getExpectedDimension())
    {
      gfan::ZMatrix lin = C0.generatorsOfLinealitySpace();
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(lin[0]);
      return FALSE;
    }
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
    if (g)
    {
      p_GetExpV(g,expv,r);
      gfan::ZVector leadexp = intStar2ZVector(n,expv);
      for (pIter(g); g; pIter(g))
      {
        p_GetExpV(g,expv,r);
        equations.appendRow(leadexp-intStar2ZVector(n,expv));
      }
    }
  }
  omFreeSize(expv,(n+1)*sizeof(int));
  return gfan::ZCone(gfan::ZMatrix(0,n),equations);
}

/***
 * Computes a starting cone in the tropical variety.
 **/
groebnerCone tropicalStartingCone(const tropicalStrategy& currentStrategy)
{
  ring r = currentStrategy.getStartingRing();
  ideal I = currentStrategy.getStartingIdeal();
  currentStrategy.reduce(I,r);
  if (currentStrategy.isValuationTrivial())
  {
    // copy the data, so that it be deleted when passed to the loop
    // s <- r
    // inI <- I
    ring s = rCopy(r);
    int k = idSize(I); ideal inI = idInit(k);
    nMapFunc identityMap = n_SetMap(r->cf,s->cf);
    for (int i=0; i<k; i++)
      inI->m[i] = p_PermPoly(I->m[i],NULL,r,s,identityMap,NULL,0);

    // repeatedly computes a point in the tropical variety outside the lineality space,
    // take the initial ideal with respect to it
    // and check whether the dimension of its homogeneity space
    // equals the dimension of the tropical variety
    gfan::ZCone zc = linealitySpaceOfGroebnerFan(inI,s);
    gfan::ZVector startingPoint; groebnerCone ambientMaximalCone;
    if (zc.dimension()>=currentStrategy.getExpectedDimension())
    {
      // check whether the lineality space is contained in the tropical variety
      // i.e. whether the ideal does not contain a monomial
      poly mon = checkForMonomialViaSuddenSaturation(I,r);
      if (mon)
      {
        groebnerCone emptyCone = groebnerCone();
        p_Delete(&mon,r);
        id_Delete(&inI,s);
        return emptyCone;
      }
      groebnerCone startingCone(inI,inI,s,currentStrategy);
      id_Delete(&inI,s);
      return startingCone;
    }
    while (zc.dimension()<currentStrategy.getExpectedDimension())
    {
      // compute a point in the tropical variety outside the lineality space
      std::pair<gfan::ZVector,groebnerCone> startingData = tropicalStartingDataViaGroebnerFan(inI,s,currentStrategy);
      startingPoint = startingData.first;
      ambientMaximalCone = groebnerCone(startingData.second);

      id_Delete(&inI,s); rDelete(s);
      inI = ambientMaximalCone.getPolynomialIdeal();
      s = ambientMaximalCone.getPolynomialRing();

      // compute the initial ideal with respect to the weight
      inI = initial(inI,s,startingPoint);
      zc = linealitySpaceOfGroebnerFan(inI,s);
    }

    // once the dimension of the homogeneity space equals that of the tropical variety
    // we know that we have an initial ideal with respect to a weight
    // in the relative interior of a maximal cone in the tropical variety
    // from this we can read of the inequalities and equations

    // but before doing so, we must lift the generating set of inI
    // to a generating set of I
    ideal J = lift(I,r,inI,s); // todo: use computeLift from tropicalStrategy
    groebnerCone startingCone(J,inI,s,currentStrategy);
    id_Delete(&inI,s);
    id_Delete(&J,s);

    // assume(checkContainmentInTropicalVariety(startingCone));
    return startingCone;
  }
  else
  {
    // copy the data, so that it be deleted when passed to the loop
    // s <- r
    // inJ <- I
    ring s = rCopy(r);
    int k = idSize(I); ideal inJ = idInit(k);
    nMapFunc identityMap = n_SetMap(r->cf,s->cf);
    for (int i=0; i<k; i++)
      inJ->m[i] = p_PermPoly(I->m[i],NULL,r,s,identityMap,NULL,0);

    // and check whether the dimension of its homogeneity space
    // equals the dimension of the tropical variety
    gfan::ZCone zc = linealitySpaceOfGroebnerFan(inJ,s);
    if (zc.dimension()>=currentStrategy.getExpectedDimension())
    { // this shouldn't happen as trivial cases should be caught beforehand
      // this is the case that the tropical variety consists soely out of the lineality space
      poly mon = checkForMonomialViaSuddenSaturation(I,r);
      if (mon)
      {
        groebnerCone emptyCone = groebnerCone();
        p_Delete(&mon,r);
        return emptyCone;
      }
      groebnerCone startingCone(I,inJ,s,currentStrategy);
      id_Delete(&inJ,s);
      rDelete(s);
      return startingCone;
    }

    // compute a point in the tropical variety outside the lineality space
    // compute the initial ideal with respect to the weight
    std::pair<gfan::ZVector,groebnerCone> startingData = tropicalStartingDataViaGroebnerFan(inJ,s,currentStrategy);
    gfan::ZVector startingPoint = startingData.first;
    groebnerCone ambientMaximalCone = groebnerCone(startingData.second);
    id_Delete(&inJ,s); rDelete(s);
    inJ = ambientMaximalCone.getPolynomialIdeal();
    s = ambientMaximalCone.getPolynomialRing();
    inJ = initial(inJ,s,startingPoint);
    ideal inI = initial(I,r,startingPoint);
    zc = linealitySpaceOfGroebnerFan(inJ,s);

    // and check whether the dimension of its homogeneity space
    // equals the dimension of the tropical variety
    if (zc.dimension()==currentStrategy.getExpectedDimension())
    { // this case shouldn't happen as trivial cases should be caught beforehand
      // this is the case that the tropical variety has a one-codimensional lineality space
      ideal J = lift(I,r,inJ,s); // todo: use computeLift from tropicalStrategy
      groebnerCone startingCone(J,inJ,s,currentStrategy);
      id_Delete(&inJ,s);
      id_Delete(&J,s);
      return startingCone;
    }

    // from this point on, inJ contains the uniformizing parameter,
    // hence it contains a monomial if and only if its residue over the residue field does.
    // so we will switch to the residue field
    ring rShortcut = rCopy0(r);
    nKillChar(rShortcut->cf);
    rShortcut->cf = nCopyCoeff((currentStrategy.getShortcutRing())->cf);
    rComplete(rShortcut);
    rTest(rShortcut);
    k = idSize(inJ);
    ideal inJShortcut = idInit(k);
    nMapFunc takingResidues = n_SetMap(s->cf,rShortcut->cf);
    for (int i=0; i<k; i++)
      inJShortcut->m[i] = p_PermPoly(inJ->m[i],NULL,s,rShortcut,takingResidues,NULL,0);
    idSkipZeroes(inJShortcut);
    id_Delete(&inJ,s);

    // we are interested in a maximal cone of the tropical variety of inJShortcut
    // this basically equivalent to the case without valuation (or constant coefficient case)
    // except that our ideal is still only homogeneous in the later variables,
    // hence we set the optional parameter completelyHomogeneous as 'false'
    tropicalStrategy shortcutStrategy(inJShortcut,rShortcut,false);
    groebnerCone startingConeShortcut = tropicalStartingCone(shortcutStrategy);
    id_Delete(&inJShortcut,rShortcut); rDelete(rShortcut);

    // now we need to obtain the initial of the residue of inJ
    // with respect to a weight in the tropical cone,
    // and obtain the initial of inJ with respect to the same weight
    ring sShortcut = startingConeShortcut.getPolynomialRing();
    inJShortcut = startingConeShortcut.getPolynomialIdeal();
    gfan::ZCone zd = startingConeShortcut.getPolyhedralCone();
    gfan::ZVector interiorPoint = startingConeShortcut.getInteriorPoint();
    inJShortcut = initial(inJShortcut,sShortcut,interiorPoint);
    inI = initial(inI,r,interiorPoint);

    s = rCopy0(sShortcut); // s will be a ring over the valuation ring
    nKillChar(s->cf);      // with the same ordering as sShortcut
    s->cf = nCopyCoeff(r->cf);
    rComplete(s);
    rTest(s);

    k = idSize(inJShortcut); // inJ will be overwritten with initial of inJ
    inJ = idInit(k+1);
    inJ->m[0] = p_One(s);    // with respect to that weight
    identityMap = n_SetMap(r->cf,s->cf); // first element will obviously be p
    p_SetCoeff(inJ->m[0],identityMap(currentStrategy.getUniformizingParameter(),r->cf,s->cf),s);
    nMapFunc findingRepresentatives = n_SetMap(sShortcut->cf,s->cf);
    for (int i=0; i<k; i++)              // and then come the rest
      inJ->m[i+1] = p_PermPoly(inJShortcut->m[i],NULL,sShortcut,s,findingRepresentatives,NULL,0);

    ideal J = currentStrategy.computeLift(inJ,s,inI,I,r);
    // currentStrategy.reduce(J,s);
    groebnerCone startingCone(J,inJ,s,currentStrategy);
    id_Delete(&inJ,s);
    id_Delete(&J,s);
    rDelete(s);
    id_Delete(&inI,r);

    // assume(checkContainmentInTropicalVariety(startingCone));
    return startingCone;
  }
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
        tropicalStrategy currentStrategy(I,p,currRing);
        groebnerCone sigma = tropicalStartingCone(currentStrategy);
        gfan::ZCone* startingCone = new gfan::ZCone(sigma.getPolyhedralCone());
        res->rtyp = coneID;
        res->data = (char*) startingCone;
        return FALSE;
      }
    }
    else
    {
      if (v==NULL)
      {
        tropicalStrategy currentStrategy(I,currRing);
        groebnerCone sigma = tropicalStartingCone(currentStrategy);
        res->rtyp = coneID;
        res->data = (char*) new gfan::ZCone(sigma.getPolyhedralCone());
        return FALSE;
      }
    }
  }
  WerrorS("tropicalStartingCone: unexpected parameters");
  return TRUE;
}
