/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.cc,v 1.54 2000-05-02 16:30:43 Singular Exp $ */

/*
* ABSTRACT - all basic methods to manipulate polynomials
*/

/* includes */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "binom.h"
#include "polys-comp.h"

/* ----------- global variables, set by pSetGlobals --------------------- */
/* initializes the internal data from the exp vector */
pSetmProc pSetm;
/* computes length and maximal degree of a POLYnomial */
pLDegProc pLDeg;
/* computes the degree of the initial term, used for std */
pFDegProc pFDeg;
/* the monomial ordering of the head monomials a and b */
/* returns -1 if a comes before b, 0 if a=b, 1 otherwise */

int pVariables;     // number of variables
//int pVariablesW;    // number of words of pVariables exponents
//int pVariables1W;   // number of words of (pVariables+1) exponents
int pMonomSize;     // size of monom (in bytes)
int pMonomSizeW;    // size of monom (in words)
int *pVarOffset;    // controls the way exponents are stored in a vector
//int pVarLowIndex;   // lowest exponent index
//int pVarHighIndex;  // highest exponent index
//int pVarCompIndex;  // Location of component in exponent vector

/* 1 for polynomial ring, -1 otherwise */
int     pOrdSgn;
/* TRUE for momomial output as x2y, FALSE for x^2*y */
int pShortOut = (int)TRUE;
// it is of type int, not BOOLEAN because it is also in ip
/* TRUE if the monomial ordering is not compatible with pFDeg */
BOOLEAN pLexOrder;
/* TRUE if the monomial ordering has polynomial and power series blocks */
BOOLEAN pMixedOrder;
/* 1 for c ordering, -1 otherwise (i.e. for C ordering) */
int  pComponentOrder;

/* ----------- global variables, set by procedures from hecke/kstd1 ----- */
/* the highest monomial below pHEdge */
poly      ppNoether = NULL;

/* -------------- static variables --------------------------------------- */
/*is the basic comparing procedure during a computation of syzygies*/
//static pCompProc pCompOld;

/*contains the headterms for the Schreyer orderings*/
static int* SchreyerOrd;
static int maxSchreyer=0;
static int indexShift=0;
static pLDegProc pLDegOld;

static int**   polys_wv;
static short *   firstwv;
static int * block0;
static int * block1;
static int firstBlockEnds;
static int * order;

/*0 implementation*/
/*-------- the several possibilities for pSetm:-----------------------*/

void rSetm(poly p)
{
  int pos=0;
  if (currRing->typ!=NULL)
  {
    loop
    {
      sro_ord* o=&(currRing->typ[pos]);
      switch(o->ord_typ)
      {
        case ro_dp:
        {
          int a,e;
          a=o->data.dp.start;
          e=o->data.dp.end;
          long ord=0;
          for(int i=a;i<=e;i++) ord+=pGetExp(p,i);
          p->exp.l[o->data.dp.place]=ord;
          break;
        }
        case ro_wp:
        {
          int a,e;
          a=o->data.wp.start;
          e=o->data.wp.end;
          int *w=o->data.wp.weights;
          long ord=0;
          for(int i=a;i<=e;i++) ord+=pGetExp(p,i)*w[i-a];
          p->exp.l[o->data.wp.place]=ord;
          break;
        }
        case ro_cp:
        {
          int a,e;
          a=o->data.cp.start;
          e=o->data.cp.end;
          int pl=o->data.cp.place;
          for(int i=a;i<=e;i++) { p->exp.e[pl]=pGetExp(p,i); pl++; }
          break;
        }
        case ro_syzcomp:
        {
          int c=pGetComp(p);
          long sc = c;
          if (o->data.syzcomp.ShiftedComponents != NULL)
          {
            assume(o->data.syzcomp.Components != NULL);
            assume(c == 0 || o->data.syzcomp.Components[c] != 0);
            sc =
              o->data.syzcomp.ShiftedComponents[o->data.syzcomp.Components[c]];
            assume(c == 0 || sc != 0);
          }
          p->exp.l[o->data.syzcomp.place]=sc;
          break;
        }
        case ro_syz:
        {
          int c=pGetComp(p);
          if (c > o->data.syz.limit)
            p->exp.l[o->data.syz.place] = o->data.syz.curr_index;
          else if (c > 0)
            p->exp.l[o->data.syz.place]= o->data.syz.syz_index[c];
          else
          {
            assume(c == 0);
            p->exp.l[o->data.syz.place]= 0;
          }
          break;
        }
        default:
          Print("wrong ord in rSetm:%d\n",o->ord_typ);
          return;
      }
      pos++;
      if(pos==currRing->OrdSize) return;
    }
  }
}

void rSetmS(poly p, int* Components, long* ShiftedComponents)
{
  int pos=0;
  assume(Components != NULL && ShiftedComponents != NULL);
  if (currRing->typ!=NULL)
  {
    loop
    {
      sro_ord* o=&(currRing->typ[pos]);
      switch(o->ord_typ)
      {
        case ro_dp:
        {
          int a,e;
          a=o->data.dp.start;
          e=o->data.dp.end;
          long ord=0;
          for(int i=a;i<=e;i++) ord+=pGetExp(p,i);
          p->exp.l[o->data.dp.place]=ord;
          break;
        }
        case ro_wp:
        {
          int a,e;
          a=o->data.wp.start;
          e=o->data.wp.end;
          int *w=o->data.wp.weights;
          long ord=0;
          for(int i=a;i<=e;i++) ord+=pGetExp(p,i)*w[i-a];
          p->exp.l[o->data.wp.place]=ord;
          break;
        }
        case ro_cp:
        {
          int a,e;
          a=o->data.cp.start;
          e=o->data.cp.end;
          int pl=o->data.cp.place;
          for(int i=a;i<=e;i++) { p->exp.e[pl]=pGetExp(p,i); pl++; }
          break;
        }
        case ro_syzcomp:
        {
#if 1
          int c=pGetComp(p);
          long sc  = ShiftedComponents[Components[c]];
          assume(c == 0 || Components[c] != 0);
          assume(c == 0 || sc != 0);
          p->exp.l[o->data.syzcomp.place]=sc;
#endif
          break;
        }
        default:
          Print("wrong ord in rSetm:%d\n",o->ord_typ);
          return;
      }
      pos++;
      if(pos==currRing->OrdSize) return;
    }
  }
}

