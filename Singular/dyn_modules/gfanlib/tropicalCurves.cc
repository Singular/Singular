#include <gfanlib/gfanlib_matrix.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <gfanlib_exceptions.h>
#include <std_wrapper.h>
#include <containsMonomial.h>
#include <initial.h>
#include <witness.h>
#include <tropicalStrategy.h>
#include <tropicalVarietyOfPolynomials.h>
#include <set>
#ifndef NDEBUG
#include <bbfan.h>
#endif

/***
 * Given two sets of cones A,B and a dimensional bound d,
 * computes the intersections of all cones of A with all cones of B,
 * and throws away those of lower dimension than d.
 **/
static std::set<gfan::ZCone> intersect(const std::set<gfan::ZCone> setA,
                                       const std::set<gfan::ZCone> setB,
                                       int d=0)
{
  if (setA.empty())
    return setB;
  if (setB.empty())
    return setA;
  std::set<gfan::ZCone> setAB;
  for (std::set<gfan::ZCone>::iterator coneOfA=setA.begin(); coneOfA!=setA.end(); coneOfA++)
  {
    for (std::set<gfan::ZCone>::iterator coneOfB=setB.begin(); coneOfB!=setB.end(); coneOfB++)
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
static ring genericlyWeightedOrdering(const ring r, const gfan::ZVector u, const gfan::ZVector w,
                                      const gfan::ZMatrix W, const tropicalStrategy& currentStrategy)
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
  gfan::ZVector uAdjusted = currentStrategy.adjustWeightForHomogeneity(u);
  s->wvhdl[0] = ZVectorToIntStar(uAdjusted,overflow);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  gfan::ZVector wAdjusted = currentStrategy.adjustWeightUnderHomogeneity(w,uAdjusted);
  s->wvhdl[1] = ZVectorToIntStar(wAdjusted,overflow);
  for (int j=0; j<h-1; j++)
  {
    s->order[j+2] = ringorder_a;
    s->block0[j+2] = 1;
    s->block1[j+2] = n;
    wAdjusted = currentStrategy.adjustWeightUnderHomogeneity(W[j],uAdjusted);
    s->wvhdl[j+2] = ZVectorToIntStar(wAdjusted,overflow);
  }
  s->order[h+1] = ringorder_wp;
  s->block0[h+1] = 1;
  s->block1[h+1] = n;
  wAdjusted = currentStrategy.adjustWeightUnderHomogeneity(W[h-1],uAdjusted);
  s->wvhdl[h+1] = ZVectorToIntStar(wAdjusted,overflow);
  s->order[h+2] = ringorder_C;

  if (overflow)
    throw 0; //weightOverflow;

  /* complete the ring and return it */
  rComplete(s,1);
  return s;
}


/***
 * Let I be an ideal. Given a weight vector u in the relative interior
 * of a one-codimensional cone of the tropical variety of I and
 * the initial ideal inI with respect to it, computes the star of the tropical variety in u.
 **/
std::set<gfan::ZCone> tropicalStar(ideal inI, const ring r, const gfan::ZVector u,
                                   const tropicalStrategy currentStrategy)
{
  int k = idSize(inI);
  int d = currentStrategy.getDimensionOfIdeal();

  /* Compute the common refinement over all tropical varieties
   * of the polynomials in the generating set */
  std::set<gfan::ZCone> C = tropicalVariety(inI->m[0],r,currentStrategy);
  for (int i=1; i<k; i++)
    C = intersect(C,tropicalVariety(inI->m[i],r,currentStrategy),d);

  /* Cycle through all maximal cones of the refinement.
   * Pick a monomial ordering corresponding to a generic weight vector in it
   * and check if the initial ideal is monomial free, generic meaning
   * that it lies in a maximal Groebner cone in the maximal cone of the refinement.
   * If the initial ideal is not monomial free, compute a witness for the monomial
   * and compute the common refinement with its tropical variety.
   * If all initial ideals are monomial free, then we have our tropical curve */
  for (std::set<gfan::ZCone>::iterator zc=C.begin(); zc!=C.end(); zc++)
  {
    gfan::ZVector w = zc->getRelativeInteriorPoint();
    gfan::ZMatrix W = zc->generatorsOfSpan();

    ring s = genericlyWeightedOrdering(r,u,w,W,currentStrategy);
    nMapFunc identity = n_SetMap(r->cf,s->cf);
    ideal inIs = idInit(k);
    for (int j=0; j<k; j++)
      inIs->m[j] = p_PermPoly(inI->m[j],NULL,r,s,identity,NULL,0);

    inIs = gfanlib_kStd_wrapper(inIs,s,isHomog);
    ideal ininIs = initial(inIs,s,W[W.getHeight()-1]);

    poly mons = checkForMonomialViaSuddenSaturation(inIs,s);
    if (mons)
    {
      poly gs = witness(mons,inIs,ininIs,s);
      C = intersect(C,tropicalVariety(gs,s,currentStrategy),d);
      nMapFunc mMap = n_SetMap(s->cf,r->cf);
      poly gr = p_PermPoly(gs,NULL,s,r,mMap,NULL,0);
      idInsertPoly(inI,gr);
      k++;
      p_Delete(&mons,s);
      p_Delete(&gs,s);
      zc = C.begin();
    }
    id_Delete(&inIs,s);
    id_Delete(&ininIs,s);
    rDelete(s);
  }
  return C;
}


std::set<gfan::ZVector> raysOfTropicalStar(ideal I, const ring r, const gfan::ZVector u, const tropicalStrategy& currentStrategy)
{
  std::set<gfan::ZCone> C = tropicalStar(I,r,u,currentStrategy);
  std::set<gfan::ZVector> raysOfC;
  for (std::set<gfan::ZCone>::iterator zc=C.begin(); zc!=C.end(); zc++)
  {
    assume(zc->dimensionOfLinealitySpace()+1 == zc->dimension());
    raysOfC.insert(zc->semiGroupGeneratorOfRay());
  }
  return raysOfC;
}


/***
 * Computes the tropical curve of an x-homogeneous ideal I
 * which is weighted homogeneous with respect to weight w in ring r
 **/
#ifndef NDEBUG
// BOOLEAN tropicalCurve0(leftv res, leftv args)
// {
//   leftv u = args;
//   ideal I = (ideal) u->CopyD();
//   leftv v = u->next;
//   int d = (int)(long) v->CopyD();
//   tropicalStrategy currentCase = nonValuedCase;
//   std::set<gfan::ZCone> C = tropicalCurve(I,currRing,d,currentCase);
//   id_Delete(&I,currRing);
//   omUpdateInfo();
//   Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
//   res->rtyp = fanID;
//   res->data = (char*) toFanStar(C);
//   return FALSE;
// }
// BOOLEAN tropicalCurve1(leftv res, leftv args)
// {
//   leftv u = args;
//   ideal I = (ideal) u->CopyD();
//   leftv v = u->next;
//   int d = (int)(long) v->CopyD();
//   tropicalStrategy currentCase = valuedCase;
//   std::set<gfan::ZCone> C = tropicalCurve(I,currRing,d,currentCase);
//   id_Delete(&I,currRing);
//   omUpdateInfo();
//   Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
//   res->rtyp = fanID;
//   res->data = (char*) toFanStar(C);
//   return FALSE;
// }
#endif
