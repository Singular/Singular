/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kbuckets.cc,v 1.2 1999-05-26 16:20:17 obachman Exp $ */

#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "spolys0.h"
#include "kbuckets.h"
#include "numbers.h"

#ifdef HAVE_BUCKETS

//////////////////////////////////////////////////////////////////////////
///
/// Some internal stuff
///

// returns ceil(log_4(l))
inline unsigned int pLogLength(unsigned int l)
{
  unsigned int i = 0;
  
  if (l == 0) return 0;
  l--;
  while ((l = (l >> 2))) i++;
  return i+1;
}

// returns ceil(log_4(pLength(p)))
inline unsigned int pLogLength(poly p)
{
  return pLogLength((unsigned int) pLength(p));
}

  

#if defined(PDEBUG) && ! defined(HAVE_PSEUDO_BUCKETS)

#define kbTests(bucket) kbDBTests(bucket, __FILE__, __LINE__)
#define kbTest(bucket, i) kbDBTest(bucket, i, __FILE__, __LINE__)

void kbDBTest(kBucket_pt bucket, int i, char* file, int line)
{
  pDBTest(bucket->buckets[i], file, line);
  if (bucket->buckets_length[i] != pLength(bucket->buckets[i]))
  {
    Warn("Bucket %d lengths difference should:%d has:%d in %s:%d\n",
          i, bucket->buckets_length[i], pLength(bucket->buckets[i]), file, line);
    assume(0);
  }
  else if (i > 0 && (int) pLogLength(bucket->buckets_length[i]) > i)
  {
    Warn("Bucket %d too long %d in %s:%d\n", 
          i, bucket->buckets_length[i], file, line);
    assume(0);
  }
  else if (i==0 && bucket->buckets_length[0] > 1)
  {
    Warn("Bucket 0 too long \n");
  }
}


void kbDBTests(kBucket_pt bucket, char* file, int line)
{
  int i;
  poly lm = bucket->buckets[0];

  kbDBTest(bucket, 0, file, line);
  for (i=1; i<= (int) bucket->buckets_used; i++)
  {
    kbDBTest(bucket, i, file, line);
    if (lm != NULL &&  bucket->buckets[i] != NULL
        && pComp0(lm, bucket->buckets[i]) != 1)
    {
      Warn("Bucket %d larger than lm in %s:%d\n", i, file, line);
      assume(0);
    }
  }
  
  for (; i<=MAX_BUCKET; i++)
  {
    if (bucket->buckets[i] != NULL || bucket->buckets_length[i] != 0)
    {
      Warn("Bucket %d not zero in %s:%d\n", i, file, line);
      assume(0);
    }
  }
}

#else

#define kbTests(bucket) (NULL)
#define kbTest(bucket, i) (NULL)

#endif // PDEBUG

//////////////////////////////////////////////////////////////////////////
///
/// Creation/Destruction of buckets
///

kBucket_pt kBucketCreate()
{
  kBucket_pt bucket = (kBucket_pt) Alloc0(sizeof(kBucket));
  return bucket;
}

