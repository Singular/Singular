/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.cc
 *  Purpose: multiplication of polynomials
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_q.cc,v 1.2 2006-01-13 18:10:05 wienand Exp $
 *******************************************************************/
#include "mod2.h"

/***************************************************************
 *
 * Returns:  p * q,
 * Destroys: if !copy then p, q
 * Assumes: pLength(p) >= 2 pLength(q) >=2
 ***************************************************************/
#include "p_polys.h"
#include "p_Procs.h"
#include "p_Numbers.h"
#include "kbuckets.h"

#include "p_Mult_q.h"

BOOLEAN pqLength(poly p, poly q, int &lp, int &lq, const int min)
{
  int l = 0;

  do
  {
    if (p == NULL)
    {
      lp = l;
      if (l < min)
      {
        if (q != NULL)
          lq = l+1;
        else
          lq = l;
        return FALSE;
      }
      lq = l + pLength(q);
      return TRUE;
    }
    pIter(p);
    if (q == NULL)
    {
      lq = l;
      if (l < min)
      {
        lp = l+1;
        return FALSE;
      }
      lp = l + 1 + pLength(p);
      return TRUE;
    }
    pIter(q);
    l++;
  }
  while (1);
}


static poly _p_Mult_q_Bucket(poly p, const int lp,
                             poly q, const int lq,
                             const int copy, const ring r)
{
  assume(p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  pAssume1(! pHaveCommonMonoms(p, q));
  assume(lp >= 1 && lq >= 1);
  p_Test(p, r);
  p_Test(q, r);

  poly res = pp_Mult_mm(p,q,r);     // holds initially q1*p
  poly qq = pNext(q);               // we iter of this
  poly qn = pp_Mult_mm(qq, p,r);    // holds p1*qi
  poly pp = pNext(p);               // used for Lm(qq)*pp
  poly rr = res;                    // last monom which is surely not NULL
  poly rn = pNext(res);             // pNext(rr)
  number n, n1;

  kBucket_pt bucket = kBucketCreate(r);

  // initialize bucket
  kBucketInit(bucket, pNext(rn), lp - 2);
  pNext(rn) = NULL;

  // now the main loop
  Top:
  if (rn == NULL) goto Smaller;
  p_LmCmpAction(rn, qn, r, goto Equal, goto Greater, goto Smaller);

  Greater:
  // rn > qn, so iter
  rr = rn;
  pNext(rn) = kBucketExtractLm(bucket);
  pIter(rn);
  goto Top;

  // rn < qn, append qn to rr, and compute next Lm(qq)*pp
  Smaller:
  pNext(rr) = qn;
  rr = qn;
  pIter(qn);
  Work: // compute res + Lm(qq)*pp
  if (rn == NULL)
  {
    pNext(rr) = pp_Mult_mm(pp, qq, r);
    kBucketInit(bucket, pNext(pNext(rr)), lp - 2);
    pNext(pNext(rr)) = NULL;
  }
  else
  {
    kBucketSetLm(bucket, rn);
    kBucket_Plus_mm_Mult_pp(bucket, qq, pp, lp - 1);
    pNext(rr) = kBucketExtractLm(bucket);
  }

  pIter(qq);
  if (qq == NULL) goto Finish;
  rn = pNext(rr);
  goto Top;

  Equal:
  n1 = pGetCoeff(rn);
  n = n_Add(n1, pGetCoeff(qn), r);
  n_Delete(&n1, r);
  if (n_IsZero(n, r))
  {
    n_Delete(&n, r);
    p_LmFree(rn, r);
  }
  else
  {
    pSetCoeff0(rn, n);
    rr = rn;
  }
  rn = kBucketExtractLm(bucket);
  n_Delete(&pGetCoeff(qn),r);
  qn = p_LmFreeAndNext(qn, r);
  goto Work;

  Finish:
  assume(rr != NULL && pNext(rr) != NULL);
  pNext(pNext(rr)) = kBucketClear(bucket);
  kBucketDestroy(&bucket);

  if (!copy)
  {
    p_Delete(&p, r);
    p_Delete(&q, r);
  }
  p_Test(res, r);
  return res;
}


static poly _p_Mult_q_Normal(poly p, poly q, const int copy, const ring r)
{
  assume(p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  pAssume1(! pHaveCommonMonoms(p, q));
  p_Test(p, r);
  p_Test(q, r);

  poly res = pp_Mult_mm(p,q,r);     // holds initially q1*p
#ifdef HAVE_RING2TOM
  if (res == NULL) {
    res = p_ISet(1, r);
    p_SetCoeff(res, (number) 0, r);
  }
#endif
  poly qq = pNext(q);               // we iter of this
  poly qn = pp_Mult_mm(qq, p,r);    // holds p1*qi
  poly pp = pNext(p);               // used for Lm(qq)*pp
  poly rr = res;                    // last monom which is surely not NULL
  poly rn = pNext(res);             // pNext(rr)
  number n, n1;

  // now the main loop
  Top:
#ifdef HAVE_RING2TOM
  if (qn == NULL && rn == NULL) goto Work;
  if (qn == NULL) goto Greater;
#endif
  if (rn == NULL) goto Smaller;
  p_LmCmpAction(rn, qn, r, goto Equal, goto Greater, goto Smaller);

  Greater:
  // rn > qn, so iter
  rr = rn;
  pIter(rn);
  goto Top;

  // rn < qn, append qn to rr, and compute next Lm(qq)*pp
  Smaller:
  pNext(rr) = qn;
  rr = qn;
  pIter(qn);

  Work: // compute res + Lm(qq)*pp
  if (rn == NULL)
    pNext(rr) = pp_Mult_mm(pp, qq, r);
  else
  {
    pNext(rr) = p_Plus_mm_Mult_qq(rn, qq, pp, r);
  }

  pIter(qq);
  if (qq == NULL) goto Finish;
  rn = pNext(rr);
  goto Top;

  Equal:
  n1 = pGetCoeff(rn);
  n = n_Add(n1, pGetCoeff(qn), r);
  n_Delete(&n1, r);
  if (n_IsZero(n, r))
  {
    n_Delete(&n, r);
    rn = p_LmFreeAndNext(rn, r);
  }
  else
  {
    pSetCoeff0(rn, n);
    rr = rn;
    pIter(rn);
  }
  n_Delete(&pGetCoeff(qn),r);
  qn = p_LmFreeAndNext(qn, r);
  goto Work;

  Finish:
  if (!copy)
  {
    p_Delete(&p, r);
    p_Delete(&q, r);
  }
#ifdef HAVE_RING2TOM
  if (n_IsZero(p_GetCoeff(res, r), r)) {
    res = p_LmDeleteAndNext(res, r);
  }
#endif
  p_Test(res, r);
  return res;
}


poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  int lp, lq, l;
  poly pt;

  pqLength(p, q, lp, lq, MIN_LENGTH_BUCKET);

  if (lp < lq)
  {
    pt = p;
    p =  q;
    q = pt;
    l = lp;
    lp = lq;
    lq = l;
  }
  if (lq < MIN_LENGTH_BUCKET || TEST_OPT_NOT_BUCKETS)
    return _p_Mult_q_Normal(p, q, copy, r);
  else
  {
    assume(lp == pLength(p));
    assume(lq == pLength(q));
    return _p_Mult_q_Bucket(p, lp, q, lq, copy, r);
  }
}
