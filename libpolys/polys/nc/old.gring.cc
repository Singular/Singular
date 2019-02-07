/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    gring.cc
 *  Purpose: noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *******************************************************************/

#define MYTEST 0
#define OUTPUT 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif




#include "misc/auxiliary.h"

#ifdef HAVE_PLURAL

# define PLURAL_INTERNAL_DECLARATIONS
#include "nc.h"
#include "sca.h"
#include "gb_hack.h"

#include "polys/monomials/ring.h"

#include "coeffs/numbers.h"

#include "misc/options.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "polys/simpleideals.h"
#include "polys/matpol.h"

#include "polys/kbuckets.h"
#include "polys/sbuckets.h"

#include "polys/prCopy.h"

#include "polys/operations/p_Mult_q.h"

#include "summator.h"

#include "ncSAMult.h" // for CMultiplier etc classes
#include "ncSAFormula.h" // for CFormulaPowerMultiplier and enum Enum_ncSAType

// #ifdef HAVE_RATGRING
// #include "polys/ratgring.h"
// #endif

static poly NF_Proc_Dummy(ideal, ideal, poly, int, int, const ring)
{ WerrorS("nc_NF not defined"); return NULL; }
static ideal BBA_Proc_Dummy (const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring)
{ WerrorS("nc_NF not defined"); return NULL; }

// the following funtion poiters are quasi-static:
// they will be set in siInit and never changes afterwards:
VAR NF_Proc nc_NF=NF_Proc_Dummy;
VAR BBA_Proc gnc_gr_bba=BBA_Proc_Dummy;
VAR BBA_Proc gnc_gr_mora=BBA_Proc_Dummy;
VAR BBA_Proc sca_bba=BBA_Proc_Dummy;
VAR BBA_Proc sca_mora=BBA_Proc_Dummy;
VAR BBA_Proc sca_gr_bba=BBA_Proc_Dummy;

/* copy : */
poly nc_p_CopyGet(poly a, const ring r);
poly nc_p_CopyPut(poly a, const ring r);

poly nc_p_Bracket_qq(poly p, const poly q, const ring r);

// only SCA can be used by default, formulas are off by default
VAR int  iNCExtensions = SCAMASK | NOFORMULAMASK;

int& getNCExtensions()
{
  return (iNCExtensions);
}

int setNCExtensions(int iMask)
{
  const int iOld = getNCExtensions();
  getNCExtensions() = iMask;
  return (iOld);
}

bool ncExtensions(int iMask) //  = 0x0FFFF
{
  return ((getNCExtensions() & iMask) == iMask);
}

/* global nc_macros : */

#define freeT(A,v) omFreeSize((ADDRESS)A,(v+1)*sizeof(int))
#define freeN(A,k) omFreeSize((ADDRESS)A,k*sizeof(number))


// some forward declarations:


// polynomial multiplication functions for p_Procs :
poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly &last);
poly gnc_p_Mult_mm(poly p, const poly m, const ring r);
poly gnc_p_mm_Mult(poly m, const poly p, const ring r);
poly gnc_pp_mm_Mult(const poly p, const poly m, const ring r);


/* syzygies : */
poly gnc_CreateSpolyOld(const poly p1, const poly p2/*, poly spNoether*/, const ring r);
poly gnc_ReduceSpolyOld(const poly p1, poly p2/*, poly spNoether*/, const ring r);

poly gnc_CreateSpolyNew(const poly p1, const poly p2/*, poly spNoether*/, const ring r);
poly gnc_ReduceSpolyNew(const poly p1, poly p2/*, poly spNoether*/, const ring r);



void gnc_kBucketPolyRedNew(kBucket_pt b, poly p, number *c);
void gnc_kBucketPolyRed_ZNew(kBucket_pt b, poly p, number *c);

void gnc_kBucketPolyRedOld(kBucket_pt b, poly p, number *c);
void gnc_kBucketPolyRed_ZOld(kBucket_pt b, poly p, number *c);


// poly gnc_ReduceSpolyNew(poly p1, poly p2, poly spNoether, const ring r);
// void gnc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r);

// void nc_kBucketPolyRed(kBucket_pt b, poly p);

void nc_CleanUp(nc_struct* p); // just free memory!
void nc_rCleanUp(ring r); // smaller than kill: just free mem


#if 0
// deprecated functions:
//  poly gnc_p_Minus_mm_Mult_qq_ign(poly p, const poly m, poly q, int & d1, poly d2, const ring ri, poly &d3);
//  poly gnc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r);
//  poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp, int lq, const ring r);
//  poly nc_p_Plus_mm_Mult_qq (poly p, const poly m, const poly q, int &lp, int lq, const ring r);
#endif


///////////////////////////////////////////////////////////////////////////////
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &shorter,
                                    const poly, const ring r)
{
  poly mc  = p_Neg( p_Copy(m, r), r );
  poly mmc = nc_mm_Mult_pp( mc, q, r );
  p_Delete(&mc, r);

  int org_p=pLength(p);
  int org_q=pLength(q);

  p = p_Add_q(p, mmc, r);

  shorter = pLength(p)-org_p-org_q; // ring independent!

  return(p);
}

// returns p + m*q destroys p, const: q, m
poly nc_p_Plus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                              const int, const ring r)
{
  p = p_Add_q(p, nc_mm_Mult_pp( m, q, r ), r);

  lp = pLength(p);

  return(p);
}

#if 0
poly gnc_p_Minus_mm_Mult_qq_ign(poly p, const poly m, poly q, int & d1, poly d2, const ring r, poly &d3)
{
  poly t;
  int  i;

  return gnc_p_Minus_mm_Mult_qq(p, m, q, d1, i, t, r);
}
#endif


//----------- auxiliary routines--------------------------
poly _gnc_p_Mult_q(poly p, poly q, const int copy, const ring r) // not used anymore!
  /* destroy p,q unless copy=1 */
{
  poly res=NULL;
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
    res=p_Add_q(res, pp_Mult_mm(pp, qq, r), r); // p_Head(qq, r)?
    qq=p_LmDeleteAndNext(qq,r);
  }
  p_Delete(&pp,r);
  return(res);
}

// return pPolyP * pPolyQ; destroy or reuse pPolyP and pPolyQ
poly _nc_p_Mult_q(poly pPolyP, poly pPolyQ, const ring rRing)
{
  assume( rIsNCRing(rRing) );
#ifdef PDEBUG
  p_Test(pPolyP, rRing);
  p_Test(pPolyQ, rRing);
#endif
#ifdef RDEBUG
  rTest(rRing);
#endif

  int lp, lq;

  pqLength(pPolyP, pPolyQ, lp, lq, MIN_LENGTH_BUCKET);

  bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (si_max(lp, lq) < MIN_LENGTH_BUCKET); // ???

  CPolynomialSummator sum(rRing, bUsePolynomial);

  if (lq <= lp) // ?
  {
    // always length(q) times "p * q[j]"
    for( ; pPolyQ!=NULL; pPolyQ  = p_LmDeleteAndNext( pPolyQ, rRing ) )
      sum += pp_Mult_mm( pPolyP, pPolyQ, rRing);

    p_Delete( &pPolyP, rRing );
  } else
  {
    // always length(p) times "p[i] * q"
    for( ; pPolyP!=NULL; pPolyP  = p_LmDeleteAndNext( pPolyP, rRing ) )
      sum += nc_mm_Mult_pp( pPolyP, pPolyQ, rRing);

    p_Delete( &pPolyQ, rRing );
  }

  return(sum);
}

// return pPolyP * pPolyQ; preserve pPolyP and pPolyQ
poly _nc_pp_Mult_qq(const poly pPolyP, const poly pPolyQ, const ring rRing)
{
  assume( rIsPluralRing(rRing) );
#ifdef PDEBUG
  p_Test(pPolyP, rRing);
  p_Test(pPolyQ, rRing);
#endif
#ifdef RDEBUG
  rTest(rRing);
#endif

  int lp, lq;

  pqLength(pPolyP, pPolyQ, lp, lq, MIN_LENGTH_BUCKET);

  bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (si_max(lp, lq) < MIN_LENGTH_BUCKET); // ???

  CPolynomialSummator sum(rRing, bUsePolynomial);

  if (lq <= lp) // ?
  {
    // always length(q) times "p * q[j]"
    for( poly q = pPolyQ; q !=NULL; q = pNext(q) )
      sum += pp_Mult_mm(pPolyP, q, rRing);
  } else
  {
    // always length(p) times "p[i] * q"
    for( poly p = pPolyP; p !=NULL; p = pNext(p) )
      sum += nc_mm_Mult_pp( p, pPolyQ, rRing);
  }

  return(sum);
}



poly gnc_mm_Mult_nn (int *F, int *G, const ring r);
poly gnc_mm_Mult_uu (int *F,int jG,int bG, const ring r);

/* #define nc_uu_Mult_ww nc_uu_Mult_ww_vert */
poly gnc_uu_Mult_ww (int i, int a, int j, int b, const ring r);
/* poly nc_uu_Mult_ww_vert (int i, int a, int j, int b, const ring r); */
/* poly nc_uu_Mult_ww_horvert (int i, int a, int j, int b, const ring r); */
/* poly nc_uu_Mult_ww_hvdiag (int i, int a, int j, int b, const ring r); */
/* not written yet */


poly gnc_p_Mult_mm_Common(poly p, const poly m, int side, const ring r)
/* p is poly, m is mono with coeff, destroys p */
/* if side==1, computes p_Mult_mm; otherwise, mm_Mult_p */
{
  if ((p==NULL) || (m==NULL)) return NULL;
  /*  if (pNext(p)==NULL) return(nc_mm_Mult_nn(p,pCopy(m),r)); */
  /* excluded  - the cycle will do it anyway - OK. */
  if (p_IsConstant(m,r)) return(__p_Mult_nn(p,p_GetCoeff(m,r),r));

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

  CPolynomialSummator sum(r, UseBuckets == 0);

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
//        if (side)
//        {
//          PrintS("gnc_p_Mult_mm: Multiplication in the left module from the right");
//        }
#endif
      }
      else
      {
        /* REPORT_ERROR */
#ifdef PDEBUG
        const char* s;
        if (side==1) s="gnc_p_Mult_mm";
        else s="gnc_p_mm_Mult";
        Print("%s: exponent mismatch %d and %d\n",s,expP,expM);
#endif
        expOut=0;
      }
    }
    p_GetExpV(p,P,r);
    cP=pGetCoeff(p);
    cOut=n_Mult(cP,cM,r->cf);
    if (side==1)
    {
      v = gnc_mm_Mult_nn(P, M, r);
    }
    else
    {
      v = gnc_mm_Mult_nn(M, P, r);
    }
    v = __p_Mult_nn(v,cOut,r);
    n_Delete(&cOut,r->cf);
    p_SetCompP(v,expOut,r);

    sum += v;

    p_LmDelete(&p,r);
  }
  freeT(P,rN);
  freeT(M,rN);

  return(sum);
}

/* poly functions defined in p_Procs : */
poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r)
{
  return( gnc_p_Mult_mm_Common(p_Copy(p,r), m, 1, r) );
}

poly gnc_p_Mult_mm(poly p, const poly m, const ring r)
{
  return( gnc_p_Mult_mm_Common(p, m, 1, r) );
}

/* m * p */
poly gnc_p_mm_Mult(poly p, const poly m, const ring r)
{
  return( gnc_p_Mult_mm_Common(p, m, 0, r) );
}

poly gnc_pp_mm_Mult(const poly p, const poly m, const ring r)
{
  return( gnc_p_Mult_mm_Common(p_Copy(p,r), m, 0, r) );
}



