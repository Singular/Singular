/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

#include "kernel/mod2.h"

// define if no buckets should be used
// #define NO_BUCKETS

#include "omalloc/omalloc.h"
#include "kernel/GBEngine/kutil.h"
#include "misc/options.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/prCopy.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#ifdef HAVE_PLURAL
#include "polys/nc/nc.h"
#endif

static poly kSplitAt(int k,TObject* h,kStrategy strat)
{
  poly p;
  if (h->t_p==NULL)
  {
    if (currRing!=strat->tailRing)
    {
      h->t_p=k_LmInit_currRing_2_tailRing(h->p, strat->tailRing);
      p=h->t_p;
    }
    else
      p=h->p;
  }
  else
    p=h->t_p;
  if (p->next==NULL) return NULL;
  const ring tailRing=strat->tailRing;
  while(p_GetComp(p->next,tailRing)<=k)
  {
    pIter(p);
    if ((p==NULL)||(p->next==NULL))
    {
      h->pLength=0; // force re-computation
      return NULL;
    }
  }
  poly t=p->next;
  p->next=NULL;
  h->pLength=0; // force re-computation
  if ((h->p!=NULL) && (h->t_p!=NULL)
  && (pNext(h->p)!=pNext(h->t_p)))
  {
    pNext(h->p)=pNext(h->t_p);
  }
  return t;
}
static poly kSplitAt(int k,LObject* h,kStrategy strat)
{
  poly p,pr,t=NULL;
  int l;
  if (h->bucket!=NULL)
  {
    kBucketClear(h->bucket,&p,&l);
    pr=p;
  }
  else
  {
    if (h->t_p==NULL)
    {
      if (currRing!=strat->tailRing)
      {
        h->t_p=k_LmInit_currRing_2_tailRing(h->p, strat->tailRing);
        p=h->t_p;
      }
      else
        p=h->p;
    }
    else
      p=h->t_p;
  }
  const ring tailRing=strat->tailRing;
  if(p==NULL) return NULL;
  if (p_GetComp(p,tailRing)>k)
  {
    return p;
  }
  if (p->next==NULL)
  {
    goto finish;
  }
  while(p_GetComp(p->next,tailRing)<=k)
  {
    pIter(p);
    if (p->next==NULL) break;
  }
  t=p->next;
  p->next=NULL;
finish:
  if (h->bucket!=NULL)
  {
    l=pLength(pr);
    kBucketInit(h->bucket,pr,l);
  }
  else
  {
    if ((h->p!=NULL) && (h->t_p!=NULL))
    {
      pNext(h->p)=pNext(h->t_p);
    }
  }

  return t;
}
static void kAppend(poly t,TObject* h)
{
  poly p;
  if (h->t_p!=NULL)
    p=h->t_p;
  else
    p=h->p;
  while(p->next!=NULL) pIter(p);
  p->next=t;
  if ((h->p!=NULL)&&(h->t_p!=NULL)) pNext(h->p)=pNext(h->t_p);
}
static poly lazyComp(number* A, poly* M,poly* T,int index,poly s,int *l,const ring tailR)
{
  if ((TEST_OPT_PROT) && (index>0)) { Print("<%d>",index+1); mflush(); }
  kBucket_pt b=kBucketCreate(tailR);
  kBucketInit(b,s,pLength(s));
  int cnt=RED_CANONICALIZE;
  for(int i=0;i<index;i++)
  {
    kBucket_Mult_n(b,A[i]);
    n_Delete(&A[i],tailR->cf);
    poly tt=T[i];
    if (tt!=NULL)
    {
      cnt--;
      int dummy=pLength(tt);
      kBucket_Minus_m_Mult_p(b,M[i],tt,&dummy);
    }
    p_Delete(&M[i],tailR);
    if (UNLIKELY(cnt==0))
    {
      cnt=RED_CANONICALIZE;
      kBucketCanonicalize(b);
    }
  }
  poly p;
  kBucketClear(b,&p,l);
  kBucketDestroy(&b);
  return p;
}

