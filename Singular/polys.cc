/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/polys.cc,v 1.1.1.1 1997-03-19 13:18:50 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/

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

/* ----------- global variables, set by pChangeRing --------------------- */
/* initializes the internal data from the exp vector */
pSetmProc pSetm;
/* computes length and maximal degree of a POLYnomial */
pLDegProc pLDeg;
/* computes the degree of the initial term, used for std */
pFDegProc pFDeg;
/* the monomial ordering of the head monomials a and b */
pCompProc pComp0;
/* returns -1 if a comes before b, 0 if a=b, 1 otherwise */

/* the number of variables             */
int     pVariables;
/* 1 for polynomial ring, -1 otherwise */
int     pOrdSgn;
/* TRUE for momomial output as x2y, FALSE for x^2*y */
int pShortOut = (int)TRUE;
// it is of type int, not BOOLEAN because it is also in ip
/* TRUE if the monomial ordering is not compatible with pFDeg */
BOOLEAN pLexOrder;
/* TRUE if the monomial ordering has polynomial and power series blocks */
BOOLEAN pMixedOrder;

#ifdef TEST_MAC_ORDER
int  pComponentOrder;
#else
static int  pComponentOrder;
#endif

#ifdef DRING
int      p2;
BOOLEAN  pDRING=FALSE;
#endif

#ifdef SRING
int      pAltVars;
BOOLEAN  pSRING=FALSE;
#endif

#ifdef SDRING
BOOLEAN  pSDRING=FALSE;
#include "polys.inc"
#endif
/* ----------- global variables, set by procedures from hecke/kstd1 ----- */
/* the highest monomial below pHEdge */
poly      ppNoether = NULL;

/* -------------- static variables --------------------------------------- */
/*is the basic comparing procedure during a computation of syzygies*/
static pCompProc pCompOld;
/*for grouping module indecees during computations*/
static int maxBound = 0;
/*contains the headterms for the Schreyer orderings*/
static int* SchreyerOrd;
static int maxSchreyer=0;
static int indexShift=0;
static pLDegProc pLDegOld;

typedef int (*bcompProc)(poly p1, poly p2, int i1, int i2, short * w);
static bcompProc bcomph[20];
static short**   polys_wv;
static short *   firstwv;
static int * block0;
static int * block1;
static int firstBlockEnds;
static int * order;

/*0 implementation*/
/*-------- the several possibilities for pSetm:-----------------------*/
/*2
* define the order of p with respect to lex. ordering, N=1
*/
static void setlex1(poly p)
{
  p->Order = (int)p->exp[1];
}

/*2
* define the order of p with respect to lex. ordering, N>1
*/
static void setlex2(poly p)
{
  p->Order = (((int)p->exp[1])<<(sizeof(short)*8))+(int)p->exp[2];
}

/*2
* define the order of p with respect to a degree ordering
*/
static void setdeg1(poly p)
{
  int i, j = p->exp[1];

  for (i = firstBlockEnds; i>1; i--) j += p->exp[i];
  p->Order = j;
}

/*2
* define the order of p with respect to a degree ordering
* with weigthts
*/
static void setdeg1w(poly p)
{
  int i, j = 0;

  for (i = firstBlockEnds; i>0; i--)
  {
     j += ((int)p->exp[i])*firstwv[i-1];
  }
  p->Order = j;
}

/*-------- the several possibilities for pComp0:-----------------------*/
/*2
* compare the head monomial of p1 and p2 with degrevlex
* handle also module case, pVariables >2
*/
//static int comp1lpc(poly p1, poly p2)
//{
//  int i;
//
//  for (i=1; i<=pVariables; i++)
//  {
//    if (p1->exp[i] > p2->exp[i]) return 1;
//    if (p1->exp[i] < p2->exp[i]) return -1;
//  }
//  /*4 handle module case:*/
//  if (p1->exp[0]==p2->exp[0]) return 0;
//  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//  return pComponentOrder;
//}
static int comp1lpc(poly p1, poly p2)
{
  int dd=p1->Order - p2->Order;
  if (dd > 0) return 1;
  if (dd < 0) return -1;

  short *m1=p1->exp+3;
  short *m2=p2->exp+3;
  int i=pVariables-2;
  short d;

  loop
  {
    d= *m1 - *m2;
    if ( d > 0 /* *m1 > *m2*/) return 1;
    if ( d < 0 /* *m1 < *m2*/) return -1;
    i--;
    if (i==0)
    {
      /*4 handle module case:*/
      if (p1->exp[0]==p2->exp[0]) return 0;
      else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
      return pComponentOrder;
    }
    m1++;m2++;
  }
}

