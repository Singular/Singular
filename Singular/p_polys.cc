/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.cc
 *  Purpose: implementation of currRing independent poly procedures
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_polys.cc,v 1.2 2000-10-23 15:21:14 Singular Exp $
 *******************************************************************/

#include "mod2.h"
#include "p_polys.h"
#include "ring.h"


// complete the fields of the monomials
void p_Setm(poly p, ring r)
{
  int pos=0;
  if (r->typ!=NULL)
  {
    while (1)
    {
      long ord=0;
      sro_ord* o=&(r->typ[pos]);
      switch(o->ord_typ)
      {
        case ro_dp:
        {
          int a,e;
          a=o->data.dp.start;
          e=o->data.dp.end;
          for(int i=a;i<=e;i++) ord+=p_GetExp(p,i,r);
          p->exp[o->data.dp.place]=ord;
          break;
        }
        case ro_wp_neg:
          ord=POLY_NEGWEIGHT_OFFSET;
          // no break;
        case ro_wp:
        {
          int a,e;
          a=o->data.wp.start;
          e=o->data.wp.end;
          int *w=o->data.wp.weights;
          for(int i=a;i<=e;i++) ord+=p_GetExp(p,i,r)*w[i-a];
          p->exp[o->data.wp.place]=ord;
          break;
        }
        case ro_cp:
        {
          int a,e;
          a=o->data.cp.start;
          e=o->data.cp.end;
          int pl=o->data.cp.place;
          for(int i=a;i<=e;i++) { p->exp[pl]=p_GetExp(p,i,r); pl++; }
          break;
        }
        case ro_syzcomp:
        {
          int c=p_GetComp(p,r);
          long sc = c;
          if (o->data.syzcomp.ShiftedComponents != NULL)
          {
            assume(o->data.syzcomp.Components != NULL);
            assume(c == 0 || o->data.syzcomp.Components[c] != 0);
            sc =
              o->data.syzcomp.ShiftedComponents[o->data.syzcomp.Components[c]];
            assume(c == 0 || sc != 0);
          }
          p->exp[o->data.syzcomp.place]=sc;
          break;
        }
        case ro_syz:
        {
          int c=p_GetComp(p, r);
          if (c > o->data.syz.limit)
            p->exp[o->data.syz.place] = o->data.syz.curr_index;
          else if (c > 0)
            p->exp[o->data.syz.place]= o->data.syz.syz_index[c];
          else
          {
            assume(c == 0);
            p->exp[o->data.syz.place]= 0;
          }
          break;
        }
        default:
          dReportError("wrong ord in rSetm:%d\n",o->ord_typ);
          return;
      }
      pos++;
      if (pos == r->OrdSize) return;
    }
  }
}


/* -------------------------------------------------------------------*/
/* several possibilities for pFDeg: the degree of the head term       */
/*2
* compute the degree of the leading monomial of p
* the ordering is compatible with degree, use a->order
*/
int pDeg(poly a, ring r)
{
  return p_GetOrder(a, r);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths 1
* (all are 1 so save multiplications or they are of different signs)
* the ordering is not compatible with degree so do not use p->Order
*/
int pTotaldegree(poly p, ring r)
{
  return (int) p_ExpVectorQuerSum(p, r);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths from the ordering
* the ordering is not compatible with degree so do not use p->Order
*/
int pWTotaldegree(poly p, ring r)
{
  assume(p != NULL);
  int i, k;
  int j =0;

  // iterate through each block:
  for (i=0;r->order[i]!=0;i++)
  {
    switch(r->order[i])
    {
      case ringorder_wp:
      case ringorder_ws:
      case ringorder_Wp:
      case ringorder_Ws:
        for (k=r->block0[i];k<=r->block1[i];k++)
        { // in jedem block:
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - r->block0[i]];
        }
        break;
      case ringorder_M:
      case ringorder_lp:
      case ringorder_dp:
      case ringorder_ds:
      case ringorder_Dp:
      case ringorder_Ds:
        for (k=r->block0[i];k<=r->block1[i];k++)
        {
          j+= p_GetExp(p,k,r);
        }
        break;
      case ringorder_c:
      case ringorder_C:
      case ringorder_S:
      case ringorder_s:
        break;
      case ringorder_a:
        for (k=r->block0[i];k<=r->block1[i];k++)
        { // only one line
          j+= p_GetExp(p,k, r)*r->wvhdl[i][ k- r->block0[i]];
        }
        return j;
    }
  }
  return  j;
}
int pWDegree(poly p, ring r)
{
  int i, k;
  int j =0;

  for(i=1;i<=r->N;i++)
    j+=p_GetExp(p,i, r)*pWeight(i, r);
  return j;
}
int pWeight(int i, ring r)
{
  if ((r->firstwv==NULL) || (i>r->firstBlockEnds))
  {
    return 1;
  }
  return r->firstwv[i-1];
}


/* ---------------------------------------------------------------------*/
/* several possibilities for pLDeg: the maximal degree of a monomial in p*/
/*  compute in l also the pLength of p                                   */

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
*/
int pLDeg0(poly p,int *l, ring r)
{
  Exponent_t k= p_GetComp(p, r);
  int ll=1;

  while ((pNext(p)!=NULL) && (p_GetComp(pNext(p), r)==k))
  {
    pIter(p);
    ll++;
  }
  *l=ll;
  return p_GetOrder(p, r);
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
* but search in all components before syzcomp
*/
int pLDeg0c(poly p,int *l, ring r)
{
  int o=pFDeg(p, r);
  int ll=1;

  if (! rIsSyzIndexRing(r))
  {
    while ((p=pNext(p))!=NULL)
    {
      o=pFDeg(p, r);
      ll++;
    }
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=curr_limit/*syzComp*/)
      {
        o=pFDeg(p, r);
        ll++;
      }
      else break;
    }
  }
  *l=ll;
  return o;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the first one
* this works for the polynomial case with degree orderings
* (both c,dp and dp,c)
*/
int pLDegb(poly p,int *l, ring r)
{
  Exponent_t k= p_GetComp(p, r);
  int o = pFDeg(p, r);
  int ll=1;

  while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
  {
    ll++;
  }
  *l=ll;
  return o;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one, we have to look for it
*/
int pLDeg1(poly p,int *l, ring r)
{
  Exponent_t k= p_GetComp(p, r);
  int ll=1;
  int  t,max;

  max=pFDeg(p);
  while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
  {
    t=pFDeg(p, r);
    if (t>max) max=t;
    ll++;
  }
  *l=ll;
  return max;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one, we have to look for it
* in all components
*/
int pLDeg1c(poly p,int *l, ring r)
{
  int ll=1;
  int  t,max;

  max=pFDeg(p, r);
  while ((p=pNext(p))!=NULL)
  {
    if (! rIsSyzIndexRing(r) ||
        (p_GetComp(p, r)<=rGetCurrSyzLimit(r)))
    {
       if ((t=pFDeg(p, r))>max) max=t;
       ll++;
    }
    else break;
  }
  *l=ll;
  return max;
}
