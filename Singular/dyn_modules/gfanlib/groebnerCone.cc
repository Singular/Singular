#include <kernel/kstd1.h>
#include <kernel/ideals.h>
#include <Singular/ipid.h>

#include <libpolys/polys/monomials/p_polys.h>
#include <libpolys/polys/monomials/ring.h>
#include <libpolys/polys/prCopy.h>

#include <gfanlib/gfanlib.h>

#include <callgfanlib_conversion.h>
#include <groebnerCone.h>
#include <initial.h>

/***
 * Computes the Groebner cone of a polynomial g in ring r containing w relatively.
 * Assumes that r has a weighted ordering with weight in the said Groebner cone.
 **/
gfan::ZCone sloppyGroebnerCone(const poly g, const ring r, const gfan::ZVector w)
{
  int n = r->N;
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);

  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  p_GetExpV(g,expv,r);
  gfan::ZVector leadexp = intStar2ZVector(n,expv);
  long d = wDeg(g,r,w);

  poly h=g->next;
  for (; h && wDeg(h,r,w)==d; pIter(h))
  {
    p_GetExpV(h,expv,r);
    equations.appendRow(leadexp-intStar2ZVector(n,expv));
  }

  for (; h; pIter(h))
  {
    p_GetExpV(h,expv,r);
    inequalities.appendRow(leadexp-intStar2ZVector(n,expv));
  }

  omFreeSize(expv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,equations);
}

/***
 * Computes the Groebner cone of an ideal I in ring r containing w relatively.
 * Assumes that r has a weighted ordering with weight in the said Groebner cone.
 **/
gfan::ZCone sloppyGroebnerCone(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I);
  gfan::ZCone zc = gfan::ZCone(r->N);
  for (int i=0; i<k; i++)
    zc = intersection(zc,sloppyGroebnerCone(I->m[i],r,w));
  return zc;
}

/***
 * Computes the Groebner cone of a polynomial g in ring r containing w relatively.
 **/
gfan::ZCone groebnerCone(const poly g, const ring r, const gfan::ZVector w)
{
  int n = r->N;
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);

  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  p_GetExpV(g,expv,r);
  gfan::ZVector leadexp = intStar2ZVector(n,expv);
  long d = wDeg(g,r,w);

  for (poly h=g->next; h; pIter(h))
  {
    p_GetExpV(h,expv,r);
    if (wDeg(h,r,w)<d)
      inequalities.appendRow(leadexp-intStar2ZVector(n,expv));
    else
      equations.appendRow(leadexp-intStar2ZVector(n,expv));
  }

  omFreeSize(expv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,equations);
}

/***
 * Computes the Groebner cone of an ideal I in ring r containing w relatively.
 * Assumes that r has a weighted ordering with weight in the said Groebner cone.
 **/
gfan::ZCone groebnerCone(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I);
  gfan::ZCone zc = gfan::ZCone(r->N);
  for (int i=0; i<k; i++)
    zc = intersection(zc,groebnerCone(I->m[i],r,w));
  return zc;
}

gfan::ZCone fullGroebnerCone(const ideal &I, const ring &r)
{
  int n = rVar(r);
  poly g = NULL;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i]; pGetExpV(g,leadexpv);
    leadexpw = intStar2ZVector(n, leadexpv);
    pIter(g);
    while (g != NULL)
    {
      pGetExpV(g,tailexpv);
      tailexpw = intStar2ZVector(n, tailexpv);
      inequalities.appendRow(leadexpw-tailexpw);
      pIter(g);
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
}

groebnerConeData::groebnerConeData():
  I(NULL),
  r(NULL),
  c(gfan::ZCone(0)),
  p(gfan::ZVector(0))
{
}

groebnerConeData::groebnerConeData(const groebnerConeData &sigma):
  I(id_Copy(sigma.getIdeal(),sigma.getRing())),
  r(rCopy(sigma.getRing())),
  c(gfan::ZCone(sigma.getCone())),
  p(gfan::ZVector(sigma.getInteriorPoint()))
{
}

groebnerConeData::groebnerConeData(const ideal &J, const ring &s, const gfan::ZCone &d, const gfan::ZVector &q):
  I(J),
  r(s),
  c(d),
  p(q)
{
}

groebnerConeData::~groebnerConeData()
{
  // if (I!=NULL) id_Delete(&I,r);
  // if (r!=NULL) rDelete(r);
}


groebnerConeData maximalGroebnerConeData(ideal I, const ring r)
{
  int n = rVar(r);
  poly g = NULL;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i]; pGetExpV(g,leadexpv);
    leadexpw = intStar2ZVector(n, leadexpv);
    pIter(g);
    while (g != NULL)
    {
      pGetExpV(g,tailexpv);
      tailexpw = intStar2ZVector(n, tailexpv);
      inequalities.appendRow(leadexpw-tailexpw);
      pIter(g);
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  gfan::ZCone zc = gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
  gfan::ZVector p = zc.getRelativeInteriorPoint();
  return groebnerConeData(I,r,zc,p);
}

