/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: pcv.cc,v 1.5 1998-11-18 14:12:24 mschulze Exp $ */
/*
* ABSTRACT: conversion between polys and coeff vectors
*/

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "intvec.h"
#include "pcv.h"

static int pcvMaxDeg;
static int pcvTableSize;
static int pcvIndexSize;
static unsigned* pcvTable=NULL;
static unsigned** pcvIndex=NULL;

// INPUT: poly p, weights w[]
// OUTPUT: w-deg(L(p))
int pcvDegW(poly p,short w[])
{
  int d=0;
  for(int i=1;i<=pVariables;i++)
    d+=(*(w++))*pGetExp(p,i);
  return d;
}

// INPUT: poly p, weights w[]
// OUTPUT: min{w-deg(m)|m monomial of p}
int pcvOrdW(poly p,short w[])
{
  if(!p) return -1;
  int o=pcvDegW(p,w);
  pIter(p);
  while(p)
  {
    int d=pcvDegW(p,w);
    if(d<o) o=d;
    pIter(p);
  }
  return o;
}

// PURPOSE: allocate and initialize pcvTable and pcvIndex:
// pcvIndex[i][j]=#{m|m i-monomial, deg(m)<j},
// 0<=i<pVariables, 0<=j<=max{deg(m)|m i-monomial, w-deg(m)<d}
// INPUT: deg d, weights w[]
// NOTE: call pcvClean() to free pcvTable and pcvIndex
void pcvInit(int d,short w[])
{
  int i,j;
  for(i=1,j=w[0];i<pVariables;i++)
    if(w[i]<j) j=w[i];
  pcvMaxDeg=d/j;
  pcvTableSize=(pVariables*(pcvMaxDeg+1))*sizeof(unsigned);
  pcvTable=(unsigned*)Alloc0(pcvTableSize);
  pcvIndexSize=pVariables*sizeof(unsigned*);
  pcvIndex=(unsigned**)Alloc(pcvIndexSize);
  for(i=0;i<pVariables;i++)
    pcvIndex[i]=pcvTable+i*(pcvMaxDeg+1);
  for(i=0;i<=pcvMaxDeg;i++)
    pcvIndex[0][i]=i;
  for(i=1;i<pVariables;i++)
  {
    unsigned x=0;
    for(j=0;j<=pcvMaxDeg;j++)
    {
      x+=pcvIndex[i-1][j];
      pcvIndex[i][j]=x;
    }
  }
}

// PURPOSE: allocate and initialize pcvTable and pcvIndex:
// pcvIndex[i][j]=#{m|m i-monomial, w-deg(m)<j},
// 0<=i<pVariables, 0<=j<=d
// INPUT: w-deg d, weights w[]
// NOTE: call pcvClean() to free pcvTable and pcvIndex
void pcvInitW(int d,short w[])
{
  pcvMaxDeg=d;
  pcvTableSize=(pVariables*(pcvMaxDeg+1))*sizeof(unsigned);
  pcvTable=(unsigned*)Alloc0(pcvTableSize);
  pcvIndexSize=pVariables*sizeof(unsigned*);
  pcvIndex=(unsigned**)Alloc(pcvIndexSize);
  int i,j,k;
  for(i=0;i<pVariables;i++)
    pcvIndex[i]=pcvTable+i*(pcvMaxDeg+1);
  for(i=0,j=1,k=1;j<=pcvMaxDeg;i++,j++)
  {
    if(i==w[0])
    {
      i=0;
      k++;
    }
    pcvIndex[0][j]=k;
  }
  for(i=1;i<pVariables;i++)
  {
    for(k=1;k<=w[i];k++)
    {
      unsigned x=0;
      for(j=k;j<=pcvMaxDeg;j+=w[i])
      {
        x+=pcvIndex[i-1][j];
        pcvIndex[i][j]=x;
      }
    }
  }
}

// PURPOSE: free pcvTable, pcvIndex
void pcvClean()
{
  if(pcvTable)
  {
    Free((ADDRESS)pcvTable,pcvTableSize);
    pcvTable=NULL;
  }
  if(pcvIndex)
  {
    Free((ADDRESS)pcvIndex,pcvIndexSize);
    pcvIndex=NULL;
  }
}

