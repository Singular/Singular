#include <gfanlib/gfanlib.h>

#include <kernel/ideals.h>
#include <Singular/subexpr.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <libpolys/polys/simpleideals.h>

#include <callgfanlib_conversion.h>
#include <gfanlib_exceptions.h>

#include <exception>

/***
 * Computes the weighted degree of the leading term of p with respect to w
 **/
long wDeg(const poly p, const ring r, const gfan::ZVector w)
{
  long d=0;
  for (unsigned i=0; i<w.size(); i++)
  {
    if (!w[i].fitsInInt())
      throw 0; //weightOverflow;
    d += p_GetExp(p,i+1,r)*w[i].toInt();
  }
  return d;
}

/***
 * Returns the first terms of p of same weighted degree under w,
 * this is not necessarily the initial form of p with respect to w!
 **/
poly sloppyInitial(const poly p, const ring r, const gfan::ZVector w)
{
  int n = r->N;
  int* expv = (int*) omAlloc(n*sizeof(int));
  poly q0 = p_Head(p,r);
  poly q1 = q0;
  long d = wDeg(p,r,w);
  for (poly currentTerm = p->next; wDeg(currentTerm,r,w)==d; pIter(currentTerm))
  {
    pNext(q1) = p_Head(currentTerm,r);
    pIter(q1);
  }
  omFreeSize(expv,n*sizeof(int));
  return q0;
}

/***
 * Runs the above procedure over all generators of an ideal.
 **/
ideal sloppyInitial(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = sloppyInitial(I->m[i],r,w);
  return inI;
}

/***
 * Returns the first terms of p of same weighted degree under w,
 * this is not necessarily the initial form of p with respect to w!
 **/
poly initial(const poly p, const ring r, const gfan::ZVector w)
{
  int n = r->N;
  int* expv = (int*) omAlloc(n*sizeof(int));
  poly q0 = p_Head(p,r);
  poly q1 = q0;
  long d = wDeg(p,r,w);
  for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
  {
    if (wDeg(currentTerm,r,w)==d)
    {
      pNext(q1) = p_Head(currentTerm,r);
      pIter(q1);
    }
  }
  omFreeSize(expv,n*sizeof(int));
  return q0;
}

/***
 * Runs the above procedure over all generators of an ideal.
 **/
ideal initial(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r,w);
  return inI;
}

#ifndef NDEBUG
BOOLEAN initial0(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  bigintmat* w0 = (bigintmat*) v->Data();
  gfan::ZVector* w = bigintmatToZVector(w0);
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  ideal inI = initial(I,currRing,*w);
  id_Delete(&I,currRing);
  delete w;
  res->rtyp = IDEAL_CMD;
  res->data = (char*) inI;
  return FALSE;
}
#endif


/***
 * Computes the initial form of p with respect to the first row in the order matrix
 **/
poly initial(const poly p, const ring r)
{
  long d = p_Deg(p,r);
  poly initialForm0 = p_Head(p,r);
  poly initialForm1 = initialForm0;
  poly currentTerm = p->next;
  while (currentTerm && p_Deg(currentTerm,r)==d)
  {
    pNext(initialForm1) = p_Head(currentTerm,r);
    pIter(currentTerm); pIter(initialForm1);
  }
  return initialForm0;
}

/***
 * Computes the initial form of all generators of I.
 * If I is a standard basis, then this is a standard basis
 * of the initial ideal.
 **/
ideal initial(const ideal I, const ring r)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r);
  return inI;
}

BOOLEAN initial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD) && (u->next == NULL))
  {
    poly p = (poly) u->Data();
    res->rtyp = POLY_CMD;
    res->data = (void*) initial(p, currRing);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD) && (u->next == NULL))
  {
    ideal I = (ideal) u->Data();
    res->rtyp = IDEAL_CMD;
    res->data = (void*) initial(I, currRing);
    return FALSE;
  }
  WerrorS("initial: unexpected parameters");
  return TRUE;
}
