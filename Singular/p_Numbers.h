/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Numbers.h
 *  Purpose: macros/inline functions for number oerations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Numbers.h,v 1.3 2000-09-18 09:19:27 obachman Exp $
 *******************************************************************/
#ifndef P_NUMBERS_H
#define P_NUMBERS_H

#include "numbers.h"
#define n_Copy_FieldGeneral(n, r)           nCopy(n)
#define n_Delete_FieldGeneral(n, r)         nDelete(n)
#define n_Mult_FieldGeneral(n1, n2, r)      nMult(n1, n2)
#define n_Add_FieldGeneral(n1, n2, r)       nAdd(n1, n2)
#define n_IsZero_FieldGeneral(n, r)	        nIsZero(n)
#define n_Equal_FieldGeneral(n1, n2, r)     nEqual(n1, n2)
#define n_Neg_FieldGeneral(n, r)	        nNeg(n)
#define n_Sub_FieldGeneral(n1, n2, r)       nSub(n1, n2)

#include "modulop.h"
#define n_Copy_FieldZp(n, r)        n
#define n_Delete_FieldZp(n, r)      ((void)0)
#define n_Mult_FieldZp(n1, n2, r)   npMultM(n1,n2)
#define n_Add_FieldZp(n1, n2, r)    npAddM(n1, n2)
#define n_IsZero_FieldZp(n, r)	    npIsZeroM(n)
#define n_Equal_FieldZp(n1, n2, r)  npEqualM(n1, n2)
#define n_Neg_FieldZp(n, r)	        npNegM(n)
#define n_Sub_FieldZp(n1, n2, r)    npSubM(n1, n2)

#endif

