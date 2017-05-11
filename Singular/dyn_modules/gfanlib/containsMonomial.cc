#include "kernel/polys.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/prCopy.h"

#include "callgfanlib_conversion.h"
#include "bbcone.h"
#include "std_wrapper.h"

poly checkForMonomialViaSuddenSaturation(const ideal I, const ring r)
{
  ring origin = currRing;
  if (currRing != r)
    rChangeCurrRing(r);

  ideal M = idInit(1);
  M->m[0] = p_Init(r);
  for (int i=1; i<=rVar(r); i++)
    p_SetExp(M->m[0],i,1,r);
  p_SetCoeff(M->m[0],n_Init(1,r->cf),r);
  p_Setm(M->m[0],r); p_Test(M->m[0],r);

  ideal J = id_Copy(I,r); bool b; int k = 0;
  if (currRing != r) rChangeCurrRing(r);
  intvec* nullVector = NULL;
  do
  {
    ideal Jstd = kStd(J,currRing->qideal,testHomog,&nullVector);
    ideal JquotM = idQuot(Jstd,M,true,true);
    ideal JquotMredJ = kNF(Jstd,currRing->qideal,JquotM);
    b = idIs0(JquotMredJ);
    id_Delete(&Jstd,r);
    id_Delete(&J,r);
    J = JquotM;
    id_Delete(&JquotMredJ,r);
    k++;
  } while (!b);

  poly monom = NULL;
  if (id_IsConstant(J,r))
  {
    monom = p_Init(r);
    for (int i=1; i<=rVar(r); i++)
      p_SetExp(monom,i,k,r);
    p_SetCoeff(monom,n_Init(1,r->cf),r);
    p_Setm(monom,r);
  }
  id_Delete(&M,r);
  id_Delete(&J,r);

  if (currRing != origin)
    rChangeCurrRing(origin);
  return monom;
}