poly gnc_mm_Mult_nn(int *F0, int *G0, const ring r)
/* destroys nothing, no coeffs and exps */
{
  poly out=NULL;
  int i,j;
  int iF,jG,iG;
  int rN=r->N;

  int *F=(int *)omAlloc0((rN+1)*sizeof(int));
  int *G=(int *)omAlloc0((rN+1)*sizeof(int));

  memcpy(F, F0,(rN+1)*sizeof(int));
  // pExpVectorCopy(F,F0);
  memcpy(G, G0,(rN+1)*sizeof(int));
  //  pExpVectorCopy(G,G0);
  F[0]=0;
  G[0]=0;

  iF=rN;
  while ((F[iF]==0)&&(iF>=1)) iF--; /* last exp_num of F */
  if (iF==0) /* F0 is zero vector */
  {
    out=p_One(r);
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

  out=p_One(r);

  if (iF<=jG)
    /* i.e. no mixed exp_num , MERGE case */
  {
    { for(int ii=rN;ii>0;ii--) F[ii]+=G[ii]; }
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  }

  number cff=n_Init(1,r->cf);
  number tmp_num=NULL;
  int cpower=0;

  if (ncRingType(r)==nc_skew)
  {
    if (r->GetNC()->IsSkewConstant==1)
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
          cpower = cpower*G[j]; // bug! here may happen an arithmetic overflow!!!
          tpower = tpower + cpower;
        }
      }
      cff = n_Copy(pGetCoeff(MATELEM(r->GetNC()->COM,1,2)),r->cf);
      n_Power(cff,tpower,&tmp_num, r->cf);
      n_Delete(&cff,r->cf);
      cff = tmp_num;
    }
    else /* skew commutative with nonequal coeffs */
    {
      number totcff=n_Init(1,r->cf);
      for(j=jG; j<=iG; j++)
      {
        if (G[j]!=0)
        {
          cpower = 0;
          for(i=j+1; i<=iF; i++)
          {
            if (F[i]!=0)
            {
              cpower = F[i]*G[j]; // bug! overflow danger!!!
              cff = n_Copy(pGetCoeff(MATELEM(r->GetNC()->COM,j,i)),r->cf);
              n_Power(cff,cpower,&tmp_num, r->cf);
              cff = n_Mult(totcff,tmp_num, r->cf);
              n_Delete(&totcff, r->cf);
              n_Delete(&tmp_num, r->cf);
              totcff = n_Copy(cff,r->cf);
              n_Delete(&cff,r->cf);
            }
          } /* end 2nd for */
        }
      }
      cff=totcff;
    }
    { for(int ii=rN;ii>0;ii--) F[ii]+=G[ii]; }
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    p_SetCoeff(out,cff,r);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  } /* end nc_skew */

  /* now we have to destroy out! */
  p_Delete(&out,r);

  if (iG==jG)
    /* g is univariate monomial */
  {
    /*    if (ri->GetNC()->type==nc_skew) -- postpone to TU */
    out = gnc_mm_Mult_uu(F,jG,G[jG],r);
    freeT(F,rN);
    freeT(G,rN);
    return(out);
  }

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
  c[0]=n_Init(1,r->cf);

  int *Op=Nxt;
  int *On=G;
  int *U=(int *)omAlloc0((rN+1)*sizeof(int));

  for (i=jG;i<=rN;i++) U[i]=Nxt[i]+G[i];  /* make leadterm */
  Nxt=NULL;
  G=NULL;
  cnt=1;
  int t=0;
  poly w=NULL;
  poly Pn=p_One(r);
  p_SetExpV(Pn,On,r);
  p_Setm(Pn,r);

  while (On[iG]!=0)
  {
     t=log[cnt];

     w=gnc_mm_Mult_uu(Op,t,On[t],r);
     c[cnt]=n_Mult(c[cnt-1],pGetCoeff(w),r->cf);
     D = pNext(w);  /* getting coef and rest D */
     p_LmDelete(&w,r);
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
       Rout=gnc_p_Mult_mm(D,Pn,r);
//       }
     }
     else
     {
       Rout=D;
       D=NULL;
     }

     if (Rout!=NULL)
     {
       Rout=__p_Mult_nn(Rout,c[cnt-1],r); /* Rest is ready */
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

  Rout=p_One(r);
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
    Rout=p_One(r);
    Prv[0]=0;
    p_SetExpV(Rout,Prv,r);
    p_Setm(Rout,r);
#ifdef PDEBUG
    p_Test(Rout,r);
#endif
    out=gnc_p_mm_Mult(out,Rout,r); /* getting the final result */
    freeT(Prv,rN);
    p_Delete(&Rout,r);
  }
  return (out);
}


poly gnc_mm_Mult_uu(int *F,int jG,int bG, const ring r)
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
   out=p_One(r);
   p_SetExp(out,jG,bG,r);
   p_Setm(out,r);
   return(out);
  }

  int jF=1;
  while ((F[jF]==0)&&(jF<=rN)) jF++;  /* first exp of F */

  if (iF<=jG)                       /* i.e. no mixed exp_num */
  {
    out=p_One(r);
    F[jG]=F[jG]+bG;
    p_SetExpV(out,F,r);
    p_Setm(out,r);
    return(out);
  }

  if (iF==jF)              /* uni times uni */
  {
   out=gnc_uu_Mult_ww(iF,F[iF],jG,bG,r);
   return(out);
  }

  /* Now: F is mono with >=2 exponents, jG<iF */
  /* check the quasi-commutative case */
//   matrix LCOM=r->GetNC()->COM;
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

  if (cnf==0)
  {
    freeT(Prv,rN); Prv = NULL;
  }

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
    poly Rout=p_One(r);
    out=gnc_uu_Mult_ww(q,Nxt[q],jG,bG,r);

    freeT(Nxt,rN);  Nxt = NULL;

    if (cnf!=0)
    {
       Prv[0]=0;
       p_SetExpV(Rout,Prv,r);
       p_Setm(Rout,r);

#ifdef PDEBUG
       p_Test(Rout,r);
#endif

       freeT(Prv,rN);
       Prv = NULL;

       out=gnc_p_mm_Mult(out,Rout,r); /* getting the final result */
    }

    freeT(lF,rN);
    lF = NULL;

    p_Delete(&Rout,r);

    assume(Nxt == NULL);
    assume(lF == NULL);
    assume(Prv == NULL);

    return (out);
  }
/* -------------------- MAIN ACTION --------------------- */

  poly D=NULL;
  poly Rout=NULL;
  number *c=(number *)omAlloc0((cnt+2)*sizeof(number));
  c[cnt+1]=n_Init(1,r->cf);
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

     poly w=gnc_uu_Mult_ww(t,Op[t],jG,bG,r);
     c[cnt]=n_Copy(pGetCoeff(w),r->cf);
     D = pNext(w);  /* getting coef and rest D */
     p_LmDelete(&w,r);
     w=NULL;

     Op[t]= 0;
     Pp=p_One(r);
     p_SetExpV(Pp,Op,r);
     p_Setm(Pp,r);

     if (t<nlast)
     {
       kk=lF[cnt+1];
       On[kk]=F[kk];

       Pn=p_One(r);
       p_SetExpV(Pn,On,r);
       p_Setm(Pn,r);

       if (t!=first)   /* typical expr */
       {
         w=gnc_p_Mult_mm(D,Pn,r);
         Rout=gnc_p_mm_Mult(w,Pp,r);
         w=NULL;
       }
       else                   /* last step */
       {
         On[t]=0;
         p_SetExpV(Pn,On,r);
         p_Setm(Pn,r);
         Rout=gnc_p_Mult_mm(D,Pn,r);
       }
#ifdef PDEBUG
       p_Test(Pp,r);
#endif
       p_Delete(&Pn,r);
     }
     else                     /* first step */
     {
       Rout=gnc_p_mm_Mult(D,Pp,r);
     }
#ifdef PDEBUG
     p_Test(Pp,r);
#endif
     p_Delete(&Pp,r);
     num=n_Mult(c[cnt+1],c[cnt],r->cf);
     n_Delete(&c[cnt],r->cf);
     c[cnt]=num;
     Rout=__p_Mult_nn(Rout,c[cnt+1],r); /* Rest is ready */
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

  Rout=p_One(r);
  p_SetExpV(Rout,U,r);
  p_Setm(Rout,r);  /* use again this name */
  p_SetCoeff(Rout,c[cnt+1],r);  /* last computed coef */

  out=p_Add_q(out,Rout,r);

  Rout=NULL;

  freeT(U, rN);
  freeN(c, i);
  freeT(lF, rN);

  if (cnf!=0)
  {
    Rout=p_One(r);
    p_SetExpV(Rout,Prv,r);
    p_Setm(Rout,r);
    freeT(Prv, rN);
    out=gnc_p_mm_Mult(out,Rout,r); /* getting the final result */
    p_Delete(&Rout,r);
  }

  return (out);
}

poly gnc_uu_Mult_ww_vert (int i, int a, int j, int b, const ring r)
{
  int k,m;
  int rN=r->N;
  const int cMTindex = UPMATELEM(j,i,rN);
  matrix cMT=r->GetNC()->MT[cMTindex];         /* cMT=current MT */

  poly x=p_One(r);p_SetExp(x,j,1,r);p_Setm(x,r);
/* var(j); */
  poly y=p_One(r);p_SetExp(y,i,1,r);p_Setm(y,r);
/*var(i);  for convenience */
#ifdef PDEBUG
  p_Test(x,r);
  p_Test(y,r);
#endif
  poly t=NULL;
/* ------------ Main Cycles ----------------------------*/

  for (k=2;k<=a;k++)
  {
     t = MATELEM(cMT,k,1);

     if (t==NULL)   /* not computed yet */
     {
       t = nc_p_CopyGet(MATELEM(cMT,k-1,1),r);
       //        t=p_Copy(MATELEM(cMT,k-1,1),r);
       t = gnc_p_mm_Mult(t,y,r);
       cMT=r->GetNC()->MT[cMTindex]; // since multiplication can change the MT table...
       assume( t != NULL );
#ifdef PDEBUG
       p_Test(t,r);
#endif
       MATELEM(cMT,k,1) = nc_p_CopyPut(t,r);
       //        omCheckAddr(cMT->m);
       p_Delete(&t,r);
     }
     t=NULL;
  }

  for (m=2;m<=b;m++)
  {
    t = MATELEM(cMT,a,m);
    //     t=MATELEM(cMT,a,m);
    if (t==NULL)   //not computed yet
    {
      t = nc_p_CopyGet(MATELEM(cMT,a,m-1),r);
      assume( t != NULL );
      //      t=p_Copy(MATELEM(cMT,a,m-1),r);
      t = gnc_p_Mult_mm(t,x,r);
      cMT=r->GetNC()->MT[cMTindex]; // since multiplication can change the MT table...
#ifdef PDEBUG
      p_Test(t,r);
#endif
      MATELEM(cMT,a,m) = nc_p_CopyPut(t,r);
      //      MATELEM(cMT,a,m) = t;
      //        omCheckAddr(cMT->m);
      p_Delete(&t,r);
    }
    t=NULL;
  }
  p_Delete(&x,r);
  p_Delete(&y,r);
  t=MATELEM(cMT,a,b);
  assume( t != NULL );

  t= nc_p_CopyGet(t,r);
#ifdef PDEBUG
  p_Test(t,r);
#endif
  //  return(p_Copy(t,r));
  /* since the last computed element was cMT[a,b] */
  return(t);
}


