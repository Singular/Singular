/****************************************************************
 *                                                                  
 * HEADER FILE:  MP_PariBigInt.h
 * Authors:  O. Bachmann, T. Metzner, H. Schoenemann, A. Sorgatz
 * Date:    February 97
 *                                                                
 ***************************************************************/

#ifndef _MP_PariBigInt_h
#define _MP_PariBigInt_h

#include "MP.h"
#ifdef MP_HAVE_PARI

#include "genpari.h"
  
EXTERN MP_BigIntOps_t imp_pari_bigint_ops;

EXTERN MP_Status_t IMP_GetPariBigInt _ANSI_ARGS_((MP_Link_pt link,
                                                 MP_ApInt_t *pari_number));
EXTERN MP_Status_t IMP_PutPariBigInt _ANSI_ARGS_((MP_Link_pt link, 
                                                 MP_ApInt_t pari_number));
EXTERN char* IMP_PariBigIntToStr _ANSI_ARGS_((MP_ApInt_t pari_number,
                                              char *buffer));
EXTERN long IMP_PariBigIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_number));

#ifdef HAVE_GMP_PARI_CONVERSIONS
#include "gmp.h"
EXTERN mpz_ptr _pari_to_gmp(GEN pnum, mpz_ptr *gnum_ptr);
EXTERN GEN     _gmp_to_pari(mpz_ptr gnum);
#endif

#endif /* MP_HAVE_PARI */

#endif  /* _MP_PariBigInt_h */
