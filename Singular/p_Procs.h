/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *  File:    pProcs.h
 *  Purpose: declaration of primitive procs for polys
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Procs.h,v 1.1 2000-08-24 14:42:43 obachman Exp $
 *******************************************************************/
#ifndef P_PROCS_H
#define P_PROCS_H
#include "structs.h"

typedef poly (*p_Copy_Proc_Ptr)(poly p, ring r);
typedef void (*p_Delete_Proc_Ptr)(poly *p, ring r);
typedef poly (*p_ShallowCopyDelete_Proc_Ptr)(poly p, ring r, omBin dest_bin);
typedef poly (*p_Mult_n_Proc_Ptr)(poly p, number n, ring r);
typedef poly (*p_Mult_m_Proc_Ptr)(poly p, poly m, 
                              poly spNoether, ring r);
typedef poly (*p_Add_q_Proc_Ptr)(poly p, poly q, int & shorter, ring r);
typedef poly (*p_Minus_m_Mult_q_Proc_Ptr)(poly p, poly m, poly q, 
                                          int &shorter, poly spNoether, 
                                          ring r);

typedef struct p_Procs_s
{
  p_Copy_Proc_Ptr               p_Copy;
  p_Delete_Proc_Ptr             p_Delete;
  p_ShallowCopyDelete_Proc_Ptr  p_ShallowCopyDelete;
  p_Mult_n_Proc_Ptr             p_Mult_n;
  p_Mult_m_Proc_Ptr             p_Mult_m;
  p_Add_q_Proc_Ptr              p_Add_q;
  p_Minus_m_Mult_q_Proc_Ptr     p_Minus_m_Mult_q;
} pProcs_s;

// returns a copy of p
inline poly p_Copy(poly p, ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Copy(p, r);
}

// deletes *p, and sets *p to NULL
inline void p_Delete(poly *p, ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  r->p_Procs->p_Delete(p, r);
}

// returns p+q, destroys p and q
inline poly p_Add_q(poly p, poly q, ring r = currRing)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Add_q(p, q, shorter, r);
}

// returns p*n, destroys p
inline poly p_Mult_n(poly p, number n, ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_n(p, n, r);
}

// returns Copy(p)*m, does neither destroy p nor m
inline poly p_Mult_m(poly p, poly m, ring r = currRing)
{
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Mult_m(p, m, NULL, r);
}

// return p - m*Copy(q), destroys p; const: p,m
inline poly p_Minus_m_Mult_q(poly p, poly m, poly q, ring r = currRing)
{
  int shorter;
  assume(r != NULL && r->p_Procs != NULL);
  return r->p_Procs->p_Minus_m_Mult_q(p, m, q, shorter, NULL, r);
}
  
void p_SetProcs(ring r, p_Procs_s* p_Procs);
#ifdef RDEBUG
void p_Debug_GetSpecNames(const ring r, char* &field, char* &length, char* &ord);
void p_Debug_GetProcNames(const ring r, p_Procs_s* p_Procs);
#endif

#endif /* ! P_PROCS_H */
