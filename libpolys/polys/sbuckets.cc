/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    sbuckets.cc
 *  Purpose: implementation of routines for sorting polys using
 *           a bucket sort
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *******************************************************************/
#include "misc/auxiliary.h"

#include "polys/sbuckets.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

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

STATIC_VAR omBin sBucket_bin = omGetSpecBin(sizeof(sBucket));


//////////////////////////////////////////////////////////////////////////
// New API:
//

/// Returns bucket ring
ring sBucketGetRing(const sBucket_pt bucket)
{ return bucket->bucket_ring; }


bool sIsEmpty(const sBucket_pt bucket)
{
  for(int i = 0; i < (BIT_SIZEOF_LONG - 3); i++)
  {
    assume( i < (BIT_SIZEOF_LONG - 3) );
    assume( pLength(bucket->buckets[i].p) == bucket->buckets[i].length );

    if( bucket->buckets[i].p != NULL )
      return false;

    if( bucket->buckets[i].length != 0 )
      return false;
  }

  return( bucket->max_bucket == 0 );

}


/// Copy sBucket non-intrusive!!!
sBucket_pt    sBucketCopy(const sBucket_pt bucket)
{
  sBucketCanonicalize(bucket);
  const ring r = bucket->bucket_ring;

  sBucket_pt newbucket = sBucketCreate(r);

  newbucket->max_bucket = bucket->max_bucket;

  for(int i = 0; i <= bucket->max_bucket; i++)
  {
    assume( i < (BIT_SIZEOF_LONG - 3) );
    assume( pLength(bucket->buckets[i].p) == bucket->buckets[i].length );

    newbucket->buckets[i].p = p_Copy(bucket->buckets[i].p, r);
    newbucket->buckets[i].length = bucket->buckets[i].length;

    assume( pLength(newbucket->buckets[i].p) == newbucket->buckets[i].length );
  }

  return newbucket;
}

//////////////////////////////////////////////////////////////////////////
// Creation/Destruction of buckets
//
sBucket_pt    sBucketCreate(const ring r)
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
    p_Delete(&(bucket->buckets[i].p), bucket->bucket_ring);
  }
  omFreeBin(bucket, sBucket_bin);
  *bucket_pt = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to SBpolys
//

void sBucket_Merge_m(sBucket_pt bucket, poly p)
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
    assume(SI_LOG2(length) == i);
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

  int i = SI_LOG2(length);

  while (bucket->buckets[i].p != NULL)
  {
    p = p_Merge_q(p, bucket->buckets[i].p, bucket->bucket_ring);
    length += bucket->buckets[i].length;
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    i++;
    assume(SI_LOG2(length) == i);
  }

  bucket->buckets[i].p = p;
  bucket->buckets[i].length = length;
  if (i > bucket->max_bucket) bucket->max_bucket = i;
}

