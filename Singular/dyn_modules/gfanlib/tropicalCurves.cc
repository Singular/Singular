#include <gfanlib/gfanlib_matrix.h>
#include <gfanlib/gfanlib_zcone.h>
#include <polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <std_wrapper.h>
#include <containsMonomial.h>
#include <initial.h>
#include <witness.h>
#include <tropicalStrategy.h>
#include <tropicalVarietyOfPolynomials.h>
#include <tropicalCurves.h>
#include <set>
#ifndef NDEBUG
#include <bbfan.h>
#endif

/***
 * Given two sets of cones A,B and a dimensional bound d,
 * computes the intersections of all cones of A with all cones of B,
 * and throws away those of lower dimension than d.
 **/
static ZConesSortedByDimension intersect(const ZConesSortedByDimension &setA,
                                         const ZConesSortedByDimension &setB,
                                         int d=0)
{
  if (setA.empty())
    return setB;
  if (setB.empty())
    return setA;
  ZConesSortedByDimension setAB;
  for (ZConesSortedByDimension::iterator coneOfA=setA.begin(); coneOfA!=setA.end(); coneOfA++)
  {
    for (ZConesSortedByDimension::iterator coneOfB=setB.begin(); coneOfB!=setB.end(); coneOfB++)
    {
      gfan::ZCone coneOfIntersection = gfan::intersection(*coneOfA,*coneOfB);
      if (coneOfIntersection.dimension()>=d)
      {
        coneOfIntersection.canonicalize();
        setAB.insert(coneOfIntersection);
      }
    }
  }
  return setAB;
}

/***
 * Given a ring r, weights u, w, and a matrix E, returns a copy of r whose ordering is,
 * for any ideal homogeneous with respect to u, weighted with respect to u and
 * whose tiebreaker is genericly weighted with respect to v and E in the following sense:
 * the ordering "lies" on the affine space A running through v and spanned by the row vectors of E,
 * and it lies in a Groebner cone of dimension at least rank(E)=dim(A).
 **/
static ring genericlyWeightedOrdering(const ring r, const gfan::ZVector &u, const gfan::ZVector &w,
                                      const gfan::ZMatrix &W, const tropicalStrategy* currentStrategy)
{
  int n = rVar(r);
  int h = W.getHeight();

  /* create a copy s of r and delete its ordering */
  ring s = rCopy0(r);
  omFree(s->order);
  s->order  = (int*) omAlloc0((h+4)*sizeof(int));
  omFree(s->block0);
  s->block0 = (int*) omAlloc0((h+4)*sizeof(int));
  omFree(s->block1);
  s->block1 = (int*) omAlloc0((h+4)*sizeof(int));
  for (int j=0; s->wvhdl[j]; j++) omFree(s->wvhdl[j]);
  omFree(s->wvhdl);
  s->wvhdl  = (int**) omAlloc0((h+4)*sizeof(int*));

  /* construct a new ordering as describe above */
  bool overflow;
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  gfan::ZVector uAdjusted = currentStrategy->adjustWeightForHomogeneity(u);
  s->wvhdl[0] = ZVectorToIntStar(uAdjusted,overflow);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  gfan::ZVector wAdjusted = currentStrategy->adjustWeightUnderHomogeneity(w,uAdjusted);
  s->wvhdl[1] = ZVectorToIntStar(wAdjusted,overflow);
  for (int j=0; j<h-1; j++)
  {
    s->order[j+2] = ringorder_a;
    s->block0[j+2] = 1;
    s->block1[j+2] = n;
    wAdjusted = currentStrategy->adjustWeightUnderHomogeneity(W[j],uAdjusted);
    s->wvhdl[j+2] = ZVectorToIntStar(wAdjusted,overflow);
  }
  s->order[h+1] = ringorder_wp;
  s->block0[h+1] = 1;
  s->block1[h+1] = n;
  wAdjusted = currentStrategy->adjustWeightUnderHomogeneity(W[h-1],uAdjusted);
  s->wvhdl[h+1] = ZVectorToIntStar(wAdjusted,overflow);
  s->order[h+2] = ringorder_C;

  if (overflow)
    throw 0; //todo: use a proper custom exception

  /* complete the ring and return it */
  rComplete(s);
  rTest(s);
  return s;
}


/***
 * Let I be an ideal. Given a weight vector u in the relative interior
 * of a one-codimensional cone of the tropical variety of I and
 * the initial ideal inI with respect to it, computes the star of the tropical variety in u.
 **/
