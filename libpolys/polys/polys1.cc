</****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT - all basic methods to manipulate polynomials:
* independent of representation
*/

/* includes */
#include <string.h>
#include "polys/config.h"
// #include <polys/options.h>
#include <coeffs/numbers.h>
#include <coeffs/ffields.h>
#include <omalloc/omalloc.h>
// #include <???/febase.h>
// #include <???/weight.h>
// #include <???/intvec.h>
#include <polys/ext_fields/longalg.h>
#include <polys/monomials/ring.h>
// #include <???/ideals.h>
#include <polys/polys.h>
// #include "ipid.h"
#ifdef HAVE_FACTORY
// #include <???/clapsing.h>
#endif

#ifdef HAVE_RATGRING
// #include <polys/ratgring.h>
#endif


poly ppJet(poly p, int m)
{
  poly r=NULL;
  poly t=NULL;

  while (p!=NULL)
  {
    if (p_Totaldegree(p,currRing)<=m)
    {
      if (r==NULL)
        r=pHead(p);
      else
      if (t==NULL)
      {
        pNext(r)=pHead(p);
        t=pNext(r);
      }
      else
      {
        pNext(t)=pHead(p);
        pIter(t);
      }
    }
    pIter(p);
  }
  return r;
}

poly pJet(poly p, int m)
{
  poly t=NULL;

  while((p!=NULL) && (p_Totaldegree(p,currRing)>m)) pLmDelete(&p);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (p_Totaldegree(pNext(p),currRing)>m)
    {
      pLmDelete(&pNext(p));
    }
    else
      pIter(p);
  }
  return r;
}

poly ppJetW(poly p, int m, short *w)
{
  poly r=NULL;
  poly t=NULL;
  while (p!=NULL)
  {
    if (totaldegreeWecart_IV(p,currRing,w)<=m)
    {
      if (r==NULL)
        r=pHead(p);
      else
      if (t==NULL)
      {
        pNext(r)=pHead(p);
        t=pNext(r);
      }
      else
      {
        pNext(t)=pHead(p);
        pIter(t);
      }
    }
    pIter(p);
  }
  return r;
}

poly pJetW(poly p, int m, short *w)
{
  while((p!=NULL) && (totaldegreeWecart_IV(p,currRing,w)>m)) pLmDelete(&p);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (totaldegreeWecart_IV(pNext(p),currRing,w)>m)
    {
      pLmDelete(&pNext(p));
    }
    else
      pIter(p);
  }
  return r;
}

int pMinDeg(poly p,intvec *w)
{
  if(p==NULL)
    return -1;
  int d=-1;
  while(p!=NULL)
  {
    int d0=0;
    for(int j=0;j<pVariables;j++)
      if(w==NULL||j>=w->length())
        d0+=pGetExp(p,j+1);
      else
        d0+=(*w)[j]*pGetExp(p,j+1);
    if(d0<d||d==-1)
      d=d0;
    pIter(p);
  }
  return d;
}

poly pSeries(int n,poly p,poly u, intvec *w)
{
  short *ww=iv2array(w);
  if(p!=NULL)
  {
    if(u==NULL)
      p=pJetW(p,n,ww);
    else
      p=pJetW(pMult(p,pInvers(n-pMinDeg(p,w),u,w)),n,ww);
  }
  omFreeSize((ADDRESS)ww,(pVariables+1)*sizeof(short));
  return p;
}

poly pInvers(int n,poly u,intvec *w)
{
  short *ww=iv2array(w);
  if(n<0)
    return NULL;
  number u0=nInvers(pGetCoeff(u));
  poly v=pNSet(u0);
  if(n==0)
    return v;
  poly u1=pJetW(pSub(pOne(),pMult_nn(u,u0)),n,ww);
  if(u1==NULL)
    return v;
  poly v1=pMult_nn(pCopy(u1),u0);
  v=pAdd(v,pCopy(v1));
  for(int i=n/pMinDeg(u1,w);i>1;i--)
  {
    v1=pJetW(pMult(v1,pCopy(u1)),n,ww);
    v=pAdd(v,pCopy(v1));
  }
  pDelete(&u1);
  pDelete(&v1);
  omFreeSize((ADDRESS)ww,(pVariables+1)*sizeof(short));
  return v;
}

long pDegW(poly p, const short *w)
{
  long r=-LONG_MAX;

  while (p!=NULL)
  {
    long t=totaldegreeWecart_IV(p,currRing,w);
    if (t>r) r=t;
    pIter(p);
  }
  return r;
}

/*-----------type conversions ----------------------------*/
#if 0
/*2
* convert a vector to a set of polys,
* allocates the polyset, (entries 0..(*len)-1)
* the vector will not be changed
*/
void  pVec2Polys(poly v, polyset *p, int *len)
{
  poly h;
  int k;

  *len=pMaxComp(v);
  if (*len==0) *len=1;
  *p=(polyset)omAlloc0((*len)*sizeof(poly));
  while (v!=NULL)
  {
    h=pHead(v);
    k=pGetComp(h);
    pSetComp(h,0);
    (*p)[k-1]=pAdd((*p)[k-1],h);
    pIter(v);
  }
}

int p_Var(poly m,const ring r)
{
  if (m==NULL) return 0;
  if (pNext(m)!=NULL) return 0;
  int i,e=0;
  for (i=r->N; i>0; i--)
  {
    int exp=p_GetExp(m,i,r);
    if (exp==1)
    {
      if (e==0) e=i;
      else return 0;
    }
    else if (exp!=0)
    {
      return 0;
    }
  }
  return e;
}

/*2
* returns TRUE if p1 = p2
*/
BOOLEAN p_EqualPolys(poly p1,poly p2, const ring r)
{
  while ((p1 != NULL) && (p2 != NULL))
  {
    if (! p_LmEqual(p1, p2,r))
      return FALSE;
    if (! n_Equal(p_GetCoeff(p1,r), p_GetCoeff(p2,r),r ))
      return FALSE;
    pIter(p1);
    pIter(p2);
  }
  return (p1==p2);
}

/*2
*returns TRUE if p1 is a skalar multiple of p2
*assume p1 != NULL and p2 != NULL
*/
BOOLEAN pComparePolys(poly p1,poly p2)
{
  number n,nn;
  pAssume(p1 != NULL && p2 != NULL);

  if (!pLmEqual(p1,p2)) //compare leading mons
      return FALSE;
  if ((pNext(p1)==NULL) && (pNext(p2)!=NULL))
     return FALSE;
  if ((pNext(p2)==NULL) && (pNext(p1)!=NULL))
     return FALSE;
  if (pLength(p1) != pLength(p2))
    return FALSE;
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    if (!nDivBy(pGetCoeff(p1), pGetCoeff(p2))) return FALSE;
  }
#endif
  n=nDiv(pGetCoeff(p1),pGetCoeff(p2));
  while ((p1 != NULL) /*&& (p2 != NULL)*/)
  {
    if ( ! pLmEqual(p1, p2))
    {
        nDelete(&n);
        return FALSE;
    }
    if (!nEqual(pGetCoeff(p1),nn=nMult(pGetCoeff(p2),n)))
    {
      nDelete(&n);
      nDelete(&nn);
      return FALSE;
    }
    nDelete(&nn);
    pIter(p1);
    pIter(p2);
  }
  nDelete(&n);
  return TRUE;
}
