//! \file tgb.cc
//       multiple rings
//       shorten_tails und dessen Aufrufe pruefen wlength!!!
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: slimgb and F4 implementation
*/
//#include <vector>
//using namespace std;

///@TODO: delay nur auf Sugarvergroesserung
///@TODO: grade aus ecartS, setze dazu strat->honey; und nutze p.ecart
///@TODO: no tail reductions in syz comp
#include "kernel/mod2.h"

#include "kernel/GBEngine/tgb.h"
#include "kernel/GBEngine/tgb_internal.h"
#include "kernel/GBEngine/tgbgauss.h"

#include "misc/options.h"
#include "kernel/digitech.h"
#include "polys/nc/nc.h"
#include "polys/nc/sca.h"
#include "polys/prCopy.h"

#include "coeffs/longrat.h" // nlQlogSize

#include <stdlib.h>
#include <stdio.h>
#include <queue>

#define BUCKETS_FOR_NORO_RED 1
#define SR_HDL(A) ((long)(A))
static const int bundle_size = 100;
static const int bundle_size_noro = 10000;
static const int delay_factor = 3;
#define ADD_LATER_SIZE 500
#if 1
STATIC_VAR omBin lm_bin = NULL;
static void add_to_reductors(slimgb_alg* c, poly h, int len, int ecart, BOOLEAN simplified=FALSE);
static void multi_reduction(red_object* los, int & losl, slimgb_alg* c);
static void multi_reduce_step(find_erg & erg, red_object* r, slimgb_alg* c);
static BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, slimgb_alg* c);
static poly gcd_of_terms(poly p, ring r);
static int tgb_pair_better_gen(const void* ap,const void* bp);
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, slimgb_alg* c=NULL);
static BOOLEAN state_is(calc_state state, const int & i, const int & j, slimgb_alg* c);
static void super_clean_top_of_pair_list(slimgb_alg* c);
static int simple_posInS (kStrategy strat, poly p,int len, wlen_type wlen);
static int* make_connections(int from, int to, poly bound, slimgb_alg* c);
static BOOLEAN has_t_rep(const int & arg_i, const int & arg_j, slimgb_alg* state);
static void shorten_tails(slimgb_alg* c, poly monom);
static poly redNF2 (poly h,slimgb_alg* c , int &len, number&  m,int n=0);
static poly redNFTail (poly h,const int sl,kStrategy strat, int len);
static int bucket_guess(kBucket* bucket);

static void simplify_poly (poly p, ring r)
{
  assume (r == currRing);
  if(TEST_OPT_INTSTRATEGY)
  {
    p_Cleardenom (p, r);
    //includes p_Content(p,r);
  }
  else
    pNorm (p);
}

//static const BOOLEAN up_to_radical=TRUE;

int slim_nsize (number n, ring r)
{
  if(rField_is_Zp (r))
  {
    return 1;
  }
  if(rField_is_Q (r))
  {
    return nlQlogSize (n, r->cf);
  }
  else
  {
    return n_Size (n, r->cf);
  }
}

static BOOLEAN monomial_root (poly m, ring r)
{
  BOOLEAN changed = FALSE;
  int i;
  for(i = 1; i <= rVar (r); i++)
  {
    int e = p_GetExp (m, i, r);
    if(e > 1)
    {
      p_SetExp (m, i, 1, r);
      changed = TRUE;
    }
  }
  if(changed)
  {
    p_Setm (m, r);
  }
  return changed;
}

static BOOLEAN polynomial_root (poly h, ring r)
{
  poly got = gcd_of_terms (h, r);
  BOOLEAN changed = FALSE;
  if((got != NULL) && (TEST_V_UPTORADICAL))
  {
    poly copy = p_Copy (got, r);
    //p_wrp(got,c->r);
    changed = monomial_root (got, r);
    if(changed)
    {
      poly div_by = pMDivide (copy, got);
      poly iter = h;
      while(iter)
      {
        pExpVectorSub (iter, div_by);
        pIter (iter);
      }
      p_Delete (&div_by, r);
      if(TEST_OPT_PROT)
        PrintS ("U");
    }
    p_Delete (&copy, r);
  }
  p_Delete (&got, r);
  return changed;
}

static inline poly p_Init_Special (const ring r)
{
  return p_Init (r, lm_bin);
}

static inline poly pOne_Special (const ring r = currRing)
{
  poly rc = p_Init_Special (r);
  pSetCoeff0 (rc, n_Init (1, r->cf));
  return rc;
}

// zum Initialiseren: in t_rep_gb plazieren:

#endif
#define LEN_VAR3
#define degbound(p) assume(pTotaldegree(p)<10)
//#define inDebug(p) assume((debug_Ideal==NULL)||(kNF(debug_Ideal,NULL,p,0,0)==0))

//die meisten Varianten stossen sich an coef_buckets

#ifdef LEN_VAR1
// erste Variante: Laenge: Anzahl der Monome
static inline int pSLength (poly p, int l)
{
  return l;
}

static inline int kSBucketLength (kBucket * bucket, poly lm)
{
  return bucket_guess (bucket);
}
#endif

#ifdef LEN_VAR2
// 2. Variante: Laenge: Platz fuer die Koeff.
int pSLength (poly p, int l)
{
  int s = 0;
  while(p != NULL)
  {
    s += nSize (pGetCoeff (p));
    pIter (p);
  }
  return s;
}

int kSBucketLength (kBucket * b, poly lm)
{
  int s = 0;
  int i;
  for(i = MAX_BUCKET; i >= 0; i--)
  {
    s += pSLength (b->buckets[i], 0);
  }
  return s;
}
#endif

#ifdef LEN_VAR3
static inline wlen_type pSLength (poly p, int l)
{
  wlen_type c;
  number coef = pGetCoeff (p);
  if(rField_is_Q (currRing))
  {
    c = nlQlogSize (coef, currRing->cf);
  }
  else
    c = nSize (coef);
  if(!(TEST_V_COEFSTRAT))
  {
    return (wlen_type) c *(wlen_type) l /*pLength(p) */ ;
  }
  else
  {
    wlen_type res = l;
    res *= c;
    res *= c;
    return res;
  }
}

//! TODO CoefBuckets bercksichtigen
wlen_type kSBucketLength (kBucket * b, poly lm = NULL)
{
  int s = 0;
  wlen_type c;
  number coef;
  if(lm == NULL)
    coef = pGetCoeff (kBucketGetLm (b));
  //c=nSize(pGetCoeff(kBucketGetLm(b)));
  else
    coef = pGetCoeff (lm);
  //c=nSize(pGetCoeff(lm));
  if(rField_is_Q (currRing))
  {
    c = nlQlogSize (coef, currRing->cf);
  }
  else
    c = nSize (coef);

  int i;
  for(i = b->buckets_used; i >= 0; i--)
  {
    assume ((b->buckets_length[i] == 0) || (b->buckets[i] != NULL));
    s += b->buckets_length[i] /*pLength(b->buckets[i]) */ ;
  }
#ifdef HAVE_COEF_BUCKETS
  assume (b->buckets[0] == kBucketGetLm (b));
  if(b->coef[0] != NULL)
  {
    if(rField_is_Q (currRing))
    {
      int modifier = nlQlogSize (pGetCoeff (b->coef[0]), currRing->cf);
      c += modifier;
    }
    else
    {
      int modifier = nSize (pGetCoeff (b->coef[0]));
      c *= modifier;
    }
  }
#endif
  if(!(TEST_V_COEFSTRAT))
  {
    return s * c;
  }
  else
  {
    wlen_type res = s;
    res *= c;
    res *= c;
    return res;
  }
}
#endif
#ifdef LEN_VAR5
static inline wlen_type pSLength (poly p, int l)
{
  int c;
  number coef = pGetCoeff (p);
  if(rField_is_Q (currRing))
  {
    c = nlQlogSize (coef, currRing->cf);
  }
  else
    c = nSize (coef);
  wlen_type erg = l;
  erg *= c;
  erg *= c;
  //PrintS("lenvar 5");
  assume (erg >= 0);
  return erg; /*pLength(p) */ ;
}

//! TODO CoefBuckets beruecksichtigen
wlen_type kSBucketLength (kBucket * b, poly lm = NULL)
{
  wlen_type s = 0;
  int c;
  number coef;
  if(lm == NULL)
    coef = pGetCoeff (kBucketGetLm (b));
  //c=nSize(pGetCoeff(kBucketGetLm(b)));
  else
    coef = pGetCoeff (lm);
  //c=nSize(pGetCoeff(lm));
  if(rField_is_Q (currRing))
  {
    c = nlQlogSize (coef, currRing->cf);
  }
  else
    c = nSize (coef);

  int i;
  for(i = b->buckets_used; i >= 0; i--)
  {
    assume ((b->buckets_length[i] == 0) || (b->buckets[i] != NULL));
    s += b->buckets_length[i] /*pLength(b->buckets[i]) */ ;
  }
#ifdef HAVE_COEF_BUCKETS
  assume (b->buckets[0] == kBucketGetLm (b));
  if(b->coef[0] != NULL)
  {
    if(rField_is_Q (currRing))
    {
      int modifier = nlQlogSize (pGetCoeff (b->coef[0]), currRing->cf);
      c += modifier;
    }
    else
    {
      int modifier = nSize (pGetCoeff (b->coef[0]));
      c *= modifier;
    }
  }
#endif
  wlen_type erg = s;
  erg *= c;
  erg *= c;
  return erg;
}
#endif

#ifdef LEN_VAR4
// 4.Variante: Laenge: Platz fuer Leitk * (1+Platz fuer andere Koeff.)
int pSLength (poly p, int l)
{
  int s = 1;
  int c = nSize (pGetCoeff (p));
  pIter (p);
  while(p != NULL)
  {
    s += nSize (pGetCoeff (p));
    pIter (p);
  }
  return s * c;
}

