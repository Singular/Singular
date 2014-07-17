#include <bbcone.h>
#include <kernel/polys.h>
#include <kernel/kstd1.h>
#include <libpolys/polys/prCopy.h>

#if 0
// /***
//  * Creates an int* representing the transposition of the last two variables
//  **/
// static inline int* createPermutationVectorForSaturation(static const ring &r)
// {
//   int* w = (int*) omAlloc0((rVar(r)+1)*sizeof(int));
//   for (int i=1; i<=rVar(r)-2; i++)
//     w[i] = i;
//   w[rVar(r)-1] = rVar(r);
//   w[rVar(r)] = rVar(r)-1;
// }


/***
 * Creates an int* representing the permutation
 * 1 -> 1, ..., i-1 -> i-1, i -> n, i+1 -> n-1, ... , n -> i
 **/
static inline int* createPermutationVectorForSaturation(const ring &r, const int i)
{
  int* sigma = (int*) omAlloc0((rVar(r)+1)*sizeof(int));
  int j;
  for (j=1; j<i; j++)
    sigma[j] = j;
  for (; j<=rVar(r); j++)
    sigma[j] = rVar(r)-j+i;
  return(sigma);
}


/***
 * Changes the int* representing the permutation
 * 1 -> 1, ..., i -> i, i+1 -> n, i+2 -> n-1, ... , n -> i+1
 * to an int* representing the permutation
 * 1 -> 1, ..., i-1 -> i-1, i -> n, i+1 -> n-1, ... , n -> i
 **/
static void changePermutationVectorForSaturation(int* sigma, const ring &r, const int i)
{
  for (int j=i; j<rVar(r); j++)
    sigma[j] = rVar(r)-j+i;
  sigma[rVar(r)] = i;
}


/***
 * returns a ring in which the weights of the ring variables are permuted
 * if handed over a poly in which the variables are permuted, this is basically
 * as good as permuting the variables of the ring itself.
 **/
static ring permuteWeighstOfRingVariables(const ring &r, const int* const sigma)
{
  ring s = rCopy0(r);
  for (int j=0; j<rVar(r); j++)
  {
    s->wvhdl[0][j] = r->wvhdl[0][sigma[j+1]];
    s->wvhdl[1][j] = r->wvhdl[1][sigma[j+1]];
  }
  rComplete(s,1);
  return s;
}


/***
 * creates a ring s that is a copy of r except with ordering wp(w)
 **/
static inline ring createInitialRingForSaturation(const ring &r, const gfan::ZVector &w, bool &ok)
{
  assume(rVar(r) == (int) w.size());

  ring s = rCopy0(r); int i;
  for (i=0; s->order[i]; i++)
    omFreeSize(s->wvhdl[i],rVar(r)*sizeof(int));
  i++;
  omFreeSize(s->order,i*sizeof(int));
  s->order  = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->block0,i*sizeof(int));
  s->block0 = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->block1,i*sizeof(int));
  s->block1 = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->wvhdl,i*sizeof(int*));
  s->wvhdl  = (int**) omAlloc0(3*sizeof(int*));

  s->order[0]  = ringorder_wp;
  s->block0[0] = 1;
  s->block1[0] = rVar(r);
  s->wvhdl[0]  = ZVectorToIntStar(w,ok);
  s->order[1]=ringorder_C;

  rComplete(s,1);
  return s;
}


/***
 * Given an weighted homogeneous ideal I with respect to weight w
 * that in standard basis form with respect to the ordering ws(-w),
 * derives the standard basis of I:<x_n>^\infty
 * and returns a long k such that I:<x_n>^\infty=I:<x_n>^k
 **/
static long deriveStandardBasisOfSaturation(ideal &I, ring &r)
{
  long k=0, l; poly current;
  for (int i=0; i<IDELEMS(I); i++)
  {
    current = I->m[i];
    l = p_GetExp(current,rVar(r),r);
    if (k<l) k=l;
    while (current)
    {
      p_SubExp(current,rVar(r),l,r); p_Setm(current,r);
      pIter(current);
    }
  }
  return k;
}


/***
 * Given a weighted homogeneous ideal I with respect to weight w
 * with constant first element,
 * returns NULL if I does not contain a monomial
 * otherwise returns the monomial contained in I
 **/
poly checkForMonomialsViaStepwiseSaturation(const ideal &I, const gfan::ZVector &w)
{
  // assume(rField_is_Ring_Z(currRing));

  // first we switch to the ground field currRing->cf / I->m[0]
  ring r = rCopy0(currRing);
  nKillChar(r->cf);
  r->cf = nInitChar(n_Zp,(void*)(long)n_Int(p_GetCoeff(I->m[0],currRing),currRing->cf));
  rComplete(r);

  ideal J = id_Copy(I, currRing); poly cache; number temp;
  for (int i=0; i<IDELEMS(I); i++)
  {
    cache = J->m[i];
    while (cache)
    {
      // TODO: temp = npMapGMP(p_GetCoeff(cache,currRing),currRing->cf,r->cf);
      p_SetCoeff(cache,temp,r); pIter(cache);
    }
  }

  J = kStd(J,NULL,isHomog,NULL);

  bool b = false;
  ring s = createInitialRingForSaturation(currRing, w, b);
  if (b)
  {
    WerrorS("containsMonomial: overflow in weight vector");
    return NULL;
  }

  return NULL;
}
#endif


poly checkForMonomialViaSuddenSaturation(const ideal I, const ring r)
{
  ring origin = currRing;
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
    ideal Jstd = kStd(J,currQuotient,testHomog,&nullVector);
    ideal JquotM = idQuot(Jstd,M,true,true); k++;
    ideal JquotMredJ = kNF(JquotM,currQuotient,Jstd);
    b = idIs0(JquotMredJ);
    id_Delete(&Jstd,r); id_Delete(&J,r); J = JquotM;
    id_Delete(&JquotMredJ,r);
  } while (!b);

  if (currRing != origin) rChangeCurrRing(origin);
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
  return monom;
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
