/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Inline.cc
 *  Purpose: implementation of p_* related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Inline.cc,v 1.1 2000-08-29 14:10:26 obachman Exp $
 *******************************************************************/
#ifndef P_INLINE_CC
#define P_INLINE_CC

#if !defined(NO_P_INLINE) || defined(POLYS_IMPL_CC)

// returns a copy of p
P_INLINE poly p_Copy(poly p, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Copy(p, r);
}

// deletes *p, and sets *p to NULL
P_INLINE void p_Delete(poly *p, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  r->p_Procs->p_Delete(p, r);
}

// returns p+q, destroys p and q
P_INLINE poly p_Add_q(poly p, poly q, const ring r = currRing)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Add_q(p, q, shorter, r);
}

// returns p*n, destroys p
P_INLINE poly p_Mult_nn(poly p, number n, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_nn(p, n, r);
}

// returns p*n, does not destroy p
P_INLINE poly pp_Mult_nn(poly p, number n, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->pp_Mult_nn(p, n, r);
}

// returns Copy(p)*m, does neither destroy p nor m
P_INLINE poly pp_Mult_mm(poly p, poly m, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->pp_Mult_mm(p, m, NULL, r);
}

// returns p*m, destroys p, const: m
P_INLINE poly p_Mult_mm(poly p, poly m, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_mm(p, m, r);
}

// return p - m*Copy(q), destroys p; const: p,m
P_INLINE poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, const ring r = currRing)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r);
}
// returns -p, destroys p
P_INLINE poly p_Neg(poly p, const ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Neg(p, r);
}

extern poly  _p_Mult_q(poly p, poly q, const int copy, const ring r);
// returns p*q, destroys p and q
P_INLINE poly p_Mult_q(poly p, poly q, const ring r = currRing)
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
// returns p*q, does neither destroy p nor q
P_INLINE poly pp_Mult_qq(poly p, poly q, const ring r = currRing)
{
  if (p == NULL || q == NULL) return NULL;

  if (pNext(p) == NULL)
    return r->p_Procs->pp_Mult_mm(q, p, NULL, r);
  
  if (pNext(q) == NULL)
    return r->p_Procs->pp_Mult_mm(p, q, NULL, r);
  
  return _p_Mult_q(p, q, 1, r);
}

#include "p_MemCmp.h"
#include "polys-impl.h"

P_INLINE int p_LmCmp(poly p, poly q, const ring r = currRing)
{
  assume(p != NULL && q != NULL);
  
  p_MemCmp_LengthGeneral_OrdGeneral(p->exp.l, q->exp.l, 
                                    r->pCompLSize, r->ordsgn,
                                    return 0, return 1, return -1);
}


#endif // defined(P_INLINE) || defined(POLYS_CC)
#endif // P_INLINE_CC

