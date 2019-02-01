#include "Singular/libsingular.h"
// same_lt: search an ideal for entries with the same lead term
// cf_at: the coefficient of the term given: cf_at(x+2y+3z,y) is 2

BOOLEAN same_lt2(leftv res, leftv arg)
{
  if ((currRing!=NULL) && (arg!=NULL) && (arg->Typ()==IDEAL_CMD))
  {
    ideal I=(ideal)arg->Data();
    int i,j;
    lists L=(lists)omAllocBin(slists_bin);
    L->Init();
    res->rtyp=LIST_CMD;
    res->data=L;
    for (i=0;i<IDELEMS(I)-1;i++)
    {
      if (I->m[i]!=NULL)
      {
        for(j=IDELEMS(I)-1;j>i;j--)
        {
          if (I->m[j]!=NULL)
          {
            if (p_LmCmp(I->m[i],I->m[j],currRing)==0)
            {
              L->Init(2);
              L->m[0].rtyp=INT_CMD; L->m[0].data=(void*)(long)(i+1);
              L->m[1].rtyp=INT_CMD; L->m[1].data=(void*)(long)(j+1);
              return FALSE;
            }
          }
        }
      }
    }
    return FALSE; /* not found */
  }
  WerrorS("same_lt(ideal)");
  return TRUE;
}

BOOLEAN cf_at(leftv res, leftv arg)
{
  const short t[]={2,POLY_CMD,POLY_CMD};
  if ((currRing!=NULL) && iiCheckTypes(arg,t,1))
  {
    poly p=(poly)arg->Data();
    poly m=(poly)arg->next->Data();
    if (m==NULL) return TRUE;
    while(p!=NULL)
    {
      int c=p_LmCmp(p,m,currRing);
      if (c==-1) // p after m in ordering
      {
        p=NULL;
        break;
      }
      else if (c==0)
        break;
      else
        pIter(p);
    }
    res->rtyp=NUMBER_CMD;
    if (p==NULL)
      res->data=n_Init(0,currRing->cf);
    else
      res->data=n_Copy(pGetCoeff(p),currRing->cf);
    return FALSE;
  }
  WerrorS("expected cf_at(`poly`,`poly`)");
  return TRUE;
}

extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"same_lt2",FALSE,same_lt2);
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"cf_at",FALSE,cf_at);
  VAR return MAX_TOK;
}
