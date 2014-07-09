#include <kernel/kstd1.h>
#include <Singular/lists.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <initial.h>
#include <tropicalStrategy.h>
#include <utility>

/***
 * Given f and G={g1,...,gk}, computes Q=(q1,...,qk) such that
 * f = q1*g1+...+qk*gk
 * is a determinate division with remainder with respect to the
 * ordering active in r.
 * Returns an error if this is not possible.
 **/
static matrix divisionDiscardingRemainder(const poly f, const ideal G, const ring r)
{
  ring origin = currRing;
  rComplete(r);
  if (origin != r) rChangeCurrRing(r);
  ideal F = idInit(1); F->m[0]=f;
  ideal m = idLift(G,F,NULL,FALSE,FALSE);
  F->m[0]=NULL; id_Delete(&F, currRing);
  matrix Q = id_Module2formatedMatrix(m,IDELEMS(G),1,currRing);
  if (origin != r) rChangeCurrRing(origin);
  return Q;
}

#ifndef NDEBUG
BOOLEAN dwr0(leftv res, leftv args)
{
  leftv u = args;
  leftv v = u->next;
  poly f = (poly) u->CopyD();
  ideal G = (ideal) v->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  matrix Q = divisionDiscardingRemainder(f,G,currRing);
  p_Delete(&f,currRing);
  id_Delete(&G,currRing);
  res->rtyp = MATRIX_CMD;
  res->data = (char*) Q;
  return FALSE;
}
#endif

/***
 * Let w be the uppermost weight vector in the matrix defining the ordering on r.
 * Let I be a Groebner basis of an ideal in r, inI its initial form with respect w.
 * Given an w-homogeneous element m of inI, computes a witness g of m in I,
 * i.e. g in I such that in_w(g)=m.
 **/
poly witness(const poly m, const ideal I, const ideal inI, const ring r)
{
  matrix Q = divisionDiscardingRemainder(m,inI,r);

  int k = idSize(I);
  poly f = p_Mult_q(p_Copy(I->m[0],r),Q->m[0],r);
  Q->m[0] = NULL;
  for (int i=1; i<k; i++)
  {
    f = p_Add_q(f,p_Mult_q(p_Copy(I->m[i],r),Q->m[i],r),r);
    Q->m[i] = NULL;
  }
  mp_Delete(&Q,r);

  return f;
}

#ifndef NDEBUG
BOOLEAN witness0(leftv res, leftv args)
{
  leftv u = args;
  leftv v = u->next;
  leftv w = v->next;
  poly m = (poly) u->CopyD();
  ideal G = (ideal) v->CopyD();
  ideal inG = (ideal) w->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  poly f = witness(m,G,inG,currRing);
  p_Delete(&m,currRing);
  id_Delete(&G,currRing);
  id_Delete(&inG,currRing);
  res->rtyp = POLY_CMD;
  res->data = (char*) f;
  return FALSE;
}
#endif
