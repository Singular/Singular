#include <kernel/mod2.h>
#ifdef HAVE_F5C
#include <unistd.h>
#include <kernel/structs.h>
#include <kernel/kutil.h>
#include <omalloc/omalloc.h>
#include <polys/polys.h>
#include <polys/monomials/p_polys.h>
#include <polys/templates/p_Procs.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <polys/kbuckets.h>
#include <polys/weight.h>
#include <misc/intvec.h>
#include <libpolys/polys/polys.h>
#include <kernel/f5c.h>
#include <kernel/F5cData.h>
#include <kernel/F5cLists.h>
#include <kernel/timer.h>
/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal f5cMain(ideal id, ring r) {
  Print("SHORT EXP VECTOR 1:  %ld\n", pGetShortExpVector(id->m[0]));
  int* expVec   = new int[(r->N)+1];
  pGetExpV(id->m[0],expVec);
  Print("EXP VECTOR 1: %d\n",expVec[1]);
  Label* label  = new Label(expVec);
  Print("EXP VECTOR 2: %d\n", label->getExpVec()[1]);
  Print("SHORT EXP VECTOR 2:  %ld\n", label->getShortExpVec());
  //Print("%ld\n", label->computeShortExpVec(expVec)); 
  Print("SHORT EXP VECTOR 1:  %ld\n", pGetShortExpVector(id->m[1]));
  //int* expVec   = new int[(r->N)+1];
  pGetExpV(id->m[1],expVec);
  Print("EXP VECTOR 1: %d\n",expVec[1]);
  Label* label2  = new Label(expVec);
  Print("EXP VECTOR 2: %d\n", label2->getExpVec()[1]);
  Print("SHORT EXP VECTOR 2:  %ld\n", label2->getShortExpVec());

  return id;
}

#endif
// HAVE_F5C
