/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    gring.cc
 *  Purpose: pMult family of procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *  Version: $Id: gring.cc,v 1.2 2000-11-20 16:02:03 levandov Exp $
 *******************************************************************/
#include "mod2.h"
#include "gring.h"
#include "polys.h"
#ifdef HAVE_PLURAL

//global nc_macros :
#define freeT(A) omFree((ADDRESS)A,(pVariables+1)*sizeof(Exponent_t))
#define freeN(A,k) omFree((ADDRESS)A,k*sizeof(number))

poly nc_pp_Mult_mm(poly p, const poly m, const poly spNoether, const ring ri)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);

  do
  {
    p_AllocBin( pNext(q), bin, ri); 
    q = pNext(q);
    pSetCoeff0(q, n_Mult(ln, pGetCoeff(p), ri));
// old:      p_MemSum(q->exp, p->exp, m_e, length);
    q = mm_Mult_mm(p->exp,m_e);  // give exponents only?
    p_MemAddAdjust(q, ri);
    p = pNext(p);
  }
  while (p != NULL);

  pNext(q) = NULL;  
  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}

poly nc_mm_Mult_nn(Exponent_t *F, Exponent_t *G, const ring ri)

/* destroys both f and g , f,g are monomials with the coefficient */
//poly pMultTT(poly f, poly g)
{
  poly out=NULL;
  int nv=pVariables;
  int i;
  number cF,cG,cOut;
// What can we do with coeffs?
//  cF=pGetCoeff(f);
//  cG=pGetCoeff(g);
  cOut=nMult(cF,cG);
  int iF,jG,iG;

  Exponent_t exp=F[0];

  iF=nv;
  while ((F[iF]==0)&&(iF>=1)) iF--; /* last exp_num of F */
  jG=1;
  while ((G[jG]==0)&&(jG<=nv)) jG++;  /* first exp_num of G */
  iG=nv;
  while ((G[iG]==0)&&(iG>=1)) iG--;  /* last exp_num of G */


  if (iF<=jG)                     /* i.e. no mixed exp_num */
  {
    out=pOne();
    for (i=1;i<=nv;i++) { F[i]=F[i]+G[i];}
//    F[0]=exp;
    pSetExpV(out,F);
    pSetCoeff(out,cOut);
    pSetm(out);
    freeT(F);freeT(G);
    return(out);
  }

  if (iG==jG)  /* g is uni */
  {
//    if (ri->nc->type==nc_skew) -- postpone to TU   
    out=pMultTU(F,jG,G[jG]);
    out=pSetCoeffP(out,cOut);
    pSetCompP(out,exp);
    freeT(F);freeT(G);
    nDelete(&cOut);
    return(out);
  }
  number n1=nInit(1);
  Exponent_t *Prv=(Exponent_t *)Alloc0((pVariables+1)*sizeof(Exponent_t));
  Exponent_t *Nxt=(Exponent_t *)Alloc0((pVariables+1)*sizeof(Exponent_t));

  int nnv=nv+1;
  int *log=(int *)Alloc0((nnv)*sizeof(int));
  int cnt=0; int cnf=0;

  /* splitting F wrt jG */
  for (i=1;i<=jG;i++)
  {
    Prv[i]=F[i]; Nxt[i]=0; /* mult at the very end */
    if (F[i]!=0) cnf++;
  }

  if (cnf==0) freeT(Prv);

  for (i=jG+1;i<=nv;i++)
  {
    Nxt[i]=F[i];
    if (cnf!=0)  Prv[i]=0;
    if (F[i]!=0)
    {
      cnt++;
    }              /* effective part for F */
  }
  freeT(F);
  cnt=0;

  for (i=1;i<=nv;i++)
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
  number *c=(number *)Alloc0((nv+1)*sizeof(number));
  c[0]=nInit(1);

  Exponent_t *Op=Nxt;
  Exponent_t *On=G;
  Exponent_t *U=(Exponent_t *)Alloc0((pVariables+1)*sizeof(Exponent_t));

  for (i=jG;i<=nv;i++) U[i]=Nxt[i]+G[i];  /* make leadterm */
  for (i=1;i<jG;i++) U[i]=0;
  Nxt=NULL; G=NULL;

  Op[0]=exp;
  cnt=1;
  int t=0;
  poly w=NULL;
  poly Pn=pOne();
  pSetExpV(Pn,On);
  pSetm(Pn);

  while (On[iG]!=0)
  {
     t=log[cnt];

     w=pMultTU(Op,t,On[t]);
     c[cnt]=nMult(c[cnt-1],pGetCoeff(w));
     D = pNext(w);  /* getting coef and rest D */
     pDelete1(&w); w=NULL;

     Op[t] += On[t];   /* update exp_vectors */
     On[t] = 0;

     if (t!=iG)    /* not the last step */
     {
       On[0]=exp;
       pSetExpV(Pn,On);
       pSetm(Pn);
       Rout=pMultT(D,Pn);
     }
     else
     {  Rout=D; D=NULL; }


     if (Rout!=NULL)
     {
       Rout=pSetCoeffP(Rout,c[cnt-1]); /* Rest is ready */
       out=pAdd(out,Rout);
       Rout=NULL;
     }
     cnt++;
  }
  freeT(On);freeT(Op);
  pDelete(&Pn);
  Free((ADDRESS)log,nnv*sizeof(int));

/* leadterm and Prv-part with cOut */

  Rout=pOne();
  /* U is lead.monomial */
  U[0]=exp;
  pSetExpV(Rout,U);pSetm(Rout);  /* use again this name Rout */
  pSetCoeff(Rout,c[cnt-1]);
  out=pAdd(out,Rout);
  Rout=NULL;
  freeT(U); freeN(c,nnv);
  if (cnf!=0)  /* Prv is non-zero vector */
  {
    Rout=pOne();
    Prv[0]=exp;
    pSetExpV(Rout,Prv);
    pSetm(Rout);
    pSetCoeff(Rout,cOut); //here cOut from begin
    out=pMultT2(Rout,out); //getting finite result
    freeT(Prv);
    pDelete(&Rout);
  }
  else
  {
    out=pSetCoeffP(out,cOut);
    nDelete(&cOut);
  }

  pSetCompP(out, exp);
  return (out);
}

