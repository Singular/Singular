/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.cc,v 1.16 2000-08-14 12:55:54 obachman Exp $ */

/*
* ABSTRACT - set order (=number of monomial) for dp
*/

/* includes */
#include "mod2.h"

#ifdef TEST_MAC_ORDER
#include <omalloc.h>
#include "structs.h"
#include "binom.h"

#include "tok.h"
#include "febase.h"
#include "polys.h"
#include "polys-comp.h"
#include "ring.h"

extern int  pComponentOrder;
/* ----------- global variables, set by bBinomSet --------------------- */
int *bBinomials=NULL;
static int  bSize;
int         bHighdeg;
int         bHighdeg_1;
BOOLEAN     bNoAdd;

/*0 implementation*/
static inline int bBinom(int i,int j)
{
  return bBinomials[(j-1)*(bHighdeg_1)+i-j+1/*j-1,i-j*/];
}

void bSetm(poly p)
{
#ifdef TEST_MAC_DEBUG
  int ord = pTotaldegree(p);
  p->Order=ord;

  if (ord<bHighdeg)
  {
    int i=1;
    ord = -INT_MAX;
    //int expsum=0;
    //while (i<=pVariables)
    //{
    //  expsum += pGetExp(p,i);
    //  ord += bBinom(expsum,i);
    //  expsum++;
    //  i++;
    //}

    int *ip=bBinomials+pGetExp(p,1);
    loop
    {
      ord += (*ip);
      if (i==pVariables) break;
      i++;
      #ifdef PDEBUG
      if(pGetExp(p,i)<0)
        Print("neg. Exp %d:%d in %s:%d\n",i,pGetExp(p,i),__FILE__,__LINE__);
      #endif
      ip+=bHighdeg_1+pGetExp(p,i);
    }
    if (ord>=0)
    {
      wrp(p);
      Print("maxdeg exceeded in bSetm:bHighdeg=%d, ord=%d,deg=%d,B[%d]i,Bsize=%d\n",
      bHighdeg,ord,pTotaldegree(p),ip-bBinomials,bSize/sizeof(int));
    }
  }
  p->MOrder=ord;
#else
  _bSetm(p);
#endif
}

// avoid checking for overflow:
void bSetm0(poly p)
{
#ifdef TEST_MAC_DEBUG
  p->Order=pTotaldegree(p);

  int i=1;
  int ord = -INT_MAX;
  //int expsum=0;
  //while (i<=pVariables)
  //{
  //  expsum += pGetExp(p,i);
  //  ord += bBinom(expsum,i);
  //  expsum++;
  //  i++;
  //}

  int *ip=bBinomials+pGetExp(p,1);
  loop
  {
    ord += (*ip);
    if (i==pVariables) break;
    i++;
    #ifdef PDEBUG
    if(pGetExp(p,i)<0)
      Print("neg. Exp %d:%d in %s:%d\n",i,pGetExp(p,i),__FILE__,__LINE__);
    #endif
    ip+=bHighdeg_1+pGetExp(p,i);
  }
  if (ord>=0)
  {
    wrp(p);
    Print("maxdeg exceeded in bSetm0:bHighdeg=%d, ord=%d,deg=%d,B[%d],Bsize:%d\n",
    bHighdeg,ord,pTotaldegree(p),ip-bBinomials,bSize/sizeof(int));
  }
  p->MOrder=ord;
#else
  _bSetm0(p);
#endif
}

#ifdef TEST_MAC_DEBUG
int bComp1dpc_org(poly p1, poly p2)
#else
int bComp1dpc(poly p1, poly p2)
#endif
{
#ifdef TEST_MAC_DEBUG
  int o1=p1->MOrder, o2=p2->MOrder;
#else
  int o1=p1->Order, o2=p2->Order;
#endif

   if (o1!=o2)
   {
     if(o1>o2) return 1;
     else    return -1;
   }

  /* now o1==o2: */
  if (o1>0)
  {
    register long d;
    if (pVariablesW >2 )
    {
      if (pVariablesW & 1)
      {
        _pMonCmp_2i_1(p1,p2, pVariablesW, d, goto NotEqual, return 0);
      }
      _pMonCmp_2i(p1,p2, pVariablesW, d, goto NotEqual, return 0);
    }
    if (pVariablesW == 1)
    {
      _pMonCmp_1(p1,p2, d, goto NotEqual, return 0);
    }
    _pMonCmp_2(p1,p2, d, goto NotEqual, return 0);

    NotEqual:
    if (d>0) return -1; /*pLexSgn*/
    return 1; /*-pLexSgn*/
  }
  o1=pGetComp(p1)-pGetComp(p2);
  if (o1 == 0) return 0;
  if (o1 > 0) return -pComponentOrder;
  return pComponentOrder;
  return 0;
}

#ifdef TEST_MAC_DEBUG
static int comp1dpc(poly p1, poly p2)
{
  /*4 compare monomials by order then revlex*/
  int d=p1->Order - p2->Order;
  if (d == 0 /*p1->Order == p2->Order*/)
  {
    int i = pVariables;
    if ((p1->exp[i] == p2->exp[i]))
    {
      do
      {
        i--;
        if (i <= 1)
        {
           /*4 handle module case:*/
           if (p1->exp[0]==p2->exp[0])
             return 0;
           else if (p1->exp[0] > p2->exp[0])
             return -pComponentOrder;
           else
             return pComponentOrder;
        }
      } while ((p1->exp[i] == p2->exp[i]));
    }
    if (p1->exp[i] < p2->exp[i])
      return 1;
    return -1;
  }
  else if (d > 0 /*p1->Order > p2->Order*/)
    return 1;
  return -1;
}
int bComp1dpc(poly p1, poly p2)
{
  int m=bComp1dpc_org(p1,p2);
  int c=comp1dpc(p1,p2);
  if (c!=m)
  {
    Print("comp mismatch\n");
    wrp(p1);
    Print(" ");
    wrp(p2);
    Print("mac:%d, org:%d\n",m,c);
  }
  return c;
}
#endif

