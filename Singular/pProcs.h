/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: pProcs.h,v 1.1 1999-09-29 10:59:35 obachman Exp $ */
/*
*  ABSTRACT -  Declaration of Routines for primitive poly arithmetic
*/

#include "mod2.h"
#include "polys.h"
#include "mmemory.h"


// just as macros, for the time being
#define p_Mult_n            p_Mult_n_General
#define p_Add_q             p_Add_q_General
#define p_Mult_m            p_Mult_m_General
#define p_Minus_m_Mult_q    p_Minus_m_Mult_q_General


/////////////////////////////////////////////////////////////////////////
// Returns:  p*n
// Destroys: p
// Const:    n
poly p_Mult_n_General(poly p, number n);

/////////////////////////////////////////////////////////////////////////
// Returns:  p + q, *lp == pLength(p+q), p+q are from heap
// Destroys: p, q
// Assume:   *lp == NULL || pLength(p) == *lp && pLength(q) == q
//           p, q are from heap
poly p_Add_q_General(poly p, poly q, 
                     int *lp = NULL, int lq = 0,
                     memHeap heap = mm_specHeap);


/////////////////////////////////////////////////////////////////////////
// Returns: m*a1, newly allocated from heap
//          if spNoether != NULL, then monoms whioch are smaller
//          then spNoether are cut
// Assume:  m is Monom 
// Const: p, m
poly  p_Mult_m_General(poly p,
                       poly m, 
                       poly spNoether = NULL,
                       memHeap heap = mm_specHeap);

/////////////////////////////////////////////////////////////////////////
// Return :  p - m*q, allocated from heap
// Assume:   p is from heap, m is Monom
//           *lp == NULL || pLength(p) == *lp && pLenth(q) == lq
// Destroy:  p
// Const:    m, q
poly p_Minus_m_Mult_q_General(poly p, 
                              poly m,
                              poly q, 
                              poly spNoether = NULL,
                              int *lp = NULL,  
                              int lq = 0,
                              memHeap heap = mm_specHeap);


