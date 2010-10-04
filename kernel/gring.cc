/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    gring.cc
 *  Purpose: noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *  Version: $Id$
 *******************************************************************/

#define MYTEST 0
#define OUTPUT 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include <kernel/mod2.h>

#ifdef HAVE_PLURAL
#define PLURAL_INTERNAL_DECLARATIONS

#include <kernel/febase.h>
#include <kernel/options.h>
#include <kernel/ring.h>
#include <kernel/polys.h>
#include <kernel/numbers.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <kernel/kbuckets.h>
#include <kernel/kstd1.h>
#include <kernel/sbuckets.h>
#include <kernel/prCopy.h>
#include <kernel/p_Mult_q.h>
#include <kernel/pInline1.h>

// dirty tricks:
#include <kernel/p_MemAdd.h>

#include <kernel/gring.h>
#include <kernel/sca.h>
#include <summator.h>

#include <kernel/ncSAMult.h> // for CMultiplier etc classes
#include <kernel/ncSAFormula.h> // for CFormulaPowerMultiplier and enum Enum_ncSAType

#ifdef HAVE_RATGRING
#include <kernel/ratgring.h>
#endif



int  iNCExtensions = 0x00001; // only SCA can be used by default


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




static const bool bNoPluralMultiplication = false;  // use only formula shortcuts in my OOP Multiplier

// the following make sense only if bNoPluralMultiplication is false:
static const bool bNoFormula = true;  // don't use any formula shortcuts
static const bool bNoCache   = false; // only formula whenever possible, only make sanse if bNoFormula is false!


// false, true, false == old "good" Plural
// false, false ==>> Plural + Cache + Direct Formula - not much
// false, false, true ==>> Plural Mult + Direct Formula (no ~cache)
// true, *, *  == new OOP multiplication!


/* global nc_macros : */

#define freeT(A,v) omFreeSize((ADDRESS)A,(v+1)*sizeof(int))
#define freeN(A,k) omFreeSize((ADDRESS)A,k*sizeof(number))


// some forward declarations:


// polynomial multiplication functions for p_Procs :
poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly &last);
poly gnc_p_Mult_mm(poly p, const poly m, const ring r);
poly gnc_mm_Mult_p(const poly m, poly p, const ring r);
poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r);


// set pProcs for r and global variable p_Procs as for general non-commutative algebras.
void gnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs);

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

ideal gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy); // Not yet!
ideal gnc_gr_bba (const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat);


void nc_CleanUp(nc_struct* p); // just free memory!
void nc_rCleanUp(ring r); // smaller than kill: just free mem


#if 0
// deprecated functions:
//  poly gnc_p_Minus_mm_Mult_qq_ign(poly p, const poly m, poly q, int & d1, poly d2, const ring ri, poly &d3);
//  poly gnc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r);
//  poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp, int lq, const ring r);
//  poly nc_p_Plus_mm_Mult_qq (poly p, const poly m, const poly q, int &lp, int lq, const ring r);
#endif



/*2
* returns the LCM of the head terms of a and b
* without coefficient!!!
*/
poly p_Lcm(const poly a, const poly b, const long lCompM, const ring r)
{
  poly m = // p_One( r);
          p_Init(r);

  const int pVariables = r->N;

  for (int i = pVariables; i!=0; i--)
  {
    const int lExpA = p_GetExp (a, i, r);
    const int lExpB = p_GetExp (b, i, r);

    p_SetExp (m, i, si_max(lExpA, lExpB), r);
  }

  p_SetComp (m, lCompM, r);

  p_Setm(m,r);

#ifdef PDEBUG
//  p_Test(m,r);
#endif

  n_New(&(p_GetCoeff(m, r)), r);

  return(m);
};

poly p_Lcm(const poly a, const poly b, const ring r)
{
#ifdef PDEBUG
  p_Test(a, r);
  p_Test(b, r);
#endif

  const long lCompP1 = p_GetComp(a, r);
  const long lCompP2 = p_GetComp(b, r);

  const poly m = p_Lcm(a, b, si_max(lCompP1, lCompP2), r);

#ifdef PDEBUG
//  p_Test(m,r);
#endif
  return(m);
};



///////////////////////////////////////////////////////////////////////////////
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                                    const int, const poly, const ring r)
{
  poly mc  = p_Neg( p_Copy(m, r), r );
  poly mmc = nc_mm_Mult_pp( mc, q, r );
  p_Delete(&mc, r);

  p = p_Add_q(p, mmc, r);

  lp = pLength(p); // ring independent!

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
    res=p_Add_q(res, pp_Mult_mm(pp, qq, r), r); // p_Head(qq, r)?
    qq=p_LmDeleteAndNext(qq,r);
  }
  p_Delete(&pp,r);
  return(res);
}

// return pPolyP * pPolyQ; destroy or reuse pPolyP and pPolyQ
poly _nc_p_Mult_q(poly pPolyP, poly pPolyQ, const ring rRing)
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
        if (side)
        {
          Print("gnc_p_Mult_mm: Multiplication in the left module from the right");
        }
#endif
      }
      else
      {
        /* REPORT_ERROR */
#ifdef PDEBUG
        const char* s;
        if (side==1) s="gnc_p_Mult_mm";
        else s="gnc_mm_Mult_p";
        Print("%s: exponent mismatch %d and %d\n",s,expP,expM);
#endif
        expOut=0;
      }
    }
    p_GetExpV(p,P,r);
    cP=p_GetCoeff(p,r);
    cOut=n_Mult(cP,cM,r);
    if (side==1)
    {
      v = gnc_mm_Mult_nn(P, M, r);
    }
    else
    {
      v = gnc_mm_Mult_nn(M, P, r);
    }
    v = p_Mult_nn(v,cOut,r);
    n_Delete(&cOut,r);
    p_SetCompP(v,expOut,r);

    sum += v;

    p_LmDelete(&p,r);
  }
  freeT(P,rN);
  freeT(M,rN);

  return(sum);
}

