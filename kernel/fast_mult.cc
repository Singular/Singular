#include "fast_mult.h"
#include "kbuckets.h"
typedef poly fastmultrec(poly f, poly g, ring r);
static const int pass_option=1;
static int mults=0;
int Mults(){
  return mults;
}
static void degsplit(poly p,int n,poly &p1,poly&p2, int vn, ring r){
  poly erg1_i, erg2_i;
  erg1_i=NULL;
  erg2_i=NULL;
  while(p){
    if(p_GetExp(p,vn,r)>=n){
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
static void div_by_x_power_n(poly p, int n, int vn, ring r){
  while(p){
    assume(p_GetExp(p,vn,r)>=n);
    int e=p_GetExp(p,vn,r);
    p_SetExp(p,vn,e-n,r);
    p=pNext(p);
  }
}

static poly do_unifastmult(poly f,int df,poly g,int dg, int vn, fastmultrec rec, ring r){
  int n=1;
  if ((f==NULL)||(g==NULL)) return NULL;
  //int df=pGetExp(f,vn);//pFDeg(f);
  //  int dg=pGetExp(g,vn);//pFDeg(g);
	
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
    return(pp_Mult_qq(f,g,r));
  }
  
  int pot=n/2;
  assume(pot*2==n);


  //splitting
  poly f1=NULL;
  poly f0=NULL;//f-(x^(pot)*F1);
  degsplit(p_Copy(f,r),pot,f1,f0,vn,r);
  div_by_x_power_n(f1,pot,vn,r);

  poly g1=NULL;
  poly g0=NULL;
  degsplit(p_Copy(g,r),pot,g1,g0,vn,r);
  div_by_x_power_n(g1,pot,vn,r);
  
  //p00, p11
  poly p00=rec(f0,g0,r);//unifastmult(f0,g0);
  poly p11=rec(f1,g1,r);

  //construct erg, factor
  poly erg=NULL;
  poly factor=p_ISet(1,r);

  p_SetExp(factor,vn,n,r);
  erg=pp_Mult_mm(p11,factor,r);
  erg=p_Add_q(erg,p_Copy(p00,r),r);

  


  
  if((f1!=NULL) &&(f0!=NULL) &&(g0!=NULL) && (g1!=NULL)){
    //if(true){
    //eat up f0,f1,g0,g1
    poly s1=p_Add_q(f0,f1,r);
    poly s2=p_Add_q(g0,g1,r);
    poly pbig=rec(s1,s2,r);
    p_Delete(&s1,r);
    p_Delete(&s2,r);

 
    //eat up pbig
    poly sum=pbig;
    p_SetExp(factor,vn,pot,r);
  
    //eat up p00
    sum=p_Add_q(sum,p_Neg(p00,r),r);
 
    //eat up p11
    sum=p_Add_q(sum,p_Neg(p11,r),r);

  
    sum=p_Mult_mm(sum,factor,r);


    //eat up sum
    erg=p_Add_q(sum,erg,r);
  } else {
    //eat up f0,f1,g0,g1 
    poly s1=rec(f0,g1,r);
    poly s2=rec(g0,f1,r);
    p_SetExp(factor,vn,pot,r);
    poly h=p_Mult_mm(((s1!=NULL)?s1:s2),factor,r);
    p_Delete(&f1,r);
    p_Delete(&f0,r);
    p_Delete(&g0,r);
    p_Delete(&g1,r);
    p_Delete(&p00,r);
    p_Delete(&p11,r);
    erg=p_Add_q(erg,h,r);
  }
  
 
  p_Delete(&factor,r);
		  
		  

  return(erg);
}

// poly do_unifastmult_buckets(poly f,poly g){
   
  


//   int n=1;
//   if ((f==NULL)||(g==NULL)) return NULL;
//   int df=pGetExp(f,1);//pFDeg(f);
//     int dg=pGetExp(g,1);//pFDeg(g);
	
//   int dm;
//   if(df>dg){
//     dm=df;
//   }else{
//     dm=dg;
//   }
//   while(n<=dm)
//     {
//       n*=2;
//     }
//   int pseudo_len=0;
//   if(n==1){
//     return ppMult_qq(f,g);

//   }
//   kBucket_pt erg_bucket= kBucketCreate(currRing);
//   kBucketInit(erg_bucket,NULL,0 /*pLength(P.p)*/);

//   int pot=n/2;
//   assume(pot*2==n);


//   //splitting
//   poly f1=NULL;
//   poly f0=NULL;//f-(x^(pot)*F1);
//   degsplit(pCopy(f),pot,f1,f0);
//   div_by_x_power_n(f1,pot);
//   poly g1=NULL;
//   poly g0=NULL;
//   degsplit(pCopy(g),pot,g1,g0);
//   div_by_x_power_n(g1,pot);
  
//   //p00
//   //p11
//   poly p00=unifastmult(f0,g0);
//   poly p11=unifastmult(f1,g1);
  



//   //eat up f0,f1,g0,g1
//   poly pbig=unifastmult(pAdd(f0,f1),pAdd(g0,g1));  
//   poly factor=pOne();//pCopy(erg_mult);
//   pSetExp(factor,1,n);
//   pseudo_len=0;
//   kBucket_Add_q(erg_bucket,ppMult_mm(p11,factor),&pseudo_len);
//   pseudo_len=0;
//   kBucket_Add_q(erg_bucket,pCopy(p00),&pseudo_len);
//   pSetExp(factor,1,pot);

//   //eat up pbig
//   pseudo_len=0;
//   kBucket_Add_q(erg_bucket,pMult_mm(pbig,factor),&pseudo_len);
//   pseudo_len=0;
//   //eat up p00
//   kBucket_Add_q(erg_bucket,pMult_mm(pNeg(p00),factor),&pseudo_len);
//   pseudo_len=0;
//   //eat up p11
//   kBucket_Add_q(erg_bucket,pMult_mm(pNeg(p11),factor),&pseudo_len);

 
//   pseudo_len=0;

 
//   pDelete(&factor);

//   int len=0;
//   poly erg=NULL;
//   kBucketClear(erg_bucket,&erg,&len);
//   kBucketDestroy(&erg_bucket);

//   return erg;
// }

static inline int max(int a, int b){
  return (a>b)? a:b;
}
static inline int min(int a, int b){
  return (a>b)? b:a;
}
poly unifastmult(poly f,poly g, ring r){
  int vn=1;
  if((f==NULL)||(g==NULL)) return NULL;
  int df=p_GetExp(f,vn,r);
  int dg=p_GetExp(g,vn,r);
  if ((df==0)||(dg==0))
    return pp_Mult_qq(f,g,r);
  if (df*dg<100)
    return pp_Mult_qq(f,g,r);
  // if (df*dg>10000)
  //  return
  //    do_unifastmult_buckets(f,g);
  //else
  return do_unifastmult(f,df,g,dg,vn,unifastmult,r);

}

poly multifastmult(poly f, poly g, ring r){
  mults++;
  if((f==NULL)||(g==NULL)) return NULL;
  if (pLength(f)*pLength(g)<100)
    return pp_Mult_qq(f,g,r);
  //find vn
  //determine df,dg simultaneously
  int i;
  int can_i=-1;
  int can_df=0;
  int can_dg=0;
  int can_crit=0;
  for(i=1;i<=rVar(r);i++){
    poly p;
    int df=0;
    int dg=0;
    //max min max Strategie
    p=f;
    while(p){
      df=max(df,p_GetExp(p,i,r));
      p=pNext(p);
    }
    if(df>can_crit){
      p=g;
      while(p){
	dg=max(dg,p_GetExp(p,i,r));
	p=pNext(p);
      }
      int crit=min(df,dg);
      if (crit>can_crit){
	can_crit=crit;
	can_i=i;
	can_df=df;
	can_dg=dg;
      }
    }
  }
  if(can_crit==0)
    return pp_Mult_qq(f,g,r);
  else
    {
      poly erg=do_unifastmult(f,can_df,g,can_dg,can_i,multifastmult,r);
      p_Normalize(erg,r);
      return(erg);
    }
}
