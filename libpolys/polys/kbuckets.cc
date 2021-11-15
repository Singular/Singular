/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "misc/auxiliary.h"
#include "misc/options.h"

#include "polys/monomials/p_polys.h"
#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "polys/monomials/ring.h"
#include "polys/kbuckets.h"

#ifdef HAVE_SHIFTBBA
#include "polys/shiftop.h"
#endif

#ifdef HAVE_COEF_BUCKETS
#define USE_COEF_BUCKETS
#endif

#ifdef USE_COEF_BUCKETS
#ifdef HAVE_RINGS_OLD
#define MULTIPLY_BUCKET(B,I) do                                        \
  { if (B->coef[I]!=NULL)                                              \
    {                                                                  \
      assume(p_IsConstant(B->Coef[i],B->bucket->ring));           \
      B->buckets[I]=p_Mult_q(B->buckets[I],B->coef[I],B->bucket_ring); \
      B->coef[I]=NULL;                                                 \
    }                                                                  \
  } while(0)                                                           \
    if (rField_is_Ring(B->bucket_ring)) B->buckets_length[i] = pLength(B->buckets[i]);
#else
#define MULTIPLY_BUCKET(B,I) do                                        \
  { if (B->coef[I]!=NULL)                                              \
    {                                                                  \
      B->buckets[I]=p_Mult_q(B->buckets[I],B->coef[I],B->bucket_ring); \
      B->coef[I]=NULL;                                                 \
    }                                                                  \
  } while(0)
#endif
#else
#define MULTIPLY_BUCKET(B,I)
#endif
STATIC_VAR omBin kBucket_bin = omGetSpecBin(sizeof(kBucket));
#ifdef USE_COEF_BUCKETS
STATIC_VAR int coef_start=1;
#endif
//////////////////////////////////////////////////////////////////////////
///
/// Some internal stuff
///

// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
#ifndef BUCKET_TWO_BASE
static inline int LOG4(int v)
{
  const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  const unsigned int S[] = {1, 2, 4, 8, 16};

  unsigned int r = 0; // result of log4(v) will go here
  if (v & b[4]) { v >>= S[4]; r |= S[3]; }
  if (v & b[3]) { v >>= S[3]; r |= S[2]; }
  if (v & b[2]) { v >>= S[2]; r |= S[1]; }
  if (v & b[1]) { v >>= S[1]; r |= S[0]; }
  return (int)r;
}
#endif

// returns ceil(log_4(l))
static inline unsigned int pLogLength(unsigned int l)
{
  unsigned int i = 0;

  if (l == 0) return 0;
  l--;
#ifdef BUCKET_TWO_BASE
  i=SI_LOG2(l);
#else
  i=LOG4(l);
#endif
  return i+1;
}

// returns ceil(log_4(pLength(p)))
static inline unsigned int pLogLength(poly p)
{
  return pLogLength((unsigned int) pLength(p));
}

#ifdef KDEBUG

