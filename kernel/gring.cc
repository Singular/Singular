/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    gring.cc
 *  Purpose: noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *  Version: $Id: gring.cc,v 1.15 2004-10-13 10:50:37 levandov Exp $
 *******************************************************************/
#include "mod2.h"
#ifdef HAVE_PLURAL
#include "gring.h"
#include "febase.h"
#include "ring.h"
#include "polys.h"
#include "numbers.h"
#include "ideals.h"
#include "matpol.h"
#include "kbuckets.h"
#include "kstd1.h"
#include "sbuckets.h"
#include "prCopy.h"
#include "p_Mult_q.h"

/* global nc_macros : */
#define freeT(A,v) omFreeSize((ADDRESS)A,(v+1)*sizeof(int))
#define freeN(A,k) omFreeSize((ADDRESS)A,k*sizeof(number))

/* poly functions defined in p_Procs : */
poly nc_pp_Mult_mm(poly p, poly m, const ring r, poly &last)
{
  return( nc_p_Mult_mm_Common(p_Copy(p,r), m, 1, r) );
}

poly nc_p_Mult_mm(poly p, const poly m, const ring r)
{
  return( nc_p_Mult_mm_Common(p, m, 1, r) );
}

poly nc_mm_Mult_p(const poly m, poly p, const ring r)
{
  return( nc_p_Mult_mm_Common(p, m, 0, r) );
}

/* poly nc_p_Mult_mm(poly p, poly m, const ring r); defined below */
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r)
{
  poly mc=p_Neg(p_Copy(m,r),r);
  poly mmc=nc_mm_Mult_p(mc,p_Copy(q,r),r);
  p_Delete(&mc,r);
  p=p_Add_q(p,mmc,r);
  return(p);
}

//----------- auxiliary routines--------------------------
poly _nc_p_Mult_q(poly p, poly q, const int copy, const ring r)
  /* destroy p,q unless copy=1 */
{
  poly res=NULL;
  poly ghost=NULL;
  poly qq,pp;
  if (copy)
  {
    qq=p_Copy(q,r);
    pp=p_Copy(p,r);
  }
  else
  {
    qq=q;
    pp=p;
  }
  while (qq!=NULL)
  {
    res=p_Add_q(res, nc_pp_Mult_mm(pp, p_Head(qq,r), r, ghost), r);
    qq=p_LmDeleteAndNext(qq,r);
  }
  p_Delete(&pp,r);
  return(res);
}

poly nc_p_Mult_mm_Common(poly p, const poly m, int side, const ring r)
/* p is poly, m is mono with coeff, destroys p */
/* if side==1, computes p_Mult_mm; otherwise, mm_Mult_p */
{
  if ((p==NULL) || (m==NULL)) return NULL;
  /*  if (pNext(p)==NULL) return(nc_mm_Mult_nn(p,pCopy(m),r)); */
  /* excluded  - the cycle will do it anyway - OK. */
  if (p_IsConstant(m,r)) return(p_Mult_nn(p,p_GetCoeff(m,r),r));

#ifdef PDEBUG
  p_Test(p,r);
  p_Test(m,r);
#endif
  poly v=NULL;
  int rN=r->N;
  int *P=(int *)omAlloc0((rN+1)*sizeof(int));
  int *M=(int *)omAlloc0((rN+1)*sizeof(int));
  /* coefficients: */
  number cP,cM,cOut;
  p_GetExpV(m, M, r);
  cM=p_GetCoeff(m,r);
  /* components:*/
  const int expM=p_GetComp(m,r);
  int expP=0;
  int expOut=0;
  /* bucket constraints: */
  int UseBuckets=1;
  if (pLength(p)< MIN_LENGTH_BUCKET || TEST_OPT_NOT_BUCKETS) UseBuckets=0;
  sBucket_pt bu_out;
  poly out=NULL;
  if (UseBuckets) bu_out=sBucketCreate(r);

  while (p!=NULL)
  {
#ifdef PDEBUG
    p_Test(p,r);
#endif
    expP=p_GetComp(p,r);
    if (expP==0)
    {
      expOut=expM;
    }
    else
    {
      if (expM==0)
      {
        expOut=expP;
#ifdef PDEBUG
	if (side) 
        {
	  Print("Multiplication in the left module from the right");
	}
#endif          
      }
      else
      {
        /* REPORT_ERROR */
	const char* s;
	if (side==1) s="nc_p_Mult_mm";
	else s="nc_mm_Mult_p";
	Print("%s: exponent mismatch %d and %d\n",s,expP,expM);
        expOut=0;
      }
    }
    p_GetExpV(p,P,r);
    cP=p_GetCoeff(p,r);
    cOut=n_Mult(cP,cM,r);
    if (side==1)
    {
      v = nc_mm_Mult_nn(P, M, r);      
    }
    else
    {
      v = nc_mm_Mult_nn(M, P, r);
    }
    v = p_Mult_nn(v,cOut,r);
    p_SetCompP(v,expOut,r);
    if (UseBuckets) sBucket_Add_p(bu_out,v,pLength(v));
    else out = p_Add_q(out,v,r);
    p_DeleteLm(&p,r);
  }
  freeT(P,rN);
  freeT(M,rN);
  if (UseBuckets)
  {
    out = NULL;
    int len = pLength(out);
    sBucketDestroyAdd(bu_out, &out, &len);
  }
#ifdef PDEBUG
  p_Test(out,r);
#endif
  return(out);
}