/*-------- IMPLEMENTATION OF MONOMIAL COMPARISONS ---------------------*/


#define NonZeroR(l, actionG, actionS)           \
do                                              \
{                                               \
  long _l = l;                                  \
  if (_l)                                       \
  {                                             \
    if (_l > 0) actionG;                        \
    actionS;                                    \
  }                                             \
}                                               \
while(0)

#define Mreturn(d, multiplier)                      \
{                                                   \
  if (d > 0) return multiplier;                     \
  return -multiplier;                               \
}


/*---------------------------------------------------*/

int pComp(poly p1, poly p2)
{
  if (p2==NULL)
    return 1;
  if (p1==NULL)
    return -1;
  return pComp0(p1,p2);
}


/*----------pComp handling for syzygies---------------------*/
static void newHeadsB(polyset actHeads,int length)
{
  int i;
  int* newOrder=(int*)Alloc(length*sizeof(int));

  for (i=0;i<length;i++)
  {
    if (actHeads[i]!=NULL)
    {
      newOrder[i] = SchreyerOrd[pGetComp(actHeads[i])-1];
    }
    else
    {
      newOrder[i]=0;
    }
  }
  Free((ADDRESS)SchreyerOrd,maxSchreyer*sizeof(int));
  SchreyerOrd = newOrder;
  maxSchreyer = length;
/*
*for (i=0;i<maxSchreyer;i++); Print("%d  ",SchreyerOrd[i]);
*PrintLn();
*/
}

int mcompSchrB(poly p1,poly p2)
{
  int CompP1=pGetComp(p1),CompP2=pGetComp(p2),result,
      cP1=SchreyerOrd[CompP1-1],cP2=SchreyerOrd[CompP2-1];

  //if (CompP1==CompP2) return pCompOld(p1,p2);
  if (CompP1==CompP2) return pComp0(p1,p2);
  pSetComp(p1,cP1);
  pSetComp(p2,cP2);
  //result = pCompOld(p1,p2);
  result = pComp0(p1,p2);
  pSetComp(p1,CompP1);
  pSetComp(p2,CompP2);
  if (!result)
  {
    if (CompP1>CompP2)
      return -1;
    else if (CompP1<CompP2)
      return 1;
  }
  return result;
}


static void newHeadsM(polyset actHeads,int length)
{
  int i;
  int* newOrder=
    (int*)Alloc0((length+maxSchreyer-indexShift)*sizeof(int));

  //for (i=0;i<length+maxSchreyer-indexShift;i++)
  //  newOrder[i]=0;
  for (i=indexShift;i<maxSchreyer;i++)
  {
    newOrder[i-indexShift] = SchreyerOrd[i];
    SchreyerOrd[i] = 0;
  }
  for (i=maxSchreyer-indexShift;i<length+maxSchreyer-indexShift;i++)
    newOrder[i] = newOrder[pGetComp(actHeads[i-maxSchreyer+indexShift])-1];
  Free((ADDRESS)SchreyerOrd,maxSchreyer*sizeof(int));
  SchreyerOrd = newOrder;
  indexShift = maxSchreyer-indexShift;
  maxSchreyer = length+indexShift;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one and the module component
* has to be <= indexShift
*/
static int ldegSchrM(poly p,int *l)
{
  int  t,max;

  (*l)=1;
  max=pFDeg(p);
  while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=indexShift))
  {
    pIter(p);
    t=pFDeg(p);
    if (t>max) max=t;
    (*l)++;
  }
  return max;
}

int mcompSchrM(poly p1,poly p2)
{
  if ( pGetComp(p1)<=indexShift)
  {
    if ( pGetComp(p2)>indexShift) return 1;
  }
  else if ( pGetComp(p2)<=indexShift)  return -1;
  return mcompSchrB(p1,p2);
}

void pSetSchreyerOrdM(polyset nextOrder, int length,int comps)
{
  int i;

  if (length!=0)
  {
    if (maxSchreyer!=0)
      newHeadsM(nextOrder, length);
    else
    {
      indexShift = comps;
      if (indexShift==0) indexShift = 1;
      SchreyerOrd = (int*)Alloc((indexShift+length)*sizeof(int));
      maxSchreyer = length+indexShift;
      for (i=0;i<indexShift;i++)
        SchreyerOrd[i] = i;
      for (i=indexShift;i<maxSchreyer;i++)
        SchreyerOrd[i] = pGetComp(nextOrder[i-indexShift]);
      //pCompOld = pComp0;
      //pComp0 = mcompSchrM;
      pLDegOld = pLDeg;
      pLDeg = ldegSchrM;
    }
  }
  else
  {
    if (maxSchreyer!=0)
    {
      Free((ADDRESS)SchreyerOrd,maxSchreyer*sizeof(int));
      maxSchreyer = 0;
      indexShift = 0;
      //pComp0 = pCompOld;
      pLDeg = pLDegOld;
    }
  }
}

