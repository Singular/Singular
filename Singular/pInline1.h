/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline1.h
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline1.h,v 1.6 2000-10-16 12:06:36 obachman Exp $
 *******************************************************************/
#ifndef PINLINE1_H
#define PINLINE1_H

#ifndef PDIV_DEBUG
// define to enable debugging/statistics of pLmShortDivisibleBy
#undef PDIV_DEBUG
#endif

#include "mod2.h"
#include "structs.h"
#include "p_MemCmp.h"
#include "polys-impl.h"

#if PDEBUG > 0 || defined(NO_PINLINE1)

#define _p_LmCmpAction(p, q, r, actionE, actionG, actionS)  \
do                                                          \
{                                                           \
  int _cmp = p_LmCmp(p,q,r);                                \
  if (_cmp == 0) actionE;                                   \
  if (_cmp == 1) actionG;                                   \
  actionS;                                                  \
}                                                           \
while(0)

#else

#define _p_LmCmpAction(p, q, r, actionE, actionG, actionS)                      \
 p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->pCompLSize, r->ordsgn,    \
                                   actionE, actionG, actionS)

#endif

#ifdef PDIV_DEBUG
BOOLEAN pDebugLmShortDivisibleBy(poly p1, unsigned long sev_1, ring r_1,
                                 poly p2, unsigned long not_sev_2, ring r_2);
BOOLEAN p_DebugLmDivisibleByNoComp(poly a, poly b, ring r);
#define pDivAssume  pAssume
#else
#define pDivAssume(x)   ((void)0)
#endif

#if !defined(NO_PINLINE1) || defined(PINLINE1_CC)

#include "omalloc.h"
#include "numbers.h"
#include "p_polys.h"
#include "p_MemAdd.h"
#include "p_MemCopy.h"

/***************************************************************
 *
 * Allocation/Initalization/Deletion 
 *
 ***************************************************************/
PINLINE1 poly p_Init(ring r, omBin bin)
{
  p_CheckRing1(r);
  pAssume1(bin != NULL && r->PolyBin->sizeW == bin->sizeW);
  poly p;
  omTypeAlloc0Bin(poly, p, bin);
  p_SetRingOfPoly(p, r);
  return p;
}
PINLINE1 poly p_Init(ring r)
{
  return p_Init(r, r->PolyBin);
}

PINLINE1 poly p_LmInit(poly p, ring r)
{
  p_CheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfPoly(np, r);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, NULL);
  return np;
}
PINLINE1 poly p_Head(poly p, ring r)
{
  if (p == NULL) return NULL;
  p_CheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfPoly(np, r);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, n_Copy(_pGetCoeff(p), r));
  return np;
}

PINLINE1 poly p_LmShallowCopyDelete(poly p, const ring r, omBin bin)
{
  p_CheckPolyRing1(p, r);
  pAssume1(bin->sizeW == r->PolyBin->sizeW);
  poly new_p = p_New(r);
  p_MemCopy_LengthGeneral(new_p->exp, p->exp, r->ExpLSize);
  pSetCoeff0(new_p, pGetCoeff(p));
  pNext(new_p) = pNext(p);
  omFreeBinAddr(p);
  return new_p;
}

/***************************************************************
 *
 * Operation on ExpVectors
 *
 ***************************************************************/
