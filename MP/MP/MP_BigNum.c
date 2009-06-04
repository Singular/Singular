/****************************************************************
 *                                                                  
 * FILE:     MP_BigNum.c
 * Purpose:  Dummy definition of the default BigNum routines
 *
 *  Change Log:
 *      2/97 obachman Life begins for this file.
 * 
 ***************************************************************/

#include "MP.h"
#include <string.h>

MP_BigIntOps_t imp_dummy_bigint_ops =
{
  IMP_PutDummyBigInt,
  IMP_GetDummyBigInt,
  IMP_DummyBigIntToStr,
  IMP_DummyBigIntAsciiSize
};

#if MP_DEFAULT_APINT_FORMAT == MP_DUMMY
MP_BigIntOps_t imp_default_bigint_ops =
{
  IMP_PutDummyBigInt,
  IMP_GetDummyBigInt,
  IMP_DummyBigIntToStr,
  IMP_DummyBigIntAsciiSize
};

MP_BigNumFormat_t imp_default_bigint_format = MP_DUMMY;
#endif

MP_BigRealOps_t imp_dummy_bigreal_ops =
{
  IMP_PutDummyBigReal,
  IMP_GetDummyBigReal,
  IMP_DummyBigRealToStr,
  IMP_DummyBigRealAsciiSize
};

#if MP_DEFAULT_APREAL_FORMAT == MP_DUMMY
MP_BigRealOps_t imp_default_bigreal_ops =
{
  IMP_PutDummyBigReal,
  IMP_GetDummyBigReal,
  IMP_DummyBigRealToStr,
  IMP_DummyBigRealAsciiSize
};
MP_BigNumFormat_t imp_default_bigreal_format = MP_DUMMY;
#endif

MP_Status_t IMP_PutDummyBigInt(MP_Link_pt link, MP_ApInt_t dummy)
{
  return MP_SetError(link, MP_DummyBigNum);
}

MP_Status_t IMP_GetDummyBigInt(MP_Link_pt link, MP_ApInt_t *dummy)
{
  return MP_SetError(link, MP_DummyBigNum);
}

#define DUMMY_BIGNUM_STR "MP_DummyBigNum"
char* IMP_DummyBigIntToStr(MP_ApInt_t dummy, char *buf)
{
  strcpy(buf, DUMMY_BIGNUM_STR);
  return buf;
}

long IMP_DummyBigIntAsciiSize(MP_ApInt_t dummy)
{
  return strlen(DUMMY_BIGNUM_STR) + 1;
}

MP_Status_t IMP_PutDummyBigReal(MP_Link_pt link, MP_ApReal_t dummy)
{
  return MP_SetError(link, MP_DummyBigNum);
}

MP_Status_t IMP_GetDummyBigReal(MP_Link_pt link, MP_ApReal_t *dummy)
{
  return MP_SetError(link, MP_DummyBigNum);
}

#define DUMMY_BIGNUM_STR "MP_DummyBigNum"
char* IMP_DummyBigRealToStr(MP_ApReal_t dummy, char *buf)
{
  strcpy(buf, "MP_DummyBigNum");
  return buf;
}

long IMP_DummyBigRealAsciiSize(MP_ApReal_t dummy)
{
  return strlen(DUMMY_BIGNUM_STR) + 1;
}

