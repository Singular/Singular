/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Numbers.h
 *  Purpose: macros/inline functions for number oerations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Numbers.h,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/
#ifndef P_NUMBERS_H
#define P_NUMBERS_H

#include "numbers.h"
#define n_Copy_FieldGeneral(n, r)           (r)->cf->cfCopy(n,r)
#define n_Delete_FieldGeneral(n, r)         (r)->cf->cfDelete(n,r)
#define n_Mult_FieldGeneral(n1, n2, r)      (r)->cf->nMult(n1, n2)
#define n_Add_FieldGeneral(n1, n2, r)       (r)->cf->nAdd(n1, n2)
#define n_IsZero_FieldGeneral(n, r)         (r)->cf->nIsZero(n)
#define n_Equal_FieldGeneral(n1, n2, r)     (r)->cf->nEqual(n1, n2)
#define n_Neg_FieldGeneral(n, r)            (r)->cf->nNeg(n)
#define n_Sub_FieldGeneral(n1, n2, r)       (r)->cf->nSub(n1, n2)
//#define n_InpMult_FieldGeneral(n1, n2, r)   (r)->cf->nInpMult(n1, n2, r)
#define n_InpMult_FieldGeneral(n1, n2, r)   ndInpMult(n1, n2, r)

#include "modulop.h"
#define n_Copy_FieldZp(n, r)        n
#define n_Delete_FieldZp(n, r)      ((void)0)
#define n_Mult_FieldZp(n1, n2, r)   npMultM(n1,n2)
#define n_Add_FieldZp(n1, n2, r)    npAddM(n1, n2)
#define n_IsZero_FieldZp(n, r)      npIsZeroM(n)
#define n_Equal_FieldZp(n1, n2, r)  npEqualM(n1, n2)
#define n_Neg_FieldZp(n, r)         npNegM(n)
#define n_Sub_FieldZp(n1, n2, r)    npSubM(n1, n2)
#define n_InpMult_FieldZp(n1, n2, r) n1=npMultM(n1, n2)

#define DO_LINLINE
#include "longrat.cc"
#define n_Copy_FieldQ(n, r)        nlCopy(n)
#define n_Delete_FieldQ(n, r)      nlDelete(n,r)
#define n_Mult_FieldQ(n1, n2, r)   nlMult(n1,n2)
#define n_Add_FieldQ(n1, n2, r)    nlAdd(n1, n2)
#define n_IsZero_FieldQ(n, r)      nlIsZero(n)
#define n_Equal_FieldQ(n1, n2, r)  nlEqual(n1, n2)
#define n_Neg_FieldQ(n, r)         nlNeg(n)
#define n_Sub_FieldQ(n1, n2, r)    nlSub(n1, n2)
#define n_InpMult_FieldQ(n1, n2, r) nlInpMult(n1, n2, r)
#endif
