/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_GmpBigReal.h
 *
 *  The bigreal routines of gmp
 *
 ****************************************************************/

#ifndef _MP_GmpBigReal_h
#define _MP_GmpBigReal_h

#include "MP.h"

#ifdef MP_HAVE_GMP_APREAL
#include "mp_gmp.h"

EXTERN MP_BigRealOps_t imp_gmp_bigreal_ops;

EXTERN MP_Status_t IMP_GetGmpReal _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApReal_t* mp_apreal));
EXTERN MP_Status_t IMP_PutGmpReal _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApReal_t mp_apreal));
EXTERN char *IMP_GmpRealToStr _ANSI_ARGS_((MP_ApReal_t mp_apreal,
                                          char *buffer));
EXTERN long IMP_GmpRealAsciiSize _ANSI_ARGS_((MP_ApReal_t mp_apreal));

#endif /* MP_HAVE_GMP_APREAL */
#endif /* _MP_GmpBigReal_h */
