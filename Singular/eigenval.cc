/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval.cc,v 1.5 2002-01-19 14:48:14 obachman Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#include "mod2.h"

#ifdef HAVE_EIGENVAL

#include "tok.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "matpol.h"
#include "clapsing.h"

matrix swap(matrix M,int i,int j)
{
  if(i==j)
    return M;
  poly p;
  for(int k=1;k<=MATROWS(M);k++)
  {
    p=MATELEM(M,i,k);
    MATELEM(M,i,k)=MATELEM(M,j,k);
    MATELEM(M,j,k)=p;
  }
  for(int k=1;k<=MATCOLS(M);k++)
  {
    p=MATELEM(M,k,i);
    MATELEM(M,k,i)=MATELEM(M,k,j);
    MATELEM(M,k,j)=p;
  }
  return(M);
}

matrix rowelim(matrix M,int i, int j, int k)
{
  if(MATELEM(M,i,k)==NULL||MATELEM(M,j,k)==NULL)
    return M;
  number n=nDiv(pGetCoeff(MATELEM(M,i,k)),
                pGetCoeff(MATELEM(M,j,k)));
  for(int l=1;l<=MATCOLS(M);l++)
  {
    MATELEM(M,i,l)=pSub(MATELEM(M,i,l),pMult_nn(pCopy(MATELEM(M,j,l)),n));
  }
  for(int l=1;l<=MATROWS(M);l++)
  {
    MATELEM(M,l,j)=pAdd(MATELEM(M,l,j),pMult_nn(pCopy(MATELEM(M,l,i)),n));
  }
  nDelete(&n);
  return M;
}

matrix colelim(matrix M,int i, int j, int k)
{
  if(MATELEM(M,k,i)==NULL||MATELEM(M,k,j)==NULL)
    return M;
  number n=nDiv(pGetCoeff(MATELEM(M,k,i)),
                pGetCoeff(MATELEM(M,k,j)));
  for(int l=1;l<=MATROWS(M);l++)
  {
    MATELEM(M,l,i)=pSub(MATELEM(M,l,i),pMult_nn(pCopy(MATELEM(M,l,j)),n));
  }
  for(int l=1;l<=MATCOLS(M);l++)
  {
    MATELEM(M,j,l)=pAdd(MATELEM(M,j,l),pMult_nn(pCopy(MATELEM(M,i,l)),n));
  }
  nDelete(&n);
  return M;
}

matrix tridiag(matrix M)
{
  int n=MATCOLS(M);
  for(int k=1;k<=n-2;k++)
  {
    int j=k+1;
    while(j<=n&&MATELEM(M,j,k)==NULL) j++;
    if(j<=n)
    {
      for(int i=j+1;i<=n;i++)
      {
        M=rowelim(M,i,j,k);
      }
      M=swap(M,j,k+1);
    }
    j=k+1;
    while(j<=n&&MATELEM(M,k,j)==NULL) j++;
    if(j<=n)
    {
      for(int i=j+1;i<=n;i++)
      {
        M=colelim(M,i,j,k);
      }
      M=swap(M,j,k+1);
    }
  }
  return(M);
}

lists addval(lists l,poly e0,int m0)
{
  ideal ee=(ideal)l->m[0].data;
  intvec *mm=(intvec*)l->m[1].data;
  int n=0;
  if(ee!=NULL)
    n=IDELEMS(ee);
  for(int i=n-1;i>=0;i--)
  {
    if(pEqualPolys(ee->m[i],e0))
    {
      (*mm)[i]+=m0;
      return l;
    }
  }
  ideal e=idInit(n+1,1);
  for(int i=n-1;i>=0;i--)
  {
    e->m[i]=ee->m[i];
    ee->m[i]=NULL;
  }
  e->m[n]=e0;
  l->m[0].data=e;
  if(ee!=NULL)
    idDelete(&ee);
  mm->resize(n+1);
  (*mm)[n]=m0;
  return l;
}

