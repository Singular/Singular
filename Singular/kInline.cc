/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    kInline.cc
 *  Purpose: implementation of std related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: kInline.cc,v 1.6 2000-10-19 15:25:41 obachman Exp $
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
  
/***************************************************************
 *
 * Operation on TObjects
 *
 ***************************************************************/

KINLINE TSet initT () 
{ 
  TSet T = (TSet)omAlloc0(setmax*sizeof(TObject)); 
  for (int i=0; i<setmax; i++)
  {
    T[i].tailRing = currRing;
  }
  return T;
}

// initialization
KINLINE void sTObject::Set(ring r)
{
  tailRing = r;
}
KINLINE void sTObject::Init(ring r)
{
  memset(this, 0, sizeof(sTObject));
  Set(r);
}
KINLINE sTObject::sTObject(ring r)
{
  Init(r);
}
KINLINE void sTObject::Set(poly p_in, ring r)
{
  if (r != currRing)
  {
    assume(r == tailRing);
    p_Test(p_in, r);
    t_p = p_in;
  }
  else
  {
    pp_Test(p_in, currRing, tailRing);
    p = p_in;
  }
}

KINLINE sTObject::sTObject(poly p_in, ring r) 
{
  Init(r);
  Set(p_in, r);
}

KINLINE void sTObject::Set(poly p_in, ring c_r, ring t_r)
{
  if (c_r != t_r)
  {
    assume(c_r == currRing && t_r == tailRing);
    pp_Test(p_in, currRing, t_r);
    p = p_in;
  }
  else 
  {
    Set(p_in, c_r);
  }
}

KINLINE sTObject::sTObject(poly p_in, ring c_r, ring t_r)
{
  Init(t_r);
  Set(p_in, c_r, t_r);
}

KINLINE sTObject::sTObject(sTObject* T, int copy)
{
  *this = *T;
  if (copy)
  {
    if (t_p != NULL)
    {
      t_p = p_Copy(t_p, tailRing);
      p = k_LmInit_tailRing_2_currRing(t_p, tailRing);
    }
    else
    {
      p = p_Copy(p, currRing, tailRing);
    }
  }
}

KINLINE void sTObject::Delete()
{
  if (t_p != NULL)
  {
    p_Delete(&t_p, tailRing);
    if (p != NULL)
      p_LmFree(p, currRing);
  }
  else
  {
    p_Delete(&p, currRing, tailRing);
  }
}

// Lm Extraction
KINLINE poly sTObject::GetLm()
{
  if (p == NULL && t_p != NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);

  return p;
}
KINLINE poly sTObject::GetLm(ring r)
{
  if (r == currRing) 
    return GetLm();
  
  if (t_p == NULL && p != NULL)
    t_p = k_LmInit_currRing_2_tailRing(p, tailRing);

  return t_p;
}

// Iterations
KINLINE void sTObject::LmDeleteAndIter()
{
  assume(p != NULL || t_p != NULL);
  if (t_p != NULL)
  {
    t_p = p_LmDeleteAndNext(t_p, tailRing);
    if (p != NULL)
    {
      p_LmFree(p, currRing);
      p = NULL;
    }
  }
  else
  {
    p = p_LmDeleteAndNext(p, currRing);
  }
}

// arithmetic
KINLINE void sTObject::Mult_nn(number n)
{
  if (t_p != NULL)
  {    t_p = tailRing->p_Procs->p_Mult_nn(t_p, n, tailRing);
    if (p != NULL) pSetCoeff0(p, pGetCoeff(t_p));
  }
  else
  {
    p = p_Mult_nn(p, n, currRing, tailRing);
  }
}

/***************************************************************
 *
 * Operation on LObjects
 *
 ***************************************************************/
// Initialization
KINLINE void sLObject::Init(ring r)
{
  memset(this, 0, sizeof(sLObject));
  Set(r);
}
KINLINE sLObject::sLObject(ring r)
{
  Init(r);
}
KINLINE sLObject::sLObject(poly p_in, ring r)
{
  Init(r);
  Set(p_in, r);
}

KINLINE sLObject::sLObject(poly p_in, ring c_r, ring t_r)
{
  Init(t_r);
  Set(p_in, c_r, t_r);
}

KINLINE void sLObject::SetLmTail(poly lm, poly p_tail, int use_bucket, ring tailRing)
{
  Set(lm, tailRing);
  if (use_bucket)
  {
    bucket = kBucketCreate(tailRing);
    kBucketInit(bucket, p_tail, 0);
    pNext(lm) = NULL;
  }
  else
  {
    pNext(lm) = p_tail;
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
    poly _p = (t_p != NULL ? t_p : p);
    assume(_p != NULL);
    pNext(_p) = tailRing->p_Procs->p_Mult_nn(pNext(_p), n, tailRing);
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
    poly _p = (t_p != NULL ? t_p : p);
    assume(_p != NULL);
    int dummy;
    pNext(_p) = tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(_p), m, q, 
                                                      dummy,spNoether,tailRing);
  }
}