/* poly functions defined in p_Procs : */
poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly &last)
{
  return( gnc_p_Mult_mm_Common(p_Copy(p,r), m, 1, r) );
}

poly gnc_p_Mult_mm(poly p, const poly m, const ring r)
{
  return( gnc_p_Mult_mm_Common(p, m, 1, r) );
}

poly gnc_mm_Mult_p(const poly m, poly p, const ring r)
{
  return( gnc_p_Mult_mm_Common(p, m, 0, r) );
}

poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r)
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
      cff = n_Copy(p_GetCoeff(MATELEM(r->GetNC()->COM,1,2),r),r);
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
              cpower = F[i]*G[j]; // bug! overflow danger!!!
              cff = n_Copy(p_GetCoeff(MATELEM(r->GetNC()->COM,j,i),r),r);
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
    /*    if (ri->GetNC()->type==nc_skew) -- postpone to TU */
    out = gnc_mm_Mult_uu(F,jG,G[jG],r);
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

     w=gnc_mm_Mult_uu(Op,t,On[t],r);
     c[cnt]=n_Mult(c[cnt-1],p_GetCoeff(w,r),r);
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
    out=gnc_mm_Mult_p(Rout,out,r); /* getting the final result */
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
    poly Rout=pOne();
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

       out=gnc_mm_Mult_p(Rout,out,r); /* getting the final result */
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

     poly w=gnc_uu_Mult_ww(t,Op[t],jG,bG,r);
     c[cnt]=n_Copy(p_GetCoeff(w,r),r);
     D = pNext(w);  /* getting coef and rest D */
     p_LmDelete(&w,r);
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
         w=gnc_p_Mult_mm(D,Pn,r);
         Rout=gnc_mm_Mult_p(Pp,w,r);
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
       Rout=gnc_mm_Mult_p(Pp,D,r);
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

  freeT(U, rN);
  freeN(c, i);
  freeT(lF, rN);

  if (cnf!=0)
  {
    Rout=pOne();
    p_SetExpV(Rout,Prv,r);
    p_Setm(Rout,r);
    freeT(Prv, rN);
    out=gnc_mm_Mult_p(Rout,out,r); /* getting the final result */
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
     t = MATELEM(cMT,k,1);

     if (t==NULL)   /* not computed yet */
     {
       t = nc_p_CopyGet(MATELEM(cMT,k-1,1),r);
       //        t=p_Copy(MATELEM(cMT,k-1,1),r);
       t = gnc_mm_Mult_p(y,t,r);
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
  if(bNoFormula)
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
  poly out=pOne();
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
    if (r->cf->nIsOne(p_GetCoeff(MATELEM(r->GetNC()->COM,j,i),r))) /* commutative case */
    {
      return(out);
    }
    else
    {
      number tmp_number=p_GetCoeff(MATELEM(r->GetNC()->COM,j,i),r); /* quasicommutative case */
      nPower(tmp_number,a*b,&tmp_number); // BUG! ;-(
      p_SetCoeff(out,tmp_number,r);
      return(out);
    }
  }/* end_of commutative or quasicommutative case */
  p_Delete(&out,r);


  if(bNoCache && !bNoFormula) // don't use cache whenever possible!
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
        t = gnc_mm_Mult_p(y,t,r);
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
        t = gnc_mm_Mult_p(y,t,r);
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
          t = gnc_mm_Mult_p(y,t,r);
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
  poly out = nc_mm_Mult_pp(m, pNext(p1), r);
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
  if ( out!=NULL ) p_Content(out,r);
  p_Delete(&m,r);
  n_Delete(&cF,r);
  n_Delete(&C,r);
  n_Delete(&MinusOne,r);
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
    Werror("gnc_ReduceSpolyNew: different non-zero components!");
#endif
    return(NULL);
  }

  poly m = pOne();
  p_ExpVectorDiff(m, p2, p1, r);
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test(m,r);
#endif

  /* pSetComp(m,r)=0? */
  poly   N  = nc_mm_Mult_p(m, p_Head(p1,r), r);

  number C  = n_Copy( p_GetCoeff(N,  r), r);
  number cF = n_Copy( p_GetCoeff(p2, r), r);

  /* GCD stuff */
  number cG = nGcd(C, cF, r);

  if (!n_IsOne(cG, r))
  {
    number n_tmp;
    n_tmp = n_Div(cF, cG, r); n_Delete(&cF,r); cF=n_tmp;
    n_tmp  = n_Div(C,  cG, r); n_Delete(&C,r); C=n_tmp;
  }

  p2 = p_Mult_nn(p2, C, r); // p2 !!!
  p_Test(p2,r);
  n_Delete(&C,r);

  poly out = nc_mm_Mult_pp(m, pNext(p1), r);
  p_Delete(&m,r);

  N = p_Add_q(N, out, r);
  p_Test(N,r);

  if (!n_IsMOne(cF,r)) // ???
  {
    cF = n_Neg(cF,r);
    N  = p_Mult_nn(N, cF, r);
    p_Test(N,r);
  }
  n_Delete(&cF,r);

  out = p_Add_q(p2,N,r); // delete N, p2
  p_Test(out,r);
  if ( out!=NULL ) p_Content(out,r);
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
  if ((ncRingType(r)==nc_lie) && pHasNotCF(p1,p2)) /* prod crit */
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
  if (M2!=NULL) M2=p_Cleardenom(M2,r);
  //if (M2!=NULL) p_Content(M2); // done by pCleardenom
  return(M2);
}