static inline poly gnc_uu_Mult_ww_formula (int i, int a, int j, int b, const ring r)
{
  if(ncExtensions(NOFORMULAMASK))
    return gnc_uu_Mult_ww_vert(i, a, j, b, r);

  CFormulaPowerMultiplier* FormulaMultiplier = GetFormulaPowerMultiplier(r);
  Enum_ncSAType PairType = _ncSA_notImplemented;

  if( FormulaMultiplier != NULL )
    PairType = FormulaMultiplier->GetPair(j, i);


  if( PairType == _ncSA_notImplemented )
    return gnc_uu_Mult_ww_vert(i, a, j, b, r);


 //    return FormulaMultiplier->Multiply(j, i, b, a);
  poly t = CFormulaPowerMultiplier::Multiply( PairType, j, i, b, a, r);

  int rN=r->N;
  matrix cMT = r->GetNC()->MT[UPMATELEM(j,i,rN)];         /* cMT=current MT */


  MATELEM(cMT, a, b) = nc_p_CopyPut(t,r);

  //  t=MATELEM(cMT,a,b);
//  t= nc_p_CopyGet(MATELEM(cMT,a,b),r);
  //  return(p_Copy(t,r));
  /* since the last computed element was cMT[a,b] */
  return(t);
}


poly gnc_uu_Mult_ww (int i, int a, int j, int b, const ring r)
  /* (x_i)^a times (x_j)^b */
  /* x_i = y,  x_j = x ! */
{
  /* Check zero exceptions, (q-)commutativity and is there something to do? */
  assume(a!=0);
  assume(b!=0);
  poly out=p_One(r);
  if (i<=j)
  {
    p_SetExp(out,i,a,r);
    p_AddExp(out,j,b,r);
    p_Setm(out,r);
    return(out);
  }/* zero exeptions and usual case */
  /*  if ((a==0)||(b==0)||(i<=j)) return(out); */

  if (MATELEM(r->GetNC()->COM,j,i)!=NULL)
    /* commutative or quasicommutative case */
  {
    p_SetExp(out,i,a,r);
    p_AddExp(out,j,b,r);
    p_Setm(out,r);
    if (n_IsOne(pGetCoeff(MATELEM(r->GetNC()->COM,j,i)),r->cf)) /* commutative case */
    {
      return(out);
    }
    else
    {
      number tmp_number=pGetCoeff(MATELEM(r->GetNC()->COM,j,i)); /* quasicommutative case */
      n_Power(tmp_number,a*b,&tmp_number, r->cf); // BUG! ;-(
      p_SetCoeff(out,tmp_number,r);
      return(out);
    }
  }/* end_of commutative or quasicommutative case */
  p_Delete(&out,r);


  if(ncExtensions(NOCACHEMASK) && !ncExtensions(NOFORMULAMASK)) // don't use cache whenever possible!
  { // without cache!?
    CFormulaPowerMultiplier* FormulaMultiplier = GetFormulaPowerMultiplier(r);
    Enum_ncSAType PairType = _ncSA_notImplemented;

     if( FormulaMultiplier != NULL )
       PairType = FormulaMultiplier->GetPair(j, i);

     if( PairType != _ncSA_notImplemented )
  // //    return FormulaMultiplier->Multiply(j, i, b, a);
       return CFormulaPowerMultiplier::Multiply( PairType, j, i, b, a, r);
  }


  /* we are here if  i>j and variables do not commute or quasicommute */
  /* in fact, now a>=1 and b>=1; and j<i */
  /* now check whether the polynomial is already computed */
  int rN=r->N;
  int vik = UPMATELEM(j,i,rN);
  int cMTsize=r->GetNC()->MTsize[vik];
  int newcMTsize=0;
  newcMTsize=si_max(a,b);

  if (newcMTsize<=cMTsize)
  {
    out =  nc_p_CopyGet(MATELEM(r->GetNC()->MT[vik],a,b),r);
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
        out = MATELEM(r->GetNC()->MT[UPMATELEM(j,i,rN)],k,m);
        if ( out != NULL )
        {
          MATELEM(tmp,k,m) = out;/*MATELEM(r->GetNC()->MT[UPMATELEM(j,i,rN)],k,m)*/
          //           omCheckAddr(tmp->m);
          MATELEM(r->GetNC()->MT[UPMATELEM(j,i,rN)],k,m)=NULL;
          //           omCheckAddr(r->GetNC()->MT[UPMATELEM(j,i,rN)]->m);
          out=NULL;
        }
      }
    }
    id_Delete((ideal *)&(r->GetNC()->MT[UPMATELEM(j,i,rN)]),r);
    r->GetNC()->MT[UPMATELEM(j,i,rN)] = tmp;
    tmp=NULL;
    r->GetNC()->MTsize[UPMATELEM(j,i,rN)] = newcMTsize;
  }
  /* The update of multiplication matrix is finished */


  return gnc_uu_Mult_ww_formula(i, a, j, b, r);

  out = gnc_uu_Mult_ww_vert(i, a, j, b, r);
  //    out = nc_uu_Mult_ww_horvert(i, a, j, b, r);
  return(out);
}

poly gnc_uu_Mult_ww_horvert (int i, int a, int j, int b, const ring r)