void sBucket_Add_m(sBucket_pt bucket, poly p)
{
  assume(bucket != NULL);
  assume(1 == pLength(p));

  int length = 1;

  int i = 0; //SI_LOG2(length);

  while (bucket->buckets[i].p != NULL)
  {
    int shorter;
    p = bucket->bucket_ring->p_Procs->p_Add_q(p, bucket->buckets[i].p,
                shorter, bucket->bucket_ring);
    length+=bucket->buckets[i].length-shorter;
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    if (p==NULL)
    {
      if (i > bucket->max_bucket) bucket->max_bucket = i;
      return;
    }
    i = SI_LOG2(length);
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
  p_Test(p,bucket->bucket_ring);
  if (length <= 0) length = pLength(p);

  int i = SI_LOG2(length);

  while (bucket->buckets[i].p != NULL)
  {
    int shorter;
    p = bucket->bucket_ring->p_Procs->p_Add_q(p, bucket->buckets[i].p, shorter,
                bucket->bucket_ring);
    length+= bucket->buckets[i].length-shorter;
    bucket->buckets[i].p = NULL;
    bucket->buckets[i].length = 0;
    if (p==NULL)
    {
      if (i > bucket->max_bucket) bucket->max_bucket = i;
      return;
    }
    i = SI_LOG2(length);
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
    assume( bucket->buckets[i].length == 0 );
    i++;
    if (i > bucket->max_bucket) goto done;
  }

  pr = bucket->buckets[i].p;
  lr = bucket->buckets[i].length;

  assume( pr != NULL && (lr > 0) );

  bucket->buckets[i].p = NULL;
  bucket->buckets[i].length = 0;
  i++;

  while (i <= bucket->max_bucket)
  {
    if (bucket->buckets[i].p != NULL)
    {
      assume( bucket->buckets[i].length == pLength(bucket->buckets[i].p) );

      pr = p_Add_q(pr, bucket->buckets[i].p, lr, bucket->buckets[i].length,
                   bucket->bucket_ring);

      bucket->buckets[i].p = NULL;
      bucket->buckets[i].length = 0;
    }

    assume( bucket->buckets[i].p == NULL );
    assume( bucket->buckets[i].length == 0 );
    i++;
  }

done:

  *p = pr;
  *length = lr;

  bucket->max_bucket = 0;

  assume( sIsEmpty(bucket) );
}




/////////////////////////////////////////////////////////////////////////////
// Sorts a poly using BucketSort
//

poly sBucketSortMerge(poly p, const ring r)
{
  if (p == NULL || pNext(p) == NULL) return p;

#ifndef SING_NDEBUG
  int l_in = pLength(p);
#endif
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
  assume(l_dummy == pLength(pn));
#ifndef SING_NDEBUG
  assume(l_in == l_dummy);
#endif
  return pn;
}

/////////////////////////////////////////////////////////////////////////////
// Sorts a poly using BucketSort
//

poly sBucketSortAdd(poly p, const ring r)
{
#ifndef SING_NDEBUG
  int l_in = pLength(p);
#endif

  if (p == NULL || pNext(p) == NULL) return p;

  sBucket_pt bucket = sBucketCreate(r);
  poly pn = pNext(p);

  do
  {
    pNext(p) = NULL;
    sBucket_Add_m(bucket, p);
    p = pn;
    if (p == NULL) break;
    pn = pNext(pn);
  }
  while (1);

  int l_dummy;
  sBucketClearAdd(bucket, &pn, &l_dummy);
  sBucketDestroy(&bucket);

  p_Test(pn, r);
#ifndef SING_NDEBUG
  assume(l_dummy == pLength(pn));
  assume(l_in >= l_dummy);
#endif
  return pn;
}

void sBucketCanonicalize(sBucket_pt bucket)
{
  poly pr = NULL;
  int  lr = 0;
  int i = 0;

  while (bucket->buckets[i].p == NULL)
  {
    assume( bucket->buckets[i].length == 0 );
    i++;
    if (i > bucket->max_bucket) goto done;
  }

  pr = bucket->buckets[i].p;
  lr = bucket->buckets[i].length;

  assume( pr != NULL && (lr > 0) );

  bucket->buckets[i].p = NULL;
  bucket->buckets[i].length = 0;
  i++;

  while (i <= bucket->max_bucket)
  {
    if (bucket->buckets[i].p != NULL)
    {
      p_Test(pr,bucket->bucket_ring);
      assume( bucket->buckets[i].length == pLength(bucket->buckets[i].p) );

      p_Test(bucket->buckets[i].p,bucket->bucket_ring);
      //pr = p_Add_q(pr, bucket->buckets[i].p, lr, bucket->buckets[i].length,
      //             bucket->bucket_ring);
      pr = p_Add_q(pr, bucket->buckets[i].p, bucket->bucket_ring);

      bucket->buckets[i].p = NULL;
      bucket->buckets[i].length = 0;
    }

    assume( bucket->buckets[i].p == NULL );
    assume( bucket->buckets[i].length == 0 );
    i++;
  }

done:
  lr=pLength(pr);
  if (pr!=NULL)
  {
    i = SI_LOG2(lr);
    bucket->buckets[i].p = pr;
    bucket->buckets[i].length = lr;
    bucket->max_bucket = i;
  }
}

poly sBucketPeek(sBucket_pt b)
{
  sBucketCanonicalize(b);
  return b->buckets[b->max_bucket].p;
}

char* sBucketString(sBucket_pt bucket)
{
  return (p_String(sBucketPeek(bucket),sBucketGetRing(bucket)));
}

void sBucketPrint(sBucket_pt bucket)
{
  p_Write0(sBucketPeek(bucket),sBucketGetRing(bucket));
}

