#include <utility>
#include <kernel/kstd1.h>
#include <gfanlib/gfanlib_vector.h>
#include <callgfanlib_conversion.h>
#include <initial.h>
#include <lift.h>
#include <tropicalStrategy.h>


static void deleteOrdering(ring r)
{
  if (r->order != NULL)
  {
    int i=rBlocks(r);
    assume(r->block0 != NULL && r->block1 != NULL && r->wvhdl != NULL);
    /* delete order */
    omFreeSize((ADDRESS)r->order,i*sizeof(int));
    omFreeSize((ADDRESS)r->block0,i*sizeof(int));
    omFreeSize((ADDRESS)r->block1,i*sizeof(int));
    /* delete weights */
    for (int j=0; j<i; j++)
      if (r->wvhdl[j]!=NULL)
        omFree(r->wvhdl[j]);
    omFreeSize((ADDRESS)r->wvhdl,i*sizeof(int *));
  }
  else
    assume(r->block0 == NULL && r->block1 == NULL && r->wvhdl == NULL);
  return;
}

/***
 * Given a Groebner basis I of an ideal in r, an interior Point
 * on a face of the maximal Groebner cone associated to the ordering on r,
 * and a vector pointing outwards from it,
 * returns a pair (Is,s) such that:
 *  (1) s is the same mathematical ring as r, but with a new ordering such that
 *        the interior point lies on the intersection of both maximal Groebner cones
 *  (2) Is is a Groebner basis of the same ideal with respect to the ordering on s
 **/
std::pair<ideal,ring> flip(const ideal I, const ring r, const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal, const tropicalStrategy& currentStrategy)
{
  /* read out appropiate functions for adjusting weights and adjust themm */
  gfan::ZVector adjustedInteriorPoint = currentStrategy.adjustWeightForHomogeneity(interiorPoint);
  gfan::ZVector adjustedFacetNormal = currentStrategy.adjustWeightUnderHomogeneity(facetNormal,adjustedInteriorPoint);

  /* create a ring with weighted ordering  */
  bool ok;
  ring sAdjusted = rCopy0(r);
  int n = rVar(sAdjusted);
  deleteOrdering(sAdjusted);
  sAdjusted->order = (int*) omAlloc0(4*sizeof(int));
  sAdjusted->block0 = (int*) omAlloc0(4*sizeof(int));
  sAdjusted->block1 = (int*) omAlloc0(4*sizeof(int));
  sAdjusted->wvhdl = (int**) omAlloc0(4*sizeof(int**));
  sAdjusted->order[0] = ringorder_a;
  sAdjusted->block0[0] = 1;
  sAdjusted->block1[0] = n;
  sAdjusted->wvhdl[0] = ZVectorToIntStar(adjustedInteriorPoint,ok);
  sAdjusted->order[1] = ringorder_wp;
  sAdjusted->block0[1] = 1;
  sAdjusted->block1[1] = n;
  sAdjusted->wvhdl[1] = ZVectorToIntStar(adjustedFacetNormal,ok);
  sAdjusted->order[2] = ringorder_C;
  rComplete(sAdjusted,1);
  nMapFunc identity = n_SetMap(r->cf,sAdjusted->cf);

  /* compute initial ideal and map it to the new ordering */
  ideal inIr = initial(I,r,interiorPoint);
  int k = idSize(I); ideal inIsAdjusted = idInit(k);
  for (int i=0; i<k; i++)
    inIsAdjusted->m[i] = p_PermPoly(inIr->m[i],NULL,r,sAdjusted,identity,NULL,0);
  id_Delete(&inIr,r);

  /* compute Groebner basis of the initial ideal  */
  intvec* nullVector = NULL;
  ring origin = currRing;
  rChangeCurrRing(sAdjusted);
  ideal inIsAdjustedGB = kStd(inIsAdjusted,currQuotient,testHomog,&nullVector);
  id_Delete(&inIsAdjusted,sAdjusted);
  ideal IsAdjustedGB = lift(I,r,inIsAdjustedGB,sAdjusted);
  id_Delete(&inIsAdjustedGB,sAdjusted);

  ring s = rCopy0(r);
  n = rVar(s);
  deleteOrdering(s);
  s->order = (int*) omAlloc0(5*sizeof(int));
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
  s->order[2] = ringorder_dp;
  s->block0[2] = 1;
  s->block1[2] = n;
  s->order[3] = ringorder_C;
  rComplete(s,1);
  identity = n_SetMap(sAdjusted->cf,s->cf);
  k = idSize(IsAdjustedGB); ideal IsGB = idInit(k);
  for (int i=0; i<k; i++)
    IsGB->m[i] = p_PermPoly(IsAdjustedGB->m[i],NULL,sAdjusted,s,identity,NULL,0);
  id_Delete(&IsAdjustedGB,sAdjusted);
  rDelete(sAdjusted);
  rChangeCurrRing(origin);

  /* lift the Groebner basis of the initial ideal
   * to a Groebner basis of the original ideal,
   * the currRingChange is solely for sake of performance */

  return std::make_pair(IsGB,s);
}
