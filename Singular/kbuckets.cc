/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kbuckets.cc,v 1.18 2000-09-25 12:26:31 obachman Exp $ */

#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "febase.h"
#include "kbuckets.h"
#include "numbers.h"
#include "p_Procs.h"

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
  while ((l = (l >> 2))) i++;
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
        && pLmCmp(lm, bucket->buckets[i]) != 1)
    {
      dReportError("Bucket %d larger than lm", i);
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

void kBucketDestroy(kBucket_pt *bucket)
{
  omFreeBin(*bucket, kBucket_bin);
  *bucket = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to Bpolys
//
#ifndef HAVE_PSEUDO_BUCKETS

inline void kBucketAdjustBucketsUsed(kBucket_pt bucket)
{
  while ( bucket->buckets_used > 0 &&
          bucket->buckets[bucket->buckets_used] == NULL)
    (bucket->buckets_used)--;
}

inline void kBucketMergeLm(kBucket_pt bucket)
{
  if (bucket->buckets[0] != NULL)
  {
    poly lm = bucket->buckets[0];
    int i = 1;
    int l = 4;
    while ( bucket->buckets_length[i] >= l)
    {
      i++;
      l = l << 2;
    }
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
  int i;
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(lm));
  assume(kBucketIsCleared(bucket));

  if (length <= 0)
    length = pLength(lm);

  bucket->buckets[0] = lm;
  if (length > 1)
  {
    unsigned int i = pLogLength(length-1);
    bucket->buckets[i] = pNext(lm);
    pNext(lm) = NULL;
    bucket->buckets_length[i] = length-1;
    bucket->buckets_used = i;
    bucket->buckets_length[0] = 1;
  }
  else
  {
    bucket->buckets_used = 0;
    bucket->buckets_length[0] = 0;
  }
}

static void kBucketSetLm(kBucket_pt bucket)
{
  int j = 0;
  poly lt;
  BOOLEAN zero = FALSE;
  assume(bucket->buckets[0] == NULL && bucket->buckets_length[0] == 0);

  do
  {
    j = 0;
    for (int i = 1; i<=bucket->buckets_used; i++)
    {
      if (bucket->buckets[i] != NULL)
      {
        int comp = (j == 0 ? 1 :
                    p_LmCmp(bucket->buckets[i], bucket->buckets[j], bucket->bucket_ring));

        if (comp > 0)
        {
          if (j > 0 &&
              bucket->buckets[j] != NULL &&
              nIsZero(pGetCoeff(bucket->buckets[j])))
          {
            p_DeleteLm(&(bucket->buckets[j]), bucket->bucket_ring);
            (bucket->buckets_length[j])--;
          }
          j = i;
        }
        else if (comp == 0)
        {
          number tn = pGetCoeff(bucket->buckets[j]);
          pSetCoeff0(bucket->buckets[j],
                     nAdd(pGetCoeff(bucket->buckets[i]), tn));
          nDelete(&tn);
          p_DeleteLm(&(bucket->buckets[i]), bucket->bucket_ring);
          (bucket->buckets_length[i])--;
        }
      }
    }
    if (j > 0 && nIsZero(pGetCoeff(bucket->buckets[j])))
    {
      p_DeleteLm(&(bucket->buckets[j]), bucket->bucket_ring);
      (bucket->buckets_length[j])--;
      j = -1;
    }
  }
  while (j < 0);

  if (j == 0)
  {
    return;
  }

  assume(bucket->buckets[j] != NULL);
  lt = bucket->buckets[j];
  bucket->buckets[j] = pNext(lt);
  bucket->buckets_length[j]--;
  pNext(lt) = NULL;
  bucket->buckets[0] = lt;
  bucket->buckets_length[0] = 1;

  kBucketAdjustBucketsUsed(bucket);
}

const poly kBucketGetLm(kBucket_pt bucket)
{
  if (bucket->buckets[0] == NULL)
    kBucketSetLm(bucket);
  return bucket->buckets[0];
}

poly kBucketExtractLm(kBucket_pt bucket)
{
  poly lm;
  if (bucket->buckets[0] == NULL) kBucketSetLm(bucket);
  lm = bucket->buckets[0];
  bucket->buckets[0] = NULL;
  bucket->buckets_length[0] = 0;
  return lm;
}

int kBucketCanonicalize(kBucket_pt bucket)
{
  poly p = bucket->buckets[1];
  poly lm;
  int pl = bucket->buckets_length[1], i;
  bucket->buckets[1] = NULL;
  bucket->buckets_length[1] = 0;


  for (i=2; i<=bucket->buckets_used; i++)
  {
    p = p_Add_q(p, bucket->buckets[i],
                 pl, bucket->buckets_length[i], bucket->bucket_ring);
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
  }
  else
  {
    *p = NULL;
    *length = 0;
  }
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
/// Multiply Bucket by number ,i.e. Bpoly == n*Bpoly
///
void kBucket_Mult_n(kBucket_pt bucket, number n)
{
#ifndef HAVE_PSEUDO_BUCKETS
  int i;

  for (i=0; i<= bucket->buckets_used; i++)
    if (bucket->buckets[i] != NULL)
      bucket->buckets[i] = p_Mult_nn(bucket->buckets[i], n, bucket->bucket_ring);
#else
  bucket->p = p_Mult_nn(bucket->p, n, bucket->bucket_ring);
#endif
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

  if (i <= bucket->buckets_used && bucket->buckets[i] != NULL)
  {
    p1 = p_Minus_mm_Mult_qq(bucket->buckets[i], m, p1,
                            bucket->buckets_length[i], l1, 
                            spNoether,
                            bucket->bucket_ring);
    l1 = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
    while (bucket->buckets[i] != NULL)
    {
      p1 = p_Add_q(p1, bucket->buckets[i],
                     l1, bucket->buckets_length[i], bucket->bucket_ring);
      bucket->buckets[i] = NULL;
      bucket->buckets_length[i] = 0;
      i = pLogLength(l1);
    }
  }
  else
  {
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
    p1 = bucket->bucket_ring->p_Procs->pp_Mult_mm(p1, m, spNoether, bucket->bucket_ring);
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
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
                               spNoether, bucket->bucket_ring);
#endif
  kbTest(bucket);
}

/////////////////////////////////////////////////////////////////////////////
//
// Extract all monomials from bucket with component comp
// Return as a polynomial *p with length *l
// In other words, afterwards
// Bpoly == Bpoly - (poly consisting of all monomials with component comp)
// and components of monomials of *p are all 0
//
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
         pDivisibleBy(p1,  kBucketGetLm(bucket)));
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
    pSetCoeff(lm, bn);
    if ((ct == 0) || (ct == 2)) kBucket_Mult_n(bucket, an);
    rn = an;
  }
  else
  {
    rn = nInit(1);
  }

  if (pGetComp(p1) != pGetComp(lm))
  {
    pSetCompP(a1, pGetComp(lm));
    reset_vec = TRUE;
    pSetComp(lm, pGetComp(p1));
    pSetm(lm);
  }

  pExpVectorSub(lm,p1);
  l1--;

  kBucket_Minus_m_Mult_p(bucket, lm, a1, &l1, spNoether);

  p_DeleteLm(&lm, bucket->bucket_ring);
  if (reset_vec) pSetCompP(a1, 0);
  kbTest(bucket);
  return rn;
}


