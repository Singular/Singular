/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_SacBigInt.h
 *                                                                
 ***************************************************************/

#ifndef _MP_SacBigInt_h
#define _MP_SacBigInt_h

#include "MP.h"

#ifdef MP_HAVE_SAC
#include "saclib.h"
#include "mp_gmp.h"

EXTERN MP_BigIntOps_t sac_ops;

EXTERN MP_Status_t IMP_PutSacBigInt _ANSI_ARGS_((MP_Link_pt link, 
                                                 MP_ApInt_t sac_int));

EXTERN MP_Status_t IMP_GetSacBigInt _ANSI_ARGS_((MP_Link_pt link,
                                                 MP_ApInt_t *sac_int));

EXTERN char * IMP_SacBigIntToStr _ANSI_ARGS_((MP_ApInt_t big_int, 
                                             char *buffer));

EXTERN long IMP_SacBigIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_apint));

#endif /* MP_HAVE_SAC */

#endif  /* _MP_SacBigInt_h */
