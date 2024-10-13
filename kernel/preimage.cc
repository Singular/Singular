#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "kernel/polys.h"
#include "polys/monomials/ring.h"


#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"

#ifdef HAVE_PLURAL
#include "polys/nc/nc.h"
#endif

/*2
*shifts the variables between minvar and maxvar of p  \in p_ring to the
*first maxvar-minvar+1 variables in the actual ring
*be careful: there is no range check for the variables of p
*/
static poly pChangeSizeOfPoly(ring p_ring, poly p,int minvar,int maxvar, const ring dst_r)
{
  int i;
  poly result = NULL,resultWorkP;
  number n;

  if (p==NULL) return result;
  else result = p_Init(dst_r);
  resultWorkP = result;
  while (p!=NULL)
  {
    for (i=minvar;i<=maxvar;i++)
      p_SetExp(resultWorkP,i-minvar+1,p_GetExp(p,i,p_ring),dst_r);
    p_SetComp(resultWorkP,p_GetComp(p,p_ring),dst_r);
    n=n_Copy(pGetCoeff(p),dst_r->cf);
    p_SetCoeff0(resultWorkP,n,dst_r);
    p_Setm(resultWorkP,dst_r);
    pIter(p);
    if (p!=NULL)
    {
      pNext(resultWorkP) = p_Init(dst_r);
      pIter(resultWorkP);
    }
  }
  return result;
}



/*2
*returns the preimage of id under theMap,
*if id is empty or zero the kernel is computed
* (assumes) that both ring have the same coeff.field
*/
ideal maGetPreimage(ring theImageRing, map theMap, ideal id, const ring dst_r)
{
  ring sourcering = dst_r;

#ifdef HAVE_PLURAL
  if (rIsPluralRing(theImageRing))
  {
    if ((rIsPluralRing(sourcering)) && (ncRingType(sourcering)!=nc_comm))
    {
      WerrorS("Sorry, not yet implemented for noncomm. rings");
      return NULL;
    }
  }
#endif

  int i,j;
  poly p,/*pp,*/q;
  ideal temp1;
  ideal temp2;

  int imagepvariables = rVar(theImageRing);
  int N = rVar(dst_r)+imagepvariables;

  ring tmpR;
  if (rSumInternal(theImageRing,sourcering,tmpR,FALSE,2)!=1)
  {
     WerrorS("error in rSumInternal");
     return NULL;
  }

  if (theImageRing->cf != dst_r->cf)
  {
    /// TODO: there might be extreme cases where this doesn't hold...
    WerrorS("Coefficient fields/rings must be equal");
    return NULL;
  }

  const ring save_ring = currRing; if (currRing!=tmpR) rChangeCurrRing(tmpR); // due to kStd

  if (id==NULL)
    j = 0;
  else
    j = IDELEMS(id);
  int j0=j;
  if (theImageRing->qideal!=NULL) j+=IDELEMS(theImageRing->qideal);
  temp1 = idInit(sourcering->N+j,1);
  for (i=0;i<sourcering->N;i++)
  {
    q = p_ISet(-1,tmpR);
    p_SetExp(q,i+1+imagepvariables,1,tmpR);
    p_Setm(q,tmpR);
    if ((i<IDELEMS(theMap)) && (theMap->m[i]!=NULL))
    {
      p = p_SortMerge(
                      pChangeSizeOfPoly(theImageRing, theMap->m[i], 1, imagepvariables, tmpR),
                      tmpR);
      p=p_Add_q(p,q,tmpR);
    }
    else
    {
      p = q;
    }
    temp1->m[i] = p;
  }
  id_Test(temp1, tmpR);
  for (i=sourcering->N;i<sourcering->N+j0;i++)
  {
    temp1->m[i] = p_SortMerge(
                              pChangeSizeOfPoly(theImageRing, id->m[i-sourcering->N], 1, imagepvariables, tmpR),
                              tmpR);
  }
  for (i=sourcering->N+j0;i<sourcering->N+j;i++)
  {
    temp1->m[i] = p_SortMerge(
                              pChangeSizeOfPoly(theImageRing, theImageRing->qideal->m[i-sourcering->N-j0], 1, imagepvariables, tmpR),
                              tmpR);
  }
  // we ignore here homogeneity - may be changed later:
  BITSET save;
  SI_SAVE_OPT2(save);
  si_opt_2|=Sy_bit(V_NOT_TRICKS);
  temp2 = kStd(temp1,NULL,isNotHomog,NULL);
  SI_RESTORE_OPT2(save);

  id_Delete(&temp1,tmpR);
  for (i=0;i<IDELEMS(temp2);i++)
  {
    if (p_LowVar(temp2->m[i], currRing)<imagepvariables) p_Delete(&(temp2->m[i]),tmpR);
  }

  // let's get back to the original ring
  //rChangeCurrRing(sourcering);
  temp1 = idInit(5,1);
  j = 0;
  for (i=0;i<IDELEMS(temp2);i++)
  {
    p = temp2->m[i];
    if (p!=NULL)
    {
      q = p_SortMerge(
                      pChangeSizeOfPoly(tmpR, p, imagepvariables+1, N, sourcering),
                      sourcering);
      if (j>=IDELEMS(temp1))
      {
        pEnlargeSet(&(temp1->m),IDELEMS(temp1),5);
        IDELEMS(temp1)+=5;
      }
      temp1->m[j] = q;
      j++;
    }
  }
  id_Delete(&temp2, tmpR);
  idSkipZeroes(temp1);

  if (currRing!=save_ring) rChangeCurrRing(save_ring);

  rDelete(tmpR);
  return temp1;
}

