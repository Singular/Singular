/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.cc,v 1.17 1998-03-18 14:28:50 obachman Exp $ */

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
#include "ipid.h"

#ifdef COMP_FAST
#include "polys-comp.h"
#endif

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
/* 1 for polynomial ring, -1 otherwise */
int     pOrdSgn;
/* TRUE for momomial output as x2y, FALSE for x^2*y */
int pShortOut = (int)TRUE;
// it is of type int, not BOOLEAN because it is also in ip
/* TRUE if the monomial ordering is not compatible with pFDeg */
BOOLEAN pLexOrder;
/* TRUE if the monomial ordering has polynomial and power series blocks */
BOOLEAN pMixedOrder;

#if defined(TEST_MAC_ORDER) || defined(COMP_FAST)
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
#ifndef COMP_FAST
static int maxBound = 0;
#else
int maxBound = 0;
#endif

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

/* Remark: These could be made more efficient by avoiding using pGetExp */

/*2
* define the order of p with respect to lex. ordering, N=1
*/
static void setlex1(poly p)
{
  p->Order = (Order_t)pGetExp(p,1);
}

/*2
* define the order of p with respect to lex. ordering, N>1
*/
static void setlex2(poly p)
{
  p->Order = (((Order_t)pGetExp(p,1))<<(sizeof(Exponent_t)*8))
    + (Order_t)pGetExp(p,2); 
}

/*2
* define the order of p with respect to a degree ordering
*/
static void setdeg1(poly p)
{
  p->Order = pExpQuerSum1(p, firstBlockEnds);
#ifdef COMP_DEBUG  
    int i, j = pGetExp(p,1);

  for (i = firstBlockEnds; i > 1; i--) j += pGetExp(p,i);
  if (j != p->Order)
  {
    Print("Error in setdeg1");
    pExpQuerSum1(p, firstBlockEnds);
  }
#endif  
}

/*2
* define the order of p with respect to a degree ordering
* with weigthts
*/
static void setdeg1w(poly p)
{
  Order_t i, j = 0;

  for (i = firstBlockEnds; i>0; i--)
  {
     j += ((Order_t) pGetExp(p,i))*firstwv[i-1];
  }
  p->Order = j;
}


/*-------- IMPLEMENTATION OF MONOMIAL COMPARISONS ---------------------*/


/***************************************************************
 ***************************************************************  
 *
 * MONOMIAL COMPARIONS:
 * They are influenced by the following macros:
 * COMP_TRADITIONAL -- (un)set in polys-impl.h
     Keeps the traditional comparison routines
     defined -- needed as long as their might be comparisons with
     negativ components.
     All the traditional routines are prefixed by t_
     
 * COMP_FAST -- (un)set in polys-impl.h
     Implements monomial comparisons using the fast
     techniques. Undefine in case there are problems. All the fast
     routines are prefixed by f_
     
 * COMP_STATISTIC -- (un)set in polys-impl.h
     Provides several routines for accumulating statistics on monomial
     comparisons
     
 * COMP_DEBUG -- (un)set in polys-impl.h
     Turns on debugging of COMP_FAST by comparing the resutsl of fast
     comparison with traditional comparison

 *    
 *
 ***************************************************************  
 ***************************************************************/

#ifdef COMP_DEBUG
static int debug_comp(poly p1, poly p2);
#endif // COMP_DEBUG

#ifdef COMP_STATISTICS
// Initializes, resets and outputs Comp statistics
void InitCompStatistics(int nvars);
void ResetCompStatistics();
void OutputCompStatistics();
unsigned long MonomCountTotal = 0;
unsigned long MonomCountOrderS = 0;
unsigned long MonomCountOrderG = 0;
unsigned long* MonomCountExp = NULL;
unsigned long MonomCountExpS = 0;
unsigned long MonomCountExpG = 0;
unsigned long MonomCountCompS = 0;
unsigned long MonomCountCompG = 0;
unsigned long MonomCountEqual = 0;
#endif // COMP_STATISTICS


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
  
/***************************************************************
 *
 * FAST COMPARISONS
 *
 ***************************************************************/
#ifdef COMP_FAST
static pCompProc f_pComp0 = NULL;
static int f_comp_otEXP_nwONE(poly p1, poly p2);
static int f_comp_otCOMPEXP_nwONE(poly p1, poly p2);
static int f_comp_otEXPCOMP_nwONE(poly p1, poly p2);
static int f_comp_otEXP_nwTWO(poly p1, poly p2);
static int f_comp_otCOMPEXP_nwTWO(poly p1, poly p2);
static int f_comp_otEXPCOMP_nwTWO(poly p1, poly p2);
static int f_comp_otEXP_nwEVEN(poly p1, poly p2);
static int f_comp_otCOMPEXP_nwEVEN(poly p1, poly p2);
static int f_comp_otEXPCOMP_nwEVEN(poly p1, poly p2);
static int f_comp_otEXP_nwODD(poly p1, poly p2);
static int f_comp_otCOMPEXP_nwODD(poly p1, poly p2);
static int f_comp_otEXPCOMP_nwODD(poly p1, poly p2);

