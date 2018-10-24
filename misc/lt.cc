#include "Singular/libsingular.h"
// search an ideal for entries with the same lead term

BOOLEAN same_lt(leftv res, leftv arg)
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
        for(j=i+1;j<IDELEMS(I);j++)
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

extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"same_lt",FALSE,same_lt);
  return MAX_TOK;
}
