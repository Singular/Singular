#include "kernel/GBEngine/kstd1.h"
#include "gfanlib/gfanlib_vector.h"
#include "callgfanlib_conversion.h"
#include "singularWishlist.h"
#include "initial.h"
#include "lift.h"

/***
 * Given a Groebner basis I of an ideal in r, an interior Point
 * on a face of the maximal Groebner cone associated to the ordering on r,
 * and a vector pointing outwards from it,
 * returns a pair (Is,s) such that:
 *  (1) s is the same mathematical ring as r, but with a new ordering such that
 *        the interior point lies on the intersection of both maximal Groebner cones
 *  (2) Is is a Groebner basis of the same ideal with respect to the ordering on s
 **/
std::pair<ideal,ring> flip(const ideal I, const ring r,
                           const gfan::ZVector interiorPoint,
                           const gfan::ZVector facetNormal,
                           const gfan::ZVector adjustedInteriorPoint,
                           const gfan::ZVector adjustedFacetNormal)
{
  /* create a ring with weighted ordering  */
  bool ok;
  ring sAdjusted = rCopy0(r,FALSE,FALSE);
  int n = rVar(sAdjusted);
  sAdjusted->order = (rRingOrder_t*) omAlloc0(5*sizeof(rRingOrder_t));
  sAdjusted->block0 = (int*) omAlloc0(5*sizeof(int));
  sAdjusted->block1 = (int*) omAlloc0(5*sizeof(int));
  sAdjusted->wvhdl = (int**) omAlloc0(5*sizeof(int**));
  sAdjusted->order[0] = ringorder_a;
  sAdjusted->block0[0] = 1;
  sAdjusted->block1[0] = n;
  sAdjusted->wvhdl[0] = ZVectorToIntStar(adjustedInteriorPoint,ok);
  sAdjusted->order[1] = ringorder_a;
  sAdjusted->block0[1] = 1;
  sAdjusted->block1[1] = n;
  sAdjusted->wvhdl[1] = ZVectorToIntStar(adjustedFacetNormal,ok);
  sAdjusted->order[2] = ringorder_lp;
  sAdjusted->block0[2] = 1;
  sAdjusted->block1[2] = n;
  sAdjusted->wvhdl[2] = ZVectorToIntStar(adjustedFacetNormal,ok);
  sAdjusted->order[3] = ringorder_C;
  rComplete(sAdjusted);
  rTest(sAdjusted);
  nMapFunc identity = n_SetMap(r->cf,sAdjusted->cf);

  /* compute initial ideal and map it to the new ordering */
  ideal inIr = initial(I,r,interiorPoint);
  int k = IDELEMS(I); ideal inIsAdjusted = idInit(k);
  for (int i=0; i<k; i++)
  {
    if (inIr->m[i]!=NULL)
    {
      inIsAdjusted->m[i] = p_PermPoly(inIr->m[i],NULL,r,sAdjusted,identity,NULL,0);
    }
  }
  id_Delete(&inIr,r);

  /* compute Groebner basis of the initial ideal  */
  intvec* nullVector = NULL;
  ring origin = currRing;
  rChangeCurrRing(sAdjusted);
  ideal inIsAdjustedGB = kStd(inIsAdjusted,currRing->qideal,testHomog,&nullVector);
  ideal IsAdjustedGB = lift(I,r,inIsAdjustedGB,sAdjusted);
  id_Delete(&inIsAdjusted,sAdjusted);
  id_Delete(&inIsAdjustedGB,sAdjusted);

  ring s = rCopy0(r,FALSE,FALSE);
  n = rVar(s);
  s->order = (rRingOrder_t*) omAlloc0(5*sizeof(rRingOrder_t));
  s->block0 = (int*) omAlloc0(5*sizeof(int));
  s->block1 = (int*) omAlloc0(5*sizeof(int));
  s->wvhdl = (int**) omAlloc0(5*sizeof(int**));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(interiorPoint,ok);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(facetNormal,ok);
  s->order[2] = ringorder_lp;
  s->block0[2] = 1;
  s->block1[2] = n;
  s->order[3] = ringorder_C;
  rComplete(s);
  rTest(s);
  identity = n_SetMap(sAdjusted->cf,s->cf);
  k = IDELEMS(IsAdjustedGB); ideal IsGB = idInit(k);
  for (int i=0; i<k; i++)
  {
    if (IsAdjustedGB->m[i]!=NULL)
    {
      IsGB->m[i] = p_PermPoly(IsAdjustedGB->m[i],NULL,sAdjusted,s,identity,NULL,0);
    }
  }
  id_Delete(&IsAdjustedGB,sAdjusted);
  rDelete(sAdjusted);
  rChangeCurrRing(origin);

  /* lift the Groebner basis of the initial ideal
   * to a Groebner basis of the original ideal,
   * the currRingChange is solely for sake of performance */

  return std::make_pair(IsGB,s);
}
