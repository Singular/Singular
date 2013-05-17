

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
    }
  }
}

KINLINE void sLObject::SetLmTail(poly lm, poly p_tail, int p_Length, int use_bucket, ring _tailRing)
{

  Set(lm, _tailRing);
  if (use_bucket)
  {
    bucket = kBucketCreate(_tailRing);
    kBucketInit(bucket, p_tail, p_Length);
    pNext(lm) = NULL;
    pLength = 0;
  }
  else
  {
    pNext(lm) = p_tail;
    pLength = p_Length + 1;
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
    pNext(_p) = p_Mult_nn(pNext(_p), n, tailRing);
  }
}

KINLINE void sLObject::Tail_Minus_mm_Mult_qq
  (poly m, poly q, int lq, poly spNoether)
{
  if (bucket != NULL)
  {
    kBucket_Minus_m_Mult_p(bucket, m, q, &lq, spNoether);
  }
  else
  {
    poly _p = (t_p != NULL ? t_p : p);
    assume(_p != NULL);

    int lp=pLength-1;
    //BOCO: Memory leak?: Where do we delete _p
    //(we only keep pNext(_p))
    //TODO: find that out
    pNext(_p) = p_Minus_mm_Mult_qq( pNext(_p), m, q, lp, lq,
                                    spNoether, tailRing );
    pLength=lp+1;
//    tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(_p), m, q, shorter,spNoether, tailRing, last);
//    pLength += lq - shorter;
  }
}

#ifdef HAVE_SHIFTBBADVEC
//adapted version of sLObject::Tail_Minus_mm_Mult_qq; merged
//with p_Minus_mm_Mult_qq from polys/pInline2.h
KINLINE void sLObject::Tail_Minus_mml_Mult_qq_Mult_mmr
  ( poly mml, poly q, poly mmr, int lq, poly spNoether )
{
  assume(bucket == NULL);

  poly _p = (t_p != NULL ? t_p : p);
  assume(_p != NULL);

  int lp=pLength-1;
  int shorter;
  //poly last;
  //BOCO: Memory leak?: Where do we delete _p 
  //(we only keep pNext(_p))
  //TODO: find that out
  pNext(_p) =
    tailRing->p_Procs->LPDV__p_Minus_mml_Mult_qq_Mult_mmr
      (pNext(_p), mml, q, mmr, shorter, spNoether, tailRing);
  pLength = (lp + lq) - shorter + 1;
}
#endif

#if 0
#ifdef HAVE_SHIFTBBADVEC
KINLINE void sLObject::Tail_Minus_mm_Mult_qq
  ( poly m, poly q, int lq, poly spNoether, int lV )
{
  if (bucket != NULL)
  {
    //BOCO: WARNING/TODO
    //We do not yet consider buckets, but we want to do it some
    //time. Up to then we have to shut buckets down or they will
    //make Singular explode !!!
    kBucket_Minus_m_Mult_p(bucket, m, q, &lq, spNoether);
  }
  else
  {
    poly _p = (t_p != NULL ? t_p : p);
    assume(_p != NULL);
    int shorter;
#if 0 //BOCO: replaced
    pNext(_p) = tailRing->p_Procs->p_Minus_mm_Mult_qq(pNext(_p), m, q,
                                                      shorter,spNoether,
                                                      tailRing, last);
#else //BOCO: replacement
    pNext(_p) = ShiftDVec::p_Minus_mm_Mult_qq__T
      ( pNext(_p), m, q, shorter,spNoether, tailRing, last, lV);
#endif
    pLength += lq - shorter;
  }
}
#endif
#endif

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
    p = p_LmShallowCopyDelete(p, currRing);
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
}

KINLINE poly sLObject::CopyGetP()
{
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

  long ldeg;
  ldeg = tailRing->pLDeg(GetLmTailRing(), &length, tailRing);
#ifdef HAVE_ASSUME
  if ( pLength == 0)
    p_Last(GetLmTailRing(), pLength, tailRing);
  assume ( pLength == length || rIsSyzIndexRing(currRing));
#else
  pLength=length;
#endif
  return ldeg;
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

  //BOCO/TODO: 
  //In our case T->p might be a shift of p2; 
  //TODO: we should test, if T->p is such a shift
  //bool SDCase = dynamic_cast<ShiftDVec::sTObject*>(T);
  assume( T->p == p2 || dynamic_cast<ShiftDVec::sTObject*>(T) );

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

