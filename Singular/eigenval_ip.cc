/*!
!
****************************************
*  Computer Algebra System SINGULAR      *
****************************************

*/
/*!
!

* ABSTRACT: eigenvalues of constant square matrices


*/




#include "kernel/mod2.h"

#ifdef HAVE_EIGENVAL

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "misc/intvec.h"
#include "coeffs/numbers.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "Singular/lists.h"
#include "polys/matpol.h"
#include "polys/clapsing.h"
#include "kernel/linear_algebra/eigenval.h"
#include "Singular/ipshell.h"
#include "Singular/eigenval_ip.h"


BOOLEAN evSwap(leftv res,leftv h)
{
  if(currRing)
  {
    const short t[]={3,MATRIX_CMD,INT_CMD,INT_CMD};
    if (iiCheckTypes(h,t,1))
    {
      matrix M=(matrix)h->Data();
      h=h->next;
      int i=(int)(long)h->Data();
      h=h->next;
      int j=(int)(long)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void *)evSwap(mp_Copy(M, currRing),i,j);
      return FALSE;
    }
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

BOOLEAN evRowElim(leftv res,leftv h)
{
  if(currRing)
  {
    const short t[]={4,MATRIX_CMD,INT_CMD,INT_CMD,INT_CMD};
    if (iiCheckTypes(h,t,1))
    {
      matrix M=(matrix)h->CopyD();
      h=h->next;
      int i=(int)(long)h->Data();
      h=h->next;
      int j=(int)(long)h->Data();
      h=h->next;
      int k=(int)(long)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void *)evRowElim(M,i,j,k);
      return FALSE;
    }
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

BOOLEAN evColElim(leftv res,leftv h)
{
  if(currRing)
  {
    const short t[]={4,MATRIX_CMD,INT_CMD,INT_CMD,INT_CMD};
    if (iiCheckTypes(h,t,1))
    {
      matrix M=(matrix)h->Data();
      h=h->next;
      int i=(int)(long)h->Data();
      h=h->next;
      int j=(int)(long)h->Data();
      h=h->next;
      int k=(int)(long)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void *)evColElim(mp_Copy(M, currRing),i,j,k);
      return FALSE;
    }
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

BOOLEAN evHessenberg(leftv res,leftv h)
{
  if(currRing)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->Data();
      res->rtyp=MATRIX_CMD;
      res->data=(void *)evHessenberg(mp_Copy(M, currRing));
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

  M=evHessenberg(M);

  int n=MATCOLS(M);
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
      ideal e0=singclap_factorize(mp_DetBareiss(M0,currRing),&m0,2, currRing);
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
          number e1=nCopy(pGetCoeff(e0->m[i]));
          e1=nInpNeg(e1);
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
  if(currRing)
  {
    if(h&&h->Typ()==MATRIX_CMD)
    {
      matrix M=(matrix)h->CopyD();
      res->rtyp=LIST_CMD;
      res->data=(void *)evEigenvals(M);
      return FALSE;
    }
    WerrorS("<matrix> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

#endif /*!
!
 HAVE_EIGENVAL 

*/
