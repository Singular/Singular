#include "Singular/libsingular.h"

#ifdef HAVE_SHIFTBBA
static BOOLEAN freegb(leftv res, leftv args)
{
  const short t1[]={2,RING_CMD,INT_CMD};
  if (iiCheckTypes(args,t1,1))
  {
    ring r=(ring)args->Data();
    int d=(int)(long)args->next->Data();
    if (d<2)
    {
      WerrorS("degree must be >=2");
      return TRUE;
    }
    int i=0;
    while(r->order[i]!=0)
    {
      if ((r->order[i]==ringorder_c) ||(r->order[i]==ringorder_C)) i++;
      else if ((r->block0[i]==1)&&(r->block1[i]==r->N)) i++;
      else
      {
        WerrorS("only for rings with a global ordering of one block");
        return TRUE;
      }
    }
    if ((r->order[i]!=0)
    || (rHasLocalOrMixedOrdering(r)))
    {
      //WerrorS("only for rings with a global ordering of one block");
      Werror("only for rings with a global ordering of one block,i=%d, o=%d",i,r->order[i]);
      return TRUE;
    }
    ring R=freeAlgebra(r,d);
    res->rtyp=RING_CMD;
    res->data=R;
    return R==NULL;
  }
  return TRUE;
}

static BOOLEAN stest(leftv res, leftv args)
{
  const short t[]={2,POLY_CMD,INT_CMD};
  if (iiCheckTypes(args,t,1))
  {
    poly p=(poly)args->CopyD();
    args=args->next;
    int sh=(int)((long)(args->Data()));
    if (sh<0)
    {
      WerrorS("negative shift for pLPshift");
      return TRUE;
    }
    int L = pLastVblock(p);
    if (L+sh > currRing->N/currRing->isLPring)
    {
      WerrorS("pLPshift: too big shift requested\n");
      return TRUE;
    }
    p_LPshift(p,sh,currRing);
    res->data = p;
    res->rtyp = POLY_CMD;
    return FALSE;
  }
  else return TRUE;
}

static BOOLEAN btest(leftv res, leftv h)
{
  const short t[]={1,POLY_CMD};
  if (iiCheckTypes(h,t,1))
  {
    poly p=(poly)h->Data();
    res->rtyp = INT_CMD;
    res->data = (void*)(long)pLastVblock(p);
    return FALSE;
  }
  else return TRUE;
}

/*==================== divide-test for freeGB  =================*/
static BOOLEAN lpLmDivides(leftv res, leftv h)
{
  const short t1[]={2,POLY_CMD,POLY_CMD};
  const short t2[]={2,IDEAL_CMD,POLY_CMD};
  if (iiCheckTypes(h,t1,0))
  {
    poly p=(poly)h->Data();
    poly q=(poly)h->next->Data();
    res->rtyp = INT_CMD;
    res->data = (void*)(long)p_LPDivisibleBy(p, q, currRing);
    return FALSE;
  }
  else if (iiCheckTypes(h,t2,1))
  {
    ideal I=(ideal)h->Data();
    poly q=(poly)h->next->Data();
    res->rtyp = INT_CMD;
    for(int i=0;i<IDELEMS(I);i++)
    {
      if (p_LPDivisibleBy(I->m[i],q, currRing))
      {
        res->data=(void*)(long)1;
        return FALSE;
      }
    }
    res->data=(void*)(long)0;
    return FALSE;
  }
  else return TRUE;
}

/*==================== get var for freeGB  ====================*/
static BOOLEAN lpVarAt(leftv res, leftv h)
{
  const short t[]={2,POLY_CMD,INT_CMD};
  if (iiCheckTypes(h,t,1))
  {
    poly p=(poly)h->Data();
    int pos=(int)((long)(h->next->Data()));
    res->rtyp = POLY_CMD;
    res->data = p_LPVarAt(p, pos, currRing);
    return FALSE;
  }
  else return TRUE;
}
#endif

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(freegb)(SModulFunctions* p)
{
#ifdef HAVE_SHIFTBBA
  p->iiAddCproc("freegb.so","freeAlgebra",FALSE,freegb);
  p->iiAddCproc("freegb.so","stest",TRUE,stest);
  p->iiAddCproc("freegb.so","btest",TRUE,btest);
  p->iiAddCproc("freegb.so","lpLmDivides",TRUE,lpLmDivides);
  p->iiAddCproc("freegb.so","lpVarAt",TRUE,lpVarAt);
#endif
  return (MAX_TOK);
}
