/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_GmpBigInt.h
 *
 *  The bigint routines of gmp
 *
 ****************************************************************/

#ifndef _MP_GmpBigInt_h
#define _MP_GmpBigInt_h

#include "MP.h"

#ifdef MP_HAVE_GMP_APINT

#include "mp_gmp.h"

EXTERN MP_BigIntOps_t imp_gmp_bigint_ops;

EXTERN MP_Status_t IMP_GetGmpInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t* mp_apint));
EXTERN MP_Status_t IMP_PutGmpInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t mp_apint));
EXTERN char *IMP_GmpIntToStr _ANSI_ARGS_((MP_ApInt_t mp_apint,
                                          char *buffer));
EXTERN long IMP_GmpIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_apint));

#endif /* MP_HAVE_GMP_APINT */ 

#endif /* _MP_GmpBigInt_h */