KINLINE void sLObject::LmDeleteAndIter()
{
  sTObject::LmDeleteAndIter();
  if (bucket != NULL)
  {
    poly _p = kBucketExtractLm(bucket);
    if (_p == NULL)
    {
      kBucketDestroy(&bucket);
      return;
    }
    Set(_p, tailRing);
  }
}

KINLINE void sLObject::CanonicalizeP()
{
  kTest_L(this);

  if (bucket != NULL)
    kBucketCanonicalize(bucket);

  if (p == NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);
}

KINLINE poly sLObject::GetP(omBin lmBin = NULL)
{
  kTest_L(this);
  if (p == NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing, 
                                     (lmBin!=NULL?lmBin:currRing->PolyBin));
  else if (lmBin != NULL && lmBin != currRing->PolyBin)
    p = p_LmShallowCopyDelete(p, currRing, lmBin);
  
  if (bucket != NULL)
  {
    kBucketClear(bucket, &pNext(p), &pLength);
    kBucketDestroy(&bucket);
    pLength++;
    if (t_p != NULL) pNext(t_p) = pNext(p);
  }
  kTest_L(this);
  return p;
}

/***************************************************************
 *
 * Conversion of polys
 *
 ***************************************************************/
  
KINLINE poly k_LmInit_currRing_2_tailRing(poly p, ring tailRing, omBin tailBin)
{
  
  poly np = p_LmInit(p, currRing, tailRing, tailBin);
  pNext(np) = pNext(p);
  pSetCoeff0(np, pGetCoeff(p));
  return np;
}

KINLINE poly k_LmInit_tailRing_2_currRing(poly p, ring tailRing, omBin lmBin)
{
  poly np = p_LmInit(p, tailRing, currRing, lmBin);
  pNext(np) = pNext(p);
  pSetCoeff0(np, pGetCoeff(p));
  return np;
}

// this should be made more efficient
KINLINE poly k_LmShallowCopyDelete_currRing_2_tailRing(poly p, ring tailRing, omBin tailBin)
{
  poly np = k_LmInit_currRing_2_tailRing(p, tailRing, tailBin);
  p_LmFree(p, currRing);
  return np;
}

KINLINE poly k_LmShallowCopyDelete_tailRing_2_currRing(poly p, ring tailRing, omBin lmBin)
{
  poly np = k_LmInit_tailRing_2_currRing(p, tailRing, lmBin);
  p_LmFree(p, tailRing);
  return np;
}

KINLINE poly k_LmInit_currRing_2_tailRing(poly p, ring tailRing)
{
  return k_LmInit_currRing_2_tailRing(p, tailRing, tailRing->PolyBin);
}

KINLINE poly k_LmInit_tailRing_2_currRing(poly p, ring tailRing)
{
  return  k_LmInit_tailRing_2_currRing(p, tailRing, currRing->PolyBin);
}

KINLINE poly k_LmShallowCopyDelete_currRing_2_tailRing(poly p, ring tailRing)
{
  return k_LmShallowCopyDelete_currRing_2_tailRing(p, tailRing, tailRing->PolyBin);
}

KINLINE poly k_LmShallowCopyDelete_tailRing_2_currRing(poly p, ring tailRing)
{
  return  k_LmShallowCopyDelete_tailRing_2_currRing(p, tailRing, currRing->PolyBin);
}

/***************************************************************
 *
 * Routines for backwards-Compatibility
 * 
 * 
 ***************************************************************/
KINLINE poly ksOldSpolyRed(poly p1, poly p2, poly spNoether)
{
  LObject L(p2);
  TObject T(p1);

  ksReducePoly(&L, &T, spNoether);
  
  return L.GetLm();
}

KINLINE poly ksOldSpolyRedNew(poly p1, poly p2, poly spNoether)
{
  LObject L(p_Copy(p2, currRing));
  TObject T(p1);

  ksReducePoly(&L, &T, spNoether);
  
  return L.GetLm();
}

KINLINE poly ksOldCreateSpoly(poly p1, poly p2, poly spNoether, ring r)
{
  LObject L(r);
  L.p1 = p1;
  L.p2 = p2;
  
  ksCreateSpoly(&L, spNoether);
  return L.GetLm();
}

KINLINE void ksOldSpolyTail(poly p1, poly q, poly q2, poly spNoether, ring r)
{
  LObject L(q,  currRing, r);
  TObject T(p1, currRing, r);

  ksReducePolyTail(&L, &T, q2, spNoether);
}
#endif // defined(KINLINE) || defined(KUTIL_CC)
#endif // KINLINE_CC