poly searchForMonomialViaStepwiseSaturation(const ideal I, const ring r, const gfan::ZVector w0)
{
  gfan::ZVector w = w0;

  ring origin = currRing;
  if (currRing != r)
    rChangeCurrRing(r);

  // copy ring including qideal but without ordering
  // set ordering to be wp(w)
  int n = rVar(r);
  ring rGraded = rCopy0(r,TRUE,FALSE);
  rGraded->order = (rRingOrder_t*) omAlloc0(3*sizeof(rRingOrder_t));
  rGraded->block0 = (int*) omAlloc0(3*sizeof(int));
  rGraded->block1 = (int*) omAlloc0(3*sizeof(int));
  rGraded->wvhdl = (int**) omAlloc0(3*sizeof(int**));
  rGraded->order[0] = ringorder_wp;
  rGraded->block0[0] = 1;
  rGraded->block1[0] = n;
  bool overflow;
  rGraded->wvhdl[0] = ZVectorToIntStar(w,overflow);
  rGraded->order[1] = ringorder_C;
  rComplete(rGraded);
  rTest(rGraded);

  // map I into the new ring so that it becomes a graded ideal
  int k = IDELEMS(I);
  nMapFunc identity = n_SetMap(r->cf,rGraded->cf);
  ideal Jold = idInit(k);
  for (int i=0; i<k; i++)
    Jold->m[i] = p_PermPoly(I->m[i],NULL,r,rGraded,identity,NULL,0);

  // compute std and check whether result contains a monomial,
  // wrap up computation if it does
  ideal Jnew = gfanlib_monomialabortStd_wrapper(Jold,rGraded);
  id_Delete(&Jold,rGraded);
  k = IDELEMS(Jnew);
  for (int i=0; i<k; i++)
  {
    poly g = Jnew->m[i];
    if (g!=NULL && pNext(g)==NULL && n_IsUnit(p_GetCoeff(g,r),r->cf))
    {
      poly monomial = p_One(r);
      for (int j=1; j<=rVar(r); j++)
        p_SetExp(monomial,j,p_GetExp(g,j,rGraded),r);
      p_Setm(monomial,r);

      id_Delete(&Jnew,rGraded);
      rDelete(rGraded);
      if (currRing != origin)
        rChangeCurrRing(origin);
      return monomial;
    }
  }

  // prepare permutation to cycle all variables
  int* cycleAllVariables = (int*) omAlloc0((n+1)*sizeof(int));
  for (int i=1; i<n; i++)
    cycleAllVariables[i]=i+1;
  cycleAllVariables[n]=1;
  // prepare storage of maximal powers that are being divided with
  int* maxPowers = (int*) omAlloc0((n+1)*sizeof(int));

  for(int currentSaturationVariable=n-1; currentSaturationVariable>0; currentSaturationVariable--)
  {
    // divide out the maximal power in the last variable,
    // storing the maximum of all powers.
    for (int i=0; i<k; i++)
    {
      poly g = Jnew->m[i];
      int d = p_GetExp(g,n,rGraded);
      if (d>0)
      {
        for (; g!=NULL; pIter(g))
        {
          p_SubExp(g,n,d,rGraded);
          p_Setm(g,rGraded);
        }
        if (d>maxPowers[currentSaturationVariable+1])
          maxPowers[currentSaturationVariable+1]=d;
      }
    }

    // cycle all variables, i.e. x_1->x_2, x_2->x_3, ..., x_n->x_1
    // so that a new variable is at the last position
    gfan::Integer cache = w[n-1];
    for (int i=n-1; i>0; i--)
      w[i] = w[i-1];
    w[0] = cache;

    ring rGradedNew = rCopy0(r,FALSE,FALSE); // cannot copy q-ideal without ordering
    rGradedNew->order = (rRingOrder_t*) omAlloc0(3*sizeof(rRingOrder_t));
    rGradedNew->block0 = (int*) omAlloc0(3*sizeof(int));
    rGradedNew->block1 = (int*) omAlloc0(3*sizeof(int));
    rGradedNew->wvhdl = (int**) omAlloc0(3*sizeof(int**));
    rGradedNew->order[0] = ringorder_wp;
    rGradedNew->block0[0] = 1;
    rGradedNew->block1[0] = n;
    bool overflow;
    rGradedNew->wvhdl[0] = ZVectorToIntStar(w,overflow);
    rGradedNew->order[1] = ringorder_C;
    rComplete(rGradedNew);
    rTest(rGradedNew);

    identity = n_SetMap(rGraded->cf,rGradedNew->cf);
    Jold = idInit(k);
    for (int i=0; i<k; i++)
      Jold->m[i] = p_PermPoly(Jnew->m[i],cycleAllVariables,rGraded,rGradedNew,identity,NULL,0);
    id_Delete(&Jnew,rGraded);
    rDelete(rGraded);

    rGraded = rGradedNew;
    rGradedNew = NULL;

    // compute std and check whether result contains a monomial,
    // wrap up computation if it does
    // adjust for the powers already divided out of the ideal
    // and the shift of variables!
    Jnew = gfanlib_monomialabortStd_wrapper(Jold,rGraded);
    id_Delete(&Jold,rGraded);

    k = IDELEMS(Jnew);
    for (int i=0; i<k; i++)
    {
      poly g = Jnew->m[i];
      if (g!=NULL && pNext(g)==NULL && n_IsUnit(p_GetCoeff(g,rGraded),rGraded->cf))
      {
        poly monomial = p_One(r);
        for (int j=1; j<=rVar(r); j++)
        {
          int jDeshifted = (j-currentSaturationVariable)%n;
          if (jDeshifted<=0) jDeshifted = jDeshifted+n;
          p_SetExp(monomial,j,p_GetExp(g,jDeshifted,rGraded)+maxPowers[j],r);
        }
        p_Setm(monomial,r);

        id_Delete(&Jnew,rGraded);
        rDelete(rGraded);
        omFree(cycleAllVariables);
        omFree(maxPowers);
        if (currRing != origin)
          rChangeCurrRing(origin);
        return monomial;
      }
    }
  }

  if (currRing != origin)
    rChangeCurrRing(origin);

  id_Delete(&Jnew,rGraded);
  rDelete(rGraded);
  omFree(cycleAllVariables);
  omFree(maxPowers);
  return NULL;
}

BOOLEAN checkForMonomial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    ideal I; poly monom;
    omUpdateInfo();
    Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
    I = (ideal) u->CopyD();
    monom = checkForMonomialViaSuddenSaturation(I,currRing);
    id_Delete(&I,currRing);
    p_Delete(&monom,currRing);
    omUpdateInfo();
    Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
    I = (ideal) u->Data();
    res->rtyp = POLY_CMD;
    res->data = (char*) checkForMonomialViaSuddenSaturation(I,currRing);
    return FALSE;
  }
  return TRUE;
}

BOOLEAN searchForMonomialViaStepwiseSaturation(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      ideal I = (ideal) u->Data();
      bigintmat* w0=NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* w00 = (intvec*) v->Data();
        bigintmat* w0t = iv2bim(w00,coeffs_BIGINT);
        w0 = w0t->transpose();
        delete w0t;
      }
      else
        w0 = (bigintmat*) v->Data();
      gfan::ZVector* w = bigintmatToZVector(w0);

      res->rtyp = POLY_CMD;
      res->data = (char*) searchForMonomialViaStepwiseSaturation(I,currRing,*w);
      delete w;
      if (v->Typ() == INTVEC_CMD)
        delete w0;

      return FALSE;
    }
  }
  WerrorS("searchForMonomialViaStepwiseSaturation: unexpected parameters");
  return TRUE;
}