ZConesSortedByDimension tropicalStar(ideal inI, const ring r, const gfan::ZVector &u,
                                     const tropicalStrategy* currentStrategy)
{
  int k = idSize(inI);
  int d = currentStrategy->getExpectedDimension();

  /* Compute the common refinement over all tropical varieties
   * of the polynomials in the generating set */
  ZConesSortedByDimension C = tropicalVarietySortedByDimension(inI->m[0],r,currentStrategy);
  for (int i=1; i<k; i++)
    C = intersect(C,tropicalVarietySortedByDimension(inI->m[i],r,currentStrategy),d);

  /* Cycle through all maximal cones of the refinement.
   * Pick a monomial ordering corresponding to a generic weight vector in it
   * and check if the initial ideal is monomial free, generic meaning
   * that it lies in a maximal Groebner cone in the maximal cone of the refinement.
   * If the initial ideal is not monomial free, compute a witness for the monomial
   * and compute the common refinement with its tropical variety.
   * If all initial ideals are monomial free, then we have our tropical curve */
  // gfan::ZFan* zf = toFanStar(C);
  // std::cout << zf->toString(2+4+8+128) << std::endl;
  // delete zf;
  for (std::set<gfan::ZCone>::iterator zc=C.begin(); zc!=C.end();)
  {
    gfan::ZVector w = zc->getRelativeInteriorPoint();
    gfan::ZMatrix W = zc->generatorsOfSpan();
    // std::cout << zc->extremeRays() << std::endl;

    ring s = genericlyWeightedOrdering(r,u,w,W,currentStrategy);
    nMapFunc identity = n_SetMap(r->cf,s->cf);
    ideal inIs = idInit(k);
    for (int j=0; j<k; j++)
      inIs->m[j] = p_PermPoly(inI->m[j],NULL,r,s,identity,NULL,0);

    ideal inIsSTD = gfanlib_kStd_wrapper(inIs,s,isHomog);
    id_Delete(&inIs,s);
    ideal ininIs = initial(inIsSTD,s,w,W);

    std::pair<poly,int> mons = currentStrategy->checkInitialIdealForMonomial(ininIs,s);

    if (mons.first!=NULL)
    {
      poly gs;
      if (mons.second>=0)
        // cheap way out, ininIsSTD already contains a monomial in its generators
        gs = inIsSTD->m[mons.second];
      else
        // compute witness
        gs = witness(mons.first,inIsSTD,ininIs,s);

      C = intersect(C,tropicalVarietySortedByDimension(gs,s,currentStrategy),d);
      nMapFunc mMap = n_SetMap(s->cf,r->cf);
      poly gr = p_PermPoly(gs,NULL,s,r,mMap,NULL,0);
      idInsertPoly(inI,gr);
      k++;

      if (mons.second<0)
      {
        // if necessary, cleanup mons and gs
        p_Delete(&mons.first,s);
        p_Delete(&gs,s);
      }
      // cleanup rest, reset zc
      id_Delete(&inIsSTD,s);
      id_Delete(&ininIs,s);
      rDelete(s);
      zc = C.begin();
    }
    else
    {
      // cleanup remaining data of first stage
      id_Delete(&inIsSTD,s);
      id_Delete(&ininIs,s);
      rDelete(s);

      gfan::ZVector wNeg = currentStrategy->negateWeight(w);
      if (zc->contains(wNeg))
      {
        s = genericlyWeightedOrdering(r,u,wNeg,W,currentStrategy);
        identity = n_SetMap(r->cf,s->cf);
        inIs = idInit(k);
        for (int j=0; j<k; j++)
          inIs->m[j] = p_PermPoly(inI->m[j],NULL,r,s,identity,NULL,0);

        inIsSTD = gfanlib_kStd_wrapper(inIs,s,isHomog);
        id_Delete(&inIs,s);
        ininIs = initial(inIsSTD,s,wNeg,W);

        mons = currentStrategy->checkInitialIdealForMonomial(ininIs,s);
        if (mons.first!=NULL)
        {
          poly gs;
          if (mons.second>=0)
            // cheap way out, ininIsSTD already contains a monomial in its generators
            gs = inIsSTD->m[mons.second];
          else
            // compute witness
            gs = witness(mons.first,inIsSTD,ininIs,s);

          C = intersect(C,tropicalVarietySortedByDimension(gs,s,currentStrategy),d);
          nMapFunc mMap = n_SetMap(s->cf,r->cf);
          poly gr = p_PermPoly(gs,NULL,s,r,mMap,NULL,0);
          idInsertPoly(inI,gr);
          k++;

          if (mons.second<0)
          {
            // if necessary, cleanup mons and gs
            p_Delete(&mons.first,s);
            p_Delete(&gs,s);
          }
          // reset zc
          zc = C.begin();
        }
        else
          zc++;
        // cleanup remaining data of second stage
        id_Delete(&inIsSTD,s);
        id_Delete(&ininIs,s);
        rDelete(s);
      }
      else
        zc++;
    }
  }
  return C;
}


