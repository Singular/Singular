#include <kernel/polys.h>
#include <Singular/ipid.h>

#include <libpolys/polys/monomials/ring.h>
#include <kernel/ideals.h>
#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_zcone.h>

#include <bbcone.h>
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

groebnerConeData::groebnerConeData(const ideal &J, const ring &s):
  I(J),
  r(s)
{
}

groebnerConeData::~groebnerConeData()
{
  id_Delete(&I,r);
  rDelete(r);
}
