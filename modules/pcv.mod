%{
/*
 *  $Id$
 *
 *  Test mod fuer modgen
 */

#include <stdio.h>
#include <mod2.h>
#include <tok.h>
#include <ipid.h>
#include <numbers.h>
#include <polys.h>
#include <ideals.h>
#include <lists.h>
#include <matpol.h>
#include <febase.h>

static int pcvMaxDegree;
static int pcvTableSize;
static int pcvIndexSize;
static unsigned* pcvTable=NULL;
static unsigned** pcvIndex=NULL;

#ifndef PCV_H
#define PCV_H

lists pcvLAddL(lists l1,lists l2);
lists pcvPMulL(poly p,lists l1);
BOOLEAN pcvLAddL(leftv res,leftv h);
BOOLEAN pcvPMulL(leftv res,leftv h);
int pcvDeg(poly p);
int pcvMinDeg(poly p);
int pcvMinDeg(matrix m);
BOOLEAN pcvMinDeg(leftv res,leftv h);
void pcvInit(int d);
void pcvClean();
int pcvM2N(poly m);
poly pcvN2M(int n);
poly pcvP2CV(poly p,int d0,int d1);
poly pcvCV2P(poly cv,int d0,int d1);
lists pcvP2CV(lists pl,int d0,int d1);
ideal pcvP2CV(ideal p,int d0,int d1);
lists pcvCV2P(lists cvl,int d0,int d1);
ideal pcvCV2P(ideal cv,int d0,int d1);
BOOLEAN pcvP2CV(leftv res,leftv h);
BOOLEAN pcvCV2P(leftv res,leftv h);
int pcvDim(int d0,int d1);
BOOLEAN pcvDim(leftv res,leftv h);
int pcvBasis(lists b,int i,poly m,int d,int n);
lists pcvBasis(int d0,int d1);
BOOLEAN pcvBasis(leftv res,leftv h);

#endif
%}


// module="pcv";
package="pcv";

version="$Id$";
info="
LIBRARY: pcv.so  CONVERSION BETWEEN POLYS AND COEF VECTORS
AUTHOR:  Mathias Schulze, email: mschulze@mathematik.uni-kl.de

 MinDeg(p);      min deg of monomials of poly p
 P2CV(l,d0,d1);  list of coef vectors from deg d0 to d1 of polys in list l
 CV2P(l,d0,d1);  list of polys with coef vectors from deg d0 to d1 in list l
 Dim(d0,d1);     number of monomials from deg d0 to d1
 Basis(d0,d1);   list of monomials from deg d0 to d1
";

%modinitial
// no commands to be run upon loading the module
%endinitial

%procedures


int MinDeg(poly p) {
   %declaration;
   %typecheck;
   %return(pcvMinDeg(p));
}

list P2CV(list pl,int d0,int d1) {
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvP2CV(pl, d0, d1));
}

list CV2P(list pl,int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvCV2P(pl, d0, d1));
}

int Dim(int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvDim);
}

list Basis(int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvBasis);
}

%C
lists pcvLAddL(lists l1,lists l2)
{
  lists l0=(lists)omAllocBin(slists_bin);
  int i=l1->nr;
  if(l1->nr<l2->nr) i=l2->nr;
  l0->Init(i+1);
  for(;i>=0;i--)
  {
    if(i<=l1->nr&&(l1->m[i].rtyp==POLY_CMD||l1->m[i].rtyp==VECTOR_CMD))
    {
      l0->m[i].rtyp=l1->m[i].rtyp;
      l0->m[i].data=pCopy((poly)l1->m[i].data);
      if(i<=l2->nr&&l2->m[i].rtyp==l1->m[i].rtyp)
        l0->m[i].data=pAdd((poly)l0->m[i].data,pCopy((poly)l2->m[i].data));
    }
    else
    if(i<=l2->nr&&(l2->m[i].rtyp==POLY_CMD||l2->m[i].rtyp==VECTOR_CMD))
    {
      l0->m[i].rtyp=l2->m[i].rtyp;
      l0->m[i].data=pCopy((poly)l2->m[i].data);
    }
  }
  return(l0);
}

