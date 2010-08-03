/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    kInline.cc
 *  Purpose: implementation of std related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef KINLINE_CC
#define KINLINE_CC

#if !defined(NO_KINLINE) || defined(KUTIL_CC)

#include <kernel/p_polys.h>
#include <kernel/polys.h>
#include <kernel/p_Procs.h>
#include <kernel/kbuckets.h>
#include <omalloc.h>

#define HAVE_TAIL_BIN
// This doesn't really work, fixme, if necessary
// #define HAVE_LM_BIN


KINLINE TObject* skStrategy::S_2_T(int i)
{
  assume(i>= 0 && i<=sl);
  assume(S_2_R[i] >= 0 && S_2_R[i] <= tl);
  TObject* TT = R[S_2_R[i]];
  assume(TT != NULL && TT->p == S[i]);
  return TT;
}

KINLINE TObject* skStrategy::s_2_t(int i)
{
  if (i >= 0 && i <= sl)
  {
    int sri= S_2_R[i];
    if (sri >= 0 && sri <= tl)
    {
      TObject* t = R[sri];
      if (t != NULL && t->p == S[i])
        return t;
    }
    // last but not least, try kFindInT
    sri = kFindInT(S[i], T, tl);
    if (sri >= 0)
      return &(T[sri]);
  }
  return NULL;
}

KINLINE poly skStrategy::kNoetherTail()
{
  if (tailRing == currRing)
    return kNoether;
  else
  {
    assume((kNoether == NULL && t_kNoether == NULL) ||
           (kNoether != NULL && t_kNoether != NULL));
    return t_kNoether;
  }
}

/***************************************************************
 *
 * Operation on TObjects
 *
 ***************************************************************/

KINLINE TSet initT ()
{
  TSet T = (TSet)omAlloc0(setmaxT*sizeof(TObject));
  for (int i=setmaxT-1; i>=0; i--)
  {
    T[i].tailRing = currRing;
    T[i].i_r = -1;
  }
  return T;
}

KINLINE TObject** initR()
{
  return (TObject**) omAlloc0(setmaxT*sizeof(TObject*));
}

KINLINE unsigned long* initsevT()
{
  return (unsigned long*) omAlloc0(setmaxT*sizeof(unsigned long));
}

// initialization
KINLINE void sTObject::Set(ring r)
{
  tailRing = r;
}
KINLINE void sTObject::Init(ring r)
{
  memset(this, 0, sizeof(sTObject));
  i_r = -1;
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
  ecart = 0;
  length = 0;
  pLength = 0;
  FDeg = 0;
  is_normalized = FALSE;
}

KINLINE void sTObject::Copy()
{
  if (t_p != NULL)
  {
    t_p = p_Copy(t_p, tailRing);
    if (p != NULL)
    {
      p = p_Head(p, currRing);
      if (pNext(t_p) != NULL) pNext(p) = pNext(t_p);
    }
  }
  else
  {
    p = p_Copy(p, currRing, tailRing);
  }
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
    p_r = t_p;
    r_r = tailRing;
  }
  else
  {
    p_r = p;
    r_r = currRing;
  }
}

KINLINE BOOLEAN sTObject::IsNull() const
{
  return (p == NULL && t_p == NULL);
}

KINLINE int sTObject::GetpLength()
{
  if (pLength <= 0) pLength = ::pLength(p != NULL ? p : t_p);
  return pLength;
}

KINLINE void sTObject::SetLmCurrRing()
{
  if (p == NULL && t_p != NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);
}

