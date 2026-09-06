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

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20503
/*
 * Estimate collisions among the products of the first few support points.
 * Equality is checked on exponent vectors rather than hashes: the sample is
 * deliberately small, and a selector should not depend on hash collisions.
 */
static int p_Mult_q_SampleUniqueSums(poly p, poly q, const ring r)
{
  poly sp[8], sq[8];
  int a = 0, b = 0;
  for (poly s = p; (s != NULL) && (a < 8); pIter(s)) sp[a++] = s;
  for (poly s = q; (s != NULL) && (b < 8); pIter(s)) sq[b++] = s;

  int pi[64], pj[64], unique = 0;
  for (int i = 0; i < a; i++)
  {
    for (int j = 0; j < b; j++)
    {
      BOOLEAN seen = FALSE;
      for (int k = 0; (k < unique) && !seen; k++)
      {
        seen = TRUE;
        for (int v = 1; v <= r->N; v++)
        {
          if ((unsigned long long)p_GetExp(sp[i], v, r)
                + (unsigned long long)p_GetExp(sq[j], v, r)
              != (unsigned long long)p_GetExp(sp[pi[k]], v, r)
                 + (unsigned long long)p_GetExp(sq[pj[k]], v, r))
          {
            seen = FALSE;
            break;
          }
        }
      }
      if (!seen)
      {
        pi[unique] = i;
        pj[unique] = j;
        unique++;
      }
    }
  }
  return unique;
}

#if (__FLINT_RELEASE >= 30400) && (__FLINT_RELEASE < 30500)
static unsigned int p_Mult_q_Bits(unsigned long value)
{
  unsigned int bits = 0;
  do
  {
    bits++;
    value >>= 1;
  }
  while (value != 0);
  return bits;
}

/*
 * Predict the conservative part of FLINT 3.4's DEG-array dispatch.  This
 * special case matters above four variables, where conversion is otherwise
 * too expensive to justify FLINT.  Returning FALSE only delays the switch to
 * FLINT; it does not affect the result.
 */
static BOOLEAN p_Mult_q_PredictFlintArray(poly p, poly q, const int lp,
                                          const int lq, const ring r)
{
  if (!rOrd_is_dp(r) || (r->N < 2) || (r->N > 7)
      || (lq < 20) || (lp < 50))
    return FALSE;

  unsigned long maxp[8] = { 0 }, maxq[8] = { 0 };
  unsigned long total_degree_p = 0, total_degree_q = 0;
  for (poly s = p; s != NULL; pIter(s))
  {
    const unsigned long degree = (unsigned long)p_Totaldegree(s, r);
    if (degree > total_degree_p) total_degree_p = degree;
    for (int v = 1; v <= r->N; v++)
    {
      const unsigned long exponent = (unsigned long)p_GetExp(s, v, r);
      if (exponent > maxp[v]) maxp[v] = exponent;
    }
  }
  for (poly s = q; s != NULL; pIter(s))
  {
    const unsigned long degree = (unsigned long)p_Totaldegree(s, r);
    if (degree > total_degree_q) total_degree_q = degree;
    for (int v = 1; v <= r->N; v++)
    {
      const unsigned long exponent = (unsigned long)p_GetExp(s, v, r);
      if (exponent > maxq[v]) maxq[v] = exponent;
    }
  }

  unsigned int bits_p = p_Mult_q_Bits(total_degree_p) + 1;
  unsigned int bits_q = p_Mult_q_Bits(total_degree_q) + 1;
  for (int v = 1; v <= r->N; v++)
  {
    unsigned int bits = p_Mult_q_Bits(maxp[v]) + 1;
    if (bits > bits_p) bits_p = bits;
    bits = p_Mult_q_Bits(maxq[v]) + 1;
    if (bits > bits_q) bits_q = bits;
  }
  if (bits_p < 8) bits_p = 8;
  if (bits_q < 8) bits_q = 8;
  if (((unsigned int)(r->N + 1) * bits_p > (unsigned int)FLINT_BITS)
      || ((unsigned int)(r->N + 1) * bits_q > (unsigned int)FLINT_BITS))
    return FALSE;

  unsigned long long array_volume = 1;
  const unsigned long long twice_degree =
    2ULL * (unsigned long long)total_degree_p;
  if (twice_degree == 0) return FALSE;
  for (int v = 0; v < r->N; v++)
  {
    if (array_volume > ~0ULL / twice_degree) return FALSE;
    array_volume *= twice_degree;
  }
  for (int v = 1; v <= r->N; v++)
    array_volume /= (unsigned long long)v;

  const unsigned long long pair_count =
    (unsigned long long)lp * (unsigned long long)lq;
  if ((array_volume > 5000000ULL)
      || (array_volume / (unsigned long long)lp
          / (unsigned long long)lq >= 10ULL))
    return FALSE;

  unsigned long long box_volume = 1;
  for (int v = 1; v <= r->N; v++)
  {
    const unsigned long long width =
      (unsigned long long)maxp[v] + (unsigned long long)maxq[v] + 1ULL;
    if (box_volume > ~0ULL / width)
    {
      box_volume = ~0ULL;
      break;
    }
    box_volume *= width;
  }

  // FLINT tries dense multiplication first.  A predicted dense success is not
  // used as an array override here.
  if (box_volume < pair_count / 128ULL) return FALSE;
  return TRUE;
}
#else
static BOOLEAN p_Mult_q_PredictFlintArray(poly, poly, const int, const int,
                                          const ring)
{
  return FALSE;
}
#endif
#endif
#endif


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
#define MIN_FLINT_QQ 60
#define MIN_FLINT_Zp 80
#define MIN_FLINT_Z 60