// INPUT: momonial p
// OUTPUT: number of p
// NOTE: call pcvInit() before, pcvClean() after
int pcvMon2Num(poly m)
{
  unsigned n=0,d=0;
  for(int i=0;i<pVariables;i++)
  {
    d+=pGetExp(m,i+1);
    n+=pcvIndex[i][d];
  }
  return n+1;
}

// INPUT: number n
// OUTPUT: monomial with number n
// NOTE: call pcvInit() before, pcvClean() after
poly pcvNum2Mon(int n)
{
  n--;
  poly m=pOne();
  int i,j,k;
  for(i=pVariables-1;i>=0;i--)
  {
    k=j;
    for(j=0;j<pcvMaxDeg&&pcvIndex[i][j]<=n;j++);
    j--;
    n-=pcvIndex[i][j];
    if(i<pVariables-1) pSetExp(m,i+2,k-j);
  }
  if(n==0)
  {
    pSetExp(m,1,j);
    pSetm(m);
    return m;
  }
  else
  {
    pDelete1(&m);
    return NULL;
  }
}

// INPUT: poly p, w-deg d0, w-deg d1, weights w[]
// OUTPUT: coeff vector of p
// considering monomials m with d0<=w-deg(m)<d1
// NOTE: call pcvInit() before, pcvClean() after
poly pcvPoly2Vec(poly p,int d0,int d1,short w[])
{
  poly r=NULL;
  while(p)
  {
    int d=pcvDegW(p,w);
    if(d0<=d&&d<d1)
    {
      poly pp=pOne();
      pSetComp(pp,pcvMon2Num(p));
      pSetCoeff(pp,nCopy(pGetCoeff(p)));
      r=pAdd(r,pp);
    }
    pIter(p);
  }
  return r;
}

// INPUT: coeff vector v, w-deg d0, w-deg d1, weights w[]
// OUTPUT: poly with coeff vector v
// considering monomials m with d0<=w-deg(m)<d1
// NOTE: call pcvInit() before, pcvClean() after
poly pcvVec2Poly(poly v,int d0,int d1,short w[])
{
  poly r=NULL;
  while(v)
  {
    poly p=pcvNum2Mon(pGetComp(v));
    if(p)
    {
      int d=pcvDegW(p,w);
      if(d0<=d&&d<d1)
      {
        pSetCoeff(p,nCopy(pGetCoeff(v)));
        r=pAdd(r,p);
      }
    }
    pIter(v);
  }
  return r;
}

// INPUT: ideal I, w-deg d0, w-deg d1, weights w[]
// OUTPUT: module M of coeff vectors of polys in I
// considering monomials m with d0<=w-deg(m)<d1
// NOTE: calls pcvInit(), pcvClean()
ideal pcvId2Mod(ideal I,int d0,int d1,short w[])
{
  if(d1<d0) d1=d0;
  pcvInit(d1,w);
  ideal M=idInit(IDELEMS(I),1);
  for(int i=IDELEMS(I)-1;i>=0;i--)
    M->m[i]=pcvPoly2Vec(I->m[i],d0,d1,w);
  M->rank=max(1,idRankFreeModule(M));
  pcvClean();
  return M;
}

// INPUT: module M of coeff vectors, w-deg d0, w-deg d1, weights w[]
// OUTPUT: ideal I of polys with coeff vectors in M
// considering monomials m with d0<=w-deg(m)<d1
// NOTE: calls pcvInit(), pcvClean()
ideal pcvMod2Id(ideal M,int d0,int d1,short w[])
{
  if(d1<d0) d1=d0;
  pcvInit(d1,w);
  ideal I=idInit(IDELEMS(M),1);
  for(int i=IDELEMS(I)-1;i>=0;i--)
    I->m[i]=pcvVec2Poly(M->m[i],d0,d1,w);
  pcvClean();
  return I;
}