{
  int k,m;
  int rN=r->N;
  matrix cMT=r->GetNC()->MT[UPMATELEM(j,i,rN)];         /* cMT=current MT */

  poly x=p_One(r);p_SetExp(x,j,1,r);p_Setm(x,r);/* var(j); */
  poly y=p_One(r);p_SetExp(y,i,1,r);p_Setm(y,r); /*var(i);  for convenience */
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
        t = gnc_p_Mult_mm(t,x,r);
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
        t = gnc_p_mm_Mult(t,y,r);
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
          t = gnc_p_Mult_mm(t,x,r);
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
        t = gnc_p_mm_Mult(t,y,r);
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
          t = gnc_p_mm_Mult(t,y,r);
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
        t = gnc_p_Mult_mm(t,x,r);
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
poly gnc_ReduceSpolyOld(const poly p1, poly p2/*,poly spNoether*/, const ring r)
{
  assume(p_LmDivisibleBy(p1, p2, r));

#ifdef PDEBUG
  if (p_GetComp(p1,r)!=p_GetComp(p2,r)
  && (p_GetComp(p1,r)!=0)
  && (p_GetComp(p2,r)!=0))
  {
    dReportError("nc_ReduceSpolyOld: different components");
    return(NULL);
  }
#endif
  poly m = p_One(r);
  p_ExpVectorDiff(m,p2,p1,r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif
  /* pSetComp(m,r)=0? */
  poly   N  = nc_mm_Mult_p(m, p_Head(p1,r), r);
  number C  =  p_GetCoeff(N,  r);
  number cF =  p_GetCoeff(p2, r);
  /* GCD stuff */
  number cG = n_SubringGcd(C, cF, r->cf);
  if ( !n_IsOne(cG,r->cf) )
  {
    cF = n_Div(cF, cG, r->cf); n_Normalize(cF, r->cf);
    C  = n_Div(C,  cG, r->cf); n_Normalize(C, r->cf);
  }
  else
  {
    cF = n_Copy(cF, r->cf);
    C  = n_Copy(C, r->cf);
  }
  n_Delete(&cG,r->cf);
  p2 = __p_Mult_nn(p2, C, r);
  poly out = nc_mm_Mult_pp(m, pNext(p1), r);
  N = p_Add_q(N, out, r);
  p_Test(p2,r);
  p_Test(N,r);
  if (!n_IsMOne(cF,r->cf))
  {
    cF = n_InpNeg(cF,r->cf);
    N  = __p_Mult_nn(N, cF, r);
    p_Test(N,r);
  }
  out = p_Add_q(p2,N,r);
  p_Test(out,r);
  if ( out!=NULL ) p_Cleardenom(out,r);
  p_Delete(&m,r);
  n_Delete(&cF,r->cf);
  n_Delete(&C,r->cf);
  return(out);
}

poly gnc_ReduceSpolyNew(const poly p1, poly p2, const ring r)
{
  assume(p_LmDivisibleBy(p1, p2, r));

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    WerrorS("gnc_ReduceSpolyNew: different non-zero components!");
#endif
    return(NULL);
  }

  poly m = p_One(r);
  p_ExpVectorDiff(m, p2, p1, r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif

  /* pSetComp(m,r)=0? */
  poly   N  = nc_mm_Mult_p(m, p_Head(p1,r), r);

  number C  = p_GetCoeff(N,  r);
  number cF = p_GetCoeff(p2, r);

  /* GCD stuff */
  number cG = n_SubringGcd(C, cF, r->cf);

  if (!n_IsOne(cG, r->cf))
  {
    cF = n_Div(cF, cG, r->cf); n_Normalize(cF, r->cf);
    C  = n_Div(C,  cG, r->cf); n_Normalize(C, r->cf);
  }
  else
  {
    cF = n_Copy(cF, r->cf);
    C  = n_Copy(C, r->cf);
  }
  n_Delete(&cG,r->cf);

  p2 = __p_Mult_nn(p2, C, r); // p2 !!!
  p_Test(p2,r);
  n_Delete(&C,r->cf);

  poly out = nc_mm_Mult_pp(m, pNext(p1), r);
  p_Delete(&m,r);

  N = p_Add_q(N, out, r);
  p_Test(N,r);

  if (!n_IsMOne(cF,r->cf)) // ???
  {
    cF = n_InpNeg(cF,r->cf);
    N  = __p_Mult_nn(N, cF, r);
    p_Test(N,r);
  }
  n_Delete(&cF,r->cf);

  out = p_Add_q(p2,N,r); // delete N, p2
  p_Test(out,r);
  if ( out!=NULL ) p_Cleardenom(out,r);
  return(out);
}


/*4
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
poly gnc_CreateSpolyOld(poly p1, poly p2/*,poly spNoether*/, const ring r)
{
#ifdef PDEBUG
  if ((p_GetComp(p1,r)!=p_GetComp(p2,r))
  && (p_GetComp(p1,r)!=0)
  && (p_GetComp(p2,r)!=0))
  {
    dReportError("gnc_CreateSpolyOld : different components!");
    return(NULL);
  }
#endif
  if ((ncRingType(r)==nc_lie) && p_HasNotCF(p1,p2, r)) /* prod crit */
  {
    return(nc_p_Bracket_qq(p_Copy(p2, r),p1, r));
  }
  poly pL=p_One(r);
  poly m1=p_One(r);
  poly m2=p_One(r);
  pL = p_Lcm(p1,p2,r);
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
  number C1  = p_GetCoeff(M1,r);
  poly M2    = nc_mm_Mult_p(m2,p_Head(p2,r),r);
  number C2  = p_GetCoeff(M2,r);
  /* GCD stuff */
  number C = n_SubringGcd(C1,C2,r->cf);
  if (!n_IsOne(C,r->cf))
  {
    C1=n_Div(C1,C, r->cf);n_Normalize(C1,r->cf);
    C2=n_Div(C2,C, r->cf);n_Normalize(C2,r->cf);
  }
  else
  {
    C1=n_Copy(C1, r->cf);
    C2=n_Copy(C2, r->cf);
  }
  n_Delete(&C,r->cf);
  M1=__p_Mult_nn(M1,C2,r);
  p_SetCoeff(m1,C2,r);
  if (n_IsMOne(C1,r->cf))
  {
    M2=p_Add_q(M1,M2,r);
  }
  else
  {
    C1=n_InpNeg(C1,r->cf);
    M2=__p_Mult_nn(M2,C1,r);
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
#ifdef PDEBUG
  p_Test(M2,r);
#endif
  if (M2!=NULL) M2=p_Cleardenom(M2,r);
  return(M2);
}

poly gnc_CreateSpolyNew(poly p1, poly p2/*,poly spNoether*/, const ring r)
{
#ifdef PDEBUG
  p_Test(p1, r);
  p_Test(p2, r);
#if MYTEST
  PrintS("p1: "); p_Write(p1, r);
  PrintS("p2: "); p_Write(p2, r);
#endif
#endif

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    WerrorS("gnc_CreateSpolyNew: different non-zero components!");
    assume(0);
#endif
    return(NULL);
  }

//   if ((r->GetNC()->type==nc_lie) && pHasNotCF(p1,p2)) /* prod crit */
//   {
//     return(nc_p_Bracket_qq(pCopy(p2),p1));
//   }

//  poly pL=p_One( r);

  poly m1=p_One( r);
  poly m2=p_One( r);

  poly pL = p_Lcm(p1,p2,r);                               // pL = lcm( lm(p1), lm(p2) )


#ifdef PDEBUG
//  p_Test(pL,r);
#endif

  p_ExpVectorDiff(m1, pL, p1, r);                  // m1 = pL / lm(p1)
  //p_SetComp(m1,0,r);
  //p_Setm(m1,r);

#ifdef PDEBUG
  p_Test(m1,r);
#endif
//  assume(p_GetComp(m1,r) == 0);

  p_ExpVectorDiff(m2, pL, p2, r);                  // m2 = pL / lm(p2)

  //p_SetComp(m2,0,r);
  //p_Setm(m2,r);
#ifdef PDEBUG
  p_Test(m2,r);
#endif

#ifdef PDEBUG
#if MYTEST
  PrintS("m1: "); pWrite(m1);
  PrintS("m2: "); pWrite(m2);
#endif
#endif


//  assume(p_GetComp(m2,r) == 0);

#ifdef PDEBUG
#if 0
  if(  (p_GetComp(m2,r) != 0) || (p_GetComp(m1,r) != 0) )
  {
    WarnS("gnc_CreateSpolyNew: wrong monomials!");


#ifdef RDEBUG
    PrintS("m1 = "); p_Write(m1, r);
    p_DebugPrint(m1, r);

    PrintS("m2 = "); p_Write(m2, r);
    p_DebugPrint(m2, r);

    PrintS("p1 = "); p_Write(p1, r);
    p_DebugPrint(p1, r);

    PrintS("p2 = "); p_Write(p2, r);
    p_DebugPrint(p2, r);

    PrintS("pL = "); p_Write(pL, r);
    p_DebugPrint(pL, r);
#endif

  }

#endif
#endif

  p_LmFree(&pL,r);

  /* zero exponents !? */
  poly M1    = nc_mm_Mult_p(m1,p_Head(p1,r),r); // M1 = m1 * lt(p1)
  poly M2    = nc_mm_Mult_p(m2,p_Head(p2,r),r); // M2 = m2 * lt(p2)

#ifdef PDEBUG
  p_Test(M1,r);
  p_Test(M2,r);

#if MYTEST
  PrintS("M1: "); pWrite(M1);
  PrintS("M2: "); pWrite(M2);
#endif
#endif

  if(M1 == NULL || M2 == NULL)
  {
#ifdef PDEBUG
       PrintS("\np1 = ");
       p_Write(p1, r);

       PrintS("m1 = ");
       p_Write(m1, r);

       PrintS("p2 = ");
       p_Write(p2, r);

       PrintS("m2 = ");
       p_Write(m2, r);

       WerrorS("ERROR in nc_CreateSpoly: result of multiplication is Zero!\n");
#endif
       return(NULL);
  }

  number C1  = p_GetCoeff(M1,r);      // C1 = lc(M1)
  number C2  = p_GetCoeff(M2,r);      // C2 = lc(M2)

  /* GCD stuff */
  number C = n_SubringGcd(C1, C2, r->cf);           // C = gcd(C1, C2)

  if (!n_IsOne(C, r->cf))                              // if C != 1
  {
    C1=n_Div(C1, C, r->cf);n_Normalize(C1,r->cf);            // C1 = C1 / C
    C2=n_Div(C2, C, r->cf);n_Normalize(C2,r->cf);            // C2 = C2 / C
  }
  else
  {
    C1=n_Copy(C1,r->cf);
    C2=n_Copy(C2,r->cf);
  }

  n_Delete(&C,r->cf); // destroy the number C

  C1=n_InpNeg(C1,r->cf);

//   number MinusOne=n_Init(-1,r);
//   if (n_Equal(C1,MinusOne,r))                   // lc(M1) / gcd( lc(M1), lc(M2)) == -1 ????
//   {
//     M2=p_Add_q(M1,M2,r);                        // ?????
//   }
//   else
//   {
  M1=__p_Mult_nn(M1,C2,r);                           // M1 = (C2*lc(p1)) * (lcm(lm(p1),lm(p2)) / lm(p1)) * lm(p1)

#ifdef PDEBUG
  p_Test(M1,r);
#endif

  M2=__p_Mult_nn(M2,C1,r);                           // M2 =(-C1*lc(p2)) * (lcm(lm(p1),lm(p2)) / lm(p2)) * lm(p2)



#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  PrintS("M1: "); pWrite(M1);
  PrintS("M2: "); pWrite(M2);
#endif
#endif


  M2=p_Add_q(M1,M2,r);                             // M1 is killed, M2 = spoly(lt(p1), lt(p2)) = C2*M1 - C1*M2

#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  PrintS("M2: "); pWrite(M2);
#endif

#endif

// M2 == 0 for supercommutative algebras!
//   }
//   n_Delete(&MinusOne,r);

  p_SetCoeff(m1,C2,r);                           // lc(m1) = C2!!!
  p_SetCoeff(m2,C1,r);                           // lc(m2) = C1!!!

#ifdef PDEBUG
  p_Test(m1,r);
  p_Test(m2,r);
#endif

//  poly tmp = p_Copy(p1,r);                         // tmp = p1
//  tmp=p_LmDeleteAndNext(tmp,r);                  // tmp = tail(p1)
//#ifdef PDEBUG
//  p_Test(tmp,r);
//#endif

  M1 = nc_mm_Mult_pp(m1, pNext(p1), r);                      // M1 = m1 * tail(p1), delete tmp // ???

#ifdef PDEBUG
  p_Test(M1,r);

#if MYTEST
  PrintS("M1: "); pWrite(M1);
#endif

#endif

  M2=p_Add_q(M2,M1,r);                           // M2 = spoly(lt(p1), lt(p2)) + m1 * tail(p1), delete M1
#ifdef PDEBUG
  M1=NULL;
  p_Test(M2,r);

#if MYTEST
  PrintS("M2: "); pWrite(M2);
#endif

#endif

//  tmp=p_Copy(p2,r);                              // tmp = p2
//  tmp=p_LmDeleteAndNext(tmp,r);                  // tmp = tail(p2)

//#ifdef PDEBUG
//  p_Test(tmp,r);
//#endif

  M1 = nc_mm_Mult_pp(m2, pNext(p2), r);                      // M1 = m2 * tail(p2), detele tmp

#ifdef PDEBUG
  p_Test(M1,r);

#if MYTEST
  PrintS("M1: "); pWrite(M1);
#endif

#endif

  M2 = p_Add_q(M2,M1,r);                           // M2 = spoly(lt(p1), lt(p2)) + m1 * tail(p1) + m2*tail(p2)

#ifdef PDEBUG
  M1=NULL;
  p_Test(M2,r);

#if MYTEST
  PrintS("M2: "); pWrite(M2);
#endif

#endif

  p_Delete(&m1,r);  //  => n_Delete(&C1,r);
  p_Delete(&m2,r);  //  => n_Delete(&C2,r);

#ifdef PDEBUG
  p_Test(M2,r);
#endif

  if (M2!=NULL) p_Cleardenom(M2,r);

  return(M2);
}




#if 0
/*5
* reduction of tail(q) with p1
* lead(p1) divides lead(pNext(q2)) and pNext(q2) is reduced
* do not destroy p1, but tail(q)
*/
void gnc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r)
{
  poly a1=p_Head(p1,r);
  poly Q=pNext(q2);
  number cQ=p_GetCoeff(Q,r);
  poly m=p_One(r);
  p_ExpVectorDiff(m,Q,p1,r);
  //  p_SetComp(m,0,r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif
  /* pSetComp(m,r)=0? */
  poly M = nc_mm_Mult_pp(m, p1,r);
  number C=p_GetCoeff(M,r);
  M=p_Add_q(M,nc_mm_Mult_p(m,p_LmDeleteAndNext(p_Copy(p1,r),r),r),r); // _pp?
  q=__p_Mult_nn(q,C,r);
  number MinusOne=n_Init(-1,r->cf);
  if (!n_Equal(cQ,MinusOne,r->cf))
  {
    cQ=nInpNeg(cQ);
    M=__p_Mult_nn(M,cQ,r);
  }
  Q=p_Add_q(Q,M,r);
  pNext(q2)=Q;

  p_Delete(&m,r);
  n_Delete(&C,r->cf);
  n_Delete(&cQ,r->cf);
  n_Delete(&MinusOne,r->cf);
  /*  return(q); */
}
#endif


/*6
* creates the commutative lcm(lm(p1),lm(p2))
* do not destroy p1 and p2
*/
poly nc_CreateShortSpoly(poly p1, poly p2, const ring r)
{
#ifdef PDEBUG
  p_Test(p1, r);
  p_Test(p2, r);
#endif

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    WerrorS("nc_CreateShortSpoly: wrong module components!"); // !!!!
#endif
    return(NULL);
  }

  poly m;

#ifdef HAVE_RATGRING
  if ( rIsRatGRing(r))
  {
    /* rational version */
    m = p_LcmRat(p1, p2, si_max(lCompP1, lCompP2), r);
  } else
#endif
  {
    m = p_Lcm(p1, p2, r);
  }

  pSetCoeff0(m,NULL);

  return(m);
}

void gnc_kBucketPolyRedOld(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  // b will not be multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=n_Init(1, r->cf);
  poly m=p_One(r);
  p_ExpVectorDiff(m,kBucketGetLm(b),p, r);
  //pSetm(m);
#ifdef PDEBUG
  p_Test(m, r);
#endif
  poly pp= nc_mm_Mult_pp(m,p, r);
  assume(pp!=NULL);
  p_Delete(&m, r);
  number n=pGetCoeff(pp);
  number nn;
  if (!n_IsMOne(n, r->cf))
  {
    nn=n_InpNeg(n_Invers(n, r->cf), r->cf);
    n= n_Mult(nn,pGetCoeff(kBucketGetLm(b)), r->cf);
    n_Delete(&nn, r->cf);
    pp=__p_Mult_nn(pp,n,r);
    n_Delete(&n, r->cf);
  }
  else
  {
    pp=__p_Mult_nn(pp,p_GetCoeff(kBucketGetLm(b), r),r);
  }
  int l=pLength(pp);
  kBucket_Add_q(b,pp,&l);
}

