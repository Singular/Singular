#include <kernel/mod2.h>
#ifdef HAVE_F5
#include <unistd.h>
#include <kernel/structs.h>
#include <kernel/kutil.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <polys/monomials/p_polys.h>
#include <polys/templates/p_Procs.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <polys/kbuckets.h>
#include <polys/weight.h>
#include <misc/intvec.h>
#include <kernel/polys.h>
#include <kernel/f5c.h>
#include <kernel/timer.h>
/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal f5cMain(ideal id, ring r) {

  return id;
}

#endif
// HAVE_F5
