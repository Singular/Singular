/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline2.h
 *  Purpose: implementation of poly procs which are of constant time
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline2.h,v 1.1 2000-09-12 16:01:05 obachman Exp $
 *******************************************************************/
#ifndef PINLINE2_H
#define PINLINE2_H

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 *
 ***************************************************************/
#if !defined(NO_PINLINE2) || defined(PINLINE2_CC)

#include "mod2.h"
#include "omalloc.h"
#include "structs.h"
#include "polys.h"
#include "numbers.h"
#include "p_Procs.h"


PINLINE2 number p_SetCoeff(poly p, number n, ring r)
{
  p_CheckPolyRing(p, r);
  p_nDelete(&(p->coef), r);
  (p)->coef=n;
  return n;
}

// order
PINLINE2 Order_t p_GetOrder(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  return ((p)->exp[r->pOrdIndex]);
}

PINLINE2 Order_t p_SetOrder(poly p, long o, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(o >= 0);
  return (p)->exp[r->pOrdIndex] = o;
}

// component
PINLINE2  unsigned long p_SetComp(poly p, unsigned long c, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(rRing_has_Comp(r));
  __p_GetComp(p,r) = c;
  return c;
}
PINLINE2 unsigned long p_IncrComp(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(rRing_has_Comp(r));
  return ++(__p_GetComp(p,r));
}
PINLINE2 unsigned long p_DecrComp(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(rRing_has_Comp(r));
  passume2(__p_GetComp(p,r) > 0);
  return --(__p_GetComp(p,r));
}
PINLINE2 unsigned long p_AddComp(poly p, unsigned long v, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(rRing_has_Comp(r));
  return __p_GetComp(p,r) += v;
}
PINLINE2 unsigned long p_SubComp(poly p, unsigned long v, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(rRing_has_Comp(r));
  passume2(__p_GetComp(p,r) >= v);
  return __p_GetComp(p,r) -= v;
}

// exponent
PINLINE2 Exponent_t p_GetExp(poly p, int v, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(v > 0 && v <= r->N);
  return (p->exp[(r->VarOffset[v] & 0xffffff)] >> (r->VarOffset[v] >> 24))
          & r->bitmask;
}
PINLINE2 Exponent_t p_SetExp(poly p, int v, int e, ring r)
{
  p_CheckPolyRing(p, r);
  passume2(v>0 && v <= r->N);
  passume2(e>=0);
  passume2((unsigned int) e<=r->bitmask);

  // shift e to the left:
  register int shift = r->VarOffset[v] >> 24;
  unsigned long ee = ((unsigned long)e) << shift /*(r->VarOffset[v] >> 24)*/;
  // clear the bits in the exponent vector:
  register int offset = (r->VarOffset[v] & 0xffffff);
  p->exp[offset]  &=
    ~( r->bitmask << shift );
  // insert e with |
  p->exp[ offset ] |= ee;
  return e;
}

// the following should be implemented more efficiently
PINLINE2  Exponent_t p_IncrExp(poly p, int v, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t e = p_GetExp(p,v,r);
  e++;
  return p_SetExp(p,v,e,r);
}
PINLINE2  Exponent_t p_DecrExp(poly p, int v, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t e = p_GetExp(p,v,r);
  passume2(e > 0);
  e--;
  return p_SetExp(p,v,e,r);
}
PINLINE2  Exponent_t p_AddExp(poly p, int v, Exponent_t ee, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t e = p_GetExp(p,v,r);
  e += ee;
  return p_SetExp(p,v,e,r);
}
PINLINE2  Exponent_t p_SubExp(poly p, int v, Exponent_t ee, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t e = p_GetExp(p,v,r);
  passume2(e >= ee);
  e -= ee;
  return p_SetExp(p,v,e,r);
}
PINLINE2  Exponent_t p_MultExp(poly p, int v, Exponent_t ee, ring r)
{
  p_CheckPolyRing(p, r);
  Exponent_t e = p_GetExp(p,v,r);
  e *= ee;
  return p_SetExp(p,v,e,r);
}

PINLINE2 Exponent_t p_GetExpSum(poly p1, poly p2, int i, ring r)
{
  p_CheckPolyRing(p1, r);
  p_CheckPolyRing(p2, r);
  return p_GetExp(p1,i,r) + p_GetExp(p2,i,r);
}
PINLINE2 Exponent_t p_GetExpDiff(poly p1, poly p2, int i, ring r)
{
  return p_GetExp(p1,i,r) - p_GetExp(p2,i,r);
}


/***************************************************************
 *
 * Allocation/Initalization/Deletion 
 *
 ***************************************************************/
PINLINE2 poly p_New(ring r)
{
  p_CheckRing(r);
  poly p;
  omTypeAllocBin(poly, p, r->PolyBin);
  p_SetRingOfPoly(p, r);
  return p;
}

