/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *  File:    pProcs.h
 *  Purpose: declaration of primitive procs for polys
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Procs.h,v 1.3 2000-09-04 13:39:02 obachman Exp $
 *******************************************************************/
#ifndef P_PROCS_H
#define P_PROCS_H
#include "structs.h"

// define/undef P_INLINE to inline some procedures
#undef NO_P_INLINE
#if defined(NDEBUG) && !defined(NO_INLINE)
#define P_INLINE inline
#else
#define NO_P_INLINE 1
#define P_INLINE
#endif


// NOTE: the actual procedures below are not inlined,
// only the dispatching function call 

/*------------- Allocation/Deletion ----------------------*/
// returns a copy of p
P_INLINE poly p_Copy(poly p, const ring r = currRing);
// deletes *p, and sets *p to NULL
P_INLINE void p_Delete(poly *p, const ring r = currRing);

/*------------- comparisons ----------------------*/
// returns 1 if Lm(p) > Lm(q)
//         0 if Lm(p) = Lm(q)
//        -1 if Lm(p) < Lm(q)
// assumes p != NULL && q != NULL
P_INLINE int p_LmCmp(const poly p, const poly q, const ring r = currRing);

/*------------- Arithmetic operations ----------------------*/
/* NOTE as a general rule that 
   pp means input is constant; p means input is destroyed */
// returns -p
P_INLINE poly p_Neg(poly p, const ring r = currRing);
// returns p*n, p is const
P_INLINE poly pp_Mult_nn(poly p, number n, const ring r = currRing);
// returns p*n, destroys p
P_INLINE poly p_Mult_nn(poly p, number n, const ring r = currRing);
// returns p*m, does neither destroy p nor m
P_INLINE poly pp_Mult_mm(poly p, poly m, const ring r = currRing);
// returns p*m, destroys p, const: m
P_INLINE poly p_Mult_mm(poly p, poly m, const ring r = currRing);
// returns p+q, destroys p and q
P_INLINE poly p_Add_q(poly p, poly q, const ring r = currRing);
// return p - m*q, destroys p; const: q,m
P_INLINE poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, const ring r = currRing);
// returns p + m*q destroys p, const: q, m
P_INLINE poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, const ring r = currRing);
// returns p*q, destroys p and q
P_INLINE poly p_Mult_q(poly p, poly q, const ring r = currRing);
// returns p*q, does neither destroy p nor q
P_INLINE poly pp_Mult_qq(poly p, poly q, const ring r = currRing);

/*------------- p_Proc stuff ----------------------*/
typedef poly (*p_Copy_Proc_Ptr)(poly p, const ring r);
typedef void (*p_Delete_Proc_Ptr)(poly *p, const ring r);
typedef poly (*p_ShallowCopyDelete_Proc_Ptr)(poly p, const ring r, omBin_s* dest_bin);
typedef poly (*p_Mult_nn_Proc_Ptr)(poly p, number n, const ring r);
typedef poly (*pp_Mult_nn_Proc_Ptr)(poly p, number n, const ring r);
typedef poly (*p_Mult_mm_Proc_Ptr)(poly p, poly m, const ring r);
typedef poly (*pp_Mult_mm_Proc_Ptr)(poly p, poly m, 
                                   poly spNoether, const ring r);
typedef poly (*p_Add_q_Proc_Ptr)(poly p, poly q, int & shorter, const ring r);
typedef poly (*p_Minus_mm_Mult_qq_Proc_Ptr)(poly p, poly m, poly q, 
                                                int &shorter, poly spNoether, 
                                                const ring r);
typedef poly (*p_Neg_Proc_Ptr)(poly p, const ring r);

typedef struct p_Procs_s
{
  p_Copy_Proc_Ptr               p_Copy;
  p_Delete_Proc_Ptr             p_Delete;
  p_ShallowCopyDelete_Proc_Ptr  p_ShallowCopyDelete;
  p_Mult_nn_Proc_Ptr            p_Mult_nn;
  pp_Mult_nn_Proc_Ptr           pp_Mult_nn;
  pp_Mult_mm_Proc_Ptr           pp_Mult_mm;
  p_Mult_mm_Proc_Ptr            p_Mult_mm;
  p_Add_q_Proc_Ptr              p_Add_q;
  p_Minus_mm_Mult_qq_Proc_Ptr   p_Minus_mm_Mult_qq;
  p_Neg_Proc_Ptr                p_Neg;
} pProcs_s;

  
void p_SetProcs(ring r, p_Procs_s* p_Procs);
#ifdef RDEBUG
void p_Debug_GetSpecNames(const ring r, char* &field, char* &length, 
                          char* &ord);
void p_Debug_GetProcNames(const ring r, p_Procs_s* p_Procs);
#endif

#include "p_Inline.cc"

#endif /* ! P_PROCS_H */
