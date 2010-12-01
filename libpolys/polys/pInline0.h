/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline.cc
 *  Purpose: implementation of poly Level 0 functions
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE0_H
#define PINLINE0_H

#if defined(DO_PINLINE0)
#define PINLINE0 static inline
#else
#define PINLINE0
#endif

#include <kernel/p_polys.h>
#include <kernel/ring.h>

PINLINE0 void p_SetCompP(poly p, int i, ring r)
{
  if (p != NULL)
  {
#ifdef PDEBUG
    poly q = p;
    int l = 0;
#endif

    if (rOrd_SetCompRequiresSetm(r))
    {
      do
      {
        p_SetComp(p, i, r);
        p_SetmComp(p, r);
#ifdef PDEBUG
        l++;
#endif
        pIter(p);
      }
      while (p != NULL);
    }
    else
    {
      do
      {
        p_SetComp(p, i, r);
#ifdef PDEBUG
        l++;
#endif
        pIter(p);
      }
      while(p != NULL);
    }
#ifdef PDEBUG
    p_Test(q, r);
    assume(l == pLength(q));
#endif
  }
}

PINLINE0 void p_SetCompP(poly p, int i, ring lmRing, ring tailRing)
{
  if (p != NULL)
  {
    p_SetComp(p, i, lmRing);
    p_SetmComp(p, lmRing);
    p_SetCompP(pNext(p), i, tailRing);
  }
}

// returns minimal column number in the modul element a (or 0)
PINLINE0 long p_MinComp(poly p, ring lmRing, ring tailRing)
{
  long result,i;

  if(p==NULL) return 0;
  result = p_GetComp(p,lmRing);
  if (result != 0)
  {
    loop
    {
      pIter(p);
      if(p==NULL) break;
      i = p_GetComp(p,tailRing);
      if (i<result) result = i;
    }
  }
  return result;
}

// returns maximal column number in the modul element a (or 0)
PINLINE0 long p_MaxComp(poly p, ring lmRing, ring tailRing)
{
  long result,i;

  if(p==NULL) return 0;
  result = p_GetComp(p, lmRing);
  if (result != 0)
  {
    loop
    {
      pIter(p);
      if(p==NULL) break;
      i = p_GetComp(p, tailRing);
      if (i>result) result = i;
    }
  }
  return result;
}

BOOLEAN   p_IsConstantPoly(const poly p, const ring r)
{
  poly pp=p;
  while(pp!=NULL)
  {
    if (! p_LmIsConstantComp(pp, r))
      return FALSE;
    pIter(pp);
  }
  return TRUE;
}

/***************************************************************
 *
 * poly things which are independent of ring
 *
 ***************************************************************/
PINLINE0 poly pReverse(poly p)
{
  if (p == NULL || pNext(p) == NULL) return p;

  poly q = pNext(p), // == pNext(p)
    qn;
  pNext(p) = NULL;
  do
  {
    qn = pNext(q);
    pNext(q) = p;
    p = q;
    q = qn;
  }
  while (qn != NULL);
  return p;
}


/*2
* returns the length of a (numbers of monomials)
*/
PINLINE0 int pLength(poly a)
{
  int l = 0;

  while (a!=NULL)
  {
    pIter(a);
    l++;
  }
  return l;
}

/*2
* returns the length of a (numbers of monomials)
* respect syzComp
*/
poly pLast(poly a, int &l);

#endif // PINLINE_CC

