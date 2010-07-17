/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
#include <kernel/mod2.h>
#include <kernel/ring.h>

#include <kernel/digitech.h>
#include <kernel/kbuckets.h>
#include <kernel/ideals.h>
static ideal zero_ideal;
#if 1
void bit_reduce(poly & f,ring r)
{
  poly p=f;
  kBucket_pt erg_bucket= kBucketCreate(r);
  kBucketInit(erg_bucket,NULL,0 /*pLength(P.p)*/);
  while(p)
  {
    poly next=pNext(p);
    pNext(p)=NULL;

    int i;
    int max=rVar(r);
    for(i=1;i<=max;i++)
    {
      unsigned long exp=p_GetExp(p,i,r);
      if(exp!=0)
        p_SetExp(p,i,1,r);

    }
    p_Setm(p,r);
    int pseudo_len=0;
    kBucket_Add_q(erg_bucket,p,&pseudo_len);
    p=next;
  }

  int len=0;
  poly erg;
  kBucketClear(erg_bucket,&erg, &len);
  kBucketDestroy(&erg_bucket);
  f=erg;
}
#else



void do_bit_reduce(poly f, kBucket_pt bucket)
{
    ring r=bucket->bucket_ring;
    int p=rChar(r);
    int max=rVar(r);
    while (f!=NULL)
        {
            // int len=0;
//             poly erg;
//             kBucketClear(bucket,&erg, &len);
//             kBucketDestroy(&bucket);
//             return erg;
            //return;
        //}

    BOOLEAN changed=FALSE;
    poly next=pNext(f);
    pNext(f)=NULL;
    int i;
    for(i=1;i<=max;i++)
    {
      unsigned long exp;
      while((exp=p_GetExp(f,i,r))>=p){
               p_SetExp(f,i,exp-p+1,r);
               changed=TRUE;
      }
    }

    if (changed)
    {
        p_Setm(f,r);

        int pseudo_len=0;
        kBucket_Add_q(bucket,f,&pseudo_len);
        //do_bit_reduce(next,bucket);
    }
    else
    {
        //do_bit_reduce(next,bucket);
        int pseudo_len=0;
        kBucket_Add_q(bucket,f,&pseudo_len);

    }
    f=next;
    }
}
poly do_bitreduce(poly f, ring r)
{
  poly erg=NULL;
  poly *append_to=&erg;
  int p=rChar(r);
  int max=rVar(r);
  kBucket_pt bucket= kBucketCreate(r);
  kBucketInit(bucket,NULL,0 /*pLength(P.p)*/);
  while(f!=NULL)
  {
    BOOLEAN changed=FALSE;
    poly next=pNext(f);
    pNext(f)=NULL;
    assume(pNext(f)==NULL);
    int i;
    for(i=1;i<=max;i++)
    {
      unsigned long exp;
      while((exp=p_GetExp(f,i,r))>=p)
      {
               p_SetExp(f,i,exp-p+1,r);
               changed=TRUE;
      }
    }
    if (changed)
    {
        p_Setm(f,r);
        int pseudo_len=0;
        kBucket_Add_q(bucket,f,&pseudo_len);
    }
    else
    {
        (*append_to)=f;
        append_to=&(pNext(f));
    }
    f=next;
  }
  {
    int pseudo_len=0;
    kBucket_Add_q(bucket,erg,&pseudo_len);
    pseudo_len=0;
    kBucketClear(bucket,&erg,&pseudo_len);
  }
    return erg;
}
void bit_reduce2(poly & f,ring r)
{
  if (f==NULL) return;
  if (pNext(f)==NULL){
    int p=rChar(r);
    int max=rVar(r);
    BOOLEAN changed=FALSE;
    poly next=pNext(f);
    assume(pNext(f)==NULL);
    int i;
    for(i=1;i<=max;i++)
    {
      unsigned long exp;
      while((exp=p_GetExp(f,i,r))>=p)
      {
               p_SetExp(f,i,exp-p+1,r);
               changed=TRUE;
      }
    }
    if (changed)
        p_Setm(f,r);
    return;
  }
  // kBucket_pt bucket= kBucketCreate(r);
//   kBucketInit(bucket,NULL,0 /*pLength(P.p)*/);
//   do_bit_reduce(f,bucket);
//   int len=0;
//   kBucketClear(bucket,&f, &len);
//   kBucketDestroy(&bucket);
    f=do_bitreduce(f,r);

}
void bit_reduce1(poly & f,ring r)
{
  if (f==NULL) return;


  if (pNext(f)==NULL)
  {
    int p=rChar(r);
    int max=rVar(r);
    BOOLEAN changed=FALSE;
    poly next=pNext(f);
    assume(pNext(f)==NULL);
    int i;
    for(i=1;i<=max;i++)
    {
      unsigned long exp;
      while((exp=p_GetExp(f,i,r))>=p)
      {
               p_SetExp(f,i,exp-p+1,r);
               changed=TRUE;
      }
    }
    if (changed)
        p_Setm(f,r);
    return;
  }
  kBucket_pt bucket= kBucketCreate(r);
  kBucketInit(bucket,NULL,0 /*pLength(P.p)*/);
  do_bit_reduce(f,bucket);
  int len=0;
  kBucketClear(bucket,&f, &len);
  kBucketDestroy(&bucket);

}
// void bit_reduce_arg(poly & f,ring r){
//   kBucket_pt bucket= kBucketCreate(r);
//   kBucketInit(bucket,NULL,0 /*pLength(P.p)*/);
//   f=do_bit_reduce(f,bucket);
// }
#endif

poly uni_subst_bits(poly outer_uni, poly inner_multi, ring r)
{
  zero_ideal=idInit(0,1);
  //assumes outer_uni is univariate and ordering global
  int d_max=p_GetExp(outer_uni,1,r);
  poly* potences=(poly*) omAlloc((d_max+1)*sizeof(poly));
  potences[0]=p_ISet(1,r);
  int i;
  for(i=1;i<=d_max;i++)
  {
    potences[i]=pp_Mult_qq(potences[i-1],inner_multi,r);
    bit_reduce(potences[i],r);
  }

  poly p=outer_uni;
  kBucket_pt erg_bucket= kBucketCreate(r);
  kBucketInit(erg_bucket,NULL,0 /*pLength(P.p)*/);


  while(p)
  {
    int d=p_GetExp(p,1,r);
    assume(potences[d]!=NULL); //mustn't always hold, but for most input
    int pseudo_len=0;
    kBucket_Add_q(erg_bucket,p_Mult_nn(potences[d],p_GetCoeff(p,r),r),&pseudo_len);
    potences[d]=NULL;
    p=pNext(p);
  }




  //free potences
  for(i=0;i<=d_max;i++)
  {
    p_Delete(&potences[i],r);
  }
  omfree(potences);
  int len=0;
  poly erg;
  kBucketClear(erg_bucket,&erg, &len);
  kBucketDestroy(&erg_bucket);
  return(erg);
}