int kSBucketLength (kBucket * b)
{
  int s = 1;
  int c = nSize (pGetCoeff (kBucketGetLm (b)));
  int i;
  for(i = MAX_BUCKET; i > 0; i--)
  {
    if(b->buckets[i] == NULL)
      continue;
    s += pSLength (b->buckets[i], 0);
  }
  return s * c;
}
#endif
//BUG/TODO this stuff will fail on internal Schreyer orderings
static BOOLEAN elength_is_normal_length (poly p, slimgb_alg * c)
{
  ring r = c->r;
  if(p_GetComp (p, r) != 0)
    return FALSE;
  if(c->lastDpBlockStart <= (currRing->N))
  {
    int i;
    for(i = 1; i < c->lastDpBlockStart; i++)
    {
      if(p_GetExp (p, i, r) != 0)
      {
        break;
      }
    }
    if(i >= c->lastDpBlockStart)
    {
      //wrp(p);
      //PrintS("\n");
      return TRUE;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
}

static BOOLEAN lies_in_last_dp_block (poly p, slimgb_alg * c)
{
  ring r = c->r;
  if(p_GetComp (p, r) != 0)
    return FALSE;
  if(c->lastDpBlockStart <= (currRing->N))
  {
    int i;
    for(i = 1; i < c->lastDpBlockStart; i++)
    {
      if(p_GetExp (p, i, r) != 0)
      {
        break;
      }
    }
    if(i >= c->lastDpBlockStart)
    {
      //wrp(p);
      //PrintS("\n");
      return TRUE;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
}

static int get_last_dp_block_start (ring r)
{
  //ring r=c->r;
  int last_block;

  if(rRing_has_CompLastBlock (r))
  {
    last_block = rBlocks (r) - 3;
  }
  else
  {
    last_block = rBlocks (r) - 2;
  }
  assume (last_block >= 0);
  if(r->order[last_block] == ringorder_dp)
    return r->block0[last_block];
  return (currRing->N) + 1;
}

static wlen_type do_pELength (poly p, slimgb_alg * c, int dlm = -1)
{
  if(p == NULL)
    return 0;
  wlen_type s = 0;
  poly pi = p;
  if(dlm < 0)
  {
    dlm = c->pTotaldegree (p);
    s = 1;
    pi = p->next;
  }

  while(pi)
  {
    int d = c->pTotaldegree (pi);
    if(d > dlm)
      s += 1 + d - dlm;
    else
      ++s;
    pi = pi->next;
  }
  return s;
}

wlen_type kEBucketLength (kBucket * b, poly lm, slimgb_alg * ca)
{
  wlen_type s = 0;
  if(lm == NULL)
  {
    lm = kBucketGetLm (b);
  }
  if(lm == NULL)
    return 0;
  if(elength_is_normal_length (lm, ca))
  {
    return bucket_guess (b);
  }
  int d = ca->pTotaldegree (lm);
#if 0
  assume (sugar >= d);
  s = 1 + (bucket_guess (b) - 1) * (sugar - d + 1);
  return s;
#else

  //int d=pTotaldegree(lm,ca->r);
  int i;
  for(i = b->buckets_used; i >= 0; i--)
  {
    if(b->buckets[i] == NULL)
      continue;

    if((ca->pTotaldegree (b->buckets[i]) <= d)
       && (elength_is_normal_length (b->buckets[i], ca)))
    {
      s += b->buckets_length[i];
    }
    else
    {
      s += do_pELength (b->buckets[i], ca, d);
    }
  }
  return s;
#endif
}

static inline int pELength (poly p, slimgb_alg * c, int l)
{
  if(p == NULL)
    return 0;
  if((l > 0) && (elength_is_normal_length (p, c)))
    return l;
  return do_pELength (p, c);
}

static inline wlen_type pQuality (poly p, slimgb_alg * c, int l = -1)
{
  if(l < 0)
    l = pLength (p);
  if(c->isDifficultField)
  {
    if(c->eliminationProblem)
    {
      wlen_type cs;
      number coef = pGetCoeff (p);
      if(rField_is_Q (currRing))
      {
        cs = nlQlogSize (coef, currRing->cf);
      }
      else
        cs = nSize (coef);
      wlen_type erg = cs;
      if(TEST_V_COEFSTRAT)
        erg *= cs;
      //erg*=cs;//for quadratic
      erg *= pELength (p, c, l);
      //FIXME: not quadratic coeff size
      //return cs*pELength(p,c,l);
      return erg;
    }
    //PrintS("I am here");
    wlen_type r = pSLength (p, l);
    assume (r >= 0);
    return r;
  }
  if(c->eliminationProblem)
    return pELength (p, c, l);
  return l;
}

wlen_type red_object::guess_quality (slimgb_alg * c)
{
  //works at the moment only for lenvar 1, because in different
  //case, you have to look on coefs
  wlen_type s = 0;
  if(c->isDifficultField)
  {
    //s=kSBucketLength(bucket,this->p);
    if(c->eliminationProblem)
    {
      wlen_type cs;
      number coef;

      coef = pGetCoeff (kBucketGetLm (bucket));
      //c=nSize(pGetCoeff(kBucketGetLm(b)));

      //c=nSize(pGetCoeff(lm));
      if(rField_is_Q (currRing))
      {
        cs = nlQlogSize (coef, currRing->cf);
      }
      else
        cs = nSize (coef);
#ifdef HAVE_COEF_BUCKETS
      if(bucket->coef[0] != NULL)
      {
        if(rField_is_Q (currRing))
        {
          int modifier = nlQlogSize (pGetCoeff (bucket->coef[0]), currRing->cf);
          cs += modifier;
        }
        else
        {
          int modifier = nSize (pGetCoeff (bucket->coef[0]));
          cs *= modifier;
        }
      }
#endif
      //FIXME:not quadratic
      wlen_type erg = kEBucketLength (this->bucket, this->p, c);
      //erg*=cs;//for quadratic
      erg *= cs;
      if(TEST_V_COEFSTRAT)
        erg *= cs;
      //return cs*kEBucketLength(this->bucket,this->p,c);
      return erg;
    }
    s = kSBucketLength (bucket, NULL);
  }
  else
  {
    if(c->eliminationProblem)
      //if (false)
      s = kEBucketLength (this->bucket, this->p, c);
    else
      s = bucket_guess (bucket);
  }
  return s;
}

#if 0                           //currently unused
static void finalize_reduction_step (reduction_step * r)
{
  delete r;
}
#endif
#if 0                           //currently unused
static int LObject_better_gen (const void *ap, const void *bp)
{
  LObject *a = *(LObject **) ap;
  LObject *b = *(LObject **) bp;
  return (pLmCmp (a->p, b->p));
}
#endif
static int red_object_better_gen (const void *ap, const void *bp)
{
  return (pLmCmp (((red_object *) ap)->p, ((red_object *) bp)->p));
}

#if 0                           //currently unused
static int pLmCmp_func_inverted (const void *ap1, const void *ap2)
{
  poly p1, p2;
  p1 = *((poly *) ap1);
  p2 = *((poly *) ap2);
  return -pLmCmp (p1, p2);
}
#endif

int tgb_pair_better_gen2 (const void *ap, const void *bp)
{
  return (-tgb_pair_better_gen (ap, bp));
}

int kFindDivisibleByInS_easy (kStrategy strat, const red_object & obj)
{
  poly p = obj.p;
  if ((strat->syzComp>0) && (pGetComp(p)>strat->syzComp)) return -1;
  long not_sev = ~obj.sev;
  for(int i = 0; i <= strat->sl; i++)
  {
    if(pLmShortDivisibleBy (strat->S[i], strat->sevS[i], p, not_sev))
      return i;
  }
  return -1;
}

int kFindDivisibleByInS_easy (kStrategy strat, poly p, long sev)
{
  if ((strat->syzComp>0) && (pGetComp(p)>strat->syzComp)) return -1;
  long not_sev = ~sev;
  for(int i = 0; i <= strat->sl; i++)
  {
    if(pLmShortDivisibleBy (strat->S[i], strat->sevS[i], p, not_sev))
      return i;
  }
  return -1;
}

static int
posInPairs (sorted_pair_node ** p, int pn, sorted_pair_node * qe,
            slimgb_alg * c, int an = 0)
{
  if(pn == 0)
    return 0;

  int length = pn - 1;
  int i;
  //int an = 0;
  int en = length;

  if(pair_better (qe, p[en], c))
    return length + 1;

  while(1)
  {
    //if (an >= en-1)
    if(en - 1 <= an)
    {
      if(pair_better (p[an], qe, c))
        return an;
      return en;
    }
    i = (an + en) / 2;
    if(pair_better (p[i], qe, c))
      en = i;
    else
      an = i;
  }
}

static BOOLEAN ascending (int *i, int top)
{
  if(top < 1)
    return TRUE;
  if(i[top] < i[top - 1])
    return FALSE;
  return ascending (i, top - 1);
}

sorted_pair_node **spn_merge (sorted_pair_node ** p, int pn,
                              sorted_pair_node ** q, int qn, slimgb_alg * c)
{
  int i;
  int *a = (int *) omalloc (qn * sizeof (int));
//   int mc;
//   PrintS("Debug\n");
//   for(mc=0;mc<qn;mc++)
// {
//     wrp(q[mc]->lcm_of_lm);
//     PrintS("\n");
// }
//    PrintS("Debug they are in\n");
//   for(mc=0;mc<pn;mc++)
// {
//     wrp(p[mc]->lcm_of_lm);
//     PrintS("\n");
// }
  int lastpos = 0;
  for(i = 0; i < qn; i++)
  {
    lastpos = posInPairs (p, pn, q[i], c, si_max (lastpos - 1, 0));
    //   cout<<lastpos<<"\n";
    a[i] = lastpos;
  }
  if((pn + qn) > c->max_pairs)
  {
    p =
      (sorted_pair_node **) omreallocSize (p,
                                    c->max_pairs *sizeof (sorted_pair_node *),
                                    2 * (pn + qn) * sizeof (sorted_pair_node *));
    c->max_pairs = 2 * (pn + qn);
  }
  for(i = qn - 1; i >= 0; i--)
  {
    size_t size;
    if(qn - 1 > i)
      size = (a[i + 1] - a[i]) * sizeof (sorted_pair_node *);
    else
      size = (pn - a[i]) * sizeof (sorted_pair_node *); //as indices begin with 0
    memmove (p + a[i] + (1 + i), p + a[i], size);
    p[a[i] + i] = q[i];
  }
  omfree (a);
  return p;
}

static BOOLEAN
trivial_syzygie (int pos1, int pos2, poly bound, slimgb_alg * c)
{
  poly p1 = c->S->m[pos1];
  poly p2 = c->S->m[pos2];

  if(pGetComp (p1) > 0 || pGetComp (p2) > 0)
    return FALSE;
  int i = 1;
  poly m = NULL;
  poly gcd1 = c->gcd_of_terms[pos1];
  poly gcd2 = c->gcd_of_terms[pos2];

  if((gcd1 != NULL) && (gcd2 != NULL))
  {
    gcd1->next = gcd2;          //may ordered incorrect
    m = gcd_of_terms (gcd1, c->r);
    gcd1->next = NULL;
  }
  if(m == NULL)
  {
    loop
    {
      if(pGetExp (p1, i) + pGetExp (p2, i) > pGetExp (bound, i))
        return FALSE;
      if(i == (currRing->N))
      {
        //PrintS("trivial");
        return TRUE;
      }
      i++;
    }
  }
  else
  {
    loop
    {
      if(pGetExp (p1, i) - pGetExp (m, i) + pGetExp (p2, i) >
         pGetExp (bound, i))
      {
        pDelete (&m);
        return FALSE;
      }
      if(i == (currRing->N))
      {
        pDelete (&m);
        //PrintS("trivial");
        return TRUE;
      }
      i++;
    }
  }
}

//! returns position sets w as weight
int find_best (red_object * r, int l, int u, wlen_type & w, slimgb_alg * c)
{
  int best = l;
  int i;
  w = r[l].guess_quality (c);
  for(i = l + 1; i <= u; i++)
  {
    wlen_type w2 = r[i].guess_quality (c);
    if(w2 < w)
    {
      w = w2;
      best = i;
    }
  }
  return best;
}

void red_object::canonicalize ()
{
  kBucketCanonicalize (bucket);
}

BOOLEAN good_has_t_rep (int i, int j, slimgb_alg * c)
{
  assume (i >= 0);
  assume (j >= 0);
  if(has_t_rep (i, j, c))
    return TRUE;
  //poly lm=pOne();
  assume (c->tmp_lm != NULL);
  poly lm = c->tmp_lm;

  pLcm (c->S->m[i], c->S->m[j], lm);
  pSetm (lm);
  assume (lm != NULL);
  //int deciding_deg= pTotaldegree(lm);
  int *i_con = make_connections (i, j, lm, c);
  //p_Delete(&lm,c->r);

  for(int n = 0; ((n < c->n) && (i_con[n] >= 0)); n++)
  {
    if(i_con[n] == j)
    {
      now_t_rep (i, j, c);
      omFree (i_con);
      return TRUE;
    }
  }
  omFree (i_con);

  return FALSE;
}

BOOLEAN lenS_correct (kStrategy strat)
{
  int i;
  for(i = 0; i <= strat->sl; i++)
  {
    if(strat->lenS[i] != pLength (strat->S[i]))
      return FALSE;
  }
  return TRUE;
}


static void cleanS (kStrategy strat, slimgb_alg * c)
{
  int i = 0;
  LObject P;
  while(i <= strat->sl)
  {
    P.p = strat->S[i];
    P.sev = strat->sevS[i];
    //int dummy=strat->sl;
    //if(kFindDivisibleByInS(strat,&dummy,&P)!=i)
    if(kFindDivisibleByInS_easy (strat, P.p, P.sev) != i)
    {
      deleteInS (i, strat);
      //remember destroying poly
      BOOLEAN found = FALSE;
      int j;
      for(j = 0; j < c->n; j++)
      {
        if(c->S->m[j] == P.p)
        {
          found = TRUE;
          break;
        }
      }
      if(!found)
        pDelete (&P.p);
      //remember additional reductors
    }
    else
      i++;
  }
}

static int bucket_guess (kBucket * bucket)
{
  int sum = 0;
  int i;
  for(i = bucket->buckets_used; i >= 0; i--)
  {
    if(bucket->buckets[i])
      sum += bucket->buckets_length[i];
  }
  return sum;
}

static void
add_to_reductors (slimgb_alg * c, poly h, int len, int ecart,
                  BOOLEAN simplified)
{
  //inDebug(h);
  assume (lenS_correct (c->strat));
  assume (len == pLength (h));
  int i;
//   if (c->isDifficultField)
//        i=simple_posInS(c->strat,h,pSLength(h,len),c->isDifficultField);
//   else
//     i=simple_posInS(c->strat,h,len,c->isDifficultField);

  if (TEST_OPT_IDLIFT &&(pGetComp(h) > c->syz_comp)) return;
  LObject P;
  memset (&P, 0, sizeof (P));
  P.tailRing = c->r;
  P.p = h;                      /*p_Copy(h,c->r); */
  P.ecart = ecart;
  P.FDeg = c->r->pFDeg (P.p, c->r);
  if(!(simplified))
  {
    if(TEST_OPT_INTSTRATEGY)
    {
      p_Cleardenom (P.p, c->r); //includes p_Content(P.p,c->r );
    }
    else
      pNorm (P.p);
    //pNormalize (P.p);
  }
  wlen_type pq = pQuality (h, c, len);
  i = simple_posInS (c->strat, h, len, pq);
  c->strat->enterS (P, i, c->strat, -1);

  c->strat->lenS[i] = len;
  assume (pLength (c->strat->S[i]) == c->strat->lenS[i]);
  if(c->strat->lenSw != NULL)
    c->strat->lenSw[i] = pq;
}

static void length_one_crit (slimgb_alg * c, int pos, int len)
{
  if(c->nc)
    return;
  if(len == 1)
  {
    int i;
    for(i = 0; i < pos; i++)
    {
      if(c->lengths[i] == 1)
        c->states[pos][i] = HASTREP;
    }
    for(i = pos + 1; i < c->n; i++)
    {
      if(c->lengths[i] == 1)
        c->states[i][pos] = HASTREP;
    }
    if(!c->nc)
      shorten_tails (c, c->S->m[pos]);
  }
}

static void move_forward_in_S (int old_pos, int new_pos, kStrategy strat)
{
  assume (old_pos >= new_pos);
  poly p = strat->S[old_pos];
  int ecart = strat->ecartS[old_pos];
  long sev = strat->sevS[old_pos];
  int s_2_r = strat->S_2_R[old_pos];
  int length = strat->lenS[old_pos];
  assume (length == (int)pLength (strat->S[old_pos]));
  wlen_type length_w;
  if(strat->lenSw != NULL)
    length_w = strat->lenSw[old_pos];
  int i;
  for(i = old_pos; i > new_pos; i--)
  {
    strat->S[i] = strat->S[i - 1];
    strat->ecartS[i] = strat->ecartS[i - 1];
    strat->sevS[i] = strat->sevS[i - 1];
    strat->S_2_R[i] = strat->S_2_R[i - 1];
  }
  if(strat->lenS != NULL)
    for(i = old_pos; i > new_pos; i--)
      strat->lenS[i] = strat->lenS[i - 1];
  if(strat->lenSw != NULL)
    for(i = old_pos; i > new_pos; i--)
      strat->lenSw[i] = strat->lenSw[i - 1];

  strat->S[new_pos] = p;
  strat->ecartS[new_pos] = ecart;
  strat->sevS[new_pos] = sev;
  strat->S_2_R[new_pos] = s_2_r;
  strat->lenS[new_pos] = length;
  if(strat->lenSw != NULL)
    strat->lenSw[new_pos] = length_w;
  //assume(lenS_correct(strat));
}

static void move_backward_in_S (int old_pos, int new_pos, kStrategy strat)
{
  assume (old_pos <= new_pos);
  poly p = strat->S[old_pos];
  int ecart = strat->ecartS[old_pos];
  long sev = strat->sevS[old_pos];
  int s_2_r = strat->S_2_R[old_pos];
  int length = strat->lenS[old_pos];
  assume (length == (int)pLength (strat->S[old_pos]));
  wlen_type length_w;
  if(strat->lenSw != NULL)
    length_w = strat->lenSw[old_pos];
  int i;
  for(i = old_pos; i < new_pos; i++)
  {
    strat->S[i] = strat->S[i + 1];
    strat->ecartS[i] = strat->ecartS[i + 1];
    strat->sevS[i] = strat->sevS[i + 1];
    strat->S_2_R[i] = strat->S_2_R[i + 1];
  }
  if(strat->lenS != NULL)
    for(i = old_pos; i < new_pos; i++)
      strat->lenS[i] = strat->lenS[i + 1];
  if(strat->lenSw != NULL)
    for(i = old_pos; i < new_pos; i++)
      strat->lenSw[i] = strat->lenSw[i + 1];

  strat->S[new_pos] = p;
  strat->ecartS[new_pos] = ecart;
  strat->sevS[new_pos] = sev;
  strat->S_2_R[new_pos] = s_2_r;
  strat->lenS[new_pos] = length;
  if(strat->lenSw != NULL)
    strat->lenSw[new_pos] = length_w;
  //assume(lenS_correct(strat));
}

static int *make_connections (int from, int to, poly bound, slimgb_alg * c)
{
  ideal I = c->S;
  int *cans = (int *) omAlloc (c->n * sizeof (int));
  int *connected = (int *) omAlloc (c->n * sizeof (int));
  cans[0] = to;
  int cans_length = 1;
  connected[0] = from;
  int last_cans_pos = -1;
  int connected_length = 1;
  long neg_bounds_short = ~p_GetShortExpVector (bound, c->r);

  int not_yet_found = cans_length;
  int con_checked = 0;
  int pos;

  while(TRUE)
  {
    if((con_checked < connected_length) && (not_yet_found > 0))
    {
      pos = connected[con_checked];
      for(int i = 0; i < cans_length; i++)
      {
        if(cans[i] < 0)
          continue;
        //FIXME: triv. syz. does not hold on noncommutative, check it for modules
        if((has_t_rep (pos, cans[i], c))
           || ((!rIsPluralRing (c->r))
               && (trivial_syzygie (pos, cans[i], bound, c))))
        {
          connected[connected_length] = cans[i];
          connected_length++;
          cans[i] = -1;
          --not_yet_found;

          if(connected[connected_length - 1] == to)
          {
            if(connected_length < c->n)
            {
              connected[connected_length] = -1;
            }
            omFree (cans);
            return connected;
          }
        }
      }
      con_checked++;
    }
    else
    {
      for(last_cans_pos++; last_cans_pos <= c->n; last_cans_pos++)
      {
        if(last_cans_pos == c->n)
        {
          if(connected_length < c->n)
          {
            connected[connected_length] = -1;
          }
          omFree (cans);
          return connected;
        }
        if((last_cans_pos == from) || (last_cans_pos == to))
          continue;
        if(p_LmShortDivisibleBy
           (I->m[last_cans_pos], c->short_Exps[last_cans_pos], bound,
            neg_bounds_short, c->r))
        {
          cans[cans_length] = last_cans_pos;
          cans_length++;
          break;
        }
      }
      not_yet_found++;
      for(int i = 0; i < con_checked; i++)
      {
        if(has_t_rep (connected[i], last_cans_pos, c))
        {
          connected[connected_length] = last_cans_pos;
          connected_length++;
          cans[cans_length - 1] = -1;
          --not_yet_found;
          if(connected[connected_length - 1] == to)
          {
            if(connected_length < c->n)
            {
              connected[connected_length] = -1;
            }
            omFree (cans);
            return connected;
          }
          break;
        }
      }
    }
  }
  if(connected_length < c->n)
  {
    connected[connected_length] = -1;
  }
  omFree (cans);
  return connected;
}

static void replace_pair (int &i, int &j, slimgb_alg * c)
{
  if(i < 0)
    return;
  c->soon_free = NULL;
  int syz_deg;
  poly lm = pOne ();

  pLcm (c->S->m[i], c->S->m[j], lm);
  pSetm (lm);

  int *i_con = make_connections (i, j, lm, c);

  for(int n = 0; ((n < c->n) && (i_con[n] >= 0)); n++)
  {
    if(i_con[n] == j)
    {
      now_t_rep (i, j, c);
      omFree (i_con);
      p_Delete (&lm, c->r);
      return;
    }
  }

  int *j_con = make_connections (j, i, lm, c);

//   if(c->n>1)
//   {
//     if (i_con[1]>=0)
//       i=i_con[1];
//     else
//     {
//       if (j_con[1]>=0)
//         j=j_con[1];
//     }
  // }

  int sugar = syz_deg = c->pTotaldegree (lm);

  p_Delete (&lm, c->r);
  if(c->T_deg_full)             //Sugar
  {
    int t_i = c->T_deg_full[i] - c->T_deg[i];
    int t_j = c->T_deg_full[j] - c->T_deg[j];
    sugar += si_max (t_i, t_j);
    //Print("\n max: %d\n",max(t_i,t_j));
  }

  for(int m = 0; ((m < c->n) && (i_con[m] >= 0)); m++)
  {
    if(c->T_deg_full != NULL)
    {
      int s1 = c->T_deg_full[i_con[m]] + syz_deg - c->T_deg[i_con[m]];
      if(s1 > sugar)
        continue;
    }
    if(c->weighted_lengths[i_con[m]] < c->weighted_lengths[i])
      i = i_con[m];
  }
  for(int m = 0; ((m < c->n) && (j_con[m] >= 0)); m++)
  {
    if(c->T_deg_full != NULL)
    {
      int s1 = c->T_deg_full[j_con[m]] + syz_deg - c->T_deg[j_con[m]];
      if(s1 > sugar)
        continue;
    }
    if(c->weighted_lengths[j_con[m]] < c->weighted_lengths[j])
      j = j_con[m];
  }

  //can also try dependend search
  omFree (i_con);
  omFree (j_con);
  return;
}

static void add_later (poly p, const char *prot, slimgb_alg * c)
{
  int i = 0;
  //check, if it is already in the queue

  while(c->add_later->m[i] != NULL)
  {
    if(p_LmEqual (c->add_later->m[i], p, c->r))
      return;
    i++;
  }
  if(TEST_OPT_PROT)
    PrintS (prot);
  c->add_later->m[i] = p;
}

static int simple_posInS (kStrategy strat, poly p, int len, wlen_type wlen)
{
  if(strat->sl == -1)
    return 0;
  if(strat->lenSw)
    return pos_helper (strat, p, (wlen_type) wlen, (wlen_set) strat->lenSw,
                       strat->S);
  return pos_helper (strat, p, len, strat->lenS, strat->S);
}

/*2
 *if the leading term of p
 *divides the leading term of some S[i] it will be canceled
 */
static inline void
clearS (poly p, unsigned long p_sev, int l, int *at, int *k, kStrategy strat)
{
  assume (p_sev == pGetShortExpVector (p));
  if(!pLmShortDivisibleBy (p, p_sev, strat->S[*at], ~strat->sevS[*at]))
    return;
  if(l >= strat->lenS[*at])
    return;
  if(TEST_OPT_PROT)
    PrintS ("!");
  mflush ();
  //pDelete(&strat->S[*at]);
  deleteInS ((*at), strat);
  (*at)--;
  (*k)--;
//  assume(lenS_correct(strat));
}

static int iq_crit (const void *ap, const void *bp)
{
  sorted_pair_node *a = *((sorted_pair_node **) ap);
  sorted_pair_node *b = *((sorted_pair_node **) bp);
  assume (a->i > a->j);
  assume (b->i > b->j);

  if(a->deg < b->deg)
    return -1;
  if(a->deg > b->deg)
    return 1;
  int comp = pLmCmp (a->lcm_of_lm, b->lcm_of_lm);
  if(comp != 0)
    return comp;
  if(a->expected_length < b->expected_length)
    return -1;
  if(a->expected_length > b->expected_length)
    return 1;
  if(a->j > b->j)
    return 1;
  if(a->j < b->j)
    return -1;
  return 0;
}

static wlen_type coeff_mult_size_estimate (int s1, int s2, ring r)
{
  if(rField_is_Q (r))
    return s1 + s2;
  else
    return s1 * s2;
}

static wlen_type pair_weighted_length (int i, int j, slimgb_alg * c)
{
  if((c->isDifficultField) && (c->eliminationProblem))
  {
    int c1 = slim_nsize (p_GetCoeff (c->S->m[i], c->r), c->r);
    int c2 = slim_nsize (p_GetCoeff (c->S->m[j], c->r), c->r);
    wlen_type el1 = c->weighted_lengths[i] / c1;
    assume (el1 != 0);
    assume (c->weighted_lengths[i] % c1 == 0);
    wlen_type el2 = c->weighted_lengths[j] / c2;
    assume (el2 != 0);
    //assume (c->weighted_lengths[j] % c2 == 0); // fails in Tst/Plural/dmod_lib.tst
    //should be * for function fields
    //return (c1+c2) * (el1+el2-2);
    wlen_type res = coeff_mult_size_estimate (c1, c2, c->r);
    res *= el1 + el2 - 2;
    return res;

  }
  if(c->isDifficultField)
  {
    //int cs=slim_nsize(p_GetCoeff(c->S->m[i],c->r),c->r)+
    //    slim_nsize(p_GetCoeff(c->S->m[j],c->r),c->r);
    if(!(TEST_V_COEFSTRAT))
    {
      wlen_type cs =
        coeff_mult_size_estimate (slim_nsize
                                  (p_GetCoeff (c->S->m[i], c->r), c->r),
                                  slim_nsize (p_GetCoeff (c->S->m[j], c->r),
                                              c->r), c->r);
      return (wlen_type) (c->lengths[i] + c->lengths[j] - 2) * (wlen_type) cs;
    }
    else
    {

      wlen_type cs =
        coeff_mult_size_estimate (slim_nsize
                                  (p_GetCoeff (c->S->m[i], c->r), c->r),
                                  slim_nsize (p_GetCoeff (c->S->m[j], c->r),
                                              c->r), c->r);
      cs *= cs;
      return (wlen_type) (c->lengths[i] + c->lengths[j] - 2) * (wlen_type) cs;
    }
  }
  if(c->eliminationProblem)
  {

    return (c->weighted_lengths[i] + c->weighted_lengths[j] - 2);
  }
  return c->lengths[i] + c->lengths[j] - 2;

}

sorted_pair_node **add_to_basis_ideal_quotient (poly h, slimgb_alg * c,
                                                int *ip)
{
  p_Test (h, c->r);
  assume (h != NULL);
  poly got = gcd_of_terms (h, c->r);
  if((got != NULL) && (TEST_V_UPTORADICAL))
  {
    poly copy = p_Copy (got, c->r);
    //p_wrp(got,c->r);
    BOOLEAN changed = monomial_root (got, c->r);
    if(changed)
    {
      poly div_by = pMDivide (copy, got);
      poly iter = h;
      while(iter)
      {
        pExpVectorSub (iter, div_by);
        pIter (iter);
      }
      p_Delete (&div_by, c->r);
      PrintS ("U");
    }
    p_Delete (&copy, c->r);
  }

#define ENLARGE(pointer, type) pointer=(type*) omreallocSize(pointer, old*sizeof(type),c->array_lengths*sizeof(type))

#define ENLARGE_ALIGN(pointer, type) {if(pointer)\
         pointer=(type*)omReallocSize(pointer, old*sizeof(type),c->array_lengths*sizeof(type));\
         else pointer=(type*)omAllocAligned(c->array_lengths*sizeof(type));}
//  BOOLEAN corr=lenS_correct(c->strat);
  int sugar;
  int ecart = 0;
  ++(c->n);
  ++(c->S->ncols);
  int i, j;
  i = c->n - 1;
  sorted_pair_node **nodes =
    (sorted_pair_node **) omalloc (sizeof (sorted_pair_node *) * i);
  int spc = 0;
  int old=c->array_lengths;
  if(c->n > c->array_lengths)
  {
    c->array_lengths = c->array_lengths * 2;
    assume (c->array_lengths >= c->n);
    ENLARGE (c->T_deg, int);
    ENLARGE_ALIGN (c->tmp_pair_lm, poly);
    ENLARGE_ALIGN (c->tmp_spn, sorted_pair_node *);

    ENLARGE_ALIGN (c->short_Exps, long);
    ENLARGE (c->lengths, int);
#ifndef HAVE_BOOST
#ifndef USE_STDVECBOOL

    ENLARGE_ALIGN (c->states, char *);
#endif
#endif
    ENLARGE_ALIGN (c->gcd_of_terms, poly);
    //if (c->weighted_lengths!=NULL) {
    ENLARGE_ALIGN (c->weighted_lengths, wlen_type);
    //}
    //ENLARGE_ALIGN(c->S->m,poly);
  }
  pEnlargeSet (&c->S->m, c->n - 1, 1);
  if(c->T_deg_full)
    ENLARGE (c->T_deg_full, int);
  sugar = c->T_deg[i] = c->pTotaldegree (h);
  if(c->T_deg_full)
  {
    sugar = c->T_deg_full[i] = c->pTotaldegree_full (h);
    ecart = sugar - c->T_deg[i];
    assume (ecart >= 0);
  }
  c->tmp_pair_lm[i] = pOne_Special (c->r);

  c->tmp_spn[i] = (sorted_pair_node *) omAlloc (sizeof (sorted_pair_node));

  c->lengths[i] = pLength (h);

  //necessary for correct weighted length

  if(TEST_OPT_INTSTRATEGY)
  {
    p_Cleardenom (h, c->r); //includes p_Content(h,c->r);
  }
  else
    pNorm (h);
  //pNormalize (h);

  c->weighted_lengths[i] = pQuality (h, c, c->lengths[i]);
  c->gcd_of_terms[i] = got;
#ifdef HAVE_BOOST
  c->states.push_back (dynamic_bitset <> (i));

#else
#ifdef USE_STDVECBOOL

  c->states.push_back (vector < bool > (i));

#else
  if(i > 0)
    c->states[i] = (char *) omAlloc (i * sizeof (char));
  else
    c->states[i] = NULL;
#endif
#endif

  c->S->m[i] = h;
  c->short_Exps[i] = p_GetShortExpVector (h, c->r);

#undef ENLARGE
#undef ENLARGE_ALIGN
  if(p_GetComp (h, currRing) <= c->syz_comp)
  {
    for(j = 0; j < i; j++)
    {


#ifndef HAVE_BOOST
      c->states[i][j] = UNCALCULATED;
#endif
      assume (p_LmDivisibleBy (c->S->m[i], c->S->m[j], c->r) ==
              p_LmShortDivisibleBy (c->S->m[i], c->short_Exps[i], c->S->m[j],
                                    ~(c->short_Exps[j]), c->r));

      if(__p_GetComp (c->S->m[i], c->r) != __p_GetComp (c->S->m[j], c->r))
      {
        //c->states[i][j]=UNCALCULATED;
        //WARNUNG: be careful
        continue;
      }
      else if((!c->nc) && (c->lengths[i] == 1) && (c->lengths[j] == 1))
      {
        c->states[i][j] = HASTREP;
      }
      else if(((!c->nc) || (c->is_homog && rIsSCA (c->r)))
              && (pHasNotCF (c->S->m[i], c->S->m[j])))
//     else if ((!(c->nc)) &&  (pHasNotCF(c->S->m[i],c->S->m[j])))
      {
        c->easy_product_crit++;
        c->states[i][j] = HASTREP;
        continue;
      }
      else
        if(extended_product_criterion
           (c->S->m[i], c->gcd_of_terms[i], c->S->m[j], c->gcd_of_terms[j],
            c))
      {
        c->states[i][j] = HASTREP;
        c->extended_product_crit++;
        //PrintS("E");
      }
      //  if (c->states[i][j]==UNCALCULATED)
      //  {

      if((TEST_V_FINDMONOM) && (!c->nc))
      {
        //PrintS("COMMU");
        //  if (c->lengths[i]==c->lengths[j])
        //  {
//             poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
//             if (short_s==NULL)
//             {
//                 c->states[i][j]=HASTREP;
//             }
//             else
//             {
//                 p_Delete(&short_s, currRing);
//             }
//         }
        if(c->lengths[i] + c->lengths[j] == 3)
        {


          poly short_s = ksCreateShortSpoly (c->S->m[i], c->S->m[j], c->r);
          if(short_s == NULL)
          {
            c->states[i][j] = HASTREP;
          }
          else
          {
            assume (pLength (short_s) == 1);
            if(TEST_V_UPTORADICAL)
              monomial_root (short_s, c->r);
            int iS = kFindDivisibleByInS_easy (c->strat, short_s,
                                               p_GetShortExpVector (short_s,
                                                                    c->r));
            if(iS < 0)
            {
              //PrintS("N");
              if(TRUE)
              {
                c->states[i][j] = HASTREP;
                add_later (short_s, "N", c);
              }
              else
                p_Delete (&short_s, currRing);
            }
            else
            {
              if(c->strat->lenS[iS] > 1)
              {
                //PrintS("O");
                if(TRUE)
                {
                  c->states[i][j] = HASTREP;
                  add_later (short_s, "O", c);
                }
                else
                  p_Delete (&short_s, currRing);
              }
              else
                p_Delete (&short_s, currRing);
              c->states[i][j] = HASTREP;
            }


          }
        }
      }
      //    if (short_s)
      //    {
      assume (spc <= j);
      sorted_pair_node *s = c->tmp_spn[spc];    //(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      if (i>j) { s->i=i; s->j=j;}
      else     { s->i=j; s->j=i;}
      s->expected_length = pair_weighted_length (i, j, c);      //c->lengths[i]+c->lengths[j]-2;

      poly lm = c->tmp_pair_lm[spc];    //=pOne_Special();

      pLcm (c->S->m[i], c->S->m[j], lm);
      pSetm (lm);
      p_Test (lm, c->r);
      s->deg = c->pTotaldegree (lm);

      if(c->T_deg_full)         //Sugar
      {
        int t_i = c->T_deg_full[s->i] - c->T_deg[s->i];
        int t_j = c->T_deg_full[s->j] - c->T_deg[s->j];
        s->deg += si_max (t_i, t_j);
        //Print("\n max: %d\n",max(t_i,t_j));
      }
      p_Test (lm, c->r);
      s->lcm_of_lm = lm;
      //          pDelete(&short_s);
      //assume(lm!=NULL);
      nodes[spc] = s;
      spc++;

      // }
      //else
      //{
      //c->states[i][j]=HASTREP;
      //}
    }
  }                             //if syz_comp end

  assume (spc <= i);
  //now ideal quotient crit
  qsort (nodes, spc, sizeof (sorted_pair_node *), iq_crit);

  sorted_pair_node **nodes_final =
    (sorted_pair_node **) omalloc (sizeof (sorted_pair_node *) * (i+1));
  int spc_final = 0;
  j = 0;
  while(j < spc)
  {
    int lower = j;
    int upper;
    BOOLEAN has = FALSE;
    for(upper = lower + 1; upper < spc; upper++)
    {
      if(!pLmEqual (nodes[lower]->lcm_of_lm, nodes[upper]->lcm_of_lm))
      {
        break;
      }
      if(has_t_rep (nodes[upper]->i, nodes[upper]->j, c))
        has = TRUE;
    }
    upper = upper - 1;
    int z;
    assume (spc_final <= j);
    for(z = 0; z < spc_final; z++)
    {
      if(p_LmDivisibleBy
         (nodes_final[z]->lcm_of_lm, nodes[lower]->lcm_of_lm, c->r))
      {
        has = TRUE;
        break;
      }
    }

    if(has)
    {
      for(; lower <= upper; lower++)
      {
        //free_sorted_pair_node(nodes[lower],c->r);
        //omfree(nodes[lower]);
        nodes[lower] = NULL;
      }
      j = upper + 1;
      continue;
    }
    else
    {
      p_Test (nodes[lower]->lcm_of_lm, c->r);
      nodes[lower]->lcm_of_lm = pCopy (nodes[lower]->lcm_of_lm);
      assume (__p_GetComp (c->S->m[nodes[lower]->i], c->r) ==
              __p_GetComp (c->S->m[nodes[lower]->j], c->r));
      nodes_final[spc_final] =
        (sorted_pair_node *) omAlloc (sizeof (sorted_pair_node));

      *(nodes_final[spc_final++]) = *(nodes[lower]);
      //c->tmp_spn[nodes[lower]->j]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      nodes[lower] = NULL;
      for(lower = lower + 1; lower <= upper; lower++)
      {
        //  free_sorted_pair_node(nodes[lower],c->r);
        //omfree(nodes[lower]);
        nodes[lower] = NULL;
      }
      j = upper + 1;
      continue;
    }
  }

  //  Print("i:%d,spc_final:%d",i,spc_final);

  assume (spc_final <= spc);
  omfree (nodes);
  nodes = NULL;

  add_to_reductors (c, h, c->lengths[c->n - 1], ecart, TRUE);
  //i=posInS(c->strat,c->strat->sl,h,0 ecart);
  if(!(c->nc))
  {
    if(c->lengths[c->n - 1] == 1)
      shorten_tails (c, c->S->m[c->n - 1]);
  }
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if(c->Rcounter > 50)
  {
    c->Rcounter = 0;
    cleanS (c->strat, c);
  }

#ifdef HAVE_PLURAL
  // for SCA:
  // here write at the end of nodes_final[spc_final,...,spc_final+lmdeg-1]
  if(rIsSCA (c->r))
  {
    const poly pNext = pNext (h);

    if(pNext != NULL)
    {
      // for additional polynomials
      const unsigned int m_iFirstAltVar = scaFirstAltVar (c->r);
      const unsigned int m_iLastAltVar = scaLastAltVar (c->r);

      int N =                   // c->r->N;
        m_iLastAltVar - m_iFirstAltVar + 1;     // should be enough
      // TODO: but we may also use got = gcd({m}_{m\in f}))!

      poly *array_arg = (poly *) omalloc (N * sizeof (poly));   // !
      int j = 0;


      for(unsigned short v = m_iFirstAltVar; v <= m_iLastAltVar; v++)
        // for all x_v | Ann(lm(h))
        if(p_GetExp (h, v, c->r))       // TODO: use 'got' here!
        {
          assume (p_GetExp (h, v, c->r) == 1);

          poly p = sca_pp_Mult_xi_pp (v, pNext, c->r);  // x_v * h;

          if(p != NULL)         // if (x_v * h != 0)
            array_arg[j++] = p;
        }                       // for all x_v | Ann(lm(h))

      c->introduceDelayedPairs (array_arg, j);

      omFree (array_arg);       // !!!
    }
//     PrintS("Saturation - done!!!\n");
  }
#endif // if SCAlgebra


  if(!ip)
  {
    qsort (nodes_final, spc_final, sizeof (sorted_pair_node *),
           tgb_pair_better_gen2);


    c->apairs =
      spn_merge (c->apairs, c->pair_top + 1, nodes_final, spc_final, c);
    c->pair_top += spc_final;
    clean_top_of_pair_list (c);
    omFree (nodes_final);
    return NULL;
  }
  {
    *ip = spc_final;
    return nodes_final;
  }
}

static poly redNF2 (poly h, slimgb_alg * c, int &len, number & m, int n)
{
  m = nInit (1);
  if(h == NULL)
    return NULL;

  assume (len == (int)pLength (h));
  kStrategy strat = c->strat;
  if(0 > strat->sl)
  {
    return h;
  }
  int j;

  LObject P (h);
  P.SetShortExpVector ();
  P.bucket = kBucketCreate (currRing);
  // BOOLEAN corr=lenS_correct(strat);
  kBucketInit (P.bucket, P.p, len /*pLength(P.p) */ );
  //wlen_set lenSw=(wlen_set) c->strat->lenS;
  //FIXME: plainly wrong
  //strat->lenS;
  //if (strat->lenSw!=NULL)
  //  lenSw=strat->lenSw;
  //int max_pos=simple_posInS(strat,P.p);
  loop
  {
    //int dummy=strat->sl;
    j = kFindDivisibleByInS_easy (strat, P.p, P.sev);
    //j=kFindDivisibleByInS(strat,&dummy,&P);
    if((j >= 0) && ((!n) ||
                    ((strat->lenS[j] <= n) &&
                     ((strat->lenSw == NULL) || (strat->lenSw[j] <= n)))))
    {
      nNormalize (pGetCoeff (P.p));
#ifdef KDEBUG
      if(TEST_OPT_DEBUG)
      {
        PrintS ("red:");
        wrp (h);
        PrintS (" with ");
        wrp (strat->S[j]);
      }
#endif

      number coef = kBucketPolyRed (P.bucket, strat->S[j],
                                    strat->lenS[j] /*pLength(strat->S[j]) */ ,
                                    strat->kNoether);
      number m2 = nMult (m, coef);
      nDelete (&m);
      m = m2;
      nDelete (&coef);
      h = kBucketGetLm (P.bucket);

      if(h == NULL)
      {
        len = 0;
        kBucketDestroy (&P.bucket);
        return NULL;
      }
      P.p = h;
      P.t_p = NULL;
      P.SetShortExpVector ();
#ifdef KDEBUG
      if(TEST_OPT_DEBUG)
      {
        PrintS ("\nto:");
        wrp (h);
        PrintLn ();
      }
#endif
    }
    else
    {
      kBucketClear (P.bucket, &(P.p), &len);
      kBucketDestroy (&P.bucket);
      pNormalize (P.p);
      assume (len == (int)pLength (P.p));
      return P.p;
    }
  }
}

static poly redTailShort (poly h, kStrategy strat)
{
  if(h == NULL)
    return NULL;                //n_Init(1,currRing);
  if(TEST_V_MODPSOLVSB)
  {
    bit_reduce (pNext (h), strat->tailRing);
  }
  int i;
  int len = pLength (h);
  for(i = 0; i <= strat->sl; i++)
  {
    if((strat->lenS[i] > 2)
       || ((strat->lenSw != NULL) && (strat->lenSw[i] > 2)))
      break;
  }
  return (redNFTail (h, i - 1, strat, len));
}

static void line_of_extended_prod (int fixpos, slimgb_alg * c)
{
  if(c->gcd_of_terms[fixpos] == NULL)
  {
    c->gcd_of_terms[fixpos] = gcd_of_terms (c->S->m[fixpos], c->r);
    if(c->gcd_of_terms[fixpos])
    {
      int i;
      for(i = 0; i < fixpos; i++)
        if((c->states[fixpos][i] != HASTREP)
           &&
           (extended_product_criterion
            (c->S->m[fixpos], c->gcd_of_terms[fixpos], c->S->m[i],
             c->gcd_of_terms[i], c)))
        {
          c->states[fixpos][i] = HASTREP;
          c->extended_product_crit++;
        }
      for(i = fixpos + 1; i < c->n; i++)
        if((c->states[i][fixpos] != HASTREP)
           &&
           (extended_product_criterion
            (c->S->m[fixpos], c->gcd_of_terms[fixpos], c->S->m[i],
             c->gcd_of_terms[i], c)))
        {
          c->states[i][fixpos] = HASTREP;
          c->extended_product_crit++;
        }
    }
  }
}

static void c_S_element_changed_hook (int pos, slimgb_alg * c)
{
  length_one_crit (c, pos, c->lengths[pos]);
  if(!c->nc)
    line_of_extended_prod (pos, c);
}

class poly_tree_node
{
public:
  poly p;
  poly_tree_node *l;
  poly_tree_node *r;
  int n;
  poly_tree_node (int sn):l (NULL), r (NULL), n (sn)
  {
  }
};
class exp_number_builder
{
public:
  poly_tree_node * top_level;
  int n;
  int get_n (poly p);
  exp_number_builder ():top_level (0), n (0)
  {
  }
};
int exp_number_builder::get_n (poly p)
{
  poly_tree_node **node = &top_level;
  while(*node != NULL)
  {
    int c = pLmCmp (p, (*node)->p);
    if(c == 0)
      return (*node)->n;
    if(c == -1)
      node = &((*node)->r);
    else
      node = &((*node)->l);
  }
  (*node) = new poly_tree_node (n);
  n++;
  (*node)->p = pLmInit (p);
  return (*node)->n;
}

//mac_polys exp are smaller iff they are greater by monomial ordering
//corresponding to solving linear equations notation

BOOLEAN is_valid_ro (red_object & ro)
{
  red_object r2 = ro;
  ro.validate ();
  if((r2.p != ro.p) || (r2.sev != ro.sev))
    return FALSE;
  return TRUE;
}

int terms_sort_crit (const void *a, const void *b)
{
  return -pLmCmp (*((poly *) a), *((poly *) b));
}

#if 0                           // currently unused
static void unify_terms (poly * terms, int &sum)
{
  if(sum == 0)
    return;
  int last = 0;
  int curr = 1;
  while(curr < sum)
  {
    if(!(pLmEqual (terms[curr], terms[last])))
    {
      terms[++last] = terms[curr];
    }
    ++curr;
  }
  sum = last + 1;
}
#endif
#if 0                           // currently unused
static void
export_mat (number * number_array, int pn, int tn, const char *format_str,
            int mat_nr)
{
  char matname[20];
  sprintf (matname, format_str, mat_nr);
  FILE *out = fopen (matname, "w");
  int i, j;
  fprintf (out, "mat=[\n");
  for(i = 0; i < pn; i++)
  {
    fprintf (out, "[\n");
    for(j = 0; j < tn; j++)
    {
      if(j > 0)
      {
        fprintf (out, ", ");
      }
      fprintf (out, "%i", npInt (number_array[i * tn + j], currRing));
    }
    if(i < pn - 1)
      fprintf (out, "],\n");
    else
      fprintf (out, "],\n");
  }
  fprintf (out, "]\n");
  fclose (out);
}
#endif
//typedef unsigned short number_type;


#ifdef USE_NORO
#ifndef NORO_CACHE
static void
linalg_step_modp (poly * p, poly * p_out, int &pn, poly * terms, int tn,
                  slimgb_alg * c)
{
  STATIC_VAR int export_n = 0;
  assume (terms[tn - 1] != NULL);
  assume (rField_is_Zp (c->r));
  //I don't do deletes, copies of number_types ...
  const number_type zero = 0;   //npInit(0);
  int array_size = pn * tn;
  number_type *number_array =
    (number_type *) omalloc (pn * tn * sizeof (number_type));
  int i;
  for(i = 0; i < array_size; i++)
  {
    number_array[i] = zero;
  }
  for(i = 0; i < pn; i++)
  {
    poly h = p[i];
    //int base=tn*i;
    write_poly_to_row (number_array + tn * i, h, terms, tn);

  }
#if 0
  //export matrix
  export_mat (number_array, pn, tn, "mat%i.py", ++export_n);
#endif
  int rank = pn;
  simplest_gauss_modp (number_array, rank, tn);
  int act_row = 0;
  int p_pos = 0;
  for(i = 0; i < pn; i++)
  {
    poly h = NULL;
    int j;
    int base = tn * i;
    number *row = number_array + base;
    h = row_to_poly (row, terms, tn, c->r);

    if(h != NULL)
    {
      p_out[p_pos++] = h;
    }
  }
  pn = p_pos;
  //assert(p_pos==rank)
  while(p_pos < pn)
  {
    p_out[p_pos++] = NULL;
  }
#if 0
  export_mat (number_array, pn, tn, "mat%i.py", ++export_n);
#endif
}
#endif
#endif
static void mass_add (poly * p, int pn, slimgb_alg * c)
{
  int j;
  int *ibuf = (int *) omalloc (pn * sizeof (int));
  sorted_pair_node ***sbuf =
    (sorted_pair_node ***) omalloc (pn * sizeof (sorted_pair_node **));
  for(j = 0; j < pn; j++)
  {
    p_Test (p[j], c->r);
    sbuf[j] = add_to_basis_ideal_quotient (p[j], c, ibuf + j);
  }
  int sum = 0;
  for(j = 0; j < pn; j++)
  {
    sum += ibuf[j];
  }
  sorted_pair_node **big_sbuf =
    (sorted_pair_node **) omalloc (sum * sizeof (sorted_pair_node *));
  int partsum = 0;
  for(j = 0; j < pn; j++)
  {
    memmove (big_sbuf + partsum, sbuf[j],
             ibuf[j] * sizeof (sorted_pair_node *));
    omFree (sbuf[j]);
    partsum += ibuf[j];
  }

  qsort (big_sbuf, sum, sizeof (sorted_pair_node *), tgb_pair_better_gen2);
  c->apairs = spn_merge (c->apairs, c->pair_top + 1, big_sbuf, sum, c);
  c->pair_top += sum;
  clean_top_of_pair_list (c);
  omfree (big_sbuf);
  omfree (sbuf);
  omfree (ibuf);
  //omfree(buf);
#ifdef TGB_DEBUG
  int z;
  for(z = 1; z <= c->pair_top; z++)
  {
    assume (pair_better (c->apairs[z], c->apairs[z - 1], c));
  }
#endif

}

#ifdef NORO_CACHE
#ifndef NORO_NON_POLY
void NoroCache::evaluateRows ()
{
  //after that can evaluate placeholders
  int i;
  buffer = (number *) omAlloc (nIrreducibleMonomials * sizeof (number));
  for(i = 0; i < root.branches_len; i++)
  {
    evaluateRows (1, root.branches[i]);
  }
  omFree (buffer);
  buffer = NULL;
}

void NoroCache::evaluateRows (int level, NoroCacheNode * node)
{
  assume (level >= 0);
  if(node == NULL)
    return;
  if(level < (currRing->N))
  {
    int i, sum;
    for(i = 0; i < node->branches_len; i++)
    {
      evaluateRows (level + 1, node->branches[i]);
    }
  }
  else
  {
    DataNoroCacheNode *dn = (DataNoroCacheNode *) node;
    if(dn->value_len != backLinkCode)
    {
      poly p = dn->value_poly;
#ifndef NORO_SPARSE_ROWS_PRE
      dn->row = new DenseRow ();
      DenseRow *row = dn->row;
      memset (buffer, 0, sizeof (number) * nIrreducibleMonomials);

      if(p == NULL)
      {
        row->array = NULL;
        row->begin = 0;
        row->end = 0;
        return;
      }
      int i = 0;
      int idx;
      number *a = buffer;
      while(p)
      {
        DataNoroCacheNode *ref = getCacheReference (p);

        idx = ref->term_index;
        assume (idx >= 0);
        a[idx] = p_GetCoeff (p, currRing);
        if(i == 0)
          row->begin = idx;
        i++;
        pIter (p);
      }
      row->end = idx + 1;
      assume (row->end > row->begin);
      int len = row->end - row->begin;
      row->array = (number *) omalloc ((len) * sizeof (number));
      memcpy (row->array, a + row->begin, len * sizeof (number));
#else
      assume (dn->value_len == pLength (dn->value_poly));
      dn->row = new SparseRow (dn->value_len);
      SparseRow *row = dn->row;
      int i = 0;
      while(p)
      {
        DataNoroCacheNode *ref = getCacheReference (p);

        int idx = ref->term_index;
        assume (idx >= 0);
        row->idx_array[i] = idx;
        row->coef_array[i] = p_GetCoeff (p, currRing);
        i++;
        pIter (p);
      }
      if(i != dn->value_len)
      {
        PrintS ("F4 calc wrong, as poly len was wrong\n");
      }
      assume (i == dn->value_len);
#endif
    }
  }
}

void
  NoroCache::evaluatePlaceHolder (number * row,
                                  std::vector < NoroPlaceHolder >
                                  &place_holders)
{
  int i;
  int s = place_holders.size ();

  if (currRing->cf-ch<=NV_MAX_PRIME)
  {
    for(i = 0; i < s; i++)
    {
      DataNoroCacheNode *ref = place_holders[i].ref;
      number coef = place_holders[i].coef;
      if(ref->value_len == backLinkCode)
      {
        row[ref->term_index] = npAddM (row[ref->term_index], coef);
      }
      else
      {
  #ifndef NORO_SPARSE_ROWS_PRE
        DenseRow *ref_row = ref->row;
        if(ref_row == NULL)
          continue;
        number *ref_begin = ref_row->array;
        number *ref_end = ref_row->array + (ref_row->end - ref_row->begin);
        number *my_pos = row + ref_row->begin;
        //TODO npisOne distinction
        if(!(npIsOne (coef)))
        {
          while(ref_begin != ref_end)
          {
            *my_pos = npAddM (*my_pos, npMult (coef, *ref_begin));
            ++ref_begin;
            ++my_pos;
          }
        }
        else
        {
          while(ref_begin != ref_end)
          {

            *my_pos = npAddM (*my_pos, *ref_begin);
            ++ref_begin;
            ++my_pos;
          }
        }
  #else
        SparseRow *ref_row = ref->row;
        if(ref_row == NULL)
          continue;
        int n = ref_row->len;
        int j;
        int *idx_array = ref_row->idx_array;
        number *coef_array = ref_row->coef_array;
        if(!(npIsOne (coef)))
        {
          for(j = 0; j < n; j++)
          {
            int idx = idx_array[j];
            number ref_coef = coef_array[j];
            row[idx] = npAddM (row[idx], npMult (coef, ref_coef));
          }
        }
        else
        {
          for(j = 0; j < n; j++)
          {
            int idx = idx_array[j];
            number ref_coef = coef_array[j];
            row[idx] = npAddM (row[idx], ref_coef);
          }
        }
  #endif
      }
    }
  }
  else /*ch >NV_MAX_PRIME */
  {
    for(i = 0; i < s; i++)
    {
      DataNoroCacheNode *ref = place_holders[i].ref;
      number coef = place_holders[i].coef;
      if(ref->value_len == backLinkCode)
      {
        row[ref->term_index] = npAddM (row[ref->term_index], coef);
      }
      else
      {
  #ifndef NORO_SPARSE_ROWS_PRE
        DenseRow *ref_row = ref->row;
        if(ref_row == NULL)
          continue;
        number *ref_begin = ref_row->array;
        number *ref_end = ref_row->array + (ref_row->end - ref_row->begin);
        number *my_pos = row + ref_row->begin;
        //TODO npisOne distinction
        if(!(npIsOne (coef)))
        {
          while(ref_begin != ref_end)
          {
            *my_pos = npAddM (*my_pos, nvMult (coef, *ref_begin));
            ++ref_begin;
            ++my_pos;
          }
        }
        else
        {
          while(ref_begin != ref_end)
          {
            *my_pos = npAddM (*my_pos, *ref_begin);
            ++ref_begin;
            ++my_pos;
          }
        }
  #else
        SparseRow *ref_row = ref->row;
        if(ref_row == NULL)
          continue;
        int n = ref_row->len;
        int j;
        int *idx_array = ref_row->idx_array;
        number *coef_array = ref_row->coef_array;
        if(!(npIsOne (coef)))
        {
          for(j = 0; j < n; j++)
          {
            int idx = idx_array[j];
            number ref_coef = coef_array[j];
            row[idx] = npAddM (row[idx], nvMult (coef, ref_coef));
          }
        }
        else
        {
          for(j = 0; j < n; j++)
          {
            int idx = idx_array[j];
            number ref_coef = coef_array[j];
            row[idx] = npAddM (row[idx], ref_coef);
          }
        }
  #endif
      }
    }
  }
}
#endif

//poly noro_red_non_unique(poly p, int &len, NoroCache* cache,slimgb_alg* c);

#ifndef NORO_NON_POLY
MonRedRes
noro_red_mon (poly t, BOOLEAN force_unique, NoroCache * cache, slimgb_alg * c)
{
  MonRedRes res_holder;

  //wrp(t);
  res_holder.changed = TRUE;
  if(force_unique)
  {
    DataNoroCacheNode *ref = cache->getCacheReference (t);
    if(ref != NULL)
    {
      res_holder.len = ref->value_len;
      if(res_holder.len == NoroCache::backLinkCode)
      {
        res_holder.len = 1;
      }
      res_holder.coef = p_GetCoeff (t, c->r);
      res_holder.p = ref->value_poly;
      res_holder.ref = ref;
      res_holder.onlyBorrowed = TRUE;
      res_holder.changed = TRUE;
      p_Delete (&t, c->r);
      return res_holder;
    }
  }
  else
  {
    BOOLEAN succ;
    poly cache_lookup = cache->lookup (t, succ, res_holder.len);        //don't own this yet
    if(succ)
    {
      if(cache_lookup == t)
      {
        //know they are equal
        //res_holder.len=1;

        res_holder.changed = FALSE;
        res_holder.p = t;
        res_holder.coef = npInit (1);

        res_holder.onlyBorrowed = FALSE;
        return res_holder;
      }

      res_holder.coef = p_GetCoeff (t, c->r);
      p_Delete (&t, c->r);

      res_holder.p = cache_lookup;

      res_holder.onlyBorrowed = TRUE;
      return res_holder;

    }
  }

  unsigned long sev = p_GetShortExpVector (t, currRing);
  int i = kFindDivisibleByInS_easy (c->strat, t, sev);
  if(i >= 0)
  {
    number coef_bak = p_GetCoeff (t, c->r);

    p_SetCoeff (t, npInit (1), c->r);
    assume (npIsOne (p_GetCoeff (c->strat->S[i], c->r)));
    number coefstrat = p_GetCoeff (c->strat->S[i], c->r);

    //poly t_copy_mon=p_Copy(t,c->r);
    poly exp_diff = cache->temp_term;
    p_ExpVectorDiff (exp_diff, t, c->strat->S[i], c->r);
    p_SetCoeff (exp_diff, npNeg (nInvers (coefstrat)), c->r);
    // nInvers may be npInvers or nvInvers
    p_Setm (exp_diff, c->r);
    assume (c->strat->S[i] != NULL);
    //poly t_to_del=t;
    poly res;
    res = pp_Mult_mm (pNext (c->strat->S[i]), exp_diff, c->r);

    res_holder.len = c->strat->lenS[i] - 1;
    res = noro_red_non_unique (res, res_holder.len, cache, c);

    DataNoroCacheNode *ref = cache->insert (t, res, res_holder.len);
    p_Delete (&t, c->r);
    //p_Delete(&t_copy_mon,c->r);
    //res=pMult_nn(res,coef_bak);
    res_holder.changed = TRUE;
    res_holder.p = res;
    res_holder.coef = coef_bak;
    res_holder.onlyBorrowed = TRUE;
    res_holder.ref = ref;
    return res_holder;
  }
  else
  {
    number coef_bak = p_GetCoeff (t, c->r);
    number one = npInit (1);
    p_SetCoeff (t, one, c->r);
    res_holder.len = 1;
    if(!(force_unique))
    {
      res_holder.ref = cache->insert (t, t, res_holder.len);
      p_SetCoeff (t, coef_bak, c->r);
      //return t;

      //we need distinction
      res_holder.changed = FALSE;
      res_holder.p = t;

      res_holder.coef = npInit (1);
      res_holder.onlyBorrowed = FALSE;
      return res_holder;
    }
    else
    {
      res_holder.ref = cache->insertAndTransferOwnerShip (t, c->r);
      res_holder.coef = coef_bak;
      res_holder.onlyBorrowed = TRUE;
      res_holder.changed = FALSE;
      res_holder.p = t;
      return res_holder;
    }
  }

}
#endif
//SparseRow* noro_red_to_non_poly(poly p, int &len, NoroCache* cache,slimgb_alg* c);
#ifndef NORO_NON_POLY
//len input and out: Idea: reverse addition
poly noro_red_non_unique (poly p, int &len, NoroCache * cache, slimgb_alg * c)
{
  assume (len == pLength (p));
  poly orig_p = p;
  if(p == NULL)
  {
    len = 0;
    return NULL;
  }
  kBucket_pt bucket = kBucketCreate (currRing);
  kBucketInit (bucket, NULL, 0);
  poly unchanged_head = NULL;
  poly unchanged_tail = NULL;
  int unchanged_size = 0;

  while(p)
  {
    poly t = p;
    pIter (p);
    pNext (t) = NULL;
#ifndef SING_NDEBUG
    number coef_debug = p_GetCoeff (t, currRing);
#endif
    MonRedRes red = noro_red_mon (t, FALSE, cache, c);
    if((!(red.changed)) && (!(red.onlyBorrowed)))
    {
      unchanged_size++;
      assume (npIsOne (red.coef));
      assume (p_GetCoeff (red.p, currRing) == coef_debug);
      if(unchanged_head)
      {
        pNext (unchanged_tail) = red.p;
        pIter (unchanged_tail);
      }
      else
      {
        unchanged_tail = red.p;
        unchanged_head = red.p;
      }
    }
    else
    {
      assume (red.len == pLength (red.p));
      if(red.onlyBorrowed)
      {
        if(npIsOne (red.coef))
        {
          t = p_Copy (red.p, currRing);
        }
        else
          t = __pp_Mult_nn (red.p, red.coef, currRing);
      }
      else
      {
        if(npIsOne (red.coef))
          t = red.p;
        else
          t = __p_Mult_nn (red.p, red.coef, currRing);
      }
      kBucket_Add_q (bucket, t, &red.len);
    }
  }
  poly res = NULL;
  len = 0;
  kBucket_Add_q (bucket, unchanged_head, &unchanged_size);
  kBucketClear (bucket, &res, &len);
  kBucketDestroy (&bucket);
  return res;
}
#endif
#ifdef NORO_SPARSE_ROWS_PRE
//len input and out: Idea: reverse addition

/*template <class number_type> SparseRow<number_type>* noro_red_to_non_poly(poly p, int &len, NoroCache<number_type>* cache,slimgb_alg* c)
 * {
  if (n_GetChar(currRing->cf)<255)
  {
    return noro_red_to_non_poly_t<tgb_uint8>(p,len,cache,c);
  }
  else
  {
    if (n_GetChar(currRing->cf)<65000)
    {
      return noro_red_to_non_poly_t<tgb_uint16>(p,len,cache,c);
    }
    else
    {
      return noro_red_to_non_poly_t<tgb_uint32>(p,len,cache,c);
    }
  }
}*/
#endif
//len input and out: Idea: reverse addition
#ifndef NORO_NON_POLY
std::vector < NoroPlaceHolder > noro_red (poly p, int &len, NoroCache * cache,
                                          slimgb_alg * c)
{
  std::vector < NoroPlaceHolder > res;
  while(p)
  {
    poly t = p;
    pIter (p);
    pNext (t) = NULL;

    MonRedRes red = noro_red_mon (t, TRUE, cache, c);
    assume (red.onlyBorrowed);
    assume (red.coef);
    assume (red.ref);
    NoroPlaceHolder h;
    h.ref = red.ref;
    h.coef = red.coef;
    assume (!((h.ref->value_poly == NULL) && (h.ref->value_len != 0)));
    if(h.ref->value_poly)
      res.push_back (h);
  }
  return res;
}
#endif

#endif
#ifdef USE_NORO
#ifndef NORO_CACHE
void noro_step (poly * p, int &pn, slimgb_alg * c)
{
  poly *reduced = (poly *) omalloc (pn * sizeof (poly));
  int j;
  int *reduced_len = (int *) omalloc (pn * sizeof (int));
  int reduced_c = 0;
  //if (TEST_OPT_PROT)
  //  PrintS("reduced system:\n");
#ifdef NORO_CACHE
  NoroCache cache;
#endif
  for(j = 0; j < pn; j++)
  {

    poly h = p[j];
    int h_len = pLength (h);

    number coef;
#ifndef NORO_CACHE
    h = redNF2 (p_Copy (h, c->r), c, h_len, coef, 0);
#else
    h = noro_red (p_Copy (h, c->r), h_len, &cache, c);
    assume (pLength (h) == h_len);
#endif
    if(h != NULL)
    {
#ifndef NORO_CACHE

      h = redNFTail (h, c->strat->sl, c->strat, h_len);
      h_len = pLength (h);
#endif
      reduced[reduced_c] = h;
      reduced_len[reduced_c] = h_len;
      reduced_c++;
      if(TEST_OPT_PROT)
        Print ("%d ", h_len);
    }
  }
  int reduced_sum = 0;
  for(j = 0; j < reduced_c; j++)
  {
    reduced_sum += reduced_len[j];
  }
  poly *terms = (poly *) omalloc (reduced_sum * sizeof (poly));
  int tc = 0;
  for(j = 0; j < reduced_c; j++)
  {
    poly h = reduced[j];

    while(h != NULL)
    {
      terms[tc++] = h;
      pIter (h);
      assume (tc <= reduced_sum);
    }
  }
  assume (tc == reduced_sum);
  qsort (terms, reduced_sum, sizeof (poly), terms_sort_crit);
  int nterms = reduced_sum;
  //if (TEST_OPT_PROT)
  //Print("orig estimation:%i\n",reduced_sum);
  unify_terms (terms, nterms);
  //if (TEST_OPT_PROT)
  //    Print("actual number of columns:%i\n",nterms);
  int rank = reduced_c;
  linalg_step_modp (reduced, p, rank, terms, nterms, c);
  omFree (terms);

  pn = rank;
  omFree (reduced);

  if(TEST_OPT_PROT)
    PrintS ("\n");
}
#else

#endif
#endif
static void go_on (slimgb_alg * c)
{
  //set limit of 1000 for multireductions, at the moment for
  //programming reasons
#ifdef USE_NORO
  //Print("module rank%d\n",c->S->rank);
  const BOOLEAN use_noro = c->use_noro;
#else
  const BOOLEAN use_noro = FALSE;
#endif
  int i = 0;
  c->average_length = 0;
  for(i = 0; i < c->n; i++)
  {
    c->average_length += c->lengths[i];
  }
  c->average_length = c->average_length / c->n;
  i = 0;
  int max_pairs = bundle_size;

#ifdef USE_NORO
  if((use_noro) || (c->use_noro_last_block))
    max_pairs = bundle_size_noro;
#endif
  poly *p = (poly *) omAlloc ((max_pairs + 1) * sizeof (poly)); //nullterminated

  int curr_deg = -1;
  while(i < max_pairs)
  {
    sorted_pair_node *s = top_pair (c); //here is actually chain criterium done

    if(!s)
      break;

    if(curr_deg >= 0)
    {
      if(s->deg > curr_deg)
        break;
    }

    else
      curr_deg = s->deg;
    quick_pop_pair (c);
    if(s->i >= 0)
    {
      //be careful replace_pair use createShortSpoly which is not noncommutative
      now_t_rep (s->i, s->j, c);
      replace_pair (s->i, s->j, c);

      if(s->i == s->j)
      {
        free_sorted_pair_node (s, c->r);
        continue;
      }
      now_t_rep (s->i, s->j, c);
    }
    poly h;
    if(s->i >= 0)
    {
#ifdef HAVE_PLURAL
      if(c->nc)
      {
        h = nc_CreateSpoly (c->S->m[s->i], c->S->m[s->j] /*, NULL */ , c->r);

        if(h != NULL)
          p_Cleardenom (h, c->r);
      }
      else
#endif
        h = ksOldCreateSpoly (c->S->m[s->i], c->S->m[s->j], NULL, c->r);
      p_Test (h, c->r);
    }
    else
    {
      h = s->lcm_of_lm;
      p_Test (h, c->r);
    }
    // if(s->i>=0)
//       now_t_rep(s->j,s->i,c);
    number coef;
    int mlen = pLength (h);
    p_Test (h, c->r);
    if((!c->nc) & (!(use_noro)))
    {
      h = redNF2 (h, c, mlen, coef, 2);
      redTailShort (h, c->strat);
      nDelete (&coef);
    }
    p_Test (h, c->r);
    free_sorted_pair_node (s, c->r);
    if(!h)
      continue;

    if(TEST_OPT_IDLIFT
    && p_GetComp(h, currRing) > c->syz_comp)
    {
      pDelete(&h);
      continue;
    }

    p[i] = h;
    i++;
  }
  p[i] = NULL;
//  pre_comp(p,i,c);
  if(i == 0)
  {
    omFree (p);
    return;
  }
#ifdef TGB_RESORT_PAIRS
  c->replaced = new bool[c->n];
  c->used_b = FALSE;
#endif

  c->normal_forms += i;
  int j;
#ifdef USE_NORO
  //if ((!(c->nc))&&(rField_is_Zp(c->r)))
  //{
  if(use_noro)
  {
    int pn = i;
    if(pn == 0)
    {
      omFree (p);
      return;
    }
    {
      if(n_GetChar(currRing->cf) < 255)
      {
        noro_step < tgb_uint8 > (p, pn, c);
      }
      else
      {
        if(n_GetChar(currRing->cf) < 65000)
        {
          noro_step < tgb_uint16 > (p, pn, c);
        }
        else
        {
          noro_step < tgb_uint32 > (p, pn, c);
        }
      }
    }

    //if (TEST_OPT_PROT)
    //{
    //  Print("reported rank:%i\n",pn);
    //}
    mass_add (p, pn, c);
    omFree (p);
    return;
    /*if (TEST_OPT_PROT)
       for(j=0;j<pn;j++)
       {
       p_wrp(p[j],c->r);
       } */
  }
#endif
  red_object *buf = (red_object *) omAlloc (i * sizeof (red_object)); /*i>0*/
  for(j = 0; j < i; j++)
  {
    p_Test (p[j], c->r);
    buf[j].p = p[j];
    buf[j].sev = pGetShortExpVector (p[j]);
    buf[j].bucket = kBucketCreate (currRing);
    p_Test (p[j], c->r);
    int len = pLength (p[j]);
    kBucketInit (buf[j].bucket, p[j], len);
    buf[j].initial_quality = buf[j].guess_quality (c);
    assume (buf[j].initial_quality >= 0);
  }
  omFree (p);
  qsort (buf, i, sizeof (red_object), red_object_better_gen);
//    Print("\ncurr_deg:%i\n",curr_deg);
  if(TEST_OPT_PROT)
  {
    Print ("%dM[%d,", curr_deg, i);
  }

  multi_reduction (buf, i, c);
#ifdef TGB_RESORT_PAIRS
  if(c->used_b)
  {
    if(TEST_OPT_PROT)
      PrintS ("B");
    int e;
    for(e = 0; e <= c->pair_top; e++)
    {
      if(c->apairs[e]->i < 0)
        continue;
      assume (c->apairs[e]->j >= 0);
      if((c->replaced[c->apairs[e]->i]) || (c->replaced[c->apairs[e]->j]))
      {
        sorted_pair_node *s = c->apairs[e];
        s->expected_length = pair_weighted_length (s->i, s->j, c);
      }
    }
    qsort (c->apairs, c->pair_top + 1, sizeof (sorted_pair_node *),
           tgb_pair_better_gen2);
  }
#endif
#ifdef TGB_DEBUG
  {
    int k;
    for(k = 0; k < i; k++)
    {
      assume (kFindDivisibleByInS_easy (c->strat, buf[k]) < 0);
      int k2;
      for(k2 = 0; k2 < i; k2++)
      {
        if(k == k2)
          continue;
        assume ((!(p_LmDivisibleBy (buf[k].p, buf[k2].p, c->r)))
                || (wrp (buf[k].p), Print (" k %d k2 %d ", k, k2),
                    wrp (buf[k2].p), FALSE));
      }
    }
  }
#endif
  //resort S

  if(TEST_OPT_PROT)
    Print ("%i]", i);

  poly *add_those = (poly *) omalloc0 (i * sizeof (poly));
  int num_to_add=0;
  for(j = 0; j < i; j++)
  {
    int len;
    poly p;
    buf[j].flatten ();
    kBucketClear (buf[j].bucket, &p, &len);
    kBucketDestroy (&buf[j].bucket);
    p_Test (p, c->r);
    //if (!c->nc) {
    if((c->tailReductions) || (lies_in_last_dp_block (p, c)))
    {
      p = redNFTail (p, c->strat->sl, c->strat, 0);
    }
    else
    {
      p = redTailShort (p, c->strat);
    }
    //}
    p_Test (p, c->r);

    if (p!=NULL)
    {
      if (TEST_OPT_IDLIFT && (p_GetComp(p,currRing) > c->syz_comp))
      {
        p_Delete(&p,currRing);
      }
      else
      {
        add_those[num_to_add++] = p;
      }
    }

    //sbuf[j]=add_to_basis(p,-1,-1,c,ibuf+j);
  }
  mass_add (add_those, num_to_add, c);
  omfree (add_those);
  omFree (buf);

  if(TEST_OPT_PROT)
    Print ("(%d)", c->pair_top + 1);
  //TODO: implement that while(!(idIs0(c->add_later)))
#ifdef TGB_RESORT_PAIRS
  delete c->replaced;
  c->replaced = NULL;
  c->used_b = FALSE;
#endif
  return;
}

#ifdef REDTAIL_S

static poly redNFTail (poly h, const int sl, kStrategy strat, int len)
{
  if(h == NULL)
    return NULL;
  pTest (h);
  if(0 > sl)
    return h;
  if(pNext (h) == NULL)
    return h;
  BOOLEAN nc = rIsPluralRing (currRing);

  int j;
  poly res = h;
  poly act = res;
  LObject P (pNext (h));
  pNext (res) = NULL;
  P.bucket = kBucketCreate (currRing);
  len--;
  h = P.p;
  if(len <= 0)
    len = pLength (h);
  kBucketInit (P.bucket, h /*P.p */ , len /*pLength(P.p) */ );
  pTest (h);
  loop
  {
    P.p = h;
    P.t_p = NULL;
    P.SetShortExpVector ();
    loop
    {
      //int dummy=strat->sl;
      j = kFindDivisibleByInS_easy (strat, P.p, P.sev); //kFindDivisibleByInS(strat,&dummy,&P);
      if(j >= 0)
      {
#ifdef REDTAIL_PROT
        PrintS ("r");
#endif
        nNormalize (pGetCoeff (P.p));
#ifdef KDEBUG
        if(TEST_OPT_DEBUG)
        {
          PrintS ("red tail:");
          wrp (h);
          PrintS (" with ");
          wrp (strat->S[j]);
        }
#endif
        number coef;
        pTest (strat->S[j]);
#ifdef HAVE_PLURAL
        if(nc)
        {
          nc_kBucketPolyRed_Z (P.bucket, strat->S[j], &coef);
        }
        else
#endif
          coef = kBucketPolyRed (P.bucket, strat->S[j],
                                 strat->lenS[j] /*pLength(strat->S[j]) */ ,
                                 strat->kNoether);
        res=__p_Mult_nn (res, coef, currRing);
        nDelete (&coef);
        h = kBucketGetLm (P.bucket);
        if(h == NULL)
        {
#ifdef REDTAIL_PROT
          PrintS (" ");
#endif
          kBucketDestroy (&P.bucket);
          return res;
        }
        P.p = h;
        P.t_p = NULL;
        P.SetShortExpVector ();
#ifdef KDEBUG
        if(TEST_OPT_DEBUG)
        {
          PrintS ("\nto tail:");
          wrp (h);
          PrintLn ();
        }
#endif
      }
      else
      {
#ifdef REDTAIL_PROT
        PrintS ("n");
#endif
        break;
      }
    }                           /* end loop current mon */
    //   poly tmp=pHead(h /*kBucketGetLm(P.bucket)*/);
    //act->next=tmp;pIter(act);
    act->next = kBucketExtractLm (P.bucket);
    pIter (act);
    h = kBucketGetLm (P.bucket);
    if(h == NULL)
    {
#ifdef REDTAIL_PROT
      PrintS (" ");
#endif
      kBucketDestroy (&P.bucket);
      return res;
    }
    pTest (h);
  }
}
#endif


//try to fill, return FALSE iff queue is empty

//transfers ownership of m to mat
void init_with_mac_poly (tgb_sparse_matrix * mat, int row, mac_poly m)
{
  assume (mat->mp[row] == NULL);
  mat->mp[row] = m;
#ifdef TGB_DEBUG
  mac_poly r = m;
  while(r)
  {
    assume (r->exp < mat->columns);
    r = r->next;
  }
#endif
}

poly
free_row_to_poly (tgb_sparse_matrix * mat, int row, poly * monoms,
                  int monom_index)
{
  poly p = NULL;
  poly *set_this = &p;
  mac_poly r = mat->mp[row];
  mat->mp[row] = NULL;
  while(r)
  {
    (*set_this) = pLmInit (monoms[monom_index - 1 - r->exp]);
    pSetCoeff ((*set_this), r->coef);
    set_this = &((*set_this)->next);
    mac_poly old = r;
    r = r->next;
    delete old;

  }
  return p;
}

static int poly_crit (const void *ap1, const void *ap2)
{
  poly p1, p2;
  p1 = *((poly *) ap1);
  p2 = *((poly *) ap2);

  int c = pLmCmp (p1, p2);
  if(c != 0)
    return c;
  int l1 = pLength (p1);
  int l2 = pLength (p2);
  if(l1 < l2)
    return -1;
  if(l1 > l2)
    return 1;
  return 0;
}

void slimgb_alg::introduceDelayedPairs (poly * pa, int s)
{
  if(s == 0)
    return;
  sorted_pair_node **si_array =
    (sorted_pair_node **) omAlloc (s * sizeof (sorted_pair_node *));

  for(int i = 0; i < s; i++)
  {
    sorted_pair_node *si =
      (sorted_pair_node *) omAlloc (sizeof (sorted_pair_node));
    si->i = -1;
    si->j = -2;
    poly p = pa[i];
    simplify_poly (p, r);
    si->expected_length = pQuality (p, this, pLength (p));
    p_Test (p, r);
    si->deg = this->pTotaldegree_full (p);
    /*if (!rField_is_Zp(r))
       {
       p_Content(p,r);
       p_Cleardenom(p,r);
       } */

    si->lcm_of_lm = p;

    //      c->apairs[n-1-i]=si;
    si_array[i] = si;
  }

  qsort (si_array, s, sizeof (sorted_pair_node *), tgb_pair_better_gen2);
  apairs = spn_merge (apairs, pair_top + 1, si_array, s, this);
  pair_top += s;
  omFree (si_array);
}

slimgb_alg::slimgb_alg (ideal I, int syz_comp, BOOLEAN F4, int deg_pos)
{
  this->deg_pos = deg_pos;
  lastCleanedDeg = -1;
  completed = FALSE;
  this->syz_comp = syz_comp;
  r = currRing;
  nc = rIsPluralRing (r);
  this->lastDpBlockStart = get_last_dp_block_start (r);
  //Print("last dp Block start: %i\n", this->lastDpBlockStart);
  is_homog = TRUE;
  {
    int hzz;
    for(hzz = 0; hzz < IDELEMS (I); hzz++)
    {
      assume (I->m[hzz] != NULL);
      int d = this->pTotaldegree (I->m[hzz]);
      poly t = I->m[hzz]->next;
      while(t)
      {
        if(d != (int)this->pTotaldegree (t))
        {
          is_homog = FALSE;
          break;
        }
        t = t->next;
      }
      if(!(is_homog))
        break;
    }
  }
  eliminationProblem = ((!(is_homog)) && ((currRing->pLexOrder) || (I->rank > 1)));
  tailReductions = ((is_homog) || ((TEST_OPT_REDTAIL) && (!(I->rank > 1))));
  //  Print("is homog:%d",c->is_homog);
  void *h;
  int i;
  to_destroy = NULL;
  easy_product_crit = 0;
  extended_product_crit = 0;
  if(rField_is_Zp (r))
    isDifficultField = FALSE;
  else
    isDifficultField = TRUE;
  //not fully correct
  //(rChar()==0);
  F4_mode = F4;

  reduction_steps = 0;
  last_index = -1;

  F = NULL;
  F_minus = NULL;

  Rcounter = 0;

  soon_free = NULL;

  tmp_lm = pOne ();

  normal_forms = 0;
  current_degree = 1;

  max_pairs = 5 * IDELEMS (I);

  apairs =
    (sorted_pair_node **) omAlloc (sizeof (sorted_pair_node *) * max_pairs);
  pair_top = -1;

  int n = IDELEMS (I);
  array_lengths = n;


  i = 0;
  this->n = 0;
  T_deg = (int *) omAlloc (n * sizeof (int));
  if(eliminationProblem)
    T_deg_full = (int *) omAlloc (n * sizeof (int));
  else
    T_deg_full = NULL;
  tmp_pair_lm = (poly *) omAlloc (n * sizeof (poly));
  tmp_spn = (sorted_pair_node **) omAlloc (n * sizeof (sorted_pair_node *));
  lm_bin = omGetSpecBin (POLYSIZE + (r->ExpL_Size) * sizeof (long));
  /* omUnGetSpecBin(&(c->HeadBin)); */
#ifndef HAVE_BOOST
#ifdef USE_STDVECBOOL
#else
  h = omAlloc (n * sizeof (char *));

  states = (char **) h;
#endif
#endif
  h = omAlloc (n * sizeof (int));
  lengths = (int *) h;
  weighted_lengths = (wlen_type *) omAllocAligned (n * sizeof (wlen_type));
  gcd_of_terms = (poly *) omAlloc (n * sizeof (poly));

  short_Exps = (long *) omAlloc (n * sizeof (long));
  if(F4_mode)
    S = idInit (n, I->rank);
  else
    S = idInit (1, I->rank);
  strat = new skStrategy;
  if(eliminationProblem)
    strat->honey = TRUE;
  strat->syzComp = syz_comp;
  initBuchMoraCrit (strat);
  initBuchMoraPos (strat);
  strat->initEcart = initEcartBBA;
  strat->tailRing = r;
  strat->enterS = enterSBba;
  strat->sl = -1;
  i = n;
  i = 1;                        //some strange bug else
  /* initS(c->S,NULL,c->strat); */
  /* intS start: */
  // i=((i+IDELEMS(c->S)+15)/16)*16;
  strat->ecartS = (intset) omAlloc (i * sizeof (int));  /*initec(i); */
  strat->sevS = (unsigned long *) omAlloc0 (i * sizeof (unsigned long));
  /*initsevS(i); */
  strat->S_2_R = (int *) omAlloc0 (i * sizeof (int));   /*initS_2_R(i); */
  strat->fromQ = NULL;
  strat->Shdl = idInit (1, 1);
  strat->S = strat->Shdl->m;
  strat->lenS = (int *) omAlloc0 (i * sizeof (int));
  if((isDifficultField) || (eliminationProblem))
    strat->lenSw = (wlen_type *) omAlloc0 (i * sizeof (wlen_type));
  else
    strat->lenSw = NULL;
  assume (n > 0);
  add_to_basis_ideal_quotient (I->m[0], this, NULL);

  assume (strat->sl == IDELEMS (strat->Shdl) - 1);
  if(!(F4_mode))
  {
    poly *array_arg = I->m;
    array_arg++;
    introduceDelayedPairs (array_arg, n - 1);
    /*
       for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
       {
       //     add_to_basis(I->m[i],-1,-1,c);
       si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
       si->i=-1;
       si->j=-2;
       si->expected_length=pQuality(I->m[i],this,pLength(I->m[i]));
       si->deg=pTotaldegree(I->m[i]);
       if (!rField_is_Zp(r))
       {
       p_Cleardenom(I->m[i], r);
       }
       si->lcm_of_lm=I->m[i];

       //      c->apairs[n-1-i]=si;
       apairs[n-i-1]=si;
       ++(pair_top);
       } */
  }
  else
  {
    for(i = 1; i < n; i++)      //the 1 is wanted, because first element is added to basis
      add_to_basis_ideal_quotient (I->m[i], this, NULL);
  }
  for(i = 0; i < IDELEMS (I); i++)
  {
    I->m[i] = NULL;
  }
  idDelete (&I);
  add_later = idInit (ADD_LATER_SIZE, S->rank);
#ifdef USE_NORO
  use_noro = ((!(nc)) && (S->rank <= 1) && (rField_is_Zp (r))
              && (!(eliminationProblem)) && (n_GetChar(currRing->cf) <= NV_MAX_PRIME));
  use_noro_last_block = false;
  if((!(use_noro)) && (lastDpBlockStart <= (currRing->N)))
  {
    use_noro_last_block = ((!(nc)) && (S->rank <= 1) && (rField_is_Zp (r))
                           && (n_GetChar(currRing->cf) <= NV_MAX_PRIME));
  }
#else
  use_noro = false;
  use_noro_last_block = false;
#endif
  //Print("NORO last block %i",use_noro_last_block);
  memset (add_later->m, 0, ADD_LATER_SIZE * sizeof (poly));
}

slimgb_alg::~slimgb_alg ()
{

  if(!(completed))
  {
    poly *add = (poly *) omAlloc ((pair_top + 2) * sizeof (poly));
    int piter;
    int pos = 0;
    for(piter = 0; piter <= pair_top; piter++)
    {
      sorted_pair_node *s = apairs[piter];
      if(s->i < 0)
      {
        //delayed element
        if(s->lcm_of_lm != NULL)
        {
          add[pos] = s->lcm_of_lm;
          pos++;
        }
      }
      free_sorted_pair_node (s, r);
      apairs[piter] = NULL;
    }
    pair_top = -1;
    add[pos] = NULL;
    pos = 0;
    while(add[pos] != NULL)
    {
      add_to_basis_ideal_quotient (add[pos], this, NULL);
      pos++;
    }
    for(piter = 0; piter <= pair_top; piter++)
    {
      sorted_pair_node *s = apairs[piter];
      assume (s->i >= 0);
      free_sorted_pair_node (s, r);
      apairs[piter] = NULL;
    }
    pair_top = -1;
  }
  id_Delete (&add_later, r);
  int i, j;
  slimgb_alg *c = this;
  while(c->to_destroy)
  {
    pDelete (&(c->to_destroy->p));
    poly_list_node *old = c->to_destroy;
    c->to_destroy = c->to_destroy->next;
    omFree (old);
  }
  while(c->F)
  {
    for(i = 0; i < c->F->size; i++)
    {
      pDelete (&(c->F->mp[i].m));
    }
    omFree (c->F->mp);
    c->F->mp = NULL;
    mp_array_list *old = c->F;
    c->F = c->F->next;
    omFree (old);
  }
  while(c->F_minus)
  {
    for(i = 0; i < c->F_minus->size; i++)
    {
      pDelete (&(c->F_minus->p[i]));
    }
    omFree (c->F_minus->p);
    c->F_minus->p = NULL;
    poly_array_list *old = c->F_minus;
    c->F_minus = c->F_minus->next;
    omFree (old);
  }
#ifndef HAVE_BOOST
#ifndef USE_STDVECBOOL
  for(int z = 1 /* zero length at 0 */ ; z < c->n; z++)
  {
    omFree (c->states[z]);
  }
  omFree (c->states);
#endif
#endif

  omFree (c->lengths);
  omFree (c->weighted_lengths);
  for(int z = 0; z < c->n; z++)
  {
    pDelete (&c->tmp_pair_lm[z]);
    omFree (c->tmp_spn[z]);
  }
  omFree (c->tmp_pair_lm);
  omFree (c->tmp_spn);

  omFree (c->T_deg);
  omfree (c->T_deg_full); /*c->T_deg_full my be NULL*/

  omFree (c->strat->ecartS);
  omFree (c->strat->sevS);
//   initsevS(i);
  omFree (c->strat->S_2_R);


  omFree (c->strat->lenS);

  if(c->strat->lenSw)
    omFree (c->strat->lenSw);

  for(i = 0; i < c->n; i++)
  {
    if(c->gcd_of_terms[i])
      pDelete (&(c->gcd_of_terms[i]));
  }
  omFree (c->gcd_of_terms);

  omFree (c->apairs);
  if(TEST_OPT_PROT)
  {
    //Print("calculated %d NFs\n",c->normal_forms);
    Print ("\nNF:%i product criterion:%i, ext_product criterion:%i \n",
           c->normal_forms, c->easy_product_crit, c->extended_product_crit);
  }

  for(i = 0; i <= c->strat->sl; i++)
  {
    if(!c->strat->S[i])
      continue;
    BOOLEAN found = FALSE;
    for(j = 0; j < c->n; j++)
    {
      if(c->S->m[j] == c->strat->S[i])
      {
        found = TRUE;
        break;
      }
    }
    if(!found)
      pDelete (&c->strat->S[i]);
  }
//   for(i=0;i<c->n;i++)
//   {
//     if (c->rep[i]!=i)
//     {
// //       for(j=0;j<=c->strat->sl;j++)
// {
// //   if(c->strat->S[j]==c->S->m[i])
// {
// //     c->strat->S[j]=NULL;
// //     break;
// //   }
// //       }
// //      PrintS("R_delete");
//       pDelete(&c->S->m[i]);
//     }
//   }

  if(completed)
  {
    for(i = 0; i < c->n; i++)
    {
      assume (c->S->m[i] != NULL);
      if(p_GetComp (c->S->m[i], currRing) > this->syz_comp)
        continue;
      for(j = 0; j < c->n; j++)
      {
        if((c->S->m[j] == NULL) || (i == j))
          continue;
        assume (p_LmShortDivisibleBy (c->S->m[j], c->short_Exps[j],
                                      c->S->m[i], ~c->short_Exps[i],
                                      c->r) == p_LmDivisibleBy (c->S->m[j],
                                                                c->S->m[i],
                                                                c->r));
        if(p_LmShortDivisibleBy (c->S->m[j], c->short_Exps[j],
                                 c->S->m[i], ~c->short_Exps[i], c->r))
        {
          pDelete (&c->S->m[i]);
          break;
        }
      }
    }
  }
  omFree (c->short_Exps);

  ideal I = c->S;
  IDELEMS (I) = c->n;
  idSkipZeroes (I);
  for(i = 0; i <= c->strat->sl; i++)
    c->strat->S[i] = NULL;
  id_Delete (&c->strat->Shdl, c->r);
  pDelete (&c->tmp_lm);
  omUnGetSpecBin (&lm_bin);
  delete c->strat;
}

ideal t_rep_gb (const ring r, ideal arg_I, int syz_comp, BOOLEAN F4_mode)
{
  assume (r == currRing);
  ring orig_ring = r;
  int pos;
  ring new_ring = rAssure_TDeg (orig_ring, pos);
  ideal s_h;
  if(orig_ring != new_ring)
  {
    rChangeCurrRing (new_ring);
    s_h = idrCopyR_NoSort (arg_I, orig_ring, new_ring);
    /*int i;
       for(i=0;i<IDELEMS(s_h);i++)
       {
       poly p=s_h->m[i];
       while(p)
       {
       p_Setm(p,new_ring);
       pIter(p);
       }
       } */
  }
  else
  {
    s_h = id_Copy (arg_I, orig_ring);
  }
  idTest (s_h);

  ideal s_result = do_t_rep_gb (new_ring, s_h, syz_comp, F4_mode, pos);
  ideal result;
  if(orig_ring != new_ring)
  {
    idTest (s_result);
    rChangeCurrRing (orig_ring);
    result = idrMoveR_NoSort (s_result, new_ring, orig_ring);

    idTest (result);
    //rChangeCurrRing(new_ring);
    rDelete(new_ring);
    //rChangeCurrRing(orig_ring);
  }
  else
    result = s_result;
  idTest (result);
  return result;
}

ideal
do_t_rep_gb (ring /*r*/, ideal arg_I, int syz_comp, BOOLEAN F4_mode, int deg_pos)
{
  //  Print("QlogSize(0) %d, QlogSize(1) %d,QlogSize(-2) %d, QlogSize(5) %d\n", QlogSize(nlInit(0)),QlogSize(nlInit(1)),QlogSize(nlInit(-2)),QlogSize(nlInit(5)));

  if(TEST_OPT_PROT)
    if(F4_mode)
      PrintS ("F4 Modus\n");

  //debug_Ideal=arg_debug_Ideal;
  //if (debug_Ideal) PrintS("DebugIdeal received\n");
  // Print("Idelems %i \n----------\n",IDELEMS(arg_I));
  ideal I = arg_I;
  id_Compactify (I,currRing);
  if(idIs0 (I))
    return I;
  int i;
  for(i = 0; i < IDELEMS (I); i++)
  {
    assume (I->m[i] != NULL);
    simplify_poly (I->m[i], currRing);
  }

  qsort (I->m, IDELEMS (I), sizeof (poly), poly_crit);
  //Print("Idelems %i \n----------\n",IDELEMS(I));
  //slimgb_alg* c=(slimgb_alg*) omalloc(sizeof(slimgb_alg));
  //int syz_comp=arg_I->rank;
  slimgb_alg *c = new slimgb_alg (I, syz_comp, F4_mode, deg_pos);

  while((c->pair_top >= 0)
        && ((!(TEST_OPT_DEGBOUND))
            || (c->apairs[c->pair_top]->deg <= Kstd1_deg)))
  {
#ifdef HAVE_F4
    if(F4_mode)
      go_on_F4 (c);
    else
#endif
      go_on (c);
  }
  if(c->pair_top < 0)
    c->completed = TRUE;
  I = c->S;
  delete c;
  if(TEST_OPT_REDSB)
  {
    ideal erg = kInterRed (I, NULL);
    assume (I != erg);
    id_Delete (&I, currRing);
    return erg;
  }
  //qsort(I->m, IDELEMS(I),sizeof(poly),pLmCmp_func);
  assume (I->rank >= id_RankFreeModule (I,currRing));
  return (I);
}

void now_t_rep (const int &arg_i, const int &arg_j, slimgb_alg * c)
{
  int i, j;
  if(arg_i == arg_j)
  {
    return;
  }
  if(arg_i > arg_j)
  {
    i = arg_j;
    j = arg_i;
  }
  else
  {
    i = arg_i;
    j = arg_j;
  }
  c->states[j][i] = HASTREP;
}

static BOOLEAN
has_t_rep (const int &arg_i, const int &arg_j, slimgb_alg * state)
{
  assume (0 <= arg_i);
  assume (0 <= arg_j);
  assume (arg_i < state->n);
  assume (arg_j < state->n);
  if(arg_i == arg_j)
  {
    return (TRUE);
  }
  if(arg_i > arg_j)
  {
    return (state->states[arg_i][arg_j] == HASTREP);
  }
  else
  {
    return (state->states[arg_j][arg_i] == HASTREP);
  }
}

static void shorten_tails (slimgb_alg * c, poly monom)
{
  return;
// BOOLEAN corr=lenS_correct(c->strat);
  for(int i = 0; i < c->n; i++)
  {
    //enter tail

    if(c->S->m[i] == NULL)
      continue;
    poly tail = c->S->m[i]->next;
    poly prev = c->S->m[i];
    BOOLEAN did_something = FALSE;
    while((tail != NULL) && (pLmCmp (tail, monom) >= 0))
    {
      if(p_LmDivisibleBy (monom, tail, c->r))
      {
        did_something = TRUE;
        prev->next = tail->next;
        tail->next = NULL;
        p_Delete (&tail, c->r);
        tail = prev;
        //PrintS("Shortened");
        c->lengths[i]--;
      }
      prev = tail;
      tail = tail->next;
    }
    if(did_something)
    {
      int new_pos;
      wlen_type q;
      q = pQuality (c->S->m[i], c, c->lengths[i]);
      new_pos = simple_posInS (c->strat, c->S->m[i], c->lengths[i], q);

      int old_pos = -1;
      //assume new_pos<old_pos
      for(int z = 0; z <= c->strat->sl; z++)
      {
        if(c->strat->S[z] == c->S->m[i])
        {
          old_pos = z;
          break;
        }
      }
      if(old_pos == -1)
        for(int z = new_pos - 1; z >= 0; z--)
        {
          if(c->strat->S[z] == c->S->m[i])
          {
            old_pos = z;
            break;
          }
        }
      assume (old_pos >= 0);
      assume (new_pos <= old_pos);
      assume ((int)pLength (c->strat->S[old_pos]) == c->lengths[i]);
      c->strat->lenS[old_pos] = c->lengths[i];
      if(c->strat->lenSw)
        c->strat->lenSw[old_pos] = q;
      if(new_pos < old_pos)
        move_forward_in_S (old_pos, new_pos, c->strat);
      length_one_crit (c, i, c->lengths[i]);
    }
  }
}

#if 0                           // currently unused
static sorted_pair_node *pop_pair (slimgb_alg * c)
{
  clean_top_of_pair_list (c);

  if(c->pair_top < 0)
    return NULL;
  else
    return (c->apairs[c->pair_top--]);
}
#endif

void slimgb_alg::cleanDegs (int lower, int upper)
{
  assume (is_homog);
  int deg;
  if(TEST_OPT_PROT)
  {
    PrintS ("C");
  }
  for(deg = lower; deg <= upper; deg++)
  {
    int i;
    for(i = 0; i < n; i++)
    {
      if(T_deg[i] == deg)
      {
        poly h;
        h = S->m[i];
        h = redNFTail (h, strat->sl, strat, lengths[i]);
        if(TEST_OPT_INTSTRATEGY)
        {
          p_Cleardenom (h, r); //includes p_Content(h,r);
        }
        else
          pNorm (h);
        //TODO:GCD of TERMS
        poly got =::gcd_of_terms (h, r);
        p_Delete (&gcd_of_terms[i], r);
        gcd_of_terms[i] = got;
        int len = pLength (h);
        wlen_type wlen = pQuality (h, this, len);
        if(weighted_lengths)
          weighted_lengths[i] = wlen;
        lengths[i] = len;
        assume (h == S->m[i]);
        int j;
        for(j = 0; j <= strat->sl; j++)
        {
          if(h == strat->S[j])
          {
            int new_pos = simple_posInS (strat, h, len, wlen);
            if(strat->lenS)
            {
              strat->lenS[j] = len;
            }
            if(strat->lenSw)
            {
              strat->lenSw[j] = wlen;
            }
            if(new_pos < j)
            {
              move_forward_in_S (j, new_pos, strat);
            }
            else
            {
              if(new_pos > j)
                new_pos = new_pos - 1;  //is identical with one element
              if(new_pos > j)
                move_backward_in_S (j, new_pos, strat);
            }
            break;
          }
        }
      }
    }
  }
  {
    int i, j;
    for(i = 0; i < this->n; i++)
    {
      for(j = 0; j < i; j++)
      {
        if(T_deg[i] + T_deg[j] <= upper)
        {
          now_t_rep (i, j, this);
        }
      }
    }
  }
  //TODO resort and update strat->S,strat->lenSw
  //TODO mark pairs
}

sorted_pair_node *top_pair (slimgb_alg * c)
{
  while(c->pair_top >= 0)
  {
    super_clean_top_of_pair_list (c);   //yeah, I know, it's odd that I use a different proc here
    if((c->is_homog) && (c->pair_top >= 0)
       && (c->apairs[c->pair_top]->deg >= c->lastCleanedDeg + 2))
    {
      int upper = c->apairs[c->pair_top]->deg - 1;
      c->cleanDegs (c->lastCleanedDeg + 1, upper);
      c->lastCleanedDeg = upper;
    }
    else
    {
      break;
    }
  }

  if(c->pair_top < 0)
    return NULL;
  else
    return (c->apairs[c->pair_top]);
}

sorted_pair_node *quick_pop_pair (slimgb_alg * c)
{
  if(c->pair_top < 0)
    return NULL;
  else
    return (c->apairs[c->pair_top--]);
}

static void super_clean_top_of_pair_list (slimgb_alg * c)
{
  while((c->pair_top >= 0)
        && (c->apairs[c->pair_top]->i >= 0)
        &&
        (good_has_t_rep
         (c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i, c)))
  {
    free_sorted_pair_node (c->apairs[c->pair_top], c->r);
    c->pair_top--;
  }
}

void clean_top_of_pair_list (slimgb_alg * c)
{
  while((c->pair_top >= 0) && (c->apairs[c->pair_top]->i >= 0)
        &&
        (!state_is
         (UNCALCULATED, c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,
          c)))
  {
    free_sorted_pair_node (c->apairs[c->pair_top], c->r);
    c->pair_top--;
  }
}

static BOOLEAN
state_is (calc_state state, const int &arg_i, const int &arg_j,
          slimgb_alg * c)
{
  assume (0 <= arg_i);
  assume (0 <= arg_j);
  assume (arg_i < c->n);
  assume (arg_j < c->n);
  if(arg_i == arg_j)
  {
    return (TRUE);
  }
  if(arg_i > arg_j)
  {
    return (c->states[arg_i][arg_j] == state);
  }
  else
    return (c->states[arg_j][arg_i] == state);
}

void free_sorted_pair_node (sorted_pair_node * s, const ring r)
{
  if(s->i >= 0)
    p_Delete (&s->lcm_of_lm, r);
  omFree (s);
}

static BOOLEAN
pair_better (sorted_pair_node * a, sorted_pair_node * b, slimgb_alg * /*c*/)
{
  if(a->deg < b->deg)
    return TRUE;
  if(a->deg > b->deg)
    return FALSE;

  int comp = pLmCmp (a->lcm_of_lm, b->lcm_of_lm);
  if(comp == 1)
    return FALSE;
  if(-1 == comp)
    return TRUE;
  if(a->expected_length < b->expected_length)
    return TRUE;
  if(a->expected_length > b->expected_length)
    return FALSE;
  if(a->i + a->j < b->i + b->j)
    return TRUE;
  if(a->i + a->j > b->i + b->j)
    return FALSE;
  if(a->i < b->i)
    return TRUE;
  if(a->i > b->i)
    return FALSE;
  return TRUE;
}

static int tgb_pair_better_gen (const void *ap, const void *bp)
{
  sorted_pair_node *a = *((sorted_pair_node **) ap);
  sorted_pair_node *b = *((sorted_pair_node **) bp);
  assume ((a->i > a->j) || (a->i < 0));
  assume ((b->i > b->j) || (b->i < 0));
  if(a->deg < b->deg)
    return -1;
  if(a->deg > b->deg)
    return 1;

  int comp = pLmCmp (a->lcm_of_lm, b->lcm_of_lm);

  if(comp == 1)
    return 1;
  if(-1 == comp)
    return -1;
  if(a->expected_length < b->expected_length)
    return -1;
  if(a->expected_length > b->expected_length)
    return 1;
  if(a->i + a->j < b->i + b->j)
    return -1;
  if(a->i + a->j > b->i + b->j)
    return 1;
  if(a->i < b->i)
    return -1;
  if(a->i > b->i)
    return 1;
  return 0;
}

static poly gcd_of_terms (poly p, ring r)
{
  int max_g_0 = 0;
  assume (p != NULL);
  int i;
  poly m = pOne ();
  poly t;
  for(i = (currRing->N); i; i--)
  {
    pSetExp (m, i, pGetExp (p, i));
    if(max_g_0 == 0)
      if(pGetExp (m, i) > 0)
        max_g_0 = i;
  }

  t = p->next;
  while(t != NULL)
  {
    if(max_g_0 == 0)
      break;
    for(i = max_g_0; i; i--)
    {
      pSetExp (m, i, si_min (pGetExp (t, i), pGetExp (m, i)));
      if(max_g_0 == i)
        if(pGetExp (m, i) == 0)
          max_g_0 = 0;
      if((max_g_0 == 0) && (pGetExp (m, i) > 0))
      {
        max_g_0 = i;
      }
    }
    t = t->next;
  }
  p_Setm (m, r);
  if(max_g_0 > 0)
    return m;
  pDelete (&m);
  return NULL;
}

static inline BOOLEAN pHasNotCFExtended (poly p1, poly p2, poly m)
{

  if(pGetComp (p1) > 0 || pGetComp (p2) > 0)
    return FALSE;
  int i = 1;
  loop
  {
    if((pGetExp (p1, i) - pGetExp (m, i) > 0)
       && (pGetExp (p2, i) - pGetExp (m, i) > 0))
      return FALSE;
    if(i == (currRing->N))
      return TRUE;
    i++;
  }
}

//for impl reasons may return false if the the normal product criterion matches
static inline BOOLEAN
extended_product_criterion (poly p1, poly gcd1, poly p2, poly gcd2,
                            slimgb_alg * c)
{
  if(c->nc)
    return FALSE;
  if(gcd1 == NULL)
    return FALSE;
  if(gcd2 == NULL)
    return FALSE;
  gcd1->next = gcd2;            //may ordered incorrect
  poly m = gcd_of_terms (gcd1, c->r);
  gcd1->next = NULL;
  if(m == NULL)
    return FALSE;

  BOOLEAN erg = pHasNotCFExtended (p1, p2, m);
  pDelete (&m);
  return erg;
}

#if 0                           //currently unused
static poly kBucketGcd (kBucket * b, ring r)
{
  int s = 0;
  int i;
  poly m, n;
  BOOLEAN initialized = FALSE;
  for(i = MAX_BUCKET - 1; i >= 0; i--)
  {
    if(b->buckets[i] != NULL)
    {
      if(!initialized)
      {
        m = gcd_of_terms (b->buckets[i], r);
        initialized = TRUE;
        if(m == NULL)
          return NULL;
      }
      else
      {
        n = gcd_of_terms (b->buckets[i], r);
        if(n == NULL)
        {
          pDelete (&m);
          return NULL;
        }
        n->next = m;
        poly t = gcd_of_terms (n, r);
        n->next = NULL;
        pDelete (&m);
        pDelete (&n);
        m = t;
        if(m == NULL)
          return NULL;

      }
    }
  }
  return m;
}
#endif

static inline wlen_type quality_of_pos_in_strat_S (int pos, slimgb_alg * c)
{
  if(c->strat->lenSw != NULL)
    return c->strat->lenSw[pos];
  return c->strat->lenS[pos];
}

#ifdef HAVE_PLURAL
static inline wlen_type
quality_of_pos_in_strat_S_mult_high (int pos, poly high, slimgb_alg * c)
  //meant only for nc
{
  poly m = pOne ();
  pExpVectorDiff (m, high, c->strat->S[pos]);
  poly product = nc_mm_Mult_pp (m, c->strat->S[pos], c->r);
  wlen_type erg = pQuality (product, c);
  pDelete (&m);
  pDelete (&product);
  return erg;
}
#endif

static void
multi_reduction_lls_trick (red_object * los, int /*losl*/, slimgb_alg * c,
                           find_erg & erg)
{
  erg.expand = NULL;
  BOOLEAN swap_roles;           //from reduce_by, to_reduce_u if fromS
  if(erg.fromS)
  {
    if(pLmEqual (c->strat->S[erg.reduce_by], los[erg.to_reduce_u].p))
    {
      wlen_type quality_a = quality_of_pos_in_strat_S (erg.reduce_by, c);
      int best = erg.to_reduce_u + 1;
/*
      for (i=erg.to_reduce_u;i>=erg.to_reduce_l;i--)
      {
        int qc=los[i].guess_quality(c);
        if (qc<quality_a)
        {
          best=i;
          quality_a=qc;
        }
      }
      if(best!=erg.to_reduce_u+1)
      {*/
      wlen_type qc;
      best = find_best (los, erg.to_reduce_l, erg.to_reduce_u, qc, c);
      if(qc < quality_a)
      {
        los[best].flatten ();
        int b_pos = kBucketCanonicalize (los[best].bucket);
        los[best].p = los[best].bucket->buckets[b_pos];
        qc = pQuality (los[best].bucket->buckets[b_pos], c);
        if(qc < quality_a)
        {
          red_object h = los[erg.to_reduce_u];
          los[erg.to_reduce_u] = los[best];
          los[best] = h;
          swap_roles = TRUE;
        }
        else
          swap_roles = FALSE;
      }
      else
      {
        swap_roles = FALSE;
      }
    }
    else
    {
      if(erg.to_reduce_u > erg.to_reduce_l)
      {
        wlen_type quality_a = quality_of_pos_in_strat_S (erg.reduce_by, c);
#ifdef HAVE_PLURAL
        if((c->nc) && (!(rIsSCA (c->r))))
          quality_a =
            quality_of_pos_in_strat_S_mult_high (erg.reduce_by,
                                                 los[erg.to_reduce_u].p, c);
#endif
        int best = erg.to_reduce_u + 1;
        wlen_type qc;
        best = find_best (los, erg.to_reduce_l, erg.to_reduce_u, qc, c);
        assume (qc == los[best].guess_quality (c));
        if(qc < quality_a)
        {
          los[best].flatten ();
          int b_pos = kBucketCanonicalize (los[best].bucket);
          los[best].p = los[best].bucket->buckets[b_pos];
          qc = pQuality (los[best].bucket->buckets[b_pos], c);
          //(best!=erg.to_reduce_u+1)
          if(qc < quality_a)
          {
            red_object h = los[erg.to_reduce_u];
            los[erg.to_reduce_u] = los[best];
            los[best] = h;
            erg.reduce_by = erg.to_reduce_u;
            erg.fromS = FALSE;
            erg.to_reduce_u--;
          }
        }
      }
      else
      {
        assume (erg.to_reduce_u == erg.to_reduce_l);
        wlen_type quality_a = quality_of_pos_in_strat_S (erg.reduce_by, c);
        wlen_type qc = los[erg.to_reduce_u].guess_quality (c);
        if(qc < 0)
          PrintS ("Wrong wlen_type");
        if(qc < quality_a)
        {
          int best = erg.to_reduce_u;
          los[best].flatten ();
          int b_pos = kBucketCanonicalize (los[best].bucket);
          los[best].p = los[best].bucket->buckets[b_pos];
          qc = pQuality (los[best].bucket->buckets[b_pos], c);
          assume (qc >= 0);
          if(qc < quality_a)
          {
            BOOLEAN exp = FALSE;
            if(qc <= 2)
            {
              //Print("\n qc is %lld \n",qc);
              exp = TRUE;
            }
            else
            {
              if(qc < quality_a / 2)
                exp = TRUE;
              else if(erg.reduce_by < c->n / 4)
                exp = TRUE;
            }
            if(exp)
            {
              poly clear_into;
              los[erg.to_reduce_u].flatten ();
              kBucketClear (los[erg.to_reduce_u].bucket, &clear_into,
                            &erg.expand_length);
              erg.expand = pCopy (clear_into);
              kBucketInit (los[erg.to_reduce_u].bucket, clear_into,
                           erg.expand_length);
              if(TEST_OPT_PROT)
                PrintS ("e");
            }
          }
        }
      }

      swap_roles = FALSE;
      return;
    }
  }
  else
  {
    if(erg.reduce_by > erg.to_reduce_u)
    {
      //then lm(rb)>= lm(tru) so =
      assume (erg.reduce_by == erg.to_reduce_u + 1);
      int best = erg.reduce_by;
      wlen_type quality_a = los[erg.reduce_by].guess_quality (c);
      wlen_type qc;
      best = find_best (los, erg.to_reduce_l, erg.to_reduce_u, qc, c);

      if(qc < quality_a)
      {
        red_object h = los[erg.reduce_by];
        los[erg.reduce_by] = los[best];
        los[best] = h;
      }
      swap_roles = FALSE;
      return;
    }
    else
    {
      assume (!pLmEqual (los[erg.reduce_by].p, los[erg.to_reduce_l].p));
      assume (erg.to_reduce_u == erg.to_reduce_l);
      //further assume, that reduce_by is the above all other polys
      //with same leading term
      int il = erg.reduce_by;
      wlen_type quality_a = los[erg.reduce_by].guess_quality (c);
      wlen_type qc;
      while((il > 0) && pLmEqual (los[il - 1].p, los[il].p))
      {
        il--;
        qc = los[il].guess_quality (c);
        if(qc < quality_a)
        {
          quality_a = qc;
          erg.reduce_by = il;
        }
      }
      swap_roles = FALSE;
    }
  }
  if(swap_roles)
  {
    if(TEST_OPT_PROT)
      PrintS ("b");
    poly clear_into;
    int new_length;
    int bp = erg.to_reduce_u;   //bucket_positon
    //kBucketClear(los[bp].bucket,&clear_into,&new_length);
    new_length = los[bp].clear_to_poly ();
    clear_into = los[bp].p;
    poly p = c->strat->S[erg.reduce_by];
    int j = erg.reduce_by;
    int old_length = c->strat->lenS[j]; // in view of S
    los[bp].p = p;
    kBucketInit (los[bp].bucket, p, old_length);
    wlen_type qal = pQuality (clear_into, c, new_length);
    int pos_in_c = -1;
    int z;
    int new_pos;
    new_pos = simple_posInS (c->strat, clear_into, new_length, qal);
    assume (new_pos <= j);
    for(z = c->n; z; z--)
    {
      if(p == c->S->m[z - 1])
      {
        pos_in_c = z - 1;
        break;
      }
    }

    int tdeg_full = -1;
    int tdeg = -1;
    if(pos_in_c >= 0)
    {
#ifdef TGB_RESORT_PAIRS
      c->used_b = TRUE;
      c->replaced[pos_in_c] = TRUE;
#endif
      tdeg = c->T_deg[pos_in_c];
      c->S->m[pos_in_c] = clear_into;
      c->lengths[pos_in_c] = new_length;
      c->weighted_lengths[pos_in_c] = qal;
      if(c->gcd_of_terms[pos_in_c] == NULL)
        c->gcd_of_terms[pos_in_c] = gcd_of_terms (clear_into, c->r);
      if(c->T_deg_full)
        tdeg_full = c->T_deg_full[pos_in_c] =
          c->pTotaldegree_full (clear_into);
      else
        tdeg_full = tdeg;
      c_S_element_changed_hook (pos_in_c, c);
    }
    else
    {
      if(c->eliminationProblem)
      {
        tdeg_full = c->pTotaldegree_full (clear_into);
        tdeg = c->pTotaldegree (clear_into);
      }
    }
    c->strat->S[j] = clear_into;
    c->strat->lenS[j] = new_length;

    assume ((int)pLength (clear_into) == new_length);
    if(c->strat->lenSw != NULL)
      c->strat->lenSw[j] = qal;
    if(TEST_OPT_INTSTRATEGY)
    {
      p_Cleardenom (clear_into, c->r);  //should be unnecessary
      //includes p_Content(clear_into, c->r);
    }
    else
      pNorm (clear_into);
#ifdef FIND_DETERMINISTIC
    erg.reduce_by = j;
    //resort later see diploma thesis, find_in_S must be deterministic
    //during multireduction if spolys are only in the span of the
    //input polys
#else
    if(new_pos < j)
    {
      if(c->strat->honey)
        c->strat->ecartS[j] = tdeg_full - tdeg;
      move_forward_in_S (j, new_pos, c->strat);
      erg.reduce_by = new_pos;
    }
#endif
  }
}

static int fwbw (red_object * los, int i)
{
  int i2 = i;
  int step = 1;

  BOOLEAN bw = FALSE;
  BOOLEAN incr = TRUE;

  while(1)
  {
    if(!bw)
    {
      if (i2 < step) step=i2;
      if(step == 0)
        break;
      i2 -= step;

      if(!pLmEqual (los[i].p, los[i2].p))
      {
        bw = TRUE;
        incr = FALSE;
      }
      else
      {
        if((!incr) && (step == 1))
          break;
      }
    }
    else
    {
      step = si_min (i - i2, step);
      if(step == 0)
        break;
      i2 += step;
      if(pLmEqual (los[i].p, los[i2].p))
      {
        if(step == 1)
          break;
        else
        {
          bw = FALSE;
        }
      }
    }
    if(incr)
      step *= 2;
    else
    {
      if(step % 2 == 1)
        step = (step + 1) / 2;
      else
        step /= 2;
    }
  }
  return i2;
}

static void
canonicalize_region (red_object * los, int l, int u, slimgb_alg * /*c*/)
{
  assume (l <= u + 1);
  int i;
  for(i = l; i <= u; i++)
  {
    kBucketCanonicalize (los[i].bucket);
  }
}

#ifdef SING_NDEBUG
static void
multi_reduction_find (red_object * los, int /*losl*/, slimgb_alg * c, int startf,
                      find_erg & erg)
#else
static void
multi_reduction_find (red_object * los, int losl, slimgb_alg * c, int startf,
                      find_erg & erg)
#endif
{
  kStrategy strat = c->strat;

  #ifndef SING_NDEBUG
  assume (startf <= losl);
  assume ((startf == losl - 1)
          || (pLmCmp (los[startf].p, los[startf + 1].p) == -1));
  #endif
  int i = startf;

  int j;
  while(i >= 0)
  {
    #ifndef SING_NDEBUG
    assume ((i == losl - 1) || (pLmCmp (los[i].p, los[i + 1].p) <= 0));
    #endif
    assume (is_valid_ro (los[i]));
    j = kFindDivisibleByInS_easy (strat, los[i]);
    if(j >= 0)
    {
      erg.to_reduce_u = i;
      erg.reduce_by = j;
      erg.fromS = TRUE;
      int i2 = fwbw (los, i);
      assume (pLmEqual (los[i].p, los[i2].p));
      assume ((i2 == 0) || (!pLmEqual (los[i2].p, los[i2 - 1].p)));
      assume (i >= i2);

      erg.to_reduce_l = i2;
      #ifndef SING_NDEBUG
      assume ((i == losl - 1) || (pLmCmp (los[i].p, los[i + 1].p) == -1));
      #endif
      canonicalize_region (los, erg.to_reduce_u + 1, startf, c);
      return;
    }
    else /*if(j < 0)*/
    {
      //not reduceable, try to use this for reducing higher terms
      int i2 = fwbw (los, i);
      assume (pLmEqual (los[i].p, los[i2].p));
      assume ((i2 == 0) || (!pLmEqual (los[i2].p, los[i2 - 1].p)));
      assume (i >= i2);
      if(i2 != i)
      {
        erg.to_reduce_u = i - 1;
        erg.to_reduce_l = i2;
        erg.reduce_by = i;
        erg.fromS = FALSE;
        #ifndef SING_NDEBUG
        assume ((i == losl - 1) || (pLmCmp (los[i].p, los[i + 1].p) == -1));
        #endif
        canonicalize_region (los, erg.to_reduce_u + 1, startf, c);
        return;
      }
      i--;
    }
  }
  erg.reduce_by = -1;           //error code
  return;
}

//  nicht reduzierbare eintrage in ergnisliste schreiben
//   nullen loeschen
//   while(finde_groessten leitterm reduzierbar(c,erg))
//   {

static int
multi_reduction_clear_zeroes (red_object * los, int losl, int l, int u, int syzComp)
{
  int deleted = 0;
  int i = l;
  int last = -1;
  while(i <= u)
  {
    if((los[i].p == NULL)
    || (TEST_OPT_IDLIFT && (p_GetComp(los[i].p,currRing) > syzComp)))
    {
      kBucketDeleteAndDestroy (&los[i].bucket);
//      delete los[i];//here we assume los are constructed with new
      //destroy resources, must be added here
      if(last >= 0)
      {
        memmove (los + (int) (last + 1 - deleted), los + (last + 1),
                 sizeof (red_object) * (i - 1 - last));
      }
      last = i;
      deleted++;
    }
    i++;
  }
  if((last >= 0) && (last != losl - 1))
    memmove (los + (int) (last + 1 - deleted), los + last + 1,
             sizeof (red_object) * (losl - 1 - last));
  return deleted;
}

int search_red_object_pos (red_object * a, int top, red_object * key)
{
  int an = 0;
  int en = top;
  if(top == -1)
    return 0;
  if(pLmCmp (key->p, a[top].p) == 1)
    return top + 1;
  int i;
  loop
  {
    if(an >= en - 1)
    {
      if(pLmCmp (key->p, a[an].p) == -1)
        return an;
      return en;
    }
    i = (an + en) / 2;
    if(pLmCmp (key->p, a[i].p) == -1)
      en = i;
    else
      an = i;
  }
}

static void sort_region_down (red_object * los, int l, int u, slimgb_alg * /*c*/)
{
  int r_size = u - l + 1;
  qsort (los + l, r_size, sizeof (red_object), red_object_better_gen);
  int i;
  int *new_indices = (int *) omalloc ((r_size) * sizeof (int));
  int bound = 0;
  BOOLEAN at_end = FALSE;
  for(i = l; i <= u; i++)
  {
    if(!(at_end))
    {
      bound = new_indices[i - l] =
        bound + search_red_object_pos (los + bound, l - bound - 1, los + i);
      if(bound == l)
        at_end = TRUE;
    }
    else
    {
      new_indices[i - l] = l;
    }
  }
  red_object *los_region =
    (red_object *) omalloc (sizeof (red_object) * (u - l + 1));
  for(int i = 0; i < r_size; i++)
  {
    new_indices[i] += i;
    los_region[i] = los[l + i];
    assume ((i == 0) || (new_indices[i] > new_indices[i - 1]));
  }

  i = r_size - 1;
  int j = u;
  int j2 = l - 1;
  while(i >= 0)
  {
    if(new_indices[i] == j)
    {
      los[j] = los_region[i];
      i--;
      j--;
    }
    else
    {
      assume (new_indices[i] < j);
      los[j] = los[j2];
      assume (j2 >= 0);
      j2--;
      j--;
    }
  }
  omfree (los_region);
  omfree (new_indices);
}

//assume that los is ordered ascending by leading term, all non zero
static void multi_reduction (red_object * los, int &losl, slimgb_alg * c)
{
  poly *delay = (poly *) omAlloc (losl * sizeof (poly));
  int delay_s = 0;
  //initialize;
  assume (c->strat->sl >= 0);
  assume (losl > 0);
  int i;
  wlen_type max_initial_quality = 0;

  for(i = 0; i < losl; i++)
  {
    //los[i].sev = pGetShortExpVector (los[i].p);
    los[i].p = kBucketGetLm (los[i].bucket);
    if(los[i].initial_quality > max_initial_quality)
      max_initial_quality = los[i].initial_quality;
    // else
//         Print("init2_qal=%lld;", los[i].initial_quality);
//     Print("initial_quality=%lld;",max_initial_quality);
  }

  int curr_pos = losl - 1;

//  nicht reduzierbare eintrage in ergnisliste schreiben
  // nullen loeschen
  while(curr_pos >= 0)
  {
    if((c->use_noro_last_block)
       && (lies_in_last_dp_block (los[curr_pos].p, c)))
    {
      int pn_noro = curr_pos + 1;
      poly *p_noro = (poly *) omAlloc (pn_noro * sizeof (poly));
      for(i = 0; i < pn_noro; i++)
      {
        int dummy_len;
        poly p;
        los[i].p = NULL;
        kBucketClear (los[i].bucket, &p, &dummy_len);
        p_noro[i] = p;
      }
      if(n_GetChar(currRing->cf) < 255)
      {
        noro_step < tgb_uint8 > (p_noro, pn_noro, c);
      }
      else
      {
        if(n_GetChar(currRing->cf) < 65000)
        {
          noro_step < tgb_uint16 > (p_noro, pn_noro, c);
        }
        else
        {
          noro_step < tgb_uint32 > (p_noro, pn_noro, c);
        }
      }
      for(i = 0; i < pn_noro; i++)
      {
        los[i].p = p_noro[i];
        los[i].sev = pGetShortExpVector (los[i].p);
        //ignore quality
        kBucketInit (los[i].bucket, los[i].p, pLength (los[i].p));
      }
      qsort (los, pn_noro, sizeof (red_object), red_object_better_gen);
      int deleted =
        multi_reduction_clear_zeroes (los, losl, pn_noro, curr_pos, c->syz_comp);
      losl -= deleted;
      curr_pos -= deleted;
      break;
    }
    find_erg erg;

    multi_reduction_find (los, losl, c, curr_pos, erg); //last argument should be curr_pos
    if(erg.reduce_by < 0)
      break;

    erg.expand = NULL;

    multi_reduction_lls_trick (los, losl, c, erg);

    int i;
    //    wrp(los[erg.to_reduce_u].p);
    //PrintLn();
    multi_reduce_step (erg, los, c);


    if(!TEST_OPT_REDTHROUGH)
    {
      for(i = erg.to_reduce_l; i <= erg.to_reduce_u; i++)
      {
        if(los[i].p != NULL)    //the check (los[i].p!=NULL) might be invalid
        {
          //
          assume (los[i].initial_quality > 0);
          if(los[i].guess_quality (c)
             > 1.5 * delay_factor * max_initial_quality)
          {
            if(TEST_OPT_PROT)
              PrintS ("v");
            los[i].canonicalize ();
            if(los[i].guess_quality (c) > delay_factor * max_initial_quality)
            {
              if(TEST_OPT_PROT)
                PrintS (".");
              los[i].clear_to_poly ();
              //delay.push_back(los[i].p);
              delay[delay_s] = los[i].p;
              delay_s++;
              los[i].p = NULL;
            }
          }
        }
      }
    }
    int deleted = multi_reduction_clear_zeroes (los, losl, erg.to_reduce_l,
                                                erg.to_reduce_u, c->syz_comp);
    if(erg.fromS == FALSE)
      curr_pos = si_max (erg.to_reduce_u, erg.reduce_by);
    else
      curr_pos = erg.to_reduce_u;
    losl -= deleted;
    curr_pos -= deleted;

    //Print("deleted %i \n",deleted);
    if((TEST_V_UPTORADICAL) && (!(erg.fromS)))
      sort_region_down (los, si_min (erg.to_reduce_l, erg.reduce_by),
                        (si_max (erg.to_reduce_u, erg.reduce_by)) - deleted,
                        c);
    else
      sort_region_down (los, erg.to_reduce_l, erg.to_reduce_u - deleted, c);

    if(erg.expand)
    {
#ifdef FIND_DETERMINISTIC
      int i;
      for(i = 0; c->expandS[i]; i++) ;
      c->expandS = (poly *) omreallocSize (c->expandS, (i+1)*sizeof(poly),
                                                       (i+2)*sizeof(poly));
      c->expandS[i] = erg.expand;
      c->expandS[i + 1] = NULL;
#else
      int ecart = 0;
      if(c->eliminationProblem)
      {
        ecart =
          c->pTotaldegree_full (erg.expand) - c->pTotaldegree (erg.expand);
      }
      add_to_reductors (c, erg.expand, erg.expand_length, ecart);
#endif
    }
  }

  c->introduceDelayedPairs (delay, delay_s);
  /*
    sorted_pair_node** pairs=(sorted_pair_node**)
           omalloc(delay_s*sizeof(sorted_pair_node*));
     for(i=0;i<delay_s;i++)
     {
       poly p=delay[i];
       //if (rPar(c->r)==0)
       simplify_poly(p,c->r);
       sorted_pair_node* si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
       si->i=-1;
       si->j=-1;
       if (!rField_is_Zp(c->r))
       {
         if (!c->nc)
           p=redTailShort(p, c->strat);
         p_Cleardenom(p, c->r);
         p_Content(p, c->r);
       }
       si->expected_length=pQuality(p,c,pLength(p));
       si->deg=pTotaldegree(p);
       si->lcm_of_lm=p;
       pairs[i]=si;
     }
     qsort(pairs,delay_s,sizeof(sorted_pair_node*),tgb_pair_better_gen2);
     c->apairs=spn_merge(c->apairs,c->pair_top+1,pairs,delay_s,c);
     c->pair_top+=delay_s;
     omfree(pairs);
  */
  omFree (delay);
  return;
}

void red_object::flatten ()
{
  assume (p == kBucketGetLm (bucket));
}

void red_object::validate ()
{
  p = kBucketGetLm (bucket);
  if(p)
    sev = pGetShortExpVector (p);
}

int red_object::clear_to_poly ()
{
  flatten ();
  int l;
  kBucketClear (bucket, &p, &l);
  return l;
}

void reduction_step::reduce (red_object * /*r*/, int /*l*/, int /*u*/)
{
}

void simple_reducer::do_reduce (red_object & ro)
{
  number coef;
#ifdef HAVE_PLURAL
  if(c->nc)
    nc_kBucketPolyRed_Z (ro.bucket, p, &coef);
  else
#endif
    coef = kBucketPolyRed (ro.bucket, p, p_len, c->strat->kNoether);
  nDelete (&coef);
}

void simple_reducer::reduce (red_object * r, int l, int u)
{
  this->pre_reduce (r, l, u);
  int i;
//debug start

  if(c->eliminationProblem)
  {
    assume (p_LmEqual (r[l].p, r[u].p, c->r));
    /*int lm_deg=pTotaldegree(r[l].p);
       reducer_deg=lm_deg+pTotaldegree_full(p)-pTotaldegree(p); */
  }

  for(i = l; i <= u; i++)
  {
    this->do_reduce (r[i]);
  }
  for(i = l; i <= u; i++)
  {
    kBucketSimpleContent (r[i].bucket);
    r[i].validate ();
  }
}

reduction_step::~reduction_step ()
{
}

simple_reducer::~simple_reducer ()
{
  if(fill_back != NULL)
  {
    kBucketInit (fill_back, p, p_len);
  }
  fill_back = NULL;
}

void multi_reduce_step (find_erg & erg, red_object * r, slimgb_alg * c)
{
  STATIC_VAR int id = 0;
  id++;
  unsigned long sev;
  BOOLEAN lt_changed = FALSE;
  int rn = erg.reduce_by;
  poly red;
  int red_len;
  simple_reducer *pointer;
  BOOLEAN work_on_copy = FALSE;
  if(erg.fromS)
  {
    red = c->strat->S[rn];
    red_len = c->strat->lenS[rn];
    assume (red_len == (int)pLength (red));
  }
  else
  {
    r[rn].flatten ();
    kBucketClear (r[rn].bucket, &red, &red_len);

    if(TEST_OPT_INTSTRATEGY)
    {
      p_Cleardenom (red, c->r); //should be unnecessary
      //includes p_Content(red, c->r);
    }
    //pNormalize (red);

    if((!(erg.fromS)) && (TEST_V_UPTORADICAL))
    {
      if(polynomial_root (red, c->r))
        lt_changed = TRUE;
      sev = p_GetShortExpVector (red, c->r);
    }
    red_len = pLength (red);
  }
  if(((TEST_V_MODPSOLVSB) && (red_len > 1))
     || ((c->nc) || (erg.to_reduce_u - erg.to_reduce_l > 5)))
  {
    work_on_copy = TRUE;
    // poly m=pOne();
    poly m = c->tmp_lm;
    pSetCoeff (m, nInit (1));
    pSetComp (m, 0);
    for(int i = 1; i <= (currRing->N); i++)
      pSetExp (m, i, (pGetExp (r[erg.to_reduce_l].p, i) - pGetExp (red, i)));
    pSetm (m);
    poly red_cp;
#ifdef HAVE_PLURAL
    if(c->nc)
      red_cp = nc_mm_Mult_pp (m, red, c->r);
    else
#endif
      red_cp = ppMult_mm (red, m);
    if(!erg.fromS)
    {
      kBucketInit (r[rn].bucket, red, red_len);
    }
    //now reduce the copy
    //static poly redNF2 (poly h,slimgb_alg* c , int &len, number&  m,int n)

    if(!c->nc)
      redTailShort (red_cp, c->strat);
    //number mul;
    // red_len--;
//     red_cp->next=redNF2(red_cp->next,c,red_len,mul,c->average_length);
//     pSetCoeff(red_cp,nMult(red_cp->coef,mul));
//     nDelete(&mul);
//     red_len++;
    red = red_cp;
    red_len = pLength (red);
    // pDelete(&m);
  }

  assume (red_len == (int)pLength (red));

  int reducer_deg = 0;
  if(c->eliminationProblem)
  {
    int lm_deg = c->pTotaldegree (r[erg.to_reduce_l].p);
    int ecart;
    if(erg.fromS)
    {
      ecart = c->strat->ecartS[erg.reduce_by];
    }
    else
    {
      ecart = c->pTotaldegree_full (red) - lm_deg;
    }
    reducer_deg = lm_deg + ecart;
  }
  pointer = new simple_reducer (red, red_len, reducer_deg, c);

  if((!work_on_copy) && (!erg.fromS))
    pointer->fill_back = r[rn].bucket;
  else
    pointer->fill_back = NULL;
  pointer->reduction_id = id;
  pointer->c = c;

  pointer->reduce (r, erg.to_reduce_l, erg.to_reduce_u);
  if(work_on_copy)
    pDelete (&pointer->p);
  delete pointer;
  if(lt_changed)
  {
    assume (!erg.fromS);
    r[erg.reduce_by].sev = sev;
  }
}

void simple_reducer::pre_reduce (red_object * /*r*/, int /*l*/, int /*u*/)
{
}

#if 0
template int pos_helper<int, int*>(skStrategy*, spolyrec*, int, int*, spolyrec**);
template int pos_helper<long, long*>(skStrategy*, spolyrec*, long, long*, spolyrec**);

template void noro_step<unsigned char>(spolyrec**, int&, slimgb_alg*);
template void noro_step<unsigned int>(spolyrec**, int&, slimgb_alg*);
template void noro_step<unsigned short>(spolyrec**, int&, slimgb_alg*);


template int term_nodes_sort_crit<unsigned char>(void const*, void const*);
template int term_nodes_sort_crit<unsigned int>(void const*, void const*);
template int term_nodes_sort_crit<unsigned short>(void const*, void const*);

template spolyrec* row_to_poly<unsigned char>(unsigned char*, spolyrec**, int, ip_sring*);
template spolyrec* row_to_poly<unsigned int>(unsigned int*, spolyrec**, int, ip_sring*);
template spolyrec* row_to_poly<unsigned short>(unsigned short*, spolyrec**, int, ip_sring*);

template void simplest_gauss_modp<unsigned char>(unsigned char*, int, int);
template void simplest_gauss_modp<unsigned int>(unsigned int*, int, int);
template void simplest_gauss_modp<unsigned short>(unsigned short*, int, int);


template int modP_lastIndexRow<unsigned char>(unsigned char*, int);
template int modP_lastIndexRow<unsigned int>(unsigned int*, int);
template int modP_lastIndexRow<unsigned short>(unsigned short*, int);

template SparseRow<unsigned char>* noro_red_to_non_poly_t<unsigned char>(spolyrec*, int&, NoroCache<unsigned char>*, slimgb_alg*);
template SparseRow<unsigned int>* noro_red_to_non_poly_t<unsigned int>(spolyrec*, int&, NoroCache<unsigned int>*, slimgb_alg*);
template SparseRow<unsigned short>* noro_red_to_non_poly_t<unsigned short>(spolyrec*, int&, NoroCache<unsigned short>*, slimgb_alg*);


template MonRedResNP<unsigned char> noro_red_mon_to_non_poly<unsigned char>(spolyrec*, NoroCache<unsigned char>*, slimgb_alg*);
template MonRedResNP<unsigned int> noro_red_mon_to_non_poly<unsigned int>(spolyrec*, NoroCache<unsigned int>*, slimgb_alg*);
template MonRedResNP<unsigned short> noro_red_mon_to_non_poly<unsigned short>(spolyrec*, NoroCache<unsigned short>*, slimgb_alg*);

template SparseRow<unsigned char>* noro_red_to_non_poly_dense<unsigned char>(MonRedResNP<unsigned char>*, int, NoroCache<unsigned char>*);
template SparseRow<unsigned char>* noro_red_to_non_poly_sparse<unsigned char>(MonRedResNP<unsigned char>*, int, NoroCache<unsigned char>*);
template SparseRow<unsigned int>* noro_red_to_non_poly_dense<unsigned int>(MonRedResNP<unsigned int>*, int, NoroCache<unsigned int>*);
template SparseRow<unsigned int>* noro_red_to_non_poly_sparse<unsigned int>(MonRedResNP<unsigned int>*, int, NoroCache<unsigned int>*);
template SparseRow<unsigned short>* noro_red_to_non_poly_dense<unsigned short>(MonRedResNP<unsigned short>*, int, NoroCache<unsigned short>*);
template SparseRow<unsigned short>* noro_red_to_non_poly_sparse<unsigned short>(MonRedResNP<unsigned short>*, int, NoroCache<unsigned short>*);



template class DataNoroCacheNode<unsigned char>;
template class DataNoroCacheNode<unsigned int>;
template class DataNoroCacheNode<unsigned short>;

template class NoroCache<unsigned char>;
template class NoroCache<unsigned int>;
template class NoroCache<unsigned short>;



template void add_coef_times_dense<unsigned char>(unsigned char*, int, unsigned char const*, int, snumber*);
template void add_coef_times_dense<unsigned int>(unsigned int*, int, unsigned int const*, int, snumber*);
template void add_coef_times_dense<unsigned short>(unsigned short*, int, unsigned short const*, int, snumber*);
template void add_coef_times_sparse<unsigned char>(unsigned char*, int, SparseRow<unsigned char>*, snumber*);
template void add_coef_times_sparse<unsigned int>(unsigned int*, int, SparseRow<unsigned int>*, snumber*);
template void add_coef_times_sparse<unsigned short>(unsigned short*, int, SparseRow<unsigned short>*, snumber*);
template void add_dense<unsigned char>(unsigned char*, int, unsigned char const*, int);
template void add_dense<unsigned int>(unsigned int*, int, unsigned int const*, int);
template void add_dense<unsigned short>(unsigned short*, int, unsigned short const*, int);
template void add_sparse<unsigned char>(unsigned char*, int, SparseRow<unsigned char>*);
template void add_sparse<unsigned int>(unsigned int*, int, SparseRow<unsigned int>*);
template void add_sparse<unsigned short>(unsigned short*, int, SparseRow<unsigned short>*);


template void sub_dense<unsigned char>(unsigned char*, int, unsigned char const*, int);
template void sub_dense<unsigned int>(unsigned int*, int, unsigned int const*, int);
template void sub_dense<unsigned short>(unsigned short*, int, unsigned short const*, int);
template void sub_sparse<unsigned char>(unsigned char*, int, SparseRow<unsigned char>*);
template void sub_sparse<unsigned int>(unsigned int*, int, SparseRow<unsigned int>*);
template void sub_sparse<unsigned short>(unsigned short*, int, SparseRow<unsigned short>*);
template void write_coef_idx_to_buffer_dense<unsigned char>(CoefIdx<unsigned char>*, int&, unsigned char*, int);
template void write_coef_idx_to_buffer_dense<unsigned int>(CoefIdx<unsigned int>*, int&, unsigned int*, int);
template void write_coef_idx_to_buffer_dense<unsigned short>(CoefIdx<unsigned short>*, int&, unsigned short*, int);
template void write_coef_idx_to_buffer<unsigned char>(CoefIdx<unsigned char>*, int&, int*, unsigned char*, int);
template void write_coef_idx_to_buffer<unsigned int>(CoefIdx<unsigned int>*, int&, int*, unsigned int*, int);
template void write_coef_idx_to_buffer<unsigned short>(CoefIdx<unsigned short>*, int&, int*, unsigned short*, int);
template void write_coef_times_xx_idx_to_buffer_dense<unsigned char>(CoefIdx<unsigned char>*, int&, unsigned char*, int, snumber*);
template void write_coef_times_xx_idx_to_buffer_dense<unsigned int>(CoefIdx<unsigned int>*, int&, unsigned int*, int, snumber*);
template void write_coef_times_xx_idx_to_buffer_dense<unsigned short>(CoefIdx<unsigned short>*, int&, unsigned short*, int, snumber*);
template void write_coef_times_xx_idx_to_buffer<unsigned char>(CoefIdx<unsigned char>*, int&, int*, unsigned char*, int, snumber*);
template void write_coef_times_xx_idx_to_buffer<unsigned int>(CoefIdx<unsigned int>*, int&, int*, unsigned int*, int, snumber*);
template void write_coef_times_xx_idx_to_buffer<unsigned short>(CoefIdx<unsigned short>*, int&, int*, unsigned short*, int, snumber*);
template void write_minus_coef_idx_to_buffer_dense<unsigned char>(CoefIdx<unsigned char>*, int&, unsigned char*, int);
template void write_minus_coef_idx_to_buffer_dense<unsigned int>(CoefIdx<unsigned int>*, int&, unsigned int*, int);
template void write_minus_coef_idx_to_buffer_dense<unsigned short>(CoefIdx<unsigned short>*, int&, unsigned short*, int);
template void write_minus_coef_idx_to_buffer<unsigned char>(CoefIdx<unsigned char>*, int&, int*, unsigned char*, int);
template void write_minus_coef_idx_to_buffer<unsigned int>(CoefIdx<unsigned int>*, int&, int*, unsigned int*, int);
template void write_minus_coef_idx_to_buffer<unsigned short>(CoefIdx<unsigned short>*, int&, int*, unsigned short*, int);


template class std::vector<DataNoroCacheNode<unsigned char>*>;
template class std::vector<DataNoroCacheNode<unsigned int>*>;
template class std::vector<DataNoroCacheNode<unsigned short>*>;
template class std::vector<PolySimple>;

template void std::sort( CoefIdx<unsigned char>* , CoefIdx<unsigned char>*  );
template void std::sort( CoefIdx<unsigned int>*  , CoefIdx<unsigned int>*   );
template void std::sort( CoefIdx<unsigned short>*, CoefIdx<unsigned short>* );

template void std::sort_heap<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::sort_heap<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::sort_heap<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);

template void std::make_heap<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::make_heap<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::make_heap<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);
#endif

#if 0
template void std::__final_insertion_sort<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::__final_insertion_sort<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::__final_insertion_sort<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);