KINLINE poly sTObject::Next()
{
  assume(p != NULL || t_p != NULL);
  if (t_p != NULL) return pNext(t_p);
  return pNext(p);
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
  is_normalized = FALSE;
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

KINLINE void sLObject::Normalize()
{
  if (t_p != NULL)
  {
    pNormalize(t_p);
    if (p != NULL) pSetCoeff0(p, pGetCoeff(t_p));
  }
  else
  {
    pNormalize(p);
  }
}

KINLINE void sLObject::HeadNormalize()
{
  if (t_p != NULL)
  {
    nNormalize(pGetCoeff(t_p));
    if (p != NULL) pSetCoeff0(p, pGetCoeff(t_p));
  }
  else
  {
    nNormalize(pGetCoeff(p));
  }
}

KINLINE void
sTObject::ShallowCopyDelete(ring new_tailRing, omBin new_tailBin,
                            pShallowCopyDeleteProc p_shallow_copy_delete,
                            BOOLEAN set_max)
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
      max = p_shallow_copy_delete(max,tailRing,new_tailRing,new_tailBin);
  }
  else if (set_max && new_tailRing != currRing && pNext(t_p) != NULL)
  {
    max = p_GetMaxExpP(pNext(t_p), new_tailRing);
  }
  tailRing = new_tailRing;
}

KINLINE long sTObject::pFDeg() const
{
  if (p != NULL) return ::pFDeg(p, currRing);
  return tailRing->pFDeg(t_p, tailRing);
}
KINLINE long sTObject::pTotalDeg() const
{
  if (p != NULL) return p_Totaldegree(p, currRing);
  return p_Totaldegree(t_p,tailRing);
}
KINLINE long sTObject::SetpFDeg()
{
  FDeg = this->pFDeg();
  return FDeg;
}
KINLINE long sTObject::GetpFDeg() const
{
  assume(FDeg == this->pFDeg());
  return FDeg;
}
KINLINE long sTObject::pLDeg()
{
  return tailRing->pLDeg(GetLmTailRing(), &length, tailRing);
}
KINLINE long sTObject::SetDegStuffReturnLDeg()
{
  FDeg = this->pFDeg();
  long d = this->pLDeg();
  ecart = d - FDeg;
  return d;
}

//extern void pCleardenom(poly p);
extern void pNorm(poly p);
// manipulations
KINLINE void  sTObject::pCleardenom()
{
  assume(p != NULL);
  if (t_p != NULL)
  {
    p_Cleardenom(t_p, tailRing);
    pSetCoeff0(p, pGetCoeff(t_p));
  }
  else
  {
    #ifdef HAVE_RATGRING
    p= p_Cleardenom(p, currRing);
    #else
    p_Cleardenom(p, currRing);
    #endif
  }
}

KINLINE void  sTObject::pNorm()
{
  assume(p != NULL);
  if (! is_normalized)
  {
    ::pNorm(p);
    if (t_p != NULL)
      pSetCoeff0(t_p, pGetCoeff(p));
    is_normalized = TRUE;
  }
}



/***************************************************************
 *
 * Operation on LObjects
 *
 ***************************************************************/
// Initialization
KINLINE void sLObject::Clear()
{
  sTObject::Clear();
  sev = 0;
}
// Initialization
KINLINE void sLObject::Delete()
{
  sTObject::Delete();
  if (bucket != NULL)
    kBucketDeleteAndDestroy(&bucket);
}

KINLINE void sLObject::Init(ring r)
{
  memset(this, 0, sizeof(sLObject));
  i_r1 = -1;
  i_r2 = -1;
  i_r = -1;
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

KINLINE void sLObject::PrepareRed(BOOLEAN use_bucket)
{
  if (bucket == NULL)
  {
    int l = GetpLength();
    if (use_bucket && l > 1)
    {
      poly tp = GetLmTailRing();
      assume(l == ::pLength(tp));
      bucket = kBucketCreate(tailRing);
      kBucketInit(bucket, pNext(tp), l-1);
      pNext(tp) = NULL;
      if (p != NULL) pNext(p) = NULL;
      pLength = 0;
      last = NULL;
    }
  }
}

KINLINE void sLObject::SetLmTail(poly lm, poly p_tail, int p_Length, int use_bucket, ring _tailRing, poly _last)
{

  Set(lm, _tailRing);
  if (use_bucket)
  {
    bucket = kBucketCreate(_tailRing);
    kBucketInit(bucket, p_tail, p_Length);
    pNext(lm) = NULL;
    pLength = 0;
    last = NULL;
  }
  else
  {
    pNext(lm) = p_tail;
    pLength = p_Length + 1;
    last = _last;
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
    int shorter;
    pNext(_p) = tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(_p), m, q,
                                                      shorter,spNoether,
                                                      tailRing, last);
    pLength += lq - shorter;
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
      p = t_p = NULL;
      return;
    }
    Set(_p, tailRing);
  }
  else
  {
    pLength--;
  }
}

