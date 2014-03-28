%{
/*
 *
 *
 *  Module file for computing locally trivial vector fields
 *  by a Groebner basis computation similar to the syzygies
 *
 */
#include <kernel/ideals.h>
#include <kernel/ring.h>
#include <kernel/GBEngine/kstd1.h>
#include <kernel/prCopy.h>
%}

//#package="loctriv";
package="kstd";
version="$Id $";
info="LIBRARY: partstd.lib  EXTERNALLY CONTROLLED GROEBNER\\n\\n kstd(module i1, int i2);       externally controlled std.\\n";

%procedures

module kstd (
  module h1,
  int k
)
{
%declaration;

  ideal s_h1;
  ideal s_h3;
  int j;
  ring orig_ring;
  ring syz_ring;
  intvec *w=NULL;

%typecheck;

  assume(currRing != NULL);
  orig_ring=currRing;
  syz_ring=rCurrRingAssure_SyzComp();

  rSetSyzComp(k);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring);
  }
  else
  {
    s_h1 = h1;
  }

  s_h3=kStd(s_h1,NULL,testHomog,&w,NULL,k);

  if (s_h3==NULL)
  {
    %return = (void *)(idFreeModule(IDELEMS(h1)));
  }

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    idSkipZeroes(s_h3);
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
    rKill(syz_ring);
  }
  else
  {
    idSkipZeroes(s_h3);
  }
  %return = (void *)s_h3;
}

%C
