/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prProcs.h,v 1.2 2000-02-01 15:30:26 Singular Exp $ */
/*
*  ABSTRACT -  Declaration of Routines for primitive poly arithmetic
*/

#include "polys.h"
#include "mmemory.h"

// just as macros, for the time being
#define pr_Mult_n            pr_Mult_n_General
#define pr_Add_q             pr_Add_q_General
#define pr_Mult_m            pr_Mult_m_General
#define pr_Minus_m_Mult_q    pr_Minus_m_Mult_q_General

/////////////////////////////////////////////////////////////////////////
// Returns:  p*n
// Destroys: p
// Const:    n
poly pr_Mult_n_General(poly p, number n);

/////////////////////////////////////////////////////////////////////////
// Returns:  p + q, *lp == pLength(p+q), p+q are from heap
// Destroys: p, q
// Assume:   *lp == NULL || pLength(p) == *lp && pLength(q) == q
//           p, q are from heap
poly pr_Add_q_General(poly p, poly q,
                     int *lp = NULL, int lq = 0,
                     ring r = currRing);


/////////////////////////////////////////////////////////////////////////
// Returns: m*a1, newly allocated from heap
//          if spNoether != NULL, then monoms whioch are smaller
//          then spNoether are cut
// Assume:  m is Monom
// Const: p, m
poly  pr_Mult_m_General(poly p,
                       poly m,
                       poly spNoether = NULL,
                       ring r = currRing);

/////////////////////////////////////////////////////////////////////////
// Return :  p - m*q, allocated from heap
// Assume:   p is from heap, m is Monom
//           *lp == NULL || pLength(p) == *lp && pLenth(q) == lq
// Destroy:  p
// Const:    m, q
poly pr_Minus_m_Mult_q_General(poly p,
                              poly m,
                              poly q,
                              poly spNoether = NULL,
                              int *lp = NULL,
                              int lq = 0,
                              ring r = currRing);