void kBucketDestroy(kBucket_pt *bucket)
{
  Free(*bucket, sizeof(kBucket));
  *bucket = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Convertion from/to Bpolys
//
#ifndef HAVE_PSEUDO_BUCKETS

inline void kBucketAdjustBucketsUsed(kBucket_pt bucket)
{
  while ( bucket->buckets_used > 0 &&  
          bucket->buckets[bucket->buckets_used] == NULL)
    (bucket->buckets_used)--;
}

inline void kBucketMergeLm(kBucket_pt bucket)
{
  if (bucket->buckets[0] != NULL)
  {
    poly lm = bucket->buckets[0];
    int i = 1;
    int l = 4;
    while ( bucket->buckets_length[i] >= l)
    {
      i++;
      l = l << 2;
    }
    pNext(lm) = bucket->buckets[i];
    bucket->buckets[i] = lm;
    bucket->buckets_length[i]++;
    assume(i <= bucket->buckets_used+1);
    if (i > bucket->buckets_used)  bucket->buckets_used = i;
    bucket->buckets[0] = NULL;
    bucket->buckets_length[0] = 0;
  }
}
           
                             
static BOOLEAN kBucketIsCleared(kBucket_pt bucket)
{
  int i;

  for (i = 0;i<=MAX_BUCKET;i++)
  {
    if (bucket->buckets[i] != NULL) return FALSE;
    if (bucket->buckets_length[i] != 0) return FALSE;
  }
  return TRUE;
}

void kBucketInit(kBucket_pt bucket, poly lm, int length,
                 kbPolyProcs_pt pprocs,
                 memHeap heap)
{
  int i;
  
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(lm));
  assume(kBucketIsCleared(bucket));

  if (length <= 0)
    length = pLength(lm);

  bucket->buckets[0] = lm;
  if (length > 1)
  {
    unsigned int i = pLogLength(length-1);
    bucket->buckets[i] = pNext(lm);
    pNext(lm) = NULL;
    bucket->buckets_length[i] = length-1;
    bucket->buckets_used = i;
    bucket->buckets_length[0] = 1;
  }
  else
  {
    bucket->buckets_used = 0;
    bucket->buckets_length[0] = 0;
  }
  bucket->heap = heap;
  assume(pprocs != NULL);
  bucket->pprocs = pprocs;
}

static int kBucketCanonicalize(kBucket_pt bucket);

static void kBucketSetLm(kBucket_pt bucket)
{
  int j = 0;
  poly lt;
  BOOLEAN zero = FALSE;
  assume(bucket->buckets[0] == NULL && bucket->buckets_length[0] == 0);
  
  do
  {
    j = 0;
    for (int i = 1; i<=bucket->buckets_used; i++)
    {
      if (bucket->buckets[i] != NULL)
      {
        int comp = (j == 0 ? 1 :
                    pComp0(bucket->buckets[i], bucket->buckets[j]));
        
        if (comp > 0) 
        {
          if (j > 0 &&
              bucket->buckets[j] != NULL &&
              nIsZero(pGetCoeff(bucket->buckets[j])))
          {
            kb_pDelete1AndAdvance(bucket->buckets[j], bucket->heap);
            (bucket->buckets_length[j])--;
          }
          j = i;
        }
        else if (comp == 0)
        {
          number tn = pGetCoeff(bucket->buckets[j]);
          pSetCoeff0(bucket->buckets[j],
                     nAdd(pGetCoeff(bucket->buckets[i]), tn));
          nDelete(&tn);
          kb_pDelete1AndAdvance(bucket->buckets[i], bucket->heap);
          (bucket->buckets_length[i])--;
        }
      }
    }
    if (j > 0 && nIsZero(pGetCoeff(bucket->buckets[j])))
    {
      kb_pDelete1AndAdvance(bucket->buckets[j], bucket->heap);
      (bucket->buckets_length[j])--;
      j = -1;
    }
  }
  while (j < 0);

  if (j == 0) 
  {
    return;
  }

  assume(bucket->buckets[j] != NULL);
  lt = bucket->buckets[j];
  bucket->buckets[j] = pNext(lt);
  bucket->buckets_length[j]--;
  pNext(lt) = NULL;
  bucket->buckets[0] = lt;
  bucket->buckets_length[0] = 1;

  kBucketAdjustBucketsUsed(bucket);
}

const poly kBucketGetLm(kBucket_pt bucket)
{
  if (bucket->buckets[0] == NULL)
    kBucketSetLm(bucket);
  return bucket->buckets[0];
}

poly kBucketExtractLm(kBucket_pt bucket)
{
  poly lm;
  if (bucket->buckets[0] == NULL) kBucketSetLm(bucket);
  lm = bucket->buckets[0];
  bucket->buckets[0] = NULL;
  bucket->buckets_length[0] = 0;
  return lm;
}