lists sortval(lists l)
{
  ideal ee=(ideal)l->m[0].data;
  intvec *mm=(intvec*)l->m[1].data;
  int n=IDELEMS(ee);
  for(int i=n-1;i>=1;i--)
    for(int j=i-1;j>=0;j--)
      if(nGreater(pGetCoeff(ee->m[j]),pGetCoeff(ee->m[i])))
      {
        poly e=ee->m[i];
        ee->m[i]=ee->m[j];
        ee->m[j]=e;
        int m=(*mm)[i];
        (*mm)[i]=(*mm)[j];
        (*mm)[j]=m;
      }
  return l;
}

lists eigenval(matrix M)
{
  M=tridiag(M);
  int n=MATCOLS(M);
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=NULL;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[1].data=new intvec;
  int j=1;
  while(j<=n)
  {
    while(j<n&&(MATELEM(M,j,j+1)==NULL||MATELEM(M,j+1,j)==NULL)||j==n)
    {
      l=addval(l,MATELEM(M,j,j),1);
      MATELEM(M,j,j)=NULL;
      j++;
    }
    if(j<n)
    {
      poly t=pOne();
      pSetExp(t,1,1);
      pSetm(t);
      poly d0=pSub(MATELEM(M,j,j),t);
      MATELEM(M,j,j)=NULL;
      j++;
      poly d1=pOne();
      poly d2=NULL;
      while(j<=n&&MATELEM(M,j,j-1)!=NULL&&MATELEM(M,j-1,j)!=NULL)
      {
        d2=d1;
        d1=pCopy(d0);
        poly t=pOne();
        pSetExp(t,1,1);
        pSetm(t);
        d0=pSub(pMult(d0,pSub(MATELEM(M,j,j),t)),
                pMult(pMult(d2,MATELEM(M,j-1,j)),MATELEM(M,j,j-1)));
        MATELEM(M,j,j)=NULL;
        MATELEM(M,j-1,j)=NULL;
        MATELEM(M,j,j-1)=NULL;
        j++;
      }
      pDelete(&d1);
      intvec *m0=NULL;
#ifdef HAVE_FACTORY
      ideal e0=singclap_factorize(d0,&m0,0);
#else 
      ideal e0 = NULL;
#endif
      pDelete(&d0);
      for(int i=IDELEMS(e0)-1;i>=1;i--)
      {
        poly p=e0->m[i];
        e0->m[i]=NULL;
        poly p0,p1;
        poly pp=p;
        while(pp!=NULL&&pGetExp(pp,1)<=1)
        {
          if(pGetExp(pp,1)==0)
            p0=pp;
          else
          if(pGetExp(pp,1)==1)
            p1=pp;
          pp=pNext(pp);
        }
        if(pp==NULL)
        {
          pp=p;
          p=pNSet(nNeg(nDiv(pGetCoeff(p0),pGetCoeff(p1))));
          pDelete(&pp);
        }
        else
        {
          p=pMult_nn(p,pGetCoeff(e0->m[0]));
        }
        l=addval(l,p,(*m0)[i]);
      }
      delete m0;
      idDelete(&e0);
    }
  }
  idDelete((ideal*)&M);
  return sortval(l);
}

BOOLEAN tridiag(leftv res,leftv h)
{
  if((h!=NULL) && (h->Typ()==MATRIX_CMD))
  {
    matrix M=(matrix)h->Data();
    if(MATCOLS(M)!=MATROWS(M))
    {
      WerrorS("square matrix expected");
    }
    res->rtyp=MATRIX_CMD;
    res->data=(void*)tridiag(mpCopy(M));
    return FALSE;
  }
  WerrorS("<matrix> expected");
  return TRUE;
}

BOOLEAN eigenval(leftv res,leftv h)
{
  if((h!=NULL) && (h->Typ()==MATRIX_CMD))
  {
    matrix M=(matrix)h->Data();
    if(MATCOLS(M)!=MATROWS(M))
    {
      WerrorS("square matrix expected");
    }
    res->rtyp=LIST_CMD;
    res->data=(void*)eigenval(mpCopy(M));
    return FALSE;
  }
  WerrorS("<matrix> expected");
  return TRUE;
}
#endif
