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

// splits a frame (e.g. x(1)*y(5)) m1 into m1 and m2 (e.g. m1=x(1) and m2=y(1))
// according to p which is inside the frame
void k_SplitFrame(poly &m1, poly &m2, int at, const ring r) {
  int lV = r->isLPring;

  number m1Coeff = pGetCoeff(m1);

  int hole = lV * at;
  m2 = p_GetExp_k_n(m1, 1, hole, r);
  m1 = p_GetExp_k_n(m1, hole, r->N, r);

  p_mLPunshift(m2, r);
  p_SetCoeff(m1, m1Coeff, r);

  assume(p_FirstVblock(m1,r) <= 1);
  assume(p_FirstVblock(m2,r) <= 1);
}

#endif
