#include "fast_mult.h"
#include "kbuckets.h"
static void degsplit(poly p,int n,poly &p1,poly&p2){
  poly erg1_i, erg2_i;
  erg1_i=NULL;
  erg2_i=NULL;
  while(p){
    if(pGetExp(p,1)>=n){
      if (p1==NULL){
	p1=p;
      } else{
	pNext(erg1_i)=p;
      }
      erg1_i=p;
    } else{
      if (p2==NULL){
	p2=p;
      } else{
	pNext(erg2_i)=p;
      }
      erg2_i=p;
    }
    p=pNext(p);
  }
  if(erg2_i){
    pNext(erg2_i)=NULL;
      }
  if (erg1_i){
    pNext(erg1_i)=NULL;
  }
  
}
static void div_by_x_power_n(poly p, int n){
  while(p){
    assume(pGetExp(p,1)>=n);
    int e=pGetExp(p,1);
    pSetExp(p,1,e-n);
    p=pNext(p);
  }
}

poly do_unifastmult(poly f,poly g){
  int n=1;
  if ((f==NULL)||(g==NULL)) return NULL;
  int df=pGetExp(f,1);//pFDeg(f);
    int dg=pGetExp(g,1);//pFDeg(g);
	
  int dm;
  if(df>dg){
    dm=df;
  }else{
    dm=dg;
  }
  while(n<=dm)
    {
      n*=2;
    }
  if(n==1){
    return(ppMult_qq(f,g));
  }
  
  int pot=n/2;
  assume(pot*2==n);
  poly f1=NULL;
  //f/(x^(pot));
  poly f0=NULL;//f-(x^(pot)*F1);
  degsplit(pCopy(f),pot,f1,f0);
  div_by_x_power_n(f1,pot);
  // poly G1=g/(x^(pot));
  //poly G0=g-(x^(pot)*G1);
  poly g1=NULL;
  poly g0=NULL;
  degsplit(pCopy(g),pot,g1,g0);
  div_by_x_power_n(g1,pot);
  
  poly p00=unifastmult(f0,g0);
    poly p11=unifastmult(f1,g1);
  
  poly pbig=unifastmult(pAdd(f0,f1),pAdd(g0,g1));
  
  poly erg=NULL;//=p11;
 
  poly factor=pOne();
  pSetExp(factor,1,n);
  erg=ppMult_mm(p11,factor);
  erg=pAdd(erg,pCopy(p00));
 
  poly sum=pbig;
  pSetExp(factor,1,pot);
 
  sum=pAdd(sum,pNeg(p00));
 

  sum=pAdd(sum,pNeg(p11));

 
  sum=pMult_mm(sum,factor);
 
  erg=pAdd(sum,erg);
  //  erg=pAdd(erg,p00);
 
  pDelete(&factor);
  return(erg);
}

poly do_unifastmult_buckets(poly f,poly g){
   
  
  // BOOLEAN corr=lenS_correct(strat);

  int n=1;
  if ((f==NULL)||(g==NULL)) return NULL;
  int df=pGetExp(f,1);//pFDeg(f);
    int dg=pGetExp(g,1);//pFDeg(g);
	
  int dm;
  if(df>dg){
    dm=df;
  }else{
    dm=dg;
  }
  while(n<=dm)
    {
      n*=2;
    }
  int pseudo_len=0;
  if(n==1){
    return ppMult_qq(f,g);

  }
  kBucket_pt erg_bucket= kBucketCreate(currRing);
  kBucketInit(erg_bucket,NULL,0 /*pLength(P.p)*/);
  //poly erg_mult=pOne();
  int pot=n/2;
  assume(pot*2==n);
  poly f1=NULL;
  //f/(x^(pot));
  poly f0=NULL;//f-(x^(pot)*F1);
  degsplit(pCopy(f),pot,f1,f0);
  div_by_x_power_n(f1,pot);
  // poly G1=g/(x^(pot));
  //poly G0=g-(x^(pot)*G1);
  poly g1=NULL;
  poly g0=NULL;
  degsplit(pCopy(g),pot,g1,g0);
  div_by_x_power_n(g1,pot);
  
  poly p00=unifastmult(f0,g0);
    poly p11=unifastmult(f1,g1);
  
  poly pbig=unifastmult(pAdd(f0,f1),pAdd(g0,g1));
  
  //poly erg=p11;
  
  poly factor=pOne();//pCopy(erg_mult);
  pSetExp(factor,1,n);
  pseudo_len=0;
  kBucket_Add_q(erg_bucket,ppMult_mm(p11,factor),&pseudo_len);
  pseudo_len=0;
  kBucket_Add_q(erg_bucket,pCopy(p00),&pseudo_len);
  pSetExp(factor,1,pot);
  //poly sum=pbig;
  pseudo_len=0;
  kBucket_Add_q(erg_bucket,pMult_mm(pbig,factor),&pseudo_len);
  pseudo_len=0;
  kBucket_Add_q(erg_bucket,pMult_mm(pNeg(p00),factor),&pseudo_len);
  pseudo_len=0;
  kBucket_Add_q(erg_bucket,pMult_mm(pNeg(p11),factor),&pseudo_len);
  //  pSetExp(factor,1,pot);
 
  // sum=pAdd(sum,pNeg(p00));
 

  //sum=pAdd(sum,pNeg(p11));

 
  //sum=pMult_mm(sum,factor);
 
  pseudo_len=0;
  //  kBucket_Add_q(erg_bucket,sum,&pseudo_len);
  //  erg=pAdd(erg,p00);
 
  pDelete(&factor);
  //return(erg);
  //  pDelete(&erg_mult);
  int len=0;
  poly erg=NULL;
  kBucketClear(erg_bucket,&erg,&len);
  kBucketDestroy(&erg_bucket);
  //pNormalize(P.p);
  return erg;
}
poly unifastmult(poly f,poly g){
  if((f==NULL)||(g==NULL)) return NULL;
  int df=pGetExp(f,1);
  int dg=pGetExp(g,1);
  if ((df==1)||(dg==1))
    return ppMult_qq(f,g);
  if (df*dg<100)
    return ppMult_qq(f,g);
  // if (df*dg>10000)
  //  return
  //    do_unifastmult_buckets(f,g);
  //else
  return do_unifastmult(f,g);

}