/*2
*  reduction procedure for the sugar-strategy (honey)
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
int redLiftstd (LObject* h, kStrategy strat)
{
  if (strat->tl<0) return 1;
  assume(h->FDeg == h->pFDeg());
  assume(TEST_OPT_IDLIFT);
  poly h_p;
  int i,j,pass,ei, ii, h_d,ci;
  unsigned long not_sev;
  long reddeg,d;
  #define START_REDUCE 512
  int red_size=START_REDUCE;
  number *A=(number*)omAlloc0(red_size*sizeof(number));
  poly *C=(poly*)omAlloc0(red_size*sizeof(poly));
  poly *T=(poly*)omAlloc0(red_size*sizeof(poly));
  const ring tailRing=strat->tailRing;

  pass = j = 0;
  d = reddeg = h->GetpFDeg() + h->ecart;
  h->SetShortExpVector();
  int li;
  h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;

  // split h into mina part (h) and tail (h_tail)
  poly h_tail=kSplitAt(strat->syzComp,h,strat);
  // fix h-pLength
  h->pLength=0;
  h->PrepareRed(strat->use_buckets);
  loop
  {
    j=kFindDivisibleByInT(strat, h);
    if (j < 0)
    {
      // lazy computation:
      int l;
      poly p=lazyComp(A,C,T,pass,h_tail,&l,strat->tailRing);
      kBucket_Add_q(h->bucket,p,&l);
      omFreeSize(A,red_size*sizeof(number));
      omFreeSize(T,red_size*sizeof(poly));
      omFreeSize(C,red_size*sizeof(poly));
      return 1;
    }

    ei = strat->T[j].ecart;
    li = strat->T[j].pLength;
    ci = nSize(pGetCoeff(strat->T[j].p));
    ii = j;
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with ecart ei (T[ii])
     */
    i = j;
    if (TEST_OPT_LENGTH)
    {
      if (li<=0) li=strat->T[j].GetpLength();
      if (li>1)
      loop
      {
        /*- possible with respect to ecart, minimal nSize -*/
        i++;
        if (i > strat->tl)
          break;
        //if (ei < h->ecart)
        //  break;
        if ((((strat->T[i].ecart < ei) && (ei> h->ecart))
           || ((strat->T[i].ecart <= h->ecart)
              && (strat->T[i].pLength <= li)
              && (nSize(pGetCoeff(strat->T[i].p)) <ci)))
           &&
            p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                                 h_p, not_sev, tailRing))
        {
          /*
           * the polynomial to reduce with is now;
           */
          ei = strat->T[i].ecart;
          li = strat->T[i].pLength;
          if (li<=0) li=strat->T[i].GetpLength();
          ii = i;
          if (li==1) break;
        }
      }
    }

    /*
     * end of search: have to reduce with pi
     */
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      h->wrp();
      Print("\nwith T[%d]:",ii);
      strat->T[ii].wrp();
    }
#endif
    assume(strat->fromT == FALSE);

    //strat->T[ii].pCleardenom();
    // split T[ii]:
    // remember pLength of strat->T[ii]
    int l_orig=strat->T[ii].pLength;
    // split strat->T[ii]
    poly T_tail=kSplitAt(strat->syzComp,&strat->T[ii],strat);
    h->pLength=0; // force re-computation of length
    ksReducePoly(h,&(strat->T[ii]),NULL,&A[pass],&C[pass], strat);
    // restore T[ii]:
    kAppend(T_tail,&strat->T[ii]);
    strat->T[ii].pLength=l_orig;
    // store T_tail
    T[pass]=T_tail;
    // delayed computation: A[pass]*tail-M[pass]*T[pass]
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto:");
      h->wrp();
      PrintLn();
    }
#endif
    if(h->IsNull())
    {
      // clean up A,C,h_tail:
      for(int i=0;i<=pass;i++)
      {
        n_Delete(&A[i],tailRing->cf);
        p_Delete(&C[i],tailRing);
      }
      p_Delete(&h_tail,tailRing);
      kDeleteLcm(h);
      h->Clear();
      omFreeSize(A,red_size*sizeof(number));
      omFreeSize(T,red_size*sizeof(poly));
      omFreeSize(C,red_size*sizeof(poly));
      return 0;
    }
    h->SetShortExpVector();
    not_sev = ~ h->sev;
    h_d = h->SetpFDeg();
    /* compute the ecart */
    if (ei <= h->ecart)
      h->ecart = d-h_d;
    else
      h->ecart = d-h_d+ei-h->ecart;

    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    d = h_d + h->ecart;
    if (pass%RED_CANONICALIZE==0) kBucketCanonicalize(h->bucket);
    // if cache is to small, double its size:
    if (pass>=red_size-1)
    {
      A=(number*)omRealloc0Size(A,red_size*sizeof(number),2*red_size*sizeof(number));
      C=(poly*)omRealloc0Size(C,red_size*sizeof(poly),2*red_size*sizeof(poly));
      T=(poly*)omRealloc0Size(T,red_size*sizeof(poly),2*red_size*sizeof(poly));
      if(TEST_OPT_PROT) {Print("+%d+",red_size);mflush();}
      red_size*=2;
    }
  }
}
