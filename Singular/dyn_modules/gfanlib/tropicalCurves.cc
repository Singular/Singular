#include <gfanlib/gfanlib_matrix.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <gfanlib_exceptions.h>
#include <containsMonomial.h>
#include <initial.h>
#include <witness.h>
#include <tropicalStrategy.h>
#include <tropicalCurves.h>
#include <set>
#ifndef NDEBUG
#include <bbfan.h>
#endif

std::set<gfan::ZCone> intersect(const std::set<gfan::ZCone> setA, const std::set<gfan::ZCone> setB, int d)
{
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
 * Given a relative interior point w of a cone in the weight space
 * and generators E of its span, returns a ring with an ordering that coincides
 * with a weighted ordering on the ideal with respect to a weight in the cone
 * which does not lie on a Groebner cone of lower dimension than that of the cone.
 **/
static ring ringWithGenericlyWeightedOrdering(const ring r,
                                              const gfan::ZVector w,
                                              const gfan::ZMatrix E,
                                              const tropicalStrategy currentCase)
{
  gfan::ZVector (*adjustWeight)(gfan::ZVector v, gfan::ZVector w);
  adjustWeight = currentCase.adjustWeightUnderHomogeneity;

  int n = r->N;
  int h = E.getHeight();

  // create a copy of r and delete its old ordering
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

  // construct a new ordering and keep an eye out for weight overflows
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
    s->wvhdl[j] = ZVectorToIntStar(adjustWeight(E[j-1],w),overflow);
  }
  s->order[h] = ringorder_wp;
  s->block0[h] = 1;
  s->block1[h] = n;
  s->wvhdl[h] = ZVectorToIntStar(adjustWeight(E[h-1],w),overflow);
  s->order[h+1] = ringorder_C;

  rComplete(s,1);
  // Return s.
  if (overflow)
    throw 0; //weightOverflow;
  return s;
}


/***
 * Computes the tropical curve of an x-homogeneous ideal I in ring r
 **/
std::set<gfan::ZCone> tropicalCurve(const ideal I, const ring r, const int d,
                         const tropicalStrategy currentCase)
{
  int k = idSize(I);
  std::set<gfan::ZCone> (*tropicalVariety)(const poly &g, const ring &r);
  tropicalVariety = currentCase.tropicalVarietyOfPolynomial;
  // Compute the common refinement of the tropical varieties
  // of the generating set
  std::set<gfan::ZCone> C = tropicalVariety(I->m[0],r);
  for (int i=1; i<k; i++)
    C = intersect(C,tropicalVariety(I->m[i],r),d);

  // cycle through all maximal cones of the refinement
  // pick a monomial ordering corresponding to a generic weight vector in it
  // check if the initial ideal is monomial free
  // if the initial ideal is not monomial free, pick a witness for the monomial
  // and intersect with its corresponding tropical variety
  // if all initial ideals are monomial free, the we have our tropical curve
  poly mon = NULL;
  do
  {
    for (std::set<gfan::ZCone>::iterator zc=C.begin(); zc!=C.end(); zc++)
    {
      gfan::ZVector v = zc->getRelativeInteriorPoint();
      gfan::ZMatrix E = zc->generatorsOfSpan();

      // std::cout << "interiorPoint: " << v << std::endl;
      // std::cout << "generators of span: " << E << std::endl;
      // ideal inIr = initial(I,r,E[E.getHeight()-1]);
      ring s = ringWithGenericlyWeightedOrdering(r,v,E,currentCase);
      nMapFunc nMap = n_SetMap(r->cf,s->cf);
      ideal Is = idInit(k);
      for (int j=0; j<k; j++)
      {
        Is->m[j] = p_PermPoly(I->m[j],NULL,r,s,nMap,NULL,0);
        p_Test(Is->m[j],s);
      }
      ideal inIs = initial(Is,s,E[E.getHeight()-1]);
      id_Delete(&Is,s);
      // id_Delete(&inIr,r);

      mon = checkForMonomialViaSuddenSaturation(inIs,s);
      if (mon)
      {
        ideal Is = idInit(k);
        for (int j=0; j<k; j++)
        {
          Is->m[j] = p_PermPoly(I->m[j],NULL,r,s,nMap,NULL,0);
          p_Test(Is->m[j],s);
        }
        poly g = witness(mon,Is,inIs,s);
        C = intersect(C,tropicalVariety(g,s),d);
        p_Delete(&mon,s);
        p_Delete(&g,s);
        id_Delete(&inIs,s);
        id_Delete(&Is,s);
        rDelete(s);
        break;
      }
      id_Delete(&inIs,s);
      rDelete(s);
    }
  } while (mon);
  return C;
}
/***
 * Computes the tropical curve of an x-homogeneous ideal I
 * which is weighted homogeneous with respect to weight w in ring r
 **/

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  int d = (long)(int) v->CopyD();
  tropicalStrategy currentCase = nonValuedCase;
  std::set<gfan::ZCone> C = tropicalCurve(I,currRing,d,currentCase);
  id_Delete(&I,currRing);
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  res->rtyp = fanID;
  res->data = (char*) toFanStar(C);
  return FALSE;
}
BOOLEAN tropicalCurve1(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  int d = (long)(int) v->CopyD();
  tropicalStrategy currentCase = valuedCase;
  std::set<gfan::ZCone> C = tropicalCurve(I,currRing,d,currentCase);
  id_Delete(&I,currRing);
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  res->rtyp = fanID;
  res->data = (char*) toFanStar(C);
  return FALSE;
}
#endif