// ExpVextor(d_p) = ExpVector(s_p)
PINLINE1 void p_ExpVectorCopy(poly d_p, poly s_p, ring r)
{
  p_CheckPolyRing1(d_p, r);
  p_CheckPolyRing1(s_p, r);
  p_MemCopy_LengthGeneral(d_p->exp, s_p->exp, r->ExpLSize);
}
// ExpVector(p1) += ExpVector(p2)
PINLINE1 void p_ExpVectorAdd(poly p1, poly p2, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif 
 
  p_MemAdd_LengthGeneral(p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(p1) -= ExpVector(p2)
PINLINE1 void p_ExpVectorSub(poly p1, poly p2, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0 ||
          p_GetComp(p1, r) == p_GetComp(p2, r));
#endif 
 
  p_MemSub_LengthGeneral(p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(p1) += ExpVector(p2) - ExpVector(p3)
PINLINE1 void p_ExpVectorAddSub(poly p1, poly p2, poly p3, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
  p_CheckPolyRing1(p3, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) + p_GetExp(p2, i, r) >= p_GetExp(p3, i, r));
  pAssume1(p_GetComp(p1, r) == 0 || 
           (p_GetComp(p2, r) - p_GetComp(p3, r) == 0) ||
           (p_GetComp(p1, r) == p_GetComp(p2, r) - p_GetComp(p3, r)));
#endif 
 
  p_MemAddSub_LengthGeneral(p1->exp, p2->exp, p3->exp, r->ExpLSize);
}
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorSum(poly pr, poly p1, poly p2, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
  p_CheckPolyRing1(pr, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif  

  p_MemSum_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(pr) = ExpVector(p1) - ExpVector(p2)
PINLINE1 void p_ExpVectorDiff(poly pr, poly p1, poly p2, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
  p_CheckPolyRing1(pr, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(!rRing_has_Comp(r) || p_GetComp(p1, r) == p_GetComp(p2, r));
#endif 
 
  p_MemDiff_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpLSize);
}

PINLINE1 BOOLEAN p_ExpVectorEqual(poly p1, poly p2, ring r)
{
  p_CheckPolyRing1(p1, r);
  p_CheckPolyRing1(p2, r);
 
  int i = r->ExpLSize;
  unsigned long *ep = p1->exp;
  unsigned long *eq = p2->exp;

  do
  {
    i--;
    if (ep[i] != eq[i]) return FALSE;
  }
  while (i);
  return TRUE;
}

PINLINE1 unsigned long p_ExpVectorQuerSum(poly p, ring r)
{
  p_CheckPolyRing1(p, r);
  unsigned long s = 0;
  unsigned long i = r->N;
  
  do
  {
    s += p_GetExp(p, i, r);
    i--;
  }
  while (i);
  return s;
}

PINLINE1 void p_GetExpV(poly p, Exponent_t *ev, ring r)
{
  p_CheckPolyRing1(p, r);
  for (int j = r->N; j; j--)
      ev[j] = p_GetExp(p, j, r);

  ev[0] = _p_GetComp(p, r);
}
PINLINE1 void p_SetExpV(poly p, Exponent_t *ev, ring r)
{
  p_CheckPolyRing1(p, r);
  for (int j = r->N; j; j--)
      p_SetExp(p, j, ev[j], r);

  p_SetComp(p, ev[0],r);
  p_Setm(p, r);
}

/***************************************************************
 *
 * Comparison w.r.t. monomial ordering
 *
 ***************************************************************/
PINLINE1 int p_LmCmp(poly p, poly q, ring r)
{
  p_CheckPolyRing1(p, r);
  p_CheckPolyRing1(q, r);

  p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->pCompLSize, r->ordsgn, 
                                    return 0, return 1, return -1);
}


/***************************************************************
 *
 * divisibility
 *
 ***************************************************************/
// return: FALSE, if there exists i, such that a->exp[i] > b->exp[i]
//         TRUE, otherwise
// (1) Consider long vars, instead of single exponents
// (2) Clearly, if la > lb, then FALSE
// (3) Suppose la <= lb, and consider first bits of single exponents in l: 
//     if TRUE, then value of these bits is la ^ lb
//     if FALSE, then la-lb causes an "overflow" into one of those bits, i.e.,
//               la ^ lb != la - lb
static inline BOOLEAN _p_LmDivisibleByNoComp(poly a, poly b, ring r)
{
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;
  
  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i];
      if ((la > lb) || 
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]];
      if ((la > lb) || 
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == TRUE);
  return TRUE;
}

static inline BOOLEAN _p_LmDivisibleByNoComp(poly a, ring r_a, poly b, ring r_b)
{
  int i=r_a->N;
  pAssume1(r_a->N == r_b->N);

  do
  {
    if (p_GetExp(a,i,r_a) > p_GetExp(b,i,r_b))
      return FALSE;
    i--;
  }
  while (i);
  return TRUE;
}
static inline BOOLEAN _p_LmDivisibleBy(poly a, poly b, ring r)
{
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
}
static inline BOOLEAN _p_LmDivisibleBy(poly a, ring r_a, poly b, ring r_b)
{
  if (p_GetComp(a, r_a) == 0 || p_GetComp(a,r_a) == p_GetComp(b,r_b))
    return _p_LmDivisibleByNoComp(a, r_a, b, r_b);
  return FALSE;
}
PINLINE1 BOOLEAN p_LmDivisibleByNoComp(poly a, poly b, ring r)
{
  p_CheckPolyRing1(a, r);
  p_CheckPolyRing1(b, r);
  return _p_LmDivisibleByNoComp(a, b, r);
}
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, poly b, ring r)
{
  p_CheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_CheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
}
PINLINE1 BOOLEAN p_DivisibleBy(poly a, poly b, ring r)
{
  pIfThen1(b!=NULL, p_CheckPolyRing1(b, r));
  pIfThen1(a!=NULL, p_CheckPolyRing1(a, r));
  
  if (a != NULL && (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r)))
    return _p_LmDivisibleByNoComp(a,b,r);
  return FALSE;
}
PINLINE1 BOOLEAN p_DivisibleBy(poly a, ring r_a, poly b, ring r_b)
{
  pIfThen1(b!=NULL, p_CheckPolyRing1(b, r_b));
  pIfThen1(a!=NULL, p_CheckPolyRing1(a, r_a));
  if (a != NULL) return _p_LmDivisibleBy(a, r_a, b, r_b);
  return FALSE;
}
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, ring r_a, poly b, ring r_b)
{
  p_CheckPolyRing(a, r_a);
  p_CheckPolyRing(b, r_b);
  return _p_LmDivisibleBy(a, r_a, b, r_b);
}
PINLINE1 BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a, 
                                    poly b, unsigned long not_sev_b, ring r)
{
  p_CheckPolyRing1(a, r);
  p_CheckPolyRing1(b, r);
#ifndef PDIV_DEBUG
  _pPolyAssume2(p_GetShortExpVector(a, r) == sev_a, a, r);
  _pPolyAssume2(p_GetShortExpVector(b, r) == ~ not_sev_b, b, r);
  
  if (sev_a & not_sev_b) 
  {
    pAssume1(p_LmDivisibleByNoComp(a, b, r) == FALSE);
    return FALSE;
  }
  return p_LmDivisibleBy(a, b, r);
#else
  return pDebugLmShortDivisibleBy(a, sev_a, r, b, not_sev_b, r);
#endif
}