poly nc_mm_Mult_nn(int *F0, int *G0, const ring r)
/* destroys nothing, no coeffs and exps */
{
  poly out=NULL;
  int i,j;
  int iF,jG,iG;
  int rN=r->N;
  int ExpSize=(((rN+1)*sizeof(int)+sizeof(long)-1)/sizeof(long))*sizeof(long);

  int *F=(int *)omAlloc0((rN+1)*sizeof(int));
  int *G=(int *)omAlloc0((rN+1)*sizeof(int));

  memcpy(F, F0,(rN+1)*sizeof(int));
  // pExpVectorCopy(F,F0);
  memcpy(G, G0,(rN+1)*sizeof(int));
  //  pExpVectorCopy(G,G0);
  F[0]=0; /* important for p_MemAdd */
  G[0]=0;

  iF=rN;
  while ((F[iF]==0)&&(iF>=1)) iF--; /* last exp_num of F */
  if (iF==0) /* F0 is zero vector */
  {
    out=pOne();
    p_SetExpV(out,G0,r);
    p_Setm(out,r);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  }
  jG=1;
  while ((G[jG]==0)&&(jG<rN)) jG++;  /* first exp_num of G */
  iG=rN;
  while ((G[iG]==0)&&(iG>1)) iG--;  /* last exp_num of G */

  out=pOne();

  if (iF<=jG)
    /* i.e. no mixed exp_num , MERGE case */
  {
    p_MemAdd_LengthGeneral(F, G, ExpSize/sizeof(long));
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    //    omFreeSize((ADDRESS)F,ExpSize);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  }

  number cff=n_Init(1,r);
  number tmp_num=NULL;
  int cpower=0;

  if (r->nc->type==nc_skew)
  {
    if (r->nc->IsSkewConstant==1)
    {
      int tpower=0;
      for(j=jG; j<=iG; j++)
      {
        if (G[j]!=0)
        {
          cpower = 0;
          for(i=j+1; i<=iF; i++)
          {
            cpower = cpower + F[i];
          }
          cpower = cpower*G[j];
          tpower = tpower + cpower;
        }
      }
      cff = n_Copy(p_GetCoeff(MATELEM(r->nc->COM,1,2),r),r);
      nPower(cff,tpower,&tmp_num);
      n_Delete(&cff,r);
      cff = tmp_num;
    }
    else /* skew commutative with nonequal coeffs */
    {
      number totcff=n_Init(1,r);
      for(j=jG; j<=iG; j++)
      {
        if (G[j]!=0)
        {
          cpower = 0;
          for(i=j+1; i<=iF; i++)
          {
            if (F[i]!=0)
            {
              cpower = F[i]*G[j];
              cff = n_Copy(p_GetCoeff(MATELEM(r->nc->COM,j,i),r),r);
              nPower(cff,cpower,&tmp_num);
              cff = nMult(totcff,tmp_num);
	      nDelete(&totcff);
              nDelete(&tmp_num);
              totcff = n_Copy(cff,r);
              n_Delete(&cff,r);
            }
          } /* end 2nd for */
        }
      }
      cff=totcff;
    }
    p_MemAdd_LengthGeneral(F, G, ExpSize/sizeof(long));
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    p_SetCoeff(out,cff,r);
    //    p_MemAdd_NegWeightAdjust(p, r); ??? do we need this?
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  } /* end nc_skew */
    
  /* now we have to destroy out! */
  p_Delete(&out,r);  
  out = NULL;  

  if (iG==jG)
    /* g is univariate monomial */
  {
    /*    if (ri->nc->type==nc_skew) -- postpone to TU */
    out = nc_mm_Mult_uu(F,jG,G[jG],r);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  }

  number n1=n_Init(1,r);
  int *Prv=(int *)omAlloc0((rN+1)*sizeof(int));
  int *Nxt=(int *)omAlloc0((rN+1)*sizeof(int));

  int *log=(int *)omAlloc0((rN+1)*sizeof(int));
  int cnt=0; int cnf=0;

  /* splitting F wrt jG */
  for (i=1;i<=jG;i++)
  {
    Prv[i]=F[i]; Nxt[i]=0; /* mult at the very end */
    if (F[i]!=0) cnf++;
  }

  if (cnf==0) freeT(Prv,rN);

  for (i=jG+1;i<=rN;i++)
  {
    Nxt[i]=F[i];
    /*    if (cnf!=0)  Prv[i]=0; */
    if (F[i]!=0)
    {
      cnt++;
    }              /* effective part for F */
  }
  freeT(F,rN);
  cnt=0;

  for (i=1;i<=rN;i++)
  {
    if (G[i]!=0)
    {
     cnt++;
     log[cnt]=i;
     }               /* lG for G */
   }

/* ---------------------- A C T I O N ------------------------ */
  poly D=NULL;
  poly Rout=NULL;
  number *c=(number *)omAlloc0((rN+1)*sizeof(number));
  c[0]=n_Init(1,r);

  int *Op=Nxt;
  int *On=G;
  int *U=(int *)omAlloc0((rN+1)*sizeof(int));

  for (i=jG;i<=rN;i++) U[i]=Nxt[i]+G[i];  /* make leadterm */
  Nxt=NULL;
  G=NULL;
  cnt=1;
  int t=0;
  poly w=NULL;
  poly Pn=pOne();
  p_SetExpV(Pn,On,r);
  p_Setm(Pn,r);

  while (On[iG]!=0)
  {
     t=log[cnt];

     w=nc_mm_Mult_uu(Op,t,On[t],r);
     c[cnt]=n_Mult(c[cnt-1],p_GetCoeff(w,r),r);
     D = pNext(w);  /* getting coef and rest D */
     p_DeleteLm(&w,r);
     w=NULL;

     Op[t] += On[t];   /* update exp_vectors */
     On[t] = 0;

     if (t!=iG)    /* not the last step */
     {
       p_SetExpV(Pn,On,r);
       p_Setm(Pn,r);
#ifdef PDEBUG
       p_Test(Pn,r);
#endif

//       if (pNext(D)==0)
// is D a monomial? could be postponed higher
//       {
//       Rout=nc_mm_Mult_nn(D,Pn,r);
//       }
//       else
//       {
       Rout=nc_p_Mult_mm(D,Pn,r);
//       }
     }
     else
     {
       Rout=D;
       D=NULL;
     }

     if (Rout!=NULL)
     {
       Rout=p_Mult_nn(Rout,c[cnt-1],r); /* Rest is ready */
       out=p_Add_q(out,Rout,r);
       Rout=NULL;
     }
     cnt++;
  }
  freeT(On,rN);
  freeT(Op,rN);
  p_Delete(&Pn,r);
  omFreeSize((ADDRESS)log,(rN+1)*sizeof(int));

  /* leadterm and Prv-part */

  Rout=pOne();
  /* U is lead.monomial */
  U[0]=0;
  p_SetExpV(Rout,U,r);
  p_Setm(Rout,r);  /* use again this name Rout */
#ifdef PDEBUG
  p_Test(Rout,r);
#endif
  p_SetCoeff(Rout,c[cnt-1],r);
  out=p_Add_q(out,Rout,r);
  freeT(U,rN);
  freeN(c,rN+1);
  if (cnf!=0)  /* Prv is non-zero vector */
  {
    Rout=pOne();
    Prv[0]=0;
    p_SetExpV(Rout,Prv,r);
    p_Setm(Rout,r);
#ifdef PDEBUG
    p_Test(Rout,r);
#endif
    out=nc_mm_Mult_p(Rout,out,r); /* getting the final result */
    freeT(Prv,rN);
    p_Delete(&Rout,r);
  }
  return (out);
}


poly nc_mm_Mult_uu(int *F,int jG,int bG, const ring r)
/* f=mono(F),g=(x_iG)^bG */
{
  poly out=NULL;
  int i;
  number num=NULL;

  int rN=r->N;
  int iF=r->N;
  while ((F[iF]==0)&&(iF>0)) iF-- ;   /* last exponent_num of F */

  if (iF==0)  /* F==zero vector in other words */
  {
   out=pOne();
   p_SetExp(out,jG,bG,r);
   p_Setm(out,r);
   return(out);
  }

  int jF=1;
  while ((F[jF]==0)&&(jF<=rN)) jF++;  /* first exp of F */

  if (iF<=jG)                       /* i.e. no mixed exp_num */
  {
    out=pOne();
    F[jG]=F[jG]+bG;
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    return(out);
  }

  if (iF==jF)              /* uni times uni */
  {
   out=nc_uu_Mult_ww(iF,F[iF],jG,bG,r);
   return(out);
  }

  /* Now: F is mono with >=2 exponents, jG<iF */
  /* check the quasi-commutative case */
//   matrix LCOM=r->nc->COM;
//   number rescoef=n_Init(1,r);
//   number tmpcoef=n_Init(1,r);
//   int tmpint;
//   i=iF;
//   while (i>=jG+1)
//     /* all the non-zero exponents */
//   {
//     if (MATELEM(LCOM,jG,i)!=NULL)
//     {
//       tmpcoef=pGetCoeff(MATELEM(LCOM,jG,i));
//       tmpint=(int)F[i];
//       nPower(tmpcoef,F[i],&tmpcoef);
//       rescoef=nMult(rescoef,tmpcoef);
//       i--;
//     }
//     else
//     {
//       if (F[i]!=0) break;
//     }
//   }
//   if (iF==i)
//   /* no action took place*/
//   {

//   }
//   else /* power the result up to bG */
//   {
//     nPower(rescoef,bG,&rescoef);
//     /* + cleanup, post-processing */
//   }

  int *Prv=(int*)omAlloc0((rN+1)*sizeof(int));
  int *Nxt=(int*)omAlloc0((rN+1)*sizeof(int));
  int *lF=(int *)omAlloc0((rN+1)*sizeof(int));
  int cnt=0; int cnf=0;
  /* splitting F wrt jG */
  for (i=1;i<=jG;i++) /* mult at the very end */
  {
    Prv[i]=F[i]; Nxt[i]=0;
    if (F[i]!=0) cnf++;
  }
  if (cnf==0)  freeT(Prv,rN);
  for (i=jG+1;i<=rN;i++)
  {
    Nxt[i]=F[i];
    if (cnf!=0) { Prv[i]=0;}
    if (F[i]!=0)
    {
      cnt++;
      lF[cnt]=i;
    }                 /* eff_part,lF_for_F */
  }

  if (cnt==1) /* Nxt consists of 1 nonzero el-t only */
  {
    int q=lF[1];
    poly Rout=pOne();
    out=nc_uu_Mult_ww(q,Nxt[q],jG,bG,r);
    freeT(Nxt,rN);

    if (cnf!=0)
    {
       Prv[0]=0;
       p_SetExpV(Rout,Prv,r);
       p_Setm(Rout,r);
#ifdef PDEBUG
       p_Test(Rout,r);
#endif
       freeT(Prv,rN);
       out=nc_mm_Mult_p(Rout,out,r); /* getting the final result */
    }

    omFreeSize((ADDRESS)lF,(rN+1)*sizeof(int));
    p_Delete(&Rout,r);
    return (out);
  }
/* -------------------- MAIN ACTION --------------------- */

  poly D=NULL;
  poly Rout=NULL;
  number *c=(number *)omAlloc0((cnt+2)*sizeof(number));
  c[cnt+1]=n_Init(1,r);
  i=cnt+2;         /* later in freeN */
  int *Op=Nxt;
  int *On=(int *)omAlloc0((rN+1)*sizeof(int));
  int *U=(int *)omAlloc0((rN+1)*sizeof(int));


  //  pExpVectorCopy(U,Nxt);
  memcpy(U, Nxt,(rN+1)*sizeof(int));
  U[jG] = U[jG] + bG;

  /* Op=Nxt and initial On=(0); */
  Nxt=NULL;

  poly Pp;
  poly Pn;
  int t=0;
  int first=lF[1];
  int nlast=lF[cnt];
  int kk=0;
  /*  cnt--;   */
  /* now lF[cnt] should be <=iF-1 */

  while (Op[first]!=0)
  {
     t=lF[cnt];   /* cnt as it was computed */

     poly w=nc_uu_Mult_ww(t,Op[t],jG,bG,r);
     c[cnt]=n_Copy(p_GetCoeff(w,r),r);
     D = pNext(w);  /* getting coef and rest D */
     p_DeleteLm(&w,r);
     w=NULL;

     Op[t]= 0;
     Pp=pOne();
     p_SetExpV(Pp,Op,r);
     p_Setm(Pp,r);

     if (t<nlast)
     {
       kk=lF[cnt+1];
       On[kk]=F[kk];

       Pn=pOne();
       p_SetExpV(Pn,On,r);
       p_Setm(Pn,r);

       if (t!=first)   /* typical expr */
       {
         w=nc_p_Mult_mm(D,Pn,r);
         Rout=nc_mm_Mult_p(Pp,w,r);
         w=NULL;
       }
       else                   /* last step */
       {
         On[t]=0;
         p_SetExpV(Pn,On,r);
         p_Setm(Pn,r);
         Rout=nc_p_Mult_mm(D,Pn,r);
       }
#ifdef PDEBUG
       p_Test(Pp,r);
#endif
       p_Delete(&Pn,r);
     }
     else                     /* first step */
     {
       Rout=nc_mm_Mult_p(Pp,D,r);
     }
#ifdef PDEBUG
     p_Test(Pp,r);
#endif
     p_Delete(&Pp,r);
     num=n_Mult(c[cnt+1],c[cnt],r);
     n_Delete(&c[cnt],r);
     c[cnt]=num;
     Rout=p_Mult_nn(Rout,c[cnt+1],r); /* Rest is ready */
     out=p_Add_q(out,Rout,r);
     Pp=NULL;
     cnt--;
  }
  /* only to feel safe:*/
  Pn=Pp=NULL;
  freeT(On,rN);
  freeT(Op,rN);

/* leadterm and Prv-part with coef 1 */
/*  U[0]=exp; */
/*  U[jG]=U[jG]+bG;  */
/* make leadterm */
/* ??????????? we have done it already :-0 */
  Rout=pOne();
  p_SetExpV(Rout,U,r);
  p_Setm(Rout,r);  /* use again this name */
  p_SetCoeff(Rout,c[cnt+1],r);  /* last computed coef */
  out=p_Add_q(out,Rout,r);
  Rout=NULL;
  freeT(U,rN);
  freeN(c,i);
  omFreeSize((ADDRESS)lF,(rN+1)*sizeof(int));

  if (cnf!=0)
  {
    Rout=pOne();
    p_SetExpV(Rout,Prv,r);
    p_Setm(Rout,r);
    freeT(Prv,rN);
    out=nc_mm_Mult_p(Rout,out,r); /* getting the final result */
    p_Delete(&Rout,r);
  }
  return (out);
}

poly nc_uu_Mult_ww_vert (int i, int a, int j, int b, const ring r)
{
  int k,m;
  int rN=r->N;
  matrix cMT=r->nc->MT[UPMATELEM(j,i,rN)];         /* cMT=current MT */

  poly x=pOne();p_SetExp(x,j,1,r);p_Setm(x,r);
/* var(j); */
  poly y=pOne();p_SetExp(y,i,1,r);p_Setm(y,r);
/*var(i);  for convenience */
#ifdef PDEBUG
  p_Test(x,r);
  p_Test(y,r);
#endif
  poly t=NULL;
/* ------------ Main Cycles ----------------------------*/

  for (k=2;k<=a;k++)
  {
     t = nc_p_CopyGet(MATELEM(cMT,k,1),r);

     if (t==NULL)   /* not computed yet */
     {
       t = nc_p_CopyGet(MATELEM(cMT,k-1,1),r);
       //        t=p_Copy(MATELEM(cMT,k-1,1),r);
       t = nc_mm_Mult_p(y,t,r);
       MATELEM(cMT,k,1) = nc_p_CopyPut(t,r);
       //        omCheckAddr(cMT->m);
       p_Delete(&t,r);
     }
     t=NULL;
  }

  for (m=2;m<=b;m++)
  {
    t = nc_p_CopyGet(MATELEM(cMT,a,m),r);
    //     t=MATELEM(cMT,a,m);
    if (t==NULL)   //not computed yet
    {
      t = nc_p_CopyGet(MATELEM(cMT,a,m-1),r);
      //      t=p_Copy(MATELEM(cMT,a,m-1),r);
      t = nc_p_Mult_mm(t,x,r);
      MATELEM(cMT,a,m) = nc_p_CopyPut(t,r);
      //      MATELEM(cMT,a,m) = t;
      //        omCheckAddr(cMT->m);
      p_Delete(&t,r);
    }
    t=NULL;
  }
  p_Delete(&x,r);
  p_Delete(&y,r);
  //  t=MATELEM(cMT,a,b);
  t= nc_p_CopyGet(MATELEM(cMT,a,b),r);
  //  return(p_Copy(t,r));
  /* since the last computed element was cMT[a,b] */
  return(t);
}

poly nc_uu_Mult_ww (int i, int a, int j, int b, const ring r)
  /* (x_i)^a times (x_j)^b */
  /* x_i = y,  x_j = x ! */
{
  /* Check zero exceptions, (q-)commutativity and is there something to do? */
  assume(a!=0);
  assume(b!=0);
  poly out=pOne();
  if (i<=j)
  {
    p_SetExp(out,i,a,r);
    p_AddExp(out,j,b,r);
    p_Setm(out,r);
    return(out);
  }/* zero exeptions and usual case */
  /*  if ((a==0)||(b==0)||(i<=j)) return(out); */

  if (MATELEM(r->nc->COM,j,i)!=NULL)
    /* commutative or quasicommutative case */
  {
    p_SetExp(out,i,a,r);
    p_AddExp(out,j,b,r);
    p_Setm(out,r);
    if (r->cf->nIsOne(p_GetCoeff(MATELEM(r->nc->COM,j,i),r))) /* commutative case */
    {
      return(out);
    }
    else
    {
      number tmp_number=p_GetCoeff(MATELEM(r->nc->COM,j,i),r); /* quasicommutative case */
      nPower(tmp_number,a*b,&tmp_number);
      p_SetCoeff(out,tmp_number,r);
      return(out);
    }
  }/* end_of commutative or quasicommutative case */
  p_Delete(&out,r);

  /* we are here if  i>j and variables do not commute or quasicommute */
  /* in fact, now a>=1 and b>=1; and j<i */
  /* now check whether the polynomial is already computed */
  int rN=r->N;
  int vik = UPMATELEM(j,i,rN);
  int cMTsize=r->nc->MTsize[vik];
  int newcMTsize=0;
  newcMTsize=si_max(a,b);

  if (newcMTsize<=cMTsize)
  {
    out =  nc_p_CopyGet(MATELEM(r->nc->MT[vik],a,b),r);
    if (out !=NULL) return (out);
  }
  int k,m;
  if (newcMTsize > cMTsize)
  {
    int inM=(((newcMTsize+6)/7)*7);
    assume (inM>=newcMTsize);
    newcMTsize = inM;
    //    matrix tmp = (matrix)omAlloc0(inM*inM*sizeof(poly));
    matrix tmp = mpNew(newcMTsize,newcMTsize);

    for (k=1;k<=cMTsize;k++)
    {
      for (m=1;m<=cMTsize;m++)
      {
	out = MATELEM(r->nc->MT[UPMATELEM(j,i,rN)],k,m);
        if ( out != NULL )
        {
          MATELEM(tmp,k,m) = out;/*MATELEM(r->nc->MT[UPMATELEM(j,i,rN)],k,m)*/
          //           omCheckAddr(tmp->m);
          MATELEM(r->nc->MT[UPMATELEM(j,i,rN)],k,m)=NULL;
          //           omCheckAddr(r->nc->MT[UPMATELEM(j,i,rN)]->m);
        }
      }
    }
    id_Delete((ideal *)&(r->nc->MT[UPMATELEM(j,i,rN)]),r);
    r->nc->MT[UPMATELEM(j,i,rN)] = tmp;
    tmp=NULL;
    r->nc->MTsize[UPMATELEM(j,i,rN)] = newcMTsize;
  }
  /* The update of multiplication matrix is finished */
    pDelete(&out);
    out = nc_uu_Mult_ww_vert(i, a, j, b, r);
    //    out = nc_uu_Mult_ww_horvert(i, a, j, b, r);
    return(out);
}

poly nc_uu_Mult_ww_horvert (int i, int a, int j, int b, const ring r)

{
  int k,m;
  int rN=r->N;
  matrix cMT=r->nc->MT[UPMATELEM(j,i,rN)];         /* cMT=current MT */

  poly x=pOne();p_SetExp(x,j,1,r);p_Setm(x,r);/* var(j); */
  poly y=pOne();p_SetExp(y,i,1,r);p_Setm(y,r); /*var(i);  for convenience */
#ifdef PDEBUG
  p_Test(x,r);
  p_Test(y,r);
#endif

  poly t=NULL;

  int toXY;
  int toYX;

  if (a==1) /* y*x^b, b>=2 */
  {
    toXY=b-1;
    while ( (MATELEM(cMT,1,toXY)==NULL) && (toXY>=2)) toXY--;
    for (m=toXY+1;m<=b;m++)
    {
      t=MATELEM(cMT,1,m);
      if (t==NULL)   /* remove after debug */
      {
        t = p_Copy(MATELEM(cMT,1,m-1),r);
        t = nc_p_Mult_mm(t,x,r);
        MATELEM(cMT,1,m) = t;
        /*        omCheckAddr(cMT->m); */
      }
      else
      {
        /* Error, should never get there */
        WarnS("Error: a=1; MATELEM!=0");
      }
      t=NULL;
    }
    return(p_Copy(MATELEM(cMT,1,b),r));
  }

  if (b==1) /* y^a*x, a>=2 */
  {
    toYX=a-1;
    while ( (MATELEM(cMT,toYX,1)==NULL) && (toYX>=2)) toYX--;
    for (m=toYX+1;m<=a;m++)
    {
      t=MATELEM(cMT,m,1);
      if (t==NULL)   /* remove after debug */
      {
        t = p_Copy(MATELEM(cMT,m-1,1),r);
        t = nc_mm_Mult_p(y,t,r);
        MATELEM(cMT,m,1) = t;
        /*        omCheckAddr(cMT->m); */
      }
      else
      {
        /* Error, should never get there */
        WarnS("Error: b=1, MATELEM!=0");
      }
      t=NULL;
    }
    return(p_Copy(MATELEM(cMT,a,1),r));
  }

/* ------------ Main Cycles ----------------------------*/
  /*            a>1, b>1              */

  int dXY=0; int dYX=0;
  /* dXY = distance for computing x-mult, then y-mult */
  /* dYX = distance for computing y-mult, then x-mult */
  int toX=a-1; int toY=b-1; /* toX = to axe X, toY = to axe Y */
  toXY=b-1; toYX=a-1;
  /* if toX==0, toXY = dist. to computed y * x^toXY */
  /* if toY==0, toYX = dist. to computed y^toYX * x */
  while ( (MATELEM(cMT,toX,b)==NULL) && (toX>=1)) toX--;
  if (toX==0) /* the whole column is not computed yet */
  {
    while ( (MATELEM(cMT,1,toXY)==NULL) && (toXY>=1)) toXY--;
    /* toXY >=1 */
    dXY=b-1-toXY;
  }
  dXY=dXY+a-toX; /* the distance to nearest computed y^toX x^b */

  while ( (MATELEM(cMT,a,toY)==NULL) && (toY>=1)) toY--;
  if (toY==0) /* the whole row is not computed yet */
  {
    while ( (MATELEM(cMT,toYX,1)==NULL) && (toYX>=1)) toYX--;
    /* toYX >=1 */
    dYX=a-1-toYX;
  }
  dYX=dYX+b-toY; /* the distance to nearest computed y^a x^toY */

  if (dYX>=dXY)
  {
    /* first x, then y */
    if (toX==0) /* start with the row*/
    {
      for (m=toXY+1;m<=b;m++)
      {
        t=MATELEM(cMT,1,m);
        if (t==NULL)   /* remove after debug */
        {
          t = p_Copy(MATELEM(cMT,1,m-1),r);
          t = nc_p_Mult_mm(t,x,r);
          MATELEM(cMT,1,m) = t;
          /*        omCheckAddr(cMT->m); */
        }
        else
        {
          /* Error, should never get there */
          WarnS("dYX>=dXY,toXY; MATELEM==0");
        }
        t=NULL;
      }
      toX=1; /* y*x^b is computed */
    }
    /* Now toX>=1 */
    for (k=toX+1;k<=a;k++)
    {
      t=MATELEM(cMT,k,b);
      if (t==NULL)   /* remove after debug */
      {
        t = p_Copy(MATELEM(cMT,k-1,b),r);
        t = nc_mm_Mult_p(y,t,r);
        MATELEM(cMT,k,b) = t;
        /*        omCheckAddr(cMT->m); */
      }
      else
      {
        /* Error, should never get there */
        WarnS("dYX>=dXY,toX; MATELEM==0");
      }
      t=NULL;
    }
  } /* endif (dYX>=dXY) */


  if (dYX<dXY)
  {
    /* first y, then x */
    if (toY==0) /* start with the column*/
    {
      for (m=toYX+1;m<=a;m++)
      {
        t=MATELEM(cMT,m,1);
        if (t==NULL)   /* remove after debug */
        {
          t = p_Copy(MATELEM(cMT,m-1,1),r);
          t = nc_mm_Mult_p(y,t,r);
          MATELEM(cMT,m,1) = t;
          /*        omCheckAddr(cMT->m); */
        }
        else
        {
          /* Error, should never get there */
          WarnS("dYX<dXY,toYX; MATELEM==0");
        }
        t=NULL;
      }
      toY=1; /* y^a*x is computed */
    }
    /* Now toY>=1 */
    for (k=toY+1;k<=b;k++)
    {
      t=MATELEM(cMT,a,k);
      if (t==NULL)   /* remove after debug */
      {
        t = p_Copy(MATELEM(cMT,a,k-1),r);
        t = nc_p_Mult_mm(t,x,r);
        MATELEM(cMT,a,k) = t;
        /*        omCheckAddr(cMT->m); */
      }
      else
      {
        /* Error, should never get there */
        WarnS("dYX<dXY,toY; MATELEM==0");
      }
      t=NULL;
    }
  } /* endif (dYX<dXY) */

  p_Delete(&x,r);
  p_Delete(&y,r);
  t=p_Copy(MATELEM(cMT,a,b),r);
  return(t);  /* since the last computed element was cMT[a,b] */
}


/* ----------------------------- Syzygies ---------------------- */

/*2
* reduction of p2 with p1
* do not destroy p1, but p2
* p1 divides p2 -> for use in NF algorithm
*/

poly nc_ReduceSpoly(poly p1, poly p2,poly spNoether, const ring r)
{
  if (p_GetComp(p1,r)!=p_GetComp(p2,r)
  && (p_GetComp(p1,r)!=0)
  && (p_GetComp(p2,r)!=0))
  {
#ifdef PDEBUG
    Print("nc_ReduceSpoly: different components");
#endif
    return(NULL);
  }
  poly m = pOne();
  p_ExpVectorDiff(m,p2,p1,r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif
  /* pSetComp(m,r)=0? */
  poly   N  = nc_mm_Mult_p(m, p_Head(p1,r), r);
  number C  = n_Copy( p_GetCoeff(N,  r), r);
  number cF = n_Copy( p_GetCoeff(p2, r),r);
  /* GCD stuff */
  number cG = nGcd(C, cF, r);
  if ( !nEqual(cG, n_Init(1,r) ) )
  {
    cF = nDiv(cF, cG);
    C  = nDiv(C,  cG);
  }
  p2 = p_Mult_nn(p2, C, r);
  poly out = nc_mm_Mult_p(m, p_Copy(pNext(p1),r), r);
  N = p_Add_q(N, out, r);
  p_Test(p2,r);
  p_Test(N,r);
  number MinusOne = n_Init(-1,r);
  if (!n_Equal(cF,MinusOne,r))
  {
    cF = n_Neg(cF,r);
    N  = p_Mult_nn(N, cF, r);
    p_Test(N,r);
  }
  out = p_Add_q(p2,N,r);
  p_Test(out,r);
  if ( out!=NULL ) pContent(out);
  p_Delete(&m,r);
  n_Delete(&cF,r);
  n_Delete(&C,r);
  n_Delete(&MinusOne,r);
  return(out);
}


/*3
* reduction of p2 with p1
* do not destroy p1 and p2
* p1 divides p2 -> for use in NF algorithm
*/
poly nc_ReduceSpolyNew(poly p1, poly p2,poly spNoether, const ring r)
{
  return(nc_ReduceSpoly(p1,p_Copy(p2,r),spNoether,r));
}

/*4
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
poly nc_CreateSpoly(poly p1, poly p2,poly spNoether, const ring r)
{
  if ((p_GetComp(p1,r)!=p_GetComp(p2,r))
  && (p_GetComp(p1,r)!=0)
  && (p_GetComp(p2,r)!=0))
  {
#ifdef PDEBUG
    Print("nc_CreateSpoly : different components!");
#endif
    return(NULL);
  }
  if ((r->nc->type==nc_lie) && pHasNotCF(p1,p2)) /* prod crit */
  {
    return(nc_p_Bracket_qq(pCopy(p2),p1));
  }
  poly pL=pOne();
  poly m1=pOne();
  poly m2=pOne();
  pLcm(p1,p2,pL);
  p_Setm(pL,r);
#ifdef PDEBUG
  p_Test(pL,r);
#endif
  p_ExpVectorDiff(m1,pL,p1,r);
  //p_SetComp(m1,0,r);
  //p_Setm(m1,r);
#ifdef PDEBUG
  p_Test(m1,r);
#endif
  p_ExpVectorDiff(m2,pL,p2,r);
  //p_SetComp(m2,0,r);
  //p_Setm(m2,r);
#ifdef PDEBUG
  p_Test(m2,r);
#endif
  p_Delete(&pL,r);
  /* zero exponents ! */
  poly M1    = nc_mm_Mult_p(m1,p_Head(p1,r),r);
  number C1  = n_Copy(p_GetCoeff(M1,r),r);
  poly M2    = nc_mm_Mult_p(m2,p_Head(p2,r),r);
  number C2  = n_Copy(p_GetCoeff(M2,r),r);
  /* GCD stuff */
  number C = nGcd(C1,C2,r);
  if (!nEqual(C,n_Init(1,r)))
  {
    C1=nDiv(C1,C);
    C2=nDiv(C2,C);
  }
  M1=p_Mult_nn(M1,C2,r);
  p_SetCoeff(m1,C2,r);
  number MinusOne=n_Init(-1,r);
  if (n_Equal(C1,MinusOne,r))
  {
    M2=p_Add_q(M1,M2,r);
  }
  else
  {
    C1=n_Neg(C1,r);
    M2=p_Mult_nn(M2,C1,r);
    M2=p_Add_q(M1,M2,r);
    p_SetCoeff(m2,C1,r);
  }
  /* M1 is killed, M2=res = C2 M1 - C1 M2 */
  poly tmp=p_Copy(p1,r);
  tmp=p_LmDeleteAndNext(tmp,r);
  M1=nc_mm_Mult_p(m1,tmp,r);
  tmp=p_Copy(p2,r);
  tmp=p_LmDeleteAndNext(tmp,r);
  M2=p_Add_q(M2,M1,r);
  M1=nc_mm_Mult_p(m2,tmp,r);
  M2=p_Add_q(M2,M1,r);
  p_Delete(&m1,r);
  p_Delete(&m2,r);
  //  n_Delete(&C1,r);
  //  n_Delete(&C2,r);
  n_Delete(&MinusOne,r);
#ifdef PDEBUG
  p_Test(M2,r);
#endif
  if (M2!=NULL) pContent(M2);
  return(M2);
}

/*5
* reduction of tail(q) with p1
* lead(p1) divides lead(pNext(q2)) and pNext(q2) is reduced
* do not destroy p1, but tail(q)
*/
void nc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r)
{
  poly a1=p_Head(p1,r);
  poly Q=pNext(q2);
  number cQ=p_GetCoeff(Q,r);
  poly m=pOne();
  p_ExpVectorDiff(m,Q,p1,r);
  //  p_SetComp(m,0,r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif
  /* pSetComp(m,r)=0? */
  poly M=nc_mm_Mult_p(m,p_Copy(p1,r),r);
  number C=p_GetCoeff(M,r);
  M=p_Add_q(M,nc_mm_Mult_p(m,p_LmDeleteAndNext(p_Copy(p1,r),r),r),r);
  q=p_Mult_nn(q,C,r);
  number MinusOne=n_Init(-1,r);
  if (!n_Equal(cQ,MinusOne,r))
  {
    cQ=nNeg(cQ);
    M=p_Mult_nn(M,cQ,r);
  }
  Q=p_Add_q(Q,M,r);
  pNext(q2)=Q;

  p_Delete(&m,r);
  n_Delete(&C,r);
  n_Delete(&cQ,r);
  n_Delete(&MinusOne,r);
  /*  return(q); */
}