KINLINE poly sLObject::LmExtractAndIter()
{
  poly ret = GetLmTailRing();
  poly pn;

  assume(p != NULL || t_p != NULL);

  if (bucket != NULL)
  {
    pn = kBucketExtractLm(bucket);
    if (pn == NULL)
      kBucketDestroy(&bucket);
  }
  else
  {
    pn = pNext(ret);
  }
  pLength--;
  pNext(ret) = NULL;
  if (p != NULL && t_p != NULL)
    p_LmFree(p, currRing);

  Set(pn, tailRing);
  return ret;
}

KINLINE poly sLObject::CanonicalizeP()
{
  //kTest_L(this);
  int i = -1;

  if (bucket != NULL)
    i = kBucketCanonicalize(bucket);

  if (p == NULL)
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing);

  if (i >= 0) pNext(p) = bucket->buckets[i];
  return p;
}

KINLINE poly sLObject::GetTP()
{
  //kTest_L(this);
  poly tp = GetLmTailRing();
  assume(tp != NULL);

  if (bucket != NULL)
  {
    kBucketClear(bucket, &pNext(tp), &pLength);
    kBucketDestroy(&bucket);
    pLength++;
  }
  return tp;
}


KINLINE poly sLObject::GetP(omBin lmBin)
{
  //kTest_L(this);
  if (p == NULL)
  {
    p = k_LmInit_tailRing_2_currRing(t_p, tailRing,
                                     (lmBin!=NULL?lmBin:currRing->PolyBin));
    FDeg = pFDeg();
  }
  else if (lmBin != NULL && lmBin != currRing->PolyBin)
  {
    p = p_LmShallowCopyDelete(p, currRing, lmBin);
    FDeg = pFDeg();
  }

  if (bucket != NULL)
  {
    kBucketClear(bucket, &pNext(p), &pLength);
    kBucketDestroy(&bucket);
    pLength++;
    if (t_p != NULL) pNext(t_p) = pNext(p);
  }
  //kTest_L(this);
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
                              new_tailRing->PolyBin,p_shallow_copy_delete,
                              FALSE);
  last = NULL;
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

KINLINE void sLObject::Copy()
{
  if (bucket != NULL)
  {
    int i = kBucketCanonicalize(bucket);
    kBucket_pt new_bucket = kBucketCreate(tailRing);
    kBucketInit(new_bucket,
                p_Copy(bucket->buckets[i], tailRing),
                bucket->buckets_length[i]);
    bucket = new_bucket;
    if (t_p != NULL) pNext(t_p) = NULL;
    if (p != NULL) pNext(p) = NULL;
  }
  TObject::Copy();
  last = NULL;
}

KINLINE poly sLObject::CopyGetP()
{
  last = NULL;
  if (bucket != NULL)
  {
    int i = kBucketCanonicalize(bucket);
    poly bp = p_Copy(bucket->buckets[i], tailRing);
    pLength = bucket->buckets_length[i] + 1;
    if (bp != NULL)
    {
      assume(t_p != NULL || p != NULL);
      if (t_p != NULL) pNext(t_p) = bp;
      else pNext(p) = bp;
    }
    bucket = NULL;
  }
  return sLObject::GetP();
}


