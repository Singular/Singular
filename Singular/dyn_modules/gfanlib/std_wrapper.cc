#include <vector>

#include "Singular/libsingular.h"

std::vector<int> gitfan_satstdSaturatingVariables;
ideal gfanlib_kStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  intvec* nullVector = NULL;
  ideal stdI = kStd(I,currRing->qideal,h,&nullVector); // there is still a memory leak here!!!
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);
  if (nullVector!=NULL) delete nullVector;

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

//------------------------------------------------------------------------
// routine that simplifies the new element by dividing it with the maximal possible
// partially saturating the ideal with respect to all variables doing so
static BOOLEAN sat_vars_sp(kStrategy strat)
{
  BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
                     // let it remain FALSE otherwise
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,currRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p=pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,currRing);
      for (int i=gitfan_satstdSaturatingVariables.size()-1; i>=0; i--)
      {
        int li = gitfan_satstdSaturatingVariables[i];
        mm[li]=si_min(mm[li],m0[li]);
        if (mm[li]>0) nonTrivialSaturationToBeDone=true;
      }
      // abort if the minimum is zero in each component
      if (nonTrivialSaturationToBeDone==false) break;
    }
    if (nonTrivialSaturationToBeDone==true)
    {
      // std::cout << "simplifying!" << std::endl;
      p=p_Copy(strat->P.p,currRing);
      strat->P.Init();
      strat->P.tailRing = strat->tailRing;
      strat->P.p=p;
      while(p!=NULL)
      {
        for (int i=gitfan_satstdSaturatingVariables.size()-1; i>=0; i--)
        {
          int li = gitfan_satstdSaturatingVariables[i];
          p_SubExp(p,li,mm[li],currRing);
        }
        p_Setm(p,currRing);
        pIter(p);
      }
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  else
  {
    poly p=strat->P.t_p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,strat->tailRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p = pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,strat->tailRing);
      for(int i=gitfan_satstdSaturatingVariables.size()-1; i>=0; i--)
      {
        int li = gitfan_satstdSaturatingVariables[i];
        mm[li]=si_min(mm[li],m0[li]);
        if (mm[li]>0) nonTrivialSaturationToBeDone = true;
      }
      // abort if the minimum is zero in each component
      if (!nonTrivialSaturationToBeDone) break;
    }
    if (nonTrivialSaturationToBeDone)
    {
      p=p_Copy(strat->P.t_p,strat->tailRing);
      strat->P.Init();
      strat->P.tailRing = strat->tailRing;
      strat->P.t_p=p;
      while(p!=NULL)
      {
        for(int i=gitfan_satstdSaturatingVariables.size()-1; i>=0; i--)
        {
          int li = gitfan_satstdSaturatingVariables[i];
          p_SubExp(p,li,mm[li],strat->tailRing);
        }
        p_Setm(p,strat->tailRing);
        pIter(p);
      }
      strat->P.GetP();
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  return b; // return TRUE if sp was changed, FALSE if not
}

ideal gfanlib_satStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  int n = rVar(currRing);
  gitfan_satstdSaturatingVariables = std::vector<int>(n);
  for (int i=n-1; i>=0; i--)
    gitfan_satstdSaturatingVariables[i] = i+1;

  ideal stdI = kStd(I,currRing->qideal,h,NULL,NULL,0,0,NULL,sat_vars_sp);
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

static BOOLEAN abort_if_monomial_sp(kStrategy strat)
{
  BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
                     // let it remain FALSE otherwise
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;
    if (pNext(p)==NULL)
    {
      while ((strat->Ll >= 0))
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      return FALSE;
    }
  }
  else
  {
    poly p=strat->P.t_p;
    if (pNext(p)==NULL)
    {
      while ((strat->Ll >= 0))
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      return FALSE;
    }
  }
  return b; // return TRUE if sp was changed, FALSE if not
}

ideal gfanlib_monomialabortStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  ideal stdI = kStd(I,currRing->qideal,h,NULL,NULL,0,0,NULL,abort_if_monomial_sp);
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}