/***
 * Given a general ring r with any ordering, changes the ordering to wp(-w)
 **/
void changeOrderingTo_wp(ring r, const gfan::ZVector w)
{
  omFree(r->order);
  r->order  = (int*) omAlloc0(3*sizeof(int));
  omFree(r->block0);
  r->block0 = (int*) omAlloc0(3*sizeof(int));
  omFree(r->block1);
  r->block1 = (int*) omAlloc0(3*sizeof(int));
  for (int i=0; r->wvhdl[i]; i++) omFree(r->wvhdl[i]);
  omFree(r->wvhdl);
  r->wvhdl  = (int**) omAlloc0(3*sizeof(int*));

  bool ok = false;
  r->order[0]  = ringorder_wp;
  r->block0[0] = 1;
  r->block1[0] = r->N;
  r->wvhdl[0]  = ZVectorToIntStar(w,ok);
  r->order[1]  = ringorder_C;
  rComplete(r,1);
}

groebnerConeData::groebnerConeData(const ideal J, const ring s, const gfan::ZVector w)
{
  r = rCopy(s);
  changeOrderingTo_wp(r,w);
  rChangeCurrRing(r);

  int k = idSize(J); I = idInit(k);
  nMapFunc identityMap = n_SetMap(s->cf,r->cf);
  for (int i=0; i<k; i++)
    I->m[i] = p_PermPoly(J->m[i],NULL,s,r,identityMap,NULL,0);
  intvec* nullVector = NULL;
  I = kStd(I,currQuotient,testHomog,&nullVector);

  c = sloppyGroebnerCone(I,r,w);
  p = c.getRelativeInteriorPoint();
}

groebnerConeData::groebnerConeData(const ideal J, const ring s, const gfan::ZCone d)
{
  c = d;
  p = d.getRelativeInteriorPoint();

  r = rCopy(s);
  changeOrderingTo_wp(r,p);
  rChangeCurrRing(r);

  int k = idSize(J); I = idInit(k);
  nMapFunc identityMap = n_SetMap(s->cf,r->cf);
  for (int i=0; i<k; i++)
    I->m[i] = p_PermPoly(J->m[i],NULL,s,r,identityMap,NULL,0);
  intvec* nullVector = NULL;
  I = kStd(I,currQuotient,testHomog,&nullVector);
}

// /***
//  * Given a general ring r with any ordering, changes the ordering to a(v),ws(-w)
//  **/
// bool changetoAWSRing(ring r, const gfan::ZVector v, const gfan::ZVector w)
// {
//   omFree(r->order);
//   r->order  = (int*) omAlloc0(4*sizeof(int));
//   omFree(r->block0);
//   r->block0 = (int*) omAlloc0(4*sizeof(int));
//   omFree(r->block1);
//   r->block1 = (int*) omAlloc0(4*sizeof(int));
//   for (int i=0; r->wvhdl[i]; i++)
//   { omFree(r->wvhdl[i]); }
//   omFree(r->wvhdl);
//   r->wvhdl  = (int**) omAlloc0(4*sizeof(int*));

//   bool ok = false;
//   r->order[0]  = ringorder_a;
//   r->block0[0] = 1;
//   r->block1[0] = r->N;
//   r->wvhdl[0]  = ZVectorToIntStar(v,ok);
//   r->order[1]  = ringorder_ws;
//   r->block0[1] = 1;
//   r->block1[1] = r->N;
//   r->wvhdl[1]  = ZVectorToIntStar(w,ok);
//   r->order[2]  = ringorder_C;
//   return ok;
// }

// groebnerConeData::groebnerConeData(const ideal J, const ring s, const gfan::ZVector w)
// {
//   r = rCopy(s);
//   changeOrderingTo_wp(r,w);
//   rChangeCurrRing(r);

//   int k = idSize(J); I = idInit(k);
//   nMapFunc identityMap = n_SetMap(s->cf,r->cf);
//   for (int i=0; i<k; i++)
//     I->m[i] = p_PermPoly(J->m[i],NULL,s,r,nMap,NULL,0);
//   intvec* nullVector = NULL;
//   I = kStd(I,currQuotient,testHomog,&nullVector);

//   c = sloppyGroebnerCone(I,r,w);
//   p = c.getRelativeInteriorPoint();
// }

gfan::ZFan* toFanStar(setOfGroebnerConeData setOfCones)
{
  if (setOfCones.size() > 0)
  {
    setOfGroebnerConeData::iterator cone = setOfCones.begin();
    gfan::ZFan* zf = new gfan::ZFan(cone->getCone().ambientDimension());
    for (; cone!=setOfCones.end(); cone++)
      zf->insert(cone->getCone());
    return zf;
  }
  else
    return new gfan::ZFan(gfan::ZFan::fullFan(currRing->N));
}
