/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    kInline.cc
 *  Purpose: implementation of std related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: kInline.cc,v 1.2 2000-09-18 09:19:06 obachman Exp $
 *******************************************************************/
#ifndef KINLINE_CC
#define KINLINE_CC

#if !defined(NO_KINLINE) || defined(KUTIL_CC)

#include "polys.h"

KINLINE TSet initT () { return (TSet)omAlloc0(setmax*sizeof(TObject)); }

KINLINE poly sTObject::SetP(poly new_p)
{
  poly old_p = p;
  memset((void*) this, 0, sizeof(sTObject));
  p = new_p;
  sev = pGetShortExpVector(p);
  return p;
}

// this should be made more efficient
KINLINE poly k_LmShallowCopyDelete_lmRing_2_tailRing(poly p, ring lmRing, ring tailRing)
{
  int i;
  poly np = p_New(tailRing);
  pNext(np) = pNext(p);
  pSetCoeff0(np, pGetCoeff(p));
  
  for (i=1; i<=tailRing->N;i++)
  {
    p_SetExp(np, i, p_GetExp(p, i,lmRing), tailRing);
  }
  if (rRing_has_Comp(tailRing))
  {
    p_SetComp(np, p_GetComp(p,lmRing), tailRing);
  }
  p_Setm(np, tailRing);
  p_LmFree(p, lmRing);
  return np;
}


#if 0
// spoly related stuff
KINLINE poly sLObject::Iter()
{
  if (bucket != NULL)
  {
    p = kBucketExtractLm(bucket);
    if (p == NULL) kBucketDestroy(&bucket);
  }
  else
  {
    if (p != NULL && pNext(p) != NULL)
      p = pNext(p);
  }
  lmRing = tailRing;
  return p;
}
KINLINE void sLObject::Tail_Minus_mm_Mult_qq(poly m, poly qq, int lq, poly spNoether)
{
  if (is_bucket)
  {
    kBucket_Minus_m_Mult_p(bucket, m, &lq, spNoether);
  }
  else
  {
    pNext(p) = tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(p),
                                                     m, qq, lq, spNoether, tailRing);
  }
}
KINLINE poly sLObject::GetP(ring LmRing)
{
  assume(p != NULL);
  if (bucket != NULL)
  {
    poly new_tail;
    kBucketClear((kBucket_pt) bucket, &new_tail, &pLength);
    kBucketDestroy(&bucket);
    pLength++;
    pNext(p) = new_tail;
  }
  if (lmRing != LmRing)
  {
    p = p_LmShallowMoveDelete(p, LmRing, lmRing);
    lmRing = LmRing;
  }
  return p;
}

#endif

#endif // defined(KINLINE) || defined(KUTIL_CC)
#endif // KINLINE_CC

