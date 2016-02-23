/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    sbuckets.h
 *  Purpose: declaration of routines for sorting and adding up polys using
 *           a bucket sort
 *  Note:    If you need to extract the leading momonial of a bucket,
 *           use kbuckets, instead.
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *******************************************************************/
#ifndef S_BUCKETS_H
#define S_BUCKETS_H

class sBucket; typedef sBucket*           sBucket_pt;
struct  spolyrec; typedef struct spolyrec polyrec; typedef polyrec* poly;
struct ip_sring; typedef struct ip_sring* ring; typedef struct ip_sring const* const_ring;



//////////////////////////////////////////////////////////////////////////
// Creation/Destruction of buckets
//
sBucket_pt    sBucketCreate(ring r);
void          sBucketDestroy(sBucket_pt *bucket);

//////////////////////////////////////////////////////////////////////////
// New API:
//

/// Copy sBucket non-intrusive!!!
sBucket_pt    sBucketCopy(const sBucket_pt bucket);

/// Returns bucket ring
ring sBucketGetRing(const sBucket_pt bucket);

/// Test whether bucket is empty!?
bool sIsEmpty(const sBucket_pt bucket);



/////////////////////////////////////////////////////////////////////////////
// Convertion from/to SBpolys
//

// Converts p into a bucket poly (SBpoly) and destroys p
// Assumes length <= 0 || pLength(p) == length
// void sBucketInit(sBucket_pt bucket, poly p, int length);

// creates and returns new bucket, initializes it with p
// sBucket_pt sBucketInit( poly p, int length, ring r = currRing);

// Converts SBpoly into a poly and clears bucket
// i.e., afterwards SBpoly == 0
// assumes all monomials in bucket are different
void sBucketClearMerge(sBucket_pt bucket, poly *p, int *length);

// Converts SBpoly into a poly and clears bucket
// i.e., afterwards SBpoly == 0
// bucket may contain equal monials
void sBucketClearAdd(sBucket_pt bucket, poly *p, int *length);

// Converts SBpoly into a poly and detroys bucket
inline void sBucketDestroyMerge(sBucket_pt bucket, poly *p, int *length)
{
  sBucketClearMerge(bucket, p, length);
  sBucketDestroy(&bucket);
}

// Converts SBpoly into a poly and detroys bucket
inline void sBucketDestroyAdd(sBucket_pt bucket, poly *p, int *length)
{
  sBucketClearAdd(bucket, p, length);
  sBucketDestroy(&bucket);
}

void sBucketDeleteAndDestroy(sBucket_pt *bucket_pt);

//////////////////////////////////////////////////////////////////////////

/// Merges p into Spoly: assumes Bpoly and p have no common monoms
/// destroys p!
void sBucket_Merge_p(sBucket_pt bucket, poly p, int lp);

/// adds poly p to bucket
/// destroys p!
void sBucket_Add_p(sBucket_pt bucket, poly p, int lp);


//////////////////////////////////////////////////////////////////////////
///
/// Sorts p with bucketSort: assumes all monomials of p are different
///
poly sBucketSortMerge(poly p, const ring r);

//////////////////////////////////////////////////////////////////////////
///
/// Sorts p with bucketSort: p may have equal monomials
///
poly sBucketSortAdd(poly p, const ring r);


#endif // P_BUCKET_SORT
