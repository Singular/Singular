/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: units.cc,v 1.2 2001-02-01 15:14:21 mschulze Exp $ */
/*
* ABSTRACT: procedures to compute with units
*/

#include <mod2.h>
#include <tok.h>
#include <ipid.h>
#include <febase.h>
#include <numbers.h>
#include <polys.h>
#include <ideals.h>
#include <matpol.h>
#include <kstd1.h>
#include <units.h>

BOOLEAN isunit(poly u)
{
  if(u==NULL||pTotaldegree(u)>0)
    return FALSE;
  return TRUE;
}

BOOLEAN isunit(matrix U)
{
  if(MATROWS(U)!=MATCOLS(U))
    return FALSE;
  for(int i=MATCOLS(U);i>=1;i--)
    if(!isunit(MATELEM(U,i,i)))
      return FALSE;
  return TRUE;
}

BOOLEAN invunit(leftv res,leftv h)
{
  if(h!=NULL)
  {
    if(h->Typ()==POLY_CMD)
    {
      poly u=(poly)h->Data();
      if(!isunit(u))
      {
        WerrorS("unit expected");
        return TRUE;
      }
      h=h->next;
      if(h!=NULL&&h->Typ()==INT_CMD)
      {
        int n=(int)h->Data();
        res->rtyp=POLY_CMD;
        res->data=(void*)invunit(pCopy(u),n);
        return FALSE;
      }
    }
    if(h->Typ()==MATRIX_CMD)
    {
      matrix U=(matrix)h->Data();
      if(!isunit(U))
      {
        WerrorS("diagonal matrix of units expected");
        return TRUE;
      }
      h=h->next;
      if(h!=NULL&&h->Typ()==INT_CMD)
      {
        int n=(int)h->Data();
        res->rtyp=MATRIX_CMD;
        res->data=(void*)invunit(mpCopy(U),n);
        return FALSE;
      }
    }
  }
  WerrorS("[<poly>|<matrix>],<int> expected");
  return TRUE;
}

poly pjet(poly p,int n)
{
  poly p0=pJet(p,n);
  pDelete(&p);
  return p0;
}

poly invunit(poly u, int n)
{
  if(n<0)
    return NULL;
  number u0=nInvers(pGetCoeff(u));
  poly v=pOne();
  pSetCoeff(v,u0);
  if(n==0)
    return v;
  poly u1=pjet(pSub(pOne(),pMult_nn(u,u0)),n);
  if(u1==NULL)
    return v;
  poly v1=pMult_nn(pCopy(u1),u0);
  v=pAdd(v,pCopy(v1));
  for(int i=n/pTotaldegree(u1);i>1;i--)
  {
    v1=pjet(pMult(v1,pCopy(u1)),n);
    v=pAdd(v,pCopy(v1));
  }
  pDelete(&u1);
  pDelete(&v1);
  return v;
}

matrix invunit(matrix U,int n)
{
  for(int i=MATCOLS(U);i>=1;i--)
    MATELEM(U,i,i)=invunit(MATELEM(U,i,i),n);
  return U;
}

BOOLEAN series(leftv res,leftv h)
{
  if(h!=NULL)
  {
    if(h->Typ()==POLY_CMD||h->Typ()==VECTOR_CMD)
    {
      int typ=h->Typ();
      poly p=(poly)h->Data();
      h=h->next;
      if(h!=NULL&&h->Typ()==POLY_CMD)
      {
        poly u=(poly)h->Data();
        if(!isunit(u))
        {
          WerrorS("unit expected");
          return TRUE;
        }
        h=h->next;
        if(h!=NULL&&h->Typ()==INT_CMD)
        {
          int n=(int)h->Data();
          res->rtyp=typ;
          res->data=(void*)series(pCopy(p),pCopy(u),n);
          return FALSE;
        }
      }
    }
    if(h->Typ()==IDEAL_CMD||h->Typ()==MODUL_CMD)
    {
      int typ=h->Typ();
      ideal M=(ideal)h->Data();
      h=h->next;
      if(h!=NULL&&h->Typ()==MATRIX_CMD)
      {
        matrix U=(matrix)h->Data();
        if(!isunit(U))
        {
          WerrorS("diagonal matrix of units expected");
          return TRUE;
        }
        if(IDELEMS(M)!=MATROWS(U))
        {
          WerrorS("incompatible matrix size");
          return TRUE;
        }
        h=h->next;
        if(h!=NULL&&h->Typ()==INT_CMD)
        {
          int n=(int)h->Data();
          res->rtyp=typ;
          res->data=(void*)series(idCopy(M),mpCopy(U),n);
          return FALSE;
        }
      }
    }
  }
  WerrorS("[<poly>,<poly>|<ideal>,<matrix>],<int> expected");
  return TRUE;
}

