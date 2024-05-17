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

static void pqLengthApprox(poly p, poly q, int &lp, int &lq, const int min)
{
  int l = 0;

  do
  {
    if (p == NULL)
    {
      lp=l;
      lq=l+(q!=NULL);
      return;
    }
    if (q == NULL) /* && p!=NULL */
    {
      lp=l+1;
      lq=l;
      return;
    }
    if (l>min) /* && p,q!=NULL */
    {
      lp=l; lq=l;
      return;
    }
    pIter(p);
    pIter(q);
    l++;
  }
  while (1);
}


poly _p_Mult_q_Bucket(poly p, const int lp,
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
poly _p_Mult_q_Normal_ZeroDiv(poly p, poly q, const int copy, const ring r)
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

poly _p_Mult_q_Normal(poly p, poly q, const int copy, const ring r)
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

#define MIN_LENGTH_MAX 81
#define MIN_FLINT_QQ 80
#define MIN_FLINT_Zp 80
#define MIN_FLINT_Z 80

/// Returns:  p * q,
/// Destroys: if !copy then p, q
/// Assumes: pLength(p) >= 2 pLength(q) >=2, !rIsPluralRing(r), nCoeff_is_Domain
poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume(r != NULL);
  int lp=0, lq=0;
  poly pt;

  //pqLengthApprox(p, q, lp, lq, MIN_LENGTH_MAX);
  BOOLEAN pure_polys=(p_GetComp(p,r)==0) && (p_GetComp(q,r)==0);
  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20503
  if (pure_polys)
  {
    pqLengthApprox(p, q, lp, lq, MIN_LENGTH_MAX);
    if (lp < lq)
    {
      int l;
      pt = p;
      p =  q;
      q = pt;
      l = lp;
      lp = lq;
      lq = l;
    }
    if ((lq>MIN_FLINT_QQ) && rField_is_Q(r))
    {
      fmpq_mpoly_ctx_t ctx;
      if (!convSingRFlintR(ctx,r))
      {
        // lq is a lower bound for the length of p and  q
        poly res=Flint_Mult_MP(p,lq,q,lq,ctx,r);
        if (!copy)
        {
          p_Delete(&p,r);
          p_Delete(&q,r);
        }
        return res;
      }
    }
    else if ((lq>MIN_FLINT_Zp) && rField_is_Zp(r))
    {
      nmod_mpoly_ctx_t ctx;
      if (!convSingRFlintR(ctx,r))
      {
        // lq is a lower bound for the length of p and  q
        poly res=Flint_Mult_MP(p,lq,q,lq,ctx,r);
        if (!copy)
        {
          p_Delete(&p,r);
          p_Delete(&q,r);
        }
        return res;
      }
    }
    else if ((lq>MIN_FLINT_Z) && rField_is_Z(r))
    {
      fmpz_mpoly_ctx_t ctx;
      if (!convSingRFlintR(ctx,r))
      {
        // lq is a lower bound for the length of p and  q
        poly res=Flint_Mult_MP(p,lq,q,lq,ctx,r);
        if (!copy)
        {
          p_Delete(&p,r);
          p_Delete(&q,r);
        }
        return res;
      }
    }
  }
  #endif
  #endif
  if (lp==0) pqLengthApprox(p, q, lp, lq, MIN_LENGTH_BUCKET);
  if (lp < lq)
  {
    int l;
    pt = p;
    p =  q;
    q = pt;
    l = lp;
    lp = lq;
    lq = l;
  }
  if (lq < MIN_LENGTH_BUCKET || TEST_OPT_NOT_BUCKETS)
    return _p_Mult_q_Normal(p, q, copy, r);
  #if 0
  else if (pure_polys
  && ((r->cf->extRing==NULL)||(r->cf->extRing->qideal!=NULL))
    /* exclude trans. extensions: may contain rat.funct as cf */
  && (lq >= MIN_LENGTH_FACTORY)
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
  #endif
  else
  {
    lp=pLength(p);
    lq=pLength(q);
    if (lp < lq)
    {
      int l;
      pt = p;
      p =  q;
      q = pt;
      l = lp;
      lp = lq;
      lq = l;
    }
    return _p_Mult_q_Bucket(p, lp, q, lq, copy, r);
  }
}
