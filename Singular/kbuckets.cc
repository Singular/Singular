/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kbuckets.cc,v 1.25 2003-03-03 15:24:28 Singular Exp $ */

#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "omalloc.h"
#include "p_polys.h"
#include "febase.h"
#include "pShallowCopyDelete.h"
#include "kbuckets.h"
#include "numbers.h"
#include "p_Procs.h"

#ifdef HAVE_COEF_BUCKETS
#define USE_COEF_BUCKETS
#endif

#ifdef USE_COEF_BUCKETS
#define MULTIPLY_BUCKET(B,I) do                                        \
  { if (B->coef[I]!=NULL)                                              \
    {                                                                  \
      B->buckets[I]=p_Mult_q(B->buckets[I],B->coef[I],B->bucket_ring); \
      B->coef[I]=NULL;                                                 \
    }                                                                  \
  } while(0)
#else
#define MULTIPLY_BUCKET(B,I)
#endif
static omBin kBucket_bin = omGetSpecBin(sizeof(kBucket));

//////////////////////////////////////////////////////////////////////////
///
/// Some internal stuff
///

// returns ceil(log_4(l))
inline unsigned int pLogLength(unsigned int l)
{
  unsigned int i = 0;

  if (l == 0) return 0;
  l--;
#ifdef BUCKET_TWO_BASE
  while ((l = (l >> 1))) i++;
#else
  while ((l = (l >> 2))) i++;
#endif
  return i+1;
}

// returns ceil(log_4(pLength(p)))
inline unsigned int pLogLength(poly p)
{
  return pLogLength((unsigned int) pLength(p));
}

#ifdef KDEBUG

#ifndef HAVE_PSEUDO_BUCKETS
BOOLEAN kbTest_i(kBucket_pt bucket, int i)
{
  #ifdef USE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  if ((bucket->coef[i]!=NULL) && (bucket->buckets[i]==NULL))
  {
    dReportError("Bucket %d coef not NULL", i);
  }
  if (bucket->coef[i]!=NULL)
    _p_Test(bucket->coef[i],bucket->bucket_ring,PDEBUG);
  #endif
  pFalseReturn(p_Test(bucket->buckets[i], bucket->bucket_ring));
  if (bucket->buckets_length[i] != pLength(bucket->buckets[i]))
  {
    dReportError("Bucket %d lengths difference should:%d has:%d",
                 i, bucket->buckets_length[i], pLength(bucket->buckets[i]));
  }
  else if (i > 0 && (int) pLogLength(bucket->buckets_length[i]) > i)
  {
    dReportError("Bucket %d too long %d",
                 i, bucket->buckets_length[i]);
  }
  if (i==0 && bucket->buckets_length[0] > 1)
  {
    dReportError("Bucket 0 too long");
  }
  return TRUE;
}


BOOLEAN kbTest(kBucket_pt bucket)
{
  int i;
  poly lm = bucket->buckets[0];

  omCheckAddrBin(bucket, kBucket_bin);
  if (! kbTest_i(bucket, 0)) return FALSE;
  for (i=1; i<= (int) bucket->buckets_used; i++)
  {
    if (!kbTest_i(bucket, i)) return FALSE;
    if (lm != NULL &&  bucket->buckets[i] != NULL
        && p_LmCmp(lm, bucket->buckets[i], bucket->bucket_ring) != 1)
    {
      dReportError("Bucket %d larger than lm", i);
      return FALSE;
    }
    if (!p_Test(bucket->buckets[i],bucket->bucket_ring))
    {
      dReportError("Bucket %d is not =0(4)", i);
      return FALSE;
    }
  }

  for (; i<=MAX_BUCKET; i++)
  {
    if (bucket->buckets[i] != NULL || bucket->buckets_length[i] != 0)
    {
      dReportError("Bucket %d not zero", i);
      return FALSE;
    }
  }
  return TRUE;
}

#else // HAVE_PSEUDO_BUCKETS
BOOLEAN kbTest(kBucket_pt bucket)
{
  return TRUE;
}
#endif // ! HAVE_PSEUDO_BUCKETS
#endif // KDEBUG

//////////////////////////////////////////////////////////////////////////
///
/// Creation/Destruction of buckets
///

kBucket_pt kBucketCreate(ring bucket_ring)
{
  assume(bucket_ring != NULL);
  kBucket_pt bucket = (kBucket_pt) omAlloc0Bin(kBucket_bin);
  bucket->bucket_ring = bucket_ring;
  return bucket;
}
void kBucketDestroy(kBucket_pt *bucket_pt)
{
  omFreeBin(*bucket_pt, kBucket_bin);
  *bucket_pt = NULL;
}


