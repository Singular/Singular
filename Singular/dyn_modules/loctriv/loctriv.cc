/*
 *
 *
 *  Module file for computing locally trivial vector fields
 *  by a Groebner basis computation similar to the syzygies
 *
 */
#include "kernel/ideals.h"
#include "libpolys/polys/monomials/ring.h"
#include "kernel/GBEngine/kstd1.h"
#include "libpolys/polys/prCopy.h"
#include "Singular/tok.h"
#include "Singular/ipshell.h"
#include "Singular/mod_lib.h"
#include "Singular/ipid.h"

static BOOLEAN kstd(leftv res, leftv args)
{
  const short t1[]={2,MODUL_CMD,INT_CMD};
  if (iiCheckTypes(args,t1,1))
  {
    ideal h1=(ideal)args->CopyD();
    int k=(int)(long)args->next->Data();
    ideal s_h1;
    ideal s_h3;
    ring orig_ring;
    ring syz_ring;
    intvec *w=NULL;

    assume(currRing != NULL);
    orig_ring=currRing;
    syz_ring=rAssure_SyzComp(orig_ring);
    rSetSyzComp(k,syz_ring);
    rChangeCurrRing(syz_ring);

    if (orig_ring != syz_ring)
    {
      s_h1=idrCopyR_NoSort(h1,orig_ring,syz_ring);
    }
    else
    {
      s_h1 = h1;
    }

    s_h3=kStd(s_h1,NULL,testHomog,&w,NULL,k);

    if (orig_ring != syz_ring)
    {
      idDelete(&s_h1);
      idSkipZeroes(s_h3);
      rChangeCurrRing(orig_ring);
      s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
      rKill(syz_ring);
    }
    else
    {
      idSkipZeroes(s_h3);
    }
    res->data=(void *)s_h3;
    res->rtyp=MODUL_CMD;
    return FALSE;
  }
  else
    return TRUE;
}

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(loctriv)(SModulFunctions* p)
{
  p->iiAddCproc("loctriv.so","kstd",FALSE,kstd);
  return (MAX_TOK);
}

