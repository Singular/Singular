/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/



#include "kernel/mod2.h"
#include "polys/monomials/ring.h"

#include "kernel/digitech.h"
#include "polys/kbuckets.h"
#include "kernel/ideals.h"
STATIC_INST_VAR ideal zero_ideal;

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