template void std::__introsort_loop<CoefIdx<unsigned char>*, long>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*, long);
template void std::__introsort_loop<CoefIdx<unsigned int>*, long>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*, long);
template void std::__introsort_loop<CoefIdx<unsigned short>*, long>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*, long);

template void std::__heap_select<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::__heap_select<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::__heap_select<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);

template void std::__insertion_sort<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::__insertion_sort<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::__insertion_sort<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);

template void std::__move_median_first<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*, CoefIdx<unsigned char>*);
template void std::__move_median_first<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*, CoefIdx<unsigned int>*);
template void std::__move_median_first<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*, CoefIdx<unsigned short>*);

template void std::__unguarded_linear_insert<CoefIdx<unsigned char>*>(CoefIdx<unsigned char>*);
template void std::__unguarded_linear_insert<CoefIdx<unsigned int>*>(CoefIdx<unsigned int>*);
template void std::__unguarded_linear_insert<CoefIdx<unsigned short>*>(CoefIdx<unsigned short>*);

template void std::__adjust_heap<CoefIdx<unsigned char>*, long, CoefIdx<unsigned char> >(CoefIdx<unsigned char>*, long, long, CoefIdx<unsigned char>);
template void std::__adjust_heap<CoefIdx<unsigned int>*, long, CoefIdx<unsigned int> >(CoefIdx<unsigned int>*, long, long, CoefIdx<unsigned int>);
template void std::__adjust_heap<CoefIdx<unsigned short>*, long, CoefIdx<unsigned short> >(CoefIdx<unsigned short>*, long, long, CoefIdx<unsigned short>);

