/*!
!
****************************************
*  Computer Algebra System SINGULAR      *
****************************************

*/
/*!
!

* ABSTRACT: Gauss-Manin system normal form


*/




#include "kernel/mod2.h"

#ifdef HAVE_GMS

#include "gms.h"

#include "coeffs/numbers.h"
#include "kernel/polys.h"

#include "ipid.h"

lists gmsNF(ideal p,ideal g,matrix B,int D,int K)
{
  ideal r=idInit(IDELEMS(p),1);
  ideal q=idInit(IDELEMS(p),1);

  matrix B0=mpNew(MATROWS(B),MATCOLS(B));
  for(int i=1;i<=MATROWS(B0);i++)
     for(int j=1;j<=MATCOLS(B0);j++)
      if(MATELEM(B,i,j)!=NULL)
        MATELEM(B0,i,j)=pDiff(MATELEM(B,i,j),i+1);

  for(int k=0;k<IDELEMS(p);k++)
  {
    while(p->m[k]!=NULL&&pGetExp(p->m[k],1)<=K)
    {
      int j=0;
      while(j<IDELEMS(g)&&!pLmDivisibleBy(g->m[j],p->m[k]))
        j++;

      if(j<IDELEMS(g))
      {
        poly m=pDivideM(pHead(p->m[k]),pHead(g->m[j]));
        p->m[k]=pSub(p->m[k],ppMult_mm(g->m[j],m));
        pIncrExp(m,1);
        pSetm(m);
        for(int i=0;i<MATROWS(B);i++)
        {
          poly m0=pDiff(m,i+2);
          if(MATELEM(B0,i+1,j+1)!=NULL)
            p->m[k]=pAdd(p->m[k],ppMult_mm(MATELEM(B0,i+1,j+1),m));
          if(MATELEM(B,i+1,j+1)!=NULL&&m0!=NULL)
            p->m[k]=pAdd(p->m[k],ppMult_mm(MATELEM(B,i+1,j+1),m0));
          pDelete(&m0);
        }
        pDelete(&m);
      }
      else
      {
        poly p0=p->m[k];
        pIter(p->m[k]);
        pNext(p0)=NULL;
        r->m[k]=pAdd(r->m[k],p0);
      }

      while(p->m[k]!=NULL&&pGetExp(p->m[k],1)<=K&&pWTotaldegree(p->m[k])>D)
      {
        int i=pGetExp(p->m[k],1);
        do
        {
          poly p0=p->m[k];
          pIter(p->m[k]);
          pNext(p0)=NULL;
          q->m[k]=pAdd(q->m[k],p0);
        }while(p->m[k]!=NULL&&pGetExp(p->m[k],1)==i);
      }

      pNormalize(p->m[k]);
    }

    q->m[k]=pAdd(q->m[k],p->m[k]);
    p->m[k]=NULL;
  }
  idDelete(&p);
  idDelete((ideal *)&B0);

  id_Normalize(r, currRing);
  id_Normalize(q, currRing);

  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);

  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=r;
  l->m[1].rtyp=IDEAL_CMD;
  l->m[1].data=q;

  return l;
}


BOOLEAN gmsNF(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==IDEAL_CMD)
    {
      ideal p=(ideal)h->CopyD();
      h=h->next;
      if(h&&h->Typ()==IDEAL_CMD)
      {
        ideal g=(ideal)h->Data();
        h=h->next;
        if(h&&h->Typ()==MATRIX_CMD)
        {
          matrix B=(matrix)h->Data();
          h=h->next;
          if(h&&h->Typ()==INT_CMD)
          {
            int D=(int)(long)h->Data();
            h=h->next;
            if(h&&h->Typ()==INT_CMD)
            {
              int K=(int)(long)h->Data();
              res->rtyp=LIST_CMD;
              res->data=(void *)gmsNF(p,g,B,D,K);
              return FALSE;
            }
          }
        }
      }
    }
    WerrorS("<ideal>,<ideal>,<matrix>,<int>,<int> expected");
    return TRUE;
  }
  WerrorS("no ring active");
  return TRUE;
}

#endif /*!
!
 HAVE_GMS 

*/

