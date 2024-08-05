/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Numbers.h
 *  Purpose: macros/inline functions for number operations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/
#ifndef P_NUMBERS_H
#define P_NUMBERS_H

#include "misc/auxiliary.h"
#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "polys/monomials/ring.h"

static FORCE_INLINE number n_Copy_FieldGeneral(number n,  const coeffs r)
{ return n_Copy(n,r); }

static FORCE_INLINE void   n_Delete_FieldGeneral(number* p, const coeffs r)
{ n_Delete(p,r); }

static FORCE_INLINE number n_Mult_FieldGeneral(number n1, number n2, const coeffs r)
{ return n_Mult(n1, n2, r); }

static FORCE_INLINE number n_Add_FieldGeneral(number n1, number n2, const coeffs r)
{ return n_Add(n1, n2, r); }

static FORCE_INLINE BOOLEAN n_IsZero_FieldGeneral(number n, const coeffs r)
{ return n_IsZero(n, r); }

static FORCE_INLINE BOOLEAN n_Equal_FieldGeneral(number n1, number n2, const coeffs r)
{ return n_Equal(n1, n2, r); }

static FORCE_INLINE number n_Neg_FieldGeneral(number n, const coeffs r)
{ return n_InpNeg(n, r); }

static FORCE_INLINE number n_Sub_FieldGeneral(number n1, number n2, const coeffs r)
{ return n_Sub(n1, n2, r); }

static FORCE_INLINE void n_InpMult_FieldGeneral(number &n1, number n2, const coeffs r)
{ n_InpMult(n1, n2, r); }

static FORCE_INLINE void n_InpAdd_FieldGeneral(number &n1, number n2, const coeffs r)
{ n_InpAdd(n1, n2, r); }

#ifdef HAVE_RINGS
#define n_Copy_RingGeneral(n, r)           n_Copy_FieldGeneral(n, r)
#define n_Delete_RingGeneral(n, r)         n_Delete_FieldGeneral(n, r)
#define n_Mult_RingGeneral(n1, n2, r)      n_Mult_FieldGeneral(n1, n2, r)
#define n_Add_RingGeneral(n1, n2, r)       n_Add_FieldGeneral(n1, n2, r)
#define n_IsZero_RingGeneral(n, r)         n_IsZero_FieldGeneral(n, r)
#define n_Equal_RingGeneral(n1, n2, r)     n_Equal_FieldGeneral(n1, n2, r)
#define n_Neg_RingGeneral(n, r)            n_Neg_FieldGeneral(n, r)
#define n_Sub_RingGeneral(n1, n2, r)       n_Sub_FieldGeneral(n1, n2, r)
#define n_InpMult_RingGeneral(n1, n2, r)   n_InpMult_FieldGeneral(n1, n2, r)
#define n_InpAdd_RingGeneral(n1,n2,r)      n_InpAdd(n1, n2, r)
#endif

#include "coeffs/modulop.h"
#include "coeffs/modulop_inl.h"

#define n_Copy_FieldZp(n, r)        n
#define n_Delete_FieldZp(n, r)      do {} while (0)
#define n_Mult_FieldZp(n1,n2,r)     npMultM(n1,n2,r)
#define n_Add_FieldZp(n1,n2,r)      npAddM(n1,n2,r)
#define n_Sub_FieldZp(n1,n2,r)      npSubM(n1,n2,r)
#define n_IsZero_FieldZp(n,r)       npIsZeroM(n,r)
#define n_Equal_FieldZp(n1,n2,r)    npEqualM(n1,n2,r)
#define n_Neg_FieldZp(n,r)          npNegM(n,r)
#define n_InpMult_FieldZp(n1,n2,r)  npInpMultM(n1,n2,r)
#define n_InpAdd_FieldZp(n1,n2,r)   npInpAddM(n1,n2,r)

#define DO_LFORCE_INLINE
#include "coeffs/longrat.cc" // for inlining... TODO: fix this Ugliness?!!!

#define n_Copy_FieldQ(n,r)          nlCopy(n,r)
#define n_Delete_FieldQ(n,r)        nlDelete(n,r)
#define n_Mult_FieldQ(n1,n2,r)      nlMult(n1,n2,r)
#define n_Add_FieldQ(n1,n2,r)       nlAdd(n1,n2,r)
#define n_Sub_FieldQ(n1,n2,r)       nlSub(n1,n2,r)
#define n_IsZero_FieldQ(n,r)        nlIsZero(n,r)
#define n_Equal_FieldQ(n1,n2,r)     nlEqual(n1,n2,r)
#define n_Neg_FieldQ(n,r)           nlNeg(n,r)
#define n_InpMult_FieldQ(n1,n2,r)   nlInpMult(n1,n2,r)
#define n_InpAdd_FieldQ(n1,n2,r)    nlInpAdd(n1,n2,r)

#endif
