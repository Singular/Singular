/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Static.h
 *  Purpose: Configuration for static p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id: p_Procs_Static.h,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/

#ifndef P_PROCS_STATIC_H
#define P_PROCS_STATIC_H

// Set HAVE_FAST_P_PROCS to:
//   0 -- only FieldGeneral_LengthGeneral_OrdGeneral
//   1 -- plus FieldZp_Length*_OrdGeneral procs
//   2 -- plus FieldZp_Length*_Ord* procs
//   3 -- plus FieldQ_Length*_Ord*
//   4 -- plus FieldGeneral_Length*_OrdGeneral procs
//   5 -- all Field*_Length*_Ord* procs
#define HAVE_FAST_P_PROCS 3

// Set HAVE_FAST_FIELD to:
//   0 -- only FieldGeneral
//   1 -- special cases for FieldZp
//   2 -- plus special cases for FieldQ
//   nothing else is implemented, yet
#define HAVE_FAST_FIELD 2

// Set HAVE_FAST_LENGTH to:
//   0 -- only LengthGeneral
//   1 -- special cases for length <= 1
//   2 -- special cases for length <= 2
//   3 -- special cases for length <= 4
//   4 -- special cases for length <= 8
#define HAVE_FAST_LENGTH 3

// Set HAVE_FAST_ORD to:
//  0  -- only OrdGeneral
//  1  -- special for ords with n_min <= 1
//  2  -- special for ords with n_min <= 2
//  3  -- special ords for with n_min <= 3
//  4  -- special for all ords
#define HAVE_FAST_ORD 4

// Set HAVE_FAST_ZERO_ORD to:
//  0 -- no zero ords are considered
//  1 -- only ZeroOrds for OrdPosNomogPosZero, OrdNomogPosZero, OrdPomogNegZero
//  2 -- ZeroOrds for all
#define HAVE_FAST_ZERO_ORD 1

// on top of all this, see the StaticKernelFilter in p_Procs_Impl.h
#endif