KINLINE long sLObject::pLDeg()
{
  poly tp = GetLmTailRing();
  assume(tp != NULL);
  if (bucket != NULL)
  {
    int i = kBucketCanonicalize(bucket);
    pNext(tp) = bucket->buckets[i];
    long ldeg = tailRing->pLDeg(tp, &length, tailRing);
    pNext(tp) = NULL;
    return ldeg;
  }
  else
    return tailRing->pLDeg(tp, &length, tailRing);
}
KINLINE long sLObject::pLDeg(BOOLEAN deg_last)
{
  if (! deg_last || bucket != NULL) return sLObject::pLDeg();

  if (last == NULL || pLength == 0)
    last = pLast(GetLmTailRing(), pLength);
#ifdef HAVE_ASSUME
  long fdeg;
  fdeg = tailRing->pLDeg(GetLmTailRing(), &length, tailRing);
  assume ( pLength == length || rIsSyzIndexRing(currRing));
  assume (fdeg == tailRing->pFDeg(last, tailRing));
  return fdeg;
#else
  length = pLength;
  return tailRing->pFDeg(last, tailRing);
#endif
}

KINLINE long sLObject::SetDegStuffReturnLDeg()
{
  FDeg = this->pFDeg();
  long d = this->pLDeg();
  ecart = d - FDeg;
  return d;
}
KINLINE long sLObject::SetDegStuffReturnLDeg(BOOLEAN use_last)
{
  FDeg = this->pFDeg();
  long d = this->pLDeg(use_last);
  ecart = d - FDeg;
  return d;
}
KINLINE int sLObject::GetpLength()
{
  if (bucket == NULL)
    return sTObject::GetpLength();
  int i = kBucketCanonicalize(bucket);
  return bucket->buckets_length[i] + 1;
}
KINLINE int sLObject::SetLength(BOOLEAN length_pLength)
{
  if (length_pLength)
  {
    length = this->GetpLength();
  }
  else
    this->pLDeg();
  return length;
}
KINLINE long sLObject::MinComp()
{
  poly tp = GetLmTailRing();
  assume(tp != NULL);
  if (bucket != NULL)
  {
    int i = kBucketCanonicalize(bucket);
    pNext(tp) = bucket->buckets[i];
    long m = p_MinComp(tp, tailRing);
    pNext(tp) = NULL;
    return m;
  }
  else
    return p_MinComp(tp, tailRing);
}
KINLINE long sLObject::Comp()
{
  poly pp;
  ring r;
  GetLm(pp, r);
  assume(pp != NULL);
  return p_GetComp(pp, r);
}

KINLINE sLObject& sLObject::operator=(const sTObject& t)
{
  memset(this, 0, sizeof(*this));
  memcpy(this, &t, sizeof(sTObject));
  return *this;
}

KINLINE TObject* sLObject::T_1(const skStrategy* s)
{
  if (p1 == NULL) return NULL;
  if (i_r1 == -1) i_r1 = kFindInT(p1, s->T, s->tl);
  assume(i_r1 >= 0 && i_r1 <= s->tl);
  TObject* T = s->R[i_r1];
  assume(T->p == p1);
  return T;
}

KINLINE TObject* sLObject::T_2(const skStrategy* strat)
{
  if (p1 == NULL) return NULL;
  assume(p2 != NULL);
  if (i_r2 == -1) i_r2 = kFindInT(p2, strat->T, strat->tl);
  assume(i_r2 >= 0 && i_r2 <= strat->tl);
  TObject* T = strat->R[i_r2];
  assume(T->p == p2);
  return T;
}

