#include "kernel/mod2.h"
#include "polys/prCopy.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "Singular/ipshell.h"
#include "Singular/mod_lib.h"
#include "Singular/ipid.h"
static ideal idPartialGB (ideal  h1, int k)
{
  ideal s_h1;
  intvec *w=NULL;

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rSetSyzComp(k,syz_ring);
  rChangeCurrRing(syz_ring);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring, syz_ring);
  }
  else
  {
    s_h1 = h1;
  }

  ideal s_h3=kStd(s_h1,NULL,testHomog,&w,NULL,k);

  if (s_h3==NULL)
  {
    return idFreeModule(IDELEMS(h1));
  }

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    idSkipZeroes(s_h3);
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rDelete(syz_ring);
    idTest(s_h3);
    return s_h3;
  }

  idSkipZeroes(s_h3);
  idTest(s_h3);
  return s_h3;
}

static BOOLEAN partialStd(leftv res, leftv h)
{
  const short t[]={2,MODUL_CMD,INT_CMD};
  if (iiCheckTypes(h,t,1))
  {
    ideal  i1=(ideal)h->CopyD();
    int  i2=(int)((long)h->next->Data());
    res->rtyp=MODUL_CMD;
    res->data=idPartialGB(i1,i2);
    return FALSE;
  }
  return TRUE;
}
//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(partialgb)(SModulFunctions* p)
{
  p->iiAddCproc("partialgb.so","partialStd",FALSE,partialStd);
  return (MAX_TOK);
}

