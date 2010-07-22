/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#include <Singular/mod2.h>

#ifdef HAVE_EIGENVAL

#include <kernel/febase.h>
#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <kernel/intvec.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <Singular/lists.h>
#include <kernel/matpol.h>
#include <kernel/clapsing.h>
#include <kernel/eigenval.h>
#include <Singular/eigenval_ip.h>


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
        int i=(int)(long)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)(long)h->Data();
          res->rtyp=MATRIX_CMD;
          res->data=(void *)evSwap(mpCopy(M),i,j);
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
        int i=(int)(long)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)(long)h->Data();
          h=h->next;
          if(h&&h->Typ()==INT_CMD)
          {
            int k=(int)(long)h->Data();
            res->rtyp=MATRIX_CMD;
            res->data=(void *)evRowElim(mpCopy(M),i,j,k);
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
        int i=(int)(long)h->Data();
        h=h->next;
        if(h&&h->Typ()==INT_CMD)
        {
          int j=(int)(long)h->Data();
          h=h->next;
          if(h&&h->Typ()==INT_CMD)
          {
            int k=(int)(long)h->Data();
            res->rtyp=MATRIX_CMD;
            res->data=(void *)evColElim(mpCopy(M),i,j,k);
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

BOOLEAN evHessenberg(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void *)evHessenberg(mpCopy(M));
      return FALSE;
    }
    WerrorS("<matrix> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}


lists evEigenvals(matrix M)
{
  lists l=(lists)omAllocBin(slists_bin);
  if(MATROWS(M)!=MATCOLS(M))
  {
    l->Init(0);
    return(l);
  }

  M=evHessenberg((matrix)idJet((ideal)M,0));

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
      if (e0==NULL)
      {
        l->Init(0);
        return(l);
      }

      for(int i=0;i<IDELEMS(e0);i++)
      {
        if(pNext(e0->m[i])==NULL)
	{
          (*m)[k]=(*m0)[i];
          k++;
        }
        else
        if(pGetExp(e0->m[i],1)<2&&pGetExp(pNext(e0->m[i]),1)<2&&
           pNext(pNext(e0->m[i]))==NULL)
	{
          number e1=nNeg(nCopy(pGetCoeff(e0->m[i])));
          if(pGetExp(pNext(e0->m[i]),1)==0)
            e->m[k]=pNSet(nDiv(pGetCoeff(pNext(e0->m[i])),e1));
          else
	    e->m[k]=pNSet(nDiv(e1,pGetCoeff(pNext(e0->m[i]))));
          nDelete(&e1);
          pNormalize(e->m[k]);
          (*m)[k]=(*m0)[i];
          k++;
        }
        else
	{
          e->m[k]=e0->m[i];
          pNormalize(e->m[k]);
          e0->m[i]=NULL;
          (*m)[k]=(*m0)[i];
          k++;
	}
      }

      delete(m0);
      idDelete(&e0);
    }
  }

  pDelete(&t);
  idDelete((ideal *)&M);

  for(int i0=0;i0<n-1;i0++)
  {
    for(int i1=i0+1;i1<n;i1++)
    {
      if(pEqualPolys(e->m[i0],e->m[i1]))
      {
        (*m)[i0]+=(*m)[i1];
        (*m)[i1]=0;
      }
      else
      {
        if(e->m[i0]==NULL&&!nGreaterZero(pGetCoeff(e->m[i1]))||
           e->m[i1]==NULL&&
	  (nGreaterZero(pGetCoeff(e->m[i0]))||pNext(e->m[i0])!=NULL)||
           e->m[i0]!=NULL&&e->m[i1]!=NULL&&
          (pNext(e->m[i0])!=NULL&&pNext(e->m[i1])==NULL||
           pNext(e->m[i0])==NULL&&pNext(e->m[i1])==NULL&&
           nGreater(pGetCoeff(e->m[i0]),pGetCoeff(e->m[i1]))))
        {
          poly e1=e->m[i0];
          e->m[i0]=e->m[i1];
          e->m[i1]=e1;
          int m1=(*m)[i0];
          (*m)[i0]=(*m)[i1];
          (*m)[i1]=m1;
        }
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


BOOLEAN evEigenvals(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      res->rtyp=LIST_CMD;
      res->data=(void *)evEigenvals(mpCopy(M));
      return FALSE;
    }
    WerrorS("<matrix> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

#endif /* HAVE_EIGENVAL */