KINLINE void    sLObject::T_1_2(const skStrategy* strat,
                                TObject* &T_1, TObject* &T_2)
{
  if (p1 == NULL)
  {
    T_1 = NULL;
    T_2 = NULL;
    return;
  }
  assume(p1 != NULL && p2 != NULL);
  if (i_r1 == -1) i_r1 = kFindInT(p1, strat->T, strat->tl);
  if (i_r2 == -1) i_r2 = kFindInT(p2, strat->T, strat->tl);
  assume(i_r1 >= 0 && i_r1 <= strat->tl);
  assume(i_r2 >= 0 && i_r2 <= strat->tl);
  T_1 = strat->R[i_r1];
  T_2 = strat->R[i_r2];
  assume(T_1->p == p1);
  assume(T_2->p == p2);
  return;
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
  long x;
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

#ifdef HAVE_RINGS
// get m1 = LCM(LM(p1), LM(p2))/LM(p1)
//     m2 = LCM(LM(p1), LM(p2))/LM(p2)   in tailRing
//    lcm = LCM(LM(p1), LM(p2)           in leadRing
KINLINE void k_GetStrongLeadTerms(const poly p1, const poly p2, const ring leadRing,
                               poly &m1, poly &m2, poly &lcm, const ring tailRing)
{
  p_LmCheckPolyRing(p1, leadRing);
  p_LmCheckPolyRing(p2, leadRing);

  int i;
  int x;
  int e1;
  int e2;
  int s;
  m1 = p_Init(tailRing);
  m2 = p_Init(tailRing);
  lcm = p_Init(leadRing);

  for (i = leadRing->N; i>=1; i--)
  {
    e1 = p_GetExp(p1,i,leadRing);
    e2 = p_GetExp(p2,i,leadRing);
    x = e1 - e2;
    if (x > 0)
    {
      p_SetExp(m2,i,x, tailRing);
      //p_SetExp(m1,i,0, tailRing); // done by p_Init
      s = e1;
    }
    else
    {
      p_SetExp(m1,i,-x, tailRing);
      //p_SetExp(m2,i,0, tailRing); // done by p_Init
      s = e2;
    }
    p_SetExp(lcm,i,s, leadRing);
  }
  if ((s=pGetComp(p1))!=0)
  {
    p_SetComp(lcm,s, leadRing);
  } 
  else if ((s=pGetComp(p2))!=0)
  {
    p_SetComp(lcm,s, leadRing);
  } 
  // else p_SetComp(lcm,0,tailRing); // done by p_Init

  p_Setm(m1, tailRing);
  p_Setm(m2, tailRing);
  p_Setm(lcm, leadRing);
}
#endif

/***************************************************************
 *
 * Misc things
 *
 ***************************************************************/
KINLINE int ksReducePolyTail(LObject* PR, TObject* PW, LObject* Red)
{
  BOOLEAN ret;
  number coef;

  assume(PR->GetLmCurrRing() != PW->GetLmCurrRing());
  Red->HeadNormalize();
  ret = ksReducePoly(Red, PW, NULL, &coef);

  if (!ret)
  {
    if (! n_IsOne(coef, currRing))
    {
      PR->Mult_nn(coef);
      // HANNES: mark for Normalize
    }
    n_Delete(&coef, currRing);
  }
  return ret;
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

KINLINE poly redtailBba (poly p,int pos,kStrategy strat,BOOLEAN normalize)
{
  LObject L(p, currRing, strat->tailRing);
  return redtailBba(&L, pos, strat,FALSE, normalize);
}

#ifdef HAVE_RINGS
KINLINE poly redtailBba_Z (poly p,int pos,kStrategy strat)
{
  LObject L(p, currRing, strat->tailRing);
  return redtailBba_Z(&L, pos, strat);
}
#endif

KINLINE poly redtailBba(TObject *T, int pos,kStrategy strat)
{
  LObject L;
  L = *T;
  poly p = redtailBba(&L, pos, strat, FALSE);
  *T = L;
  //kTest_T(T);
  assume( p == T->p);
  return p;
}

KINLINE void clearS (poly p, unsigned long p_sev, int* at, int* k,
                    kStrategy strat)
{
  assume(p_sev == pGetShortExpVector(p));
  if (strat->noClearS) return;
  if (!pLmShortDivisibleBy(p,p_sev, strat->S[*at], ~ strat->sevS[*at])) return;
  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
}

#endif // defined(KINLINE) || defined(KUTIL_CC)
#endif // KINLINE_CC
