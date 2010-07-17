/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */
/*
* ABSTRACT: procedures to compute with units
*/

#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/intvec.h>
#include <kernel/matpol.h>
#include <kernel/kstd1.h>
#include <kernel/units.h>

ideal redNF(ideal N,ideal M,matrix U,int d,intvec *w)
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
  ideal M1=kNF(N,currQuotient,M,0,KSTD_NF_ECART);
  while(idElem(M1)>0&&(d==-1||idMinDegW(M1,w)<=d))
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
    M1=kNF(N,currQuotient,M,0,KSTD_NF_ECART);
  }
  idDelete(&M1);
  idDelete(&N);
  idDelete(&M);
  if(U0!=NULL)
    idDelete((ideal*)&U0);
  return M0;
}

poly redNF(ideal N,poly p,poly u,int d,intvec *w)
{
  ideal M=idInit(1,pGetComp(p));
  M->m[0]=p;
  ideal M0;
  if(u==NULL)
    M0=redNF(N,M,NULL,d,w);
  else
  {
    matrix U=mpNew(1,1);
    MATELEM(U,1,1)=u;
    M0=redNF(N,M,U,d,w);
    idDelete((ideal*)&U);
  }
  poly p0=M0->m[0];
  M0->m[0]=NULL;
  idDelete(&M0);
  return p0;
}

