/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: units.cc,v 1.5 2001-02-07 12:48:27 Singular Exp $ */
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
  return (u!=NULL) && pIsConstant(u);
//  if(u==NULL||pTotaldegree(u)>0)
//    return FALSE;
//  return TRUE;
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
  if(h!=NULL&&h->Typ()==INT_CMD)
  {
    int n=(int)h->Data();
    h=h->next;
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
        res->rtyp=POLY_CMD;
        res->data=(void*)invunit(n,pCopy(u));
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
        res->rtyp=MATRIX_CMD;
        res->data=(void*)invunit(n,mpCopy(U));
        return FALSE;
      }
    }
 }
  WerrorS("<int>,[<poly>|<matrix>] expected");
  return TRUE;
}

poly pjet(int n,poly p)
{
  poly p0=pJet(p,n);
  pDelete(&p);
  return p0;
}

poly invunit(int n,poly u)
{
  if(n<0)
    return NULL;
  number u0=nInvers(pGetCoeff(u));
  poly v=pNSet(u0);
  if(n==0)
    return v;
  poly u1=pjet(n,pSub(pOne(),pMult_nn(u,u0)));
  if(u1==NULL)
    return v;
  poly v1=pMult_nn(pCopy(u1),u0);
  v=pAdd(v,pCopy(v1));
  for(int i=n/pTotaldegree(u1);i>1;i--)
  {
    v1=pjet(n,pMult(v1,pCopy(u1)));
    v=pAdd(v,pCopy(v1));
  }
  pDelete(&u1);
  pDelete(&v1);
  return v;
}

matrix invunit(int n,matrix U)
{
  assume(MATCOLS(u)==MATROWS(U));
  for(int i=MATCOLS(U);i>=1;i--)
    MATELEM(U,i,i)=invunit(n,MATELEM(U,i,i));
  return U;
}

BOOLEAN series(leftv res,leftv h)
{
  if(h!=NULL&&h->Typ()==INT_CMD)
  {
    int n=(int)h->Data();
    h=h->next;
    if(h!=NULL)
    {
      if(h->Typ()==POLY_CMD||h->Typ()==VECTOR_CMD)
      {
        int typ=h->Typ();
        poly p=(poly)h->Data();
        h=h->next;
        if(h==NULL)
        {
          res->rtyp=typ;
          res->data=(void*)series(n,pCopy(p));
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
          res->data=(void*)series(n,pCopy(p),pCopy(u));
          return FALSE;
        }
      }
      if(h->Typ()==IDEAL_CMD||h->Typ()==MODUL_CMD)
      {
        int typ=h->Typ();
        ideal M=(ideal)h->Data();
        h=h->next;
        if(h==NULL)
        {
          res->rtyp=typ;
          res->data=(void*)series(n,idCopy(M));
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
          res->data=(void*)series(n,idCopy(M),mpCopy(U));
          return FALSE;
        }
      }
    }
  }
  WerrorS("<int>,[<poly>[,<poly>]|<ideal>[,<matrix>]] expected");
  return TRUE;
}

poly series(int n,poly p,poly u=NULL)
{
  if(p!=NULL)
    if(u==NULL)
      p=pjet(n,p);
    else
      p=pjet(n,pMult(p,invunit(n-pTotaldegree(p),u)));
  return p;
}

ideal series(int n,ideal M,matrix U=NULL)
{
  for(int i=IDELEMS(M)-1;i>=0;i--)
    if(U==NULL)
      M->m[i]=series(n,M->m[i]);
    else
    {
      M->m[i]=series(n,M->m[i],MATELEM(U,i+1,i+1));
      MATELEM(U,i+1,i+1)=NULL;
    }
  if(U!=NULL)
    idDelete((ideal*)&U);
  return M;
}

BOOLEAN rednf(leftv res,leftv h)
{
  if(h!=NULL)
  {
    assumeStdFlag(h);
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
  WerrorS("<ideal>,[<ideal>[,<matrix>]|<poly>[,<poly>]] expected");
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
    matrix U=mpNew(1,1);
    MATELEM(U,1,1)=u;
    M0=rednf(N,M,U);
    idDelete((ideal*)&U);
  }
  poly p0=M0->m[0];
  M0->m[0]=NULL;
  idDelete(&M0);
  return p0;
}
