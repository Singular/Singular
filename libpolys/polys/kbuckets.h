#ifndef KBUCKETS_H
#define KBUCKETS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

//#define HAVE_COEF_BUCKETS

/////////////////////////////////////////////////////////////////////////
// configuration
//

// define to not really use the bucket feature
// #define HAVE_PSEUDO_BUCKETS
//class  kBucket; typedef kBucket* kBucket_pt; // ring.h

#include "polys/monomials/ring.h" // for ring->p_Procs->p_kBucketSetLm!
#include "polys/templates/p_Procs.h" // for p_kBucketSetLm_Proc_Ptr

//////////////////////////////////////////////////////////////////////////
// Creation/Destruction of buckets
//
kBucket_pt kBucketCreate(const ring r);
// only free memory allocated for bucket
void kBucketDestroy(kBucket_pt *bucket);
// frees polys/monomials in bucket and destroys bucket
void kBucketDeleteAndDestroy(kBucket_pt *bucket);


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to Bpolys
//

// Converts p into a bucket poly (Bpoly) and destroys p
// Assumes length <= 0 || pLength(p) == length
void kBucketInit(kBucket_pt bucket, poly p, int length);

// Converts Bpoly into a poly and clears bucket
// i.e., afterwards Bpoly == 0
void kBucketClear(kBucket_pt bucket, poly *p, int *length);

inline poly kBucketClear(kBucket_pt bucket)
{
  int dummy;
  poly p;
  kBucketClear(bucket, &p, &dummy);
  return p;
}

/// Canonicalizes Bpoly, i.e. converts polys of buckets into one poly in
/// one bucket: Returns number of bucket into which it is canonicalized
int kBucketCanonicalize(kBucket_pt bucket);

/// apply n_Normalize to all coefficients
void kBucketNormalize(kBucket_pt bucket);

/////////////////////////////////////////////////////////////////////////////
// Extracts lm of Bpoly, i.e. Bpoly is changed s.t.
// Bpoly == Bpoly - Lm(Bpoly)
//
inline poly kBucketExtractLm(kBucket_pt bucket);

/////////////////////////////////////////////////////////////////////////////
// Sets Lm of Bpoly, i.e. Bpoly is changed s.t.
// Bpoly = Bpoly + m
// assumes that m is larger than all monomials of Bpoly
void kBucketSetLm(kBucket_pt bucket, poly lm);


//////////////////////////////////////////////////////////////////////////
///
/// Bucket number i from bucket is out of length sync, resync
///
void kBucketAdjust(kBucket_pt bucket, int i);

/////////////////////////////////////////////////////////////////////////////
// Reduces Bpoly (say, q) with p, i.e.:
// q = (Lc(p) / gcd(Lc(p), Lc(q)))*q - (Lc(q)/gcd(Lc(p),Lc(q)))*p*(Lm(q)/Lm(p))
// Assumes p1 != NULL, Bpoly != NULL
//         Lm(p1) divides Lm(Bpoly)
//         pLength(p1) == l1
// Returns: Lc(p) / gcd(Lc(p), Lc(q))
number kBucketPolyRed(kBucket_pt bucket,
                      poly p, int l,
                      poly spNoether);
void kBucketPolyRedNF(kBucket_pt bucket,
                      poly p1, int l1,
                      poly spNoether);



/////////////////////////////////////////////////////////////////////////////
//
// Extract all monomials from bucket with component comp
// Return as a polynomial *p with length *l
// In other words, afterwards
// Bpoly == Bpoly - (poly consisting of all monomials with component comp)
// and components of monomials of *p are all 0

void kBucketTakeOutComp(kBucket_pt bucket,
                        long comp,
                        poly *p, int *l);

//////////////////////////////////////////////////////////////////////////
///
/// Multiply Bucket by number ,i.e. Bpoly == n*Bpoly
///
void kBucket_Mult_n(kBucket_pt bucket, number n);

//////////////////////////////////////////////////////////////////////////
///
/// Extract all monomials of bucket which are larger than q
/// Append those to append, and return last monomial of append
poly kBucket_ExtractLarger(kBucket_pt bucket, poly q, poly append);


//////////////////////////////////////////////////////////////////////////
///
/// Add to Bucket a poly ,i.e. Bpoly == Bpoly + q
///
void kBucket_Add_q(kBucket_pt bucket, poly q, int* lq);

