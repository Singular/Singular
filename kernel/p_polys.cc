/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.cc
 *  Purpose: implementation of currRing independent poly procedures
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_polys.cc,v 1.12 2007-11-16 18:37:28 Singular Exp $
 *******************************************************************/

#include "mod2.h"
#include "structs.h"
#include "structs.h"
#include "p_polys.h"
#include "ring.h"
#include "int64vec.h"
#ifndef NDEBUG
#include "febase.h"
#endif

/***************************************************************
 *
 * Completing what needs to be set for the monomial
 *
 ***************************************************************/
// this is special for the syz stuff
static int* _Components = NULL;
static long* _ShiftedComponents = NULL;
static int _ExternalComponents = 0;

BOOLEAN pSetm_error=0;

void p_Setm_General(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  int pos=0;
  if (r->typ!=NULL)
  {
    loop
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
#if 1
          for(int i=a;i<=e;i++) ord+=p_GetExp(p,i,r)*w[i-a];
#else
          long ai;
          int ei,wi;
          for(int i=a;i<=e;i++)
          {
             ei=p_GetExp(p,i,r);
             wi=w[i-a];
             ai=ei*wi;
             if (ai/ei!=wi) pSetm_error=TRUE;
             ord+=ai;
             if (ord<ai) pSetm_error=TRUE;
          }
#endif
          p->exp[o->data.wp.place]=ord;
          break;
        }
      case ro_wp64:
        {
          int64 ord=0;
          int a,e;
          a=o->data.wp64.start;
          e=o->data.wp64.end;
          int64 *w=o->data.wp64.weights64;
          int64 ei,wi,ai;
          for(int i=a;i<=e;i++) {
            //Print("exp %d w %d \n",p_GetExp(p,i,r),(int)w[i-a]);
            //ord+=((int64)p_GetExp(p,i,r))*w[i-a];
            ei=(int64)p_GetExp(p,i,r);
            wi=w[i-a];
            ai=ei*wi;
            if(ei!=0 && ai/ei!=wi){
              pSetm_error=TRUE;
              Print("ai %lld, wi %lld\n",ai,wi);
            }
            ord+=ai;
            if (ord<ai){
               pSetm_error=TRUE;
               Print("ai %lld, ord %lld\n",ai,ord);
            }
          }
          int64 mask=(int64)0x7fffffff;
          long a_0=(long)(ord&mask); //2^31
          long a_1=(long)(ord >>31 ); /*(ord/(mask+1));*/

          //Print("mask: %x,  ord: %d,  a_0: %d,  a_1: %d\n"
          //,(int)mask,(int)ord,(int)a_0,(int)a_1);
                    //Print("mask: %d",mask);

          p->exp[o->data.wp64.place]=a_1;
          p->exp[o->data.wp64.place+1]=a_0;
//            if(p_Setm_error) Print("***************************\n
//                                    ***************************\n
//                                    **WARNING: overflow error**\n
//                                    ***************************\n
//                                    ***************************\n");
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
          int* Components = (_ExternalComponents ? _Components :
                             o->data.syzcomp.Components);
          long* ShiftedComponents = (_ExternalComponents ? _ShiftedComponents:
                                     o->data.syzcomp.ShiftedComponents);
          if (ShiftedComponents != NULL)
          {
            assume(Components != NULL);
            assume(c == 0 || Components[c] != 0);
            sc = ShiftedComponents[Components[c]];
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

void p_Setm_Syz(poly p, ring r, int* Components, long* ShiftedComponents)
{
  _Components = Components;
  _ShiftedComponents = ShiftedComponents;
  _ExternalComponents = 1;
  p_Setm_General(p, r);
  _ExternalComponents = 0;
}

// dummy for lp, ls, etc
void p_Setm_Dummy(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
}

// for dp, Dp, ds, etc
void p_Setm_TotalDegree(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  p->exp[r->pOrdIndex] = p_ExpVectorQuerSum(p, r);
}

// for wp, Wp, ws, etc
void p_Setm_WFirstTotalDegree(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  p->exp[r->pOrdIndex] = pWFirstTotalDegree(p, r);
}

p_SetmProc p_GetSetmProc(ring r)
{
  // covers lp, rp, ls,
  if (r->typ == NULL) return p_Setm_Dummy;

  if (r->OrdSize == 1)
  {
    if (r->typ[0].ord_typ == ro_dp &&
        r->typ[0].data.dp.start == 1 &&
        r->typ[0].data.dp.end == r->N &&
        r->typ[0].data.dp.place == r->pOrdIndex)
      return p_Setm_TotalDegree;
    if (r->typ[0].ord_typ == ro_wp &&
        r->typ[0].data.wp.start == 1 &&
        r->typ[0].data.wp.end == r->N &&
        r->typ[0].data.wp.place == r->pOrdIndex &&
        r->typ[0].data.wp.weights == r->firstwv)
      return p_Setm_WFirstTotalDegree;
  }
  return p_Setm_General;
}


/* -------------------------------------------------------------------*/
/* several possibilities for pFDeg: the degree of the head term       */
/*2
* compute the degree of the leading monomial of p
* the ordering is compatible with degree, use a->order
*/
inline long _pDeg(poly a, ring r)
{
  p_LmCheckPolyRing(a, r);
  assume(p_GetOrder(a, r) == pWTotaldegree(a, r));
  return p_GetOrder(a, r);
}

long pDeg(poly a, ring r)
{
  return _pDeg(a, r);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths 1
* (all are 1 so save multiplications or they are of different signs)
* the ordering is not compatible with degree so do not use p->Order
*/
inline long _pTotaldegree(poly p, ring r)
{
  p_LmCheckPolyRing(p, r);
  return (long) p_ExpVectorQuerSum(p, r);
}
long pTotaldegree(poly p, ring r)
{
  return (long) _pTotaldegree(p, r);
}

// pWTotalDegree for weighted orderings
// whose first block covers all variables
inline long _pWFirstTotalDegree(poly p, ring r)
{
  int i;
  long sum = 0;

  for (i=1; i<= r->firstBlockEnds; i++)
  {
    sum += p_GetExp(p, i, r)*r->firstwv[i-1];
  }
  return sum;
}

long pWFirstTotalDegree(poly p, ring r)
{
  return (long) _pWFirstTotalDegree(p, r);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths from the ordering
* the ordering is not compatible with degree so do not use p->Order
*/
long pWTotaldegree(poly p, ring r)
{
  p_LmCheckPolyRing(p, r);
  int i, k;
  long j =0;

  // iterate through each block:
  for (i=0;r->order[i]!=0;i++)
  {
    int b0=r->block0[i];
    int b1=r->block1[i];
    switch(r->order[i])
    {
      case ringorder_M:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // in jedem block:
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - b0 /*r->block0[i]*/]*r->OrdSgn;
        }
        break;
      case ringorder_wp:
      case ringorder_ws:
      case ringorder_Wp:
      case ringorder_Ws:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // in jedem block:
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - b0 /*r->block0[i]*/];
        }
        break;
      case ringorder_lp:
      case ringorder_ls:
      case ringorder_dp:
      case ringorder_ds:
      case ringorder_Dp:
      case ringorder_Ds:
      case ringorder_rp:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        {
          j+= p_GetExp(p,k,r);
        }
        break;
      case ringorder_a64:
        {
          int64* w=(int64*)r->wvhdl[i];
          for (k=0;k<=(b1 /*r->block1[i]*/ - b0 /*r->block0[i]*/);k++)
          {
            //there should be added a line which checks if w[k]>2^31
            j+= p_GetExp(p,k+1, r)*(long)w[k];
          }
          //break;
          return j;
        }
      case ringorder_c:
      case ringorder_C:
      case ringorder_S:
      case ringorder_s:
      case ringorder_aa:
        break;
      case ringorder_a:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // only one line
          j+= p_GetExp(p,k, r)*r->wvhdl[i][ k- b0 /*r->block0[i]*/];
        }
        //break;
        return j;

#ifndef NDEBUG
      default:
        Print("missing order %d in pWTotaldegree\n",r->order[i]);
        break;
#endif
    }
  }
  return  j;
}