template void std::__push_heap<CoefIdx<unsigned char>*, long, CoefIdx<unsigned char> >(CoefIdx<unsigned char>*, long, long, CoefIdx<unsigned char>);
template void std::__push_heap<CoefIdx<unsigned int>*, long, CoefIdx<unsigned int> >(CoefIdx<unsigned int>*, long, long, CoefIdx<unsigned int>);
template void std::__push_heap<CoefIdx<unsigned short>*, long, CoefIdx<unsigned short> >(CoefIdx<unsigned short>*, long, long, CoefIdx<unsigned short>);

template CoefIdx<unsigned char>* std::__unguarded_partition<CoefIdx<unsigned char>*, CoefIdx<unsigned char> >(CoefIdx<unsigned char>*, CoefIdx<unsigned char>*, CoefIdx<unsigned char> const&);
template CoefIdx<unsigned int>* std::__unguarded_partition<CoefIdx<unsigned int>*, CoefIdx<unsigned int> >(CoefIdx<unsigned int>*, CoefIdx<unsigned int>*, CoefIdx<unsigned int> const&);
template CoefIdx<unsigned short>* std::__unguarded_partition<CoefIdx<unsigned short>*, CoefIdx<unsigned short> >(CoefIdx<unsigned short>*, CoefIdx<unsigned short>*, CoefIdx<unsigned short> const&);

#endif

