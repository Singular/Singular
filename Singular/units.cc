/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: units.cc,v 1.6 2001-02-08 13:13:05 Singular Exp $ */
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

matrix invunit(int n,matrix U)
{
  assume(MATCOLS(U)==MATROWS(U));
  for(int i=MATCOLS(U);i>=1;i--)
    MATELEM(U,i,i)=pInvers(n,MATELEM(U,i,i));
  return U;
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