int pWeight(int i, const ring r)
{
  if ((r->firstwv==NULL) || (i>r->firstBlockEnds))
  {
    return 1;
  }
  return r->firstwv[i-1];
}

long pWDegree(poly p, ring r)
{
  if (r->firstwv==NULL) return pTotaldegree(p, r);
  p_LmCheckPolyRing(p, r);
  int i, k;
  long j =0;

  for(i=1;i<=r->firstBlockEnds;i++)
    j+=p_GetExp(p, i, r)*r->firstwv[i-1];

  for (;i<=r->N;i++)
    j+=p_GetExp(p,i, r)*pWeight(i, r);

  return j;
}


/* ---------------------------------------------------------------------*/
/* several possibilities for pLDeg: the maximal degree of a monomial in p*/
/*  compute in l also the pLength of p                                   */

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
*/
long pLDeg0(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  int ll=1;

  if (k > 0)
  {
    while ((pNext(p)!=NULL) && (p_GetComp(pNext(p), r)==k))
    {
      pIter(p);
      ll++;
    }
  }
  else
  {
     while (pNext(p)!=NULL)
     {
       pIter(p);
       ll++;
     }
  }
  *l=ll;
  return r->pFDeg(p, r);
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
* but search in all components before syzcomp
*/
long pLDeg0c(poly p,int *l, ring r)
{
  assume(p!=NULL);
#ifdef PDEBUG
  _p_Test(p,r,PDEBUG);
#endif
  p_CheckPolyRing(p, r);
  long o;
  int ll=1;

  if (! rIsSyzIndexRing(r))
  {
    while (pNext(p) != NULL)
    {
      pIter(p);
      ll++;
    }
    o = r->pFDeg(p, r);
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit(r);
    poly pp = p;
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=curr_limit/*syzComp*/)
        ll++;
      else break;
      pp = p;
    }
#ifdef PDEBUG
    _p_Test(pp,r,PDEBUG);
#endif
    o = r->pFDeg(pp, r);
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
long pLDegb(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  long o = r->pFDeg(p, r);
  int ll=1;

  if (k != 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
    {
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p)) !=NULL)
    {
      ll++;
    }
  }
  *l=ll;
  return o;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one, we have to look for it
