/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: eigenval.cc,v 1.7 2002-02-16 13:49:03 mschulze Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#include "mod2.h"

#ifdef HAVE_EIGENVAL

#include "febase.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "lists.h"
#include "matpol.h"
#include "clapsing.h"
#include "eigenval.h"


matrix evSwap(matrix M,int i,int j)
{
  if(i==j)
    return(M);

  for(int k=1;k<=MATROWS(M);k++)
  {
    poly p=MATELEM(M,i,k);
    MATELEM(M,i,k)=MATELEM(M,j,k);
    MATELEM(M,j,k)=p;
  }

  for(int k=1;k<=MATCOLS(M);k++)
  {
    poly p=MATELEM(M,k,i);
    MATELEM(M,k,i)=MATELEM(M,k,j);
    MATELEM(M,k,j)=p;
  }

  return(M);
}


BOOLEAN evSwap(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int i=(int)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)h->Data();
          res->rtyp=MATRIX_CMD;
          res->data=(void*)evSwap(mpCopy(M),i,j);
          return FALSE;
        }
      }
    }
    WerrorS("<matrix>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}


matrix evRowElim(matrix M,int i,int j,int k)
{
  if(MATELEM(M,i,k)==NULL||MATELEM(M,j,k)==NULL)
    return(M);

  poly p=pNSet(nDiv(pGetCoeff(MATELEM(M,i,k)),pGetCoeff(MATELEM(M,j,k))));

  for(int l=1;l<=MATCOLS(M);l++)
    MATELEM(M,i,l)=pSub(MATELEM(M,i,l),pMult(pCopy(p),pCopy(MATELEM(M,j,l))));

  for(int l=1;l<=MATROWS(M);l++)
    MATELEM(M,l,j)=pAdd(MATELEM(M,l,j),pMult(pCopy(p),pCopy(MATELEM(M,l,i))));

  pDelete(&p);

  return(M);
}


BOOLEAN evRowElim(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int i=(int)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)h->Data();
          h=h->next;
          if(h&&h->Typ()==INT_CMD)
          {
            int k=(int)h->Data();
            res->rtyp=MATRIX_CMD;
            res->data=(void*)evRowElim(mpCopy(M),i,j,k);
            return FALSE;
	  }
        }
      }
    }
    WerrorS("<matrix>,<int>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}


matrix evColElim(matrix M,int i,int j,int k)
{
  if(MATELEM(M,k,i)==0||MATELEM(M,k,j)==0)
    return(M);

  poly p=pNSet(nDiv(pGetCoeff(MATELEM(M,k,i)),pGetCoeff(MATELEM(M,k,j))));

  for(int l=1;l<=MATROWS(M);l++)
    MATELEM(M,l,i)=pSub(MATELEM(M,l,i),pMult(pCopy(p),pCopy(MATELEM(M,l,j))));

  for(int l=1;l<=MATCOLS(M);l++)
    MATELEM(M,j,l)=pAdd(MATELEM(M,j,l),pMult(pCopy(p),pCopy(MATELEM(M,i,l))));

  pDelete(&p);

  return(M);
}


BOOLEAN evColElim(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      h=h->next;
      if(h&&h->Typ()==INT_CMD)
      {
        int i=(int)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)h->Data();
          h=h->next;
          if(h&&h->Typ()==INT_CMD)
          {
            int k=(int)h->Data();
            res->rtyp=MATRIX_CMD;
            res->data=(void*)evColElim(mpCopy(M),i,j,k);
            return FALSE;
	  }
        }
      }
    }
    WerrorS("<matrix>,<int>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}


matrix evHessenberg(matrix M)
{
  int n=MATROWS(M);
  int i,j;

  for(int k=1;k<n-1;k++)
  {
    j=k+1;
    while(j<n&&MATELEM(M,j,k)==0)
      j++;

    if(MATELEM(M,j,k)!=0)
    {
      M=evSwap(M,j,k+1);

      for(i=j+1;i<=n;i++)
        M=evRowElim(M,i,k+1,k);
    }
  }

  return(M);
}


BOOLEAN evHessenberg(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void*)evHessenberg(mpCopy(M));
      return FALSE;
    }
    WerrorS("<matrix> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}


lists evEigenvalue(matrix M)
{
  lists l=(lists)omAllocBin(slists_bin);
  if(MATROWS(M)!=MATCOLS(M))
  {
    l->Init(0);
    return(l);
  }

  M=evHessenberg(M);

  int n=MATROWS(M);
  ideal e=idInit(n,1);
  intvec *m=new intvec(n);

  poly t=pOne();
  pSetExp(t,1,1);
  pSetm(t);

  for(int j0=1,j=2,k=0;j<=n+1;j0=j,j++)
  {
    while(j<=n&&MATELEM(M,j,j-1)!=NULL)
      j++;
    if(j==j0+1)
    {
      e->m[k]=pHead(MATELEM(M,j0,j0));
      (*m)[k]=1;
      k++;
    }
    else
    {
      int n0=j-j0;
      matrix M0=mpNew(n0,n0);

      j0--;
      for(int i=1;i<=n0;i++)
        for(int j=1;j<=n0;j++)
          MATELEM(M0,i,j)=pCopy(MATELEM(M,j0+i,j0+j));
      for(int i=1;i<=n0;i++)
        MATELEM(M0,i,i)=pSub(MATELEM(M0,i,i),pCopy(t));

      intvec *m0;
      ideal e0=singclap_factorize(mpDetBareiss(M0),&m0,2);

      for(int i=0;i<IDELEMS(e0);i++)
      {
        number e1=nNeg(pGetCoeff(e0->m[i]));
        pDeleteLm(&e0->m[i]);
        if(pGetExp(e0->m[i],1)==0)
          e->m[k]=pNSet(nDiv(pGetCoeff(e0->m[i]),e1));
        else
	  e->m[k]=pNSet(nDiv(e1,pGetCoeff(e0->m[i])));
        nDelete(&e1);
        (*m)[k]=(*m0)[i];
        k++;
      }

      delete(m0);
      idDelete(&e0);
    }
  }

  pDelete(&t);
  idDelete((ideal *)&M);

  for(int i=0;i<n-1;i++)
  {
    if(e->m[i]!=NULL)
    for(int j=i+1;j<n;j++)
    {
      if(e->m[j]!=NULL)
      if(nEqual(pGetCoeff(e->m[i]),pGetCoeff(e->m[j])))
      {
        (*m)[i]+=(*m)[j];
        (*m)[j]=0;
      }
      else
      if(nGreater(pGetCoeff(e->m[i]),pGetCoeff(e->m[j])))
      {
        poly p=e->m[i];
        e->m[i]=e->m[j];
        e->m[j]=p;
        int k=(*m)[i];
        (*m)[i]=(*m)[j];
        (*m)[j]=k;
      }
    }
  }

  int n0=0;
  for(int i=0;i<n;i++)
    if((*m)[i]>0)
      n0++;

  ideal e0=idInit(n0,1);
  intvec *m0=new intvec(n0);

  for(int i=0,i0=0;i<n;i++)
    if((*m)[i]>0)
    {
      e0->m[i0]=e->m[i];
      e->m[i]=NULL;
      (*m0)[i0]=(*m)[i];
      i0++;
    }

  idDelete(&e);
  delete(m);

  l->Init(2);
  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=e0;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[1].data=m0;

  return(l);
}


BOOLEAN evEigenvalue(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      res->rtyp=LIST_CMD;
      res->data=(void*)evEigenvalue(mpCopy(M));
      return FALSE;
    }
    WerrorS("<matrix> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

#endif /* HAVE_EIGENVAL */
