/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: gms.cc,v 1.3 2002-02-12 18:04:23 mschulze Exp $ */
/*
* ABSTRACT: Gauss-Manin system normal form
*/

#include "mod2.h"

#ifdef HAVE_GMS

#include "febase.h"
#include "tok.h"
#include "ipid.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "lists.h"
#include "matpol.h"
#include "gms.h"

lists gmsnf(ideal p,ideal g,matrix B,int D,int K)
{
  ideal r=idInit(IDELEMS(p),1);
  ideal q=idInit(IDELEMS(p),1);

  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);

  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=r;
  l->m[1].rtyp=IDEAL_CMD;
  l->m[1].data=q;

  int i,j,k;
  poly d;
  for(k=0;k<IDELEMS(p);k++)
  {
    while(p->m[k]!=NULL&&pGetExp(p->m[k],1)<=K)
    {
      j=0;
      while(j<IDELEMS(g)&&!pLmDivisibleBy(g->m[j],p->m[k]))
        j++;

      if(j<IDELEMS(g))
      {
        d=pDivideM(pHead(p->m[k]),pHead(g->m[j]));
        p->m[k]=pSub(p->m[k],pMult(pCopy(d),pCopy(g->m[j])));
        pSetExp(d,1,pGetExp(d,1)+1);
        for(i=0;i<MATROWS(B);i++)
          p->m[k]=pAdd(p->m[k],
            pDiff(pMult(pCopy(d),pCopy(MATELEM(B,i+1,j+1))),i+2));
      }
      else
      {
        r->m[k]=pAdd(r->m[k],pHead(p->m[k]));
        pDeleteLm(&p->m[k]);
      }

      while(p->m[k]!=NULL&&pWTotaldegree(p->m[k])>D&&pGetExp(p->m[k],1)<=K)
      {
        q->m[k]=pAdd(q->m[k],pHead(p->m[k]));
        pDeleteLm(&p->m[k]);
      }
    }

    q->m[k]=pAdd(q->m[k],pCopy(p->m[k]));
  }

  return l;
}

BOOLEAN gmsnf(leftv res,leftv h)
{
  if(currRingHdl)
  {
    if(h&&h->Typ()==IDEAL_CMD)
    {
      ideal p=(ideal)h->Data();
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
            int D=(int)h->Data();
            h=h->next;
            if(h&&h->Typ()==INT_CMD)
            {
              int K=(int)h->Data();
              res->rtyp=LIST_CMD;
              res->data=(void*)gmsnf(idCopy(p),g,B,D,K);
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

#endif /* HAVE_GMS */