PINLINE2 void p_Delete1(poly *p, ring r)
{
  p_CheckIf(*p != NULL, p_CheckPolyRing(*p, r));
  assume(r != NULL);
  poly h = *p;
  if (h != NULL)
  {
    p_nDelete(&_pGetCoeff(h), r);
    *p = _pNext(h);
    omFreeBinAddr(h);
  }

}
PINLINE2 void p_Free(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  omFreeBinAddr(p);
}
PINLINE2 poly p_FreeAndNext(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  poly pnext = _pNext(p);
  omFreeBinAddr(p);
  return pnext;
}
PINLINE2 void p_LmDelete(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  p_nDelete(&_pGetCoeff(p), r);
  omFreeBinAddr(p);
}
PINLINE2 poly p_LmDeleteAndNext(poly p, ring r)
{
  p_CheckPolyRing(p, r);
  poly pnext = _pNext(p);
  p_nDelete(&_pGetCoeff(p), r);
  omFreeBinAddr(p);
  return pnext;
}

/***************************************************************
 *
 * Misc routines
 *
 ***************************************************************/
PINLINE2 int p_Cmp(poly p1, poly p2, ring r)
{
  if (p2==NULL)
    return 1;
  if (p1==NULL)
    return -1;
  return p_LmCmp(p1,p2,r);
}

/***************************************************************
 *
 * Dispatcher to r->p_Procs
 *
 ***************************************************************/
// returns a copy of p
PINLINE2 poly p_Copy(poly p, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Copy(p, r);
}

// deletes *p, and sets *p to NULL
PINLINE2 void p_Delete(poly *p, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  r->p_Procs->p_Delete(p, r);
}

// returns p+q, destroys p and q
PINLINE2 poly p_Add_q(poly p, poly q, const ring r)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Add_q(p, q, shorter, r);
}

PINLINE2 poly p_Add_q(poly p, poly q, int &lp, int lq, const ring r)
{
  int shorter;
  poly res = r->p_Procs->p_Add_q(p, q, shorter, r);
  lp = (lp + lq) - shorter;
  return res;
}

// returns p*n, destroys p
PINLINE2 poly p_Mult_nn(poly p, number n, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_nn(p, n, r);
}

// returns p*n, does not destroy p
PINLINE2 poly pp_Mult_nn(poly p, number n, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->pp_Mult_nn(p, n, r);
}

// returns Copy(p)*m, does neither destroy p nor m
PINLINE2 poly pp_Mult_mm(poly p, poly m, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->pp_Mult_mm(p, m, NULL, r);
}

// returns p*m, destroys p, const: m
PINLINE2 poly p_Mult_mm(poly p, poly m, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_mm(p, m, r);
}

// return p - m*Copy(q), destroys p; const: p,m
PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, const ring r)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r);
}
PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq, 
                                 poly spNoether, const ring r)
{
  int shorter;
  poly res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, spNoether, r);
  lp = (lp + lq) - shorter;
  return res;
}
  
// returns -p, destroys p
PINLINE2 poly p_Neg(poly p, const ring r)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Neg(p, r);
}

extern poly  _p_Mult_q(poly p, poly q, const int copy, const ring r);
// returns p*q, destroys p and q
PINLINE2 poly p_Mult_q(poly p, poly q, const ring r)
{
  if (p == NULL) 
  {
    r->p_Procs->p_Delete(&q, r);
    return NULL;
  }
  if (q == NULL)
  {
    r->p_Procs->p_Delete(&p, r);
    return NULL;
  }

  if (pNext(p) == NULL)
  {
    
    q = r->p_Procs->p_Mult_mm(q, p, r);
    r->p_Procs->p_Delete(&p, r);
    return q;
  }
  
  if (pNext(q) == NULL)
  {
    p = r->p_Procs->p_Mult_mm(p, q, r);
    r->p_Procs->p_Delete(&q, r);
    return p;
  }
  
  return _p_Mult_q(p, q, 0, r);
}

BOOLEAN pEqualPolys(poly p1,poly p2);
// returns p*q, does neither destroy p nor q
PINLINE2 poly pp_Mult_qq(poly p, poly q, const ring r)
{
  if (p == NULL || q == NULL) return NULL;

  if (pNext(p) == NULL)
    return r->p_Procs->pp_Mult_mm(q, p, NULL, r);
  
  if (pNext(q) == NULL)
    return r->p_Procs->pp_Mult_mm(p, q, NULL, r);

  poly qq = q;
  if (p == q)
    qq = p_Copy(q, r);
    
  poly res = _p_Mult_q(p, qq, 1, r);
  if (qq != q)
    p_Delete(&qq, r);
  return res;
}

// returns p + m*q destroys p, const: q, m
// this should be implemented more efficiently
PINLINE2 poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, const ring r)
{
  poly res;
  int shorter;
  number n_old = pGetCoeff(m);
  number n_neg = p_nCopy(n_old, r);
  n_neg = p_nNeg(n_neg, r);
  pSetCoeff0(m, n_neg);
  
  res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r);
  pSetCoeff0(m, n_old);
  p_nDelete(&n_neg, r);
  return res;
}


#endif // !defined(NO_PINLINE2) || defined(POLYS_IMPL_CC)
#endif // PINLINE2_H