int bComp1cdp(poly p1, poly p2)
{
  int o1=pGetComp(p1)-pGetComp(p2);
  if (o1 > 0) return -pComponentOrder;
  if (o1 < 0) return pComponentOrder;

  o1=p1->Order; int o2=p2->Order;
  if (o1 > o2) return 1;
  if (o1 < o2) return -1;

  /* now o1==o2: */
  if (o1>0)
  {
    int i = pVariables;
    while ((pGetExp(p1,i)==pGetExp(p2,i)) && (i>1))
      i--;
    if (i>1)
    {
      if (pGetExp(p1,i) < pGetExp(p2,i)) return 1;
      return -1;
    }
  }
  return 0;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the first one
* this works for the polynomial case with degree orderings
* (both c,dp and dp,c)
*/
static int bLDegb(poly p,int *l)
{
  Exponent_t k=pGetComp(p);
  int o = pTotaldegree(p);
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
* and the degree of the monomial with maximal degree: the last one
*/
static int bLDeg0(poly p,int *l)
{
  Exponent_t k=pGetComp(p);
  int ll=1;

  while ((pNext(p)!=NULL) && (pGetComp(pNext(p))==k))
  {
    pIter(p);
    ll++;
  }
  *l=ll;
  return (pTotaldegree(p));
}

/*
* (i,j) in bBinomials[j-1,i-j+1]
* table size is: pVariables * (bHighdeg+1)
* bHighdeg_1==bHighdeg+1
*/
void bBinomSet(int * orders)
{
  bNoAdd=TRUE;
#if 0
  int bbHighdeg=1;
  long long t=1;
  long long h_n=1+pVariables;
  while ((bbHighdeg<256)
  && ((t=(((long long)t*(long long)h_n)/(long long)bbHighdeg))<INT_MAX))
  {
    bbHighdeg++;
    h_n++;
  }
  if((bBinomials==NULL)
  ||(bbHighdeg!=bHighdeg)
  ||(bSize!=(pVariables*bbHighdeg*sizeof(int))))
  {
    bHighdeg=bbHighdeg;
    bHighdeg_1=bbHighdeg;
    bHighdeg--;

    if(bBinomials!=NULL) omFreeSize((ADDRESS)bBinomials,bSize);
    bSize = pVariables*bHighdeg_1*sizeof(int);
    bBinomials = (int*)omAlloc(bSize);

    //Print("max deg=%d, table size=%d bytes\n",bHighdeg,bSize);

    for(int j=1;j<=bHighdeg;j++)
    {
      bBinomials[j/*0,j*/] = j;
      for (int i=1;i<pVariables;i++)
      {
        bBinomials[i*(bHighdeg_1)+j/*i,j*/]
        = ((long long)bBinomials[(i-1)*(bHighdeg_1)+j/*i-1,j*/])
           *((long long)(j+i))/((long long)(i+1));
        if (bBinomials[i*(bHighdeg_1)+j/*i,j*/]<=0)
        {
          Print("overflow in bBinomials:i=%s,j=%d\n",i,j);
        }
      }
    }
    for (int i=0;i<pVariables;i++)
    {
      bBinomials[i*(bHighdeg_1)/*i,0*/]=0;
    }
    Print("last table entry:%d,%d - %d\n", pVariables,bHighdeg,pVariables*(bHighdeg_1)+bHighdeg);
  }
#else
  bHighdeg=1;
  long long t=1;
  long long h_n=1+pVariables;
  while ((bHighdeg<256)
  && ((t=(((long long)t*(long long)h_n)/(long long)bHighdeg))<INT_MAX))
  {
    bHighdeg++;
    h_n++;
  }
  bHighdeg-=2;
  bHighdeg_1=bHighdeg;
  bHighdeg--;

  if(bBinomials!=NULL) omFreeSize((ADDRESS)bBinomials,bSize);
  bSize = (pVariables+1)*bHighdeg_1*sizeof(int);
  bBinomials = (int*)omAlloc(bSize);

  // Print("max deg=%d, table size=%d bytes\n",bHighdeg,bSize);

  for(int j=1;j<=bHighdeg;j++)
  {
    bBinomials[j/*0,j*/] = j;
    for (int i=1;i<=pVariables;i++)
    {
      bBinomials[i*(bHighdeg_1)+j/*i,j*/]
      = ((long long)bBinomials[(i-1)*(bHighdeg_1)+j/*i-1,j*/])
      *((long long)(j+i))/((long long)(i+1));
      //if (bBinomials[i*(bHighdeg_1)+j/*i,j*/]<=0)
      //{
      //  Print("overflow in bBinomials:i=%s,j=%d\n",i,j);
      //}
    }
  }
  for (int i=0;i<pVariables;i++)
  {
    bBinomials[i*(bHighdeg_1)/*i,0*/]=0;
  }
  //  Print("last table entry:%d,%d - %d\n", pVariables,bHighdeg,pVariables*(bHighdeg_1)+bHighdeg);
#endif
  pSetm =bSetm;
  if (orders[0]==ringorder_dp)
    pComp0=bComp1dpc;
  else if (orders[1]==ringorder_dp)
    pComp0=bComp1cdp;
  pFDeg =pTotaldegree;
  pLDeg =bLDegb; /* if pOrdSgn==1 */
}
#endif