// INPUT: w-deg d0, w-deg d1, weights w[]
// OUTPUT: number of monomials m with d0<=w-deg(m)<d1
// NOTE: calls pcvInitW(), pcvClean()
int pcvDimW(int d0,int d1,short w[])
{
  if(d1<d0) d1=d0;
  pcvInitW(d1,w);
  int d=pcvIndex[pVariables-1][d1]-pcvIndex[pVariables-1][d0];
  pcvClean();
  return d;
}

// PURPOSE: see pcvBasisW()
int pcvDegBasisW(ideal I,int i,poly m,int d,int n,short w[])
{
  if(n+1<pVariables)
  {
    int l=d/w[0];
    for(int k=0;k<=l;k++,d-=w[0])
    {
      pSetExp(m,n+1,k);
      i=pcvDegBasisW(I,i,m,d,n+1,w+1);
    }
  }
  else
  if(d%w[0]==0)
  {
    pSetExp(m,n+1,d/w[0]);
    pSetm(m);
    I->m[i++]=pCopy(m);
  }
  return i;
}

// INPUT: w-deg d0, w-deg d1, weights w[]
// OUTPUT: ideal of monomials m with d0<=w-deg(m)<d1
// NOTE: calls pcvDimW(), pcvDegBasisW()
ideal pcvBasisW(int d0,int d1,short w[])
{
  ideal I;
  if(d1<=d0) I=idInit(1,1);
  else I=idInit(pcvDimW(d0,d1,w),1);
  poly m=pOne();
  for(int d=d0,i=0;d<d1;d++)
    i=pcvDegBasisW(I,i,m,d,0,w);
  pDelete1(&m);
  return I;
}

/*2
* interface to interpreter
*/
BOOLEAN iiPcvConv(leftv res, leftv h)
{
  if(h&&(h->Typ()==IDEAL_CMD||h->Typ()==MODUL_CMD))
  {
    leftv hh=h->next;
    int i0=0,i1;
    short* w=(short*)Alloc(currRing->N*sizeof(short));
    BOOLEAN defi1=FALSE,defw=FALSE;
    while(hh)
    {
      if(hh->Typ()==INT_CMD)
      {
        if(defi1)
        {
          i0=i1;
          i1=(int)hh->Data();
        }
        else
        {
          i1=(int)hh->Data();
          defi1=TRUE;
        }
      }
      else
      if(hh->Typ()==INTVEC_CMD)
      {
        intvec *iv=(intvec*)hh->Data();
        if(iv->rows()==currRing->N&&iv->cols()==1)
        {
          for(int i=0;i<currRing->N;i++) w[i]=(*iv)[i];
          defw=TRUE;
        }
      }
      hh=hh->next;
    }
    if(defi1)
    {
      if(!defw) for(int i=0;i<currRing->N;i++) w[i]=1;
      if(h->Typ()==IDEAL_CMD)
      {
        // "pcvConv",<ideal>[,<int d0>],<int d1>[,<intvec w>]:
        // convert ideal to module of coeff vectors
        // considering monomials m with d0<=w-deg(m)<d1
        res->rtyp=MODUL_CMD;
        res->data=(void*)pcvId2Mod((ideal)h->Data(),i0,i1,w);
        Free((ADDRESS)w,currRing->N*sizeof(short));
        return FALSE;
      }
      else
      {
        // "pcvConv",<module>[,<int d0>],<int d1>[,<intvec w>]:
        // convert module of coeff vectors to ideal
        // considering monomials m with d0<=w-deg(m)<d1
        res->rtyp=IDEAL_CMD;
        res->data=(void*)pcvMod2Id((ideal)h->Data(),i0,i1,w);
        Free((ADDRESS)w,currRing->N*sizeof(short));
        return FALSE;
      }
    }
    Free((ADDRESS)w,currRing->N*sizeof(short));
  }
  WerrorS("<ideal/module>[,<int>],<int>[,<intvec>] expected");
  return TRUE;
}

