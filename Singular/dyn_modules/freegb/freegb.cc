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
    if ((r->order[2]!=0)
    || (rHasLocalOrMixedOrdering(r)))
    {
      WerrorS("only for rings with a global ordering of one block");
      return TRUE;
    }
    ring R=freeAlgebra(r,d);
    res->rtyp=RING_CMD;
    res->data=R;
    return R==NULL;
  }
  return TRUE;
}
#endif

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(freegb)(SModulFunctions* p)
{
#ifdef HAVE_SHIFTBBA
  p->iiAddCproc("freegb.so","freegb",FALSE,freegb);
#endif
  return (MAX_TOK);
}
