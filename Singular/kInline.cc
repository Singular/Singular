/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    kInline.cc
 *  Purpose: implementation of std related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: kInline.cc,v 1.12 2000-11-03 14:50:16 obachman Exp $
 *******************************************************************/
#ifndef KINLINE_CC
#define KINLINE_CC

#if !defined(NO_KINLINE) || defined(KUTIL_CC)

#include "p_polys.h"
#include "p_Procs.h"
#include "kbuckets.h"
#include "omalloc.h"

#define HAVE_TAIL_BIN
// Hmm ... this I don't understand:
// with HAVE_LM_BIN, cyclic_7 is appr. 10% slower (on Intel)
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
                            (tailRing != NULL ? tailRing->PolyBin:
                             currRing->PolyBin));
  if (currRing != tailRing)
    rKillModifiedRing(tailRing);
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

KINLINE TObject** initR()
{
  return (TObject**) omAlloc0(setmax*sizeof(TObject*));
}

KINLINE unsigned long* initsevT()
{
  return (unsigned long*) omAlloc0(setmax*sizeof(unsigned long));
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

KINLINE void sTObject::Clear()
{
  p = NULL;
  t_p = NULL;
}

KINLINE poly sTObject::GetLmCurrRing()
{
  if (p == NULL && t_p != NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);

  return p;
}
KINLINE poly sTObject::GetLmTailRing()
{
  if (t_p == NULL)
  {
    if (p != NULL && tailRing != currRing)
    {
      t_p = k_LmInit_currRing_2_tailRing(p, tailRing);
      return t_p;
    }
    return p;
  }
  return t_p;
}
KINLINE poly sTObject::GetLm(ring r)
{
  assume(r == tailRing || r == currRing);
  if (r == currRing) 
    return GetLmCurrRing();
  
  if (t_p == NULL && p != NULL)
    t_p = k_LmInit_currRing_2_tailRing(p, tailRing);

  return t_p;
}

KINLINE void sTObject::GetLm(poly &p_r, ring &r_r) const
{
  if (t_p != NULL)
  {
    p_r = p;
    r_r = tailRing;
  }
  else
  {
    p_r = p;
    r_r = currRing;
  }
}

KINLINE void sTObject::SetLmCurrRing()
{
  if (p == NULL && t_p != NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);
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

KINLINE void 
sTObject::ShallowCopyDelete(ring new_tailRing, omBin new_tailBin,
                            pShallowCopyDeleteProc p_shallow_copy_delete)
{
  if (new_tailBin == NULL) new_tailBin = new_tailRing->PolyBin;
  if (t_p != NULL)
  {
    t_p = p_shallow_copy_delete(t_p, tailRing, new_tailRing, new_tailBin);
    if (p != NULL) 
      pNext(p) = pNext(t_p);
    if (new_tailRing == currRing)
    {
      if (p == NULL) p = t_p;
      else p_LmFree(t_p, tailRing);
      t_p = NULL;
    }
  }
  else if (p != NULL)
  {
    if (pNext(p) != NULL)
    {
      pNext(p) = p_shallow_copy_delete(pNext(p), 
                                       tailRing, new_tailRing, new_tailBin);
    }
    if (new_tailRing != currRing)
    {
      t_p = k_LmInit_currRing_2_tailRing(p, new_tailRing);
      pNext(t_p) = pNext(p);
    }
  }
  if (max != NULL)
  {
    if (new_tailRing == currRing)
    {
      p_LmFree(max, tailRing);
      max = NULL;
    }
    else
      max = p_shallow_copy_delete(max,tailRing,new_tailRing,tailRing->PolyBin);
  }
  tailRing = new_tailRing;
}
  
KINLINE int sTObject::pFDeg()
{
  if (p != NULL) return ::pFDeg(p, currRing);
  return tailRing->pFDeg(t_p, tailRing);
}

extern void pCleardenom(poly p);
extern void pNorm(poly p);
// manipulations
KINLINE void  sTObject::pCleardenom()
{
  assume(p != NULL);
  ::pCleardenom(p);
  if (t_p != NULL)
    pSetCoeff0(t_p, pGetCoeff(p));
}

KINLINE void  sTObject::pNorm()
{
  assume(p != NULL);
  ::pNorm(p);
  if (t_p != NULL)
    pSetCoeff0(t_p, pGetCoeff(p));
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

KINLINE void 
sLObject::ShallowCopyDelete(ring new_tailRing, 
                            pShallowCopyDeleteProc p_shallow_copy_delete)
{
  if (bucket != NULL)
    kBucketShallowCopyDelete(bucket, new_tailRing, new_tailRing->PolyBin,
                             p_shallow_copy_delete);
  sTObject::ShallowCopyDelete(new_tailRing, 
                              new_tailRing->PolyBin,p_shallow_copy_delete);
}

KINLINE void sLObject::SetShortExpVector()
{
  if (t_p != NULL)
  {
    sev = p_GetShortExpVector(t_p, tailRing);
  }
  else 
  {
    sev = p_GetShortExpVector(p, currRing);
  }
}

KINLINE sLObject& sLObject::operator=(const sTObject& t)
{
  memset(this, 0, sizeof(*this));
  memcpy(this, &t, sizeof(sTObject));
  return *this;
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
 * Lcm business
 *
 ***************************************************************/
// get m1 = LCM(LM(p1), LM(p2))/LM(p1)
//     m2 = LCM(LM(p1), LM(p2))/LM(p2)
KINLINE BOOLEAN k_GetLeadTerms(const poly p1, const poly p2, const ring p_r, 
                               poly &m1, poly &m2, const ring m_r)
{
  p_LmCheckPolyRing(p1, p_r);
  p_LmCheckPolyRing(p2, p_r);
  
  int i;
  Exponent_t x;
  m1 = p_Init(m_r);
  m2 = p_Init(m_r);
  
  for (i = p_r->N; i; i--)
  {
    x = p_GetExpDiff(p1, p2, i, p_r);
    if (x > 0)
    {
      if (x > (long) m_r->bitmask) goto false_return;
      p_SetExp(m2,i,x, m_r);
      p_SetExp(m1,i,0, m_r);
    }
    else
    {
      if (-x > (long) m_r->bitmask) goto false_return;
      p_SetExp(m1,i,-x, m_r);
      p_SetExp(m2,i,0, m_r);
    }
  }

  p_Setm(m1, m_r);
  p_Setm(m2, m_r);
  return TRUE;
  
  false_return:
  p_LmFree(m1, m_r);
  p_LmFree(m2, m_r);
  m1 = m2 = NULL;
  return FALSE;
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
  
  return L.GetLmCurrRing();
}

KINLINE poly ksOldSpolyRedNew(poly p1, poly p2, poly spNoether)
{
  LObject L(p_Copy(p2, currRing));
  TObject T(p1);

  ksReducePoly(&L, &T, spNoether);
  
  return L.GetLmCurrRing();
}

KINLINE poly ksOldCreateSpoly(poly p1, poly p2, poly spNoether, ring r)
{
  LObject L(r);
  L.p1 = p1;
  L.p2 = p2;
  
  ksCreateSpoly(&L, spNoether);
  return L.GetLmCurrRing();
}

void ksOldSpolyTail(poly p1, poly q, poly q2, poly spNoether, ring r)
{
  LObject L(q,  currRing, r);
  TObject T(p1, currRing, r);

  ksReducePolyTail(&L, &T, q2, spNoether);
}

KINLINE poly redtailBba (poly p,int pos,kStrategy strat)
{
  LObject L(p, currRing, strat->tailRing);
  return redtailBba(&L, pos, strat);
}

#endif // defined(KINLINE) || defined(KUTIL_CC)
#endif // KINLINE_CC