#define Mreturn(d, multiplier)                      \
{                                                   \
  if (d > 0) return multiplier;                     \
  return -multiplier;                               \
}                                               
  
// comp_nwONE is used if pVariables1W == 1 and component is compatible with ordering
static int f_comp_otEXP_nwONE(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwONE(p1, p2, d, goto NotEqual, return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwONE is used if pVariables1W == 1, priority is given to exponents,
// component is incompatible with ordering
static int f_comp_otEXPCOMP_nwONE(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXPCOMP_nwONE(p1, p2, d, goto NotEqual , return 0);

  NotEqual:
  Mreturn(d, pLexSgn)
}

// comp_otEXPCOMP_nwONE is used if pVariables1W == 1, priority is given to component,
// component is incompatible with ordering
static int f_comp_otCOMPEXP_nwONE(poly p1, poly p2)  
{
  register long d = pGetComp(p2) - pGetComp(p1);
  if (d) Mreturn(d, pComponentOrder);
  d = pGetOrder(p1) - pGetOrder(p2);
  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwONE(p1, p2, d, goto NotEqual , return 0);
  
  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_nwTWO is used if pVariables1W == 2 and component is compatible with ordering
static int f_comp_otEXP_nwTWO(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwTWO(p1, p2, d, goto NotEqual , return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwTWO is used if pVariables1W == 2, priority is given to exponents,
// component is incompatible with ordering
static int f_comp_otEXPCOMP_nwTWO(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXPCOMP_nwTWO(p1, p2, d, goto NotEqual, return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwTWO is used if pVariables1W == 2, priority is given to component,
// component is incompatible with ordering
static int f_comp_otCOMPEXP_nwTWO(poly p1, poly p2)  
{
  register long d = pGetComp(p2) - pGetComp(p1);
  if (d) Mreturn(d, pComponentOrder);
  d = pGetOrder(p1) - pGetOrder(p2);
  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwTWO(p1, p2, d, goto NotEqual , return 0);
  
  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_nwEVEN is used if pVariables1W == 2*i and component is compatible
// with ordering
static int f_comp_otEXP_nwEVEN(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwEVEN(p1, p2, pVariables1W, d, goto NotEqual , return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwEVEN is used if pVariables1W == 2*i, priority is given to exponents,
// component is incompatible with ordering
static int f_comp_otEXPCOMP_nwEVEN(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXPCOMP_nwEVEN(p1, p2, pVariables1W, d, goto NotEqual , return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwEVEN is used if pVariables1W == 2*i, priority is given to component,
// component is incompatible with ordering
static int f_comp_otCOMPEXP_nwEVEN(poly p1, poly p2)  
{
  register long d = pGetComp(p2) - pGetComp(p1);
  if (d) Mreturn(d, pComponentOrder);
  d = pGetOrder(p1) - pGetOrder(p2);
  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwEVEN(p1, p2, pVariablesW, d, goto NotEqual, return 0);
  
  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_nwODD is used if pVariables1W == 2*i and component is compatible
// with ordering
static int f_comp_otEXP_nwODD(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);

  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwODD(p1, p2, pVariables1W, d, goto NotEqual, return 0);

  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otEXPCOMP_nwODD is used if pVariables1W == 2*i, priority is given to exponents,
// component is incompatible with ordering
static int f_comp_otEXPCOMP_nwODD(poly p1, poly p2)  
{
  register long d = pGetOrder(p1) - pGetOrder(p2);
  if (d) 
  {
    if (d > 0) return pOrdSgn;
    return -pOrdSgn;
  }
  
  _pMonComp_otEXPCOMP_nwODD(p1, p2, pVariables1W, d, goto NotEqual , return 0);
  
  NotEqual:
  Mreturn(d, pLexSgn);
}

// comp_otCOMPEXP_nwODD is used if pVariables1W == 2*i, priority is given to component,
// component is incompatible with ordering
static int f_comp_otCOMPEXP_nwODD(poly p1, poly p2)  
{
  register long d = pGetComp(p2) - pGetComp(p1);
  if (d) Mreturn(d, pComponentOrder);
  d = pGetOrder(p1) - pGetOrder(p2);
  if (d) Mreturn(d, pOrdSgn);
  _pMonComp_otEXP_nwODD(p1, p2, pVariablesW, d, goto NotEqual, return 0);
  
  NotEqual:
  Mreturn(d, pLexSgn);
}
#endif // COMP_FAST

/***************************************************************
 *
 * FAST COMPARISONS
 *
 ***************************************************************/
#ifdef COMP_TRADITIONAL

static int t_comp_otCOMPEXP_nwONE(poly p1, poly p2);
static int t_comp_otEXPCOMP_nwONE(poly p1, poly p2);
static int t_comp_lex_otCOMPEXP_i(poly p1, poly p2);
static int t_comp_otCOMPEXP_lex_i(poly p1, poly p2);
static int t_comp_revlex_otCOMPEXP_i(poly p1, poly p2);
static int t_comp_otCOMPEXP_revlex_i(poly p1, poly p2);

inline long LexComp(poly p1, poly p2)
{
  long d, i;
  for (i=1; i<pVariables; i++)
  {
    d = pGetExpDiff(p1, p2, i);
    if (d) return d;
  }
  return pGetExpDiff(p1, p2, pVariables);
}

inline long RevLexComp(poly p1, poly p2)
{
  long d, i;
  for (i=pVariables; i>1; i--)
  {
    d = pGetExpDiff(p1, p2, i);
    if (d) return d;
  }
  return pGetExpDiff(p1, p2, 1);
}

static int t_comp_otCOMPEXP_nwONE(poly p1, poly p2)
{
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  return 0;
}

static int t_comp_otEXPCOMP_nwONE(poly p1, poly p2)
{
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  return 0;
}

static int t_comp_lex_otCOMPEXP_i(poly p1, poly p2)
{
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  NonZeroR(LexComp(p1, p2), return pLexSgn, return -pLexSgn);
  return 0;
}

static int t_comp_otEXPCOMP_lex_i(poly p1, poly p2)
{
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  NonZeroR(LexComp(p1, p2), return pLexSgn, return -pLexSgn);
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  return 0;
}

static int t_comp_revlex_otCOMPEXP_i(poly p1, poly p2)
{
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  NonZeroR(RevLexComp(p1, p2), return pLexSgn, return -pLexSgn);
  return 0;
}

static int t_comp_otEXPCOMP_revlex_i(poly p1, poly p2)
{
  NonZeroR(pGetOrder(p1) - pGetOrder(p2), return pOrdSgn, return -pOrdSgn);
  NonZeroR(RevLexComp(p1, p2), return pLexSgn, return -pLexSgn);
  NonZeroR(pGetComp(p1) - pGetComp(p2),
           return -pComponentOrder, return pComponentOrder);
  return 0;
}

#endif // COMP_TRADITIONAL

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
    return -1;
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
  return pExpQuerSum(p);  
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
  Exponent_t k= pGetComp(p);
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
* of variables (o_r is the number of the ordering)
*/
#ifdef COMP_FAST
static void SimpleChoose(int o_r, int comp_order, pCompProc *p)
#else  
static void SimpleChoose(int o_r, pCompProc *p)
#endif  
{
#ifdef COMP_FAST
  switch(o_r)
  {
      case ringorder_dp:
      case ringorder_wp:
      case ringorder_ds:
      case ringorder_ws:
      case ringorder_ls:
      case ringorder_unspec:
        pSetVarIndicies_RevLex(pVariables);
        pLexSgn = -1;
        if (comp_order == ringorder_C || o_r == ringorder_unspec)
        {
          if (pVariables1W == 1)
            f_pComp0 = f_comp_otEXPCOMP_nwONE;
          else if (pVariables1W == 2)
            f_pComp0 = f_comp_otEXPCOMP_nwTWO;
          else if (pVariables1W & 1)
            f_pComp0 = f_comp_otEXPCOMP_nwODD;
          else
            f_pComp0 = f_comp_otEXPCOMP_nwEVEN;
        }
        else
        {
          if (pVariables1W == 1)
            f_pComp0 = f_comp_otEXP_nwONE;
          else if (pVariables1W == 2)
            f_pComp0 = f_comp_otEXP_nwTWO;
          else if (pVariables1W & 1)
            f_pComp0 = f_comp_otEXP_nwODD;
          else
            f_pComp0 = f_comp_otEXP_nwEVEN;
        }
        break;
        
#ifdef PDEBUG
      case ringorder_lp:
      case ringorder_Dp:
      case ringorder_Wp:
      case ringorder_Ds:
      case ringorder_Ws:
#else
      default:
#endif
        pSetVarIndicies_Lex(pVariables);
        pLexSgn = 1;
        if (comp_order == ringorder_c)
        {
          if (pVariables1W == 1)
            f_pComp0 = f_comp_otEXPCOMP_nwONE;
          else if (pVariables1W == 2)
            f_pComp0 = f_comp_otEXPCOMP_nwTWO;
          else if (pVariables1W & 1)
            f_pComp0 = f_comp_otEXPCOMP_nwODD;
          else
            f_pComp0 = f_comp_otEXPCOMP_nwEVEN;
        }
        else
        {
          if (pVariables1W == 1)
            f_pComp0 = f_comp_otEXP_nwONE;
          else if (pVariables1W == 2)
            f_pComp0 = f_comp_otEXP_nwTWO;
          else if (pVariables1W & 1)
            f_pComp0 = f_comp_otEXP_nwODD;
          else
            f_pComp0 = f_comp_otEXP_nwEVEN;
        }
#ifdef PDEBUG
        break;
      default:
        Werror("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
#endif
  }
  
  if (o_r == ringorder_lp || o_r == ringorder_ls)
  {
    pLexOrder=TRUE;
    pFDeg = pTotaldegree;
    pLDeg = ldeg1c;
    if (o_r == ringorder_ls)
      pSetVarIndicies_Lex(pVariables);
  }
  *p = f_pComp0;
#endif // COMP_FAST  

}

/*2
* sets pSetm
* (according o_r = order of first block)
*/
static void SetpSetm(int o_r, int ip)
{
  switch(o_r)
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
      Werror("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
#endif
  }
  firstBlockEnds=block1[ip];
}

/*2
* sets the comparision routine for monomials: for the first block
* of variables (o_r is the number of the ordering)
*/
#ifdef COMP_FAST
static void SimpleChooseC(int o_r, int comp_order, pCompProc *p)
#else  
static void SimpleChooseC(int o_r, pCompProc *p)
#endif  
{
#ifdef COMP_FAST
  switch(o_r)
  {
      case ringorder_dp:
      case ringorder_wp:
      case ringorder_ds:
      case ringorder_ls:
      case ringorder_ws:
        pSetVarIndicies_RevLex(pVariables);
        pLexSgn = -1;
        if (pVariablesW == 1)
          f_pComp0 = f_comp_otCOMPEXP_nwONE;
        else if (pVariablesW == 2)
          f_pComp0 = f_comp_otCOMPEXP_nwTWO;
        else if (pVariablesW & 1)
          f_pComp0 = f_comp_otCOMPEXP_nwODD;
        else
          f_pComp0 = f_comp_otCOMPEXP_nwEVEN;
        break;
        
#ifdef PDEBUG
      case ringorder_lp:
      case ringorder_Dp:
      case ringorder_Wp:
      case ringorder_Ds:
      case ringorder_Ws:
#else
      default:
#endif
        pSetVarIndicies_Lex(pVariables);
        pLexSgn = 1;
        if (pVariablesW == 1)
          f_pComp0 = f_comp_otCOMPEXP_nwONE;
        else if (pVariablesW == 2)
          f_pComp0 = f_comp_otCOMPEXP_nwTWO;
        else if (pVariablesW & 1)
          f_pComp0 = f_comp_otCOMPEXP_nwODD;
        else
          f_pComp0 = f_comp_otCOMPEXP_nwEVEN;
#ifdef PDEBUG
        break;
      default:
        Werror("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
#endif
  }
  if (o_r == ringorder_lp || o_r == ringorder_ls)
  {
    pLexOrder=TRUE;
    pFDeg = pTotaldegree;
    pLDeg = ldeg1c;
    if (o_r == ringorder_ls)
     pSetVarIndicies_Lex(pVariables);
  }
  *p = f_pComp0;
#endif // COMP_FAST  

#ifdef COMP_DEBUG
    *p = debug_comp;
#endif  
}

/*2
* sets the comparision routine for monomials: for all but the first
* block of variables (ip is the block number, o_r the number of the ordering)
*/
static void HighSet(int ip, int o_r)
{
  switch(o_r)
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
      Werror("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
#endif
  }
}

#ifdef DIV_COUNT
struct div_triple
{
  unsigned long greater;
  unsigned long equal;
  unsigned long smaller;
};

static div_triple* DivTriples = NULL;
static int NumberOfDivTriples = 0;
static unsigned long DivCountTotal = 0;
static unsigned long DivCountDiv = 0;
static unsigned long LexDivCount = 0;
static unsigned long RevLexDivCount = 0;
static unsigned long LexDivCount2 = 0;
static unsigned long RevLexDivCount2 = 0;
static int pVariables2;
static int pVariablesEven;

void InitDivCount(int nvars)
{
  if (nvars & 1) pVariablesEven = 0;
  else pVariablesEven = 1;

  pVariables2 = (pVariables -1) / 2;

  if (DivTriples == NULL)
    {
      DivTriples = (div_triple*) Alloc0(nvars*sizeof(div_triple));
      NumberOfDivTriples = nvars;
    }
  else if (nvars > NumberOfDivTriples)
    {
      DivTriples = (div_triple*) ReAlloc(DivTriples, 
					 NumberOfDivTriples*sizeof(div_triple),
					 nvars*sizeof(div_triple));
      for (int i=NumberOfDivTriples; i<nvars; i++)
	{
	  DivTriples[i].greater = 0;
	  DivTriples[i].smaller = 0;
	  DivTriples[i].equal = 0;
	}

      NumberOfDivTriples = nvars;
    }
}

void ResetDivCount()
{
  int i;
  for (i=0; i<NumberOfDivTriples;i++)
    {
      DivTriples[i].greater = 0;
      DivTriples[i].equal = 0;
      DivTriples[i].smaller = 0;
    }

  DivCountTotal = 0;
  DivCountDiv = 0;
  LexDivCount = 0;
  RevLexDivCount = 0;
  LexDivCount2 = 0;
  RevLexDivCount2 = 0;
}

void OutputDivCount()
{
  printf("Total      : %10u\n", DivCountTotal);
  if (DivCountTotal == 0) DivCountTotal++;
  printf("Div        : %10u \t %.4f\n", DivCountDiv,
	 (float) DivCountDiv / (float) DivCountTotal);
  printf("LexCount   :  %10u \t %.4f\n", LexDivCount,
	 (float) LexDivCount / (float) DivCountTotal);
  printf("RevLexCount:  %10u \t %.4f\n", RevLexDivCount,
	 (float) RevLexDivCount / (float) DivCountTotal);
  printf("LexCount2   :  %10u \t %.4f\n", LexDivCount2,
	 (float) LexDivCount2 / (float) DivCountTotal);
  printf("RevLexCount2:  %10u \t %.4f\n", RevLexDivCount2,
	 (float) RevLexDivCount2 / (float) DivCountTotal);

  for (int i=0; i<NumberOfDivTriples; i++)
    {
      unsigned long total = DivTriples[i].greater + 
	DivTriples[i].equal + DivTriples[i].smaller;
      // avoid total==0
      if (total == 0) total=1;
      printf("  [%d]: %10u (%.4f)  %10u (%.4f)  %10u (%.4f)\n", i,
	     DivTriples[i].greater, 
	     (float) DivTriples[i].greater / (float) total ,
	     DivTriples[i].equal, 
	     (float) DivTriples[i].equal / (float) total  ,
	     DivTriples[i].smaller, 
	     (float) DivTriples[i].smaller / (float) total  );
    }
}

BOOLEAN pDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&(( pGetComp(a)==0) || ( pGetComp(a) ==  pGetComp(b))))
  {
    int i;
    Exponent_t *e1=&( pGetExp(a,1));
    Exponent_t *e2=&( pGetExp(b,1));
    BOOLEAN res = TRUE, res2 = TRUE;
    DivCountTotal++;

    for (i=0; i<pVariables; i++)
    {
      if (res == TRUE) LexDivCount++;
      if (*e1 > *e2) 
      {
        DivTriples[i].greater++;
        res = FALSE;
      }
      else if (*e1 == *e2)  DivTriples[i].equal++;
      else DivTriples[i].smaller++;
      e1++;
      e2++;
    } 
    if (res == TRUE) DivCountDiv++;
    e1 = &(pGetExp(a,pVariables));
    e2 = &(pGetExp(b,pVariables));

    for (i=0; i<pVariables; i++)
    {
      RevLexDivCount++;
      if (*e1 > *e2)
      {
        res2 = FALSE;
        break;
      }
      e1--;
      e2--;
    }
    if (res != res2)
      fprintf(stderr, "Error in RevLexCount\n");
    
    if (pVariables < 4) return res;
    LexTop:
    res2 = TRUE;
    int* s1 = (int*) &( pGetExp(a,2));
    int* s2 = (int*) &( pGetExp(b,2));
    for (i=0; i<pVariables2; i++, s1++, s2++)
    {
      LexDivCount2++;
      if (*s1 > *s2)
	  {
	    res2 = FALSE;
	    break;
	  }
    }
    if (res2 == TRUE)
    {
      LexDivCount2++;
      if ( pGetExp(a,1) >  pGetExp(b,1)) res2 = FALSE;
      if (res2 == TRUE && pVariablesEven)
	  {
	    LexDivCount2++;
	    if (pGetExp(a,pVariables) > pGetExp(b,pVariables))
	      res2 = FALSE;
	  }
    }
    if (res2 == TRUE)
    {
      e1 = &( pGetExp(a,2));
      e2 = &( pGetExp(b,2));
      for (i=0; i<pVariables2; i++, e1 += 2, e2 += 2)
      {
        LexDivCount++;
#ifdef WORDS_BIG_ENDIAN
        if (e1[1] > e2[1])
#else
        if (*e1 > *e2)
#endif
        {
          res2 = FALSE;
          break;
        }
      }
    }
    if (res != res2)
    {
      fprintf(stderr, "Error in LexDivCount2\n");
      // goto LexTop;
    }
    
    RevLexTop:
    res2 = TRUE;
    if (pVariablesEven)
    {
      s1 = (int*) &(pGetExp(a,pVariables-2));
      s2 = (int*) &(pGetExp(b,pVariables-2));
    }
    else
    {
      s1 = (int*) &(pGetExp(a,pVariables-1));
      s2 = (int*) &(pGetExp(b,pVariables-1));
    }
    for (i=0; i< pVariables2; i++, s1--, s2--)
    {
      RevLexDivCount2++;
      if (*s1 > *s2)
	  {
	    res2 = FALSE;
	    break;
	  }
    }
    if (res2 == TRUE)
    {
      if (pVariablesEven)
	  {
	    RevLexDivCount2++;
	    if (pGetExp(a,pVariables) > pGetExp(b,pVariables))
	      res2 = FALSE;
	  }
      if (res2 == TRUE)
	  {
	    RevLexDivCount2++;
	    if ( pGetExp(a,1) >  pGetExp(b,1)) res2 = FALSE;
	  }
    }
    if (res2 == TRUE)
    {
      if (pVariablesEven)
      {
        e1 = &(pGetExp(a,pVariables-2));
        e2 = &(pGetExp(b,pVariables-2));
      }
      else
      {
        e1 = &(pGetExp(a,pVariables-1));
        e2 = &(pGetExp(b,pVariables-1));
      }
      for (i=0; i<pVariables2; i++, e1 -= 2, e2 -= 2)
      {
#ifdef WORDS_BIG_ENDIAN      
      if (e1[1] > e2[1])
#else
      if (*e1 > *e2)  
#endif        
        {
          res2 = FALSE;
          break;
        }
      }
    }
    if (res != res2)
    {
      fprintf(stderr, "Error in RevLexDivCount2\n");
      // goto RevLexTop;
    }
    return res;
  }
  return FALSE;
}
#endif

/* -------------------------------------------------------- */
/*2
* change all variables to fit the description of the new ring
*/

void pChangeRing(int n, int Sgn, int * orders, int * b0, int * b1,
         short ** wv)
{
  sip_sring tmpR;
  memset(&tmpR, 0, sizeof(sip_sring));
  tmpR.N = n;
  tmpR.OrdSgn = Sgn;
  tmpR.order = orders;
  tmpR.block0 = b0;
  tmpR.block1 = b1;
  tmpR.wvhdl = wv;
  pSetGlobals(&tmpR);
}

void pSetGlobals(ring r, BOOLEAN complete)
{
#ifdef TEST_MAC_ORDER
  bNoAdd=FALSE;
#endif
  int i;
  pComponentOrder=1;
  if (ppNoether!=NULL) pDelete(&ppNoether);
#ifdef SRING
  pSRING=FALSE;
  pAltVars=r->N+1;
#endif
#ifdef COMP_FAST
  f_pComp0 = NULL;
#endif  
  pVariables = r->N;

  // set the various size parameters and initialize memory
  pMonomSize = POLYSIZE + (pVariables + 1) * sizeof(Exponent_t);
  if ((pMonomSize % sizeof(void*)) == 0)
  {
    pMonomSizeW = pMonomSize/sizeof(void*);
  }
  else
  {
    pMonomSizeW = pMonomSize/sizeof(void*) + 1;
    pMonomSize = pMonomSizeW*sizeof(void*);
  }

#ifdef COMP_FAST  
  if ((((pVariables+1)*sizeof(Exponent_t)) % sizeof(void*)) == 0)
    pVariables1W = (pVariables+1)*sizeof(Exponent_t) / sizeof(void*);
  else
    pVariables1W = ((pVariables+1)*sizeof(Exponent_t) / sizeof(void*)) + 1;
  if ((((pVariables)*sizeof(Exponent_t)) % sizeof(void*)) == 0)
    pVariablesW = (pVariables)*sizeof(Exponent_t) / sizeof(void*);
  else
    pVariablesW = ((pVariables)*sizeof(Exponent_t) / sizeof(void*)) + 1;
  
  // Set default Var Indicies
  pSetVarIndicies(pVariables);
#endif
  mmSpecializeBlock(pMonomSize);
  
  pOrdSgn = r->OrdSgn;
  pVectorOut=(r->order[0]==ringorder_c);
  order=r->order;
  block0=r->block0;
  block1=r->block1;
  firstwv=NULL;
  polys_wv=r->wvhdl;
  /*------- only one real block ----------------------*/
  pLexOrder=FALSE;
  pMixedOrder=FALSE;
  pFDeg=pDeg;
  if (pOrdSgn == 1) pLDeg = ldegb;
  else              pLDeg = ldeg0;
  /*======== ordering type is (_,c) =========================*/
  if ((order[0]==ringorder_unspec)
  ||(
    ((order[1]==ringorder_c)||(order[1]==ringorder_C))
    && (order[0]!=ringorder_M)
    && (order[2]==0))
  )
  {
    if ((order[0]!=ringorder_unspec)
    && (order[1]==ringorder_C))
      pComponentOrder=-1;
    if (pOrdSgn == -1) pLDeg = ldeg0c;
#ifdef COMP_FAST    
    SimpleChoose(order[0],order[1], &pComp0);
#else    
    SimpleChoose(order[0],&pComp0);
#endif    
    SetpSetm(order[0],0);
#ifdef TEST_MAC_ORDER
    if (order[0]==ringorder_dp)
       bBinomSet(order);
#endif
  }
  /*======== ordering type is (c,_) =========================*/
  else if (((order[0]==ringorder_c)||(order[0]==ringorder_C))
  && (order[1]!=ringorder_M)
  &&  (order[2]==0))
  {
    /* pLDeg = ldeg0; is standard*/
    if (order[0]==ringorder_C)
      pComponentOrder=-1;
#ifdef COMP_FAST
    SimpleChooseC(order[1],pComponentOrder, &pComp0);
#else    
    SimpleChooseC(order[1],&pComp0);
#endif    
    SetpSetm(order[1],1);
#ifdef TEST_MAC_ORDER
    if (order[1]==ringorder_dp)
       bBinomSet(order);
#endif
  }
  /*------- more than one block ----------------------*/
  else
  {
    //pLexOrder=TRUE;
    pVectorOut=order[0]==ringorder_c;
    if ((pVectorOut)||(order[0]==ringorder_C))
    {
      if(block1[1]!=pVariables) pLexOrder=TRUE;
    }  
    else
    {
      if(block1[0]!=pVariables) pLexOrder=TRUE;
    }
    /*the number of orderings:*/
    i = 0;
    while (order[++i] != 0);
    do
    {
      i--;
      HighSet(i, order[i]);/*sets also pMixedOrder to TRUE, if...*/
      SetpSetm(order[i],i);
    }
    while (i != 0);

    pComp0 = BlockComp;
    if ((order[0]!=ringorder_c)&&(order[0]!=ringorder_C))
    {
      pLDeg = ldeg1c;
    }
    else
    {
      pLDeg = ldeg1; 
    }
    pFDeg = pWTotaldegree; // may be improved: pTotaldegree for lp/dp/ls/.. blocks
  }
  if ((pLexOrder) || (pOrdSgn==-1))
  {
    test &= ~Sy_bit(OPT_REDTAIL); /* noredTail */
  }

#ifdef COMP_FAST
  if (f_pComp0 == NULL)
    f_pComp0 = pComp0;
#endif
  
#ifdef DIV_COUNT
  InitDivCount(pVariables);
#endif
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
           pAddExp(a,i, pGetExp(exp,i));
        }
        #ifdef TEST_MAC_ORDER
        if (bNoAdd)
          pSetm(a);
        else
        #endif
          a->Order += exp->Order;
        if (pMultT_nok)
        {
          if (pGetComp(a) == 0)
          {
             pSetComp(a, pGetComp(exp));
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
    pCopy2(result,a);
    pNext(result)=NULL;
    pGetCoeff(result)=nMult(pGetCoeff(a),c);
    pIter(a);
    hp=result;
    while (a)
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
* returns TRUE if the head term of b is a multiple of the head term of a
*/
#if defined(macintosh)
BOOLEAN pDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&(( pGetComp(a)==0) || ( pGetComp(a) ==  pGetComp(b))))
  {
    int i=pVariables;
    Exponent_t *e1=&( pGetExp(a,1));
    Exponent_t *e2=&( pGetExp(b,1));
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
#ifdef DRING
    if (pDRING)
    {
      for(i=1;i<=pdN;i++)
      {
        if(pGetExp(rc,pdDX(i))>0)
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
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (pGetComp(q) > k) pDecrComp(q);
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
      if (pGetComp(q) > k) pDecrComp(q);
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
  if (pGetComp(q)>k) pDecrComp(q);
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
      pDelete1(&(pNext(q)));
    else
    {
      pIter(q);
      if (pGetComp(q)>k) pDecrComp(q);
    }
  }
}
/*----------end of utilities for syzygies--------------*/

/*2
* pair has no common factor ? or is no polynomial
*/
#ifdef COMP_FAST
BOOLEAN pHasNotCF(poly p1, poly p2)
{
#ifdef SRING
  if (pSRING)
    return FALSE;
#endif

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  Exponent_t * m1 = &(p1->exp[pVarLowIndex]);
  Exponent_t * m2 = &(p2->exp[pVarLowIndex]);
  int i = 1;
  loop
  {
    if (((*m1) > 0) && ((*m2) > 0))
      return FALSE;
    if (i == pVariables)
      return TRUE;
    m1++;m2++;
    i++;
  }
}
#else
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
#endif
/*
*void    pSFactors(poly f, poly g, poly a, poly b)
*{
*  int i,d;
*
*  for (i=pVariables;i>0;i--)
*  {
*    d =  pGetExp(f,i)- pGetExp(g,i);
*    if (d >= 0)
*    {
*       pGetExp(a,i) = 0;
*       pGetExp(b,i) = d;
*    }
*    else
*    {
*       pGetExp(a,i) = -d;
*       pGetExp(b,i) = 0;
*    }
*  }
*   pGetComp(a) = 0;
*   pGetComp(b) = 0;
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
*    d =  pGetExp(f,i)- pGetExp(g,i);
*     pGetExp(b,i) = d;
*  }
*   pGetComp(b) = 0;
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
*     pGetExp(f,i) +=  pGetExp(m,i);
* //  if (notok)
* //  {
*    if (!( pGetComp(f)))
*    {
*       pGetComp(f) =  pGetComp(m);
*    }
* //    else
* //    {
* //      HALT;
* //    }
* //  }
*}
*/


/*2
*should return 1 if p divides q and p<q,
*             -1 if q divides p and q<p
*              0 otherwise
*/
#ifdef COMP_FAST
int     pDivComp(poly p, poly q)
{
  if (pGetComp(p) == pGetComp(q))
  {
    Exponent_t * mp = &(p->exp[pVarLowIndex]);
    Exponent_t * mq = &(q->exp[pVarLowIndex]);
    int i=pVariables;
    BOOLEAN a=FALSE, b=FALSE;
    for (; i>0; i--)
    {
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
      mp++;mq++;
    }
    if (a) return 1;
    else if (b)  return -1;
  }
  return 0;
}
#else
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
#endif
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
    nTest(pGetCoeff(p));
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


#ifdef COMP_STATISTICS
static int s_comp_lp_otCOMPEXP_1(poly p1, poly p2)  
{
  MonomCountTotal++;

  OrderCmp(p2, p1,
           {MonomCountOrderS++; return -1;},
           {MonomCountOrderG++; return 1;});
  
  CompCmp(p2, p1,
          {MonomCountCompS++; return -pComponentOrder},
          {MonomCountCompG++; return pComponentOrder});

  MonomCountEqual++;
  return 0;
}
#endif

#ifdef COMP_STATISTICS
static int s_comp_lp_otCOMPEXP_i(poly p1, poly p2)  
{
  MonomCountTotal++;

  OrderCmp(p2, p1,
           {MonomCountOrderS++; return -1;},
           {MonomCountOrderG++; return 1;});

  int i = SIZEOF_ORDER / SIZEOF_EXPONENT + 1;
  Exponent_t d;
  

  for (;;)
  {
    d = pGetExp(p1, i)  - pGetExp(p2, i);
    if (d)
    {
      MonomCountExp[i]++;
      if (d < 0)
      {
        return -1;
        MonomCountExpS++;
      }
      MonomCountExpG++;
      return 1;
    }
    i++;
    if (i == pVariables) break;
  }
  
  CompCmp(p2, p1,
          {MonomCountCompS++; return -pComponentOrder},
          {MonomCountCompG++; return pComponentOrder});

  MonomCountEqual++;
  return 0;
}
#endif

#ifdef MONOM_COUNT
static void InitMonomCount(unsigned int nvars)
{
  if (gMonomCount == NULL)
  {
    gMonomCount = (unsigned long *) Alloc0(nvars*sizeof(int));
    gMonomCountLength = nvars;
  }
  else if (gMonomCountLength < nvars)
  {
    int i;
    gMonomCount = (unsigned long *) ReAlloc(gMonomCount,
                                          gMonomCountLength*sizeof(int),
                                          nvars*sizeof(int));
    for (i = gMonomCountLength; i< nvars; i++)
      gMonomCount[i] = 0; 
    gMonomCountLength = nvars;
  }
}

void OutputMonomCount()
{
  unsigned int i;
  unsigned long max = gMonomCount0;
  float fmax;
  unsigned long sum = 0;
  

  // check that everything went ok
  if (gMonomCountS !=
      gMonomCount0 + gMonomCountOrder + gMonomCountL + gMonomCountG)
  {
    printf("MonomCountTotalError: %10lu %10lu\n", gMonomCountS,
           gMonomCount0 + gMonomCountOrder + gMonomCountL + gMonomCountG);
  }
  for (i=0; i<gMonomCountLength; i++)
    sum += gMonomCount[i];
  if (sum != gMonomCountL + gMonomCountG)
  {
    printf("MonomCountNotEqualError: %10lu %10lu\n",
           gMonomCountL + gMonomCountG, sum);
  }

  printf("Total   : %10lu\n", gMonomCountS);
  if (gMonomCountS == 0) gMonomCountS = 1;
  
  printf("Order   : %10lu \t %.4f\n",gMonomCountOrder,
         (float) gMonomCountOrder / (float) gMonomCountS);
  printf("Equal   : %10lu \t %.4f\n",gMonomCount0,
         (float) gMonomCount0 / (float) gMonomCountS);
  printf("NotEqual: %10lu \t %.4f\n",gMonomCountL + gMonomCountG,
         ((float) gMonomCountL + gMonomCountG) / (float) gMonomCountS);
  printf("\tGreater: %10lu \t %.4f\n",gMonomCountG,
         (float) gMonomCountG / (float) gMonomCountS);
  printf("\tLess   : %10lu \t %.4f\n",gMonomCountL,
         (float) gMonomCountL / (float) gMonomCountS);

  for (i=0; i<gMonomCountLength; i++)
  {
    printf("\t\t[%d]    : %10lu \t %.4f\n", i, gMonomCount[i],
           (float) gMonomCount[i] / (float) gMonomCountS);
  }
  printf("E1Neg   : %10lu \t %.4f\n",gMonomCountN1,
         (float) gMonomCountN1 / (float) gMonomCountS);
  printf("E2Neg   : %10lu \t %.4f\n",gMonomCountN2,
         (float) gMonomCountN2 / (float) gMonomCountS);
  printf("BothNeg : %10lu \t %.4f\n",gMonomCountNN,
         (float) gMonomCountNN / (float) gMonomCountS);
  printf("Mcount2 : %u\n", gMcount2);
}

void ResetMonomCount()
{
  int i;

  for (i=0; i<gMonomCountLength; i++)
    gMonomCount[i] = 0;
  gMonomCountOrder = 0;
  gMonomCount0 = 0;
  gMonomCountS = 0;
  gMonomCountG = 0;
  gMonomCountL = 0;
  gMonomCountN1 = 0;
  gMonomCountN2 = 0;
  gMonomCountNN = 0;
  gMcount2 = 0;
}

#endif
#ifdef MONOM_COUNT
unsigned long  gMonomCount0 = 0;
unsigned long  gMonomCountLength = 0;
unsigned long  gMonomCountOrder = 0;
unsigned long  gMonomCountG = 0;
unsigned long  gMonomCountL = 0;
unsigned long  gMonomCountS = 0;
unsigned long  gMonomCountN1 = 0;
unsigned long  gMonomCountN2 = 0;
unsigned long  gMonomCountNN = 0;
unsigned long  gMcount2 = 0;
unsigned long* gMonomCount = NULL;
#endif
