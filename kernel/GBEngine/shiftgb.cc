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

/* for poly in lmCR/tailTR presentation */
int itoInsert(poly p, const ring r)
{
  /* the below situation (commented out) might happen! */
//   if (r == currRing)
//   {
//     "Current ring is not expected in toInsert";
//     return(0);
//   }
  /* compute the number of insertions */
  int i = p_mLastVblock(p, currRing);
  if (pNext(p) != NULL)
  {
    i = si_max(i, p_LastVblock(pNext(p), r) );
  }
  //  i = uptodeg  - i +1;
  int uptodeg = r->N/r->isLPring;
  //  p_wrp(p,currRing,r); Print("----i:%d",i); PrintLn();
  return uptodeg - i;
}

#endif