//----------pMultUU--------- 
poly nc_uu_Mult_ww (int i, int a, int j, int b,const ring r)
{
  int nv=pVariables;
  poly out=NULL;

  if (i<=j)        /* usual expression fg */
  {
      out=pOne();
      p_SetExp(out,j,b,r);
      p_SetExp(out,i,a,r);
      if (i==j) p_SetExp(out,j,a+b,r);
      p_Setm(out,r);
      return(out);
   }
   else    /* when i>j */
   {
      if (a==0)
      {
         if (b==0)
         {
           out=pOne();
           return(out);
         }
         else
         {
            out=pOne();
            p_SetExp(out,j,b,r);p_Setm(out,r);
            return(out);
          }
      }
      else
      {
         if (b==0)
         {
            out=pOne();
            p_SetExp(out,i,a,r);p_Setm(out,r);
            return(out);
         }
      }

    }
// is it already computed ?

  int vik = UPMATELEM(j,i);
// FOR future nc_skew case  
//  if ((ri->nc->type==nc_skew)||(ri->nc->MF[vik]==0))
//   {
//     out=pOne();
//     pSetExp(out,j,b); pSetExp(out,i,a);
//     pSetm(out);
//     number tmp_number=NULL;
//     nPower(c,a*b,&tmp_number);
//     pSetCoeff(out,tmp_number);
//     return (out);
//   }
  matrix cMT=r->MT[vik];
  int cMTsize=r->MTsize[vik];

  if (((a<cMTsize)&&(b<cMTsize))&&(MATELEM(cMT,a,b)!=NULL))
  {
     out=p_Copy(MATELEM(cMT,a,b),r);
     return (out);
  }

//End(Zero_Exceptions = (0,0),(0,b),(a,0),(a==b) )
//in fact, now a>=1 and b>=1; and j<i

  poly C=MATELEM(r->C,j,i);
  number c=p_GetCoeff(C,r); //coeff
  poly D=MATELEM(r->D,j,i);       //rest

  int newcMTsize=0;

  if (D==NULL)                   /* (skew)-commutativity check */
  {
     out=pOne();
     p_SetExp(out,j,b,r); p_SetExp(out,i,a,r);
     p_Setm(out,r);
     number tmp_number=NULL;
     nPower(c,a*b,&tmp_number);
     p_SetCoeff(out,tmp_number,r);
     return(out);
  }

  if ((a==1)&&(b==1))
  {
     out= p_Copy(MATELEM(cMT,1,1),r);  /* already computed */
     return(out);
  }
  D=NULL;

  if (a>=b) {newcMTsize=a;} else {newcMTsize=b;}
  if (newcMTsize>cMTsize)
  {
     newcMTsize = newcMTsize+cMTsize;
     matrix tmp = mpNew(newcMTsize,newcMTsize);
     for (int p=1;p<nv;p++)
     {
        for (int q=p;q<=nv;q++)
        {
           MATELEM(tmp,p,q) = MATELEM(r->MT[UPMATELEM(j,i)],p,q);
           MATELEM(r->MT[UPMATELEM(j,i)],p,q)=NULL;
        }
     }
     id_Delete((ideal *)&(r->MT[UPMATELEM(j,i)]),r);
     r->MT[UPMATELEM(j,i)] = tmp;
     r->MTsize[UPMATELEM(j,i)] = newcMTsize;
  }  /* The update of multiplication matrix is finished */

  cMT=r->MT[UPMATELEM(j,i)];         //cMT=current MT

  poly x=pOne();p_SetExp(x,j,1,r);p_Setm(x,r);//var(j);
  poly y=pOne();p_SetExp(y,i,1,r);pSetm(y,r);//var(i);  for convenience

  int k,m;
  poly t=NULL;
/* ------------ Main Cycles ----------------------------*/

  for (k=2;k<=a;k++)
  {
     t=MATELEM(cMT,k,1);

     if (t==NULL)   /* not computed yet */
     {
        t=p_Copy(MATELEM(cMT,k-1,1),r);
//        t = pMultT2(y,t);
        t = nc_m_Mult_pp(y,t,,r);
        MATELEM(cMT,k,1) = t;
     }
     t=NULL;
  }


  for (m=2;m<=b;m++)
  {
     t=MATELEM(cMT,a,m);

     if (t==NULL)   //not computed yet
     {
        t=p_Copy(MATELEM(cMT,a,m-1),r);
//        t = pMultT(t,x);
        t = nc_p_Mult_q(t,x,,r);
        MATELEM(cMT,a,m) = t;
     }
     t=NULL;
  }
  p_Delete(&x,r); p_Delete(&y,r);
  t=MATELEM(cMT,a,b);
  return(p_Copy(t,r));  /* as last computed element was cMT[a,b] */
}
#endif
