#include <bbcone.h>
#include <containsMonomial.h>
#include <tropical.h>
#include <initial.h>

/***
 * Given a general ring r with any ordering,
 * changes the ordering to a(v),ws(-w)
 **/
bool changetoAWSRing(ring r, gfan::ZVector v, gfan::ZVector w)
{
  omFree(r->order);
  r->order  = (int*) omAlloc0(4*sizeof(int));
  omFree(r->block0);
  r->block0 = (int*) omAlloc0(4*sizeof(int));
  omFree(r->block1);
  r->block1 = (int*) omAlloc0(4*sizeof(int));
  for (int i=0; r->wvhdl[i]; i++)
  { omFree(r->wvhdl[i]); }
  omFree(r->wvhdl);
  r->wvhdl  = (int**) omAlloc0(4*sizeof(int*));

  bool ok = false;
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = r->N;
  r->wvhdl[0]  = ZVectorToIntStar(v,ok);
  r->order[1]  = ringorder_ws;
  r->block0[1] = 1;
  r->block1[1] = r->N;
  r->wvhdl[1]  = ZVectorToIntStar(w,ok);
  r->order[2]=ringorder_C;
  return ok;
}


/***
 * Given a ring with ordering a(v'),ws(w'),
 * changes the weights to v,w
 **/
bool changeAWSWeights(ring r, gfan::ZVector v, gfan::ZVector w)
{
  omFree(r->wvhdl[0]);
  omFree(r->wvhdl[1]);
  bool ok = false;
  r->wvhdl[0]  = ZVectorToIntStar(v,ok);
  r->wvhdl[1]  = ZVectorToIntStar(w,ok);
  return ok;
}


gfan::ZVector* tropicalStartingPoint(const gfan::ZCone &zc, const ideal &I, const ring r)
{
  gfan::ZMatrix ws=zc.extremeRays();
  for (int i=0; i<ws.getHeight(); i++)
  {
    ideal inI = initial(I,currRing,ws[i]);
    poly s = checkForMonomialViaSuddenSaturation(inI);
    id_Delete(&inI,r);
    if (s == NULL)
      return new gfan::ZVector(ws[i]);
  }
  return NULL;
}


gfan::ZVector* tropicalStartingPoint(ideal &I, ring r)
{
  gfan::ZCone* zc = maximalGroebnerCone(currRing, I);
  gfan::ZVector* w = tropicalStartingPoint(zc,I,r);
  while (w==NULL)
  {
    w = tropicalStartingPoint(zc,I,r);
  }
  return w;
}


gfan::ZCone* tropicalStartingCone(const ideal &I, ring r)
{
  gfan::ZVector* w = tropicalStartingPoint(I,r);
  return groebnerCone(I,r,w);
}
