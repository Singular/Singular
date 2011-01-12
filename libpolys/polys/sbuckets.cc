/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    sbuckets.cc
 *  Purpose: implementation of routines for sorting polys using
 *           a bucket sort
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>

#include <kernel/sbuckets.h>
#include <omalloc/omalloc.h>
#include <kernel/ring.h>
#include <kernel/p_Procs.h>
#include <kernel/p_polys.h>



//////////////////////////////////////////////////////////////////////////
// Declarations
//

class sBucketPoly
{
public:
  poly p;
  long length;
};

class sBucket
{
public:
  ring          bucket_ring;
  long          max_bucket;
  sBucketPoly   buckets[BIT_SIZEOF_LONG - 3];
}
;

static omBin sBucket_bin = omGetSpecBin(sizeof(sBucket));


//////////////////////////////////////////////////////////////////////////
// New API:
//

/// Returns bucket ring
const ring sBucketGetRing(const sBucket_pt bucket)
{ return bucket->bucket_ring; }

/// Copy sBucket non-intrusive!!!
sBucket_pt    sBucketCopy(const sBucket_pt bucket)
{
  const ring r = bucket->bucket_ring;

  sBucket_pt newbucket = sBucketCreate(r);

  for(int i = 0; bucket->buckets[i].p != NULL; i++)
  {
    assume( i < (BIT_SIZEOF_LONG - 3) );
    assume( pLength(bucket->buckets[i].p) == bucket->buckets[i].length );

    newbucket->buckets[i].p = p_Copy(bucket->buckets[i].p, r);
    newbucket->buckets[i].length = bucket->buckets[i].length;

    assume( pLength(newbucket->buckets[i].p) == newbucket->buckets[i].length );
  }

  return newbucket;
}

/////////////////////////////////////////////////////////////////////////////
// internal routines
//
// returns floor(log_2(i))
static inline int LOG2(int i)
{
  assume (i > 0);
  int j = 0;

  do
  {
    i = i >> 1;
    if (i == 0) return j;
    j++;
  }
  while (1);

  return j;
}

//////////////////////////////////////////////////////////////////////////
// Creation/Destruction of buckets
//
sBucket_pt    sBucketCreate(ring r)
{
  sBucket_pt bucket = (sBucket_pt) omAlloc0Bin(sBucket_bin);
  bucket->bucket_ring = r;
  return bucket;
}

void          sBucketDestroy(sBucket_pt *bucket)
{
  assume(bucket != NULL);
  omFreeBin(*bucket, sBucket_bin);
  *bucket = NULL;
}

void sBucketDeleteAndDestroy(sBucket_pt *bucket_pt)
{
  sBucket_pt bucket = *bucket_pt;
  int i;
  for (i=0; i<= bucket->max_bucket; i++)
  {

    if (bucket->buckets[i].p != NULL)
    {
      p_Delete(&(bucket->buckets[i].p), bucket->bucket_ring);
    }
  }
  omFreeBin(bucket, sBucket_bin);
  *bucket_pt = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to SBpolys
//

static void sBucket_Merge_m(sBucket_pt bucket, poly p)
{
  assume(p != NULL && pNext(p) == NULL);
  int length = 1;
  int i = 0;

  while (bucket->buckets[i].p != NULL)
  {
    p = p_Merge_q(p, bucket->buckets[i].p, bucket->bucket_ring);
    length += bucket->buckets[i].length;
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    i++;
    assume(LOG2(length) == i);
  }

  bucket->buckets[i].p = p;
  bucket->buckets[i].length = length;
  if (i > bucket->max_bucket) bucket->max_bucket = i;
}

void sBucket_Merge_p(sBucket_pt bucket, poly p, int length)
{
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(p));

  if (p == NULL) return;
  if (length <= 0) length = pLength(p);

  int i = LOG2(length);

  while (bucket->buckets[i].p != NULL)
  {
    p = p_Merge_q(p, bucket->buckets[i].p, bucket->bucket_ring);
    length += bucket->buckets[i].length;
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    i++;
    assume(LOG2(length) == i);
  }

  bucket->buckets[i].p = p;
  bucket->buckets[i].length = length;
  if (i > bucket->max_bucket) bucket->max_bucket = i;
}