static int kBucketCanonicalize(kBucket_pt bucket)
{
  poly p = bucket->buckets[1];
  poly lm;
  int pl = bucket->buckets_length[1], i;
  bucket->buckets[1] = NULL;
  bucket->buckets_length[1] = 0;

  
  for (i=2; i<=bucket->buckets_used; i++)
  {
    bucket->pprocs->p_Add_q(&p, &pl,
                            &(bucket->buckets[i]),&(bucket->buckets_length[i]),
                            bucket->heap);
  }

  lm = bucket->buckets[0];
  if (lm != NULL)
  {
    pNext(lm) = p;
    p = lm;
    pl++;
    bucket->buckets[0] = NULL;
    bucket->buckets_length[0] = 0;
  }
  if (pl > 0)
  {
    i = pLogLength(pl);
    bucket->buckets[i] = p;
    bucket->buckets_length[i] = pl;
  }
  else
  {
    i = 0;
  }
  bucket->buckets_used = i;
  assume(pLength(p) == (int) pl);
  return i;
}

void kBucketClear(kBucket_pt bucket, poly *p, int *length)
{
  int i = kBucketCanonicalize(bucket);
  if (i > 0)
  {
    *p = bucket->buckets[i];
    *length = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    bucket->buckets_used = 0;
  }
  else
  {
    *p = NULL;
    *length = 0;
  }
}

#else // HAVE_PSEUDO_BUCKETS

void kBucketInit(kBucket_pt bucket, poly lm, int length,
                 kbPolyProcs_pt pprocs,
                 memHeap heap)
{
  int i;
  
  assume(bucket != NULL);
  assume(length <= 0 || length == pLength(lm));

  bucket->p = lm;
  if (length <= 0) bucket->l = pLength(lm);
  else bucket->l = length;
  
  bucket->heap = heap;
  assume(pprocs != NULL);
  bucket->pprocs = pprocs;
}

const poly kBucketGetLm(kBucket_pt bucket)
{
  return bucket->p;
}

poly kBucketExtractLm(kBucket_pt bucket)
{
  poly lm = bucket->p;
  assume(pLength(bucket->p) == bucket->l);
  pIter(bucket->p);
  (bucket->l)--;
  pNext(lm) = NULL;
  return lm;
}

void kBucketClear(kBucket_pt bucket, poly *p, int *length)
{
  assume(pLength(bucket->p) == bucket->l); 
  *p = bucket->p;
  *length = bucket->l;
  bucket->p = NULL;
  bucket->l = 0;
}

#endif // ! HAVE_PSEUDO_BUCKETS

/////////////////////////////////////////////////////////////////////////////
// Compactification
//

#if 0
// #if defined(KB_USE_HEAPS) && defined(KB_HAVE_HEAPS_GC)

void kBucketCompactifyIfNecessary(kBucket_pt bucket)
{
  if (pShouldCompactify(bucket->heap, bucket->length))
  {
    int i = kBucketCanonicalize(bucket);
    assume(i > 0 && bucket->length > 0);
    pCompactifyHeap(bucket->heap, &(bucket->buckets[i]), bucket->length);
    bucket->buckets[0] = bucket->buckets[i];
    bucket->buckets[i] = pNext(bucket->buckets[i]);
    (bucket->buckets_length[i])--;
    pNext(bucket->buckets[0]) = NULL;
  }
}
    
#else

#define kBucketCompactifyIfNecessary(b) 

#endif

//////////////////////////////////////////////////////////////////////////
///
/// Multiply Bucket by number ,i.e. Bpoly == n*Bpoly
///
void kBucket_Mult_n(kBucket_pt bucket, number n)
{
#ifndef HAVE_PSEUDO_BUCKETS
  int i;
  
  for (i=0; i<= bucket->buckets_used; i++)
    if (bucket->buckets[i] != NULL) 
      bucket->pprocs->n_Mult_p(n, bucket->buckets[i]);
#else
  bucket->pprocs->n_Mult_p(n, bucket->p);
#endif  
}