lists pcvPMulL(poly p,lists l1)
{
  lists l0=(lists)omAllocBin(slists_bin);
  l0->Init(l1->nr+1);
  for(int i=l1->nr;i>=0;i--)
  {
    if(l1->m[i].rtyp==POLY_CMD)
    {
      l0->m[i].rtyp=POLY_CMD;
      l0->m[i].data=ppMult_qq(p,(poly)l1->m[i].data);
    }
  }
  return(l0);
}

BOOLEAN pcvLAddL(leftv res,leftv h)
{
  if(h&&h->Typ()==LIST_CMD)
  {
    lists l1=(lists)h->Data();
    h=h->next;
    if(h&&h->Typ()==LIST_CMD)
    {
      lists l2=(lists)h->Data();
      res->rtyp=LIST_CMD;
      res->data=(void*)pcvLAddL(l1,l2);
      return FALSE;
    }
  }
  WerrorS("<list>,<list> expected");
  return TRUE;
}

BOOLEAN pcvPMulL(leftv res,leftv h)
{
  if(h&&h->Typ()==POLY_CMD)
  {
    poly p=(poly)h->Data();
    h=h->next;
    if(h&&h->Typ()==LIST_CMD)
    {
      lists l=(lists)h->Data();
      res->rtyp=LIST_CMD;
      res->data=(void*)pcvPMulL(p,l);
      return FALSE;
    }
  }
  WerrorS("<poly>,<list> expected");
  return TRUE;
}

int pcvDeg(poly p)
{
  int d=0;
  for(int i=pVariables;i>=1;i--) d+=pGetExp(p,i);
  return d;
}

int pcvMinDeg(poly p)
{
  if(!p) return -1;
  int md=pcvDeg(p);
  pIter(p);
  while(p)
  {
    int d=pcvDeg(p);
    if(d<md) md=d;
    pIter(p);
  }
  return md;
}

int pcvMinDeg(matrix m)
{
  int i,j,d;
  int md=-1;
  for(i=1;i<=MATROWS(m);i++)
  {
    for(j=1;j<=MATCOLS(m);j++)
    {
      d=pcvMinDeg(MATELEM(m,i,j));
      if((d>=0&&md>d)||md==-1) md=d;
    }
  }
  return(md);
}

BOOLEAN pcvMinDeg(leftv res,leftv h)
{
  if(h)
  {
    if(h->Typ()==POLY_CMD)
    {
      res->rtyp=INT_CMD;
      res->data=(void*)pcvMinDeg((poly)h->Data());
      return FALSE;
    }
    else
    if(h->Typ()==MATRIX_CMD)
    {
      res->rtyp=INT_CMD;
      res->data=(void*)pcvMinDeg((matrix)h->Data());
      return FALSE;
    }
  }
  WerrorS("<poly> expected");
  return TRUE;
}

void pcvInit(int d)
{
  if(d<0) d=1;
  pcvMaxDegree=d+1;
  pcvTableSize=pVariables*pcvMaxDegree*sizeof(unsigned);
  pcvTable=(unsigned*)omAlloc0(pcvTableSize);
  pcvIndexSize=pVariables*sizeof(unsigned*);
  pcvIndex=(unsigned**)omAlloc(pcvIndexSize);
  for(int i=0;i<pVariables;i++)
    pcvIndex[i]=pcvTable+i*pcvMaxDegree;
  for(int i=0;i<pcvMaxDegree;i++)
    pcvIndex[0][i]=i;
  unsigned k,l;
  for(int i=1;i<pVariables;i++)
  {
    k=0;
    for(int j=0;j<pcvMaxDegree;j++)
    {
      l=pcvIndex[i-1][j];
      if(l>unsigned(~0)-k)
      {
        j=pcvMaxDegree;
        i=pVariables;
        WerrorS("unsigned overflow");
      }
      else pcvIndex[i][j]=k+=l;
    }
  }
}

