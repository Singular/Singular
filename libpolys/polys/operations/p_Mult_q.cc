/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.cc
 *  Purpose: multiplication of polynomials
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

#include "misc/auxiliary.h"
#include "factory/factory.h"

#include "misc/options.h"

#include "coeffs/numbers.h"
#include "polys/monomials/p_polys.h"
#include "polys/kbuckets.h"
#include "polys/clapsing.h"

#include "polys/templates/p_Procs.h"
#include "polys/templates/p_MemCmp.h"
#include "polys/templates/p_MemAdd.h"
#include "polys/templates/p_MemCopy.h"
#include "polys/flintconv.h"
#include "polys/flint_mpoly.h"

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
  assume(!rField_is_Ring(r) || rField_is_Domain(r));
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
  n = n_Add(n1, pGetCoeff(qn), r->cf);
  n_Delete(&n1, r->cf);
  if (n_IsZero(n, r->cf))
  {
    n_Delete(&n, r->cf);
    p_LmFree(rn, r);
  }
  else
  {
    pSetCoeff0(rn, n);
    rr = rn;
  }
  rn = kBucketExtractLm(bucket);
  n_Delete(&pGetCoeff(qn),r->cf);
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

#ifdef HAVE_RINGS
static poly _p_Mult_q_Normal_ZeroDiv(poly p, poly q, const int copy, const ring r)
{
  assume(p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  pAssume1(! pHaveCommonMonoms(p, q));
  p_Test(p, r);
  p_Test(q, r);

  poly res = pp_Mult_mm(p,q,r);     // holds initially q1*p
  poly qq = pNext(q);               // we iter of this

  while (qq != NULL)
  {
    res = p_Plus_mm_Mult_qq(res, qq, p, r);
    pIter(qq);
  }

  if (!copy)
  {
    p_Delete(&p, r);
    p_Delete(&q, r);
  }

  p_Test(res, r);

  return res;
}
#endif

static poly _p_Mult_q_Normal(poly p, poly q, const int copy, const ring r)
{
  assume(r != NULL);
  assume(p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
#ifdef HAVE_RINGS
  assume(nCoeff_is_Domain(r->cf));
#endif
  pAssume1(! p_HaveCommonMonoms(p, q, r));
  p_Test(p, r);
  p_Test(q, r);

  poly res = pp_Mult_mm(p,q,r);     // holds initially q1*p
  poly qq = pNext(q);               // we iter of this
  poly qn = pp_Mult_mm(qq, p,r);    // holds p1*qi
  poly pp = pNext(p);               // used for Lm(qq)*pp
  poly rr = res;                    // last monom which is surely not NULL
  poly rn = pNext(res);             // pNext(rr)
  number n, n1;

  // now the main loop
  Top:
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
  n = n_Add(n1, pGetCoeff(qn), r->cf);
  n_Delete(&n1, r->cf);
  if (n_IsZero(n, r->cf))
  {
    n_Delete(&n, r->cf);
    rn = p_LmFreeAndNext(rn, r);
  }
  else
  {
    pSetCoeff0(rn, n);
    rr = rn;
    pIter(rn);
  }
  n_Delete(&pGetCoeff(qn),r->cf);
  qn = p_LmFreeAndNext(qn, r);
  goto Work;

  Finish:
  if (!copy)
  {
    p_Delete(&p, r);
    p_Delete(&q, r);
  }
  p_Test(res, r);
  return res;
}


// Use factory if min(pLength(p), pLength(q)) >= MIN_LENGTH_FACTORY (>MIN_LENGTH_BUCKET)
// Not thoroughly tested what is best
#define MIN_LENGTH_FACTORY 60
#define MIN_FLINT_QQ 10
#define MIN_FLINT_Zp 20

/// Returns:  p * q,
/// Destroys: if !copy then p, q
/// Assumes: pLength(p) >= 2 pLength(q) >=2, !rIsPluralRing(r)
poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume(r != NULL);
#ifdef HAVE_RINGS
  if (!nCoeff_is_Domain(r->cf))
    return _p_Mult_q_Normal_ZeroDiv(p, q, copy, r);
#endif
  int lp, lq, l;
  poly pt;

  pqLength(p, q, lp, lq, MIN_LENGTH_FACTORY);

  if (lp < lq)
  {
    pt = p;
    p =  q;
    q = pt;
    l = lp;
    lp = lq;
    lq = l;
  }
  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20503
  if (lq>MIN_FLINT_QQ)
  {
    fmpq_mpoly_ctx_t ctx;
    if ((p_GetComp(p,r)==0) && (p_GetComp(q,r)==0)
    && rField_is_Q(r) && !convSingRFlintR(ctx,r))
    {
      lp=pLength(p);
      //printf("mul in flint\n");
      return Flint_Mult_MP(p,lp,q,lq,ctx,r);
    }
  }
  if (lq>MIN_FLINT_Zp)
  {
    nmod_mpoly_ctx_t ctx;
    if ((p_GetComp(p,r)==0) && (p_GetComp(q,r)==0)
    && rField_is_Zp(r) && !convSingRFlintR(ctx,r))
    {
      lp=pLength(p);
      //printf("mul in flint\n");
      return Flint_Mult_MP(p,lp,q,lq,ctx,r);
    }
  }
  #endif
  #endif
  if (lq < MIN_LENGTH_BUCKET || TEST_OPT_NOT_BUCKETS)
    return _p_Mult_q_Normal(p, q, copy, r);
  else if ((lq >= MIN_LENGTH_FACTORY)
  && (r->cf->convSingNFactoryN!=ndConvSingNFactoryN))
  {
    poly h=singclap_pmult(p,q,r);
    if (!copy)
    {
      p_Delete(&p,r);
      p_Delete(&q,r);
    }
    return h;
  }
  else
  {
    lp=pLength(p);
    assume(lq == pLength(q));
    return _p_Mult_q_Bucket(p, lp, q, lq, copy, r);
  }
}
