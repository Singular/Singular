/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Numbers.h
 *  Purpose: macros/inline functions for number operations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef P_NUMBERS_H
#define P_NUMBERS_H

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>
#include <polys/monomials/ring.h>

static inline number n_Copy_FieldGeneral(number n,    const ring r)
{ return r->cf->cfCopy(n,r->cf); }

static inline void   n_Delete_FieldGeneral(number* p, const ring r)
{ r->cf->cfDelete(p,r->cf); }

static inline number n_Mult_FieldGeneral(number n1, number n2, const ring r)
{ return r->cf->cfMult(n1, n2, r->cf); }

static inline number n_Add_FieldGeneral(number n1, number n2, const ring r)
{ return r->cf->cfAdd(n1, n2, r->cf); }

static inline BOOLEAN n_IsZero_FieldGeneral(number n, const ring r)
{ return r->cf->cfIsZero(n, r->cf); }

static inline BOOLEAN n_Equal_FieldGeneral(number n1, number n2, const ring r)
{ return r->cf->cfEqual(n1, n2, r->cf); }

static inline number n_Neg_FieldGeneral(number n,     const ring r)
{ return r->cf->cfNeg(n, r->cf); }

static inline number n_Sub_FieldGeneral(number n1, number n2, const ring r)
{ return r->cf->cfSub(n1, n2, r->cf); }

static inline void n_InpMult_FieldGeneral(number &n1, number n2, const ring r)
{ ndInpMult(n1, n2, r->cf); }

static inline void n_InpAdd_FieldGeneral(number &n1, number n2, const ring r)
{ ndInpAdd(n1, n2, r->cf); }

#ifdef HAVE_RINGS
#define n_Copy_RingGeneral(n, r)           r->cf->cfCopy(n,r->cf)
#define n_Delete_RingGeneral(n, r)         r->cf->cfDelete(n,r->cf)
#define n_Mult_RingGeneral(n1, n2, r)      r->cf->cfMult(n1, n2, r->cf)
#define n_Add_RingGeneral(n1, n2, r)       r->cf->cfAdd(n1, n2, r->cf)
#define n_IsZero_RingGeneral(n, r)         r->cf->cfIsZero(n, r->cf)
#define n_Equal_RingGeneral(n1, n2, r)     r->cf->cfEqual(n1, n2, r->cf)
#define n_Neg_RingGeneral(n, r)            r->cf->cfNeg(n, r->cf)
#define n_Sub_RingGeneral(n1, n2, r)       r->cf->cfSub(n1, n2, r->cf)
//#define n_InpMult_RingGeneral(n1, n2, r)   r->cf->nInpMult(n1, n2, r->cf)
#define n_InpMult_RingGeneral(n1, n2, r)   ndInpMult(n1, n2, r->cf)
#define n_InpAdd_RingGeneral(n1, n2, r)    ndInpAdd(n1, n2, r->cf)
#endif

#include <coeffs/modulop.h>
#define n_Copy_FieldZp(n, r)        n
#define n_Delete_FieldZp(n, r)      ((void)0)
#define n_Mult_FieldZp(n1, n2, r)   npMultM(n1,n2, r->cf)
#define n_Add_FieldZp(n1, n2, r)    npAddM(n1, n2, r->cf)
#define n_IsZero_FieldZp(n, r)      npIsZeroM(n, r->cf)
#define n_Equal_FieldZp(n1, n2, r)  npEqualM(n1, n2, r->cf)
#define n_Neg_FieldZp(n, r)         npNegM(n, r->cf)
#define n_Sub_FieldZp(n1, n2, r)    npSubM(n1, n2, r->cf)
#define n_InpMult_FieldZp(n1, n2, r) n1=npMultM(n1, n2, r->cf)

static inline void n_InpAdd_FieldZp(number &n1, number n2, const ring r)
{ n1=npAddM(n1, n2, r->cf); }

#define DO_LINLINE
#include "coeffs/longrat.cc"
#define n_Copy_FieldQ(n, r)        nlCopy(n, r->cf)
#define n_Delete_FieldQ(n, r)      nlDelete(n,r->cf)
#define n_Mult_FieldQ(n1, n2, r)   nlMult(n1,n2, r->cf)
#define n_Add_FieldQ(n1, n2, r)    nlAdd(n1, n2, r->cf)
#define n_IsZero_FieldQ(n, r)      nlIsZero(n, r->cf)
#define n_Equal_FieldQ(n1, n2, r)  nlEqual(n1, n2, r->cf)
#define n_Neg_FieldQ(n, r)         nlNeg(n, r->cf)
#define n_Sub_FieldQ(n1, n2, r)    nlSub(n1, n2, r->cf)
#define n_InpMult_FieldQ(n1, n2, r) nlInpMult(n1, n2, r->cf)
#define n_InpAdd_FieldQ(n1, n2, r)  nlInpAdd(n1, n2, r->cf)
#endif