void gnc_kBucketPolyRedNew(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
#ifdef PDEBUG
//   PrintS(">*");
#endif

#ifdef KDEBUG
  if( !kbTest(b) ) WerrorS("nc_kBucketPolyRed: broken bucket!");
#endif

#ifdef PDEBUG
  p_Test(p, r);
#if MYTEST
  PrintS("p: "); p_Write(p, r);
#endif
#endif

  // b will not be multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=n_Init(1, r->cf);
  poly m = p_One(r);
  const poly pLmB = kBucketGetLm(b); // no new copy!

  assume( pLmB != NULL );

#ifdef PDEBUG
  p_Test(pLmB, r);

#if MYTEST
  PrintS("pLmB: "); p_Write(pLmB, r);
#endif
#endif

  p_ExpVectorDiff(m, pLmB, p, r);
  //pSetm(m);

#ifdef PDEBUG
  p_Test(m, r);
#if MYTEST
  PrintS("m: "); p_Write(m, r);
#endif
#endif

  poly pp = nc_mm_Mult_pp(m, p, r);
  p_Delete(&m, r);

  assume( pp != NULL );
  const number n = pGetCoeff(pp); // bug!

  if (!n_IsMOne(n, r->cf) ) // does this improve performance??!? also see below... // TODO: check later on.
  // if n == -1 => nn = 1 and -1/n
  {
    number nn=n_InpNeg(n_Invers(n, r->cf), r->cf);
    number t = n_Mult(nn,pGetCoeff(pLmB), r->cf);
    n_Delete(&nn, r->cf);
    pp = __p_Mult_nn(pp,t,r);
    n_Delete(&t, r->cf);
  }
  else
  {
    pp = __p_Mult_nn(pp,p_GetCoeff(pLmB, r), r);
  }

  int l = pLength(pp);

#ifdef PDEBUG
  p_Test(pp, r);
//   PrintS("PP: "); pWrite(pp);
#endif

  kBucket_Add_q(b,pp,&l);


#ifdef PDEBUG
//   PrintS("*>");
#endif
}


void gnc_kBucketPolyRed_ZOld(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  // b is multiplied by a constant in this impl.
  number ctmp;
  poly m=p_One(r);
  p_ExpVectorDiff(m,kBucketGetLm(b),p, r);
  //pSetm(m);
#ifdef PDEBUG
  p_Test(m, r);
#endif
  if(p_IsConstant(m,r))
  {
    p_Delete(&m, r);
    ctmp = kBucketPolyRed(b,p,pLength(p),NULL);
  }
  else
  {
    poly pp = nc_mm_Mult_pp(m,p,r);
    number c2;
    p_Cleardenom_n(pp,r,c2);
    p_Delete(&m, r);
    ctmp = kBucketPolyRed(b,pp,pLength(pp),NULL);
    //cc=*c;
    //*c=nMult(*c,c2);
    n_Delete(&c2, r->cf);
    //nDelete(&cc);
    p_Delete(&pp, r);
  }
  if (c!=NULL) *c=ctmp;
  else n_Delete(&ctmp, r->cf);
}

void gnc_kBucketPolyRed_ZNew(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  // b is multiplied by a constant in this impl.
  number ctmp;
  poly m=p_One(r);
  p_ExpVectorDiff(m,kBucketGetLm(b),p, r);
  //pSetm(m);
#ifdef PDEBUG
  p_Test(m, r);
#endif

  if(p_IsConstant(m,r))
  {
    p_Delete(&m, r);
    ctmp = kBucketPolyRed(b,p,pLength(p),NULL);
  }
  else
  {
    poly pp = nc_mm_Mult_pp(m,p,r);
    number c2;
    p_Cleardenom_n(pp,r,c2);
    p_Delete(&m, r);
    ctmp = kBucketPolyRed(b,pp,pLength(pp),NULL);
    //cc=*c;
    //*c=nMult(*c,c2);
    n_Delete(&c2, r->cf);
    //nDelete(&cc);
    p_Delete(&pp, r);
  }
  if (c!=NULL) *c=ctmp;
  else n_Delete(&ctmp, r->cf);
}


inline void nc_PolyPolyRedOld(poly &b, poly p, number *c, const ring r)
  // reduces b with p, do not delete both
{
  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=n_Init(1, r->cf);
  poly m=p_One(r);
  p_ExpVectorDiff(m,p_Head(b, r),p, r);
  //pSetm(m);
#ifdef PDEBUG
  p_Test(m, r);
#endif
  poly pp=nc_mm_Mult_pp(m,p,r);
  assume(pp!=NULL);

  p_Delete(&m, r);
  number n=pGetCoeff(pp);
  number nn;
  if (!n_IsMOne(n, r->cf))
  {
    nn=n_InpNeg(n_Invers(n, r->cf), r->cf);
    n =n_Mult(nn,pGetCoeff(b), r->cf);
    n_Delete(&nn, r->cf);
    pp=__p_Mult_nn(pp,n,r);
    n_Delete(&n, r->cf);
  }
  else
  {
    pp=__p_Mult_nn(pp,p_GetCoeff(b, r),r);
  }
  b=p_Add_q(b,pp,r);
}


inline void nc_PolyPolyRedNew(poly &b, poly p, number *c, const ring r)
  // reduces b with p, do not delete both
{
#ifdef PDEBUG
  p_Test(b, r);
  p_Test(p, r);
#endif

#if MYTEST
  PrintS("nc_PolyPolyRedNew(");
  p_Write0(b, r);
  PrintS(", ");
  p_Write0(p, r);
  PrintS(", *c): ");
#endif

  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=n_Init(1, r->cf);

  poly pp = NULL;

  // there is a problem when p is a square(=>0!)

  while((b != NULL) && (pp == NULL))
  {

//    poly pLmB = p_Head(b, r);
    poly m = p_One(r);
    p_ExpVectorDiff(m, b, p, r);
//    pDelete(&pLmB);
  //pSetm(m);

#ifdef PDEBUG
    p_Test(m, r);
    p_Test(b, r);
#endif

    pp = nc_mm_Mult_pp(m, p, r);

#if MYTEST
    PrintS("\n{b': ");
    p_Write0(b, r);
    PrintS(", m: ");
    p_Write0(m, r);
    PrintS(", pp: ");
    p_Write0(pp, r);
    PrintS(" }\n");
#endif

    p_Delete(&m, r); // one m for all tries!

//    assume( pp != NULL );

    if( pp == NULL )
    {
      b = p_LmDeleteAndNext(b, r);

      if( !p_DivisibleBy(p, b, r) )
        return;

    }
  }

#if MYTEST
  PrintS("{b': ");
  p_Write0(b, r);
  PrintS(", pp: ");
  p_Write0(pp, r);
  PrintS(" }\n");
#endif


  if(b == NULL) return;


  assume(pp != NULL);

  const number n = pGetCoeff(pp); // no new copy

  number nn;

  if (!n_IsMOne(n, r->cf)) // TODO: as above.
  {
    nn=n_InpNeg(n_Invers(n, r->cf), r->cf);
    number t = n_Mult(nn, pGetCoeff(b), r->cf);
    n_Delete(&nn, r->cf);
    pp=__p_Mult_nn(pp, t, r);
    n_Delete(&t, r->cf);
  }
  else
  {
    pp=__p_Mult_nn(pp, pGetCoeff(b), r);
  }


  b=p_Add_q(b,pp,r);

}

void nc_PolyPolyRed(poly &b, poly p, number *c, const ring r)
{
#if 0
  nc_PolyPolyRedOld(b, p, c, r);
#else
  nc_PolyPolyRedNew(b, p, c, r);
#endif
}


poly nc_mm_Bracket_nn(poly m1, poly m2, const ring r);

/// returns [p,q], destroys p
poly nc_p_Bracket_qq(poly p, const poly q, const ring r)
{
  assume(p != NULL && q!= NULL);

  if (!rIsPluralRing(r)) return(NULL);
  if (p_ComparePolys(p,q, r)) return(NULL);
  /* Components !? */
  poly Q=NULL;
  number coef=NULL;
  poly pres=NULL;
  int UseBuckets=1;
  if (((pLength(p)< MIN_LENGTH_BUCKET/2) && (pLength(q)< MIN_LENGTH_BUCKET/2))
  || TEST_OPT_NOT_BUCKETS)
    UseBuckets=0;


  CPolynomialSummator sum(r, UseBuckets == 0);

  while (p!=NULL)
  {
    Q=q;
    while(Q!=NULL)
    {
      pres=nc_mm_Bracket_nn(p,Q, r); /* since no coeffs are taken into account there */
      if (pres!=NULL)
      {
        coef = n_Mult(pGetCoeff(p),pGetCoeff(Q), r->cf);
        pres = __p_Mult_nn(pres,coef,r);

        sum += pres;
        n_Delete(&coef, r->cf);
      }
      pIter(Q);
    }
    p=p_LmDeleteAndNext(p, r);
  }
  return(sum);
}

/// returns [m1,m2] for two monoms, destroys nothing
/// without coeffs
poly nc_mm_Bracket_nn(poly m1, poly m2, const ring r)
{
  if (p_LmIsConstant(m1, r) || p_LmIsConstant(m1, r)) return(NULL);
  if (p_LmCmp(m1,m2, r)==0) return(NULL);
  int rN=r->N;
  int *M1=(int *)omAlloc0((rN+1)*sizeof(int));
  int *M2=(int *)omAlloc0((rN+1)*sizeof(int));
  int *aPREFIX=(int *)omAlloc0((rN+1)*sizeof(int));
  int *aSUFFIX=(int *)omAlloc0((rN+1)*sizeof(int));
  p_GetExpV(m1,M1, r);
  p_GetExpV(m2,M2, r);
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
          if ( (i==j) || ((MATELEM(r->GetNC()->COM,nMin,nMax)!=NULL) && n_IsOne(pGetCoeff(MATELEM(r->GetNC()->C,nMin,nMax)), r->cf) )) /* not (the same exp. or commuting exps)*/
          { bres=NULL; }
          else
          {
            if (i<j) { bres=gnc_uu_Mult_ww(j,M1[j],i,M2[i], r); }
            else bres=gnc_uu_Mult_ww(i,M2[i],j,M1[j], r);
            if (n_IsOne(pGetCoeff(bres), r->cf))
            {
              bres=p_LmDeleteAndNext(bres, r);
            }
            else
            {
              nTmp=n_Sub(pGetCoeff(bres),n_Init(1, r->cf), r->cf);
              p_SetCoeff(bres,nTmp, r); /* only lc ! */
            }
#ifdef PDEBUG
            p_Test(bres, r);
#endif
            if (i>j)  bres=p_Neg(bres, r);
          }
          if (bres!=NULL)
          {
            /* now mult (prefix, bres, suffix) */
            memcpy(aSUFFIX, M1,(rN+1)*sizeof(int));
            memcpy(aPREFIX, M1,(rN+1)*sizeof(int));
            for (k=1;k<=j;k++) aSUFFIX[k]=0;
            for (k=j;k<=rN;k++) aPREFIX[k]=0;
            aSUFFIX[0]=0;
            aPREFIX[0]=0;
            prefix=p_One(r);
            suffix=p_One(r);
            p_SetExpV(prefix,aPREFIX, r);
            p_Setm(prefix, r);
            p_SetExpV(suffix,aSUFFIX, r);
            p_Setm(suffix, r);
            if (!p_LmIsConstant(prefix, r)) bres = gnc_p_mm_Mult(bres, prefix, r);
            if (!p_LmIsConstant(suffix, r)) bres = gnc_p_Mult_mm(bres, suffix, r);
            ares=p_Add_q(ares, bres, r);
            /* What to give free? */
        /* Do we have to free aPREFIX/aSUFFIX? it seems so */
            p_Delete(&prefix, r);
            p_Delete(&suffix, r);
          }
        }
      }
      if (ares!=NULL)
      {
        /* now mult (prefix, bres, suffix) */
        memcpy(aSUFFIX, M2,(rN+1)*sizeof(int));
        memcpy(aPREFIX, M2,(rN+1)*sizeof(int));
        for (k=1;k<=i;k++) aSUFFIX[k]=0;
        for (k=i;k<=rN;k++) aPREFIX[k]=0;
        aSUFFIX[0]=0;
        aPREFIX[0]=0;
        prefix=p_One(r);
        suffix=p_One(r);
        p_SetExpV(prefix,aPREFIX, r);
        p_Setm(prefix, r);
        p_SetExpV(suffix,aSUFFIX, r);
        p_Setm(suffix, r);
        bres=ares;
        if (!p_LmIsConstant(prefix, r)) bres = gnc_p_mm_Mult(bres, prefix, r);
        if (!p_LmIsConstant(suffix, r)) bres = gnc_p_Mult_mm(bres, suffix, r);
        res=p_Add_q(res, bres, r);
        p_Delete(&prefix, r);
        p_Delete(&suffix, r);
      }
    }
  }
  freeT(M1, rN);
  freeT(M2, rN);
  freeT(aPREFIX, rN);
  freeT(aSUFFIX, rN);
