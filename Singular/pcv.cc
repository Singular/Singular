/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: pcv.cc,v 1.10 1998-11-25 10:27:50 mschulze Exp $ */
/*
* ABSTRACT: conversion between polys and coeff vectors
*/

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "numbers.h"
#include "polys.h"
#include "lists.h"
#include "matpol.h"
#include "pcv.h"

static int pcvMaxDeg;
static int pcvTableSize;
static int pcvIndexSize;
static unsigned* pcvTable=NULL;
static unsigned** pcvIndex=NULL;

int pcvDeg(poly p)
{
  int dp=0;
  for(int i=1;i<=pVariables;i++) dp+=pGetExp(p,i);
  return dp;
}

int pcvOrd(poly p)
{
  if(!p) return -1;
  int op=pcvDeg(p);
  pIter(p);
  while(p)
  {
    int d=pcvDeg(p);
    if(d<op) op=d;
    pIter(p);
  }
  return op;
}

int pcvOrd(matrix m)
{
  int om=-1;
  for(int i=MATROWS(m);i>=1;i--)
  {
    for(int j=MATCOLS(m);j>=1;j--)
    {
      int o=pcvOrd(MATELEM(m,i,j));
      if(o<om&&o>=0||om==-1) om=o;
    }
  }
  return om;
}

BOOLEAN pcvOrd(leftv res,leftv h)
{
  if(h)
  {
    if(h->Typ()==POLY_CMD)
    {
      res->rtyp=INT_CMD;
      res->data=pcvOrd((poly)h->Data());
      return FALSE;
    }
    if(h->Typ()==MATRIX_CMD)
    {
      res->rtyp=INT_CMD;
      res->data=pcvOrd((matrix)h->Data());
      return FALSE;
    }
  }
  WerrorS("<matrix> expected");
  return TRUE;
}

void pcvInit(int d)
{
  if(d<0) d=0;
  pcvMaxDeg=d;
  pcvTableSize=pVariables*pcvMaxDeg*sizeof(unsigned);
  pcvTable=Alloc0(pcvTableSize);
  pcvIndexSize=pVariables*sizeof(unsigned*);
  pcvIndex=Alloc(pcvIndexSize);
  for(int i=0;i<pVariables;i++)
    pcvIndex[i]=pcvTable+i*pcvMaxDeg;
  for(int i=0;i<pcvMaxDeg;i++)
    pcvIndex[0][i]=i;
  for(int i=1;i<pVariables;i++)
  {
    unsigned x=0;
    for(int j=0;j<pcvMaxDeg;j++)
    {
      x+=pcvIndex[i-1][j];
      pcvIndex[i][j]=x;
    }
  }
}

void pcvClean()
{
  if(pcvTable)
  {
    Free(pcvTable,pcvTableSize);
    pcvTable=NULL;
  }
  if(pcvIndex)
  {
    Free(pcvIndex,pcvIndexSize);
    pcvIndex=NULL;
  }
}

int pcvM2n(poly m)
{
  unsigned n=0,d=0;
  for(int i=0;i<pVariables;i++)
  {
    d+=pGetExp(m,i+1);
    n+=pcvIndex[i][d];
  }
  return n+1;
}

poly pcvN2m(int n)
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

poly pcvP2cv(poly p,int d0,int d1)
{
  poly cv=NULL;
  while(p)
  {
    int d=pcvDeg(p);
    if(d0<=d&&d<d1)
    {
      poly c=pOne();
      pSetComp(c,pcvM2n(p));
      pSetCoeff(c,nCopy(pGetCoeff(p)));
      cv=pAdd(cv,c);
    }
    pIter(p);
  }
  return cv;
}

poly pcvCv2p(poly cv,int d0,int d1)
{
  poly p=NULL;
  while(cv)
  {
    poly m=pcvN2m(pGetComp(cv));
    if(m)
    {
      int d=pcvDeg(m);
      if(d0<=d&&d<d1)
      {
        pSetCoeff(m,nCopy(pGetCoeff(cv)));
        p=pAdd(p,m);
      }
    }
    pIter(cv);
  }
  return p;
}

lists pcvP2cv(lists pl,int d0,int d1)
{
  lists cvl=(lists)Alloc(sizeof(slists));
  cvl->Init(pl->nr+1);
  pcvInit(d1);
  for(int i=pl->nr;i>=0;i--)
  {
    if(pl->m[i].rtyp==POLY_CMD)
    {
      cvl->m[i].rtyp=VECTOR_CMD;
      cvl->m[i].data=pcvP2cv((poly)pl->m[i].data,d0,d1);
    }
  }
  pcvClean();
  return cvl;
}

lists pcvCv2p(lists cvl,int d0,int d1)
{
  lists pl=(lists)Alloc(sizeof(slists));
  pl->Init(cvl->nr+1);
  pcvInit(d1);
  for(int i=cvl->nr;i>=0;i--)
  {
    if(cvl->m[i].rtyp==VECTOR_CMD)
    {
      pl->m[i].rtyp=POLY_CMD;
      pl->m[i].data=pcvCv2p((poly)cvl->m[i].data,d0,d1);
    }
  }
  pcvClean();
  return pl;
}

BOOLEAN pcvP2cv(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==LIST_CMD)
    {
      lists pl=(lists)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d0=(int)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int d1=(int)h->Data();
          res->rtyp=LIST_CMD;
          res->data=pcvP2cv(pl,d0,d1);
          return FALSE;
        }
      }
    }
    WerrorS("<list>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

BOOLEAN pcvCv2p(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==LIST_CMD)
    {
      lists pl=(lists)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d0=(int)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int d1=(int)h->Data();
          res->rtyp=LIST_CMD;
          res->data=pcvCv2p(pl,d0,d1);
          return FALSE;
        }
      }
    }
    WerrorS("<list>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

int pcvDim(int d0,int d1)
{
  if(d0<0) d0=0;
  if(d1<0) d1=0;
  pcvInit(d1+1);
  int d=pcvIndex[pVariables-1][d1]-pcvIndex[pVariables-1][d0];
  pcvClean();
  return d;
}

BOOLEAN pcvDim(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==INT_CMD)
    {
      int d0=(int)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d1=(int)h->Data();
        res->rtyp=INT_CMD;
        res->data=pcvDim(d0,d1);
        return FALSE;
      }
    }
    WerrorS("<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

int pcvBasis(lists b,int i,poly m,int d,int n)
{
  if(n<pVariables)
  {
    for(int k=0,l=d;k<=l;k++,d--)
    {
      pSetExp(m,n,k);
      i=pcvBasis(b,i,m,d,n+1);
    }
  }
  else
  {
    pSetExp(m,n,d);
    pSetm(m);
    b->m[i].rtyp=POLY_CMD;
    b->m[i++].data=pCopy(m);
  }
  return i;
}

lists pcvBasis(int d0,int d1)
{
  if(d0<0) d0=0;
  if(d1<0) d1=0;
  lists b=(lists)Alloc(sizeof(slists));
  b->Init(pcvDim(d0,d1));
  poly m=pOne();
  for(int d=d0,i=0;d<d1;d++)
    i=pcvBasis(b,i,m,d,1);
  pDelete1(&m);
  return b;
}

BOOLEAN pcvBasis(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==INT_CMD)
    {
      int d0=(int)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d1=(int)h->Data();
        res->rtyp=LIST_CMD;
        res->data=pcvBasis(d0,d1);
        return FALSE;
      }
    }
    WerrorS("<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}
