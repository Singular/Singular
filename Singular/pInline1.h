/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline1.h
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline1.h,v 1.1 2000-09-12 16:01:04 obachman Exp $
 *******************************************************************/
#ifndef PINLINE1_H
#define PINLINE1_H

// define to enable debugging/statistics of pShortDivisibleBy
#undef PDIV_DEBUG

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
BOOLEAN pDebugShortDivisibleBy(poly p1, unsigned long sev_1, ring r_1,
                               poly p2, unsigned long not_sev_2, ring r_2);
#endif

#if !defined(NO_PINLINE1) || defined(PINLINE1_CC)

#include "omalloc.h"
#include "numbers.h"
#include "pInline2.h"
#include "p_MemAdd.h"
#include "p_MemCopy.h"
#include "polys.h"

/***************************************************************
 *
 * Allocation/Initalization/Deletion 
 *
 ***************************************************************/
PINLINE1 poly p_Init(ring r)
{
  p_CheckRing(r);
  poly p;
  omTypeAlloc0Bin(poly, p, r->PolyBin);
  p_SetRingOfPoly(p, r);
  return p;
}
PINLINE1 poly p_Init(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, NULL);
  return np;
}
PINLINE1 poly pInit()
{
  return p_Init(currRing);
}
PINLINE1 poly pInit(poly p)
{
  return p_Init(p, currRing);
}
PINLINE1 poly p_Head(poly p, ring r)
{
  if (p == NULL) return NULL;
  p_CheckPolyRing(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, p_nCopy(_pGetCoeff(p), r));
  return np;
}

/***************************************************************
 *
 * Operation on ExpVectors
 *
 ***************************************************************/
// ExpVextor(d_p) = ExpVector(s_p)
PINLINE1 void p_ExpVectorCopy(poly d_p, poly s_p, ring r)
{
  p_CheckPolyRing(d_p, r);
  p_CheckPolyRing(s_p, r);
  p_MemCopy_LengthGeneral(d_p->exp, s_p->exp, r->ExpLSize);
}
// ExpVector(p1) += ExpVector(p2)
PINLINE1 void p_ExpVectorAdd(poly p1, poly p2, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    passume2((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  passume2(!rRing_has_Comp(r) || p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif 
 
  p_MemAdd_LengthGeneral(p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(p1) -= ExpVector(p2)
PINLINE1 void p_ExpVectorSub(poly p1, poly p2, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    passume2(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  passume2(!rRing_has_Comp(r) || p_GetComp(p1, r) == p_GetComp(p2, r));
#endif 
 
  p_MemSub_LengthGeneral(p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorSum(poly pr, poly p1, poly p2, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
  p_CheckPolyRing(pr, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    passume2((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  passume2(!rRing_has_Comp(r) || p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif  

  p_MemSum_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpLSize);
}
// ExpVector(pr) = ExpVector(p1) - ExpVector(p2)
PINLINE1 void p_ExpVectorDiff(poly pr, poly p1, poly p2, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
  p_CheckPolyRing(pr, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    passume2(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  passume2(!rRing_has_Comp(r) || p_GetComp(p1, r) == p_GetComp(p2, r));
#endif 
 
  p_MemDiff_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpLSize);
}

PINLINE1 BOOLEAN p_ExpVectorEqual(poly p1, poly p2, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
 
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
  p_CheckPolyRing(p, r);
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
  p_CheckPolyRing(p, r);
  for (int j = r->N; j; j--)
      ev[j] = p_GetExp(p, j, r);

  ev[0] = _p_GetComp(p, r);
}
PINLINE1 void p_SetExpV(poly p, Exponent_t *ev, ring r)
{
  p_CheckPolyRing(p, r);
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
  p_CheckPolyRing(p, r);
  p_CheckPolyRing(q, r);

  p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->pCompLSize, r->ordsgn, 
                                    return 0, return 1, return -1);
}

PINLINE1 BOOLEAN p_LmEqual(poly p, poly q, ring r)
{
  p_CheckPolyRing(p, r);
  p_CheckPolyRing(q, r);
  int i = r->ExpLSize;
  unsigned long *ep = p->exp;
  unsigned long *eq = q->exp;

  do
  {
    i--;
    if (ep[i] != eq[i]) return FALSE;
  }
  while (i);
  return p_nEqual(pGetCoeff(p), pGetCoeff(q), r);
}

/***************************************************************
 *
 * divisibility
 *
 ***************************************************************/
static inline BOOLEAN _p_DivisibleBy2(poly a, poly b, ring r)
{
  int i=r->N;

  do
  {
    if (p_GetExp(a,i,r) > p_GetExp(b,i,r))
      return FALSE;
    i--;
  }
  while (i);
  return TRUE;
}
static inline BOOLEAN _p_DivisibleBy2(poly a, ring r_a, poly b, ring r_b)
{
  int i=r_a->N;
  passume1(r_a->N == r_b->N);

  do
  {
    if (p_GetExp(a,i,r_a) > p_GetExp(b,i,r_b))
      return FALSE;
    i--;
  }
  while (i);
  return TRUE;
}
PINLINE1 BOOLEAN p_DivisibleBy2(poly a, poly b, ring r)
{
  p_CheckPolyRing(a, r);
  p_CheckPolyRing(b, r);
  return _p_DivisibleBy2(a, b, r);
}
PINLINE1 BOOLEAN p_DivisibleBy1(poly a, poly b, ring r)
{
  p_CheckPolyRing(b, r);
  p_CheckIf(a != NULL, p_CheckPolyRing(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_DivisibleBy2(a, b, r);
  return FALSE;
}
PINLINE1 BOOLEAN p_DivisibleBy(poly a, poly b, ring r)
{
  p_CheckPolyRing(b, r);
  p_CheckIf(a!=NULL, p_CheckPolyRing(a, r));
  
  if (a != NULL && (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r)))
    return _p_DivisibleBy2(a,b,r);
  return FALSE;
}

PINLINE1 BOOLEAN p_ShortDivisibleBy(poly a, unsigned long sev_a, 
                                    poly b, unsigned long not_sev_b, ring r)
{
  p_CheckPolyRing(a, r);
  p_CheckPolyRing(b, r);
#ifndef PDIV_DEBUG
  passume2(p_GetShortExpVector(a, r) == sev_a);
  passume2(p_GetShortExpVector(b, r) == ~ not_sev_b);
  
  if (sev_a & not_sev_b) 
  {
    passume2(p_DivisibleBy2(a, b, r) == FALSE);
    return FALSE;
  }
  return p_DivisibleBy1(a, b, r);
#else
  return pDebugShortDivisibleBy(a, sev_a, r, b, not_sev_b, r);
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