void pcvClean()
{
  if(pcvTable)
  {
    omFreeSize(pcvTable,pcvTableSize);
    pcvTable=NULL;
  }
  if(pcvIndex)
  {
    omFreeSize(pcvIndex,pcvIndexSize);
    pcvIndex=NULL;
  }
}

int pcvM2N(poly m)
{
  unsigned n=0,dn,d=0;
  for(int i=0;i<pVariables;i++)
  {
    d+=pGetExp(m,i+1);
    dn=pcvIndex[i][d];
    if(dn>INT_MAX-n)
    {
      i=pVariables;
      WerrorS("component overflow");
    }
    else n+=dn;
  }
  return n+1;
}

poly pcvN2M(int n)
{
  n--;
  poly m=pOne();
  int i,j,k;
  for(i=pVariables-1;i>=0;i--)
  {
    k=j;
    for(j=0; (j<pcvMaxDegree) && (pcvIndex[i][j]<=(unsigned)n); j++);
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
    pDeleteLm(&m);
    return NULL;
  }
}

poly pcvP2CV(poly p,int d0,int d1)
{
  poly cv=NULL;
  while(p)
  {
    int d=pcvDeg(p);
    if(d0<=d&&d<d1)
    {
      poly c=pNSet(nCopy(pGetCoeff(p)));
      pSetComp(c,pcvM2N(p));
      cv=pAdd(cv,c);
    }
    pIter(p);
  }
  return cv;
}

poly pcvCV2P(poly cv,int d0,int d1)
{
  poly p=NULL;
  while(cv)
  {
    poly m=pcvN2M(pGetComp(cv));
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

lists pcvP2CV(lists pl,int d0,int d1)
{
  lists cvl=(lists)omAllocBin(slists_bin);
  cvl->Init(pl->nr+1);
  pcvInit(d1);
  for(int i=pl->nr;i>=0;i--)
  {
    if(pl->m[i].rtyp==POLY_CMD)
    {
      cvl->m[i].rtyp=VECTOR_CMD;
      cvl->m[i].data=pcvP2CV((poly)pl->m[i].data,d0,d1);
    }
  }
  pcvClean();
  return cvl;
}

lists pcvCV2P(lists cvl,int d0,int d1)
{
  lists pl=(lists)omAllocBin(slists_bin);
  pl->Init(cvl->nr+1);
  pcvInit(d1);
  for(int i=cvl->nr;i>=0;i--)
  {
    if(cvl->m[i].rtyp==VECTOR_CMD)
    {
      pl->m[i].rtyp=POLY_CMD;
      pl->m[i].data=pcvCV2P((poly)cvl->m[i].data,d0,d1);
    }
  }
  pcvClean();
  return pl;
}

BOOLEAN pcvP2CV(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==LIST_CMD)
    {
      lists p=(lists)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d0=(int)(long)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int d1=(int)(long)h->Data();
          res->rtyp=LIST_CMD;
          res->data=(void*)pcvP2CV(p,d0,d1);
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

BOOLEAN pcvCV2P(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==LIST_CMD)
    {
      lists pl=(lists)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d0=(int)(long)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int d1=(int)(long)h->Data();
          res->rtyp=LIST_CMD;
          res->data=(void*)pcvCV2P(pl,d0,d1);
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
  pcvInit(d1);
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
      int d0=(int)(long)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d1=(int)(long)h->Data();
        res->rtyp=INT_CMD;
        res->data=(void*)pcvDim(d0,d1);
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
  lists b=(lists)omAllocBin(slists_bin);
  b->Init(pcvDim(d0,d1));
  poly m=pOne();
  for(int d=d0,i=0;d<d1;d++)
    i=pcvBasis(b,i,m,d,1);
  pDeleteLm(&m);
  return b;
}

BOOLEAN pcvBasis(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==INT_CMD)
    {
      int d0=(int)(long)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int d1=(int)(long)h->Data();
        res->rtyp=LIST_CMD;
        res->data=(void*)pcvBasis(d0,d1);
        return FALSE;
      }
    }
    WerrorS("<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}