#ifndef HAVE_PSEUDO_BUCKETS
BOOLEAN kbTest_i(kBucket_pt bucket, int i)
{//sBucketSortMerge
  #ifdef USE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  if ((bucket->coef[i]!=NULL) && (bucket->buckets[i]==NULL))
  {
    dReportError("Bucket %d coef not NULL", i);
  }
  if (bucket->coef[i]!=NULL)
  {
    assume(bucket->buckets[i]!=NULL);
    p_Test(bucket->coef[i],bucket->bucket_ring);
  }
  #endif
  pFalseReturn(p_Test(bucket->buckets[i], bucket->bucket_ring));
  if ((unsigned)bucket->buckets_length[i] != pLength(bucket->buckets[i]))
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
  #ifdef HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  #endif
  int i;
  poly lm = bucket->buckets[0];

  omCheckAddrBin(bucket, kBucket_bin);
  assume(bucket->buckets_used <= MAX_BUCKET);
  if (! kbTest_i(bucket, 0)) return FALSE;
  for (i=1; i<= (int) bucket->buckets_used; i++)
  {
    if (!kbTest_i(bucket, i)) return FALSE;
    if (lm != NULL &&  bucket->buckets[i] != NULL
        && p_LmCmp(lm, bucket->buckets[i], bucket->bucket_ring) != 1)
    {
      dReportError("Bucket %d larger or equal than lm", i);
      if (p_LmCmp(lm, bucket->buckets[i], bucket->bucket_ring) ==0)
        dReportError("Bucket %d equal to lm", i);
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
  for(i=0;i<=MAX_BUCKET;i++)
  {
    if (bucket->buckets[i]!=NULL)
    {
      int j;
      for(j=i+1;j<=MAX_BUCKET;j++)
      {
        if (bucket->buckets[j]==bucket->buckets[i])
        {
          dReportError("Bucket %d %d equal", i,j);
          return FALSE;
        }
      }
    }
    #ifdef HAVE_COEF_BUCKETS
    if (bucket->coef[i]!=NULL)
    {
      int j;
      for(j=i+1;j<=MAX_BUCKET;j++)
      {
        if (bucket->coef[j]==bucket->coef[i])
        {
          dReportError("internal coef %d %d equal", i,j);
          return FALSE;
        }
      }
    }
    #endif
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

kBucket_pt kBucketCreate(const ring bucket_ring)
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
    p_Delete(&(bucket->buckets[i]), bucket->bucket_ring);
#ifdef USE_COEF_BUCKETS
    p_Delete(&(bucket->coef[i]), bucket->bucket_ring);
#endif
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
  kbTest(bucket);
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
#ifndef USE_COEF_BUCKETS
    MULTIPLY_BUCKET(bucket,i);
    pNext(lm) = bucket->buckets[i];
    bucket->buckets[i] = lm;
    bucket->buckets_length[i]++;
    assume(i <= bucket->buckets_used+1);
    if (i > bucket->buckets_used)  bucket->buckets_used = i;
    bucket->buckets[0] = NULL;
    bucket->buckets_length[0] = 0;
    kbTest(bucket);
#else
    if (i > bucket->buckets_used)  bucket->buckets_used = i;
    assume(i!=0);
    if (bucket->buckets[i]!=NULL)
    {
       MULTIPLY_BUCKET(bucket,i);
       pNext(lm) = bucket->buckets[i];
       bucket->buckets[i] = lm;
       bucket->buckets_length[i]++;
       assume(i <= bucket->buckets_used+1);
    }
    else
    {
      #if 1
       assume(bucket->buckets[i]==NULL);
       assume(bucket->coef[0]==NULL);
       assume(pLength(lm)==1);
       assume(pNext(lm)==NULL);
       number coef=p_GetCoeff(lm,bucket->bucket_ring);
       //WARNING: not thread_safe
       p_SetCoeff0(lm, n_Init(1,bucket->bucket_ring), bucket->bucket_ring);
       bucket->buckets[i]=lm;
       bucket->buckets_length[i]=1;
       bucket->coef[i]=p_NSet(n_Copy(coef,bucket->bucket_ring),bucket->bucket_ring);

       bucket->buckets[i]=lm;
       bucket->buckets_length[i]=1;
      #else
       MULTIPLY_BUCKET(bucket,i);
       pNext(lm) = bucket->buckets[i];
       bucket->buckets[i] = lm;
       bucket->buckets_length[i]++;
       assume(i <= bucket->buckets_used+1);
      #endif
    }
    bucket->buckets[0]=NULL;
    bucket->buckets_length[0] = 0;
    bucket->coef[0]=NULL;
    kbTest(bucket);
    #endif
  }

}

BOOLEAN kBucketIsCleared(kBucket_pt bucket)
{
  int i;

  for (i = 0;i<=MAX_BUCKET;i++)
  {
    if (bucket->buckets[i] != NULL) return FALSE;
    #ifdef HAVE_COEF_BUCKETS
    if (bucket->coef[i] != NULL) return FALSE;
    #endif
    if (bucket->buckets_length[i] != 0) return FALSE;
  }
  return TRUE;
}

void kBucketInit(kBucket_pt bucket, poly lm, int length)
{
  //assume(false);
  assume(bucket != NULL);
  assume(length <= 0 || (unsigned)length == pLength(lm));
  assume(kBucketIsCleared(bucket));

  if (lm == NULL) return;

  if (length <= 0)
    length = pLength(lm);

  bucket->buckets[0] = lm;
  #ifdef HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  #endif
  #ifdef USE_COEF_BUCKETS
  bucket->coef[0]=NULL;
  #endif
  if (lm!=NULL)
    bucket->buckets_length[0] = 1;
  else
    bucket->buckets_length[0]= 0;
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
#ifndef HAVE_PSEUDO_BUCKETS
  assume(bucket->buckets_used<=MAX_BUCKET);
  MULTIPLY_BUCKET(bucket,1);
  kbTest(bucket);
  poly p = bucket->buckets[1];
  poly lm;
  int pl = bucket->buckets_length[1];//, i;
  int i;
  bucket->buckets[1] = NULL;
  bucket->buckets_length[1] = 0;
  #ifdef USE_COEF_BUCKETS
    assume(bucket->coef[1]==NULL);
  #endif
  ring r=bucket->bucket_ring;


  for (i=1; i<=bucket->buckets_used; i++)
  {
  #ifdef USE_COEF_BUCKETS
    if (bucket->coef[i]!=NULL)
    {
      assume(bucket->buckets[i]!=NULL);
      p = p_Plus_mm_Mult_qq(p, bucket->coef[i], bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
      p_Delete(&bucket->coef[i],r);
      p_Delete(&bucket->buckets[i],r);
    }
    else
    p = p_Add_q(p, bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
  #else
    p = p_Add_q(p, bucket->buckets[i],
                 pl, bucket->buckets_length[i], r);
  #endif
    if (i==1) continue;
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
  }
  #ifdef HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  #endif
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
  assume(bucket->buckets_used <= MAX_BUCKET);
  #ifdef USE_COEF_BUCKETS
    assume(bucket->coef[0]==NULL);
    assume(bucket->coef[i]==NULL);
  #endif
  assume(pLength(p) == (unsigned)pl);
  //if (TEST_OPT_PROT) { Print("C(%d)",pl); }
  kbTest(bucket);
  return i;
#endif
}

void kBucketNormalize(kBucket_pt bucket)
{
#ifdef HAVE_PSEUDO_BUCKETS
  p_Normalize(bucket->p,bucket->bucket_ring);
#else
  MULTIPLY_BUCKET(bucket,1);
  for (int i=0; i<=bucket->buckets_used; i++)
  {
    p_Normalize(bucket->buckets[i],bucket->bucket_ring);
  }
#endif
}

void kBucketClear(kBucket_pt bucket, poly *p, int *length)
{
  int i = kBucketCanonicalize(bucket);
  if (i > 0)
  {
  #ifdef USE_COEF_BUCKETS
    MULTIPLY_BUCKET(bucket,i);
    //bucket->coef[i]=NULL;
  #endif
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
/// Bucket number i from bucket is out of length sync, resync
///
void kBucketAdjust(kBucket_pt bucket, int i) {

  MULTIPLY_BUCKET(bucket,i);

  int l1 = bucket->buckets_length[i];
  poly p1 = bucket->buckets[i];
  bucket->buckets[i] = NULL;
  bucket->buckets_length[i] = 0;
  i = pLogLength(l1);

  while (bucket->buckets[i] != NULL)
  {
    //kbTest(bucket);
    MULTIPLY_BUCKET(bucket,i);
    p1 = p_Add_q(p1, bucket->buckets[i],
                 l1, bucket->buckets_length[i], bucket->bucket_ring);
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
      if (i<coef_start)
        bucket->buckets[i] = __p_Mult_nn(bucket->buckets[i], n, r);
        /* Frank Seelisch on March 11, 2010:
           This looks a bit strange: The following "if" is indented
           like the previous line of code. But coded as it is,
           it should actually be two spaces less indented.
           Question: Should the following "if" also only be
           performed when "(i<coef_start)" is true?
           For the time being, I leave it as it is. */
        if (rField_is_Ring(r) && !(rField_is_Domain(r)))
        {
          bucket->buckets_length[i] = pLength(bucket->buckets[i]);
          kBucketAdjust(bucket, i);
        }
      else
      if (bucket->coef[i]!=NULL)
      {
        bucket->coef[i] = __p_Mult_nn(bucket->coef[i],n,r);
      }
      else
      {
        bucket->coef[i] = p_NSet(n_Copy(n,r),r);
      }
#else
      bucket->buckets[i] = __p_Mult_nn(bucket->buckets[i], n, r);
#endif
    }
  }
  if (rField_is_Ring(r) && !(rField_is_Domain(r)))
  {
    for (i=0; i<= bucket->buckets_used; i++)
    {
      if (bucket->buckets[i] != NULL)
      {
        bucket->buckets_length[i] = pLength(bucket->buckets[i]);
        kBucketAdjust(bucket, i);
      }
    }
  }
  kbTest(bucket);
#else
  bucket->p = __p_Mult_nn(bucket->p, n, bucket->bucket_ring);
#endif
}


//////////////////////////////////////////////////////////////////////////
///
/// Add to Bucket a poly ,i.e. Bpoly == q+Bpoly
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
    assume(i<= MAX_BUCKET);
    assume(bucket->buckets_used<= MAX_BUCKET);
  }

  kbTest(bucket);
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

#if defined(HAVE_PLURAL)
  if ((rField_is_Ring(r) && !(rField_is_Domain(r)))
  ||(rIsPluralRing(r)))
  {
    pSetCoeff0(m, n_InpNeg(pGetCoeff(m),r->cf));
    p1=r->p_Procs->pp_mm_Mult(p,m,r);
    pSetCoeff0(m, n_InpNeg(pGetCoeff(m),r->cf));
    l1=pLength(p1);
    i = pLogLength(l1);
  }
  else
#endif
  {
    if ((i <= bucket->buckets_used) && (bucket->buckets[i] != NULL))
    {
      assume(pLength(bucket->buckets[i])==(unsigned)bucket->buckets_length[i]);
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
      pSetCoeff0(m, n_InpNeg(pGetCoeff(m),r->cf));
      if (spNoether != NULL)
      {
        l1 = -1;
        p1 = r->p_Procs->pp_Mult_mm_Noether(p1, m, spNoether, l1, r);
        i = pLogLength(l1);
      }
      else
      {
        p1 = r->p_Procs->pp_mm_Mult(p1, m, r);
      }
      pSetCoeff0(m, n_InpNeg(pGetCoeff(m),r->cf));
    }
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
    assume((!rIsPluralRing(bucket->bucket_ring))||p_IsConstant(m, bucket->bucket_ring));
  assume(l <= 0 || pLength(p) == (unsigned)l);
  int i, l1;
  poly p1 = p;
  ring r = bucket->bucket_ring;

  if (m == NULL || p == NULL) return;

  if (l <= 0)
  {
    l1 = pLength(p1);
    l = l1;
  }
  else
    l1 = l;

  kBucketMergeLm(bucket);
  kbTest(bucket);
  i = pLogLength(l1);
  #ifdef USE_COEF_BUCKETS
  number n=n_Init(1,r->cf);
  #endif
  if (i <= bucket->buckets_used && bucket->buckets[i] != NULL)
  {
    //if (FALSE){
    #ifdef USE_COEF_BUCKETS
    if ((bucket->coef[i]!=NULL) &&(i>=coef_start))
    {
      number orig_coef=p_GetCoeff(bucket->coef[i],r);
      //we take ownership:
      p_SetCoeff0(bucket->coef[i],n_Init(0,r),r);
      number add_coef=n_Copy(p_GetCoeff(m,r),r);
      number gcd=n_Gcd(add_coef, orig_coef,r);

      if (!(n_IsOne(gcd,r)))
      {
        number orig_coef2=n_ExactDiv(orig_coef,gcd,r);
        number add_coef2=n_ExactDiv(add_coef, gcd,r);
        n_Delete(&orig_coef,r);
        n_Delete(&add_coef,r);
        orig_coef=orig_coef2;
        add_coef=add_coef2;

        //p_Mult_nn(bucket->buckets[i], orig_coef,r);
        n_Delete(&n,r);
        n=gcd;
      }

      //assume(n_IsOne(n,r));
      number backup=p_GetCoeff(m,r);

      p_SetCoeff0(m,add_coef,r);
      bucket->buckets[i]=__p_Mult_nn(bucket->buckets[i],orig_coef,r);

      n_Delete(&orig_coef,r);
      p_Delete(&bucket->coef[i],r);

      p1 = p_Plus_mm_Mult_qq(bucket->buckets[i], m, p1,
                           bucket->buckets_length[i], l1, r);
      l1=bucket->buckets_length[i];
      bucket->buckets[i]=NULL;
      bucket->buckets_length[i] = 0;
      i = pLogLength(l1);
      assume(l1==pLength(p1));

      p_SetCoeff(m,backup,r); //deletes add_coef
    }
    else
    #endif
    {
      MULTIPLY_BUCKET(bucket,i);
      p1 = p_Plus_mm_Mult_qq(bucket->buckets[i], m, p1,
                           bucket->buckets_length[i], l1, r);
      l1 = bucket->buckets_length[i];
      bucket->buckets[i] = NULL;
      bucket->buckets_length[i] = 0;
      i = pLogLength(l1);
    }
  }
  else
  {
    #ifdef USE_COEF_BUCKETS
    number swap_n=p_GetCoeff(m,r);

    assume(n_IsOne(n,r));
    p_SetCoeff0(m,n,r);
    n=swap_n;
    //p_SetCoeff0(n, swap_n, r);
    //p_GetCoeff0(n, swap_n,r);
    #endif
    p1 = r->p_Procs->pp_Mult_mm(p1, m, r);
    #ifdef USE_COEF_BUCKETS
    //m may not be changed
    p_SetCoeff(m,n_Copy(n,r),r);
    #endif
  }

  while ((bucket->buckets[i] != NULL) && (p1!=NULL))
  {
    assume(i!=0);
    #ifdef USE_COEF_BUCKETS
    if ((bucket->coef[i]!=NULL) &&(i>=coef_start))
    {
      number orig_coef=p_GetCoeff(bucket->coef[i],r);
      //we take ownership:
      p_SetCoeff0(bucket->coef[i],n_Init(0,r),r);
      number add_coef=n_Copy(n,r);
      number gcd=n_Gcd(add_coef, orig_coef,r);

      if (!(n_IsOne(gcd,r)))
      {
        number orig_coef2=n_ExactDiv(orig_coef,gcd,r);
        number add_coef2=n_ExactDiv(add_coef, gcd,r);
        n_Delete(&orig_coef,r);
        n_Delete(&n,r);
        n_Delete(&add_coef,r);
        orig_coef=orig_coef2;
        add_coef=add_coef2;
        //p_Mult_nn(bucket->buckets[i], orig_coef,r);
        n=gcd;
      }
      //assume(n_IsOne(n,r));
      bucket->buckets[i]=__p_Mult_nn(bucket->buckets[i],orig_coef,r);
      p1=__p_Mult_nn(p1,add_coef,r);

      p1 = p_Add_q(p1, bucket->buckets[i],r);
      l1=pLength(p1);

      bucket->buckets[i]=NULL;
      n_Delete(&orig_coef,r);
      p_Delete(&bucket->coef[i],r);
      //l1=bucket->buckets_length[i];
      assume(l1==pLength(p1));
    }
    else
    #endif
    {
      //don't do that, pull out gcd
      #ifdef USE_COEF_BUCKETS
      if(!(n_IsOne(n,r)))
      {
        p1=__p_Mult_nn(p1, n, r);
        n_Delete(&n,r);
        n=n_Init(1,r);
      }
      #endif
      MULTIPLY_BUCKET(bucket,i);
      p1 = p_Add_q(p1, bucket->buckets[i],
                 l1, bucket->buckets_length[i], r);
      bucket->buckets[i] = NULL;
      bucket->buckets_length[i] = 0;
    }
    i = pLogLength(l1);
  }

  bucket->buckets[i] = p1;
#ifdef USE_COEF_BUCKETS
  assume(bucket->coef[i]==NULL);

  if (!(n_IsOne(n,r)))
  {
    bucket->coef[i]=p_NSet(n,r);
  }
  else
  {
    bucket->coef[i]=NULL;
    n_Delete(&n,r);
  }

  if (p1==NULL)
    p_Delete(&bucket->coef[i],r);
#endif
  bucket->buckets_length[i]=l1;
  if (i > bucket->buckets_used)
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
extern void p_TakeOutComp(poly *p, long comp, poly *q, int *lq, const ring r);

void kBucketTakeOutComp(kBucket_pt bucket,
                        long comp,
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
      p_TakeOutComp(&(bucket->buckets[i]), comp, &q, &lq, bucket->bucket_ring);
      if (q != NULL)
      {
        assume(pLength(q) == (unsigned)lq);
        bucket->buckets_length[i] -= lq;
        assume(pLength(bucket->buckets[i]) == (unsigned)bucket->buckets_length[i]);
        p = p_Add_q(p, q, lp, lq, bucket->bucket_ring);
      }
    }
  }
  kBucketAdjustBucketsUsed(bucket);
#else
  p_TakeOutComp(&(bucket->p), comp, &p, &lp,bucket->bucket_ring);
  (bucket->l) -= lp;
#endif
  *r_p = p;
  *l = lp;

  kbTest(bucket);
}

/////////////////////////////////////////////////////////////////////////////
// Reduction of Bpoly with a given poly
//

extern int ksCheckCoeff(number *a, number *b);

number kBucketPolyRed(kBucket_pt bucket,
                      poly p1, int l1,
                      poly spNoether)
{
  ring r=bucket->bucket_ring;
  assume((!rIsPluralRing(r))||p_LmEqual(p1,kBucketGetLm(bucket), r));
  assume(p1 != NULL &&
         p_DivisibleBy(p1,  kBucketGetLm(bucket), r));
  assume(pLength(p1) == (unsigned) l1);

  poly a1 = pNext(p1), lm = kBucketExtractLm(bucket);
  BOOLEAN reset_vec=FALSE;
  number rn;

  /* we shall reduce bucket=bn*lm+... by p1=an*t+a1 where t=lm(p1)
     and an,bn shall be defined further down only if lc(p1)!=1
     we already know: an|bn and t|lm */
  if(a1==NULL)
  {
    p_LmDelete(&lm, r);
    return n_Init(1,r->cf);
  }

  if (! n_IsOne(pGetCoeff(p1),r->cf))
  {
    number an = pGetCoeff(p1), bn = pGetCoeff(lm);
//StringSetS("##### an = "); nWrite(an); PrintS(StringEndS("\n")); // NOTE/TODO: use StringAppendS("\n"); omFree(s);
//StringSetS("##### bn = "); nWrite(bn); PrintS(StringEndS("\n")); // NOTE/TODO: use StringAppendS("\n"); omFree(s);
    /* ksCheckCoeff: divide out gcd from an and bn: */
    int ct = ksCheckCoeff(&an, &bn,r->cf);
    /* the previous command returns ct=0 or ct=2 iff an!=1
       note: an is now 1 or -1 */

    /* setup factor for p1 which cancels leading terms */
    p_SetCoeff(lm, bn, r);
    if ((ct == 0) || (ct == 2))
    {
      /* next line used to be here before but is WRONG:
      kBucket_Mult_n(bucket, an);
        its use would result in a wrong sign for the tail of bucket
        in the reduction */

      /* correct factor for cancelation by changing sign if an=-1 */
      if (rField_is_Ring(r))
        lm = __p_Mult_nn(lm, an, r);
      else
        kBucket_Mult_n(bucket, an);
    }
    rn = an;
  }
  else
  {
    rn = n_Init(1,r->cf);
  }

  if (p_GetComp(p1, r) != p_GetComp(lm, r))
  {
    p_SetCompP(a1, p_GetComp(lm, r), r);
    reset_vec = TRUE;
    p_SetComp(lm, p_GetComp(p1, r), r);
    p_Setm(lm, r);
  }

  p_ExpVectorSub(lm, p1, r);
  l1--;

  assume((unsigned)l1==pLength(a1));

#ifdef HAVE_SHIFTBBA
  poly lmRight;
  if (r->isLPring) {
    int firstBlock = p_mFirstVblock(p1, r);
    k_SplitFrame(lm, lmRight, si_max(firstBlock, 1), r);
  }
#endif
#if 0
  BOOLEAN backuped=FALSE;
  number coef;
  //@Viktor, don't ignore coefficients on monomials
  if(l1==1) {

    //if (rField_is_Q(r)) {
      //avoid this for function fields, as gcds are expensive at the moment


      coef=p_GetCoeff(a1,r);
      lm=p_Mult_nn(lm, coef, r);
      p_SetCoeff0(a1, n_Init(1,r), r);
      backuped=TRUE;
      //WARNING: not thread_safe
    //deletes coef as side effect
    //}
  }
#endif

#ifdef HAVE_SHIFTBBA
  if (r->isLPring)
  {
    kBucket_Minus_m_Mult_p(bucket, lm, r->p_Procs->p_Mult_mm(a1, lmRight, r), &l1, spNoether);
  }
  else
#endif
  {
    kBucket_Minus_m_Mult_p(bucket, lm, a1, &l1, spNoether);
  }

#if 0
  if (backuped)
    p_SetCoeff0(a1,coef,r);
#endif

  p_LmDelete(&lm, r);
#ifdef HAVE_SHIFTBBA
  if (r->isLPring)
  {
    p_LmDelete(&lmRight, r);
  }
#endif
  if (reset_vec) p_SetCompP(a1, 0, r);
  kbTest(bucket);
  return rn;
}

#ifndef USE_COEF_BUCKETS
void kBucketSimpleContent(kBucket_pt bucket)
{
  if (bucket->buckets[0]==NULL) return;

  ring r=bucket->bucket_ring;
  if (rField_is_Ring(r)) return;

  coeffs cf=r->cf;
  if (cf->cfSubringGcd==ndGcd) /* trivial gcd*/ return;

  number nn=pGetCoeff(bucket->buckets[0]);
  //if ((bucket->buckets_used==0)
  //&&(!n_IsOne(nn,cf)))
  //{
  //  if (TEST_OPT_PROT) PrintS("@");
  //  p_SetCoeff(bucket->buckets[0],n_Init(1,cf),r);
  //  return;
  //}

  if (n_Size(nn,cf)<2) return;

  //kBucketAdjustBucketsUsed(bucket);
  number coef=n_Copy(nn,cf);
  // find an initial guess of a gcd
  for (int i=1; i<=bucket->buckets_used;i++)
  {
    if (bucket->buckets[i]!=NULL)
    {
      number t=p_InitContent(bucket->buckets[i],r);
      if (n_Size(t,cf)<2)
      {
        n_Delete(&t,cf);
        n_Delete(&coef,cf);
        return;
      }
      number t2=n_SubringGcd(coef,t,cf);
      n_Delete(&t,cf);
      n_Delete(&coef,cf);
      coef=t2;
      if (n_Size(coef,cf)<2) { n_Delete(&coef,cf);return;}
    }
  }
  // find the gcd
  for (int i=0; i<=bucket->buckets_used;i++)
  {
    if (bucket->buckets[i]!=NULL)
    {
      poly p=bucket->buckets[i];
      while(p!=NULL)
      {
        number t=n_SubringGcd(coef,pGetCoeff(p),cf);
        if (n_Size(t,cf)<2)
        {
          n_Delete(&t,cf);
          n_Delete(&coef,cf);
          return;
        }
        pIter(p);
      }
    }
  }
  // divided by the gcd
  if (TEST_OPT_PROT) PrintS("@");
  for (int i=bucket->buckets_used;i>=0;i--)
  {
    if (bucket->buckets[i]!=NULL)
    {
      poly p=bucket->buckets[i];
      while(p!=NULL)
      {
        number d = n_ExactDiv(pGetCoeff(p),coef,cf);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
  }
  n_Delete(&coef,cf);
}
#else
static BOOLEAN nIsPseudoUnit(number n, ring r)
{
  if (rField_is_Zp(r))
    return TRUE;

  if (rParameter(r)==NULL)
  {
    return (n_Size(n,r->cf)==1);
  }
  //if (r->parameter!=NULL)
  return (n_IsOne(n,r->cf) || n_IsMOne(n,r->cf));
}

void kBucketSimpleContent(kBucket_pt bucket)
{
  ring r=bucket->bucket_ring;
  int i;
  //PrintS("HHHHHHHHHHHHH");
  for (i=0;i<=MAX_BUCKET;i++)
  {
    //if ((bucket->buckets[i]!=NULL) && (bucket->coef[i]!=NULL))
    //    PrintS("H2H2H2");
    if (i==0)
    {
      assume(bucket->buckets[i]==NULL);
    }
    if ((bucket->buckets[i]!=NULL) && (bucket->coef[i]==NULL))
      return;
  }
  for (i=0;i<=MAX_BUCKET;i++)
  {
    //if ((bucket->buckets[i]!=NULL) && (bucket->coef[i]!=NULL))
    //    PrintS("H2H2H2");
    if (i==0)
    {
      assume(bucket->buckets[i]==NULL);
    }
    if ((bucket->buckets[i]!=NULL)
    && (nIsPseudoUnit(p_GetCoeff(bucket->coef[i],r),r)))
      return;
  }
  //return;

  number coef=n_Init(0,r);
  //ATTENTION: will not work correct for GB over ring
  //if (TEST_OPT_PROT)
  //    PrintS("CCCCCCCCCCCCC");
  for (i=MAX_BUCKET;i>=0;i--)
  {
    if (i==0)
    {
      assume(bucket->buckets[i]==NULL);
    }
    if (bucket->buckets[i]!=NULL)
    {
      assume(bucket->coef[i]!=NULL);
      assume(!(n_IsZero(pGetCoeff(bucket->coef[i]),r)));

      //in this way it should crash on programming errors, yeah
      number temp=n_Gcd(coef, pGetCoeff(bucket->coef[i]),r);
      n_Delete(&coef,r );
      coef=temp;
      if (nIsPseudoUnit(coef,r))
      {
        n_Delete(&coef,r);
        return;
      }
      assume(!(n_IsZero(coef,r)));
    }
  }
  if (n_IsZero(coef,r))
  {
    n_Delete(&coef,r);
    return;
  }
  if (TEST_OPT_PROT)
    PrintS("S");
  for(i=0;i<=MAX_BUCKET;i++)
  {
    if (bucket->buckets[i]!=NULL)
    {
      assume(!(n_IsZero(coef,r)));
      assume(bucket->coef[i]!=NULL);
      number lc=p_GetCoeff(bucket->coef[i],r);
      p_SetCoeff(bucket->coef[i], n_ExactDiv(lc,coef,r),r);
      assume(!(n_IsZero(p_GetCoeff(bucket->coef[i],r),r)));
    }
  }
  n_Delete(&coef,r);
}
#endif


poly kBucketExtractLmOfBucket(kBucket_pt bucket, int i)
{
  assume(bucket->buckets[i]!=NULL);

  poly p=bucket->buckets[i];
  bucket->buckets_length[i]--;
#ifdef USE_COEF_BUCKETS
  ring r=bucket->bucket_ring;
  if (bucket->coef[i]!=NULL)
  {
    poly next=pNext(p);
    if (next==NULL)
    {
      MULTIPLY_BUCKET(bucket,i);
      p=bucket->buckets[i];
      bucket->buckets[i]=NULL;
      return p;
    }
    else
    {
      bucket->buckets[i]=next;
      number c=p_GetCoeff(bucket->coef[i],r);
      pNext(p)=NULL;
      p=__p_Mult_nn(p,c,r);
      assume(p!=NULL);
      return p;
    }
  }
  else
#endif
  {
    bucket->buckets[i]=pNext(bucket->buckets[i]);
    pNext(p)=NULL;
    assume(p!=NULL);
    return p;
  }
}

/*
* input - output: a, b
* returns:
*   a := a/gcd(a,b), b := b/gcd(a,b)
*   and return value
*       0  ->  a != 1,  b != 1
*       1  ->  a == 1,  b != 1
*       2  ->  a != 1,  b == 1
*       3  ->  a == 1,  b == 1
*   this value is used to control the spolys
*/
int ksCheckCoeff(number *a, number *b, const coeffs r)
{
  int c = 0;
  number an = *a, bn = *b;
  n_Test(an,r);
  n_Test(bn,r);

  number cn = n_SubringGcd(an, bn, r);

  if(n_IsOne(cn, r))
  {
    an = n_Copy(an, r);
    bn = n_Copy(bn, r);
  }
  else
  {
    an = n_ExactDiv(an, cn, r);
    bn = n_ExactDiv(bn, cn, r);
  }
  n_Delete(&cn, r);
  if (n_IsOne(an, r))
  {
    c = 1;
  }
  if (n_IsOne(bn, r))
  {
    c += 2;
  }
  *a = an;
  *b = bn;
  return c;
}

