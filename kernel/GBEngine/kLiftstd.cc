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

//---------------------------------------------------------------------------
static void setUnit(int e, ideal *unit)
{
  if (unit!=NULL)
  {
    *unit=idInit(e,e);
    for(int i=e-1;i>=0;i--)
    {
      poly p=pOne();
      p_Shift(&p,i+1,currRing);
      (*unit)->m[i]=p;
    }
  }
}
ideal idDivRem(ideal A,const ideal quot, ideal &factor,ideal *unit,int lazyReduce)
{
  /* special cases */
  if (idIs0(A) || idIs0(quot))
  {
    factor=idInit(1,IDELEMS(quot));
    setUnit(A->rank,unit);
    return idCopy(A);
  }
  /* ideal or module? */
  ring orig_ring=currRing;
  int k=id_RankFreeModule(quot,orig_ring);
  int lsmod=0;
  if (k==0) { lsmod=1;k=1;}  /*ideal*/
  else { k=A->rank;}
  /* NF(A 0 E,quot E 0)
   * A,quot: 1..k, 0,E: k+1..k+IDELEMS(quot),
   * E,0: k+IDELEMS(quot)..k+IDELEMS(quot)+IDELEMS(A) */
  /* new ring */
  ring syz_ring=rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(1,syz_ring);
  rChangeCurrRing(syz_ring);
  /* move ideals to new ring */
  ideal s_quot;
  ideal s_A;
  if (orig_ring != syz_ring)
  {
    s_quot=idrCopyR_NoSort(quot,orig_ring,syz_ring);
    s_A=idrCopyR_NoSort(A,orig_ring,syz_ring);
  }
  else
  {
    s_quot=id_Copy(quot,syz_ring);
    s_A=id_Copy(A,syz_ring);
  }
  /* quot[i] -> quot[i]+e(k+i+1) */
  for(int i=0;i<IDELEMS(s_quot);i++)
  {
    poly p=p_One(syz_ring);
    p_SetComp(p,k+i+1,syz_ring);
    p_Setm(p,syz_ring);
    if (lsmod==1) p_Shift(&(s_quot->m[i]),1,syz_ring);
    s_quot->m[i]=p_Add_q(s_quot->m[i],p,syz_ring);
  }
  s_quot->rank=k+IDELEMS(quot)+1;
  /* A[i] -> A[i]*e(1) */
  if (lsmod==1)
  {
    for(int i=0;i<IDELEMS(s_A);i++)
    {
      p_Shift(&s_A->m[i],1,syz_ring);
    }
  }
  if (unit!=NULL)
  {
    int u_k=k+IDELEMS(quot)+2;
    for(int i=0;i<IDELEMS(s_A);i++)
    {
      poly p=p_One(syz_ring);
      p_SetComp(p,u_k+i,syz_ring);
      p_Setm(p,syz_ring);
      s_A->m[i]=p_Add_q(s_A->m[i],p,syz_ring);
    }
    s_A->rank=k+IDELEMS(quot)+IDELEMS(A)+1;
  }
  /* normalform */
  #if 0
  PrintS("to reduce:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(s_A,currRing),currRing);
    ipPrint_MA0(m, "A");
    id_Delete((ideal *) &m,currRing);
  }
  PrintS("with:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(s_quot,currRing),currRing);
    ipPrint_MA0(m, "B");
    id_Delete((ideal *) &m,currRing);
  }
  #endif
  ideal rest=kNF(s_quot,syz_ring->qideal,s_A,0,lazyReduce);
  #if 0
  PrintS("result NF:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(rest,currRing),currRing);
    ipPrint_MA0(m, "A");
    id_Delete((ideal *) &m,currRing);
  }
  #endif
  /* clean s_quot,s_A */
  id_Delete(&s_quot,syz_ring);
  id_Delete(&s_A,syz_ring);
  /* interpret rest: remainder */
  ideal result=idInit(IDELEMS(rest),A->rank);
  for(int i=0;i<IDELEMS(rest);i++)
  {
    poly p=rest->m[i];
    poly d=NULL;
    while(p!=NULL)
    {
      poly q=p; pIter(p);
      pNext(q)=NULL;
      if (p_GetComp(q,syz_ring)<=k)
      {
        result->m[i]=p_Add_q(result->m[i],q,syz_ring);
      }
      else
      {
        d=p_Add_q(d,q,syz_ring);
      }
    }
    p_Shift(&d,-k,syz_ring);
    rest->m[i]=d;
  }
  rest->rank-=k;
  #if 0
  PrintS("rest:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(result,currRing),currRing);
    ipPrint_MA0(m, "_");
    id_Delete((ideal *) &m,currRing);
  }
  PrintS("factor+unit:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(rest,currRing),currRing);
    ipPrint_MA0(m, "_");
    id_Delete((ideal *) &m,currRing);
  }
  #endif
  /* interpret rest: factors */
  factor=idInit(IDELEMS(rest),IDELEMS(quot));
  int uk=IDELEMS(quot);
  for(int i=0;i<IDELEMS(rest);i++)
  {
    poly d=NULL;
    poly p=rest->m[i];
    while(p!=NULL)
    {
      poly q=p; pIter(p);
      pNext(q)=NULL;
      if (p_GetComp(q,syz_ring)<=uk)
      {
        factor->m[i]=p_Add_q(factor->m[i],q,syz_ring);
      }
      else
      {
        d=p_Add_q(d,q,syz_ring);
      }
    }
    p_Shift(&d,-uk-1,syz_ring);
    rest->m[i]=d;
    factor->m[i]=p_Neg(factor->m[i],syz_ring);
  }
  if (unit!=NULL)
  {
  #if 0
  PrintS("unit:\n");
  {
    void ipPrint_MA0(matrix m, const char *name);
    matrix m = id_Module2Matrix(id_Copy(rest,currRing),currRing);
    ipPrint_MA0(m, "_");
    id_Delete((ideal *) &m,currRing);
  }
  #endif
    *unit=idInit(IDELEMS(A),IDELEMS(A));
    for(int i=0;i<IDELEMS(rest);i++)
    {
      poly p=rest->m[i];
      rest->m[i]=NULL;
      (*unit)->m[i]=p;
    }
  }
  id_Delete(&rest,syz_ring);
  if (orig_ring != syz_ring)
  {
    rChangeCurrRing(orig_ring);
    result=idrMoveR_NoSort(result, syz_ring, orig_ring);
    factor=idrMoveR(factor, syz_ring, orig_ring);
    if (unit!=NULL)
    {
      *unit=idrMoveR(*unit, syz_ring, orig_ring);
    }
    rDelete(syz_ring);
  }
  return result;
}