#ifdef PDEBUG
  p_Test(res, r);
#endif
   return(res);
}
/// returns matrix with the info on noncomm multiplication
matrix nc_PrintMat(int a, int b, ring r, int metric)
{

  if ( (a==b) || !rIsPluralRing(r) ) return(NULL);
  int i;
  int j;
  if (a>b) {j=b; i=a;}
  else {j=a; i=b;}
  /* i<j */
  int rN=r->N;
  int size=r->GetNC()->MTsize[UPMATELEM(i,j,rN)];
  matrix M = r->GetNC()->MT[UPMATELEM(i,j,rN)];
  /*  return(M); */
/*
  int sizeofres;
  if (metric==0)
  {
    sizeofres=sizeof(int);
  }
  if (metric==1)
  {
    sizeofres=sizeof(number);
  }
*/
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
            totdeg=totdeg+p_Deg(p,r);
            pIter(p);
          }
          number ntd = n_Init(totdeg, r->cf);
          number nln = n_Init(length, r->cf);
          number nres= n_Div(ntd,nln, r->cf);
          n_Delete(&ntd, r->cf);
          n_Delete(&nln, r->cf);
          MATELEM(res,s,t)=p_NSet(nres,r);
        }
      }
    }
  }
  return(res);
}

inline void nc_CleanUp(nc_struct* p)
{
  assume(p != NULL);
  omFreeSize((ADDRESS)p,sizeof(nc_struct));
}

inline void nc_CleanUp(ring r)
{
  /* small CleanUp of r->GetNC() */
  assume(r != NULL);
  nc_CleanUp(r->GetNC());
  r->GetNC() = NULL;
}

void nc_rKill(ring r)
// kills the nc extension of ring r
{
  if( r->GetNC()->GetGlobalMultiplier() != NULL )
  {
    delete r->GetNC()->GetGlobalMultiplier();
    r->GetNC()->GetGlobalMultiplier() = NULL;
  }

  if( r->GetNC()->GetFormulaPowerMultiplier() != NULL )
  {
    delete r->GetNC()->GetFormulaPowerMultiplier();
    r->GetNC()->GetFormulaPowerMultiplier() = NULL;
  }


  int i,j;
  int rN=r->N;
  if ( rN > 1 )
  {
    for(i=1;i<rN;i++)
    {
      for(j=i+1;j<=rN;j++)
      {
        id_Delete((ideal *)&(r->GetNC()->MT[UPMATELEM(i,j,rN)]),r);
      }
    }
    omFreeSize((ADDRESS)r->GetNC()->MT,rN*(rN-1)/2*sizeof(matrix));
    omFreeSize((ADDRESS)r->GetNC()->MTsize,rN*(rN-1)/2*sizeof(int));
    id_Delete((ideal *)&(r->GetNC()->COM),r);
  }
  id_Delete((ideal *)&(r->GetNC()->C),r);
  id_Delete((ideal *)&(r->GetNC()->D),r);

  if( rIsSCA(r) && (r->GetNC()->SCAQuotient() != NULL) )
  {
    id_Delete(&r->GetNC()->SCAQuotient(), r); // Custom SCA destructor!!!
  }


  nc_CleanUp(r);
}


////////////////////////////////////////////////////////////////////////////////////////////////

// deprecated:
/* for use in getting the mult. matrix elements*/
/* ring r must be a currRing! */
/* for consistency, copies a poly from the comm. r->GetNC()->basering */
/* to its image in NC ring */
poly nc_p_CopyGet(poly a, const ring r)
{
#ifndef PDEBUG
  p_Test(a, r);
#endif

//  if (r != currRing)
//  {
//#ifdef PDEBUF
//    WerrorS("nc_p_CopyGet call not in currRing");
//#endif
//    return(NULL);
//  }
  return(p_Copy(a,r));
}

// deprecated:
/* for use in defining the mult. matrix elements*/
/* ring r must be a currRing! */
/* for consistency, puts a polynomial from the NC ring */
/* to its presentation in the comm. r->GetNC()->basering */
poly nc_p_CopyPut(poly a, const ring r)
{
#ifndef PDEBUG
  p_Test(a, r);
#endif

//  if (r != currRing)
//  {
//#ifdef PDEBUF
//    WerrorS("nc_p_CopyGet call not in currRing");
//#endif
//    return(NULL);
//  }

  return(p_Copy(a,r));
}

/* returns TRUE if there were errors */
/* checks whether product of vars from PolyVar defines */
/* an admissible subalgebra of r */
/* r is indeed currRing and assumed to be noncomm. */
BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r)
{
//  ring save = currRing;
//  int WeChangeRing = 0;
//  if (currRing != r)
//    rChangeCurrRing(r);
//    WeChangeRing = 1;
//  }
  int rN=r->N;
  int *ExpVar=(int*)omAlloc0((rN+1)*sizeof(int));
  int *ExpTmp=(int*)omAlloc0((rN+1)*sizeof(int));
  p_GetExpV(PolyVar, ExpVar, r);
  int i; int j; int k;
  poly test=NULL;
  int OK=1;
  for (i=1; i<rN; i++)
  {
    if (ExpVar[i]==0) /* i.e. not in PolyVar */
    {
      for (j=i+1; j<=rN; j++)
      {
        if (ExpVar[j]==0)
        {
          test = MATELEM(r->GetNC()->D,i,j);
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
            if (!OK)
            {
//              if ( WeChangeRing )
//                rChangeCurrRing(save);
              return(TRUE);
            }
            pIter(test);
          }
        }
      }
    }
  }
  freeT(ExpVar,rN);
  freeT(ExpTmp,rN);
//  if ( WeChangeRing )
//    rChangeCurrRing(save);
  return(FALSE);
}


/* returns TRUE if there were errors */
/* checks whether the current ordering */
/* is admissible for r and D == r->GetNC()->D */
/* to be executed in a currRing */
BOOLEAN gnc_CheckOrdCondition(matrix D, ring r)
{
  /* analyze D: an upper triangular matrix of polys */
  /* check the ordering condition for D */
//  ring save = currRing;
//  int WeChangeRing = 0;
//  if (r != currRing)
//  {
//    rChangeCurrRing(r);
//    WeChangeRing = 1;
//  }
  poly p,q;
  int i,j;
  int report = 0;
  for(i=1; i<r->N; i++)
  {
    for(j=i+1; j<=r->N; j++)
    {
      p = nc_p_CopyGet(MATELEM(D,i,j),r);
      if ( p != NULL)
      {
         q = p_One(r);
         p_SetExp(q,i,1,r);
         p_SetExp(q,j,1,r);
         p_Setm(q,r);
         if (p_LmCmp(q,p,r) != 1) /* i.e. lm(p)==xy < lm(q)==D_ij  */
           {
              Werror("Bad ordering at %d,%d\n",i,j);
#if 0 /*Singularg should not differ from Singular except in error case*/
      p_Write(p,r);
      p_Write(q,r);
#endif
              report = 1;
           }
         p_Delete(&q,r);
         p_Delete(&p,r);
         p = NULL;
      }
    }
  }
//  if ( WeChangeRing )
//    rChangeCurrRing(save);
  return(report);
}



BOOLEAN gnc_InitMultiplication(ring r, bool bSetupQuotient = false); // just for a moment

