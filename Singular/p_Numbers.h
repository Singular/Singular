/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Numbers.h
 *  Purpose: macros/inline functions for number oerations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Numbers.h,v 1.2 2000-09-12 16:01:07 obachman Exp $
 *******************************************************************/
#ifndef P_NUMBERS_H
#define P_NUMBERS_H

#include "numbers.h"
#define p_nCopy_FieldGeneral(n, r)          nCopy(n)
#define p_nDelete_FieldGeneral(n, r)        nDelete(n)
#define p_nMult_FieldGeneral(n1, n2, r)     nMult(n1, n2)
#define p_nAdd_FieldGeneral(n1, n2, r)      nAdd(n1, n2)
#define p_nIsZero_FieldGeneral(n, r)	    nIsZero(n)
#define p_nEqual_FieldGeneral(n1, n2, r)    nEqual(n1, n2)
#define p_nNeg_FieldGeneral(n, r)	        nNeg(n)
#define p_nSub_FieldGeneral(n1, n2, r)      nSub(n1, n2)

#include "modulop.h"
#define p_nCopy_FieldZp(n, r)       n
#define p_nDelete_FieldZp(n, r)     ((void)0)
#define p_nMult_FieldZp(n1, n2, r)  npMultM(n1,n2)
#define p_nAdd_FieldZp(n1, n2, r)   npAddM(n1, n2)
#define p_nIsZero_FieldZp(n, r)	    npIsZeroM(n)
#define p_nEqual_FieldZp(n1, n2, r) npEqualM(n1, n2)
#define p_nNeg_FieldZp(n, r)	    npNegM(n)
#define p_nSub_FieldZp(n1, n2, r)   npSubM(n1, n2)

#endif

