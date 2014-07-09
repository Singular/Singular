#include <gfanlib/gfanlib_matrix.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <gfanlib_exceptions.h>
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
 * Given a weight w and k more weights E[1], ..., E[h] stored as row vectors of a matrix E,
 + returns a ring whose ordering is weighted with respect to any
 * w+\varepsilon_1*E[1]+...+\varepsilon_k*E[h] for \varepsilon_i sufficiently small.
 * In particular, if E[1], ..., E[h] generate a vector space of dimension d,
 * this ordering can be used to compute a Groebner cone of dimension d containing w.
 **/
static ring genericlyWeightedOrdering(const ring r, const gfan::ZVector w, const gfan::ZMatrix E,
                                      const tropicalStrategy& currentStrategy)
{
  int n = r->N;
  int h = E.getHeight();

  /* create a copy s of r and delete its ordering */
  ring s = rCopy0(r);
  omFree(s->order);
  s->order  = (int*) omAlloc0((h+3)*sizeof(int));
  omFree(s->block0);
  s->block0 = (int*) omAlloc0((h+3)*sizeof(int));
  omFree(s->block1);
  s->block1 = (int*) omAlloc0((h+3)*sizeof(int));
  for (int j=0; s->wvhdl[j]; j++) omFree(s->wvhdl[j]);
  omFree(s->wvhdl);
  s->wvhdl  = (int**) omAlloc0((h+3)*sizeof(int*));

  /* construct a new ordering and keep an eye out for weight overflows */
  bool overflow;
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(w,overflow);
  for (int j=1; j<h; j++)
  {
    s->order[j] = ringorder_a;
    s->block0[j] = 1;
    s->block1[j] = n;
    s->wvhdl[j] = ZVectorToIntStar(currentStrategy.adjustWeightUnderHomogeneity(E[j-1],w),overflow);
  }
  s->order[h] = ringorder_wp;
  s->block0[h] = 1;
  s->block1[h] = n;
  s->wvhdl[h] = ZVectorToIntStar(currentStrategy.adjustWeightUnderHomogeneity(E[h-1],w),overflow);
  s->order[h+1] = ringorder_C;

  if (overflow)
    throw 0; //weightOverflow;

  /* complete the ring and return it */
  rComplete(s,1);
  return s;
}


/***
 * Given an ideal I which is homogeneous in the later variables of r,
 * whose tropicalization is, modulo its lineality space, a tropical Curve,
 * computes the said tropicalization.
 * At the end of the computation, I will contain a tropical basis.
 * If the dimension of the tropical variety is known beforehand,
 * it can be provided in order to speed up the computation.
 **/
std::set<gfan::ZCone> tropicalCurve(ideal I, const ring r, const tropicalStrategy currentStrategy)
{
  int k = idSize(I);

  /* Compute the common refinement over all tropical varieties
   * of the polynomials in the generating set */
  std::set<gfan::ZCone> C = tropicalVariety(I->m[0],r,currentStrategy);
  for (int i=1; i<k; i++)
    C = intersect(C,tropicalVariety(I->m[i],r,currentStrategy),currentStrategy.getDimensionOfIdeal());

  /* Cycle through all maximal cones of the refinement.
   * Pick a monomial ordering corresponding to a generic weight vector in it
   * and check if the initial ideal is monomial free, generic meaning
   * that it lies in a maximal Groebner cone in the maximal cone of the refinement.
   * If the initial ideal is not monomial free, compute a witness for the monomial
   * and compute the common refinement with its tropical variety.
   * If all initial ideals are monomial free, then we have our tropical curve */
  for (std::set<gfan::ZCone>::iterator zc=C.begin(); zc!=C.end(); zc++)
  {
    gfan::ZVector v = zc->getRelativeInteriorPoint();
    gfan::ZMatrix E = zc->generatorsOfSpan();

    ring s = genericlyWeightedOrdering(r,v,E,currentStrategy);
    nMapFunc nMap = n_SetMap(r->cf,s->cf);
    ideal Is = idInit(k);
    for (int j=0; j<k; j++)
      Is->m[j] = p_PermPoly(I->m[j],NULL,r,s,nMap,NULL,0);
    ideal inIs = initial(Is,s,E[E.getHeight()-1]);

    poly mons = checkForMonomialViaSuddenSaturation(inIs,s);
    if (mons)
    {
      poly gs = witness(mons,Is,inIs,s);
      C = intersect(C,tropicalVariety(gs,s,currentStrategy),currentStrategy.getDimensionOfIdeal());
      nMapFunc mMap = n_SetMap(s->cf,r->cf);
      poly gr = p_PermPoly(gs,NULL,s,r,mMap,NULL,0);
      idInsertPoly(I,gr);
      k++;
      p_Delete(&mons,s);
      p_Delete(&gs,s);
      zc = C.begin();
    }
    id_Delete(&Is,s);
    id_Delete(&inIs,s);
    rDelete(s);
  }
  return C;
}


std::set<gfan::ZVector> raysOfTropicalCurve(ideal I, const ring r, const tropicalStrategy& currentStrategy)
{
  std::set<gfan::ZCone> C = tropicalCurve(I,r,currentStrategy);
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