/*2
* interface to interpreter
*/
BOOLEAN iiPcvDim(leftv res, leftv h)
{
  if(!currRingHdl)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  int i0=0,i1;
  short* w=(short*)Alloc(currRing->N*sizeof(short));
  BOOLEAN defi1=FALSE,defw=FALSE;
  while(h)
  {
    if(h->Typ()==INT_CMD)
    {
      if(defi1)
      {
        i0=i1;
        i1=(int)h->Data();
      }
      else
      {
        i1=(int)h->Data();
        defi1=TRUE;
      }
    }
    else
    if(h->Typ()==INTVEC_CMD)
    {
      intvec *iv=(intvec*)h->Data();
      if(iv->rows()==currRing->N&&iv->cols()==1)
      {
        int i;
        for(i=0;i<currRing->N;i++) w[i]=(*iv)[i];
        defw=TRUE;
      }
    }
    h=h->next;
  }
  if(!defw)
  {
    int i;
    for(i=0;i<currRing->N;i++) w[i]=1;
  }
  if(defi1)
  {
    // "pcvDim"[,<int d0>],<int d1>[,<intvec w>]:
    // number of monomials m with d0<=w-deg(m)<d1
    res->rtyp=INT_CMD;
    res->data=(void*)pcvDimW(i0,i1,w);
    return FALSE;
  }
  Free((ADDRESS)w,(currRing->N)*sizeof(short));
  WerrorS("[<int>],<int>[,<intvec>] expected");
  return TRUE;
}

/*2
* interface to interpreter
*/
BOOLEAN iiPcvBasis(leftv res, leftv h)
{
  if(!currRingHdl)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  int i0=0,i1;
  short* w=(short*)Alloc(currRing->N*sizeof(short));
  BOOLEAN defi1=FALSE,defw=FALSE;
  while(h!=NULL)
  {
    if(h->Typ()==INT_CMD)
    {
      if(defi1)
      {
        i0=i1;
        i1=(int)h->Data();
      }
      else
      {
        i1=(int)h->Data();
        defi1=TRUE;
      }
    }
    else
    if(h->Typ()==INTVEC_CMD)
    {
      intvec *iv=(intvec*)h->Data();
      if(iv->rows()==currRing->N&&iv->cols()==1)
      {
        int i;
        for(i=0;i<currRing->N;i++) w[i]=(*iv)[i];
        defw=TRUE;
      }
    }
    h=h->next;
  }
  if(!defw)
  {
    int i;
    for(i=0;i<currRing->N;i++) w[i]=1;
  }
  if(defi1)
  {
    // "pcvBasis"[,<int d0>],<int d1>[,<intvec w>]:
    // ideal of monomials m with d0<=w-deg(m)<d1
    res->rtyp=IDEAL_CMD;
    res->data=(void*)pcvBasisW(i0,i1,w);
    return FALSE;
  }
  Free((ADDRESS)w,(currRing->N)*sizeof(short));
  WerrorS("[<int>],<int>[,<intvec>] expected");
  return TRUE;
}

/*2
* interface to interpreter
*/
BOOLEAN iiPcvOrd(leftv res, leftv h)
{
  if(h&&h->Typ()==POLY_CMD)
  {
    leftv hh=h->next;
    short* w=(short*)Alloc(currRing->N*sizeof(short));
    BOOLEAN defw=FALSE;
    while(hh)
    {
      if(hh->Typ()==INTVEC_CMD)
      {
        intvec *iv=(intvec*)hh->Data();
        if(iv->rows()==currRing->N&&iv->cols()==1)
        {
          for(int i=0;i<currRing->N;i++) w[i]=(*iv)[i];
          defw=TRUE;
        }
      }
      hh=hh->next;
    }
    if(!defw) for(int i=0;i<currRing->N;i++) w[i]=1;
    // "pcvOrd",<poly p>[,<intvec w>]:
    // min{w-deg(m)|m monomial of p}
    res->rtyp=INT_CMD;
    res->data=(void*)pcvOrdW((poly)h->Data(),w);
    Free((ADDRESS)w,currRing->N*sizeof(short));
    return FALSE;
  }
  WerrorS("<poly>[,<intvec>] expected");
  return TRUE;
}