poly gnc_CreateSpolyNew(poly p1, poly p2/*,poly spNoether*/, const ring r)
{
  assume(r == currRing);

#ifdef PDEBUG
  pTest(p1);
  pTest(p2);
#if MYTEST
  Print("p1: "); pWrite(p1);
  Print("p2: "); pWrite(p2);
#endif
#endif

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    Werror("gnc_CreateSpolyNew: different non-zero components!");
#endif
    return(NULL);
  }

#ifdef PDEBUG
  if (lCompP1!=lCompP2)
  {
    WarnS("gnc_CreateSpolyNew: vector & poly in SPoly!");
  }
#endif


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
  Print("m1: "); pWrite(m1);
  Print("m2: "); pWrite(m2);
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

  p_Delete(&pL,r);

  /* zero exponents !? */
  poly M1    = nc_mm_Mult_p(m1,p_Head(p1,r),r); // M1 = m1 * lt(p1)
  poly M2    = nc_mm_Mult_p(m2,p_Head(p2,r),r); // M2 = m2 * lt(p2)

#ifdef PDEBUG
  p_Test(M1,r);
  p_Test(M2,r);

#if MYTEST
  Print("M1: "); pWrite(M1);
  Print("M2: "); pWrite(M2);
#endif
#endif

  if(M1 == NULL || M2 == NULL)
  {
#ifdef PDEBUG
       Print("\np1 = ");
       p_Write(p1, r);

       Print("m1 = ");
       p_Write(m1, r);

       Print("p2 = ");
       p_Write(p2, r);

       Print("m2 = ");
       p_Write(m2, r);

       Werror("ERROR in nc_CreateSpoly: result of multiplication is Zero!\n");
#endif
       return(NULL);
  }

  number C1  = n_Copy(p_GetCoeff(M1,r),r);      // C1 = lc(M1)
  number C2  = n_Copy(p_GetCoeff(M2,r),r);      // C2 = lc(M2)

  /* GCD stuff */
  number C = nGcd(C1, C2, r);                     // C = gcd(C1, C2)

  if (!n_IsOne(C, r))                              // if C != 1
  {
    C1=n_Div(C1, C, r);                              // C1 = C1 / C
    C2=n_Div(C2, C, r);                              // C2 = C2 / C
  }

  n_Delete(&C,r); // destroy the number C

  C1=n_Neg(C1,r);

//   number MinusOne=n_Init(-1,r);
//   if (n_Equal(C1,MinusOne,r))                   // lc(M1) / gcd( lc(M1), lc(M2)) == -1 ????
//   {
//     M2=p_Add_q(M1,M2,r);                        // ?????
//   }
//   else
//   {
  M1=p_Mult_nn(M1,C2,r);                           // M1 = (C2*lc(p1)) * (lcm(lm(p1),lm(p2)) / lm(p1)) * lm(p1)

#ifdef PDEBUG
  p_Test(M1,r);
#endif

  M2=p_Mult_nn(M2,C1,r);                           // M2 =(-C1*lc(p2)) * (lcm(lm(p1),lm(p2)) / lm(p2)) * lm(p2)



#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  Print("M1: "); pWrite(M1);
  Print("M2: "); pWrite(M2);
#endif
#endif


  M2=p_Add_q(M1,M2,r);                             // M1 is killed, M2 = spoly(lt(p1), lt(p2)) = C2*M1 - C1*M2

#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  Print("M2: "); pWrite(M2);
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
  Print("M1: "); pWrite(M1);
#endif

#endif

  M2=p_Add_q(M2,M1,r);                           // M2 = spoly(lt(p1), lt(p2)) + m1 * tail(p1), delete M1
#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  Print("M2: "); pWrite(M2);
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
  Print("M1: "); pWrite(M1);
#endif

#endif

  M2 = p_Add_q(M2,M1,r);                           // M2 = spoly(lt(p1), lt(p2)) + m1 * tail(p1) + m2*tail(p2)

#ifdef PDEBUG
  p_Test(M2,r);

#if MYTEST
  Print("M2: "); pWrite(M2);
#endif

#endif
                                                 // delete M1

  p_Delete(&m1,r);  //  => n_Delete(&C1,r);
  p_Delete(&m2,r);  //  => n_Delete(&C2,r);

#ifdef PDEBUG
  p_Test(M2,r);
#endif

  if (M2!=NULL) p_Cleardenom(M2,r);
//  if (M2!=NULL) p_Content(M2);

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
  poly m=pOne();
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
    Werror("nc_CreateShortSpoly: wrong module components!"); // !!!!
#endif
    return(NULL);
  }

  poly m;
  if ( ! rIsRatGRing(currRing))
  {
    m = p_Lcm(p1, p2, si_max(lCompP1, lCompP2), r);
  }
#ifdef HAVE_RATGRING
  else
  {
    /* rational version */
    m = p_LcmRat(p1, p2, si_max(lCompP1, lCompP2), r);
  }
#endif

//  n_Delete(&p_GetCoeff(m, r), r);
//  pSetCoeff0(m, NULL);

#ifdef PDEBUG
//  p_Test(m,r);
#endif

  return(m);
}