void kBucketDeleteAndDestroy(kBucket_pt *bucket_pt)
{
  kBucket_pt bucket = *bucket_pt;
  kbTest(bucket);
  int i;
  for (i=0; i<= bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
      p_Delete(&(bucket->buckets[i]), bucket->bucket_ring);
#ifdef USE_COEF_BUCKETS
      if (bucket->coef[i]!=NULL)
        p_Delete(&(bucket->coef[i]), bucket->bucket_ring);
#endif
    }
  }
  omFreeBin(bucket, kBucket_bin);
  *bucket_pt = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Convertion from/to Bpolys
//
#ifndef HAVE_PSEUDO_BUCKETS

inline void kBucketMergeLm(kBucket_pt bucket)
{
  if (bucket->buckets[0] != NULL)
  {
    poly lm = bucket->buckets[0];
    int i = 1;
#ifdef BUCKET_TWO_BASE
    int l = 2;
    while ( bucket->buckets_length[i] >= l)
    {
      i++;
      l = l << 1;
    }
#else
    int l = 4;
    while ( bucket->buckets_length[i] >= l)
    {
      i++;
      l = l << 2;
    }
#endif
    MULTIPLY_BUCKET(bucket,i);
    pNext(lm) = bucket->buckets[i];
    bucket->buckets[i] = lm;
    bucket->buckets_length[i]++;
    assume(i <= bucket->buckets_used+1);
    if (i > bucket->buckets_used)  bucket->buckets_used = i;
    bucket->buckets[0] = NULL;
    bucket->buckets_length[0] = 0;
  }
}

static BOOLEAN kBucketIsCleared(kBucket_pt bucket)
{
  int i;

  for (i = 0;i<=MAX_BUCKET;i++)
  {
    if (bucket->buckets[i] != NULL) return FALSE;
    if (bucket->buckets_length[i] != 0) return FALSE;
  }
  return TRUE;
}

void kBucketInit(kBucket_pt bucket, poly lm, int length)
{
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(lm));
  assume(kBucketIsCleared(bucket));

  if (lm == NULL) return;

  if (length <= 0)
    length = pLength(lm);

  bucket->buckets[0] = lm;
  bucket->buckets_length[0] = 1;
  if (length > 1)
  {
    unsigned int i = pLogLength(length-1);
    bucket->buckets[i] = pNext(lm);
    pNext(lm) = NULL;
    bucket->buckets_length[i] = length-1;
    bucket->buckets_used = i;
  }
  else
  {
    bucket->buckets_used = 0;
  }
}