// first, do ExtractLarger
// then add q
inline poly
kBucket_ExtractLarger_Add_q(kBucket_pt bucket, poly append, poly q, int *lq)
{
  append = kBucket_ExtractLarger(bucket, q, append);
  kBucket_Add_q(bucket, q, lq);
  return append;
}

//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly - m*p; where m is a monom
/// Does not destroy p and m (TODO: rename into kBucket_Minus_mm_Mult_pp!?)
/// assume (*l <= 0 || pLength(p) == *l)
void kBucket_Minus_m_Mult_p(kBucket_pt bucket, poly m, poly p, int *l,
                            poly spNother = NULL);

//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly + m*p; where m is a monom
/// Does not destroy p and m
/// assume (l <= 0 || pLength(p) == l)
void kBucket_Plus_mm_Mult_pp(kBucket_pt bucket, poly m, poly p, int l);

//////////////////////////////////////////////////////////////////////////
///
/// For changing the ring of the Bpoly to new_tailBin
///
void kBucketShallowCopyDelete(kBucket_pt bucket,
                              ring new_tailRing, omBin new_tailBin,
                              pShallowCopyDeleteProc p_shallow_copy_delete);

//////////////////////////////////////////////////////////////////////////
///
/// Tests
///
///
#ifdef KDEBUG
BOOLEAN kbTest(kBucket_pt bucket);
#else
#define kbTest(bucket)  do {} while (0)
#endif

//////////////////////////////////////////////////////////////////////////
///
/// Bucket definition (should be no one elses business, though)
///

// define this if length of bucket polys are 2, 4, 8, etc
// instead of 4, 16, 64 ... --
// this seems to be less efficient, both, in theory and in practice
// #define BUCKET_TWO_BASE
#ifdef BUCKET_TWO_BASE
#define MAX_BUCKET 28
#else
#define MAX_BUCKET 14 // suitable for polys up to a length of 4^14 = 2^28
#endif

class kBucket
{
public:
#ifdef HAVE_PSEUDO_BUCKETS
  poly p;
  int l;
#else
  poly buckets[MAX_BUCKET + 1];        // polys in bucket
#ifdef HAVE_COEF_BUCKETS
  poly coef[MAX_BUCKET + 1];        // coeff of polys in bucket or NULL : 2..max
#endif
  int  buckets_length[MAX_BUCKET + 1]; // length if i-th poly
  int buckets_used;                    // max number of used bucket
#endif
  ring bucket_ring;
};

#ifndef HAVE_PSEUDO_BUCKETS
static inline void kBucketAdjustBucketsUsed(kBucket_pt bucket)
{
  while ( bucket->buckets_used > 0 &&
          bucket->buckets[bucket->buckets_used] == NULL)
    (bucket->buckets_used)--;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Gets leading monom of bucket, does NOT change Bpoly!!!!!
// Returned monom is READ ONLY, i.e. no manipulations are allowed !!!!
//
inline poly kBucketGetLm(kBucket_pt bucket, p_kBucketSetLm_Proc_Ptr _p_kBucketSetLm)
{
#ifdef   HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
#endif

  poly& lead = bucket->buckets[0];

  if (lead == NULL)
    _p_kBucketSetLm(bucket);

#ifdef  HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
#endif

  return lead;
}

inline poly kBucketGetLm(kBucket_pt bucket)
{
  return kBucketGetLm(bucket, bucket->bucket_ring->p_Procs->p_kBucketSetLm); // TODO: needs ring :(
}

inline poly kBucketExtractLm(kBucket_pt bucket)
{
  poly lm = kBucketGetLm(bucket);
  #ifdef   HAVE_COEF_BUCKETS
  assume(bucket->coef[0]==NULL);
  #endif
  bucket->buckets[0] = NULL;
  bucket->buckets_length[0] = 0;

  return lm;
}

poly kBucketExtractLmOfBucket(kBucket_pt bucket, int i);
void kBucketSimpleContent(kBucket_pt bucket);
BOOLEAN kBucketIsCleared(kBucket_pt bucket);
int ksCheckCoeff(number *a, number *b, const coeffs r);
#endif /* KBUCKETS_H */