void sBucket_Add_p(sBucket_pt bucket, poly p, int length)
{
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(p));

  if (p == NULL) return;
  if (length <= 0) length = pLength(p);

  int i = LOG2(length);

  while (bucket->buckets[i].p != NULL)
  {
    p = p_Add_q(p, bucket->buckets[i].p, length, bucket->buckets[i].length,
                bucket->bucket_ring);
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    if (p==NULL)
    {
      if (i > bucket->max_bucket) bucket->max_bucket = i;
      return;
    }
    i = LOG2(length);
  }

  bucket->buckets[i].p = p;
  bucket->buckets[i].length = length;
  if (i > bucket->max_bucket) bucket->max_bucket = i;
}

// Converts SBpoly into a poly and clears bucket
// i.e., afterwards SBpoly == 0
void sBucketClearMerge(sBucket_pt bucket, poly *p, int *length)
{
  poly pr = NULL;
  int  lr = 0;
  int i = 0;

  while (bucket->buckets[i].p == NULL)
  {
    i++;
    if (i > bucket->max_bucket) goto done;
  }

  pr = bucket->buckets[i].p;
  lr = bucket->buckets[i].length;
  bucket->buckets[i].p = NULL;
  bucket->buckets[i].length = 0;
  i++;

  while (i <= bucket->max_bucket)
  {
    if (bucket->buckets[i].p != NULL)
    {
      pr = p_Merge_q(pr, bucket->buckets[i].p,bucket->bucket_ring);
      lr += bucket->buckets[i].length;
      bucket->buckets[i].p = NULL;
      bucket->buckets[i].length = 0;
    }
    i++;
  }

  done:
  *p = pr;
  *length = lr;
  bucket->max_bucket = 0;
}

// Converts SBpoly into a poly and clears bucket
// i.e., afterwards SBpoly == 0
void sBucketClearAdd(sBucket_pt bucket, poly *p, int *length)
{
  poly pr = NULL;
  int  lr = 0;
  int i = 0;

  while (bucket->buckets[i].p == NULL)
  {
    i++;
    if (i > bucket->max_bucket) goto done;
  }

  pr = bucket->buckets[i].p;
  lr = bucket->buckets[i].length;
  bucket->buckets[i].p = NULL;
  bucket->buckets[i].length = 0;
  i++;

  while (i <= bucket->max_bucket)
  {
    if (bucket->buckets[i].p != NULL)
    {
      pr = p_Add_q(pr, bucket->buckets[i].p, lr, bucket->buckets[i].length,
                   bucket->bucket_ring);
      bucket->buckets[i].p = NULL;
      bucket->buckets[i].length = 0;
    }
    i++;
  }

  done:
  *p = pr;
  *length = lr;
  bucket->max_bucket = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Sorts a poly using BucketSort
//

poly sBucketSortMerge(poly p, ring r)
{
#ifdef HAVE_ASSUME
  int l_in = pLength(p);
#endif

  if (p == NULL || pNext(p) == NULL) return p;

  sBucket_pt bucket = sBucketCreate(r);
  poly pn = pNext(p);

  do
  {
    pNext(p) = NULL;
    sBucket_Merge_m(bucket, p);
    p = pn;
    if (p == NULL) break;
    pn = pNext(pn);
  }
  while (1);

  int l_dummy;
  sBucketClearMerge(bucket, &pn, &l_dummy);
  sBucketDestroy(&bucket);

  p_Test(pn, r);
#ifdef HAVE_ASSUME
  assume(l_dummy == pLength(pn));
  assume(l_in == l_dummy);
#endif
  return pn;
}

/////////////////////////////////////////////////////////////////////////////
// Sorts a poly using BucketSort
//

poly sBucketSortAdd(poly p, ring r)
{
#ifdef HAVE_ASSUME
  int l_in = pLength(p);
#endif

  if (p == NULL || pNext(p) == NULL) return p;

  sBucket_pt bucket = sBucketCreate(r);
  poly pn = pNext(p);

  do
  {
    pNext(p) = NULL;
    sBucket_Add_p(bucket, p, 1);
    p = pn;
    if (p == NULL) break;
    pn = pNext(pn);
  }
  while (1);

  int l_dummy;
  sBucketClearAdd(bucket, &pn, &l_dummy);
  sBucketDestroy(&bucket);

  p_Test(pn, r);
#ifdef HAVE_ASSUME
  assume(l_dummy == pLength(pn));
  assume(l_in >= l_dummy);
#endif
  return pn;
}