*/
long pLDeg1(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=r->pFDeg(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
    {
      t=r->pFDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=r->pFDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
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
long pLDeg1c(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=r->pFDeg(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=limit)
      {
        if ((t=r->pFDeg(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=r->pFDeg(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == pDeg
long pLDeg1_Deg(poly p,int *l, ring r)
{
  assume(r->pFDeg == pDeg);
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=_pDeg(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
    {
      t=_pDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=_pDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_Deg(poly p,int *l, ring r)
{
  assume(r->pFDeg == pDeg);
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=_pDeg(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=limit)
      {
        if ((t=_pDeg(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=_pDeg(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == pTotoalDegree
long pLDeg1_Totaldegree(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=_pTotaldegree(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
    {
      t=_pTotaldegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=_pTotaldegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_Totaldegree(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=_pTotaldegree(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=limit)
      {
        if ((t=_pTotaldegree(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=_pTotaldegree(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == pWFirstTotalDegree
long pLDeg1_WFirstTotalDegree(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=_pWFirstTotalDegree(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
    {
      t=_pWFirstTotalDegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=_pWFirstTotalDegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_WFirstTotalDegree(poly p,int *l, ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=_pWFirstTotalDegree(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=limit)
      {
        if ((t=_pTotaldegree(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=_pTotaldegree(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

/***************************************************************
 *
 * Maximal Exponent business
 *
 ***************************************************************/

static inline unsigned long
p_GetMaxExpL2(unsigned long l1, unsigned long l2, ring r,
              unsigned long number_of_exp)
{
  const unsigned long bitmask = r->bitmask;
  unsigned long ml1 = l1 & bitmask;
  unsigned long ml2 = l2 & bitmask;
  unsigned long max = (ml1 > ml2 ? ml1 : ml2);
  unsigned long j = number_of_exp - 1;

  if (j > 0)
  {
    unsigned long mask = bitmask << r->BitsPerExp;
    while (1)
    {
      ml1 = l1 & mask;
      ml2 = l2 & mask;
      max |= ((ml1 > ml2 ? ml1 : ml2) & mask);
      j--;
      if (j == 0) break;
      mask = mask << r->BitsPerExp;
    }
  }
  return max;
}

static inline unsigned long
p_GetMaxExpL2(unsigned long l1, unsigned long l2, ring r)
{
  return p_GetMaxExpL2(l1, l2, r, r->ExpPerLong);
}

poly p_GetMaxExpP(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  if (p == NULL) return p_Init(r);
  poly max = p_LmInit(p, r);
  pIter(p);
  if (p == NULL) return max;
  int i, offset;
  unsigned long l_p, l_max;
  unsigned long divmask = r->divmask;

  do
  {
    offset = r->VarL_Offset[0];
    l_p = p->exp[offset];
    l_max = max->exp[offset];
    // do the divisibility trick to find out whether l has an exponent
    if (l_p > l_max ||
        (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
      max->exp[offset] = p_GetMaxExpL2(l_max, l_p, r);

    for (i=1; i<r->VarL_Size; i++)
    {
      offset = r->VarL_Offset[i];
      l_p = p->exp[offset];
      l_max = max->exp[offset];
      // do the divisibility trick to find out whether l has an exponent
      if (l_p > l_max ||
          (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
        max->exp[offset] = p_GetMaxExpL2(l_max, l_p, r);
    }
    pIter(p);
  }
  while (p != NULL);
  return max;
}

unsigned long p_GetMaxExpL(poly p, ring r, unsigned long l_max)
{
  unsigned long l_p, divmask = r->divmask;
  int i;

  while (p != NULL)
  {
    l_p = p->exp[r->VarL_Offset[0]];
    if (l_p > l_max ||
        (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
      l_max = p_GetMaxExpL2(l_max, l_p, r);
    for (i=1; i<r->VarL_Size; i++)
    {
      l_p = p->exp[r->VarL_Offset[i]];
      // do the divisibility trick to find out whether l has an exponent
      if (l_p > l_max ||
          (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
        l_max = p_GetMaxExpL2(l_max, l_p, r);
    }
    pIter(p);
  }
  return l_max;
}




/***************************************************************
 *
 * Misc things
 *
 ***************************************************************/
// returns TRUE, if all monoms have the same component
BOOLEAN p_OneComp(poly p, ring r)
{
  if(p!=NULL)
  {
    long i = p_GetComp(p, r);
    while (pNext(p)!=NULL)
    {
      pIter(p);
      if(i != p_GetComp(p, r)) return FALSE;
    }
  }
  return TRUE;
}

/*2
*test if a monomial /head term is a pure power
*/
int p_IsPurePower(const poly p, const ring r)
{
  int i,k=0;

  for (i=r->N;i;i--)
  {
    if (p_GetExp(p,i, r)!=0)
    {
      if(k!=0) return 0;
      k=i;
    }
  }
  return k;
}

/*2
*test if a polynomial is univariate
* return -1 for constant,
* 0 for not univariate,s
* i if dep. on var(i)
*/
int p_IsUnivariate(poly p, const ring r)
{
  int i,k=-1;

  while (p!=NULL)
  {
    for (i=r->N;i;i--)
    {
      if (p_GetExp(p,i, r)!=0)
      {
        if((k!=-1)&&(k!=i)) return 0;
        k=i;
      }
    }
    pIter(p);
  }
  return k;
}

// set entry e[i] to 1 if var(i) occurs in p, ignore var(j) if e[j]>0
int  p_GetVariables(poly p, int * e, const ring r)
{
  int i;
  int n=0;
  while(p!=NULL)
  {
    n=0;
    for(i=r->N; i>0; i--)
    {
      if(e[i]==0)
      {
        if (p_GetExp(p,i,r)>0)
        {
          e[i]=1;
          n++;
        }
      }
      else
        n++;
    }
    if (n==r->N) break;
    pIter(p);
  }
  return n;
}


/*2
* returns a polynomial representing the integer i
*/
poly p_ISet(int i, ring r)
{
  poly rc = NULL;
  if (i!=0)
  {
    rc = p_Init(r);
    pSetCoeff0(rc,r->cf->nInit(i));
    if (r->cf->nIsZero(p_GetCoeff(rc,r)))
      p_DeleteLm(&rc,r);
  }
  return rc;
}

/*2
* returns a polynomial representing the number n
* destroys n
*/
poly p_NSet(number n, ring r)
{
  if (r->cf->nIsZero(n))
  {
    r->cf->cfDelete(&n, r);
    return NULL;
  }
  else
  {
    poly rc = p_Init(r);
    pSetCoeff0(rc,n);
    return rc;
  }
}

/***************************************************************
 *
 * p_ShallowDelete
 *
 ***************************************************************/
#undef LINKAGE
#define LINKAGE
#undef p_Delete
#define p_Delete p_ShallowDelete
#undef n_Delete
#define n_Delete(n, r) ((void)0)

#include "p_Delete__T.cc"