/*2
* compare the head monomial of p1 and p2 with degrevlex
* handle also module case, pVariables <=2
*/
static int comp2lpc(poly p1, poly p2)
{
  int d=p1->Order - p2->Order;
  if (d > 0)
    return 1;
  if (d < 0)
    return -1;
  if (p1->exp[0]==p2->exp[0])
    return 0;
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

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
//static int comp1dpc(poly p1, poly p2)
//{
//  /*4 compare monomials by order then revlex*/
//  int d=p1->Order - p2->Order;
//  if (d==0 /*p1->Order == p2->Order*/)
//  {
//    int i=pVariables;
//    short *m1=p1->exp+/*pVariables*/ i;
//    short *m2=p2->exp+/*pVariables*/ i;
//    d=(*m1)-(*m2);
//    while ( d==0 /* *m1 == *m2*/)
//    {
//      i--;
//      if (i<0)
//      {
//         /*4 handle module case:*/
//         if (p1->exp[0]==p2->exp[0]) return 0;
//         else if ( p1->exp[0] > p2->exp[0] ) return -pComponentOrder;
//         return pComponentOrder;
//      }
//      m1--;m2--;
//      d=(*m1)-(*m2);
//    }
//    if (d<0 /* *m1 < *m2*/ ) return 1;
//    return -1;
//  }
//  else if ( d > 0 /*p1->Order > p2->Order*/) return 1;
//  return -1;
//}

static int comp1Dpc(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  int d=p1->Order - p2->Order;
  if ( d > 0 /*p1->Order > p2->Order*/)
    return 1;
  else if (d < 0 /*p1->Order < p2->Order*/)
    return -1;
  for (i = 1; i<=pVariables; i++)
  {
    if (p1->exp[i] > p2->exp[i])
      return 1;
    else if (p1->exp[i] < p2->exp[i])
      return -1;
  }
  /*4 handle module case:*/
  if (p1->exp[0]==p2->exp[0])
    return 0;
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

//static int comp1lsc(poly p1, poly p2)
//{
//  int i;
//
//  for (i=1; i<=pVariables; i++)
//  {
//    if (p1->exp[i] > p2->exp[i]) return -1;
//    if (p1->exp[i] < p2->exp[i]) return 1;
//  }
//  /*4 handle module case:*/
//  if (p1->exp[0]==p2->exp[0]) return 0;
//  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//  return pComponentOrder;
//}
static int comp1lsc(poly p1, poly p2)
{
  int i;
  short *m1=p1->exp+1;
  short *m2=p2->exp+1;

  for (i=pVariables; i>0; i--,m1++,m2++)
  {
    if (*m1 > *m2) return -1;
    else if (*m1 < *m2) return 1;
  }
  /*4 handle module case:*/
  if (p1->exp[0]==p2->exp[0])
    return 0;
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

//static int comp1dsc(poly p1, poly p2)
//{
//  int i;
//
//  /*4 compare monomials by order then revlex*/
//  if (p1->Order == p2->Order)
//  {
//    i = pVariables;
//    if ((p1->exp[i] == p2->exp[i]))
//    {
//      do
//      {
//        i--;
//        if (i <= 1)
//        {
//          /*4 handle module case:*/
//          if (p1->exp[0]==p2->exp[0]) return 0;
//          else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//          return pComponentOrder;
//        }
//      } while ((p1->exp[i] == p2->exp[i]));
//    }
//    if (p1->exp[i] < p2->exp[i]) return 1;
//    return -1;
//  }
//  else if (p1->Order > p2->Order) return -1;
//  return 1;
//}
static int comp1dsc(poly p1, poly p2)
{
  /*4 compare monomials by order then revlex*/
  int d=p1->Order - p2->Order;
  if (d==0 /*p1->Order == p2->Order*/)
  {
    int i=pVariables;
    short *m1=p1->exp+/*pVariables*/ i;
    short *m2=p2->exp+/*pVariables*/ i;
    while ((*m1 == *m2))
    {
      i--;
      if (i==0)
      {
        /*4 handle module case:*/
        //if (/*p1->exp[0]==p2->exp[0]*/ *m1==*m2) return 0;
        //else if ( *m1 > *m2) return -pComponentOrder;
        if (p1->exp[0]==p2->exp[0])
          return 0;
        else if ( p1->exp[0] > p2->exp[0])
          return -pComponentOrder;
        else
          return pComponentOrder;
      }
      m1--;
      m2--;
    }
    if (*m1 < *m2)
      return 1;
    return -1;
  }
  else if ( d > 0 /*p1->Order > p2->Order*/)
    return -1;
  return 1;
}

static int comp1Dsc(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  int d=p1->Order - p2->Order;
  if (d > 0 /*p1->Order > p2->Order*/)
    return -1;
  else if (d < 0 /*p1->Order < p2->Order*/)
    return 1;
  for (i = 1; i<=pVariables; i++)
  {
    if (p1->exp[i] > p2->exp[i])
      return 1;
    else if (p1->exp[i] < p2->exp[i])
      return -1;
  }
  /*4 handle module case:*/
  if (p1->exp[0]==p2->exp[0])
    return 0;
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

//static int comp1clp(poly p1, poly p2)
//{
//  int i;
//
//  if (p1->exp[0] == p2->exp[0])
//  {
//    for (i=1; i<=pVariables; i++)
//    {
//      if (p1->exp[i] > p2->exp[i]) return 1;
//      if (p1->exp[i] < p2->exp[i]) return -1;
//    }
//    return 0;
//  }
//  /*4 handle module case:*/
//  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//  return pComponentOrder;
//}
static int comp1clp(poly p1, poly p2)
{
  int i;
  short *m1=p1->exp;
  short *m2=p2->exp;

  if (*m1 == *m2)
  {
    m1++;
    m2++;
    for (i=pVariables; i>0; i--,m1++,m2++)
    {
      if (*m1 > *m2)
        return 1;
      if (*m1 < *m2)
        return -1;
    }
    return 0;
  }
  /*4 handle module case:*/
  else if (*m1 > *m2)
    return -pComponentOrder;
  return pComponentOrder;
}

static int comp2clp(poly p1, poly p2)
{
  if (p1->exp[0]==p2->exp[0])
  {
    int dd=p1->Order - p2->Order;
    if (dd > 0)
      return 1;
    if (dd < 0)
      return -1;
    return 0;
  }
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

static int comp1cdp(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  if (p1->exp[0] == p2->exp[0])
  {
    int d= p1->Order - p2->Order;
    if (d == 0 /*p1->Order == p2->Order*/)
    {
      i = pVariables;
      if ((p1->exp[i] == p2->exp[i]))
      {
        do
        {
          i--;
          if (i <= 1)
            return 0;
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
  /*4 handle module case:*/
  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
  return pComponentOrder;
}

static int comp1cDp(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  if (p1->exp[0] == p2->exp[0])
  {
    int d= p1->Order - p2->Order;
    if ( d > 0 /*p1->Order > p2->Order*/)
      return 1;
    else if (d < 0 /*p1->Order < p2->Order*/)
      return -1;
    for (i = 1; i<=pVariables; i++)
    {
      if (p1->exp[i] > p2->exp[i])
        return 1;
      else if (p1->exp[i] < p2->exp[i])
        return -1;
    }
    return 0;
  }
  /*4 handle module case:*/
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

//static int comp1cls(poly p1, poly p2)
//{
//  int i;
//
//  if (p1->exp[0] == p2->exp[0])
//  {
//    for (i=1; i<=pVariables; i++)
//    {
//      if (p1->exp[i] > p2->exp[i]) return -1;
//      if (p1->exp[i] < p2->exp[i]) return 1;
//    }
//    return 0;
//  }
//  /*4 handle module case:*/
//  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//  return pComponentOrder;
//}
static int comp1cls(poly p1, poly p2)
{
  int i;
  short *m1=p1->exp;
  short *m2=p2->exp;

  if (*m1 == *m2)
  {
    m1++;
    m2++;
    for (i=pVariables; i>0; i--,m1++,m2++)
    {
      if (*m1 > *m2)
        return -1;
      else if (*m1 < *m2)
        return 1;
    }
    return 0;
  }
  /*4 handle module case:*/
  else if (*m1 > *m2)
    return -pComponentOrder;
  return pComponentOrder;
}

static int comp1cds(poly p1, poly p2)
{
  int i;
  short *m1=p1->exp;
  short *m2=p2->exp;

  /*4 compare monomials by order then revlex*/
  if (*m1 == *m2)
  {
    int d= p1->Order - p2->Order;
    if ( d == 0 /*p1->Order == p2->Order*/)
    {
      i = pVariables;
      m1=p1->exp+pVariables;
      m2=p2->exp+pVariables;
      if ((*m1 == *m2))
      {
        do
        {
          i--;
          if (i <= 1)
            return 0;
          m1--;
          m2--;
        } while ((*m1 == *m2));
      }
      if (*m1 < *m2)
        return 1;
      return -1;
    }
    else if ( d > 0 /*p1->Order > p2->Order*/)
      return -1;
    return 1;
  }
  /*4 handle module case:*/
  else if (*m1 > *m2)
    return -pComponentOrder;
  return pComponentOrder;
}
//static int comp1cds(poly p1, poly p2)
//{
//  int i;
//
//  /*4 compare monomials by order then revlex*/
//  if (p1->exp[0] == p2->exp[0])
//  {
//    if (p1->Order == p2->Order)
//    {
//      i = pVariables;
//      if ((p1->exp[i] == p2->exp[i]))
//      {
//        do
//        {
//          i--;
//          if (i <= 1) return 0;
//        } while ((p1->exp[i] == p2->exp[i]));
//      }
//      if (p1->exp[i] < p2->exp[i]) return 1;
//      return -1;
//    }
//    else if (p1->Order > p2->Order) return -1;
//    return 1;
//  }
//  /*4 handle module case:*/
//  else if (p1->exp[0] > p2->exp[0]) return -pComponentOrder;
//  return pComponentOrder;
//}

static int comp1cDs(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  if (p1->exp[0] == p2->exp[0])
  {
    int d= p1->Order - p2->Order;
    if (d > 0 /*p1->Order > p2->Order*/)
      return -1;
    else if (d < 0 /*p1->Order < p2->Order*/)
      return 1;
    for (i = 1; i<=pVariables; i++)
    {
      if (p1->exp[i] > p2->exp[i])
        return 1;
      else if (p1->exp[i] < p2->exp[i])
        return -1;
    }
    return 0;
  }
  /*4 handle module case:*/
  else if (p1->exp[0] > p2->exp[0])
    return -pComponentOrder;
  return pComponentOrder;
}

/*2
* compare the head monomial of p1 and p2 with weight vector
*/
static int comp1a  ( poly p1, poly p2, int f, int l, short * w )
{
  int d= p1->Order - p2->Order;
  if ( d > 0 /*p1->Order > p2->Order*/ )
    return 1;
  else if ( d < 0 /*p1->Order < p2->Order*/ )
    return -1;
  return 0;
}


/*---------------------------------------------------*/

/* These functions could be made faster if you use pointers to the
* exponent vectors and pointer arithmetic instead of using the
* macro pGetExp !!!
*/

/*2
* compare the head monomial of p1 and p2 with lexicographic ordering
*/
//static int comp_lp ( poly p1, poly p2, int f, int l, short * w )
//{
//  int i = f;
//  short *m1=p1->exp+f;
//  short *m2=p2->exp+f;
//  while ( ( i <= l ) && ( *m1 == *m2 ) ) { i++;m1++;m2++; }
//  if ( i > l ) return 0;
//  if ( *m1 > *m2 ) return 1;
//  return -1;
//}
static int comp_lp ( poly p1, poly p2, int f, int l, short * w )
{
  int i = f;
  while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
    i++;
  if ( i > l )
    return 0;
  if ( pGetExp(p1,i) > pGetExp(p2,i) )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with degree reverse lexicographic
* ordering
*/
//static int comp_dp ( poly p1, poly p2, int f, int l, short * w )
//{
//  int i, s1 = 0, s2 = 0;
//  short *m1=p1->exp+f;
//  short *m2=p2->exp+f;
//
//  for ( i = f; i <= l; i++,m1++,m2++ )
//  {
//    s1 += (*m1);
//    s2 += (*m2);
//  }
//  if ( s1 == s2 )
//  {
//    i = l;
//    while ( (i >= f ) && ( *m1 == *m2 ) ) { i--;m1--;m2--; }
//    if ( i < f ) return 0;
//    if ( *m1 > *m2 ) return -1;
//    return 1;
//  }
//  if ( s1 > s2 ) return 1;
//  return -1;
//}
static int comp_dp ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++ )
  {
    s1 += pGetExp(p1,i);
    s2 += pGetExp(p2,i);
  }
  if ( s1 == s2 )
  {
    i = l;
    while ( (i >= f ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i--;
    if ( i < f )
      return 0;
    if ( pGetExp(p1,i) > pGetExp(p2,i) )
      return -1;
    return 1;
  }
  if ( s1 > s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with degree lexicographic ordering
*/
//static int comp_Dp ( poly p1, poly p2, int f, int l, short * w )
//{
//  int i, s1 = 0, s2 = 0;
//  short *m1=p1->exp+f;
//  short *m2=p2->exp+f;
//
//  for ( i = f; i <= l; i++,m1++,m2++ )
//  {
//    s1 += (*m1);
//    s2 += (*m2);
//  }
//  if ( s1 == s2 )
//  {
//    i = f;
//    while ( (i <= l ) && ( *m1 == *m2 ) ) { i++;m1++;m2++; }
//    if ( i > l ) return 0;
//    if ( *m1 > *m2 ) return 1;
//    return -1;
//  }
//  if ( s1 > s2 ) return 1;
//  return -1;
//}
static int comp_Dp ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++ )
  {
    s1 += pGetExp(p1,i);
    s2 += pGetExp(p2,i);
  }
  if ( s1 == s2 )
  {
    i = f;
    while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i++;
    if ( i > l )
      return 0;
    if ( pGetExp(p1,i) > pGetExp(p2,i) )
      return 1;
    return -1;
  }
  if ( s1 > s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with weighted degree reverse
* lexicographic ordering
*/
static int comp_wp ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++, w++ )
  {
    s1 += (int)pGetExp(p1,i)*(*w);
    s2 += (int)pGetExp(p2,i)*(*w);
  }
  if ( s1 == s2 )
  {
    i = l;
    while ( ( i >= f ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i--;
    if ( i < f )
      return 0;
    if ( pGetExp(p1,i) > pGetExp(p2,i) )
      return -1;
    return 1;
  }
  if ( s1 > s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with weighted degree lexicographic
* ordering
*/
static int comp_Wp ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++, w++ )
  {
    s1 += (int)pGetExp(p1,i)*(*w);
    s2 += (int)pGetExp(p2,i)*(*w);
  }
  if ( s1 == s2 )
  {
    i = f;
    while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i++;
    if ( i > l )
      return 0;
    if ( pGetExp(p1,i) > pGetExp(p2,i) )
      return 1;
    return -1;
  }
  if ( s1 > s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with lexicographic ordering
* (power series case)
*/
static int comp_ls ( poly p1, poly p2, int f, int l, short * w )
{
  int i;

  i = f;
  while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
    i++;
  if ( i > l )
    return 0;
  if ( pGetExp(p1,i) < pGetExp(p2,i) )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with degree reverse lexicographic
* ordering (power series case)
*/
static int comp_ds ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++ )
  {
    s1 += pGetExp(p1,i);
    s2 += pGetExp(p2,i);
  }
  if ( s1 == s2 )
  {
    i = l;
    while ( ( i >= f ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i--;
    if ( i < f )
      return 0;
    if ( pGetExp(p1,i) < pGetExp(p2,i) )
      return 1;
    return -1;
  }
  if ( s1 < s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with degree lexicographic ordering
* (power series case)
*/
static int comp_Ds ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++ )
  {
    s1 += pGetExp(p1,i);
    s2 += pGetExp(p2,i);
  }
  if ( s1 == s2 )
  {
    i = f;
    while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i++;
    if ( i > l )
      return 0;
    if ( pGetExp(p1,i) < pGetExp(p2,i) )
      return -1;
    return 1;
  }
  if ( s1 < s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with weighted degree reverse
* lexicographic ordering (power series case)
*/
static int comp_ws ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++, w++ )
  {
    s1 += (int)pGetExp(p1,i)*(*w);
    s2 += (int)pGetExp(p2,i)*(*w);
  }
  if ( s1 == s2 )
  {
    i = l;
    while ( ( i >= f ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i--;
    if ( i < f )
      return 0;
    if ( pGetExp(p1,i) < pGetExp(p2,i) )
      return 1;
    return -1;
  }
  if ( s1 < s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with weighted degree lexicographic
* ordering (power series case)
*/
static int comp_Ws ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++, w++ )
  {
    s1 += (int)pGetExp(p1,i)*(*w);
    s2 += (int)pGetExp(p2,i)*(*w);
  }
  if ( s1 == s2 )
  {
    i = f;
    while ( ( i <= l ) && ( pGetExp(p1,i) == pGetExp(p2,i) ) )
      i++;
    if ( i > l )
      return 0;
    if ( pGetExp(p1,i) < pGetExp(p2,i) )
      return -1;
    return 1;
  }
  if ( s1 < s2 )
    return 1;
  return -1;
}

/*2
* compare the head monomial of p1 and p2 with matrix order
* w contains a series of l-f+1 lines
*/
static int comp_M ( poly p1, poly p2, int f, int l, short * w )
{
  int i, j, s1, s2;

  for ( i = f; i <= l; i++ )
  {
    s1 = s2 = 0;
    for ( j = f; j <= l; j++, w++ )
    {
      s1 += (int)pGetExp(p1,j)*(int)(*w);
      s2 += (int)pGetExp(p2,j)*(int)(*w);
    }
    if ( s1 < s2 )
      return -1;
    if ( s1 > s2 )
      return 1;
    /* now w points to the last element of the current row, the next w++ */
    /* moves on to the first element of the next row ! */
  }
  return 0;
}

/*2
* compare the head monomial of p1 and p2 with weight vector
*/
static int comp_a  ( poly p1, poly p2, int f, int l, short * w )
{
  int i, s1 = 0, s2 = 0;

  for ( i = f; i <= l; i++, w++ )
  {
    s1 += (int)pGetExp(p1,i)*(*w);
    s2 += (int)pGetExp(p2,i)*(*w);
  }
  if ( s1 > s2 )
    return 1;
  if ( s1 < s2 )
    return -1;
  return 0;
}

/*2
* compare the head monomial of p1 and p2 with module component
*/
static int comp_c  ( poly p1, poly p2, int f, int l, short * w )
{
  if ( pGetComp(p1) > pGetComp(p2) )
    return -pComponentOrder;
  if ( pGetComp(p1) < pGetComp(p2) )
    return pComponentOrder;
  return 0;
}

/*---------------------------------------------------------------*/

/*2
* compare p1 and p2 by a block ordering
* uses (*bcomph[])() to do the real work
*/
static int BlockComp(poly p1, poly p2)
{
  int res, i, e, a;

  /*4 compare in all blocks,*
  * each block has var numbers a(=block0[i]) to e (=block1[i])*
  * the block number starts with 0*/
  e = 0;
  i = 0;
  loop
  {
    a = block0[i];
    e = block1[i];
    res = (*bcomph[i])(p1, p2, a, e , polys_wv[i]);
    if (res)
      return res;
    i++;
    if (order[i]==0)
      break;
  }
  return 0;
}

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
    if (actHeads[i])
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

  if (CompP1==CompP2) return pCompOld(p1,p2);
  pSetComp(p1,cP1);
  pSetComp(p2,cP2);
  result = pCompOld(p1,p2);
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

void pSetSchreyerOrdB(polyset nextOrder, int length)
{
  int i;

  if (length)
  {
    if (maxSchreyer)
      newHeadsB(nextOrder, length);
    else
    {
      SchreyerOrd = (int*)Alloc(length*sizeof(int));
      for (i=0;i<length;i++) SchreyerOrd[i] = pGetComp(nextOrder[i]);
      maxSchreyer = length;
      pCompOld = pComp0;
      pComp0 = mcompSchrB;
    }
  }
  else
  {
    if (maxSchreyer!=0)
    {
      Free((ADDRESS)SchreyerOrd,maxSchreyer*sizeof(int));
      maxSchreyer = 0;
      pComp0 = pCompOld;
    }
  }
}

static void newHeadsM(polyset actHeads,int length)
{
  int i;
  int* newOrder=
    (int*)Alloc((length+maxSchreyer-indexShift)*sizeof(int));

  for (i=0;i<length+maxSchreyer-indexShift;i++)
    newOrder[i]=0;
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
  while ((pNext(p)!=NULL) && (pNext(p)->exp[0]<=indexShift))
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
  if (p1->exp[0]<=indexShift)
  {
    if (p2->exp[0]>indexShift) return 1;
  }
  else if (p2->exp[0]<=indexShift)  return -1;
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
      if (!indexShift) indexShift = 1;
      SchreyerOrd = (int*)Alloc((indexShift+length)*sizeof(int));
      maxSchreyer = length+indexShift;
      for (i=0;i<indexShift;i++)
        SchreyerOrd[i] = i;
      for (i=indexShift;i<maxSchreyer;i++)
        SchreyerOrd[i] = pGetComp(nextOrder[i-indexShift]);
      pCompOld = pComp0;
      pComp0 = mcompSchrM;
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
      pComp0 = pCompOld;
      pLDeg = pLDegOld;
    }
  }
}

/*2
*the pComp0 for normal syzygies
*compares monomials in the usual ring order (pCompOld)
*but groups module indecees according indexBounds befor
*/
static int mcompSyz(poly p1,poly p2)
{
  if (pGetComp(p1)<=maxBound)
  {
    if (pGetComp(p2)>maxBound) return 1;
  }
  else if (pGetComp(p2)<=maxBound)
  {
    if (pGetComp(p1)>maxBound) return -1;
  }
  return pCompOld(p1,p2);
}

void pSetSyzComp(int k)
{
  if (k!=0)
  {
    if (maxBound==0)
    {
      pCompOld = pComp0;
      pComp0 = mcompSyz;
    }
    maxBound = k;
  }
  else
  {
    if (maxBound!=0)
    {
      pComp0 = pCompOld;
      maxBound = 0;
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
  return ((a!=NULL) ? (a->Order) : (-1));
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths 1
* (all are 1 so save multiplications or they are of different signs)
* the ordering is not compatible with degree so do not use p->Order
*/
int pTotaldegree(poly p)
{
  int i;
  int j =(int)p->exp[1];

  for (i=pVariables; i>1; i--)
    j += (int)p->exp[i];
  return  j;
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths from the ordering
* the ordering is not compatible with degree so do not use p->Order
*/
int pWTotaldegree(poly p)
{
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
          j+=p->exp[k]*polys_wv[i][k-block0[i]];
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
          j+=p->exp[k];
        }
        break;
      case ringorder_c:
      case ringorder_C:
      case ringorder_a:
        break;
    }
  }
  return  j;
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
  short k=p->exp[0];
  int ll=1;

  while ((pNext(p)!=NULL) && (pGetComp(pNext(p))==k))
  {
    pIter(p);
    ll++;
  }
  *l=ll;
  return (p->Order);
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

  if (maxBound/*syzComp*/==0)
  {
    while ((p=pNext(p))!=NULL)
    {
      o=pFDeg(p);
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if (pGetComp(p)<=maxBound/*syzComp*/)
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
  short k=p->exp[0];
  int o = p->Order;
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
  short k=p->exp[0];
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
    if ((maxBound/*syzComp*/==0) || (pGetComp(p)<=maxBound/*syzComp*/))
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
* sets the comparision routine for monomials: for the first block
* of variables (or is the number of the ordering)
*/
static void SimpleChoose(int or, pCompProc *p)
{
  switch(or)
  {
    case ringorder_lp:
      if (pVariables<3) *p= comp2lpc;
      else              *p= comp1lpc;
      pLexOrder=TRUE;
      pFDeg = pTotaldegree;
      pLDeg = ldeg1c;
      break;
    case ringorder_unspec:
    case ringorder_dp:
      *p= comp1dpc;
      break;
    case ringorder_Dp:
      *p= comp1Dpc;
      break;
    case ringorder_wp:
      *p= comp1dpc;
      break;
    case ringorder_Wp:
      *p= comp1Dpc;
      break;
    case ringorder_ls:
      *p= comp1lsc;
      pLexOrder=TRUE;
      pFDeg = pTotaldegree;
      pLDeg = ldeg1c;
      break;
    case ringorder_ds:
      *p= comp1dsc;
      break;
    case ringorder_Ds:
      *p= comp1Dsc;
      break;
    case ringorder_ws:
      *p= comp1dsc;
      break;
    case ringorder_Ws:
      *p= comp1Dsc;
      break;
#ifdef TEST
    default:
      Werror("wrong internal ordering:%d at %s, l:%d\n",or,__FILE__,__LINE__);
#endif
  }
}

/*2
* sets pSetm
* (according or = order of first block)
*/
static void SetpSetm(int or, int ip)
{
  switch(or)
  {
    case ringorder_lp:
    case ringorder_ls:
      if (pVariables>1)
        pSetm= setlex2;
      else
        pSetm= setlex1;
      break;
    case ringorder_dp:
    case ringorder_Dp:
    case ringorder_ds:
    case ringorder_Ds:
    case ringorder_unspec:
      pSetm= setdeg1;
      break;
    case ringorder_a:
    case ringorder_wp:
    case ringorder_Wp:
    case ringorder_ws:
    case ringorder_Ws:
    case ringorder_M:
      pSetm= setdeg1w;
      firstwv=polys_wv[ip];
      break;
    case ringorder_c:
    case ringorder_C:
      return;
      /*do not set firstBlockEnds for this orderings*/
#ifdef TEST
    default:
      Werror("wrong internal ordering:%d at %s, l:%d\n",or,__FILE__,__LINE__);
#endif
  }
  firstBlockEnds=block1[ip];
}

/*2
* sets the comparision routine for monomials: for the first block
* of variables (or is the number of the ordering)
*/
static void SimpleChooseC(int or, pCompProc *p)
{
  switch(or)
  {
    case ringorder_lp:
      if (pVariables <3) *p= comp2clp;
      else               *p= comp1clp;
      pLexOrder=TRUE;
      pFDeg = pTotaldegree;
      pLDeg = ldeg1;
      break;
    case ringorder_dp:
      *p= comp1cdp;
      break;
    case ringorder_Dp:
      *p= comp1cDp;
      break;
    case ringorder_wp:
      *p= comp1cdp;
      break;
    case ringorder_Wp:
      *p= comp1cDp;
      break;
    case ringorder_ls:
      *p= comp1cls;
      pLexOrder=TRUE;
      pFDeg = pTotaldegree;
      pLDeg = ldeg1;
      break;
    case ringorder_ds:
      *p= comp1cds;
      break;
    case ringorder_Ds:
      *p= comp1cDs;
      break;
    case ringorder_ws:
      *p= comp1cds;
      break;
    case ringorder_Ws:
      *p= comp1cDs;
      break;
#ifdef TEST
    default:
      Werror("wrong internal ordering:%d at %s, l:%d\n",or,__FILE__,__LINE__);
#endif
  }
}

/*2
* sets the comparision routine for monomials: for all but the first
* block of variables (ip is the block number, or the number of the ordering)
*/
static void HighSet(int ip, int or)
{
  switch(or)
  {
    case ringorder_lp:
      bcomph[ip]=comp_lp;
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_dp:
      bcomph[ip]=comp_dp;
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_Dp:
      bcomph[ip]=comp_Dp;
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_wp:
      bcomph[ip]=comp_wp;
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_Wp:
      bcomph[ip]=comp_Wp;
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      break;
    case ringorder_ls:
      bcomph[ip]=comp_ls;
      break;
    case ringorder_ds:
      bcomph[ip]=comp_ds;
      break;
    case ringorder_Ds:
      bcomph[ip]=comp_Ds;
      break;
    case ringorder_ws:
      bcomph[ip]=comp_ws;
      break;
    case ringorder_Ws:
      bcomph[ip]=comp_Ws;
      break;
    case ringorder_c:
      pComponentOrder=1;
      bcomph[ip]=comp_c;
      break;
    case ringorder_C:
      pComponentOrder=-1;
      bcomph[ip]=comp_c;
      break;
    case ringorder_M:
      bcomph[ip]=comp_M;
      pMixedOrder=TRUE;
      break;
    case ringorder_a:
      if (pOrdSgn==-1) pMixedOrder=TRUE;
      if (ip==0)
        bcomph[0]=comp1a;
      else
        bcomph[ip]=comp_a;
      break;
#ifdef TEST
    default:
      Werror("wrong internal ordering:%d at %s, l:%d\n",or,__FILE__,__LINE__);
#endif
  }
}

/* -------------------------------------------------------- */
/*2
* change all variables to fit the description of the new ring
*/
void pChangeRing(int n, int Sgn, int * orders, int * b0, int * b1,
         short ** wv)
{

  int i;
  pComponentOrder=1;
  if (ppNoether!=NULL) pDelete(&ppNoether);
#ifdef SRING
  pSRING=FALSE;
  pAltVars=n+1;
#endif

  pVariables = n;
  pOrdSgn = Sgn;
  pVectorOut=(orders[0]==ringorder_c);
  order=orders;
  block0=b0;
  block1=b1;
  firstwv=NULL;
  polys_wv=wv;
  /*------- only one real block ----------------------*/
  pLexOrder=FALSE;
  pMixedOrder=FALSE;
  pFDeg=pDeg;
  if (pOrdSgn == 1) pLDeg = ldegb;
  else              pLDeg = ldeg0;
  /*======== ordering type is (_,c) =========================*/
  if ((orders[0]==ringorder_unspec)
  ||(
    ((orders[1]==ringorder_c)||(orders[1]==ringorder_C))
    && (orders[0]!=ringorder_M)
    && (orders[2]==0))
  )
  {
    if (pOrdSgn == -1) pLDeg = ldeg0c;
    SimpleChoose(orders[0],&pComp0);
    SetpSetm(orders[0],0);
    if ((orders[0]!=ringorder_unspec)
    && (orders[1]==ringorder_C))
      pComponentOrder=-1;
#ifdef TEST_MAC_ORDER
    if (orders[0]==ringorder_dp)
       bBinomSet();
#endif
  }
  /*======== ordering type is (c,_) =========================*/
  else if (((orders[0]==ringorder_c)||(orders[0]==ringorder_C))
  && (orders[1]!=ringorder_M)
  &&  (orders[2]==0))
  {
    /* pLDeg = ldeg0; is standard*/
    SimpleChooseC(orders[1],&pComp0);
    SetpSetm(orders[1],1);
    if (orders[0]==ringorder_C)
      pComponentOrder=-1;
  }
  /*------- more than one block ----------------------*/
  else
  {
    pLexOrder=TRUE;
    pVectorOut=orders[0]==ringorder_c;
    /*the number of orderings:*/
    i = 0;
    while (orders[++i] != 0);
    do
    {
      i--;
      HighSet(i, orders[i]);/*sets also pMixedOrder to TRUE, if...*/
      SetpSetm(orders[i],i);
    }
    while (i != 0);

    pComp0 = BlockComp;
    if ((orders[0]!=ringorder_c)&&(orders[0]!=ringorder_C))
    {
      pLDeg = ldeg1c;
    }
    else
    {
      pLDeg = ldeg1;
    }
    pFDeg = pWTotaldegree; // may be improved: pTotaldegree for lp/dp/ls/.. blocks
  }
  pMonomSize = POLYSIZE + (pVariables + 1) * sizeof(short);
  mmSpecializeBlock(pMonomSize);
  if ((pLexOrder) || (pOrdSgn==-1))
  {
    test &= ~Sy_bit(OPT_REDTAIL); /* noredTail */
  }
}

/* -------------------------------------------------------- */

static BOOLEAN pMultT_nok;
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
#ifdef SDRING
  poly pDRINGres=NULL;
#endif

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
#ifdef DRING
      if (pDRING)
      {
         if (pdDFlag(a)==1)
         {
           if (pdDFlag(exp)==1)
           {
             pDRINGres=pAdd(pDRINGres,pMultDD(a,exp));
           }
           else
           {
             pDRINGres=pAdd(pDRINGres,pMultDT(a,exp));
           }
         }
         else
         {
           if (pdDFlag(exp)==1)
           {
             pDRINGres=pAdd(pDRINGres,pMultDD(a,exp));
           }
           else
           {
             pDRINGres=pAdd(pDRINGres,pMultTT(a,exp));
           }
         }
      }
      else
#endif
#ifdef SRING
      if (pSRING)
      {
        pDRINGres=pAdd(pDRINGres,psMultM(a,exp));
      }
      else
#endif
      {
        for (i=pVariables; i != 0; i--)
        {
          a->exp[i] += exp->exp[i];
        }
        #ifndef TEST_MAC_ORDER
        a->Order += exp->Order;
        #else
        pSetm(a);
        #endif
        if (pMultT_nok)
        {
          if (pGetComp(a) == 0)
          {
            a->exp[0] = pGetComp(exp);
          }
          else
          {
            return NULL /*FALSE*/;
          }
        }
      }
      prev=a;
      pIter(a);
    }
  }
  pMultT_nok=0;
#ifdef DRING
   if (pDRING)
   {
     pDelete(&aa);
     return pDRINGres;
   }
#endif
#ifdef SRING
   if (pSRING)
   {
     pDelete(&aa);
     return pDRINGres;
   }
#endif
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
    memcpy4(result,a,pMonomSize);
    pNext(result)=NULL;
    pGetCoeff(result)=nMult(pGetCoeff(a),c);
    pIter(a);
    hp=result;
    while (a)
    {
      hp=pNext(hp)=pNew();
      memcpy4(hp,a,pMonomSize);
      pSetCoeff0(hp,nMult(pGetCoeff(a), c));
      pIter(a);
    }
    pNext(hp)=NULL;
  }
  return result;
}

/* -------------------------------------------------------- */
/*2
* are the head monomials (without coeff) equal ?
*/
BOOLEAN pEqual(poly a, poly b)
{
  int i;
  short *e1=a->exp;
  short *e2=b->exp;

  if (a->Order != b->Order) return FALSE;
  for (i=pVariables; i>=0; i--,e1++,e2++)
    if (*e1 != *e2) return FALSE;
  return TRUE;
}

/*2
* returns TRUE if the head term of b is a multiple of the head term of a
*/
#ifdef macintosh
BOOLEAN pDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&((a->exp[0]==0) || (a->exp[0] == b->exp[0])))
  {
    int i=pVariables;
    short *e1=&(a->exp[1]);
    short *e2=&(b->exp[1]);
    if ((*e1) > (*e2)) return FALSE;
    do
    {
      i--;
      if (i == 0) return TRUE;
      e1++;
      e2++;
     } while ((*e1) <= (*e2));
   }
   return FALSE;
}
#endif

/*2
* assumes that the head term of b is a multiple of the head term of a
* and return the multiplicant *m
*/
poly pDivide(poly a, poly b)
{
  int i;
  poly result=pInit();

  memset(result,0, pMonomSize);
  for(i=(int)pVariables; i>=0; i--)
    result->exp[i] = a->exp[i]-b->exp[i];
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
      for(i=(int)pVariables; i>=0; i--)
        a->exp[i]-=b->exp[i];
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
  for (i=pVariables; i>=0; i--)
  {
     m->exp[i] = max(a->exp[i],b->exp[i]);
  }
}

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
poly pmInit(char *st, BOOLEAN &ok)
{
  int i,j;
  ok=FALSE;
  BOOLEAN b=FALSE;
  poly rc = pNew();
  memset(rc,0,pMonomSize);
  char *s = nRead(st,&(rc->coef));
  if (s==st)
  /* i.e. it does not start with a coeff: test if it is a ringvar*/
  {
    j = rIsRingVar(s);
    if (j >= 0)
    {
      rc->exp[1+j] += (short)1;
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
      rc->exp[1+j] += (short)i;
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
#ifdef DRING
    if (pDRING)
    {
      for(i=1;i<=pdN;i++)
      {
        if(rc->exp[pdDX(i)]>0)
        {
          pdDFlag(rc)=1;
          break;
        }
      }
    }
#endif
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
        q->exp[varnum] += (short)ii;
        pSetm(q);
      }
      pIter(q);
    }
    q = pOrdPoly(res);
  }
  return q;
}

/*2
*re-orders a polynomial
*/
poly pOrdPolySchreyer(poly p)
{
  poly qq,result=p;

  if (p == NULL) return NULL;
  while ((pNext(p)) && (pComp(p,pNext(p))==1)) pIter(p);
  if (pNext(p)==NULL) return result;
  qq = pNext(p);
  pNext(p) = NULL;
  qq = pOrdPolySchreyer(qq);
  return pAdd(result,qq);
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

  P = (polyset)Alloc(5*sizeof(poly));
  for (i=0; i<5; i++)
  {
    P[i] = NULL;
  }
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
    if (p2->exp[i] != 0)
    {
      k = p1->exp[i] / p2->exp[i];
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
    poly p = pNew();

    p->next = NULL;
    for (i=1; i<=pVariables; i++)
    {
      p->exp[i] = p1->exp[i]-k*p2->exp[i];
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
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (pGetComp(q) > k) pGetComp(q)--;
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
    }
    else
    {
      /*pIter(q);*/ q=pNext_q;
      if (pGetComp(q) > k) pGetComp(q)--;
    }
  }
  return result;
}

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
    }
    else
    {
      pIter(q);
//      if (pGetComp(q) > k) pGetComp(q)--;
    }
  }
  return result;
}

void pDeleteComp(poly * p,int k)
{
  poly q;

  while ((*p!=NULL) && (pGetComp(*p)==k)) pDelete1(p);
  if (*p==NULL) return;
  q = *p;
  if (pGetComp(q)>k) pGetComp(q)--;
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
      pDelete1(&(pNext(q)));
    else
    {
      pIter(q);
      if (pGetComp(q)>k) pGetComp(q)--;
    }
  }
}
/*----------end of utilities for syzygies--------------*/

/*2
* pair has no common factor ? or is no polynomial
*/
BOOLEAN pHasNotCF(poly p1, poly p2)
{
#ifdef SRING
  if (pSRING)
    return FALSE;
#endif
  short * m1 = p1->exp;
  short * m2 = p2->exp;

  if (((*m1) > 0)||((*m2) > 0))
    return FALSE;
  int i = 1;
  loop
  {
    m1++;m2++;
    if (((*m1) > 0) && ((*m2) > 0))
      return FALSE;
    if (i == pVariables)
      return TRUE;
    i++;
  }
}

/*
*void    pSFactors(poly f, poly g, poly a, poly b)
*{
*  int i,d;
*
*  for (i=pVariables;i>0;i--)
*  {
*    d = f->exp[i]-g->exp[i];
*    if (d >= 0)
*    {
*      a->exp[i] = 0;
*      b->exp[i] = d;
*    }
*    else
*    {
*      a->exp[i] = -d;
*      b->exp[i] = 0;
*    }
*  }
*  a->exp[0] = 0;
*  b->exp[0] = 0;
*  pSetm(a);
*  pSetm(b);
*}
*/

/*
*void    pSDiv(poly f, poly g, poly b)
*{
*  int i,d;
*
*  for (i=pVariables;i>0;i--)
*  {
*    d = f->exp[i]-g->exp[i];
*    b->exp[i] = d;
*  }
*  b->exp[0] = 0;
*  pSetm(b);
*}
*/

/*2
* update the initial term of a polynomial a by multipying it by
* the (number) coefficient
* and the exponent vector (of) exp (a well initialized polynomial)
*/
/*
*void    pSMultBy(poly f, poly m)
*{
*  number t;
*  int i;
* //  short notok;
*
*  t=nMult(f->coef, m->coef);
*  nDelete(&(f->coef));
*  f->coef = t;
*  f->Order  += m->Order;
*  for (i=pVariables; i; i--)
*    f->exp[i] += m->exp[i];
* //  if (notok)
* //  {
*    if (!(f->exp[0]))
*    {
*      f->exp[0] = m->exp[0];
*    }
* //    else
* //    {
* //      HALT;
* //    }
* //  }
*}
*/

/*2
* creates a copy of the initial monomial of p
* sets the coeff of the copy to a defined value
*/
poly pCopy1(poly p)
{
  poly w;
  w = pNew();
  memcpy4(w,p,pMonomSize);
  nNew(&(w->coef));
  pNext(w) = NULL;
  return w;
}

/*2
*should return 1 if p divides q and p<q,
*             -1 if q divides p and q<p
*              0 otherwise
*/
int     pDivComp(poly p, poly q)
{
  short * mp = p->exp;
  short * mq = q->exp;

  if (*mp==*mq)
  {
    int i=pVariables;
    BOOLEAN a=FALSE, b=FALSE;
    for (; i>0; i--)
    {
      mp++;mq++;
      if (*mp<*mq)
      {
        if (b) return 0;
        a =TRUE;
      }
      else if (*mp>*mq)
      {
        if (a) return 0;
        b = TRUE;
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
  while (p!=NULL)
  {
    nNormalize(pGetCoeff(p));
    pIter(p);
  }
}

/*3
* substitute the n-th variable by 1 in p
* destroy p
*/
static poly pSubst1 (poly p,int n)
{
  poly qq,result = NULL;

  while (p != NULL)
  {
    qq = p;
    pIter(p);
    qq->next = NULL;
    pSetExp(qq,n,0);
    pSetm(qq);
    result = pAdd(result,qq);
  }
  return result;
}

/*2
* substitute the n-th variable by e in p
* destroy p
*/
poly pSubst(poly p, int n, poly e)
{
  if ((e!=NULL)&&(pIsConstant(e))&&(nIsOne(pGetCoeff(e))))
    return pSubst1(p,n);

  int exponent,i;
  poly h, res, m;
  short *me,*ee;
  number nu,nu1;

  me=(short *)Alloc((pVariables+1)*sizeof(short));
  ee=(short *)Alloc((pVariables+1)*sizeof(short));
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
      for(i=1;i<=pVariables;i++)
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
  Free((ADDRESS)me,(pVariables+1)*sizeof(short));
  Free((ADDRESS)ee,(pVariables+1)*sizeof(short));
  return res;
}

BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=pVariables; j; j--)
    if (p->exp[j] > lcm->exp[j]) return FALSE;
  if (p->exp[0] != lcm->exp[0]) return FALSE;
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
        for (k=j-1; k; k--)
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

#ifdef PDEBUG
BOOLEAN pDBTest(poly p, char *f, int l)
{
  poly old=NULL;
  BOOLEAN ismod=FALSE;
  while (p!=NULL)
  {
#ifdef MDEBUG
    if (!mmDBTestBlock(p,pMonomSize,f,l))
      return FALSE;
#endif
#ifdef LDEBUG
    if (!nDBTest(p->coef,f,l))
       return FALSE;
#endif
    if ((p->coef!=NULL) && nIsZero(p->coef))
    {
      Print("zero coef in poly in %s:%d\n",f,l);
      return FALSE;
    }
    int i=pVariables;
#ifndef DRING
    for(;i>=0;i--)
    {
      if (pGetExp(p,i)<0)
      {
        Print("neg. Exponent in %s:%l\n",f,l);
        return FALSE;
      }
    }
#endif
    if (ismod==0)
    {
      if (pGetComp(p)>0) ismod=1;
      else               ismod=2;
    }
    else if (ismod==1)
    {
      if (pGetComp(p)==0)
      {
        Print("mix vec./poly in %s:%d\n",f,l);
        return FALSE;
      }
    }
    else if (ismod==2)
    {
      if (pGetComp(p)!=0)
      {
        Print("mix poly/vec. in %s:%d\n",f,l);
        return FALSE;
      }
    }
    i=p->Order;
    pSetm(p);
    if(i!=p->Order)
    {
      Print("wrong ord-field in %s:%d\n",f,l);
      return FALSE;
    }
    old=p;
    pIter(p);
    if (pComp(old,p)!=1)
    {
      Print("wrong order in %s:%d\n",f,l);
      return FALSE;
    }
  }
  return TRUE;
}
#endif

