#include <kernel/kstd1.h>
#include <Singular/lists.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <initial.h>
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
  if (currRing != r) rChangeCurrRing(r);
  ideal F = idInit(1); F->m[0]=f;
  ideal Rest; matrix U;
  ideal m = idLift(G,F,&Rest,FALSE,TRUE,TRUE,&U);
  id_Delete((ideal*)&U, r);
  F->m[0]=NULL; id_Delete(&F, currRing);
  matrix Q = id_Module2formatedMatrix(m,IDELEMS(G),1,currRing);
  assume(Rest->m[0] == NULL);
  id_Delete((ideal*) &Rest, r);
  if (currRing != origin) rChangeCurrRing(origin);
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
 * Computes a witness g of the w-homogeneous m in ideal I in the ring r,
 * such that in_w(g)=m, where w is the uppermost weight vector of r.
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


/***
 * Suppose r and s the same ring but with two adjacent orderings,
 * w a weight vector in the relative interior of their common facet.
 * Given a standard basis of an ideal I with respect to the ordering of r,
 * a standard basis of its w-initial ideal inI with respect to
 * the ordering of s, computes a standard basis of I with respect to
 * the ordering of s.
 **/
ideal lift(const ideal I, const ring r, const ideal inI, const ring s)
{
  nMapFunc identity = n_SetMap(r->cf,s->cf);
  int k = idSize(I); ideal Is = idInit(k);
  for (int i=0; i<k; i++)
    Is->m[i] = p_PermPoly(I->m[i],NULL,r,s,identity,NULL,0);
  ideal J = idInit(k);
  for (int i=0; i<k; i++)
    J->m[i] = witness(inI->m[i],Is,inI,s);
  id_Delete(&Is,s);
  return J;
}

void deleteOrdering(ring r)
{
  if (r->order != NULL)
  {
    int i=rBlocks(r);
    assume(r->block0 != NULL && r->block1 != NULL && r->wvhdl != NULL);
    // delete order
    omFreeSize((ADDRESS)r->order,i*sizeof(int));
    omFreeSize((ADDRESS)r->block0,i*sizeof(int));
    omFreeSize((ADDRESS)r->block1,i*sizeof(int));
    // delete weights
    for (int j=0; j<i; j++)
    {
      if (r->wvhdl[j]!=NULL)
        omFree(r->wvhdl[j]);
    }
    omFreeSize((ADDRESS)r->wvhdl,i*sizeof(int *));
  }
  else
    assume(r->block0 == NULL && r->block1 == NULL && r->wvhdl == NULL);
  return;
}

std::pair<ideal,ring> flip(const ideal I, const ring r, const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal)
{
  ring origin = currRing;
  ideal inIr = initial(I,r,interiorPoint);

  bool ok;
  ring s = rCopy0(r);
  int n = rVar(s);
  deleteOrdering(s);
  // for (i=0; s->order[i]; i++)
  //   omFree(s->wvhdl[i]);
  // i++; omFreeSize(s->order, i*sizeof(int));
  // omFreeSize(s->block0, i*sizeof(int));
  // omFreeSize(s->block1, i*sizeof(int));
  // omFreeSize(s->wvhdl, i*sizeof(int));
  s->order = (int*) omAlloc0(4*sizeof(int));
  s->block0 = (int*) omAlloc0(4*sizeof(int));
  s->block1 = (int*) omAlloc0(4*sizeof(int));
  s->wvhdl = (int**) omAlloc0(4*sizeof(int));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(interiorPoint,ok);
  s->order[1] = ringorder_wp;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(facetNormal,ok);
  s->order[2] = ringorder_C;
  rComplete(s,1);
  rChangeCurrRing(s);
  nMapFunc identity = n_SetMap(r->cf,currRing->cf);

  int k = idSize(I); ideal inIs = idInit(k);
  for (int i=0; i<k; i++)
    inIs->m[i] = p_PermPoly(inIr->m[i],NULL,r,s,identity,NULL,0);
  intvec* nullVector = NULL;
  ideal inIsGB = kStd(inIs,currQuotient,testHomog,&nullVector);
  ideal IsGB = lift(I,r,inIsGB,s);

  rChangeCurrRing(origin);
  id_Delete(&inIr,r);
  id_Delete(&inIs,s);
  id_Delete(&inIsGB,s);
  return std::make_pair(IsGB,s);
}
