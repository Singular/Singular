#include <kernel/ideals.h>
#include <polys/monomials/p_polys.h>

#include <gfanlib/gfanlib.h>

#include <exception>

long wDeg(const poly p, const ring r, const gfan::ZVector w)
{
  long d=0;
  for (unsigned i=0; i<w.size(); i++)
  {
    if (!w[i].fitsInInt())
      throw 0; // weightOverflow;
    d += p_GetExp(p,i+1,r)*w[i].toInt();
  }
  return d;
}

gfan::ZVector WDeg(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  gfan::ZVector d = gfan::ZVector(W.getHeight()+1);
  d[0] = wDeg(p,r,w);
  for (int i=0; i<W.getHeight(); i++)
    d[i+1] = wDeg(p,r,W[i]);
  return d;
}

poly initial(const poly p, const ring r, const gfan::ZVector w)
{
  if (p==NULL)
    return NULL;

  poly q0 = p_Head(p,r);
  poly q1 = q0;
  long d = wDeg(p,r,w);
  for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
  {
    long e = wDeg(currentTerm,r,w);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p_Head(currentTerm,r);
      q1 = q0;
      d = e;
    }
    else
      if (e==d)
      {
        pNext(q1) = p_Head(currentTerm,r);
        pIter(q1);
      }
  }
  return q0;
}

ideal initial(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r,w);
  return inI;
}

poly initial(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  if (p==NULL)
    return NULL;

  poly q0 = p_Head(p,r);
  poly q1 = q0;
  gfan::ZVector d = WDeg(p,r,w,W);
  for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
  {
    gfan::ZVector e = WDeg(currentTerm,r,w,W);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p_Head(p,r);
      q1 = q0;
      d = e;
    }
    else
      if (d==e)
      {
        pNext(q1) = p_Head(currentTerm,r);
        pIter(q1);
      }
  }
  return q0;
}

ideal initial(const ideal I, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r,w,W);
  return inI;
}

void initial(poly* pStar, const ring r, const gfan::ZVector w)
{
  poly p = *pStar;
  if (p==NULL)
    return;

  long d = wDeg(p,r,w);
  poly q0 = p;
  poly q1 = q0;
  pNext(q1) = NULL;
  pIter(p);

  while(p)
  {
    long e = wDeg(p,r,w);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p;
      q1 = q0;
      pNext(q1) = NULL;
      d = e;
      pIter(p);
    }
    else
      if (e==d)
      {
        pNext(q1) = p;
        pIter(q1);
        pNext(q1) = NULL;
        pIter(p);
      }
      else
        p = p_LmDeleteAndNext(p,r);
  }
  pStar = &q0;
  return;
}

void initial(ideal* IStar, const ring r, const gfan::ZVector w)
{
  ideal I = *IStar;
  int k = idSize(I);
  for (int i=0; i<k; i++)
    initial(&I->m[i],r,w);
  return;
}

void initial(poly* pStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  poly p = *pStar;
  if (p==NULL)
    return;

  gfan::ZVector d = WDeg(p,r,w,W);
  poly q0 = p;
  poly q1 = q0;
  pNext(q1) = NULL;
  pIter(p);

  while(p)
  {
    gfan::ZVector e = WDeg(p,r,w,W);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p;
      q1 = q0;
      pNext(q1) = NULL;
      d = e;
      pIter(p);
    }
    else
      if (d==e)
      {
        pNext(q1) = p;
        pIter(q1);
        pNext(q1) = NULL;
        pIter(p);
      }
      else
        p = p_LmDeleteAndNext(p,r);
  }
  pStar = &q0;
  return;
}

void initial(ideal* IStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  ideal I = *IStar;
  int k = idSize(I);
  for (int i=0; i<k; i++)
    initial(&I->m[i],r,w,W);
  return;
}
