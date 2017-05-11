#include "kernel/GBEngine/kstd1.h"
#include "Singular/lists.h"
#include "polys/monomials/p_polys.h"
#include "callgfanlib_conversion.h"
#include "tropicalDebug.h"
#include "initial.h"
#include "tropicalStrategy.h"

matrix divisionDiscardingRemainder(const poly f, const ideal G, const ring r)
{
  ring origin = currRing;
  if (origin != r) rChangeCurrRing(r);
  ideal F = idInit(1); F->m[0]=f;
  ideal m = idLift(G,F);
  F->m[0]=NULL; id_Delete(&F, currRing);
  matrix Q = id_Module2formatedMatrix(m,IDELEMS(G),1,currRing);
  if (origin != r) rChangeCurrRing(origin);
  return Q;
}

matrix divisionDiscardingRemainder(const ideal F, const ideal G, const ring r)
{
  ring origin = currRing;
  if (origin != r) rChangeCurrRing(r);
  ideal R; matrix U;
  ideal m = idLift(G,F,&R,FALSE,FALSE,TRUE,&U);
  matrix Q = id_Module2formatedMatrix(m,IDELEMS(G),IDELEMS(F),currRing);
  id_Delete(&R,r);
  mp_Delete(&U,r);
  if (origin != r) rChangeCurrRing(origin);
  return Q;
}

poly witness(const poly m, const ideal I, const ideal inI, const ring r)
{
  assume(IDELEMS(I)==IDELEMS(inI));
  matrix Q = divisionDiscardingRemainder(m,inI,r);

  int k = IDELEMS(I);
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

ideal witness(const ideal inI, const ideal J, const ring r)
{
  ring origin = currRing;
  if (origin!=r)
    rChangeCurrRing(r);
  ideal NFinI = kNF(J,r->qideal,inI);
  if (origin!=r)
    rChangeCurrRing(origin);

  int k = IDELEMS(inI);
  ideal I = idInit(k);
  for (int i=0; i<k; i++)
  {
    I->m[i] = p_Add_q(p_Copy(inI->m[i],r),p_Neg(NFinI->m[i],r),r);
    NFinI->m[i] = NULL;
  }

  assume(areIdealsEqual(I,r,J,r));

  return I;
}
