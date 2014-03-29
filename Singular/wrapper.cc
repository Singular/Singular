#include <string.h>
#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include <kernel/mod2.h>
#include <polys/monomials/ring.h>

#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/GBEngine/kstd1.h>
#include <kernel/ideals.h>

#include <kernel/GBEngine/janet.h>

#include <Singular/subexpr.h>

//extern int (*ListGreatMove)(jList *,jList *,poly);

static BOOLEAN jInitBasis(ideal v, jList **TT,jList **QQ)
{
  if (rHasLocalOrMixedOrdering_currRing())
  {
    WerrorS("janet only for well-orderings");
    return TRUE;
  }

  Initialization(rOrdStr(currRing));

  jList *Q=(jList *)GCM(sizeof(jList));
  Q->root=NULL;

  jList *T=(jList *)GCM(sizeof(jList));
  T->root=NULL;

  for (int i=0; i < IDELEMS(v); i++)
  {
    if (v->m[i]!=NULL)
    {
      Poly *beg=NewPoly(pCopy(v->m[i]));

      InitHistory(beg);
      InitProl(beg);
      InitLead(beg);

      InsertInCount(Q,beg);
    }
  }

  BOOLEAN r= !(ComputeBasis(T,Q));
  *TT=T;
  *QQ=Q;
  return r;
}

/// flag: 0: JB, 1: SB
BOOLEAN jjStdJanetBasis(leftv res, leftv v, int flag)
{
  ideal result;

  jList *T;
  jList *Q;
  ideal I=(ideal)v->Data();
  BOOLEAN is_zero=TRUE;
  for (int i=0; i < IDELEMS(I); i++)
  {
    if ((I->m[i]!=NULL)&& (pIsConstant(I->m[i])))
    {
      goto zero;
    }
    else
     is_zero=FALSE;
  }
  if (is_zero)
    goto zero;
  if (!jInitBasis(I,&T,&Q))
  {
    int dpO=(strstr(rOrdStr(currRing),"dp")!=NULL);
    int ideal_length;
    if (flag==1)
      ideal_length= dpO ? GB_length() : CountList(T);
    else
      ideal_length=CountList(T);

    result=idInit(ideal_length,1);

    int ideal_index=0;

    LCI iT=T->root;

    while(iT)
    {
      pTest(iT->info->root);
      if ((flag==1) && dpO)
      {
        //if (pTotaldegree(iT->info->lead) == pTotaldegree(iT->info->history))
        if (p_Deg(iT->info->lead,currRing) == p_Deg(iT->info->history,currRing))
        {
          result->m[ideal_length-ideal_index-1]=pCopy(iT->info->root);
          if (!nGreaterZero(pGetCoeff(iT->info->root)))
            result->m[ideal_length-ideal_index-1]
                                  =pNeg(result->m[ideal_length-ideal_index-1]);

          ideal_index++;
        }
      }
      else
      {
        result->m[ideal_length-ideal_index-1]=pCopy(iT->info->root);
        if (!nGreaterZero(pGetCoeff(iT->info->root)))
          result->m[ideal_length-ideal_index-1]
                                  =pNeg(result->m[ideal_length-ideal_index-1]);

        ideal_index++;
      }
      iT=iT->next;
    }

    if ((flag==1) && (dpO==0))
    {
      //Print ("interred\n");
      result=kInterRedOld(result);
      idSkipZeroes(result);
    }
    res->data = (char *)result;
    res->rtyp = IDEAL_CMD;
    DestroyList(Q);
    DestroyList(T);
    return FALSE;
  }
  else
    return TRUE;

zero:
  result=idInit(1,1);
  if (!is_zero) result->m[0]=pOne();
  res->data = (char *)result;
  res->rtyp = IDEAL_CMD;
  return FALSE;
}
