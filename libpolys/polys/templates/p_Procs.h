/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *  File:    pProcs.h
 *  Purpose: declaration of primitive procs for polys
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/
#ifndef P_PROCS_H
#define P_PROCS_H

#include "polys/monomials/ring.h"

/*------------- p_Proc stuff ----------------------*/
typedef poly (*p_Copy_Proc_Ptr)(poly p, const ring r);
typedef void (*p_Delete_Proc_Ptr)(poly *p, const ring r);
typedef poly (*p_ShallowCopyDelete_Proc_Ptr)(poly p, const ring r, omBin dest_bin);
typedef poly (*p_Mult_nn_Proc_Ptr)(poly p, const number n, const ring r);
typedef poly (*pp_Mult_nn_Proc_Ptr)(poly p, const number n, const ring r);
typedef poly (*p_Mult_mm_Proc_Ptr)(poly p, const poly m, const ring r);
typedef poly (*pp_Mult_mm_Proc_Ptr)(poly p, const poly m, const ring r);
typedef poly (*pp_Mult_mm_Noether_Proc_Ptr)(poly p, const poly m,
                                            const poly spNoether, int &ll,
                                            const ring r);
typedef poly (*p_Add_q_Proc_Ptr)(poly p, poly q, int & shorter, const ring r);
typedef poly (*p_Minus_mm_Mult_qq_Proc_Ptr)(poly p, poly m, poly q,
                                            int &shorter, const poly spNoether,
                                            const ring r);
typedef poly (*p_Neg_Proc_Ptr)(poly p, const ring r);
typedef poly (*pp_Mult_Coeff_mm_DivSelect_Proc_Ptr)(poly p, const poly m,
                                                    int &shorter,const ring r);
typedef poly (*pp_Mult_Coeff_mm_DivSelectMult_Proc_Ptr)
  (poly p,const poly m, const poly a, const poly b, int &shorter,const ring r);

typedef poly (*p_Merge_q_Proc_Ptr)(poly p, poly q, const ring r);
typedef void (*p_kBucketSetLm_Proc_Ptr)(kBucket_pt bucket);

typedef struct p_Procs_s
{
  p_Copy_Proc_Ptr                       p_Copy;
  p_Delete_Proc_Ptr                     p_Delete;
  p_ShallowCopyDelete_Proc_Ptr          p_ShallowCopyDelete;
  p_Mult_nn_Proc_Ptr                    p_Mult_nn;
  pp_Mult_nn_Proc_Ptr                   pp_Mult_nn;
  pp_Mult_mm_Proc_Ptr                   pp_Mult_mm;
  pp_Mult_mm_Proc_Ptr                   pp_mm_Mult;
  pp_Mult_mm_Noether_Proc_Ptr           pp_Mult_mm_Noether;
  p_Mult_mm_Proc_Ptr                    p_Mult_mm;
  p_Mult_mm_Proc_Ptr                    p_mm_Mult;
  p_Add_q_Proc_Ptr                      p_Add_q;
  p_Minus_mm_Mult_qq_Proc_Ptr           p_Minus_mm_Mult_qq;
  p_Neg_Proc_Ptr                        p_Neg;
  pp_Mult_Coeff_mm_DivSelect_Proc_Ptr   pp_Mult_Coeff_mm_DivSelect;
  pp_Mult_Coeff_mm_DivSelectMult_Proc_Ptr   pp_Mult_Coeff_mm_DivSelectMult;
  p_Merge_q_Proc_Ptr                    p_Merge_q;
  p_kBucketSetLm_Proc_Ptr               p_kBucketSetLm;
} pProcs_s;


void p_ProcsSet(ring r, p_Procs_s* p_Procs);
#ifdef RDEBUG
void p_Debug_GetSpecNames(const ring r, const char* &field, const char* &length,
                          const char* &ord);
void p_Debug_GetProcNames(const ring r, p_Procs_s* p_Procs);
#endif
// if set, then dynamic p_procs are used, otherwise static
extern const BOOLEAN p_procs_dynamic;

#endif /* ! P_PROCS_H */