PINLINE1 BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a, ring r_a,
                                    poly b, unsigned long not_sev_b, ring r_b)
{
  p_CheckPolyRing1(a, r_a);
  p_CheckPolyRing1(b, r_b);
#ifndef PDIV_DEBUG
  _pPolyAssume2(p_GetShortExpVector(a, r_a) == sev_a, a, r_a);
  _pPolyAssume2(p_GetShortExpVector(b, r_b) == ~ not_sev_b, b, r_b);
  
  if (sev_a & not_sev_b) 
  {
    pAssume1(_p_LmDivisibleByNoComp(a, r_a, b, r_b) == FALSE);
    return FALSE;
  }
  return _p_LmDivisibleBy(a, r_a, b, r_b);
#else
  return pDebugLmShortDivisibleBy(a, sev_a, r_a, b, not_sev_b, r_b);
#endif
}

#endif // !defined(NO_PINLINE1) || defined(PINLINE1_CC)
#endif // PINLINE1_CC

#if 0
#include <stdio.h>

const unsigned long mask = 0x01010101;
int old_test(unsigned long l1,unsigned long l2)
{
  unsigned char* c1 = &l1;
  unsigned char* c2 = &l2;
  
  if (c1[0] < c2[0] || c1[1] < c2[1] || c1[2] < c2[2] || c1[3] < c2[3])
    return 0;
  return 1;
}

int new_test(unsigned long l1,unsigned long l2)
{
  
  if ( (l2 > l1) || ((l1 & mask) ^ (l2 & mask)) != ((l1 - l2) & mask))
    return 0;
  return 1;
}

print_it(unsigned long l1,unsigned long l2)
{
  printf("%d:%d\n", old_test(l1, l2), new_test(l1, l2));
  fflush(NULL);
}


void main()
{
  unsigned long mask = 0x01010101;
  unsigned long l1, l2;
  char *c1, *c2;
  unsigned long seed = time(NULL);
  
  for (;;)
  {
    l1 = random();
    l2 = random();
    
    if (old_test(l1,l2) != new_test(l1, l2))
    {
      print_it(l1, l2);
    }
  }
}

#endif