gfan::ZMatrix raysOfTropicalStar(ideal I, const ring r, const gfan::ZVector &u, const tropicalStrategy* currentStrategy)
{
  ZConesSortedByDimension C = tropicalStar(I,r,u,currentStrategy);
  // gfan::ZFan* zf = toFanStar(C);
  // std::cout << zf->toString();
  // delete zf;
  gfan::ZMatrix raysOfC(0,u.size());
  if (!currentStrategy->restrictToLowerHalfSpace())
  {
    for (ZConesSortedByDimension::iterator zc=C.begin(); zc!=C.end(); zc++)
    {
      assume(zc->dimensionOfLinealitySpace()+1 >= zc->dimension());
      if (zc->dimensionOfLinealitySpace()+1 >= zc->dimension())
        raysOfC.appendRow(zc->getRelativeInteriorPoint());
      else
      {
        gfan::ZVector interiorPoint = zc->getRelativeInteriorPoint();
        if (!currentStrategy->homogeneitySpaceContains(interiorPoint))
        {
          raysOfC.appendRow(interiorPoint);
          raysOfC.appendRow(currentStrategy->negateWeight(interiorPoint));
        }
        else
        {
          gfan::ZMatrix zm = zc->generatorsOfLinealitySpace();
          for (int i=0; i<zm.getHeight(); i++)
          {
            gfan::ZVector point = zm[i];
            if (currentStrategy->homogeneitySpaceContains(point))
            {
              raysOfC.appendRow(point);
              raysOfC.appendRow(currentStrategy->negateWeight(point));
              break;
            }
          }
        }
      }
    }
  }
  else
  {
    for (ZConesSortedByDimension::iterator zc=C.begin(); zc!=C.end(); zc++)
    {
      assume(zc->dimensionOfLinealitySpace()+2 >= zc->dimension());
      if (zc->dimensionOfLinealitySpace()+2 == zc->dimension())
        raysOfC.appendRow(zc->getRelativeInteriorPoint());
      else
      {
        gfan::ZVector interiorPoint = zc->getRelativeInteriorPoint();
        if (!currentStrategy->homogeneitySpaceContains(interiorPoint))
        {
          raysOfC.appendRow(interiorPoint);
          raysOfC.appendRow(currentStrategy->negateWeight(interiorPoint));
        }
        else
        {
          gfan::ZMatrix zm = zc->generatorsOfLinealitySpace();
          for (int i=0; i<zm.getHeight(); i++)
          {
            gfan::ZVector point = zm[i];
            if (currentStrategy->homogeneitySpaceContains(point))
            {
              raysOfC.appendRow(point);
              raysOfC.appendRow(currentStrategy->negateWeight(point));
              break;
            }
          }
        }
      }
    }
  }
  return raysOfC;
}


/***
 * Computes the tropical curve of an x-homogeneous ideal I
 * which is weighted homogeneous with respect to weight w in ring r
 **/
#ifndef NDEBUG
BOOLEAN tropicalStarDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==BIGINTMAT_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      ideal inI = (ideal) u->CopyD();
      bigintmat* u = (bigintmat*) v->CopyD();
      tropicalStrategy currentCase(inI,currRing);
      gfan::ZVector* v = bigintmatToZVector(u);
      ZConesSortedByDimension C = tropicalStar(inI,currRing,*v,&currentCase);
      id_Delete(&inI,currRing);
      delete u;
      delete v;
      res->rtyp = NONE;
      res->data = NULL;
      // res->rtyp = fanID;
      // res->data = (char*) toFanStar(C);
      return FALSE;
    }
  }
  WerrorS("tropicalStarDebug: unexpected parameters");
  return TRUE;
}
#endif