/// returns TRUE if there were errors
/// analyze inputs, check them for consistency
/// detects nc_type, DO NOT initialize multiplication but call for it at the end
/// checks the ordering condition and evtl. NDC
/// NOTE: all the data belong to the curr,
/// we change r which may be the same ring, and must have the same representation!
BOOLEAN nc_CallPlural(matrix CCC, matrix DDD,
                      poly CCN, poly DDN,
                      ring r,
                      bool bSetupQuotient, bool bCopyInput, bool bBeQuiet,
                      ring curr, bool dummy_ring /*=false*/)
{
  assume( r != NULL );
  assume( curr != NULL );

  if( !bSetupQuotient)
    assume( (r->qideal == NULL) ); // The basering must NOT be a qring!??

  assume( rSamePolyRep(r, curr) || bCopyInput ); // wrong assumption?


  if( r->N == 1 ) // clearly commutative!!!
  {
    assume(
           ( (CCC != NULL) && (MATCOLS(CCC) == 1) && (MATROWS(CCC) == 1) && (MATELEM(CCC,1,1) == NULL) ) ||
           ( (CCN == NULL) )
          );

    assume(
           ( (DDD != NULL) && (MATCOLS(DDD) == 1) && (MATROWS(DDD) == 1) && (MATELEM(DDD,1,1) == NULL) ) ||
           ( (DDN == NULL) )
          );
    if(!dummy_ring)
    {
      WarnS("commutative ring with 1 variable");
      return FALSE;
    }
  }

  // there must be:
  assume( (CCC != NULL) != (CCN != NULL) ); // exactly one data about coeffs (C).
  assume( !((DDD != NULL) && (DDN != NULL)) ); // at most one data about tails (D).

//  ring save = currRing;
//  if( save != curr )
//    rChangeCurrRing(curr);


#if OUTPUT
  if( CCC != NULL )
  {
    PrintS("nc_CallPlural(), Input data, CCC: \n");
    iiWriteMatrix(CCC, "C", 2, curr, 4);
  }
  if( DDD != NULL )
  {
    PrintS("nc_CallPlural(), Input data, DDD: \n");
    iiWriteMatrix(DDD, "D", 2, curr, 4);
  }
#endif


#ifndef SING_NDEBUG
  if (CCC!=NULL) id_Test((ideal)CCC, curr);
  if (DDD!=NULL) id_Test((ideal)DDD, curr);
  p_Test(CCN, curr);
  p_Test(DDN, curr);
#endif

  if( (!bBeQuiet) && (r->GetNC() != NULL) )
    WarnS("going to redefine the algebra structure");

//  if( currRing != r )
//    rChangeCurrRing(r);

  matrix CC = NULL;
  poly CN = NULL;
  matrix C; bool bCnew = false;

  matrix DD = NULL;
  poly DN = NULL;
  matrix D; bool bDnew = false;

  number nN, pN, qN;

  bool IsSkewConstant = false, tmpIsSkewConstant;
  int i, j;

  nc_type nctype = nc_undef;

  //////////////////////////////////////////////////////////////////
  // check the correctness of arguments, without any real chagnes!!!



  // check C
  if ((CCC != NULL) && ( (MATCOLS(CCC)==1) || MATROWS(CCC)==1 ) )
  {
    CN = MATELEM(CCC,1,1);
  }
  else
  {
    if ((CCC != NULL) && ( (MATCOLS(CCC)!=r->N) || (MATROWS(CCC)!=r->N) ))
    {
      Werror("Square %d x %d  matrix expected", r->N, r->N);

//      if( currRing != save )
//        rChangeCurrRing(save);
      return TRUE;
    }
  }
  if (( CCC != NULL) && (CC == NULL)) CC = CCC; // mp_Copy(CCC, ?); // bug!?
  if (( CCN != NULL) && (CN == NULL)) CN = CCN;

  // check D
  if ((DDD != NULL) && ( (MATCOLS(DDD)==1) || MATROWS(DDD)==1 ) )
  {
    DN = MATELEM(DDD,1,1);
  }
  else
  {
    if ((DDD != NULL) && ( (MATCOLS(DDD)!=r->N) || (MATROWS(DDD)!=r->N) ))
    {
      Werror("Square %d x %d  matrix expected",r->N,r->N);

//      if( currRing != save )
//        rChangeCurrRing(save);
      return TRUE;
    }
  }

  if (( DDD != NULL) && (DD == NULL)) DD = DDD; // mp_Copy(DDD, ?); // ???
  if (( DDN != NULL) && (DN == NULL)) DN = DDN;

  // further checks and some analysis:
  // all data in 'curr'!
  if (CN != NULL)       /* create matrix C = CN * Id */
  {
    if (!p_IsConstant(CN,curr))
    {
      WerrorS("Incorrect input : non-constants are not allowed as coefficients (first argument)");
      return TRUE;
    }
    assume(p_IsConstant(CN,curr));

    nN = pGetCoeff(CN);
    if (n_IsZero(nN, curr->cf))
    {
      WerrorS("Incorrect input : zero coefficients are not allowed");

//      if( currRing != save )
//        rChangeCurrRing(save);
      return TRUE;
    }

    if (n_IsOne(nN, curr->cf))
      nctype = nc_lie;
    else
      nctype = nc_general;

    IsSkewConstant = true;

    C = mpNew(r->N,r->N); // ring independent!
    bCnew = true;

    for(i=1; i<r->N; i++)
      for(j=i+1; j<=r->N; j++)
        MATELEM(C,i,j) = prCopyR_NoSort(CN, curr, r); // nc_p_CopyPut(CN, r); // copy CN from curr into r

#ifndef SING_NDEBUG
    id_Test((ideal)C, r);
#endif

  } else
  if ( (CN == NULL) && (CC != NULL) ) /* copy matrix C */
  {
    /* analyze C */

    BOOLEAN pN_set=FALSE;
    pN = n_Init(0,curr->cf);

    if( r->N > 1 )
      if ( MATELEM(CC,1,2) != NULL )
      {
        if (!pN_set) n_Delete(&pN,curr->cf); // free initial nInit(0)
        pN = p_GetCoeff(MATELEM(CC,1,2), curr);
	pN_set=TRUE;
      }

    tmpIsSkewConstant = true;

    for(i=1; i<r->N; i++)
      for(j=i+1; j<=r->N; j++)
      {
        if (MATELEM(CC,i,j) == NULL)
          qN = NULL;
        else
        {
          if (!p_IsConstant(MATELEM(CC,i,j),curr))
          {
            Werror("Incorrect input : non-constants are not allowed as coefficients (first argument at [%d, %d])", i, j);
            return TRUE;
          }
          assume(p_IsConstant(MATELEM(CC,i,j),curr));
          qN = p_GetCoeff(MATELEM(CC,i,j),curr);
        }


        if ( qN == NULL )   /* check the consistency: Cij!=0 */
        // find also illegal pN
        {
          WerrorS("Incorrect input : matrix of coefficients contains zeros in the upper triangle");

//        if( currRing != save )
//            rChangeCurrRing(save);
          return TRUE;
        }

        if (!n_Equal(pN, qN, curr->cf)) tmpIsSkewConstant = false;
      }

    if( bCopyInput )
    {
      C = mp_Copy(CC, curr, r); // Copy C into r!!!???
#ifndef SING_NDEBUG
      id_Test((ideal)C, r);
#endif
      bCnew = true;
    }
    else
      C = CC;

    IsSkewConstant = tmpIsSkewConstant;

    if ( tmpIsSkewConstant && n_IsOne(pN, curr->cf) )
      nctype = nc_lie;
    else
      nctype = nc_general;
    if (!pN_set) n_Delete(&pN,curr->cf); // free initial nInit(0)
  }

  /* initialition of the matrix D */
  if ( DD == NULL ) /* we treat DN only (it could also be NULL) */
  {
    D = mpNew(r->N,r->N); bDnew = true;

    if (DN  == NULL)
    {
      if ( (nctype == nc_lie) || (nctype == nc_undef) )
        nctype = nc_comm; /* it was nc_skew earlier */
      else /* nc_general, nc_skew */
        nctype = nc_skew;
    }
    else /* DN  != NULL */
      for(i=1; i<r->N; i++)
        for(j=i+1; j<=r->N; j++)
          MATELEM(D,i,j) = prCopyR_NoSort(DN, curr, r); // project DN into r->GetNC()->basering!
#ifndef SING_NDEBUG
  id_Test((ideal)D, r);
#endif
  }
  else /* DD != NULL */
  {
    bool b = true; // DD == null ?

    for(int i = 1; (i < r->N) && b; i++)
    for(int j = i+1; (j <= r->N) && b; j++)
      if (MATELEM(DD, i, j) != NULL)
      {
        b = false;
        break;
      }

    if (b) // D == NULL!!!
    {
      if ( (nctype == nc_lie) || (nctype == nc_undef) )
        nctype = nc_comm; /* it was nc_skew earlier */
      else /* nc_general, nc_skew */
        nctype = nc_skew;
    }

    if( bCopyInput )
    {
      D = mp_Copy(DD, curr, r); // Copy DD into r!!!
#ifndef SING_NDEBUG
      id_Test((ideal)D, r);
#endif
      bDnew = true;
    }
    else
      D = DD;
  }

  assume( C != NULL );
  assume( D != NULL );

#if OUTPUT
  PrintS("nc_CallPlural(), Computed data, C: \n");
  iiWriteMatrix(C, "C", 2, r, 4);

  PrintS("nc_CallPlural(), Computed data, D: \n");
  iiWriteMatrix(D, "D", 2, r, 4);

  Print("\nTemporary: type = %d, IsSkewConstant = %d\n", nctype, IsSkewConstant);
#endif


  // check the ordering condition for D (both matrix and poly cases):
  if ( gnc_CheckOrdCondition(D, r) )
  {
    if( bCnew ) mp_Delete( &C, r );
    if( bDnew ) mp_Delete( &D, r );

    WerrorS("Matrix of polynomials violates the ordering condition");

//    if( currRing != save )
//      rChangeCurrRing(save);
    return TRUE;
  }

  // okay now we are ready for this!!!

  // create new non-commutative structure
  nc_struct *nc_new = (nc_struct *)omAlloc0(sizeof(nc_struct));

  ncRingType(nc_new, nctype);

  nc_new->C = C; // if C and D were given by matrices at the beginning they are in r
  nc_new->D = D; // otherwise they should be in r->GetNC()->basering(polynomial * Id_{N})

  nc_new->IsSkewConstant = (IsSkewConstant?1:0);

  // Setup new NC structure!!!
  if (r->GetNC() != NULL)
  {
#ifndef SING_NDEBUG
    WarnS("Changing the NC-structure of an existing NC-ring!!!");
#endif
    nc_rKill(r);
  }

  r->GetNC() = nc_new;

  r->ext_ref=NULL;

//  if( currRing != save )
//    rChangeCurrRing(save);

  return gnc_InitMultiplication(r, bSetupQuotient);
}

//////////////////////////////////////////////////////////////////////////////

