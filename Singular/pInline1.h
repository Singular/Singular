/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline1.h
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline1.h,v 1.21 2003-03-11 16:43:15 Singular Exp $
 *******************************************************************/
#ifndef PINLINE1_H
#define PINLINE1_H

#ifndef PDIV_DEBUG
// define to enable debugging/statistics of pLmShortDivisibleBy
// #define PDIV_DEBUG
#endif
#include <mylimits.h>
#include "p_MemCmp.h"
#include "tok.h"
#include "numbers.h"

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
 p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->CmpL_Size, r->ordsgn,    \
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
// adjustments for negative weights
PINLINE1 void p_MemAdd_NegWeightAdjust(poly p, ring r)
{
  if (r->NegWeightL_Offset != NULL)
  {
    for (int i=r->NegWeightL_Size-1; i>=0; i--)
    {
      p->exp[r->NegWeightL_Offset[i]] -= POLY_NEGWEIGHT_OFFSET;
    }
  }
}
PINLINE1 void p_MemSub_NegWeightAdjust(poly p, ring r)
{
  if (r->NegWeightL_Offset != NULL)
  {
    for (int i=r->NegWeightL_Size-1; i>=0; i--)
    {
      p->exp[r->NegWeightL_Offset[i]] += POLY_NEGWEIGHT_OFFSET;
    }
  }
}
// ExpVextor(d_p) = ExpVector(s_p)
PINLINE1 void p_ExpVectorCopy(poly d_p, poly s_p, ring r)
{
  p_LmCheckPolyRing1(d_p, r);
  p_LmCheckPolyRing1(s_p, r);
  p_MemCopy_LengthGeneral(d_p->exp, s_p->exp, r->ExpL_Size);
}

PINLINE1 poly p_Init(ring r, omBin bin)
{
  p_CheckRing1(r);
  pAssume1(bin != NULL && r->PolyBin->sizeW == bin->sizeW);
  poly p;
  omTypeAlloc0Bin(poly, p, bin);
  p_MemAdd_NegWeightAdjust(p, r);
  p_SetRingOfLm(p, r);
  return p;
}
PINLINE1 poly p_Init(ring r)
{
  return p_Init(r, r->PolyBin);
}

PINLINE1 poly p_LmInit(poly p, ring r)
{
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, NULL);
  return np;
}
PINLINE1 poly p_LmInit(poly s_p, ring s_r, ring d_r)
{
  pAssume1(d_r != NULL);
  return p_LmInit(s_p, s_r, d_r, d_r->PolyBin);
}
PINLINE1 poly p_LmInit(poly s_p, ring s_r, ring d_r, omBin d_bin)
{
  p_LmCheckPolyRing1(s_p, s_r);
  p_CheckRing(d_r);
  pAssume1(d_r->N <= s_r->N);
  poly d_p = p_Init(d_r, d_bin);
  for (int i=d_r->N; i>0; i--)
  {
    p_SetExp(d_p, i, p_GetExp(s_p, i,s_r), d_r);
  }
  if (rRing_has_Comp(d_r))
  {
    p_SetComp(d_p, p_GetComp(s_p,s_r), d_r);
  }
  p_Setm(d_p, d_r);
  return d_p;
}
PINLINE1 poly p_Head(poly p, ring r)
{
  if (p == NULL) return NULL;
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  p_ExpVectorCopy(np, p, r);
  _pNext(np) = NULL;
  _pSetCoeff0(np, n_Copy(_pGetCoeff(p), r));
  return np;
}

PINLINE1 poly p_LmShallowCopyDelete(poly p, const ring r, omBin bin)
{
  p_LmCheckPolyRing1(p, r);
  pAssume1(bin->sizeW == r->PolyBin->sizeW);
  poly new_p = p_New(r);
  p_MemCopy_LengthGeneral(new_p->exp, p->exp, r->ExpL_Size);
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
// ExpVector(p1) += ExpVector(p2)
PINLINE1 void p_ExpVectorAdd(poly p1, poly p2, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif

  p_MemAdd_LengthGeneral(p1->exp, p2->exp, r->ExpL_Size);
  p_MemAdd_NegWeightAdjust(p1, r);
}
// ExpVector(p1) -= ExpVector(p2)
PINLINE1 void p_ExpVectorSub(poly p1, poly p2, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0 ||
          p_GetComp(p1, r) == p_GetComp(p2, r));
#endif

  p_MemSub_LengthGeneral(p1->exp, p2->exp, r->ExpL_Size);
  p_MemSub_NegWeightAdjust(p1, r);

}
// ExpVector(p1) += ExpVector(p2) - ExpVector(p3)
PINLINE1 void p_ExpVectorAddSub(poly p1, poly p2, poly p3, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(p3, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) + p_GetExp(p2, i, r) >= p_GetExp(p3, i, r));
  pAssume1(p_GetComp(p1, r) == 0 ||
           (p_GetComp(p2, r) - p_GetComp(p3, r) == 0) ||
           (p_GetComp(p1, r) == p_GetComp(p2, r) - p_GetComp(p3, r)));
#endif

  p_MemAddSub_LengthGeneral(p1->exp, p2->exp, p3->exp, r->ExpL_Size);
  // no need to adjust in case of NegWeights
}

// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorSum(poly pr, poly p1, poly p2, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(pr, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif

  p_MemSum_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpL_Size);
  p_MemAdd_NegWeightAdjust(p1, r);
}
// ExpVector(pr) = ExpVector(p1) - ExpVector(p2)
PINLINE1 void p_ExpVectorDiff(poly pr, poly p1, poly p2, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(pr, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(!rRing_has_Comp(r) || p_GetComp(p1, r) == p_GetComp(p2, r));
#endif

  p_MemDiff_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpL_Size);
  p_MemSub_NegWeightAdjust(p1, r);
}

PINLINE1 BOOLEAN p_ExpVectorEqual(poly p1, poly p2, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);

  int i = r->ExpL_Size;
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
  p_LmCheckPolyRing1(p, r);
  unsigned long s = p_GetTotalDegree(p->exp[r->VarL_Offset[0]],
                                     r,
                                     r->MinExpPerLong);
  for (int i=r->VarL_Size-1; i>0; i--)
  {
    s += p_GetTotalDegree(p->exp[r->VarL_Offset[i]], r);
  }
  return s;
}

PINLINE1 void p_GetExpV(poly p, int *ev, ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (int j = r->N; j; j--)
      ev[j] = p_GetExp(p, j, r);

  ev[0] = _p_GetComp(p, r);
}
PINLINE1 void p_SetExpV(poly p, int *ev, ring r)
{
  p_LmCheckPolyRing1(p, r);
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
  p_LmCheckPolyRing1(p, r);
  p_LmCheckPolyRing1(q, r);

  p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->CmpL_Size, r->ordsgn,
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
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
  return _p_LmDivisibleByNoComp(a, b, r);
}
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, poly b, ring r)
{
  p_LmCheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
}
PINLINE1 BOOLEAN p_DivisibleBy(poly a, poly b, ring r)
{
  pIfThen1(b!=NULL, p_LmCheckPolyRing1(b, r));
  pIfThen1(a!=NULL, p_LmCheckPolyRing1(a, r));

  if (a != NULL && (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r)))
    return _p_LmDivisibleByNoComp(a,b,r);
  return FALSE;
}
PINLINE1 BOOLEAN p_DivisibleBy(poly a, ring r_a, poly b, ring r_b)
{
  pIfThen1(b!=NULL, p_LmCheckPolyRing1(b, r_b));
  pIfThen1(a!=NULL, p_LmCheckPolyRing1(a, r_a));
  if (a != NULL) return _p_LmDivisibleBy(a, r_a, b, r_b);
  return FALSE;
}
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, ring r_a, poly b, ring r_b)
{
  p_LmCheckPolyRing(a, r_a);
  p_LmCheckPolyRing(b, r_b);
  return _p_LmDivisibleBy(a, r_a, b, r_b);
}
PINLINE1 BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a,
                                    poly b, unsigned long not_sev_b, ring r)
{
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
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
  p_LmCheckPolyRing1(a, r_a);
  p_LmCheckPolyRing1(b, r_b);
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

/***************************************************************
 *
 * Misc things on Lm
 *
 ***************************************************************/
// test if the monomial is a constant as a vector component
// i.e., test if all exponents are zero
PINLINE1 BOOLEAN p_LmIsConstantComp(const poly p, const ring r)
{
  //p_LmCheckPolyRing(p, r);
  int i = r->VarL_Size - 1;

  do
  {
    if (p->exp[r->VarL_Offset[i]] != 0)
      return FALSE;
    i--;
  }
  while (i >= 0);
  return TRUE;
}
// test if monomial is a constant, i.e. if all exponents and the component
// is zero
PINLINE1 BOOLEAN p_LmIsConstant(const poly p, const ring r)
{
  if (p_LmIsConstantComp(p, r))
    return (p_GetComp(p, r) == 0);
  return FALSE;
}

// like the respective p_LmIs* routines, except that p might be empty
PINLINE1 BOOLEAN p_IsConstantComp(const poly p, const ring r)
{
  if (p == NULL) return TRUE;
  return (pNext(p)==NULL) && p_LmIsConstantComp(p, r);
}

PINLINE1 BOOLEAN p_IsConstant(const poly p, const ring r)
{
  if (p == NULL) return TRUE;
  return (pNext(p)==NULL) && p_LmIsConstant(p, r);
}

PINLINE1 BOOLEAN p_IsUnit(const poly p, const ring r)
{
  if (p == NULL) return FALSE;
  return p_LmIsConstant(p, r);
}

PINLINE1 BOOLEAN p_LmExpVectorAddIsOk(const poly p1, const poly p2,
                                      const ring r)
{
  p_LmCheckPolyRing(p1, r);
  p_LmCheckPolyRing(p2, r);
  unsigned long l1, l2, divmask = r->divmask;
  int i;

  for (i=0; i<r->VarL_Size; i++)
  {
    l1 = p1->exp[r->VarL_Offset[i]];
    l2 = p2->exp[r->VarL_Offset[i]];
    // do the divisiblity trick
    if ( (l1 > ULONG_MAX - l2) ||
         (((l1 & divmask) ^ (l2 & divmask)) != ((l1 + l2) & divmask)))
      return FALSE;
  }
  return TRUE;
}

#endif // !defined(NO_PINLINE1) || defined(PINLINE1_CC)
#endif // PINLINE1_CC