poly series(poly p,poly u,int n)
{
  if(p!=NULL)
    p=pjet(pMult(p,invunit(u,n-pTotaldegree(p))),n);
  return p;
}

ideal series(ideal M,matrix U,int n)
{
  for(int i=IDELEMS(M)-1;i>=0;i--)
    M->m[i]=series(M->m[i],pCopy(MATELEM(U,i+1,i+1)),n);
  idDelete((ideal*)&U);
  return M;
}

BOOLEAN rednf(leftv res,leftv h)
{
  if(h!=NULL)
  {
    if(h->Typ()==IDEAL_CMD||h->Typ()==MODUL_CMD)
    {
      int typ=h->Typ();
      ideal N=(ideal)h->Data();
      h=h->next;
      if(h!=NULL&&h->Typ()==typ)
      {
        ideal M=(ideal)h->Data();
        h=h->next;
        if(h==NULL)
        {
          res->rtyp=typ;
          res->data=(void*)rednf(idCopy(N),idCopy(M));
          return FALSE;
        }
        if(h->Typ()==MATRIX_CMD)
        {
          matrix U=(matrix)h->Data();
          if(!isunit(U))
          {
            WerrorS("diagonal matrix of units expected");
            return TRUE;
          }
          if(IDELEMS(M)!=MATROWS(U))
          {
            WerrorS("incompatible matrix size");
            return TRUE;
          }
          res->rtyp=typ;
          res->data=(void*)rednf(idCopy(N),idCopy(M),mpCopy(U));
          return FALSE;
        }
      }
      if(typ==IDEAL_CMD&&h->Typ()==POLY_CMD||
         typ==MODUL_CMD&&h->Typ()==VECTOR_CMD)
      {
        typ=h->Typ();
        poly p=(poly)h->Data();
        h=h->next;
        if(h==NULL)
        {
          res->rtyp=typ;
          res->data=(void*)rednf(idCopy(N),pCopy(p));
          return FALSE;
        }
        if(h->Typ()==POLY_CMD)
        {
          poly u=(poly)h->Data();
          if(!isunit(u))
          {
            WerrorS("unit expected");
            return TRUE;
          }
          res->rtyp=typ;
          res->data=(void*)rednf(idCopy(N),pCopy(p),pCopy(u));
          return FALSE;
	}
      }
    }
  }
  WerrorS("<ideal>,[<ideal>|<poly>][,<matrix>] expected");
  return TRUE;
}

ideal rednf(ideal N,ideal M,matrix U=NULL)
{
  matrix U0=NULL;
  if(U!=NULL)
  {
    U0=mpCopy(U);
    number u0;
    for(int i=IDELEMS(M)-1;i>=0;i--)
    {
      u0=nInvers(pGetCoeff(MATELEM(U0,i+1,i+1)));
      MATELEM(U0,i+1,i+1)=pMult_nn(MATELEM(U0,i+1,i+1),u0);
      M->m[i]=pMult_nn(M->m[i],u0);
    }
  }
  ideal M0=idInit(IDELEMS(M),M->rank);
  ideal M1=kNF(N,currQuotient,M);
  while(idElem(M1)>0)
  {
    for(int i=IDELEMS(M)-1;i>=0;i--)
    {
      M0->m[i]=pAdd(M0->m[i],pHead(pCopy(M1->m[i])));
      if(U0!=NULL)
        M->m[i]=pSub(M->m[i],pMult(pHead(pCopy(M1->m[i])),
                                   pCopy(MATELEM(U0,i+1,i+1))));
      else
        M->m[i]=pSub(M->m[i],pHead(pCopy(M1->m[i])));
    }
    idDelete(&M1);
    M1=kNF(N,currQuotient,M);
  }
  idDelete(&M1);
  idDelete(&N);
  idDelete(&M);
  if(U0!=NULL)
    idDelete((ideal*)&U0);
  return M0;
}

poly rednf(ideal N,poly p,poly u=NULL)
{
  ideal M=idInit(1,pGetComp(p));
  M->m[0]=p;
  ideal M0;
  if(u==NULL)
    M0=rednf(N,M);
  else
  {
    matrix U=mpInitI(1,1,NULL);
    MATELEM(U,1,1)=u;
    M0=rednf(N,M,U);
  }
  poly p0=M0->m[0];
  M0->m[0]=NULL;
  idDelete(&M0);
  return p0;
}