bool nc_rCopy(ring res, const ring r, bool bSetupQuotient)
{
  if (nc_CallPlural(r->GetNC()->C, r->GetNC()->D, NULL, NULL, res, bSetupQuotient, true, true, r))
  {
    WarnS("Error occurred while coping/setuping the NC structure!"); // No reaction!???
    return true; // error
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////
BOOLEAN gnc_InitMultiplication(ring r, bool bSetupQuotient)
{
  /* returns TRUE if there were errors */
  /* initialize the multiplication: */
  /*  r->GetNC()->MTsize, r->GetNC()->MT, r->GetNC()->COM, */
  /* and r->GetNC()->IsSkewConstant for the skew case */
  if (rVar(r)==1)
  {
    ncRingType(r, nc_comm);
    r->GetNC()->IsSkewConstant=1;
    return FALSE;
  }

//  ring save = currRing;
//  int WeChangeRing = 0;

//  if (currRing!=r)
//  {
//    rChangeCurrRing(r);
//    WeChangeRing = 1;
//  }
//  assume( (currRing == r)
//       && (currRing->GetNC()!=NULL) );   // otherwise we cannot work with all these matrices!

  int i,j;
  r->GetNC()->MT = (matrix *)omAlloc0((r->N*(r->N-1))/2*sizeof(matrix));
  r->GetNC()->MTsize = (int *)omAlloc0((r->N*(r->N-1))/2*sizeof(int));
  id_Test((ideal)r->GetNC()->C, r);
  matrix COM = mp_Copy(r->GetNC()->C, r);
  poly p,q;
  short DefMTsize=7;
  int IsNonComm=0;
//  bool tmpIsSkewConstant = false;

  for(i=1; i<r->N; i++)
  {
    for(j=i+1; j<=r->N; j++)
    {
      if ( MATELEM(r->GetNC()->D,i,j) == NULL ) /* quasicommutative case */
      {
        /* 1x1 mult.matrix */
        r->GetNC()->MTsize[UPMATELEM(i,j,r->N)] = 1;
        r->GetNC()->MT[UPMATELEM(i,j,r->N)] = mpNew(1,1);
      }
      else /* pure noncommutative case */
      {
        /* TODO check the special multiplication properties */
        IsNonComm = 1;
        p_Delete(&(MATELEM(COM,i,j)),r);
        //MATELEM(COM,i,j) = NULL; // done by p_Delete
        r->GetNC()->MTsize[UPMATELEM(i,j,r->N)] = DefMTsize; /* default sizes */
        r->GetNC()->MT[UPMATELEM(i,j,r->N)] = mpNew(DefMTsize, DefMTsize);
      }
      /* set MT[i,j,1,1] to c_i_j*x_i*x_j + D_i_j */
      p = p_One(r);
      if (MATELEM(r->GetNC()->C,i,j)!=NULL)
        p_SetCoeff(p,n_Copy(pGetCoeff(MATELEM(r->GetNC()->C,i,j)),r->cf),r);
      p_SetExp(p,i,1,r);
      p_SetExp(p,j,1,r);
      p_Setm(p,r);
      p_Test(MATELEM(r->GetNC()->D,i,j),r);
      q =  nc_p_CopyGet(MATELEM(r->GetNC()->D,i,j),r);
      p = p_Add_q(p,q,r);
      MATELEM(r->GetNC()->MT[UPMATELEM(i,j,r->N)],1,1) = nc_p_CopyPut(p,r);
      p_Delete(&p,r);
      // p = NULL;// done by p_Delete
    }
  }
  if (ncRingType(r)==nc_undef)
  {
    if (IsNonComm==1)
    {
      //      assume(pN!=NULL);
      //      if ((tmpIsSkewConstant==1) && (nIsOne(pGetCoeff(pN)))) r->GetNC()->type=nc_lie;
      //      else r->GetNC()->type=nc_general;
    }
    if (IsNonComm==0)
    {
      ncRingType(r, nc_skew); // TODO: check whether it is commutative
      r->GetNC()->IsSkewConstant = 0; // true; //tmpIsSkewConstant; //  BUG???
    } else
       assume( FALSE );
  }
  r->GetNC()->COM=COM;

  nc_p_ProcsSet(r, r->p_Procs);

  if(bSetupQuotient) // Test me!!!
    nc_SetupQuotient(r, NULL, false); // no copy!


//  if (save != currRing)
//    rChangeCurrRing(save);

  return FALSE;
}


// set pProcs for r and global variable p_Procs as for general non-commutative algebras.
static inline
void gnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{
  // "commutative"
  p_Procs->p_Mult_mm  = rGR->p_Procs->p_Mult_mm  = gnc_p_Mult_mm;
  p_Procs->pp_Mult_mm = rGR->p_Procs->pp_Mult_mm = gnc_pp_Mult_mm;
  p_Procs->p_Minus_mm_Mult_qq = rGR->p_Procs->p_Minus_mm_Mult_qq = nc_p_Minus_mm_Mult_qq;

  // non-commutaitve multiplication by monomial from the left
  p_Procs->p_mm_Mult   = gnc_p_mm_Mult;
  p_Procs->pp_mm_Mult  = gnc_pp_mm_Mult;

#if 0
  // Previous Plural's implementation...
  rGR->GetNC()->p_Procs.SPoly       = gnc_CreateSpolyOld;
  rGR->GetNC()->p_Procs.ReduceSPoly = gnc_ReduceSpolyOld;

  rGR->GetNC()->p_Procs.BucketPolyRed  = gnc_kBucketPolyRedOld;
  rGR->GetNC()->p_Procs.BucketPolyRed_Z= gnc_kBucketPolyRed_ZOld;
#else
  // A bit cleaned up and somewhat rewritten functions...
  rGR->GetNC()->p_Procs.SPoly       = gnc_CreateSpolyNew;
  rGR->GetNC()->p_Procs.ReduceSPoly = gnc_ReduceSpolyNew;

  rGR->GetNC()->p_Procs.BucketPolyRed_NF= gnc_kBucketPolyRedNew;
  rGR->GetNC()->p_Procs.BucketPolyRed_Z= gnc_kBucketPolyRed_ZNew;
#endif

  // warning: ISO C++ forbids casting between pointer-to-function and pointer-to-object?
  if (rHasLocalOrMixedOrdering(rGR))
    rGR->GetNC()->p_Procs.GB = cast_A_to_vptr(gnc_gr_mora);
  else
    rGR->GetNC()->p_Procs.GB = cast_A_to_vptr(gnc_gr_bba);

///////////  rGR->GetNC()->p_Procs.GB          = gnc_gr_bba; // bba even for local case!
// old ///    r->GetNC()->GB()            = gnc_gr_bba;
//   rGR->GetNC()->p_Procs.GlobalGB    = gnc_gr_bba;
//   rGR->GetNC()->p_Procs.LocalGB     = gnc_gr_mora;
//  const ring save = currRing; if( save != r ) rChangeCurrRing(r);
//  ideal res = gnc_gr_bba(F, Q, w, hilb, strat/*, r*/);
//  if( save != r )     rChangeCurrRing(save);     return (res);


#if 0
    // Old Stuff
    p_Procs->p_Mult_mm   = gnc_p_Mult_mm;
    _p_procs->p_Mult_mm  = gnc_p_Mult_mm;

    p_Procs->pp_Mult_mm  = gnc_pp_Mult_mm;
    _p_procs->pp_Mult_mm = gnc_pp_Mult_mm;

    p_Procs->p_Minus_mm_Mult_qq = NULL; // gnc_p_Minus_mm_Mult_qq_ign;
    _p_procs->p_Minus_mm_Mult_qq= NULL; // gnc_p_Minus_mm_Mult_qq_ign;

    r->GetNC()->mmMultP()       = gnc_mm_Mult_p;
    r->GetNC()->mmMultPP()      = gnc_mm_Mult_pp;

    r->GetNC()->SPoly()         = gnc_CreateSpoly;
    r->GetNC()->ReduceSPoly()   = gnc_ReduceSpoly;

#endif
}


// set pProcs table for rGR and global variable p_Procs
void nc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{
  assume(rIsPluralRing(rGR));
  assume(p_Procs!=NULL);

  gnc_p_ProcsSet(rGR, p_Procs);

  if(rIsSCA(rGR) && ncExtensions(SCAMASK) )
  {
    sca_p_ProcsSet(rGR, p_Procs);
  }

  if( ncExtensions(NOPLURALMASK) )
    ncInitSpecialPairMultiplication(rGR);

  if(!rIsSCA(rGR) && !ncExtensions(NOFORMULAMASK))
    ncInitSpecialPowersMultiplication(rGR);

}


/// substitute the n-th variable by e in p
/// destroy p
/// e is not a constant
poly nc_pSubst(poly p, int n, poly e, const ring r)
{
  int rN = r->N;
  int *PRE = (int *)omAlloc0((rN+1)*sizeof(int));
  int *SUF = (int *)omAlloc0((rN+1)*sizeof(int));
  int i,pow;
  number C;
  poly suf,pre;
  poly res = NULL;
  poly out = NULL;
  while ( p!= NULL )
  {
    C =  p_GetCoeff(p, r);
    p_GetExpV(p, PRE, r); /* faster splitting? */
    pow = PRE[n]; PRE[n]=0;
    res = NULL;
    if (pow!=0)
    {
      for (i=n+1; i<=rN; i++)
      {
         SUF[i] = PRE[i];
         PRE[i] = 0;
      }
      res =  p_Power(p_Copy(e, r),pow, r);
      /* multiply with prefix */
      pre = p_One(r);
      p_SetExpV(pre,PRE, r);
      p_Setm(pre, r);
      res = nc_mm_Mult_p(pre,res, r);
      /* multiply with suffix */
      suf = p_One(r);
      p_SetExpV(suf,SUF, r);
      p_Setm(suf, r);
      res = p_Mult_mm(res,suf, r);
      res = __p_Mult_nn(res,C, r);
      p_SetComp(res,PRE[0], r);
    }
    else /* pow==0 */
    {
      res = p_Head(p, r);
    }
    p   = p_LmDeleteAndNext(p, r);
    out = p_Add_q(out,res, r);
  }
  freeT(PRE,rN);
  freeT(SUF,rN);
  return(out);
}


// creates a commutative nc extension; "converts" comm.ring to a Plural ring
ring nc_rCreateNCcomm(ring r)
{
  if (rIsPluralRing(r)) return r;

  ring rr = rCopy(r);

  matrix C = mpNew(rr->N,rr->N); // ring-independent!?!
  matrix D = mpNew(rr->N,rr->N);

  for(int i=1; i<rr->N; i++)
    for(int j=i+1; j<=rr->N; j++)
      MATELEM(C,i,j) = p_One(rr);

  if (nc_CallPlural(C, D, NULL, NULL, rr, false, true, false, rr, TRUE)) // TODO: what about quotient ideal?
    WarnS("Error initializing multiplication!"); // No reaction!???

  return rr;
}

  /* NOT USED ANYMORE: replaced by maFindPerm in ring.cc */
  /* for use with embeddings: currRing is a sum of smaller rings */
  /* and srcRing is one of such smaller rings */
  /* shift defines the position of a subring in srcRing */
  /* par_shift defines the position of a subfield in basefield of CurrRing */
poly p_CopyEmbed(poly p, ring srcRing, int shift, int /*par_shift*/, ring dstRing)
{
  if (dstRing == srcRing)
  {
    return(p_Copy(p,dstRing));
  }
  nMapFunc nMap=n_SetMap(srcRing->cf, dstRing->cf);
  poly q;
  //  if ( nMap == nCopy)
  //  {
  //    q = prCopyR(p,srcRing);
  //  }
  //  else
  {
    int *perm = (int *)omAlloc0((rVar(srcRing)+1)*sizeof(int));
    int *par_perm = (int *)omAlloc0((rPar(srcRing)+1)*sizeof(int));
    //    int *par_perm = (int *)omAlloc0((rPar(srcRing)+1)*sizeof(int));
    int i;
    //    if (srcRing->P > 0)
    //    {
    //      for (i=0; i<srcRing->P; i++)
    //  par_perm[i]=-i;
    //    }
    if ((shift<0) || (shift > rVar(srcRing))) // ???
    {
      WerrorS("bad shifts in p_CopyEmbed");
      return(0);
    }
    for (i=1; i<= srcRing->N; i++)
    {
      perm[i] = shift+i;
    }
    q = p_PermPoly(p,perm,srcRing, dstRing, nMap,par_perm, rPar(srcRing));
  }
  return(q);
}

BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate)
{
  /* the same basefield */
  int diagnose = TRUE;
  nMapFunc nMap = n_SetMap(rCandidate->cf, rBase->cf); // reverse?

//////  if (nMap != nCopy) diagnose = FALSE;
  if (nMap == NULL) diagnose = FALSE;


  /* same number of variables */
  if (rBase->N != rCandidate->N) diagnose = FALSE;
  /* nc and comm ring */
  if ( rIsPluralRing(rBase) != rIsPluralRing(rCandidate) ) diagnose = FALSE;
  /* both are qrings */
  /* NO CHECK, since it is used in building opposite qring */
  /*  if ( ((rBase->qideal != NULL) && (rCandidate->qideal == NULL)) */
  /*       || ((rBase->qideal == NULL) && (rCandidate->qideal != NULL)) ) */
  /*  diagnose = FALSE; */
  /* TODO: varnames are e->E etc */
  return diagnose;
}




/// opposes a vector p from Rop to currRing (dst!)
poly pOppose(ring Rop, poly p, const ring dst)
{
  /* the simplest case:*/
  if (  Rop == dst )  return(p_Copy(p, dst));
  /* check Rop == rOpposite(currRing) */


  if ( !rIsLikeOpposite(dst, Rop) )
  {
    WarnS("an opposite ring should be used");
    return NULL;
  }

  nMapFunc nMap = n_SetMap(Rop->cf, dst->cf); // reverse?

  /* nMapFunc nMap = nSetMap(Rop);*/
  /* since we know that basefields coinside! */

  // coinside???

  int *perm=(int *)omAlloc0((Rop->N+1)*sizeof(int));
  if (!p_IsConstantPoly(p, Rop))
  {
    /* we know perm exactly */
    int i;
    for(i=1; i<=Rop->N; i++)
    {
      perm[i] = Rop->N+1-i;
    }
  }
  poly res = p_PermPoly(p, perm, Rop, dst, nMap);
  omFreeSize((ADDRESS)perm,(Rop->N+1)*sizeof(int));

  p_Test(res, dst);

  return res;
}

/// opposes a module I from Rop to currRing(dst)
ideal idOppose(ring Rop, ideal I, const ring dst)
{
  /* the simplest case:*/
  if ( Rop == dst ) return id_Copy(I, dst);

  /* check Rop == rOpposite(currRing) */
  if (!rIsLikeOpposite(dst, Rop))
  {
    WarnS("an opposite ring should be used");
    return NULL;
  }
  int i;
  ideal idOp = idInit(I->ncols, I->rank);
  for (i=0; i< (I->ncols)*(I->nrows); i++)
  {
    idOp->m[i] = pOppose(Rop,I->m[i], dst);
  }
  id_Test(idOp, dst);
  return idOp;
}


bool nc_SetupQuotient(ring rGR, const ring rG, bool bCopy)
{
  if( rGR->qideal == NULL )
    return false; // no quotient = no work! done!? What about factors of SCA?

  bool ret = true;
  // currently only super-commutative extension deals with factors.

  if( ncExtensions(SCAMASK)  )
  {
    bool sca_ret = sca_SetupQuotient(rGR, rG, bCopy);

    if(sca_ret) // yes it was dealt with!
      ret = false;
  }

  if( bCopy )
  {
    assume(rIsPluralRing(rGR) == rIsPluralRing(rG));
    assume((rGR->qideal==NULL) == (rG->qideal==NULL));
    assume(rIsSCA(rGR) == rIsSCA(rG));
    assume(ncRingType(rGR) == ncRingType(rG));
  }

  return ret;
}



// int Commutative_Context(ring r, leftv expression)
//   /* returns 1 if expression consists */
//   /*  of commutative elements */
// {
//   /* crucial: poly -> ideal, module, matrix  */
// }

// int Comm_Context_Poly(ring r, poly p)
// {
//   poly COMM=r->GetNC()->COMM;
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

#endif