void gnc_kBucketPolyRedOld(kBucket_pt b, poly p, number *c)
{
  // b will not be multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=nInit(1);
  poly m=pOne();
  pExpVectorDiff(m,kBucketGetLm(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif
  poly pp= nc_mm_Mult_pp(m,p,currRing);
  assume(pp!=NULL);
  pDelete(&m);
  number n=pGetCoeff(pp);
  number nn;
  if (!n_IsMOne(n,currRing))
  {
    nn=nNeg(nInvers(n));
    n=nMult(nn,pGetCoeff(kBucketGetLm(b)));
    nDelete(&nn);
    pp=p_Mult_nn(pp,n,currRing);
    nDelete(&n);
  }
  else
  {
    pp=p_Mult_nn(pp,pGetCoeff(kBucketGetLm(b)),currRing);
  }
  int l=pLength(pp);
  kBucket_Add_q(b,pp,&l);
}

void gnc_kBucketPolyRedNew(kBucket_pt b, poly p, number *c)
{
#ifdef PDEBUG
//   Print(">*");
#endif

#ifdef KDEBUG
  if( !kbTest(b) )Werror("nc_kBucketPolyRed: broken bucket!");
#endif

#ifdef PDEBUG
  pTest(p);
#if MYTEST
  Print("p: "); pWrite(p);
#endif
#endif

  // b will not be multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=nInit(1);
  poly m = pOne();
  const poly pLmB = kBucketGetLm(b); // no new copy!

  assume( pLmB != NULL );

#ifdef PDEBUG
  pTest(pLmB);

#if MYTEST
  Print("pLmB: "); pWrite(pLmB);
#endif
#endif

  pExpVectorDiff(m, pLmB, p);
  //pSetm(m);

#ifdef PDEBUG
  pTest(m);
#if MYTEST
  Print("m: "); pWrite(m);
#endif
#endif

  poly pp = nc_mm_Mult_pp(m, p, currRing);
  pDelete(&m);

  assume( pp != NULL );
  const number n = pGetCoeff(pp); // bug!

  if (!n_IsMOne(n,currRing) ) // does this improve performance??!? also see below... // TODO: check later on.
  // if n == -1 => nn = 1 and -1/n
  {
    number nn=nNeg(nInvers(n));
    number t = nMult(nn,pGetCoeff(pLmB));
    nDelete(&nn);
    pp = p_Mult_nn(pp,t,currRing);
    nDelete(&t);
  }
  else
  {
    pp = p_Mult_nn(pp,pGetCoeff(pLmB),currRing);
  }

  int l = pLength(pp);

#ifdef PDEBUG
  pTest(pp);
//   Print("PP: "); pWrite(pp);
#endif

  kBucket_Add_q(b,pp,&l);


#ifdef PDEBUG
//   Print("*>");
#endif
}


void gnc_kBucketPolyRed_ZOld(kBucket_pt b, poly p, number *c)
{
  // b is multiplied by a constant in this impl.
  number ctmp;
  poly m=pOne();
  pExpVectorDiff(m,kBucketGetLm(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif
  if(p_IsConstant(m,currRing))
  {
    pDelete(&m);
    ctmp = kBucketPolyRed(b,p,pLength(p),NULL);
  }
  else
  {
    poly pp = nc_mm_Mult_pp(m,p,currRing);
    number c2,cc;
    p_Cleardenom_n(pp,currRing,c2);
    pDelete(&m);
    ctmp = kBucketPolyRed(b,pp,pLength(pp),NULL);
    //cc=*c;
    //*c=nMult(*c,c2);
    nDelete(&c2);
    //nDelete(&cc);
    pDelete(&pp);
  }
  if (c!=NULL) *c=ctmp;
  else nDelete(&ctmp);
}

void gnc_kBucketPolyRed_ZNew(kBucket_pt b, poly p, number *c)
{
  // b is multiplied by a constant in this impl.
  number ctmp;
  poly m=pOne();
  pExpVectorDiff(m,kBucketGetLm(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif

  if(p_IsConstant(m,currRing))
  {
    pDelete(&m);
    ctmp = kBucketPolyRed(b,p,pLength(p),NULL);
  }
  else
  {
    poly pp = nc_mm_Mult_pp(m,p,currRing);
    number c2,cc;
    p_Cleardenom_n(pp,currRing,c2);
    pDelete(&m);
    ctmp = kBucketPolyRed(b,pp,pLength(pp),NULL);
    //cc=*c;
    //*c=nMult(*c,c2);
    nDelete(&c2);
    //nDelete(&cc);
    pDelete(&pp);
  }
  if (c!=NULL) *c=ctmp;
  else nDelete(&ctmp);
}


inline void nc_PolyPolyRedOld(poly &b, poly p, number *c)
  // reduces b with p, do not delete both
{
  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=nInit(1);
  poly m=pOne();
  pExpVectorDiff(m,pHead(b),p);
  //pSetm(m);
#ifdef PDEBUG
  pTest(m);
#endif
  poly pp=nc_mm_Mult_pp(m,p,currRing);
  assume(pp!=NULL);

  pDelete(&m);
  number n=pGetCoeff(pp);
  number nn;
  if (!nIsMOne(n))
  {
    nn=nNeg(nInvers(n));
    n=nMult(nn,pGetCoeff(b));
    nDelete(&nn);
    pp=p_Mult_nn(pp,n,currRing);
    nDelete(&n);
  }
  else
  {
    pp=p_Mult_nn(pp,pGetCoeff(b),currRing);
  }
  b=p_Add_q(b,pp,currRing);
}


inline void nc_PolyPolyRedNew(poly &b, poly p, number *c)
  // reduces b with p, do not delete both
{
#ifdef PDEBUG
  pTest(b);
  pTest(p);
#endif

#if MYTEST
  PrintS("nc_PolyPolyRedNew(");
  pWrite0(b);
  PrintS(", ");
  pWrite0(p);
  PrintS(", *c): ");
#endif

  // b will not by multiplied by any constant in this impl.
  // ==> *c=1
  if (c!=NULL) *c=nInit(1);

  poly pp = NULL;

  // there is a problem when p is a square(=>0!)

  while((b != NULL) && (pp == NULL))
  {

//    poly pLmB = pHead(b);
    poly m = pOne();
    pExpVectorDiff(m, b, p);
//    pDelete(&pLmB);
  //pSetm(m);

#ifdef PDEBUG
    pTest(m);
    pTest(b);
#endif

    pp = nc_mm_Mult_pp(m, p, currRing);

#if MYTEST
    PrintS("\n{b': ");
    pWrite0(b);
    PrintS(", m: ");
    pWrite0(m);
    PrintS(", pp: ");
    pWrite0(pp);
    PrintS(" }\n");
#endif

    pDelete(&m); // one m for all tries!

//    assume( pp != NULL );

    if( pp == NULL )
    {
      b = p_LmDeleteAndNext(b, currRing);

      if( !p_DivisibleBy(p, b, currRing) )
        return;

    }
  }

#if MYTEST
  PrintS("{b': ");
  pWrite0(b);
  PrintS(", pp: ");
  pWrite0(pp);
  PrintS(" }\n");
#endif


  if(b == NULL) return;


  assume(pp != NULL);

  const number n = pGetCoeff(pp); // no new copy

  number nn;

  if (!n_IsMOne(n, currRing)) // TODO: as above.
  {
    nn=nNeg(nInvers(n));
    number t = nMult(nn, pGetCoeff(b));
    nDelete(&nn);
    pp=p_Mult_nn(pp, t, currRing);
    nDelete(&t);
  }
  else
  {
    pp=p_Mult_nn(pp, pGetCoeff(b), currRing);
  }


  b=p_Add_q(b,pp,currRing);

}

void nc_PolyPolyRed(poly &b, poly p, number *c)
{
#if 0
  nc_PolyPolyRedOld(b, p, c);
#else
  nc_PolyPolyRedNew(b, p, c);
#endif
}


poly nc_mm_Bracket_nn(poly m1, poly m2);

poly nc_p_Bracket_qq(poly p, const poly q)
  /* returns [p,q], destroys p */
{
  assume(p != NULL && q!= NULL);

  if (!rIsPluralRing(currRing)) return(NULL);
  if (pComparePolys(p,q)) return(NULL);
  /* Components !? */
  poly Q=NULL;
  number coef=NULL;
  poly res=NULL;
  poly pres=NULL;
  int UseBuckets=1;
  if ((pLength(p)< MIN_LENGTH_BUCKET/2) && (pLength(q)< MIN_LENGTH_BUCKET/2) || TEST_OPT_NOT_BUCKETS) UseBuckets=0;


  CPolynomialSummator sum(currRing, UseBuckets == 0);

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

        sum += pres;
        nDelete(&coef);
      }
      pIter(Q);
    }
    p=pLmDeleteAndNext(p);
  }
  return(sum);
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
          if ( (i==j) || ((MATELEM(currRing->GetNC()->COM,nMin,nMax)!=NULL) && nIsOne(pGetCoeff(MATELEM(currRing->GetNC()->C,nMin,nMax))) )) /* not (the same exp. or commuting exps)*/
          { bres=NULL; }
          else
          {
            if (i<j) { bres=gnc_uu_Mult_ww(j,M1[j],i,M2[i],currRing); }
            else bres=gnc_uu_Mult_ww(i,M2[i],j,M1[j],currRing);
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
            if (!pLmIsConstant(prefix)) bres = gnc_mm_Mult_p(prefix, bres,currRing);
            if (!pLmIsConstant(suffix)) bres = gnc_p_Mult_mm(bres, suffix,currRing);
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
        if (!pLmIsConstant(prefix)) bres = gnc_mm_Mult_p(prefix, bres,currRing);
        if (!pLmIsConstant(suffix)) bres = gnc_p_Mult_mm(bres, suffix,currRing);
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

ideal twostd(ideal I) // works in currRing only!
{
  ideal J = kStd(I, currQuotient, testHomog, NULL, NULL, 0, 0, NULL); // in currRing!!!
  idSkipZeroes(J); // ring independent!

  const int rN = currRing->N;

  loop
  {
    ideal     K    = NULL;
    const int s    = idElem(J); // ring independent

    for(int i = 0; i < s; i++)
    {
      const poly p = J->m[i];

#ifdef PDEBUG
      p_Test(p, currRing);
#if 0
      Print("p: "); // !
      p_Write(p, currRing);
#endif
#endif

      for (int j = 1; j <= rN; j++) // for all j = 1..N
      {
        poly varj = p_One( currRing);
        p_SetExp(varj, j, 1, currRing);
        p_Setm(varj, currRing);

        poly q = pp_Mult_mm(p, varj, currRing); // q = J[i] * var(j),

#ifdef PDEBUG
        p_Test(varj, currRing);
        p_Test(p, currRing);
        p_Test(q, currRing);
#if 0
        Print("Reducing p: "); // !
        p_Write(p, currRing);
        Print("With q: "); // !
        p_Write(q, currRing);
#endif
#endif

        p_Delete(&varj, currRing);

        if (q != NULL)
        {
#ifdef PDEBUG
#if 0
          Print("Reducing q[j = %d]: ", j); // !
          p_Write(q, currRing);

          Print("With p:");
          p_Write(p, currRing);

#endif
#endif

          // bug: lm(p) may not divide lm(p * var(i)) in a SCA!
          if( p_LmDivisibleBy(p, q, currRing) )
            q = nc_ReduceSpoly(p, q, currRing);


#ifdef PDEBUG
          p_Test(q, currRing);
#if 0
          Print("reductum q/p: ");
          p_Write(q, currRing);

          // Print("With J!\n");
#endif
#endif

//          if( q != NULL)
          q = kNF(J, currQuotient, q, 0, KSTD_NF_NONORM); // in currRing!!!

#ifdef PDEBUG
          p_Test(q, currRing);
#if 0
          Print("NF(J/currQuotient)=> q: "); // !
          p_Write(q, currRing);
#endif
#endif
          if (q!=NULL)
          {
            if (p_IsConstant(q, currRing)) // => return (1)!
            {
              p_Delete(&q, currRing);
              id_Delete(&J, currRing);

              if (K != NULL)
                id_Delete(&K, currRing);

              ideal Q = idInit(1,1); // ring independent!
              Q->m[0] = p_One(currRing);

              return(Q);
            }

//            flag = false;

            // K += q:

            ideal Q = idInit(1,1); // ring independent
            Q->m[0]=q;

            if( K == NULL )
              K = Q;
            else
            {
              ideal id_tmp = idSimpleAdd(K, Q); // in currRing
              id_Delete(&K, currRing);
              id_Delete(&Q, currRing);
              K = id_tmp; // K += Q
            }
          }


        } // if q != NULL
      } // for all variables

    }

    if (K == NULL) // nothing new: i.e. all elements are two-sided
      return(J);
    /* now we update GrBasis J with K */
    //    iSize=IDELEMS(J);
#ifdef PDEBUG
    idTest(J); // in currRing!
#if 0
    Print("J:");
    idPrint(J);
    PrintLn();
#endif // debug
#endif



#ifdef PDEBUG
    idTest(K); // in currRing!
#if 0
    Print("+K:");
    idPrint(K);
    PrintLn();
#endif // debug
#endif


    int iSize = idElem(J); // ring independent

    // J += K:
    ideal id_tmp = idSimpleAdd(J,K); // in currRing
    id_Delete(&K, currRing); id_Delete(&J, currRing);

#if 1
    BITSET save_test=test;
    test|=Sy_bit(OPT_SB_1); // ring independent
    J = kStd(id_tmp, currQuotient, testHomog, NULL, NULL, 0, iSize); // J = J + K, J - std // in currRing!
    test = save_test;
#else
    J=kStd(id_tmp, currQuotient,testHomog,NULL,NULL,0,0,NULL);
#endif

    id_Delete(&id_tmp, currRing);
    idSkipZeroes(J); // ring independent

#ifdef PDEBUG
    idTest(J); // in currRing!
#if 0
    Print("J:");
    idPrint(J);
    PrintLn();
#endif // debug
#endif
  } // loop
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
  int size=r->GetNC()->MTsize[UPMATELEM(i,j,rN)];
  matrix M = r->GetNC()->MT[UPMATELEM(i,j,rN)];
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


poly nc_p_CopyGet(poly a, const ring r)
/* for use in getting the mult. matrix elements*/
/* ring r must be a currRing! */
/* for consistency, copies a poly from the comm. r->GetNC()->basering */
/* to its image in NC ring */
{
  if (r != currRing)
  {
#ifdef PDEBUF
    Werror("nc_p_CopyGet call not in currRing");
#endif
    return(NULL);
  }
  return(p_Copy(a,r));
}

poly nc_p_CopyPut(poly a, const ring r)
/* for use in defining the mult. matrix elements*/
/* ring r must be a currRing! */
/* for consistency, puts a polynomial from the NC ring */
/* to its presentation in the comm. r->GetNC()->basering */
{
  if (r != currRing)
  {
#ifdef PDEBUF
    Werror("nc_p_CopyGet call not in currRing");
#endif
    return(NULL);
  }

  return(p_Copy(a,r));
}

BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r)
  /* returns TRUE if there were errors */
  /* checks whether product of vars from PolyVar defines */
  /* an admissible subalgebra of r */
  /* r is indeed currRing and assumed to be noncomm. */
{
  ring save = currRing;
  int WeChangeRing = 0;
  if (currRing != r)
  {
    rChangeCurrRing(r);
    WeChangeRing = 1;
  }
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
              if ( WeChangeRing )
                rChangeCurrRing(save);
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
  if ( WeChangeRing )
    rChangeCurrRing(save);
  return(FALSE);
}


BOOLEAN gnc_CheckOrdCondition(matrix D, ring r)
/* returns TRUE if there were errors */
/* checks whether the current ordering */
/* is admissible for r and D == r->GetNC()->D */
/* to be executed in a currRing */
{
  /* analyze D: an upper triangular matrix of polys */
  /* check the ordering condition for D */
  ring save = currRing;
  int WeChangeRing = 0;
  if (r != currRing)
  {
    rChangeCurrRing(r);
    WeChangeRing = 1;
  }
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
    q = p_One(r); // replaces pOne();
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
  if ( WeChangeRing )
    rChangeCurrRing(save);
  return(report);
}


BOOLEAN nc_CallPlural(
                      matrix CCC, matrix DDD,
                      poly CCN, poly DDN,
                      ring r,
                      bool bSetupQuotient, bool bCopyInput, bool bBeQuiet,
                      ring curr, BOOLEAN dummy_ring /*=FALSE*/)
  /* returns TRUE if there were errors */
  /* analyze inputs, check them for consistency */
  /* detects nc_type, DO NOT initialize multiplication but call for it at the end*/
  /* checks the ordering condition and evtl. NDC */
// NOTE: all the data belong to the curr,
// we change r which may be the same ring, and must have the same representation!
{
  assume( r->qideal == NULL ); // The basering must NOT be a qring!

//  assume( curr != r );
  assume( rSamePolyRep(r, curr) );


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

  ring save = currRing;

  if( save != curr )
    rChangeCurrRing(curr);

#if OUTPUT
  if( CCC != NULL )
  {
    PrintS("nc_CallPlural(), Input data, CCC: \n");
    iiWriteMatrix(CCC, "C", 2, 4);
  }
  if( DDD != NULL )
  {
    PrintS("nc_CallPlural(), Input data, DDD: \n");
    iiWriteMatrix(DDD, "D", 2, 4);
  }
#endif


#ifndef NDEBUG
  idTest((ideal)CCC);
  idTest((ideal)DDD);
  pTest(CCN);
  pTest(DDN);
#endif

  if( (!bBeQuiet) && (r->GetNC() != NULL) )
    WarnS("going to redefine the algebra structure");

  if( currRing != r )
    rChangeCurrRing(r);

#ifndef NDEBUG
  idTest((ideal)CCC);
  idTest((ideal)DDD);
  pTest(CCN);
  pTest(DDN);
#endif

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

      if( currRing != save )
        rChangeCurrRing(save);
      return TRUE;
    }
  }
  if (( CCC != NULL) && (CC == NULL)) CC = CCC; // mpCopy(CCC); // bug!?
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

      if( currRing != save )
        rChangeCurrRing(save);
      return TRUE;
    }
  }

  if (( DDD != NULL) && (DD == NULL)) DD = DDD; // mpCopy(DDD); // ???
  if (( DDN != NULL) && (DN == NULL)) DN = DDN;

  // further checks and some analysis:
  // all data in 'curr'!
  if (CN != NULL)       /* create matrix C = CN * Id */
  {
    nN = p_GetCoeff(CN, curr);
    if (n_IsZero(nN, curr))
    {
      Werror("Incorrect input : zero coefficients are not allowed");

      if( currRing != save )
        rChangeCurrRing(save);
      return TRUE;
    }

    if (n_IsOne(nN, curr))
      nctype = nc_lie;
    else
      nctype = nc_general;

    IsSkewConstant = true;

    C = mpNew(r->N,r->N); // ring independent!
    bCnew = true;

    for(i=1; i<r->N; i++)
      for(j=i+1; j<=r->N; j++)
        MATELEM(C,i,j) = prCopyR_NoSort(CN, curr, r); // nc_p_CopyPut(CN, r); // copy CN from curr into r
  } else
  if ( (CN == NULL) && (CC != NULL) ) /* copy matrix C */
  {
    /* analyze C */

    pN = NULL; /* check the consistency later */

    if( r->N > 1 )
      if ( MATELEM(CC,1,2) != NULL )
        pN = p_GetCoeff(MATELEM(CC,1,2), curr);

    tmpIsSkewConstant = true;

    for(i=1; i<r->N; i++)
      for(j=i+1; j<=r->N; j++)
      {
        if (MATELEM(CC,i,j) == NULL)
          qN = NULL;
        else
          qN = p_GetCoeff(MATELEM(CC,i,j),curr);

        if ( qN == NULL )   /* check the consistency: Cij!=0 */
        // find also illegal pN
        {
          Werror("Incorrect input : matrix of coefficients contains zeros in the upper triangle");

        if( currRing != save )
            rChangeCurrRing(save);
          return TRUE;
        }

        if (!n_Equal(pN, qN, curr)) tmpIsSkewConstant = false;
      }

    if( bCopyInput )
    {
      C = mpCopy(CC, curr, r); // Copy C into r!!!???
      bCnew = true;
    }
    else
      C = CC;

    IsSkewConstant = tmpIsSkewConstant;

    if ( tmpIsSkewConstant && n_IsOne(pN, curr) )
      nctype = nc_lie;
    else
      nctype = nc_general;
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
      D = mpCopy(DD, curr, r); // Copy DD into r!!!
      bDnew = true;
    }
    else
      D = DD;

  }

  assume( C != NULL );
  assume( D != NULL );

#if OUTPUT
  PrintS("nc_CallPlural(), Computed data, C: \n");
  iiWriteMatrix(C, "C", 2, 4);

  PrintS("nc_CallPlural(), Computed data, D: \n");
  iiWriteMatrix(D, "D", 2, 4);

  Print("\nTemporary: type = %d, IsSkewConstant = %d\n", nctype, IsSkewConstant);
#endif




  // check the ordering condition for D (both matrix and poly cases):
  if ( gnc_CheckOrdCondition(D, r) )
  {
    if( bCnew ) mpDelete( &C, r );
    if( bDnew ) mpDelete( &D, r );

    Werror("Matrix of polynomials violates the ordering condition");

    if( currRing != save )
      rChangeCurrRing(save);
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
    nc_rKill(r);

  r->GetNC() = nc_new;

  if( currRing != save )
    rChangeCurrRing(save);

  return gnc_InitMultiplication(r, bSetupQuotient);
}

//////////////////////////////////////////////////////////////////////////////

bool nc_rCopy(ring res, const ring r, bool bSetupQuotient)
{
  if (nc_CallPlural(r->GetNC()->C, r->GetNC()->D, NULL, NULL, res, bSetupQuotient, true, true, r))
  {
    WarnS("Error occured while coping/setuping the NC structure!"); // No reaction!???
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

  ring save = currRing;

  int WeChangeRing = 0;
  if (currRing!=r)
  {
    rChangeCurrRing(r);
    WeChangeRing = 1;
  }
  assume( (currRing == r)
       && (currRing->GetNC()!=NULL) );   // otherwise we cannot work with all these matrices!

  int i,j;
  r->GetNC()->MT = (matrix *)omAlloc0((r->N*(r->N-1))/2*sizeof(matrix));
  r->GetNC()->MTsize = (int *)omAlloc0((r->N*(r->N-1))/2*sizeof(int));
  idTest(((ideal)r->GetNC()->C));
  matrix COM = mpCopy(r->GetNC()->C);
  poly p,q;
  short DefMTsize=7;
  int IsNonComm=0;
  int tmpIsSkewConstant;

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
      p = p_One(r); /* instead of     p = pOne(); */
      if (MATELEM(r->GetNC()->C,i,j)!=NULL)
        p_SetCoeff(p,n_Copy(pGetCoeff(MATELEM(r->GetNC()->C,i,j)),r),r);
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
      ncRingType(r, nc_skew); /* TODO: check whether it is commutative */
      r->GetNC()->IsSkewConstant=tmpIsSkewConstant;
    }
  }
  r->GetNC()->COM=COM;

  nc_p_ProcsSet(r, r->p_Procs);

  if(bSetupQuotient) // Test me!!!
  {
    nc_SetupQuotient(r);
  }


  // ???
  if( bNoPluralMultiplication )
    ncInitSpecialPairMultiplication(r);


  if(!rIsSCA(r) && !bNoFormula)
    ncInitSpecialPowersMultiplication(r);


  if (save != currRing)
    rChangeCurrRing(save);

  return FALSE;
}

void gnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{
  // "commutative"
  p_Procs->p_Mult_mm  = rGR->p_Procs->p_Mult_mm  = gnc_p_Mult_mm;
  p_Procs->pp_Mult_mm = rGR->p_Procs->pp_Mult_mm = gnc_pp_Mult_mm;
  p_Procs->p_Minus_mm_Mult_qq = rGR->p_Procs->p_Minus_mm_Mult_qq = NULL;
  // gnc_p_Minus_mm_Mult_qq_ign; // should not be used!!!???



  // non-commutaitve multiplication by monomial from the left
  rGR->GetNC()->p_Procs.mm_Mult_p   = gnc_mm_Mult_p;
  rGR->GetNC()->p_Procs.mm_Mult_pp  = gnc_mm_Mult_pp;

  rGR->GetNC()->p_Procs.GB          = gnc_gr_bba; // bba even for local case!

//   rGR->GetNC()->p_Procs.GlobalGB    = gnc_gr_bba;
//   rGR->GetNC()->p_Procs.LocalGB     = gnc_gr_mora;


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

  rGR->GetNC()->p_Procs.BucketPolyRed  = gnc_kBucketPolyRedNew;
  rGR->GetNC()->p_Procs.BucketPolyRed_Z= gnc_kBucketPolyRed_ZNew;
#endif




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

    r->GetNC()->GB()            = gnc_gr_bba;

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
      res = nc_mm_Mult_p(pre,res,currRing);
      /* multiply with suffix */
      suf = pOne();
      pSetExpV(suf,SUF);
      pSetm(suf);
      res = p_Mult_mm(res,suf,currRing);
      res = p_Mult_nn(res,C,currRing);
      pSetComp(res,PRE[0]);
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
  int flag, flagcnt=0, syzcnt=0;
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
      q = pp_Mult_mm(I->m[j],var[i],currRing);
      q = kNF(I,currQuotient,q,0,0);
      if (q!=0)
      {
    h2->m[j]=pCopy(q);
    //  pShift(&(h2->m[flag]),1);
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
      //idShow(h2);
      ring orig_ring=currRing;
      ring syz_ring=rCurrRingAssure_SyzComp();
      syzcomp = 1;
      rSetSyzComp(syzcomp);
      if (orig_ring != syz_ring)
      {
        s_h2=idrCopyR_NoSort(h2,orig_ring);
        //  s_trickyQuotient=idrCopyR_NoSort(trickyQuotient,orig_ring);
        //  rDebugPrint(syz_ring);
        s_I=idrCopyR_NoSort(I,orig_ring);
      }
      else
      {
        s_h2 = h2;
        s_I  = I;
        //  s_trickyQuotient=trickyQuotient;
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
      //idShow(s_h3);
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
        //  s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
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
    //idShow(SI);
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


// creates a commutative nc extension; "converts" comm.ring to a Plural ring
ring nc_rCreateNCcomm(ring r)
{
  if (rIsPluralRing(r)) return r;

  matrix C = mpNew(r->N,r->N); // ring-independent!?!
  matrix D = mpNew(r->N,r->N);

  for(int i=1; i<r->N; i++)
    for(int j=i+1; j<=r->N; j++)
      MATELEM(C,i,j) = p_One( r);

  if (nc_CallPlural(C, D, NULL, NULL, r, false, true, false, currRing, TRUE)) // TODO: what about quotient ideal?
    WarnS("Error initializing multiplication!"); // No reaction!???

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
    //  par_perm[i]=-i;
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

poly pOppose(ring Rop, poly p)
  /* opposes a vector p from Rop to currRing */
{
  /* the simplest case:*/
  if (  Rop == currRing )  return(pCopy(p));
  /* check Rop == rOpposite(currRing) */
  if ( !rIsLikeOpposite(currRing, Rop) )
  {
    WarnS("an opposite ring should be used");
    return NULL;
  }
  /* nMapFunc nMap = nSetMap(Rop);*/
  /* since we know that basefields coinside! */
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
  poly res = pPermPoly(p, perm, Rop, nCopy);
  omFreeSize((ADDRESS)perm,(Rop->N+1)*sizeof(int));
  return res;
}

ideal idOppose(ring Rop, ideal I)
  /* opposes a module I from Rop to currRing */
{
  /* the simplest case:*/
  if ( Rop == currRing ) return idCopy(I);
  /* check Rop == rOpposite(currRing) */
  if (!rIsLikeOpposite(currRing, Rop))
  {
    WarnS("an opposite ring should be used");
    return NULL;
  }
  int i;
  ideal idOp = idInit(I->ncols, I->rank);
  for (i=0; i< (I->ncols)*(I->nrows); i++)
  {
    idOp->m[i] = pOppose(Rop,I->m[i]);
  }
  idTest(idOp);
  return idOp;
}

BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate)
  /* checks whether rings rBase and rCandidate */
  /* could be opposite to each other */
  /* returns TRUE if it is so */
{
  /* the same basefield */
  int diagnose = TRUE;
  ring save = currRing;
  rChangeCurrRing(rBase);
  nMapFunc nMap = nSetMap(rCandidate);
  if (nMap != nCopy) diagnose = FALSE;
  rChangeCurrRing(save);
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
