/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: binom.cc,v 1.5 1997-12-03 16:58:30 obachman Exp $ */

/*
* ABSTRACT - set order (=number of monomial) for dp
*/

/* includes */
#include <limits.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "polys.h"
#include "binom.h"

#ifdef TEST_MAC_ORDER
extern int  pComponentOrder;
/* ----------- global variables, set by bBinomSet --------------------- */
static int *bBinomials=NULL;
static int  bSize;
int         bHighdeg;
static int  bHighdeg_1;

/*0 implementation*/
static inline int bBinom(int i,int j)
{
  return bBinomials[(j-1)*(bHighdeg_1)+i-j+1/*j-1,i-j*/];
}

void bSetm(poly p)
{
  int i = pTotaldegree(p);

  if (i<=bHighdeg)
  {
    i=1;
    p->Order = -INT_MAX;
    //int expsum=0;
    //while (i<=pVariables)
    //{
    //  expsum += pGetExp(p,i);
    //  p->Order += bBinom(expsum,i);
    //  expsum++;
    //  i++;
    //}

    int *ip=bBinomials+pGetExp(p,1);
    loop
    {
      p->Order += (*ip);
      if (i==pVariables) break;
      i++;
      ip+=bHighdeg_1+pGetExp(p,i);
    }
  }
  else
   p->Order=i;
}

static int bComp1dpc(poly p1, poly p2)
{
  int o1=p1->Order, o2=p2->Order;
  if (o1 > o2) return 1;
  if (o1 < o2) return -1;

  /* now o1==o2: */
  if (o1>0)
  {
    int i = pVariables;
    while ((i>1) && (pGetExp(p1,i)==pGetExp(p2,i)))
      i--;
    if (i>1)
    {
      if (pGetExp(p1,i) < pGetExp(p2,i)) return 1;
      return -1;
    }
  }
  o1=pGetComp(p1);
  o2=pGetComp(p2);
  if (o1==o2) return 0;
  if (o1>o2) return -pComponentOrder;
  return pComponentOrder;
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
  return (pFDeg(p));
}

/*
* (i,j) in bBinomials[j-1,i-j+1]
* table size is: pVariables * (bHighdeg+1)
* bHighdeg_1==bHighdeg+1
*/
void bBinomSet()
{
  bHighdeg=1;
  long long t=1;
  long long h_n=1+pVariables;
  while ((bHighdeg<512)
  && ((t=(((long long)t*(long long)h_n)/(long long)bHighdeg))<INT_MAX))
  {
    bHighdeg++;
    h_n++;
  }
  bHighdeg_1=bHighdeg;
  bHighdeg--;

  if(bBinomials!=NULL) Free((ADDRESS)bBinomials,bSize);
  bSize = pVariables*(bHighdeg+1)*sizeof(int);
  bBinomials = (int*)Alloc(bSize);

  Print("max deg=%d, table size=%d bytes\n",bHighdeg,bSize);

  for(int j=1;j<=bHighdeg;j++)
  {
    bBinomials[j/*0,j*/] = j;
    for (int i=1;i<pVariables;i++)
    {
      bBinomials[i*(bHighdeg_1)+j/*i,j*/]
      = bBinomials[(i-1)*(bHighdeg_1)+j/*i-1,j*/]*(j+i)/(i+1);
    }
  }
  for (int i=0;i<pVariables;i++)
  {
    bBinomials[i*(bHighdeg_1)/*i,0*/]=0;
  }
  pSetm =bSetm;
  pComp0=bComp1dpc;
  pFDeg =pTotaldegree;
  pLDeg =bLDegb; /* if pOrdSgn==1 */
}
#endif