//////////////////////////////////////////////////////////////////////////
///
/// Bpoly == Bpoly - m*p; where m is a monom
/// Does not destroy p and m
/// assume (*l <= 0 || pLength(p) == *l)
void kBucket_Minus_m_Mult_p(kBucket_pt bucket, poly m, poly p, int *l, 
                            poly spNoether)
{
  assume (*l <= 0 || pLength(p) == *l);
  int i, l1;
  poly p1 = p;
  
  if (*l <= 0) 
  {
    l1 = pLength(p1);
    *l = l1;
  }
  else
    l1 = *l;
  
  if (m == NULL || p == NULL) return;
  
#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  i = pLogLength(l1);

  if (i <= bucket->buckets_used && bucket->buckets[i] != NULL)
  {
    bucket->pprocs->p_Minus_m_Mult_q
      (&(bucket->buckets[i]), &(bucket->buckets_length[i]),
       m,
       p1, l1, 
       spNoether,
       bucket->pprocs->p_Mult_m,
       bucket->heap);
    p1 = bucket->buckets[i];
    l1 = bucket->buckets_length[i];
    bucket->buckets[i] = NULL;
    bucket->buckets_length[i] = 0;
    i = pLogLength(l1);
    while (bucket->buckets[i] != NULL)
    {
      bucket->pprocs->p_Add_q(&p1, &l1,
                              &(bucket->buckets[i]), 
                              &(bucket->buckets_length[i]),
                              bucket->heap);
      i = pLogLength(l1);
    }
  }
  else
  {
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
    p1 = bucket->pprocs->p_Mult_m(p1, m, spNoether, bucket->heap);
    pSetCoeff0(m, nNeg(pGetCoeff(m)));
  }

  bucket->buckets[i] = p1;
  bucket->buckets_length[i]=l1;
  if (i >= bucket->buckets_used) 
    bucket->buckets_used = i;
  else
    kBucketAdjustBucketsUsed(bucket);
#else // HAVE_PSEUDO_BUCKETS
  bucket->pprocs->p_Minus_m_Mult_q(&(bucket->p), &(bucket->l),
                                   m, p, l1, spNoether, 
                                   bucket->pprocs->p_Mult_m,
                                   bucket->heap);
#endif  
  kbTests(bucket);
}

/////////////////////////////////////////////////////////////////////////////
// 
// Extract all monomials from bucket with component comp
// Return as a polynomial *p with length *l
// In other words, afterwards 
// Bpoly == Bpoly - (poly consisting of all monomials with component comp)
// and components of monomials of *p are all 0
// 
void kBucketTakeOutComp(kBucket_pt bucket, 
                        Exponent_t comp, 
                        poly *r_p, int *l)
{
  poly p = NULL, q;
  int i, lp = 0, lq;

#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  for (i=1; i<=bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
      pTakeOutComp(&(bucket->buckets[i]), comp, &q, &lq);
      if (q != NULL)
      {
        assume(pLength(q) == lq);
        bucket->buckets_length[i] -= lq;
        assume(pLength(bucket->buckets[i]) == bucket->buckets_length[i]);
        bucket->pprocs->p_Add_q(&p, &lp, &q, &lq, bucket->heap);
      }
    }
  }
  kBucketAdjustBucketsUsed(bucket);
#else
  pTakeOutComp(&(bucket->p), comp, &p, &lp);
  (bucket->l) -= lp;
#endif  
  *r_p = p;
  *l = lp;
  
  kbTests(bucket);
}

void kBucketDecrOrdTakeOutComp(kBucket_pt bucket, 
                               Exponent_t comp, Order_t order, 
                               poly *r_p, int *l)
{
  poly p = NULL, q;
  int i, lp = 0, lq;
  
#ifndef HAVE_PSEUDO_BUCKETS
  kBucketMergeLm(bucket);
  for (i=1; i<=bucket->buckets_used; i++)
  {
    if (bucket->buckets[i] != NULL)
    {
      pDecrOrdTakeOutComp(&(bucket->buckets[i]), comp, order, &q, &lq);
      if (q != NULL)
      {
        bucket->buckets_length[i] -= lq;
        bucket->pprocs->p_Add_q(&p, &lp, &q, &lq, bucket->heap);
      }
    }
  }
  kBucketAdjustBucketsUsed(bucket);
#else
  pDecrOrdTakeOutComp(&(bucket->p), comp, order, &p, &lp);
  (bucket->l) -= lp;
#endif  

  *r_p = p;
  *l = lp;
}
  
