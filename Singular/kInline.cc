/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    kInline.cc
 *  Purpose: implementation of std related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: kInline.cc,v 1.4 2000-10-04 13:12:01 obachman Exp $
 *******************************************************************/
#ifndef KINLINE_CC
#define KINLINE_CC

#if !defined(NO_KINLINE) || defined(KUTIL_CC)

#include "p_polys.h"
#include "p_Procs.h"
#include "kbuckets.h"
#include "omalloc.h"

#define HAVE_TAIL_BIN
// #define HAVE_LM_BIN

KINLINE skStrategy::skStrategy()
{
  memset(this, 0, sizeof(skStrategy));
  tailRing = currRing;
  P.tailRing = currRing;
  P.lmRing = currRing;
#ifdef HAVE_LM_BIN
  lmBin = omGetStickyBinOfBin(currRing->PolyBin);
#endif
#ifdef HAVE_TAIL_BIN
  tailBin = omGetStickyBinOfBin(currRing->PolyBin);
#endif
}

KINLINE skStrategy::~skStrategy()
{
  if (lmBin != NULL)
    omMergeStickyBinIntoBin(lmBin, currRing->PolyBin);
  if (tailBin != NULL)
    omMergeStickyBinIntoBin(tailBin, 
                            (tailRing!= NULL ? tailRing->PolyBin:
                             currRing->PolyBin));
}
  
KINLINE TSet initT () { return (TSet)omAlloc0(setmax*sizeof(TObject)); }

KINLINE void sLObject::SetLmTail(poly lm, poly p_tail, int use_bucket)
{
  p = lm;
  assume(p != NULL);
  if (use_bucket)
  {
    bucket = kBucketCreate(tailRing);
    kBucketInit(bucket, p_tail, 0);
    pNext(p) = NULL;
  }
  else
  {
    pNext(p) = p_tail;
  }
}

KINLINE void sLObject::Tail_Mult_nn(number n)
{
  if (bucket != NULL)
  {
    kBucket_Mult_n(bucket, n);
  }
  else
  {
    assume(p != NULL);
    pNext(p) = tailRing->p_Procs->p_Mult_nn(pNext(p), n, tailRing);
  }
}

KINLINE void sLObject::Tail_Minus_mm_Mult_qq(poly m, poly q, int lq, 
                                             poly spNoether)
{
  if (bucket != NULL)
  {
    kBucket_Minus_m_Mult_p(bucket, m, q, &lq, spNoether);
  }
  else
  {
    assume(p != NULL);
    int dummy;
    pNext(p) = tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(p), m, q, 
                                                     dummy,spNoether,tailRing);
  }
}

KINLINE void sLObject::Iter(ring r = currRing)
{
  assume(p != NULL);
  if (bucket != NULL)
  {
    p = kBucketExtractLm(bucket);
    if (p == NULL)
    {
      kBucketDestroy(&bucket);
      return;
    }
  }
  else
  {
    pIter(p);
  }
  if (p != NULL)
  {
    if (r != tailRing)
    {
      p = k_LmShallowCopyDelete_tailRing_2_lmRing(p, tailRing, r);
    }
    lmRing = r;
  }
}

KINLINE void sLObject::CanonicalizeP()
{
  if (bucket != NULL)
    kBucketCanonicalize(bucket);
}

KINLINE poly sLObject::GetP(ring get_lmRing = currRing, omBin lmBin = NULL)
{
  kTest_L(this);
  if (p != NULL)
  {
    if (bucket != NULL)
    {
      kBucketClear(bucket, &pNext(p), &pLength);
      kBucketDestroy(&bucket);
      pLength++;
    }
    
    if (get_lmRing == NULL) get_lmRing = currRing;
    if (get_lmRing != lmRing)
    {
      if (lmBin == NULL) lmBin = get_lmRing->PolyBin;
      p = k_LmShallowCopyDelete_tailRing_2_lmRing(p,lmRing, get_lmRing,lmBin);
      lmRing = get_lmRing;
    }
    else if (lmBin != NULL)
    {
      p = p_LmShallowCopyDelete(p, lmRing, lmBin);
    }
  }
    
  kTest_L(this);
  return p;
}


/***************************************************************
 *
 * Conversion of polys
 *
 ***************************************************************/
  
KINLINE poly k_LmInit_lmRing_2_tailRing(poly p, ring lmRing, ring tailRing, omBin tailBin)
{
  int i;
  poly np = p_Init(tailRing, tailBin);
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
  return np;

}

KINLINE poly k_LmInit_tailRing_2_lmRing(poly p, ring tailRing, ring lmRing, omBin lmBin)
{
  return k_LmInit_lmRing_2_tailRing(p, tailRing, lmRing, lmBin);
}

// this should be made more efficient
KINLINE poly k_LmShallowCopyDelete_lmRing_2_tailRing(poly p, ring lmRing, ring tailRing, omBin tailBin)
{
  poly np = k_LmInit_lmRing_2_tailRing(p, lmRing, tailRing, tailBin);
  p_LmFree(p, lmRing);
  return np;
}

KINLINE poly k_LmShallowCopyDelete_tailRing_2_lmRing(poly p, ring tailRing, ring lmRing, omBin lmBin)
{
  poly np = k_LmInit_tailRing_2_lmRing(p, tailRing, lmRing, lmBin);
  p_LmFree(p, tailRing);
  return np;
}

KINLINE poly k_LmInit_lmRing_2_tailRing(poly p, ring lmRing, ring tailRing)
{
  return k_LmInit_lmRing_2_tailRing(p, lmRing, tailRing, tailRing->PolyBin);
}

KINLINE poly k_LmInit_tailRing_2_lmRing(poly p, ring tailRing, ring lmRing)
{
  return  k_LmInit_tailRing_2_lmRing(p, tailRing, lmRing, lmRing->PolyBin);
}

KINLINE poly k_LmShallowCopyDelete_lmRing_2_tailRing(poly p, ring lmRing, ring tailRing)
{
  return k_LmShallowCopyDelete_lmRing_2_tailRing(p, lmRing, tailRing, tailRing->PolyBin);
}

KINLINE poly k_LmShallowCopyDelete_tailRing_2_lmRing(poly p, ring tailRing, ring lmRing)
{
  return  k_LmShallowCopyDelete_tailRing_2_lmRing(p, tailRing, lmRing, lmRing->PolyBin);
}

#endif // defined(KINLINE) || defined(KUTIL_CC)
#endif // KINLINE_CC

