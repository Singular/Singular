#ifndef KBUCKETS_H
#define KBUCKETS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kbuckets.h,v 1.10 2000-09-12 16:00:58 obachman Exp $ */
#include "structs.h"

/////////////////////////////////////////////////////////////////////////
// configuration
//

// define to not really use the bucket feature
// #define HAVE_PSEUDO_BUCKETS

//////////////////////////////////////////////////////////////////////////
// Creation/Destruction of buckets
//
kBucket_pt kBucketCreate(ring r = currRing);
void kBucketDestroy(kBucket_pt *bucket);


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to Bpolys
//

// Converts p into a bucket poly (Bpoly) and destroys p
// Assumes length <= 0 || pLength(p) == length
//         Monoms of p are from heap
//         Uses heap for intermediate monom allocations
void kBucketInit(kBucket_pt bucket, poly p, int length);

// Converts Bpoly into a poly and clears bucket
// i.e., afterwards Bpoly == 0
void kBucketClear(kBucket_pt bucket, poly *p, int *length);


/////////////////////////////////////////////////////////////////////////////
// Gets leading monom of bucket, does NOT change Bpoly!!!!!
// Returned monom is READ ONLY, i.e. no manipulations are allowed !!!!
//
const poly kBucketGetLm(kBucket_pt bucket);

/////////////////////////////////////////////////////////////////////////////
// Extracts lm of Bpoly, i.e. Bpoly is changed s.t.
// Bpoly == Bpoly - Lm(Bpoly)
//
poly kBucketExtractLm(kBucket_pt bucket);

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


/////////////////////////////////////////////////////////////////////////////
//
// Extract all monomials from bucket with component comp
// Return as a polynomial *p with length *l
// In other words, afterwards
// Bpoly == Bpoly - (poly consisting of all monomials with component comp)
// and components of monomials of *p are all 0

void kBucketTakeOutComp(kBucket_pt bucket,
                        Exponent_t comp,
                        poly *p, int *l);

// Here we only extract such monoms which have component == comp and
// degree == order
// ASSUME: monomial ordering is Order compatible, i.e., if m1, m2 Monoms then
//         m1 >= m2 ==> pGetOrder(m1) >= pGetOrder(m2)
void kBucketDecrOrdTakeOutComp(kBucket_pt bucket,
                               Exponent_t comp, Order_t order,
                               poly *p, int *l);

//////////////////////////////////////////////////////////////////////////
///
/// Multiply Bucket by number ,i.e. Bpoly == n*Bpoly
///
void kBucket_Mult_n(kBucket_pt bucket, number n);


//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly - m*p; where m is a monom
/// Does not destroy p and m
/// assume (*l <= 0 || pLength(p) == *l)
void kBucket_Minus_m_Mult_p(kBucket_pt bucket, poly m, poly p, int *l,
                            poly spNother = NULL);


//////////////////////////////////////////////////////////////////////////
///
/// Bucket definition (should be no one elses business, though)
///


#define MAX_BUCKET 14 // suitable for polys up to a length of 4^14 = 2^28
class kBucket
{
public:
#ifdef HAVE_PSEUDO_BUCKETS
  poly p;
  int l;
#else
  poly buckets[MAX_BUCKET + 1];        // polys in bucket
  int  buckets_length[MAX_BUCKET + 1]; // length if i-th poly
  int buckets_used;                    // max number of used bucket
#endif
  ring bucket_ring;
};

#endif /* KBUCKETS_H */