/////////////////////////////////////////////////////////////////////////////
// Reduction of Bpoly with a given poly
//

extern int spCheckCoeff(number *a, number *b);

number kBucketPolyRed(kBucket_pt bucket, 
                      poly p1, int l1, 
                      poly spNoether)
{
  assume(p1 != NULL && 
         pDivisibleBy(p1,  kBucketGetLm(bucket)));
  assume(pLength(p1) == (int) l1);

  poly a1 = pNext(p1), lm = kBucketExtractLm(bucket);
  BOOLEAN reset_vec=FALSE;
  number rn;
  
  if(a1==NULL)
  {
    kb_pDelete1(lm, bucket->heap);
    return nInit(1);
  }

  if (! nIsOne(pGetCoeff(p1)))
  {
    number an = pGetCoeff(p1), bn = pGetCoeff(lm);
    int ct = spCheckCoeff(&an, &bn);
    pSetCoeff(lm, bn);
    if ((ct == 0) || (ct == 2)) kBucket_Mult_n(bucket, an);
    rn = an;
  }
  else
  {
    rn = nInit(1);
  }

  if (pGetComp(p1) != pGetComp(lm))
  {
    pSetCompP(a1, pGetComp(lm));
    reset_vec = TRUE;
  }
  
  spMonSub(lm,p1);
  l1--;

  kBucket_Minus_m_Mult_p(bucket, lm, a1, &l1, spNoether);
  
  kb_pDelete1(lm, bucket->heap);
  if (reset_vec) spModuleToPoly(a1);
  kbTests(bucket);
  return rn;
}

/////////////////////////////////////////////////////////////////////////////
// Reduction of a poly using buckets
//

#ifdef KB_USE_BUCKETS
/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
void kBucketRedHomog (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    return;
  }
  int j = 0;
  int k = 0;
  poly lm;
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
#endif
  
  kBucketInit(&(strat->bucket), h->p, h->length, &(strat->pprocs),
              h->heap);
  
  lm = kBucketGetLm(&(strat->bucket));
  
  for(;;)
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) Print("%d",j);
#endif
    j = 0;
    
#ifdef KB_HAVE_SHORT_EVECTORS
    unsigned long ev = ~ pGetShortExpVector(lm);
#endif
    
    if (strat->ak)
    {
      while(j <= strat->tl)
      {
        if (pDivisibleBy1(strat->T[j].p,lm)) goto Found;
        j++;
      }
    }
    else
    {
      while(j <= strat->tl)
      {
#ifdef KB_HAVE_SHORT_EVECTORS
        if ((strat->T[j].sev & ev) || ! pDivisibleBy2(strat->T[j].p,lm))
          j++;
        else
          goto Found;
#else
        if (pDivisibleBy2(strat->T[j].p,lm)) goto Found;
        j++;
#endif
      }
    }

    assume(j > strat->tl);
    kBucketClear(&(strat->bucket), &(h->p), &(h->length));
    if (TEST_OPT_INTSTRATEGY) pCleardenom((*h).p);
    assume(pLength(h->p) == h->length);
    return;

    Found:
    // now we found one to reduce with
    assume(pDivisibleBy(strat->T[j].p,lm));

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
        wrp(strat->T[j].p);
    }
