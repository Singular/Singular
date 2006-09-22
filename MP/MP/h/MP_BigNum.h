/****************************************************************
 *
 *   HEADER FILE:  MP_BigNum.h
 *
 * The relevant declarations for the handling of BigNums
 *
 ****************************************************************/

#ifndef _MP_BigNum_h
#define _MP_BigNum_h

#include "MP.h"

/* Note: a type_to_string conversion array using the ordering given */
/* below is defined in MP_Env.c.  So if you change the order below, */
/* make the corresponding change in MP_Env.c, okay?                 */

typedef MP_Uint8_t MP_BigNumFormat_t;

typedef struct MP_BigIntOps_t
{
    MP_Status_t  (*put_bigint)(MP_Link_pt, MP_ApInt_t);
    MP_Status_t  (*get_bigint)(MP_Link_pt link, MP_ApInt_t *bigint);
    char*        (*bigint_to_str)(MP_ApInt_t bigint, char *buffer);
    long         (*bigint_ascii_size)(MP_ApInt_t bigint);
} MP_BigIntOps_t;
typedef MP_BigIntOps_t * MP_BigIntOps_pt;

EXTERN MP_BigIntOps_t imp_default_bigint_ops;
EXTERN MP_BigNumFormat_t imp_default_bigint_format;

typedef struct   MP_BigRealOps_t
{
    MP_Status_t  (*put_bigreal)(MP_Link_pt link, MP_ApReal_t bigreal);
    MP_Status_t  (*get_bigreal)(MP_Link_pt link, MP_ApReal_t *bigreal);
    char*        (*bigreal_to_str)(MP_ApReal_t bigreal, char *buffer);
    long         (*bigreal_ascii_size)(MP_ApReal_t bigreal);
} MP_BigRealOps_t;
typedef MP_BigRealOps_t * MP_BigRealOps_pt;

EXTERN MP_BigRealOps_t imp_default_bigreal_ops;
EXTERN MP_BigNumFormat_t imp_default_bigreal_format;

typedef struct  MP_BigNum_t
{
    MP_BigNumFormat_t native_bigint_format;
    MP_BigIntOps_pt   bigint_ops;
    MP_BigNumFormat_t native_bigreal_format;
    MP_BigRealOps_pt  bigreal_ops;
} MP_BigNum_t;

EXTERN MP_Status_t IMP_PutDummyBigInt(MP_Link_pt link, MP_ApInt_t dummy);
EXTERN MP_Status_t IMP_GetDummyBigInt(MP_Link_pt link, MP_ApInt_t *dummy);
EXTERN char* IMP_DummyBigIntToStr(MP_ApInt_t dummy, char *buf);
EXTERN long IMP_DummyBigIntAsciiSize(MP_ApInt_t dummy);
EXTERN MP_Status_t IMP_PutDummyBigReal(MP_Link_pt link, MP_ApReal_t dummy);
EXTERN MP_Status_t IMP_GetDummyBigReal(MP_Link_pt link, MP_ApReal_t *dummy);
EXTERN char* IMP_DummyBigRealToStr(MP_ApReal_t dummy, char *buf);
EXTERN long IMP_DummyBigRealAsciiSize(MP_ApReal_t dummy);


#endif /* _MP_BigNum_h */