/*2
* the type of the module ordering: C: -1, c: 1
*/
int pModuleOrder()
{
  return pComponentOrder;
}

/* -------------------------------------------------------------------*/
/* several possibilities for pFDeg: the degree of the head term       */
/*2
* compute the degree of the leading monomial of p
* the ordering is compatible with degree, use a->order
*/
int pDeg(poly a)
{
  return pGetOrder(a);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths 1
* (all are 1 so save multiplications or they are of different signs)
* the ordering is not compatible with degree so do not use p->Order
*/
int pTotaldegree(poly p)
{
  return pExpQuerSum(p);
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths from the ordering
* the ordering is not compatible with degree so do not use p->Order
*/
int pWTotaldegree(poly p)
{
  assume(p != NULL);
  int i, k;
  int j =0;

  // iterate through each block:
  for (i=0;order[i]!=0;i++)
  {
    switch(order[i])
    {
      case ringorder_wp:
      case ringorder_ws:
      case ringorder_Wp:
      case ringorder_Ws:
        for (k=block0[i];k<=block1[i];k++)
        { // in jedem block:
          j+= pGetExp(p,k)*polys_wv[i][k-block0[i]];
        }
        break;
      case ringorder_M:
      case ringorder_lp:
      case ringorder_dp:
      case ringorder_ds:
      case ringorder_Dp:
      case ringorder_Ds:
        for (k=block0[i];k<=block1[i];k++)
        {
          j+= pGetExp(p,k);
        }
        break;
      case ringorder_c:
      case ringorder_C:
      case ringorder_S:
      case ringorder_s:
        break;
      case ringorder_a:
        for (k=block0[i];k<=block1[i];k++)
        { // only one line
          j+= pGetExp(p,k)*polys_wv[i][k-block0[i]];
        }
        return j;
    }
  }
  return  j;
}
int pWDegree(poly p)
{
  int i, k;
  int j =0;

  for(i=1;i<=pVariables;i++)
    j+=pGetExp(p,i)*pWeight(i);
  return j;
}

/* ---------------------------------------------------------------------*/
/* several possibilities for pLDeg: the maximal degree of a monomial in p*/
/*  compute in l also the pLength of p                                   */

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
*/
static int ldeg0(poly p,int *l)
{
  Exponent_t k= pGetComp(p);
  int ll=1;

  while ((pNext(p)!=NULL) && (pGetComp(pNext(p))==k))
  {
    pIter(p);
    ll++;
  }
  *l=ll;
  return pGetOrder(p);
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
* but search in all components before syzcomp
*/
static int ldeg0c(poly p,int *l)
{
  int o=pFDeg(p);
  int ll=1;

  if (! rIsSyzIndexRing(currRing))
  {
    while ((p=pNext(p))!=NULL)
    {
      o=pFDeg(p);
      ll++;
    }
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit();
    while ((p=pNext(p))!=NULL)
    {
      if (pGetComp(p)<=curr_limit/*syzComp*/)
      {
        o=pFDeg(p);
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
static int ldegb(poly p,int *l)
{
  Exponent_t k= pGetComp(p);
  int o = pFDeg(p);
  int ll=1;

  while (((p=pNext(p))!=NULL) && (pGetComp(p)==k))
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
static int ldeg1(poly p,int *l)
{
  Exponent_t k= pGetComp(p);
  int ll=1;
  int  t,max;

  max=pFDeg(p);
  while (((p=pNext(p))!=NULL) && (pGetComp(p)==k))
  {
    t=pFDeg(p);
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
static int ldeg1c(poly p,int *l)
{
  int ll=1;
  int  t,max;

  max=pFDeg(p);
  while ((p=pNext(p))!=NULL)
  {
    if (! rIsSyzIndexRing(currRing) || 
        (pGetComp(p)<=rGetCurrSyzLimit()))
    {
       if ((t=pFDeg(p))>max) max=t;
       ll++;
    }
    else break;
  }
  *l=ll;
  return max;
}

/* -------------------------------------------------------- */
/* set the variables for a choosen ordering                 */


/*2
* sets the comparision routine for monomials: for all but the first
* block of variables (ip is the block number, o_r the number of the ordering)
*/
static void HighSet(int ip, int o_r)
{
  switch(o_r)
  {
    case ringorder_lp:
    case ringorder_dp:
    case ringorder_Dp:
    case ringorder_wp:
    case ringorder_Wp:
    case ringorder_a:
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_ls:
    case ringorder_ds:
    case ringorder_Ds:
    case ringorder_ws:
    case ringorder_Ws:
    case ringorder_s:
      break;
    case ringorder_c:
      pComponentOrder=1;
      break;
    case ringorder_C:
    case ringorder_S:
      pComponentOrder=-1;
      break;
    case ringorder_M:
      pMixedOrder=TRUE;
      break;
#ifdef TEST
    default:
      Werror("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
#endif
  }
}

/* -------------------------------------------------------- */
/*2
* change all variables to fit the description of the new ring
*/

//void pChangeRing(ring newRing)
//{
//  rComplete(newRing);
//  pSetGlobals(newRing);
//}

void pSetGlobals(ring r, BOOLEAN complete)
{
  int i;
  pComponentOrder=1;
  if (ppNoether!=NULL) pDelete(&ppNoether);
  pVariables = r->N;

  // set the various size parameters and initialize memory
  pMonomSize = POLYSIZE + r->ExpLSize * sizeof(long);
  pMonomSizeW = pMonomSize/sizeof(void*);

  // Initialize memory management
  mm_specHeap = r->mm_specHeap;

  pVarOffset = r->VarOffset;

  pOrdSgn = r->OrdSgn;
  pVectorOut=(r->order[0]==ringorder_c);
  order=r->order;
  block0=r->block0;
  block1=r->block1;
  firstwv=NULL;
  polys_wv=r->wvhdl;
  if (order[0]==ringorder_S ||order[0]==ringorder_s)
  {
    order++;
    block0++;
    block1++;
    polys_wv++;
  }
  pFDeg=pTotaldegree;
  /*------- only one real block ----------------------*/
  pLexOrder=FALSE;
  pMixedOrder=FALSE;
  if (pOrdSgn == 1) pLDeg = ldegb;
  else              pLDeg = ldeg0;
  /*======== ordering type is (_,c) =========================*/
  if ((order[0]==ringorder_unspec)
      ||(
    ((order[1]==ringorder_c)||(order[1]==ringorder_C)
     ||(order[1]==ringorder_S)
     ||(order[1]==ringorder_s))
    && (order[0]!=ringorder_M)
    && (order[2]==0))
    )
  {
    if ((order[0]!=ringorder_unspec)
    && ((order[1]==ringorder_C)||(order[1]==ringorder_S)||
        (order[1]==ringorder_s)))
      pComponentOrder=-1;
    if (pOrdSgn == -1) pLDeg = ldeg0c;
    if ((order[0] == ringorder_lp) || (order[0] == ringorder_ls))
    {
      pLexOrder=TRUE;
      pLDeg = ldeg1c;
    }
    if (order[0] == ringorder_wp || order[0] == ringorder_Wp ||
        order[0] == ringorder_ws || order[0] == ringorder_Ws)
      pFDeg = pWTotaldegree;
    firstBlockEnds=block1[0];
  }
  /*======== ordering type is (c,_) =========================*/
  else if (((order[0]==ringorder_c)
            ||(order[0]==ringorder_C)
            ||(order[0]==ringorder_S)
            ||(order[0]==ringorder_s))
  && (order[1]!=ringorder_M)
  &&  (order[2]==0))
  {
    /* pLDeg = ldeg0; is standard*/
    if ((order[0]==ringorder_C)||(order[0]==ringorder_S)||
        order[0]==ringorder_s)
      pComponentOrder=-1;
    if ((order[1] == ringorder_lp) || (order[1] == ringorder_ls))
    {
      pLexOrder=TRUE;
      pLDeg = ldeg1c;
    }
    firstBlockEnds=block1[1];
    if (order[1] == ringorder_wp || order[1] == ringorder_Wp ||
        order[1] == ringorder_ws || order[1] == ringorder_Ws)
      pFDeg = pWTotaldegree;
  }
  /*------- more than one block ----------------------*/
  else
  {
    //pGetVarIndicies(pVariables, pVarOffset, pVarCompIndex, pVarLowIndex,
    //                pVarHighIndex);
    //pLexOrder=TRUE;
    pVectorOut=order[0]==ringorder_c;
    if ((pVectorOut)||(order[0]==ringorder_C)||(order[0]==ringorder_S)||(order[0]==ringorder_s))
    {
      if(block1[1]!=pVariables) pLexOrder=TRUE;
      firstBlockEnds=block1[1];
    }
    else
    {
      if(block1[0]!=pVariables) pLexOrder=TRUE;
      firstBlockEnds=block1[0];
    }
    /*the number of orderings:*/
    i = 0;
    while (order[++i] != 0);
    do
    {
      i--;
      HighSet(i, order[i]);/*sets also pMixedOrder to TRUE, if...*/
    }
    while (i != 0);

    if ((order[0]!=ringorder_c)
        && (order[0]!=ringorder_C)
        && (order[0]!=ringorder_S)
        && (order[0]!=ringorder_s))
    {
      pLDeg = ldeg1c;
    }
    else
    {
      pLDeg = ldeg1;
    }
    pFDeg = pWTotaldegree; // may be improved: pTotaldegree for lp/dp/ls/.. blocks
  }
  if (complete)
  {
    if ((pLexOrder) || (pOrdSgn==-1))
    {
      test &= ~Sy_bit(OPT_REDTAIL); /* noredTail */
    }
    pSetm=rSetm;
  }
  if (pFDeg!=pWTotaldegree) pFDeg=pTotaldegree;
}

/* -------------------------------------------------------- */

static Exponent_t pMultT_nok;
/*2
* update the polynomial a by multipying it by
* the (number) coefficient
* and the exponent vector (of) exp (a well initialized polynomial)
*/
poly pMultT(poly a, poly exp )
{
  int i;
  number t,x,y=pGetCoeff(exp);
  poly aa=a;
  poly prev=NULL;

  pMultT_nok = pGetComp(exp);
#ifdef PDEBUG
  pTest(a);
  pTest(exp);
#endif
  while (a !=NULL)
  {
    x=pGetCoeff(a);
    t=nMult(x/*pGetCoeff(a)*/,y/*pGetCoeff(exp)*/);
    nDelete(&x/*pGetCoeff(a)*/);
    pSetCoeff0(a,t);
    if (nIsZero(t))
    {
      if (prev==NULL) { pDelete1(&a); aa=a; }
      else            { pDelete1(&prev->next); a=prev->next;}
    }
    else
    {
      {
        if (pMultT_nok)  /* comp of exp != 0 */
        {
          if (pGetComp(a) != 0)
          {
            return NULL /*FALSE*/;
          }
        }
        pMonAddOn(a,exp);
      }
      prev=a;
      pIter(a);
    }
  }
  pMultT_nok=0;
  pTest(aa);
  return aa; /*TRUE*/
}

/*2
* multiply p1 with p2, p1 and p2 are destroyed
* do not put attention on speed: the procedure is only used in the interpreter
*/
poly pMult(poly p1, poly p2)
{
  poly res, r, rn, a;
  BOOLEAN cont;

  if ((p1!=NULL) && (p2!=NULL))
  {
#ifdef PDEBUG
    pTest(p1);
    pTest(p2);
#endif
    cont = TRUE;
    a = p1;
    if (pNext(p2)!=NULL)
      a = pCopy(a);
    else
      cont = FALSE;
    res = pMultT(a, p2);
    if (pMultT_nok)
    {
      if (cont) pDelete(&p1);
      pDelete(&res);
      pDelete(&p2);
      return NULL;
    }
    pTest(res);
    pDelete1(&p2);
    r = res;
    if (r!=NULL) rn = pNext(r);
    else         rn=NULL;
    while (cont)
    {
      if (pNext(p2)==NULL)
      {
        a = p1;
        cont = FALSE;
      }
      else
      {
        a = pCopy(p1);
      }
      a=pMultT(a, p2); //sets pMultT_nok
      pTest(a);
      if (pMultT_nok)
      {
        if (cont) pDelete(&p1);
        pDelete(&a);
        pDelete(&res);
        pDelete(&p2);
        return NULL;
      }
      while ((rn!=NULL) && (pComp0(rn,a)>0))
      {
        r = rn;
        pIter(rn);
      }
      if (r!=NULL) pNext(r) = rn = pAdd(a, rn);
      else         res=r=a;
      pDelete1(&p2);
    }
    pTest(res);
    return res;
  }
  pDelete(&p1);
  pDelete(&p2);
  return NULL;
}

/*2
* update a by multiplying it with c (c will not be destroyed)
*/
void pMultN(poly a, number c)
{
  number t;

  while (a!=NULL)
  {
    t=nMult(pGetCoeff(a), c);
    //nNormalize(t);
    pSetCoeff(a,t);
    pIter(a);
  }
}

/*2
* return a copy of the poly a times the number c (a,c will not be destroyed)
*/
poly pMultCopyN(poly a, number c)
{
  poly result=NULL,hp;

  if (a != NULL)
  {
    result=pNew();
    pCopy2(result,a);
    pNext(result)=NULL;
    pGetCoeff(result)=nMult(pGetCoeff(a),c);
    pIter(a);
    hp=result;
    while (a!=NULL)
    {
      hp=pNext(hp)=pNew();
      pCopy2(hp,a);
      pSetCoeff0(hp,nMult(pGetCoeff(a), c));
      pIter(a);
    }
    pNext(hp)=NULL;
  }
  return result;
}

/*2
* assumes that the head term of b is a multiple of the head term of a
* and return the multiplicant *m
*/
poly pDivide(poly a, poly b)
{
  int i;
  poly result=pInit();

  for(i=(int)pVariables; i; i--)
    pSetExp(result,i, pGetExp(a,i)- pGetExp(b,i));
  pSetComp(result, pGetComp(a) - pGetComp(b));
  pSetm(result);
  return result;
}

/*2
* divides a by the monomial b, ignores monomials wihich are not divisible
* assumes that b is not NULL
*/
poly pDivideM(poly a, poly b)
{
  if (a==NULL) return NULL;
  poly result=a;
  poly prev=NULL;
  int i;
  number inv=nInvers(pGetCoeff(b));

  while (a!=NULL)
  {
    if (pDivisibleBy(b,a))
    {
      for(i=(int)pVariables; i; i--)
         pSubExp(a,i, pGetExp(b,i));
      pSubComp(a, pGetComp(b));
      pSetm(a);
      prev=a;
      pIter(a);
    }
    else
    {
      if (prev==NULL)
      {
        pDelete1(&result);
        a=result;
      }
      else
      {
        pDelete1(&pNext(prev));
        a=pNext(prev);
      }
    }
  }
  pMultN(result,inv);
  nDelete(&inv);
  pDelete(&b);
  return result;
}

/*2
* returns the LCM of the head terms of a and b in *m
*/
void pLcm(poly a, poly b, poly m)
{
  int i;
  for (i=pVariables; i; i--)
  {
    pSetExp(m,i, max( pGetExp(a,i), pGetExp(b,i)));
  }
  pSetComp(m, max(pGetComp(a), pGetComp(b)));
  /* Don't do a pSetm here, otherwise hres/lres chockes */
}

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
poly pmInit(char *st, BOOLEAN &ok)
{
  int i,j;
  ok=FALSE;
  BOOLEAN b=FALSE;
  poly rc = pInit();
  char *s = nRead(st,&(rc->coef));
  if (s==st)
  /* i.e. it does not start with a coeff: test if it is a ringvar*/
  {
    j = rIsRingVar(s);
    if (j >= 0)
    {
      pIncrExp(rc,1+j);
      goto done;
    }
  }
  else
    b=TRUE;
  while (*s!='\0')
  {
    char ss[2];
    ss[0] = *s++;
    ss[1] = '\0';
    j = rIsRingVar(ss);
    if (j >= 0)
    {
      s = eati(s,&i);
      pAddExp(rc,1+j, (Exponent_t)i);
    }
    else
    {
      if ((s!=st)&&isdigit(st[0]))
      {
        errorreported=TRUE;
      }
      pDelete(&rc);
      return NULL;
    }
  }
done:
  ok=!errorreported;
  if (nIsZero(pGetCoeff(rc))) pDelete1(&rc);
  else
  {
    pSetm(rc);
  }
  return rc;
}

/*2
*make p homgeneous by multiplying the monomials by powers of x_varnum
*/
poly pHomogen (poly p, int varnum)
{
  poly q=NULL;
  poly res;
  int  o,ii;

  if (p!=NULL)
  {
    if ((varnum < 1) || (varnum > pVariables))
    {
      return NULL;
    }
    o=pWTotaldegree(p);
    q=pNext(p);
    while (q != NULL)
    {
      ii=pWTotaldegree(q);
      if (ii>o) o=ii;
      pIter(q);
    }
    q = pCopy(p);
    res = q;
    while (q != NULL)
    {
      ii = o-pWTotaldegree(q);
      if (ii!=0)
      {
        pAddExp(q,varnum, (Exponent_t)ii);
        pSetm(q);
      }
      pIter(q);
    }
    q = pOrdPolyInsertSetm(res);
  }
  return q;
}


/*2
*replaces the maximal powers of the leading monomial of p2 in p1 by
*the same powers of n, utility for dehomogenization
*/
poly pDehomogen (poly p1,poly p2,number n)
{
  polyset P;
  int     SizeOfSet=5;
  int     i;
  poly    p;
  number  nn;

  P = (polyset)Alloc0(5*sizeof(poly));
  //for (i=0; i<5; i++)
  //{
  //  P[i] = NULL;
  //}
  pCancelPolyByMonom(p1,p2,&P,&SizeOfSet);
  p = P[0];
  //P[0] = NULL ;// for safety, may be remoeved later
  for (i=1; i<SizeOfSet; i++)
  {
    if (P[i] != NULL)
    {
      nPower(n,i,&nn);
      pMultN(P[i],nn);
      p = pAdd(p,P[i]);
      //P[i] =NULL; // for safety, may be removed later
      nDelete(&nn);
    }
  }
  Free((ADDRESS)P,SizeOfSet*sizeof(poly));
  return p;
}

/*4
*Returns the exponent of the maximal power of the leading monomial of
*p2 in that of p1
*/
static int pGetMaxPower (poly p1,poly p2)
{
  int     i,k,res = 32000; /*a very large integer*/

  if (p1 == NULL) return 0;
  for (i=1; i<=pVariables; i++)
  {
    if ( pGetExp(p2,i) != 0)
    {
      k =  pGetExp(p1,i) /  pGetExp(p2,i);
      if (k < res) res = k;
    }
  }
  return res;
}

/*2
*Returns as i-th entry of P the coefficient of the (i-1) power of
*the leading monomial of p2 in p1
*/
void pCancelPolyByMonom (poly p1,poly p2,polyset * P,int * SizeOfSet)
{
  int   maxPow;
  poly  p,qp,Coeff;

  if (*P == NULL)
  {
    *P = (polyset) Alloc(5*sizeof(poly));
    *SizeOfSet = 5;
  }
  p = pCopy(p1);
  while (p != NULL)
  {
    qp = p->next;
    p->next = NULL;
    maxPow = pGetMaxPower(p,p2);
    Coeff = pDivByMonom(p,p2);
    if (maxPow > *SizeOfSet)
    {
      pEnlargeSet(P,*SizeOfSet,maxPow+1-*SizeOfSet);
      *SizeOfSet = maxPow+1;
    }
    (*P)[maxPow] = pAdd((*P)[maxPow],Coeff);
    pDelete(&p);
    p = qp;
  }
}

/*2
*returns the leading monomial of p1 divided by the maximal power of that
*of p2
*/
poly pDivByMonom (poly p1,poly p2)
{
  int     k, i;

  if (p1 == NULL) return NULL;
  k = pGetMaxPower(p1,p2);
  if (k == 0)
    return pHead(p1);
  else
  {
    number n;
    poly p = pInit();

    p->next = NULL;
    for (i=1; i<=pVariables; i++)
    {
       pSetExp(p,i, pGetExp(p1,i)-k* pGetExp(p2,i));
    }
    nPower(p2->coef,k,&n);
    pSetCoeff0(p,nDiv(p1->coef,n));
    nDelete(&n);
    pSetm(p);
    return p;
  }
}
/*----------utilities for syzygies--------------*/
poly pTakeOutComp(poly * p, int k)
{
  poly q = *p,qq=NULL,result = NULL;

  if (q==NULL) return NULL;
  if (pGetComp(q)==k)
  {
    result = q;
    while ((q!=NULL) && (pGetComp(q)==k))
    {
      pSetComp(q,0);
      pSetmComp(q);
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (pGetComp(q) > k) 
  {
    pDecrComp(q);
    pSetmComp(q);
  }
  poly pNext_q;
  while ((pNext_q=pNext(q))!=NULL)
  {
    if (pGetComp(pNext_q)==k)
    {
      if (result==NULL)
      {
        result = pNext_q;
        qq = result;
      }
      else
      {
        pNext(qq) = pNext_q;
        pIter(qq);
      }
      pNext(q) = pNext(pNext_q);
      pNext(qq) =NULL;
      pSetComp(qq,0);
      pSetmComp(qq);
    }
    else
    {
      /*pIter(q);*/ q=pNext_q;
      if (pGetComp(q) > k) 
      {
        pDecrComp(q);
        pSetmComp(q);
      }
    }
  }
  return result;
}

// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
void pTakeOutComp(poly *r_p, Exponent_t comp, poly *r_q, int *lq)
{
  spolyrec pp, qq;
  poly p, q, p_prev;
  int l = 0;

#ifdef HAVE_ASSUME
  int lp = pLength(*r_p);
#endif

  pNext(&pp) = *r_p;
  p = *r_p;
  p_prev = &pp;
  q = &qq;

  while(p != NULL)
  {
    while (pGetComp(p) == comp)
    {
      pNext(q) = p;
      pIter(q);
      pSetComp(p, 0);
      pSetmComp(p);
      pIter(p);
      l++;
      if (p == NULL)
      {
        pNext(p_prev) = NULL;
        goto Finish;
      }
    }
    pNext(p_prev) = p;
    p_prev = p;
    pIter(p);
  }

  Finish:
  pNext(q) = NULL;
  *r_p = pNext(&pp);
  *r_q = pNext(&qq);
  *lq = l;
#ifdef HAVE_ASSUME
  assume(pLength(*r_p) + pLength(*r_q) == lp);
#endif
  pTest(*r_p);
  pTest(*r_q);
}

void pDecrOrdTakeOutComp(poly *r_p, Exponent_t comp, Order_t order,
                         poly *r_q, int *lq)
{
  spolyrec pp, qq;
  poly p, q, p_prev;
  int l = 0;

  pNext(&pp) = *r_p;
  p = *r_p;
  p_prev = &pp;
  q = &qq;

#ifdef HAVE_ASSUME
  if (p != NULL)
  {
    while (pNext(p) != NULL)
    {
      assume(pGetOrder(p) >= pGetOrder(pNext(p)));
      pIter(p);
    }
  }
  p = *r_p;
#endif

  while (p != NULL && pGetOrder(p) > order) pIter(p);

  while(p != NULL && pGetOrder(p) == order)
  {
    while (pGetComp(p) == comp)
    {
      pNext(q) = p;
      pIter(q);
      pIter(p);
      pSetComp(p, 0);
      pSetmComp(p);
      l++;
      if (p == NULL || pGetOrder(p) != order)
      {
        pNext(p_prev) = p;
        goto Finish;
      }
    }
    pNext(p_prev) = p;
    p_prev = p;
    pIter(p);
  }

  Finish:
  pNext(q) = NULL;
  *r_p = pNext(&pp);
  *r_q = pNext(&qq);
  *lq = l;
}

#if 1
poly pTakeOutComp1(poly * p, int k)
{
  poly q = *p;

  if (q==NULL) return NULL;

  poly qq=NULL,result = NULL;

  if (pGetComp(q)==k)
  {
    result = q; /* *p */
    while ((q!=NULL) && (pGetComp(q)==k))
    {
      pSetComp(q,0);
      pSetmComp(q);
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
//  if (pGetComp(q) > k) pGetComp(q)--;
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
    {
      if (result==NULL)
      {
        result = pNext(q);
        qq = result;
      }
      else
      {
        pNext(qq) = pNext(q);
        pIter(qq);
      }
      pNext(q) = pNext(pNext(q));
      pNext(qq) =NULL;
      pSetComp(qq,0);
      pSetmComp(qq);
    }
    else
    {
      pIter(q);
//      if (pGetComp(q) > k) pGetComp(q)--;
    }
  }
  return result;
}
#endif

void pDeleteComp(poly * p,int k)
{
  poly q;

  while ((*p!=NULL) && (pGetComp(*p)==k)) pDelete1(p);
  if (*p==NULL) return;
  q = *p;
  if (pGetComp(q)>k) 
  {
    pDecrComp(q);
    pSetmComp(q);
  }
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
      pDelete1(&(pNext(q)));
    else
    {
      pIter(q);
      if (pGetComp(q)>k) 
      {
        pDecrComp(q);
        pSetmComp(q);
      }
    }
  }
}
/*----------end of utilities for syzygies--------------*/

/*2
* pair has no common factor ? or is no polynomial
*/
BOOLEAN pHasNotCF(poly p1, poly p2)
{

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  int i = 1;
  loop
  {
    if ((pGetExp(p1, i) > 0) && (pGetExp(p2, i) > 0))   return FALSE;
    if (i == pVariables)                                return TRUE;
    i++;
  }
}


/*2
*should return 1 if p divides q and p<q,
*             -1 if q divides p and q<p
*              0 otherwise
*/
int     pDivComp(poly p, poly q)
{
  if (pGetComp(p) == pGetComp(q))
  {
    int i=pVariables;
    long d;
    BOOLEAN a=FALSE, b=FALSE;
    for (; i>0; i--)
    {
      d = pGetExpDiff(p, q, i);
      if (d)
      {
        if (d < 0)
        {
          if (b) return 0;
          a =TRUE;
        }
        else
        {
          if (a) return 0;
          b = TRUE;
        }
      }
    }
    if (a) return 1;
    else if (b)  return -1;
  }
  return 0;
}
/*2
*divides p1 by its leading monomial
*/
void pNorm(poly p1)
{
  poly h;
  number k, c;

  if (p1!=NULL)
  {
    if (!nIsOne(pGetCoeff(p1)))
    {
      nNormalize(pGetCoeff(p1));
      k=pGetCoeff(p1);
      c = nInit(1);
      pSetCoeff0(p1,c);
      h = pNext(p1);
      while (h!=NULL)
      {
        c=nDiv(pGetCoeff(h),k);
        if (!nIsOne(c)) nNormalize(c);
        pSetCoeff(h,c);
        pIter(h);
      }
      nDelete(&k);
    }
    else
    {
      h = pNext(p1);
      while (h!=NULL)
      {
        nNormalize(pGetCoeff(h));
        pIter(h);
      }
    }
  }
}

/*2
*normalize all coeffizients
*/
void pNormalize(poly p)
{
  if (rField_has_simple_inverse()) return; /* Z/p, GF(p,n), R, long R/C */
  while (p!=NULL)
  {
    nTest(pGetCoeff(p));
    nNormalize(pGetCoeff(p));
    pIter(p);
  }
}

// splits p into polys with Exp(n) == 0 and Exp(n) != 0
// Poly with Exp(n) != 0 is reversed
static void pSplitAndReversePoly(poly p, int n, poly *non_zero, poly *zero)
{
  if (p == NULL)
  {
    *non_zero = NULL;
    *zero = NULL;
    return;
  }
  spolyrec sz;
  poly z, n_z, next;
  z = &sz;
  n_z = NULL;

  while(p != NULL)
  {
    next = pNext(p);
    if (pGetExp(p, n) == 0)
    {
      pNext(z) = p;
      pIter(z);
    }
    else
    {
      pNext(p) = n_z;
      n_z = p;
    }
    p = next;
  }
  pNext(z) = NULL;
  *zero = pNext(&sz);
  *non_zero = n_z;
  return;
}
  
/*3
* substitute the n-th variable by 1 in p
* destroy p
*/
static poly pSubst1 (poly p,int n)
{
  poly qq,result = NULL;
  poly zero, non_zero;

  // reverse, so that add is likely to be linear
  pSplitAndReversePoly(p, n, &non_zero, &zero);

  while (non_zero != NULL)
  {
    assume(pGetExp(non_zero, n) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    pSetExp(qq,n,0);
    pSetm(qq);
    result = pAdd(result,qq);
  }
  p = pAdd(result, zero);
  pTest(p);
  return p;
}

/*3
* substitute the n-th variable by number e in p
* destroy p
*/
static poly pSubst2 (poly p,int n, number e)
{
  assume( ! nIsZero(e) );
  poly qq,result = NULL;
  number nn, nm;
  poly zero, non_zero;

  // reverse, so that add is likely to be linear
  pSplitAndReversePoly(p, n, &non_zero, &zero);

  while (non_zero != NULL)
  {
    assume(pGetExp(non_zero, n) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    nPower(e, pGetExp(qq, n), &nn);
    nm = nMult(nn, pGetCoeff(qq));
    pSetCoeff(qq, nm);
    nDelete(&nn);
    pSetExp(qq, n, 0);
    pSetm(qq);
    result = pAdd(result,qq);
  }
  p = pAdd(result, zero);
  pTest(p);
  return p;
}


/* delete monoms whose n-th exponent is different from zero */
poly pSubst0(poly p, int n)
{
  spolyrec res;
  poly h = &res;
  pNext(h) = p;

  while (pNext(h)!=NULL)
  {
    if (pGetExp(pNext(h),n)!=0)
    {
      pDelete1(&pNext(h));
    }
    else
    {
      pIter(h);
    }
  }
  pTest(pNext(&res));
  return pNext(&res);
}

/*2
* substitute the n-th variable by e in p
* destroy p
*/
poly pSubst(poly p, int n, poly e)
{
  if (e == NULL) return pSubst0(p, n);

  if (pIsConstant(e))
  {
    if (nIsOne(pGetCoeff(e))) return pSubst1(p,n);
    else return pSubst2(p, n, pGetCoeff(e));
  }

  int exponent,i;
  poly h, res, m;
  Exponent_t *me,*ee;
  number nu,nu1;

  me=(Exponent_t *)Alloc((pVariables+1)*sizeof(Exponent_t));
  ee=(Exponent_t *)Alloc((pVariables+1)*sizeof(Exponent_t));
  if (e!=NULL) pGetExpV(e,ee);
  res=NULL;
  h=p;
  while (h!=NULL)
  {
    if ((e!=NULL) || (pGetExp(h,n)==0))
    {
      m=pHead(h);
      pGetExpV(m,me);
      exponent=me[n];
      me[n]=0;
      for(i=pVariables;i>0;i--)
        me[i]+=exponent*ee[i];
      pSetExpV(m,me);
      if (e!=NULL)
      {
        nPower(pGetCoeff(e),exponent,&nu);
        nu1=nMult(pGetCoeff(m),nu);
        nDelete(&nu);
        pSetCoeff(m,nu1);
      }
      res=pAdd(res,m);
    }
    pDelete1(&h);
  }
  Free((ADDRESS)me,(pVariables+1)*sizeof(Exponent_t));
  Free((ADDRESS)ee,(pVariables+1)*sizeof(Exponent_t));
  return res;
}

BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=pVariables; j; j--)
    if ( pGetExp(p,j) >  pGetExp(lcm,j)) return FALSE;
  if ( pGetComp(p) !=  pGetComp(lcm)) return FALSE;
  for (j=pVariables; j; j--)
  {
    if (pGetExp(p1,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=pVariables; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
    else if (pGetExp(p2,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=pVariables; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k!=0 ; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
  }
  return FALSE;
}

int pWeight(int i)
{
  if ((firstwv==NULL) || (i>firstBlockEnds))
  {
    return 1;
  }
  return firstwv[i-1];
}