#endif
    assume(strat->T[j].length <= 0 ||
           strat->T[j].length == pLength(strat->T[j].p));
      /*- compute the s-polynomial -*/
    if (strat->T[j].length <= 0)
      strat->T[j].length = pLength(strat->T[j].p);
      
    // Compactify 
    kBucketCompactifyIfNecessary(&(strat->bucket));

    number up = kBucketPolyRed(&(strat->bucket),
                               strat->T[j].p, strat->T[j].length,
                               strat->kNoether);
    nDelete(&up);
    lm = kBucketGetLm(&(strat->bucket));
      
    if (lm == NULL)
    {
      h->p = NULL;
      h->length = 0;
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
      if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return;
    }
  }
}


/*2
*  reduction procedure for the sugar-strategy (honey)
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
void kBucketRedHoney (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    return;
  }

  poly pi;
  int i,j,at,reddeg,d,pass,ei, li;

  pass = j = 0;
  d = reddeg = pFDeg((*h).p)+(*h).ecart;
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp((*h).p);
  }
#endif
  kTest(strat);
  
  if (strat->fromT)
  {
    HeapPoly b_hp(h->heap), r_hp(h->p, h->length);
    pCopyHeapPoly(&b_hp, &r_hp, FALSE);
    kBucketInit(&(strat->bucket), b_hp.p, b_hp.length, &(strat->pprocs),
                h->heap);
  }
  else
    kBucketInit(&(strat->bucket),  h->p, h->length, &(strat->pprocs),
                h->heap);

  poly lm = kBucketGetLm(&(strat->bucket));
  
  strat->fromT=FALSE;

  while (1)
  {
    j = 0;
#ifdef KB_HAVE_SHORT_EVECTORS
    unsigned long ev = ~ pGetShortExpVector(lm);
#endif
 
    if (strat->ak)
    {
      while(j <= strat->tl)
      {
        if (pDivisibleBy1(strat->T[j].p,lm)) goto Found;
        j++;
      }
    }
    else
    {
      while(j <= strat->tl)
      {
#ifdef KB_HAVE_SHORT_EVECTORS
        if ((strat->T[j].sev & ev) || ! pDivisibleBy2(strat->T[j].p,lm))
          j++;
        else
          goto Found;
#else        
        if (pDivisibleBy2(strat->T[j].p,lm)) goto Found;
        j++;
#endif
      }
    }

    assume(j > strat->tl);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) PrintLn();
#endif  
    kBucketClear(&(strat->bucket), &(h->p), &(h->length));
    if (TEST_OPT_INTSTRATEGY) pCleardenom(h->p);// also does a pContent
    return;

    Found:
    assume(pDivisibleBy(strat->T[j].p,lm));
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) Print(" T[%d]",j);
#endif
    pi = strat->T[j].p;
    ei = strat->T[j].ecart;
    li = strat->T[j].length;
    if (li == 0)
    {
      strat->T[j].length = pLength(pi);
      li =  strat->T[j].length;
    }

      
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with ecart ei
     */
    i = j;
    loop
      {
        /*- takes the first possible with respect to ecart -*/
        i++;
        if (i > strat->tl)
          break;
        if ((!BTEST1(20)) && (ei <= (*h).ecart))
          break;
        if ((strat->T[i].ecart < ei) && pDivisibleBy1(strat->T[i].p,lm))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" T[%d]",i);
#endif
          /*
           * the polynomial to reduce with is now;
           */
          pi = strat->T[i].p;
          ei = strat->T[i].ecart;
          li = strat->T[i].length;
          if (li == 0)
          {
            strat->T[i].length = pLength(pi);
            li =  strat->T[i].length;
          }
        }
      }

    /*
     * end of search: have to reduce with pi
     */
    if ((pass!=0) && (ei > (*h).ecart))
    {
      /*
       * It is not possible to reduce h with smaller ecart;
       * if possible h goes to the lazy-set L,i.e
       * if its position in L would be not the last one
       */
      if (strat->Ll >= 0) /* L is not empty */
      {
        h->p = lm;
        at = strat->posInL(strat->L,strat->Ll,*h,strat);
        if(at <= strat->Ll)
          /*- h will not become the next element to reduce -*/
        {
          kBucketClear(&(strat->bucket), &(h->p), &(h->length));
          pCompactifyHeap( h->heap, &(h->p), h->length);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
#endif
          h->p = NULL;
          h->length = 0;
          h->heap = NULL;
          return;
        }
      }
    }
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nwith ");
      wrp(pi);
    }