int kBucketCanonicalize(kBucket_pt bucket)
{
  kbTest(bucket);
  poly p = bucket->buckets[1];
  poly lm;
  int pl = bucket->buckets_length[1], i;
  bucket->buckets[1] = NULL;
  bucket->buckets_length[1] = 0;
  ring r=bucket->bucket_ring;


  for (i=2; i<=bucket->buckets_used; i++)
  {
  #ifdef USE_COEF_BUCKETS
    if (bucket->coef[i]!=NULL)
    {
      p = p_Plus_mm_Mult_qq(p, bucket->coef[i], bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
      p_Delete(&bucket->coef[i],r);
      p_Delete(&bucket->buckets[i],r);
    }
    else
    p = p_Add_q(p, bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
  #else
    q = p_Add_q(q, bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
  #endif
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
  }

  lm = bucket->buckets[0];
  if (lm != NULL)
  {
    pNext(lm) = p;
    p = lm;
    pl++;
    bucket->buckets[0] = NULL;
    bucket->buckets_length[0] = 0;
  }
  if (pl > 0)
  {
    i = pLogLength(pl);
    bucket->buckets[i] = p;
    bucket->buckets_length[i] = pl;
  }
  else
  {
    i = 0;
  }
  bucket->buckets_used = i;
  assume(pLength(p) == (int) pl);
  kbTest(bucket);
  return i;
}

void kBucketClear(kBucket_pt bucket, poly *p, int *length)
{
  int i = kBucketCanonicalize(bucket);
  if (i > 0)
  {
    *p = bucket->buckets[i];
    *length = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    bucket->buckets_used = 0;
#ifdef USE_COEF_BUCKETS
    bucket->coef[i]=NULL;
#endif
  }
  else
  {
    *p = NULL;
    *length = 0;
  }
}

void kBucketSetLm(kBucket_pt bucket, poly lm)
{
  kBucketMergeLm(bucket);
  pNext(lm) = NULL;
  bucket->buckets[0] = lm;
  bucket->buckets_length[0] = 1;
}

#else // HAVE_PSEUDO_BUCKETS

void kBucketInit(kBucket_pt bucket, poly lm, int length)
{
  int i;

  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(lm));

  bucket->p = lm;
  if (length <= 0) bucket->l = pLength(lm);
  else bucket->l = length;

}

const poly kBucketGetLm(kBucket_pt bucket)
{
  return bucket->p;
}

poly kBucketExtractLm(kBucket_pt bucket)
{
  poly lm = bucket->p;
  assume(pLength(bucket->p) == bucket->l);
  pIter(bucket->p);
  (bucket->l)--;
  pNext(lm) = NULL;
  return lm;
}

void kBucketClear(kBucket_pt bucket, poly *p, int *length)
{
  assume(pLength(bucket->p) == bucket->l);
  *p = bucket->p;
  *length = bucket->l;
  bucket->p = NULL;
  bucket->l = 0;
}

#endif // ! HAVE_PSEUDO_BUCKETS
//////////////////////////////////////////////////////////////////////////
///
/// For changing the ring of the Bpoly to new_tailBin
///
void kBucketShallowCopyDelete(kBucket_pt bucket,
                              ring new_tailRing, omBin new_tailBin,
                              pShallowCopyDeleteProc p_shallow_copy_delete)
{
#ifndef HAVE_PSEUDO_BUCKETS
  int i;

  kBucketCanonicalize(bucket);
  for (i=0; i<= bucket->buckets_used; i++)
    if (bucket->buckets[i] != NULL)
    {
      MULTIPLY_BUCKET(bucket,i);
      bucket->buckets[i] = p_shallow_copy_delete(bucket->buckets[i],
                                                 bucket->bucket_ring,
                                                 new_tailRing,
                                                 new_tailBin);
    }
#else
  bucket->p = p_shallow_copy_delete(p,
                                    bucket_ring,
                                    new_tailRing,
                                    new_tailBin);
#endif
  bucket->bucket_ring = new_tailRing;
}



//////////////////////////////////////////////////////////////////////////
///
/// Multiply Bucket by number ,i.e. Bpoly == n*Bpoly
///
void kBucket_Mult_n(kBucket_pt bucket, number n)
{
#ifndef HAVE_PSEUDO_BUCKETS
  kbTest(bucket);
  ring r=bucket->bucket_ring;
  int i;

  for (i=0; i<= bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
#ifdef USE_COEF_BUCKETS
      if (i<2)
        bucket->buckets[i] = p_Mult_nn(bucket->buckets[i], n, r);
      else
      if (bucket->coef[i]!=NULL)
      {
        bucket->coef[i] = p_Mult_nn(bucket->coef[i],n,r);
      }
      else
      {
        bucket->coef[i] = p_NSet(n_Copy(n,bucket->bucket_ring),r);
      }
#else
      bucket->buckets[i] = p_Mult_nn(bucket->buckets[i], n, r);
#endif
    }
  }
  kbTest(bucket);
#else
  bucket->p = p_Mult_nn(bucket->p, n, bucket->bucket_ring);
#endif
}


//////////////////////////////////////////////////////////////////////////
///
/// Add to Bucket a poly ,i.e. Bpoly == n*Bpoly
///
void kBucket_Add_q(kBucket_pt bucket, poly q, int *l)
{
  if (q == NULL) return;
  assume(*l <= 0 || pLength(q) == *l);

  int i, l1;
  ring r = bucket->bucket_ring;

  if (*l <= 0)
  {
    l1 = pLength(q);
    *l = l1;
  }
  else
    l1 = *l;

  kBucketMergeLm(bucket);
  kbTest(bucket);
  i = pLogLength(l1);

  while (bucket->buckets[i] != NULL)
  {
    //MULTIPLY_BUCKET(bucket,i);
  #ifdef USE_COEF_BUCKETS
    if (bucket->coef[i]!=NULL)
    {
      q = p_Plus_mm_Mult_qq(q, bucket->coef[i], bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
      p_Delete(&bucket->coef[i],r);
      p_Delete(&bucket->buckets[i],r);
    }
    else
    q = p_Add_q(q, bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
  #else
    q = p_Add_q(q, bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
  #endif
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
  }

  bucket->buckets[i] = q;
  bucket->buckets_length[i]=l1;
  if (i >= bucket->buckets_used)
    bucket->buckets_used = i;
  else
    kBucketAdjustBucketsUsed(bucket);
  kbTest(bucket);
}



//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly - m*p; where m is a monom
/// Does not destroy p and m
/// assume (*l <= 0 || pLength(p) == *l)
void kBucket_Minus_m_Mult_p(kBucket_pt bucket, poly m, poly p, int *l,
                            poly spNoether)
{
  assume(*l <= 0 || pLength(p) == *l);
  int i, l1;
  poly p1 = p;
  poly last;
  ring r = bucket->bucket_ring;

  if (*l <= 0)
  {
    l1 = pLength(p1);
    *l = l1;
  }
  else
    l1 = *l;

  if (m == NULL || p == NULL) return;

#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  kbTest(bucket);
  i = pLogLength(l1);

  if ((i <= bucket->buckets_used) && (bucket->buckets[i] != NULL))
  {
    assume(pLength(bucket->buckets[i])==bucket->buckets_length[i]);
//#ifdef USE_COEF_BUCKETS
//     if(bucket->coef[i]!=NULL)
//     {
//       poly mult=p_Mult_mm(bucket->coef[i],m,r);
//       bucket->coef[i]=NULL;
//       p1 = p_Minus_mm_Mult_qq(bucket->buckets[i], mult, p1,
//                               bucket->buckets_length[i], l1,
//                             spNoether, r);
//     }
//     else
//#endif
    MULTIPLY_BUCKET(bucket,i);
    p1 = p_Minus_mm_Mult_qq(bucket->buckets[i], m, p1,
                            bucket->buckets_length[i], l1,
                            spNoether, r);
    l1 = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
  }
  else
  {
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
    if (spNoether != NULL)
    {
      l1 = -1;
      p1 = r->p_Procs->pp_Mult_mm_Noether(p1, m, spNoether, l1, r, last);
      i = pLogLength(l1);
    }
    else
      p1 = r->p_Procs->pp_Mult_mm(p1, m, r, last);
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
  }

  while (bucket->buckets[i] != NULL)
  {
    //kbTest(bucket);
    MULTIPLY_BUCKET(bucket,i);
    p1 = p_Add_q(p1, bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
  }

  bucket->buckets[i] = p1;
  bucket->buckets_length[i]=l1;
  if (i >= bucket->buckets_used)
    bucket->buckets_used = i;
  else
    kBucketAdjustBucketsUsed(bucket);
#else // HAVE_PSEUDO_BUCKETS
  bucket->p = p_Minus_mm_Mult_qq(bucket->p, m,  p,
                               bucket->l, l1,
                               spNoether, r);
#endif
}

//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly + m*p; where m is a monom
/// Does not destroy p and m
/// assume (l <= 0 || pLength(p) == l)
void kBucket_Plus_mm_Mult_pp(kBucket_pt bucket, poly m, poly p, int l)
{
  assume(l <= 0 || pLength(p) == l);
  int i, l1;
  poly p1 = p;
  poly last;
  ring r = bucket->bucket_ring;

  if (l <= 0)
  {
    l1 = pLength(p1);
    l = l1;
  }
  else
    l1 = l;

  if (m == NULL || p == NULL) return;

  kBucketMergeLm(bucket);
  kbTest(bucket);
  i = pLogLength(l1);

  if (i <= bucket->buckets_used && bucket->buckets[i] != NULL)
  {
    MULTIPLY_BUCKET(bucket,i);
    p1 = p_Plus_mm_Mult_qq(bucket->buckets[i], m, p1,
                           bucket->buckets_length[i], l1, r);
    l1 = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
  }
  else
  {
    p1 = r->p_Procs->pp_Mult_mm(p1, m, r, last);
  }

  while (bucket->buckets[i] != NULL)
  {
    MULTIPLY_BUCKET(bucket,i);
    p1 = p_Add_q(p1, bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
  }

  bucket->buckets[i] = p1;
  bucket->buckets_length[i]=l1;
  if (i >= bucket->buckets_used)
    bucket->buckets_used = i;
  else
    kBucketAdjustBucketsUsed(bucket);

  kbTest(bucket);
}

poly kBucket_ExtractLarger(kBucket_pt bucket, poly q, poly append)
{
  if (q == NULL) return append;
  poly lm;
  loop
  {
    lm = kBucketGetLm(bucket);
    if (lm == NULL) return append;
    if (p_LmCmp(lm, q, bucket->bucket_ring) == 1)
    {
      lm = kBucketExtractLm(bucket);
      pNext(append) = lm;
      pIter(append);
    }
    else
    {
      return append;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Extract all monomials from bucket with component comp
// Return as a polynomial *p with length *l
// In other words, afterwards
// Bpoly = Bpoly - (poly consisting of all monomials with component comp)
// and components of monomials of *p are all 0
//

// Hmm... for now I'm too lazy to implement those independent of currRing
// But better declare it extern than including polys.h
extern void pTakeOutComp(poly *p, Exponent_t comp, poly *q, int *lq);
void pDecrOrdTakeOutComp(poly *p, Exponent_t comp, Order_t order,
                         poly *q, int *lq);

void kBucketTakeOutComp(kBucket_pt bucket,
                        Exponent_t comp,
                        poly *r_p, int *l)
{
  poly p = NULL, q;
  int i, lp = 0, lq;

#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  for (i=1; i<=bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
      MULTIPLY_BUCKET(bucket,i);
      pTakeOutComp(&(bucket->buckets[i]), comp, &q, &lq);
      if (q != NULL)
      {
        assume(pLength(q) == lq);
        bucket->buckets_length[i] -= lq;
        assume(pLength(bucket->buckets[i]) == bucket->buckets_length[i]);
        p = p_Add_q(p, q, lp, lq, bucket->bucket_ring);
      }
    }
  }
  kBucketAdjustBucketsUsed(bucket);
#else
  pTakeOutComp(&(bucket->p), comp, &p, &lp);
  (bucket->l) -= lp;
#endif
  *r_p = p;
  *l = lp;

  kbTest(bucket);
}

void kBucketDecrOrdTakeOutComp(kBucket_pt bucket,
                               Exponent_t comp, Order_t order,
                               poly *r_p, int *l)
{
  poly p = NULL, q;
  int i, lp = 0, lq;

#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  for (i=1; i<=bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
      MULTIPLY_BUCKET(bucket,i);
      pDecrOrdTakeOutComp(&(bucket->buckets[i]), comp, order, &q, &lq);
      if (q != NULL)
      {
        bucket->buckets_length[i] -= lq;
        p = p_Add_q(p, q, lp, lq, bucket->bucket_ring);
      }
    }
  }
  kBucketAdjustBucketsUsed(bucket);
#else
  pDecrOrdTakeOutComp(&(bucket->p), comp, order, &p, &lp);
  (bucket->l) -= lp;
#endif

  *r_p = p;
  *l = lp;
}

/////////////////////////////////////////////////////////////////////////////
// Reduction of Bpoly with a given poly
//

extern int ksCheckCoeff(number *a, number *b);

number kBucketPolyRed(kBucket_pt bucket,
                      poly p1, int l1,
                      poly spNoether)
{
  assume(p1 != NULL &&
         p_DivisibleBy(p1,  kBucketGetLm(bucket), bucket->bucket_ring));
  assume(pLength(p1) == (int) l1);

  poly a1 = pNext(p1), lm = kBucketExtractLm(bucket);
  BOOLEAN reset_vec=FALSE;
  number rn;

  if(a1==NULL)
  {
    p_DeleteLm(&lm, bucket->bucket_ring);
    return nInit(1);
  }

  if (! nIsOne(pGetCoeff(p1)))
  {
    number an = pGetCoeff(p1), bn = pGetCoeff(lm);
    int ct = ksCheckCoeff(&an, &bn);
    p_SetCoeff(lm, bn, bucket->bucket_ring);
    if ((ct == 0) || (ct == 2)) kBucket_Mult_n(bucket, an);
    rn = an;
  }
  else
  {
    rn = nInit(1);
  }

  if (p_GetComp(p1, bucket->bucket_ring) != p_GetComp(lm, bucket->bucket_ring))
  {
    p_SetCompP(a1, p_GetComp(lm, bucket->bucket_ring), bucket->bucket_ring);
    reset_vec = TRUE;
    p_SetComp(lm, p_GetComp(p1, bucket->bucket_ring), bucket->bucket_ring);
    p_Setm(lm, bucket->bucket_ring);
  }

  p_ExpVectorSub(lm,p1, bucket->bucket_ring);
  l1--;

  kBucket_Minus_m_Mult_p(bucket, lm, a1, &l1, spNoether);

  p_DeleteLm(&lm, bucket->bucket_ring);
  if (reset_vec) p_SetCompP(a1, 0, bucket->bucket_ring);
  kbTest(bucket);
  return rn;
}