/*6
* creates the commutative lcm(lm(p1),lm(p2))
* do not destroy p1 and p2
*/
poly nc_CreateShortSpoly(poly p1, poly p2, const ring r)
{
  if (p_GetComp(p1,r)!=p_GetComp(p2,r))
  {
    Print("spShort:exponent mismatch!");
    return(NULL);
  }
  poly m=pOne();
  pLcm(p1,p2,m);
  p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif
  return(m);
}

void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c)
{
  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  *c=nInit(1);
  poly m=pOne();
  pExpVectorDiff(m,kBucketGetLm(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif
  poly pp=nc_mm_Mult_p(m,pCopy(p),currRing);
  pDelete(&m);
  number n=nCopy(pGetCoeff(pp));
  number MinusOne=nInit(-1);
  number nn;
  if (!nEqual(n,MinusOne))
  {
    nn=nNeg(nInvers(n));
  }
  else nn=nInit(1);
  nDelete(&n);
  n=nMult(nn,pGetCoeff(kBucketGetLm(b)));
  nDelete(&nn);
  pp=p_Mult_nn(pp,n,currRing);
  nDelete(&n);
  nDelete(&MinusOne);
  int l=pLength(pp);
  kBucket_Add_q(b,pp,&l);
}

void nc_PolyPolyRed(poly &b, poly p, number *c)
  // reduces b with p, do not delete both
{
  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  *c=nInit(1);
  poly m=pOne();
  pExpVectorDiff(m,pHead(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif
  poly pp=nc_mm_Mult_p(m,pCopy(p),currRing);
  pDelete(&m);
  number n=nCopy(pGetCoeff(pp));
  number MinusOne=nInit(-1);
  number nn;
  if (!nEqual(n,MinusOne))
  {
    nn=nNeg(nInvers(n));
  }
  else nn=nInit(1);
  nDelete(&n);
  n=nMult(nn,pGetCoeff(b));
  nDelete(&nn);
  pp=p_Mult_nn(pp,n,currRing);
  nDelete(&n);
  nDelete(&MinusOne);
  b=p_Add_q(b,pp,currRing);
}

poly nc_p_Bracket_qq(poly p, poly q)
  /* returns [p,q], destroys p */
{
  if (!rIsPluralRing(currRing)) return(NULL);
  if (pComparePolys(p,q)) return(NULL);
  /* Components !? */
  poly Q=NULL;
  number coef=NULL;
  poly res=NULL;
  poly pres=NULL;
  int UseBuckets=1;
  if ((pLength(p)< MIN_LENGTH_BUCKET/2) && (pLength(q)< MIN_LENGTH_BUCKET/2) || TEST_OPT_NOT_BUCKETS) UseBuckets=0;
  sBucket_pt bu_out;
  if (UseBuckets) bu_out=sBucketCreate(currRing);
  while (p!=NULL)
  {
    Q=q;
    while(Q!=NULL)
    {
      pres=nc_mm_Bracket_nn(p,Q); /* since no coeffs are taken into account there */
      if (pres!=NULL)
      {
        coef = nMult(pGetCoeff(p),pGetCoeff(Q));
        pres = p_Mult_nn(pres,coef,currRing);
	if (UseBuckets) sBucket_Add_p(bu_out,pres,pLength(pres));
	else res=p_Add_q(res,pres,currRing);
        nDelete(&coef);
      }
      pIter(Q);
    }
    p=pLmDeleteAndNext(p);
  }
  if (UseBuckets)
  {
    res = NULL;
    int len = pLength(res);
    sBucketDestroyAdd(bu_out, &res, &len);
  }
  return(res);
}

poly nc_mm_Bracket_nn(poly m1, poly m2)
  /*returns [m1,m2] for two monoms, destroys nothing */
  /* without coeffs */
{
  if (pLmIsConstant(m1) || pLmIsConstant(m1)) return(NULL);
  if (pLmCmp(m1,m2)==0) return(NULL);
  int rN=currRing->N;
  int *M1=(int *)omAlloc0((rN+1)*sizeof(int));
  int *M2=(int *)omAlloc0((rN+1)*sizeof(int));
  int *PREFIX=(int *)omAlloc0((rN+1)*sizeof(int));
  int *SUFFIX=(int *)omAlloc0((rN+1)*sizeof(int));
  pGetExpV(m1,M1);
  pGetExpV(m2,M2);
  poly res=NULL;
  poly ares=NULL;
  poly bres=NULL;
  poly prefix=NULL;
  poly suffix=NULL;
  int nMin,nMax;
  number nTmp=NULL;
  int i,j,k;
  for (i=1;i<=rN;i++)
  {
    if (M2[i]!=0)
    {
      ares=NULL;
      for (j=1;j<=rN;j++)
      {
        if (M1[j]!=0)
        {
          bres=NULL;
          /* compute [ x_j^M1[j],x_i^M2[i] ] */
          if (i<j) {nMax=j;  nMin=i;} else {nMax=i;  nMin=j;}
          if ( (i==j) || ((MATELEM(currRing->nc->COM,nMin,nMax)!=NULL) && nIsOne(pGetCoeff(MATELEM(currRing->nc->C,nMin,nMax))) )) /* not (the same exp. or commuting exps)*/
          { bres=NULL; }
          else
          {
            if (i<j) { bres=nc_uu_Mult_ww(j,M1[j],i,M2[i],currRing); }
            else bres=nc_uu_Mult_ww(i,M2[i],j,M1[j],currRing);
            if (nIsOne(pGetCoeff(bres)))
            {
              bres=pLmDeleteAndNext(bres);
            }
            else
            {
              nTmp=nSub(pGetCoeff(bres),nInit(1));
              pSetCoeff(bres,nTmp); /* only lc ! */
            }
#ifdef PDEBUG
            pTest(bres);
#endif
            if (i>j)  bres=p_Neg(bres, currRing);
          }
          if (bres!=NULL)
          {
            /* now mult (prefix, bres, suffix) */
            memcpy(SUFFIX, M1,(rN+1)*sizeof(int));
            memcpy(PREFIX, M1,(rN+1)*sizeof(int));
            for (k=1;k<=j;k++) SUFFIX[k]=0;
            for (k=j;k<=rN;k++) PREFIX[k]=0;
            SUFFIX[0]=0;
            PREFIX[0]=0;
            prefix=pOne();
            suffix=pOne();
            pSetExpV(prefix,PREFIX);
            pSetm(prefix);
            pSetExpV(suffix,SUFFIX);
            pSetm(suffix);
            if (!pLmIsConstant(prefix)) bres = nc_mm_Mult_p(prefix, bres,currRing);
            if (!pLmIsConstant(suffix)) bres = nc_p_Mult_mm(bres, suffix,currRing);
            ares=p_Add_q(ares, bres,currRing);
            /* What to give free? */
	    /* Do we have to free PREFIX/SUFFIX? it seems so */
            pDelete(&prefix);
            pDelete(&suffix);
          }
        }
      }
      if (ares!=NULL)
      {
        /* now mult (prefix, bres, suffix) */
        memcpy(SUFFIX, M2,(rN+1)*sizeof(int));
        memcpy(PREFIX, M2,(rN+1)*sizeof(int));
        for (k=1;k<=i;k++) SUFFIX[k]=0;
        for (k=i;k<=rN;k++) PREFIX[k]=0;
        SUFFIX[0]=0;
        PREFIX[0]=0;
        prefix=pOne();
        suffix=pOne();
        pSetExpV(prefix,PREFIX);
        pSetm(prefix);
        pSetExpV(suffix,SUFFIX);
        pSetm(suffix);
        bres=ares;
        if (!pLmIsConstant(prefix)) bres = nc_mm_Mult_p(prefix, bres,currRing);
        if (!pLmIsConstant(suffix)) bres = nc_p_Mult_mm(bres, suffix,currRing);
        res=p_Add_q(res, bres,currRing);
        pDelete(&prefix);
        pDelete(&suffix);
      }
    }
  }
  freeT(M1, rN);
  freeT(M2, rN);
  freeT(PREFIX, rN);
  freeT(SUFFIX, rN);
  pTest(res);
  return(res);
}

ideal twostd(ideal I)
{
  int i;
  int j;
  int s;
  int flag;
  poly p=NULL;
  poly q=NULL;
  ideal J=kStd(I, currQuotient,testHomog,NULL,NULL,0,0,NULL);
  idSkipZeroes(J);
  ideal K=NULL;
  poly varj=NULL;
  ideal Q=NULL;
  ideal id_tmp=NULL;
  int rN=currRing->N;
  int iSize=0;
  loop
  {
    flag=0;
    K=NULL;
    s=idElem(J);
    for (i=0;i<=s-1;i++)
    {
      p=J->m[i];
      for (j=1;j<=rN;j++)
      {
        varj = pOne();
        pSetExp(varj,j,1);
        pSetm(varj);
        q = nc_p_Mult_mm(pCopy(p),varj,currRing);
        pDelete(&varj);
        q = nc_ReduceSpoly(p,q,NULL,currRing);
        q = kNF(J,currQuotient,q,0,0);
        if (q!=NULL)
        {
          if (pIsConstant(q))
          {
            Q=idInit(1,1);
            Q->m[0]=pOne();
            idDelete(&J);
            pDelete(&q);
            if (K!=NULL) idDelete(&K);
            return(Q);
          }
          flag=1;
          Q=idInit(1,1);
          Q->m[0]=q;
          id_tmp=idSimpleAdd(K,Q);
          idDelete(&K);
          K=id_tmp;
          idDelete(&Q);
        }
      }
    }
    if (flag==0)
      /* i.e. all elements are two-sided */
    {
      idDelete(&K);
      return(J);
    }
    /* now we update GrBasis J with K */
    //    iSize=IDELEMS(J);
    iSize=idElem(J);
    id_tmp=idSimpleAdd(J,K);
    idDelete(&K);
    idDelete(&J); 
    BITSET save_test=test;
    test|=Sy_bit(OPT_SB_1);
    J=kStd(id_tmp, currQuotient, testHomog,NULL,NULL,0,iSize);
    test=save_test;
    idSkipZeroes(J);
  }
}

matrix nc_PrintMat(int a, int b, ring r, int metric)
  /* returns matrix with the info on noncomm multiplication */
{

  if ( (a==b) || !rIsPluralRing(r) ) return(NULL);
  int i;
  int j;
  if (a>b) {j=b; i=a;}
  else {j=a; i=b;}
  /* i<j */
  int rN=r->N;
  int size=r->nc->MTsize[UPMATELEM(i,j,rN)];
  matrix M = r->nc->MT[UPMATELEM(i,j,rN)];
  /*  return(M); */
  int sizeofres;
  if (metric==0)
  {
    sizeofres=sizeof(int);
  }
  if (metric==1)
  {
    sizeofres=sizeof(number);
  }
  matrix res=mpNew(size,size);
  int s;
  int t;
  int length;
  long totdeg;
  poly p;
  for(s=1;s<=size;s++)
  {
    for(t=1;t<=size;t++)
    {
      p=MATELEM(M,s,t);
      if (p==NULL)
      {
        MATELEM(res,s,t)=0;
      }
      else
      {
        length = pLength(p);
        if (metric==0) /* length */
        {
          MATELEM(res,s,t)= p_ISet(length,r);
        }
        else if (metric==1) /* sum of deg divided by the length */
        {
          totdeg=0;
          while (p!=NULL)
          {
            totdeg=totdeg+pDeg(p,r);
            pIter(p);
          }
          number ntd = nInit(totdeg);
          number nln = nInit(length);
          number nres=nDiv(ntd,nln);
          nDelete(&ntd);
          nDelete(&nln);
          MATELEM(res,s,t)=p_NSet(nres,r);
        }
      }
    }
  }
  return(res);
}

void ncKill(ring r)
  /* kills the nc extension of ring r */
{
  int i,j;
  int rN=r->N;
  if ( rN > 1 )
  {
    for(i=1;i<rN;i++)
    {
      for(j=i+1;j<=rN;j++)
      {
	id_Delete((ideal *)&(r->nc->MT[UPMATELEM(i,j,rN)]),r->nc->basering);
      }
    }
    omFreeSize((ADDRESS)r->nc->MT,rN*(rN-1)/2*sizeof(matrix));
    omFreeSize((ADDRESS)r->nc->MTsize,rN*(rN-1)/2*sizeof(int));
    id_Delete((ideal *)&(r->nc->COM),r->nc->basering);
  }
  id_Delete((ideal *)&(r->nc->C),r->nc->basering);
  id_Delete((ideal *)&(r->nc->D),r->nc->basering);
  omFreeSize((ADDRESS)r->nc,sizeof(nc_struct));
  r->nc=NULL;
}

void ncCleanUp(ring r)
{
  /* small CleanUp of r->nc */
  omFreeSize((ADDRESS)r->nc,sizeof(nc_struct));
  r->nc = NULL;
}

poly nc_p_CopyGet(poly a, const ring r)
/* for use in getting the mult. matrix elements*/
{
  if (!rIsPluralRing(r)) return(p_Copy(a,r));
  if (r==r->nc->basering) return(p_Copy(a,r));
  else
  {
    return(prCopyR_NoSort(a,r->nc->basering,r));
  }
}

poly nc_p_CopyPut(poly a, const ring r)
/* for use in defining the mult. matrix elements*/
{
  if (!rIsPluralRing(r)) return(p_Copy(a,r));
  if (r==r->nc->basering) return(p_Copy(a,r));
  else
  {
    return(prCopyR_NoSort(a,r,r->nc->basering));
  }
}

int nc_CheckSubalgebra(poly PolyVar, ring r)
  /* returns TRUE if product of vars from PolyVar defines */
  /* an admissible subalgebra of r */
{
  int rN=r->N;
  int *ExpVar=(int*)omAlloc0((rN+1)*sizeof(int));
  int *ExpTmp=(int*)omAlloc0((rN+1)*sizeof(int));
  p_GetExpV(PolyVar, ExpVar, r);
  int i; int j; int k;
  poly test=NULL;
  int OK=1;
  for (i=1;i<rN;i++)
  {
    if (ExpVar[i]==0) /* i.e. not in PolyVar */
    {  
      for (j=i+1;j<=rN;j++)
      {
	if (ExpVar[j]==0)
	{
	  test=nc_p_CopyGet(MATELEM(r->nc->D,i,j),r);
	  while (test!=NULL)
	  {
            p_GetExpV(test, ExpTmp, r);
	    OK=1;
	    for (k=1;k<=rN;k++)
            {
	      if (ExpTmp[k]!=0)
	      {
		if (ExpVar[k]!=0) OK=0;
	      }
            }
	    if (!OK) return(FALSE);
	    pIter(test);
          }
	}
      }
    }
  }
  p_Delete(&test,r);
  freeT(ExpVar,rN);
  freeT(ExpTmp,rN);
  return(TRUE);
}

// int Commutative_Context(ring r, leftv expression)
//   /* returns 1 if expression consists */
//   /*  of commutative elements */
// {
//   /* crucial: poly -> ideal, module, matrix  */
  
// }

// int Comm_Context_Poly(ring r, poly p)
// {
//   poly COMM=r->nc->COMM;
//   poly pp=pOne();
//   memset(pp->exp,0,r->ExpL_Size*sizeof(long));
//   while (p!=NULL)
//   {
//     for (i=0;i<=r->ExpL_Size;i++)
//     {
//       if ((p->exp[i]) && (pp->exp[i]))  return(FALSE); 
//       /* nonzero exponent of non-comm variable */
//     }
//     pIter(p);
//   }
//   return(TRUE);
// }

BOOLEAN nc_CallPlural(matrix CCC, matrix DDD, poly CCN, poly DDN, ring r)
  /* returns TRUE if there were errors */
  /* analyze inputs, check them for consistency */
  /* detect nc_type, DO NOT initialize multiplication */
  /* check the ordering condition and evtl. NDC */
{
  matrix CC = NULL; 
  matrix DD = NULL;
  poly CN = NULL;
  poly DN = NULL;
  matrix C;
  matrix D;
  number nN,pN,qN;
  int tmpIsSkewConstant;
  int i,j;
  if (r->nc != NULL)
  {
    WarnS("redefining algebra structure");
    if (r->nc->ref>1) /* in use by somebody else */
    {
      r->nc->ref--;
    }
    else  /* kill the previous nc data */
    {
      ncKill(r); 
    }
  }
  r->nc = (nc_struct *)omAlloc0(sizeof(nc_struct));
  r->nc->ref = 1;
  r->nc->basering = r;
  r->nc->type = nc_undef;

  /* initialition of the matrix C */
  /* check the correctness of arguments */

  if ((CCC != NULL) && ( (MATCOLS(CCC)==1) || MATROWS(CCC)==1 ) )
  {
    CN = MATELEM(CCC,1,1);
  }
  else 
  {
    if ((CCC != NULL) && ( (MATCOLS(CCC)!=r->N) || (MATROWS(CCC)!=r->N) ))
    {
      Werror("Square %d x %d  matrix expected",r->N,r->N);
      ncCleanUp(r);
      return TRUE;
    }
  }
  if (( CCC != NULL) && (CC == NULL)) CC = mpCopy(CCC);
  if (( CCN != NULL) && (CN == NULL)) CN = CCN;

  /* initialition of the matrix D */
  /* check the correctness of arguments */

  if ((DDD != NULL) && ( (MATCOLS(DDD)==1) || MATROWS(DDD)==1 ) )
  {
    DN = MATELEM(DDD,1,1);
  }
  else 
  {
    if ((DDD != NULL) && ( (MATCOLS(DDD)!=r->N) || (MATROWS(DDD)!=r->N) ))
    {
      Werror("Square %d x %d  matrix expected",r->N,r->N);
      ncCleanUp(r);
      return TRUE;
    }
  }
  if (( DDD != NULL) && (DD == NULL)) DD = mpCopy(DDD);
  if (( DDN != NULL) && (DN == NULL)) DN = DDN;

  /* further checks */

  if (CN != NULL)       /* create matrix C = CN * Id */
  {
    nN = p_GetCoeff(CN,r);
    if (n_IsZero(nN,r))
    {
      Werror("Incorrect input : zero coefficients are not allowed");
      ncCleanUp(r);
      return TRUE;
    }
    if (nIsOne(nN)) 
    {
      r->nc->type = nc_lie; 
    }
    else 
    {
      r->nc->type = nc_general;
    }
    r->nc->IsSkewConstant = 1;
    C = mpNew(r->N,r->N);
    for(i=1; i<r->N; i++)
    {
      for(j=i+1; j<=r->N; j++)
      {
	MATELEM(C,i,j) = nc_p_CopyPut(CN,r);
      }
    }
  }
  if ( (CN == NULL) && (CC != NULL) ) /* copy matrix C */
  {
    C = mpCopy(CC);
    /* analyze C */
    pN = p_GetCoeff(MATELEM(C,1,2),r);
    tmpIsSkewConstant = 1;
    for(i=1; i<r->N; i++)
    {
      for(j=i+1; j<=r->N; j++)
      { 
	qN = p_GetCoeff(MATELEM(C,i,j),r);
	if ( qN == NULL )   /* check the consistency: Cij!=0 */
	{
	  Werror("Incorrect input : matrix of coefficients contains zeros in the upper triangle");
	  ncCleanUp(r);
	  return TRUE;
	}
	if (!nEqual(pN,qN)) tmpIsSkewConstant = 0;
      }
    }
    r->nc->IsSkewConstant=tmpIsSkewConstant;
    if ( (tmpIsSkewConstant) && (nIsOne(pN)) ) 
    {
      r->nc->type = nc_lie;
    }
    else 
    {
      r->nc->type = nc_general;
    }
  }

  /* initialition of the matrix D */
  if ( DD == NULL ) 
    /* we treat DN only (it could also be NULL) */
  {
    D = mpNew(r->N,r->N);
    if (DN  == NULL)
    {
      if ( (currRing->nc->type == nc_lie) || (currRing->nc->type == nc_undef) )  
      {
	currRing->nc->type = nc_comm; /* it was nc_skew earlier */
      }
      else /* nc_general, nc_skew */
      {
	currRing->nc->type = nc_skew;
      }
    }
    else /* DN  != NULL */
    { 
      for(i=1; i<r->N; i++)
      {
	for(j=i+1; j<=r->N; j++)
	{
	  MATELEM(D,i,j) = nc_p_CopyPut(DN,r);
	}
      }
    }
  }
  else /* DD != NULL */
  { 
    D = mpCopy(DD); 
  }
  /* analyze D */ 
  /* check the ordering condition for D (both matrix and poly cases) */
  poly p,q;
  int report = 1;
  for(i=1; i<r->N; i++)
  {
    for(j=i+1; j<=r->N; j++)
    { 
      p = MATELEM(D,i,j);
      if ( p != NULL)
      {
	q = pOne();
	p_SetExp(q,i,1,r);
	p_SetExp(q,j,1,r);
	p_Setm(q,r);
	if (p_LmCmp(q,p,r) != 1) /* i.e. lm(p)<=lm(q)  */
	{
	  Print("Bad ordering at %d,%d",i,j);
	  report = 0;
	}
	p_Delete(&q,r);
	p = NULL;
      }
    }
  }
  if (!report)
  {
    Werror("Matrix of polynomials violates the ordering condition");
    ncCleanUp(r);
    return TRUE;
  }
  r->nc->C = C;
  r->nc->D = D;
  return nc_InitMultiplication(r);
}

BOOLEAN nc_InitMultiplication(ring r)
{
  /* returns TRUE if there were errors */
  /* initialize the multiplication: */
  /*  r->nc->MTsize, r->nc->MT, r->nc->COM, */
  /* and r->nc->IsSkewConstant for the skew case */
  if (rVar(r)==1)
  {
    r->nc->type=nc_comm;
    r->nc->IsSkewConstant=1;
    return FALSE;
  }
  int i,j;
  matrix COM;
  r->nc->MT = (matrix *)omAlloc0(r->N*(r->N-1)/2*sizeof(matrix));
  r->nc->MTsize = (int *)omAlloc0(r->N*(r->N-1)/2*sizeof(int));
  COM = mpCopy(r->nc->C);
  poly p,q;
  short DefMTsize=7;
  int IsNonComm=0;
  int tmpIsSkewConstant;
  
  for(i=1; i<r->N; i++)
  {
    for(j=i+1; j<=r->N; j++)
    {
      if ( MATELEM(r->nc->D,i,j) == NULL ) /* quasicommutative case */
      {
	/* 1x1 mult.matrix */
	r->nc->MTsize[UPMATELEM(i,j,r->N)] = 1;
	r->nc->MT[UPMATELEM(i,j,r->N)] = mpNew(1,1);
      }
      else /* pure noncommutative case */
      {
	/* TODO check the special multiplication properties */
	IsNonComm = 1;
	MATELEM(COM,i,j) = NULL;
	r->nc->MTsize[UPMATELEM(i,j,r->N)] = DefMTsize; /* default sizes */
	r->nc->MT[UPMATELEM(i,j,r->N)] = mpNew(DefMTsize, DefMTsize);
      }
      /* set MT[i,j,1,1] to c_i_j*x_i*x_j + D_i_j */
      p = pOne();
      p_SetCoeff(p,nCopy(pGetCoeff(MATELEM(r->nc->C,i,j))),r);
      p_SetExp(p,i,1,r);
      p_SetExp(p,j,1,r);
      p_Setm(p,r);
      q =  nc_p_CopyGet(MATELEM(r->nc->D,i,j),r);
      p = p_Add_q(p,q,r);
      MATELEM(r->nc->MT[UPMATELEM(i,j,r->N)],1,1) = nc_p_CopyPut(p,r);
      pDelete(&p);
      p = NULL;
    }
  }
  if (r->nc->type==nc_undef)
  {
    if (IsNonComm==1)
    {
      //      assume(pN!=NULL);
      //      if ((tmpIsSkewConstant==1) && (nIsOne(pGetCoeff(pN)))) r->nc->type=nc_lie;
      //      else r->nc->type=nc_general;
    }
    if (IsNonComm==0) 
    {
      r->nc->type=nc_skew; /* TODO: check whether it is commutative */
      r->nc->IsSkewConstant=tmpIsSkewConstant;
    }
  }
  r->nc->COM=COM;
  return FALSE;
}

/* substitute the n-th variable by e in p
* destroy p
* e is not a constant
*/
poly nc_pSubst(poly p, int n, poly e)
{
  int rN=currRing->N;
  int *PRE = (int *)omAlloc0((rN+1)*sizeof(int));
  int *SUF = (int *)omAlloc0((rN+1)*sizeof(int));
  int i,j,pow;
  number C;
  poly suf,pre;
  poly res = NULL;
  poly out = NULL;
  while ( p!= NULL )
  {
    C =  pGetCoeff(p);
    pGetExpV(p, PRE); /* faster splitting? */
    pow = PRE[n]; PRE[n]=0;
    res = NULL;
    if (pow!=0)
    {
      for (i=n+1; i<=rN; i++)
      {
	SUF[i] = PRE[i];
	PRE[i] = 0;
      }
      res =  pPower(pCopy(e),pow);
      /* multiply with prefix */
      pre = pOne();
      pSetExpV(pre,PRE);
      pSetm(pre);
      pSetComp(pre,PRE[0]);
      res = nc_mm_Mult_p(pre,res,currRing);
      /* multiply with suffix */
      suf = pOne();
      pSetExpV(suf,SUF);
      pSetm(suf);
      pSetComp(suf,PRE[0]);
      res = nc_p_Mult_mm(res,suf,currRing);
      res = p_Mult_nn(res,C,currRing);
    }
    else /* pow==0 */
    {
      res = pHead(p);
    }
    p   = pLmDeleteAndNext(p);
    out = pAdd(out,res);
  }
  freeT(PRE,rN);
  freeT(SUF,rN);
  return(out);
}

static ideal idPrepareStd(ideal T, ideal s,  int k)
{
  /* T is a left SB, without zeros, s is a list with zeros */
#ifdef PDEBUG
  if (IDELEMS(s)!=IDELEMS(T))
  {
    Print("ideals of diff. size!!!");
  }
#endif
  ideal t = idCopy(T);
  int j,rs=idRankFreeModule(s),rt=idRankFreeModule(t);
  poly p,q;

  ideal res = idInit(2*idElem(t),1+idElem(t));
  if (rs == 0)
  {
    for (j=0; j<IDELEMS(t); j++)
    {
      if (s->m[j]!=NULL) pSetCompP(s->m[j],1);
      if (t->m[j]!=NULL) pSetCompP(t->m[j],1);
    }
    k = si_max(k,1);
  }
  for (j=0; j<IDELEMS(t); j++)
  {
    if (s->m[j]!=NULL)
    {
      p = s->m[j];
      q = pOne();
      pSetComp(q,k+1+j);
      pSetmComp(q);
#if 0      
      while (pNext(p)) pIter(p);
      pNext(p) = q;
#else
      p = pAdd(p,q);
      s->m[j] = p;
#ifdef PDEBUG
    pTest(p);
#endif
#endif
    }
  }
  res = idSimpleAdd(t,s);
  idDelete(&t);
  res->rank = 1+idElem(T);
  return(res);
}

ideal Approx_Step(ideal L)
{
  int N=currRing->N;
  int i,j; // k=syzcomp
  int flag, flagcnt, syzcnt=0;
  int syzcomp = 0;
  int k=1; /* for ideals not modules */
  ideal I = kStd(L, currQuotient,testHomog,NULL,NULL,0,0,NULL);
  idSkipZeroes(I);
  ideal s_I;
  int idI = idElem(I);
  ideal trickyQuotient,s_trickyQuotient;
  if (currQuotient !=NULL)
  {
    trickyQuotient = idSimpleAdd(currQuotient,I);
  }
  else
    trickyQuotient = I;
  idSkipZeroes(trickyQuotient);
  poly *var = (poly *)omAlloc0((N+1)*sizeof(poly));
  //  poly *W = (poly *)omAlloc0((2*N+1)*sizeof(poly));
  resolvente S = (resolvente)omAlloc0((N+1)*sizeof(ideal));
  ideal SI, res;
  matrix MI;
  poly x=pOne();
  var[0]=x;
  ideal   h2, h3, s_h2, s_h3;
  poly    p,q,qq;
  /* init vars */
  for (i=1; i<=N; i++ )
  {
    x = pOne();
    pSetExp(x,i,1);
    pSetm(x);
    var[i]=pCopy(x);
  }
  /* init NF's */
  for (i=1; i<=N; i++ )
  {
    h2 = idInit(idI,1);
    flag = 0;
    for (j=0; j< idI; j++ )
    {
      q = nc_p_Mult_mm(pCopy(I->m[j]),var[i],currRing);
      q = kNF(I,currQuotient,q,0,0);
      if (q!=0)
      {
	h2->m[j]=pCopy(q);
	//	pShift(&(h2->m[flag]),1);
	flag++;
	pDelete(&q);
      }
      else
	h2->m[j]=0;
    }
    /* W[1..idElems(I)] */
    if (flag >0)
    {
      /* compute syzygies with values in I*/
      //      idSkipZeroes(h2);
      //      h2 = idSimpleAdd(h2,I);
      //      h2->rank=flag+idI+1;
      idTest(h2);
      idShow(h2);
      ring orig_ring=currRing;
      ring syz_ring=rCurrRingAssure_SyzComp();
      syzcomp = 1;
      rSetSyzComp(syzcomp);
      if (orig_ring != syz_ring)
      {
	s_h2=idrCopyR_NoSort(h2,orig_ring);
	//	s_trickyQuotient=idrCopyR_NoSort(trickyQuotient,orig_ring);
	//	rDebugPrint(syz_ring);
	s_I=idrCopyR_NoSort(I,orig_ring);
      }
      else
      {
	s_h2 = h2;
	s_I  = I;
	//	s_trickyQuotient=trickyQuotient;
      }
      idTest(s_h2);
      //      idTest(s_trickyQuotient);
      Print(".proceeding with the variable %d\n",i);
      s_h3 = idPrepareStd(s_I, s_h2, 1);
      BITSET save_test=test;
      test|=Sy_bit(OPT_SB_1);
      idTest(s_h3);
      idDelete(&s_h2);
      s_h2=idCopy(s_h3);
      idDelete(&s_h3);
      Print("...computing Syz");
      s_h3 = kStd(s_h2, currQuotient,(tHomog)FALSE,NULL,NULL,syzcomp,idI);
      test=save_test;
      idShow(s_h3);
      if (orig_ring != syz_ring)
      {
	idDelete(&s_h2);
	for (j=0; j<IDELEMS(s_h3); j++)
	{
	  if (s_h3->m[j] != NULL)
	  {
	    if (p_MinComp(s_h3->m[j],syz_ring) > syzcomp) /* i.e. it is a syzygy */
	      pShift(&s_h3->m[j], -syzcomp);
	    else
	      pDelete(&s_h3->m[j]);
	  }
	}
	idSkipZeroes(s_h3);
	s_h3->rank -= syzcomp;
	rChangeCurrRing(orig_ring);
	//	s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
	s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
	rKill(syz_ring);
      }
      idTest(s_h3);
      S[syzcnt]=kStd(s_h3,currQuotient,(tHomog)FALSE,NULL,NULL);
      syzcnt++;
      idDelete(&s_h3);
    } /* end if flag >0 */
    else 
    {
      flagcnt++;
    }
  }
  if (flagcnt == N) 
  {
    Print("the input is a two--sided ideal");
    return(I);
  }
  if (syzcnt >0)
  {
    Print("..computing Intersect of %d modules\n",syzcnt);
    if (syzcnt == 1)
      SI = S[0];
    else
      SI = idMultSect(S, syzcnt);
    idShow(SI);
    MI = idModule2Matrix(SI);
    res= idInit(MATCOLS(MI),1);
    for (i=1; i<= MATCOLS(MI); i++)
    {    
      p = NULL;
      for (j=0; j< idElem(I); j++)
      { 
	q = pCopy(MATELEM(MI,j+1,i));
	if (q!=NULL)
	{
	  q = pMult(q,pCopy(I->m[j]));
	  p = pAdd(p,q);
	}
      }
      res->m[i-1]=p;
    }
    Print("final std");
    res = kStd(res, currQuotient,testHomog,NULL,NULL,0,0,NULL);
    idSkipZeroes(res);
    return(res);
  }
  else
  {
    Print("No syzygies");
    return(I);
  }
}


ring nc_rCreateNCcomm(ring r)
  /* creates a commutative nc extension; "converts" comm.ring to a Plural ring */
{
  if (rIsPluralRing(r)) return r;
  r->nc = (nc_struct *)omAlloc0(sizeof(nc_struct));
  r->nc->ref = 1;
  r->nc->basering = r;
  r->nc->type = nc_comm;
  r->nc->IsSkewConstant = 1;
  matrix C = mpNew(r->N,r->N);
  matrix D = mpNew(r->N,r->N);
  int i,j;
  for(i=1; i<r->N; i++)
  {
    for(j=i+1; j<=r->N; j++)
    {
      MATELEM(C,i,j) = pOne();
    }
  }
  r->nc->C = C;
  r->nc->D = D;
  if (nc_InitMultiplication(r))
  {
    WarnS("Error initializing multiplication!");
  }
  return r;
}

poly p_CopyEmbed(poly p, ring srcRing, int shift, int par_shift)
  /* NOT USED ANYMORE: replaced by maFindPerm in ring.cc */
  /* for use with embeddings: currRing is a sum of smaller rings */
  /* and srcRing is one of such smaller rings */
  /* shift defines the position of a subring in srcRing */
  /* par_shift defines the position of a subfield in basefield of CurrRing */
{
  if (currRing == srcRing)
  {
    return(p_Copy(p,currRing));
  }
  nMapFunc nMap=nSetMap(srcRing);
  poly q;
  //  if ( nMap == nCopy)
  //  {
  //    q = prCopyR(p,srcRing);
  //  }
  //  else
  {
    int *perm = (int *)omAlloc0((srcRing->N+1)*sizeof(int));
    int *par_perm = (int *)omAlloc0((srcRing->P+1)*sizeof(int));
    //    int *par_perm = (int *)omAlloc0((srcRing->P+1)*sizeof(int));
    int i;
    //    if (srcRing->P > 0)
    //    {
    //      for (i=0; i<srcRing->P; i++)
    //	par_perm[i]=-i;
    //    }
    if ((shift<0) || (shift > currRing->N))
    {
      Werror("bad shifts in p_CopyEmbed");
      return(0);
    }
    for (i=1; i<= srcRing->N; i++)
    {
      perm[i] = shift+i;
    }
    q = pPermPoly(p,perm,srcRing,nMap,par_perm,srcRing->P);
  }
  return(q);
}

poly p_Oppose(ring Rop, poly p)
{
  /* TODO check Rop == rOpposite(currRing) */
  /* the same basefield, same number of variables */

  /* the simplest case:*/
  if (Rop==currRing) return pCopy(p); // ok
  nMapFunc nMap=nSetMap(Rop);
  int *perm=(int *)omAlloc0((Rop->N+1)*sizeof(int));
  /* we know perm exactly */
  int i;
  for(i=1; i<=Rop->N; i++)
  {
    perm[i] = Rop->N+1-i;
  }
  /*  int *par_perm=NULL; */
  poly res = pPermPoly(p, perm, Rop, nMap);
  omFreeSize((ADDRESS)perm,(Rop->N+1)*sizeof(int));
  //omFreeSize((ADDRESS)par_perm,rPar(r)*sizeof(int));
  return res;
}


#endif