#endif

    // Poly Reduction
    kBucketCompactifyIfNecessary(&(strat->bucket));
    number up = kBucketPolyRed(&(strat->bucket),
                               pi, li,
                               strat->kNoether);
    nDelete(&up);
    lm = kBucketGetLm(&(strat->bucket));

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS(" to ");
      wrp((*h).p);
      PrintLn();
    }
#endif
    if (lm == NULL)
    {
      if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      h->p = NULL;
      h->length = 0;
      return;
    }
    /* compute the ecart */
    if (ei <= (*h).ecart)
      (*h).ecart = d-pFDeg(lm);
    else
      (*h).ecart = d-pFDeg(lm)+ei-(*h).ecart;
    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    d = pFDeg(lm)+(*h).ecart;
    if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->p = lm;
      at = strat->posInL(strat->L,strat->Ll,*h,strat);
      if (at <= strat->Ll)
      {
        kBucketClear(&(strat->bucket), &(h->p), &(h->length));
        assume(pLength(h->p) == h->length);
        /*test if h is already standardbasis element*/
        i=strat->sl+1;
        do
        {
          i--;
          if (i<0)
          {
            // we actually should do a pCleardenom(h->p) here
            return;
          }
        } while (!pDivisibleBy1(strat->S[i], h->p));

        // enter in Lazyset and return
        pCompactifyHeap( h->heap, &(h->p), h->length);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
          Print(" degree jumped: -> L%d\n",at);
#endif
        h->p = NULL;
        h->length = 0;
        h->heap = NULL;
        return;
      }
    }
    else if (TEST_OPT_PROT && (strat->Ll < 0) && (d > reddeg))
    {
      reddeg = d;
      Print(".%d",d); mflush();
    }
  }
}

/*2
*compute the normalform of the tail p->next of p
*with respect to S
*/
void kBucketRedTail(LObject *hp, int pos, kStrategy strat)
{
  poly h, hn, redstart = NULL;
  int length = 1, j;

  if (strat->noTailReduction) return;
  h = hp->p;
  hn = pNext(h);
  strat->redTailChange=FALSE;

  while(hn != NULL)
  {
    j = 0;
    while (j <= pos)
    {
      if (pDivisibleBy(strat->S[j], hn))
      {
        if (redstart == NULL)
        {
#ifdef KB_USE_HEAPS
          int i;
          if (hp->length <= 0) hp->length = pLength(hp->p);
          pCompactifyHeap(hp->heap, &(hp->p), hp->length);
          h = hp->p;
          for (i = 1, h = hp->p; i < length; i++, h = pNext(h));
          hn = pNext(h);
#endif
          strat->redTailChange = TRUE;
          redstart = h;

          kBucketInit(&(strat->bucket), hn, -1,
                      &(strat->pprocs),
                      hp->heap);
        }
        number up = kBucketPolyRed(&(strat->bucket), strat->S[j], 
                                   pLength(strat->S[j]), 
                                   strat->kNoether);
        nDelete(&up);
        hn = kBucketGetLm(&(strat->bucket));
        if (hn == NULL)
        {
          pNext(h) = NULL;
          goto Finish;
        }
        j = 0;
      }
      else
      {
        j++;
      }
    }

    if (redstart != NULL)
    {
      hn = kBucketExtractLm(&(strat->bucket));
      pNext(h) = hn;
      h = hn;
      hn = kBucketGetLm(&(strat->bucket));
    }
    else
    {
      pNext(h) = hn;
      h = hn;
      hn = pNext(h);
    }
    length++;
  }
  

  Finish:
  assume(pLength(hp->p) == length);
  hp->length = length;
}

#endif // KB_USE_BUCKETS

#endif // HAVE_BUCKETS
