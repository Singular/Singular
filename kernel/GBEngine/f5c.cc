


#include "kernel/mod2.h"
#ifdef HAVE_F5
#include <unistd.h>
#include "kernel/structs.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/polys.h"
#include "polys/monomials/p_polys.h"
#include "polys/templates/p_Procs.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/f5c.h"
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
