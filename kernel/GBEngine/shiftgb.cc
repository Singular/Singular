/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: kernel: utils for shift GB and free GB
*/

#include "kernel/mod2.h"

#ifdef HAVE_SHIFTBBA
#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "coeffs/numbers.h"
#include "kernel/ideals.h"
#include "polys/matpol.h"
#include "polys/kbuckets.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/sbuckets.h"
#include "polys/operations/p_Mult_q.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/kInline.h"
#include "kernel/combinatorics/stairc.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/GBEngine/shiftgb.h"
#include "polys/nc/sca.h"
#include "polys/shiftop.h"

poly p_LPCopyAndShiftLM(poly p, int sh, const ring r)
{
  if (sh == 0 || p == NULL) return p;

  poly q = p_Head(p, r);
  p_mLPshift(q, sh, r);
  pNext(q) = pNext(p);
  return q;
}

int p_mLPmaxPossibleShift(poly p, const ring r)
{
  int uptodeg = r->N/r->isLPring;
  return uptodeg - p_mLastVblock(p, r);
}

#endif
