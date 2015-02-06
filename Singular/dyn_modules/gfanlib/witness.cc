#include <kernel/GBEngine/kstd1.h>
#include <Singular/lists.h>
#include <polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>
#include <initial.h>
#include <tropicalStrategy.h>
#include <utility>

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
  assume(idSize(I)==idSize(inI));
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

ideal witness(const ideal inI, const ideal J, const ring r)
{
  ring origin = currRing;
  if (origin!=r)
    rChangeCurrRing(r);
  ideal NFinI = kNF(J,r->qideal,inI);
  if (origin!=r)
    rChangeCurrRing(origin);

  int k = idSize(inI);
  ideal I = idInit(k);
  for (int i=0; i<k; i++)
  {
    I->m[i] = p_Add_q(p_Copy(inI->m[i],r),p_Neg(NFinI->m[i],r),r);
    NFinI->m[i] = NULL;
  }

  return I;
}

#ifndef NDEBUG
BOOLEAN dwrDebug(leftv res, leftv args)
{
  leftv u = args;
  leftv v = u->next;
  ideal F = (ideal) u->CopyD();
  ideal G = (ideal) v->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  matrix Q = divisionDiscardingRemainder(F,G,currRing);
  id_Delete(&F,currRing);
  id_Delete(&G,currRing);
  res->rtyp = MATRIX_CMD;
  res->data = (char*) Q;
  return FALSE;
}

BOOLEAN witnessDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==IDEAL_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      ideal inI = (ideal) u->CopyD();
      ideal J = (ideal) v->CopyD();
      ideal I = witness(inI,J,currRing);
      id_Delete(&inI,currRing);
      id_Delete(&J,currRing);
      res->rtyp = IDEAL_CMD;
      res->data = (char*) I;
      return FALSE;
    }
  }
  return TRUE;
}
#endif