/// Returns:  p * q,
/// Destroys: if !copy then p, q
/// Assumes: pLength(p) >= 2 pLength(q) >=2, !rIsPluralRing(r), nCoeff_is_Domain
poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume(r != NULL);
  int lp=0, lq=0;
  poly pt;

  BOOLEAN pure_polys=(p_GetComp(p,r)==0) && (p_GetComp(q,r)==0);
  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20503
  // Previous fixed-length selector conditions:
  // if ((lq>MIN_FLINT_QQ) && rField_is_Q(r))
  // else if ((lq>MIN_FLINT_Zp) && rField_is_Zp(r))
  // else if ((lq>MIN_FLINT_Z) && rField_is_Z(r))
  if (pure_polys && rField_is_Zp(r))
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
    if (lq>MIN_FLINT_Zp)
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
  }
  else if (pure_polys && (rField_is_Q(r) || rField_is_Z(r)))
  {
    // No Q decision can use FLINT below 20 terms; the corresponding bound for
    // Z is 8.  Avoid all support analysis for smaller products.
    const int guard = rField_is_Q(r) ? 20 : 8;
    const BOOLEAN both_long = pqLength(p, q, lp, lq, guard);
    if (lp < lq)
    {
      int length = lp;
      lp = lq;
      lq = length;
      pt = p;
      p = q;
      q = pt;
    }
    if (both_long)
    {
      // With five or six variables FLINT conversion is still cheap enough for
      // the former long-input cutoff.  Above that range it can dominate even
      // when both inputs are long.
      const int former_cutoff = rField_is_Q(r) ? MIN_FLINT_QQ : MIN_FLINT_Z;
      BOOLEAN use_flint = (r->N >= 5) && (r->N <= 6)
                          && (lq > former_cutoff);

      if (r->N <= 4)
      {
        /*
         * Let P=lp*lq.  A sum of supports has between lp+lq-1 and P
         * elements.  Normalize the number of distinct sums in an 8-by-8
         * sample between the same bounds and extrapolate it to an estimated
         * output length.  Repeated sums make Singular merge many of the P
         * partial products, whereas FLINT can exploit their denser structure.
         */
        const unsigned long long pair_count =
          (unsigned long long)lp * (unsigned long long)lq;
        const int sample_p = (lp < 8) ? lp : 8;
        const int sample_q = (lq < 8) ? lq : 8;
        const int unique = p_Mult_q_SampleUniqueSums(p, q, r);
        const unsigned long long lower_bound =
          (unsigned long long)lp + (unsigned long long)lq - 1ULL;
        unsigned long long estimated_length = lower_bound;
        const int denominator =
          sample_p * sample_q - (sample_p + sample_q - 1);
        if (denominator > 0)
        {
          int numerator = unique - (sample_p + sample_q - 1);
          if (numerator < 0) numerator = 0;
          if (numerator > denominator) numerator = denominator;
          const unsigned long long difference = pair_count - lower_bound;
          estimated_length +=
            (difference / (unsigned long long)denominator)
              * (unsigned long long)numerator
            + ((difference % (unsigned long long)denominator)
                * (unsigned long long)numerator
               + (unsigned long long)denominator / 2ULL)
              / (unsigned long long)denominator;
        }

        // 20*pair_count > 21*estimated_length, written without overflow.
        const BOOLEAN collision_dense =
          pair_count - estimated_length > estimated_length / 20ULL;
        if (rField_is_Q(r))
          use_flint = (lq >= 25) && collision_dense;
        else
          use_flint = (lq >= 8) && collision_dense;
      }

      // At seven variables use only FLINT's narrow array case; from eight
      // variables onward conversion dominates the tested sparse products.
      if (!use_flint && (r->N <= 7))
        use_flint = p_Mult_q_PredictFlintArray(p, q, lp, lq, r);

      if (use_flint && rField_is_Q(r))
      {
        fmpq_mpoly_ctx_t ctx;
        if (!convSingRFlintR(ctx,r))
        {
          poly res = Flint_Mult_MP(p, lp, q, lq, ctx, r);
          if (!copy)
          {
            p_Delete(&p,r);
            p_Delete(&q,r);
          }
          return res;
        }
      }
      else if (use_flint)
      {
        fmpz_mpoly_ctx_t ctx;
        if (!convSingRFlintR(ctx,r))
        {
          poly res = Flint_Mult_MP(p, lp, q, lq, ctx, r);
          if (!copy)
          {
            p_Delete(&p,r);
            p_Delete(&q,r);
          }
          return res;
        }
      }
    }
  }
  else if (pure_polys)
    pqLengthApprox(p, q, lp, lq, MIN_LENGTH_MAX);
  #endif
  #endif
  if (lp==0)
    pqLengthApprox(p, q, lp, lq, MIN_LENGTH_BUCKET);
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
