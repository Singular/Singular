/*******************************************************************
 *   IMPLEMENTATION FILE:  MP_GmpBigNum.c
 *
 *      The default BigInt and BigReal package is the Gnu Multiple  
 *      Precision package (see root directory for MP for licensing 
 *      details).  Here we have the routines to put/get/print a GMP  
 *      bigint.
 *
 *  Change Log:
 *      4/19/96 sgray Life begins for this file.
 *      5/6/96  sgray Upgraded from GMP 1.3.2 to 2.0 and added support
 *                    for GMP big reals.
 *      5/7/96  sgray Okay, had to fix a few bugs that crept into the
 *                    code during the night.  I hate it when that happens.
 *      2/97    obachman Updated stuff to reflect the somewhat changed
 *                       BigNum philosphy of MP-1.1.2 and used Vector routines
 ************************************************************************/

#include "MP.h"

#ifdef MP_HAVE_GMP_APINT

MP_BigIntOps_t imp_gmp_bigint_ops = {
    IMP_PutGmpInt,
    IMP_GetGmpInt,
    IMP_GmpIntToStr,
    IMP_GmpIntAsciiSize
};

#if MP_DEFAULT_APINT_FORMAT == MP_GMP
MP_BigIntOps_t imp_default_bigint_ops =
{
    IMP_PutGmpInt,
    IMP_GetGmpInt,
    IMP_GmpIntToStr,
    IMP_GmpIntAsciiSize
};
MP_BigNumFormat_t imp_default_bigint_format = MP_GMP;
#endif


#ifdef __STDC__
MP_Status_t IMP_PutGmpInt(MP_Link_pt link, MP_ApInt_t mp_apint)
#else
MP_Status_t IMP_PutGmpInt(link, mp_apint)
    MP_Link_pt  link;
    MP_ApInt_t  mp_apint;
#endif
{
#ifdef MP_DEBUG
  fprintf(stderr, "IMP_PutGmpInt: entering\n"); fflush(stderr);
#endif
  mpz_ptr apint = (mpz_ptr) mp_apint;

  if (!IMP_PutLong(link, (long *)&(apint->_mp_size)))
    return MP_SetError(link, MP_CantPutDataPacket);

  if (apint->_mp_size != 0)
    return IMP_PutUint32Vector(link, apint->_mp_d,
                               (apint->_mp_size < 0 ?
                                (- apint->_mp_size) : apint->_mp_size));
  else
    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetGmpInt(MP_Link_pt link, MP_ApInt_t *mp_apint)
#else
MP_Status_t IMP_GetGmpInt(link, mp_apint)
    MP_Link_pt  link;
    MP_ApInt_t *mp_apint;
#endif
{
    long int size;
    mp_ptr   dp;
    mpz_ptr  apint;
    if (*mp_apint == NULL)
    {
      *mp_apint = (MP_ApInt_t) IMP_MemAllocFnc(sizeof(*apint));
      apint = (mpz_ptr) *mp_apint;
      mpz_init(apint);
    }
    else
    {
      apint = (mpz_ptr) *mp_apint;
    }
    
    if (!IMP_GetLong(link, &size))
        return MP_SetError(link, MP_CantGetDataPacket);

    apint->_mp_size = size;
    if (size < 0)
        size = -size;
    _mpz_realloc(apint, size);
    apint->_mp_alloc = size;

    dp = apint->_mp_d;
    if (size != 0)
      return IMP_GetUint32Vector(link, &(apint->_mp_d), size);
    else
      return MP_ClearError(link);
}

#ifdef __STDC__
char * IMP_GmpIntToStr(MP_ApInt_t mp_apint, char *buffer)
#else
char * IMP_GmpIntToStr(mp_apint, buffer)
    MP_ApInt_t mp_apint;
    char *buffer;
#endif
{
  mpz_get_str(buffer, 10, (mpz_ptr) mp_apint);
  return buffer;
}

#ifdef __STDC__
long IMP_GmpIntAsciiSize(MP_ApInt_t mp_apint)
#else
long IMP_GmpIntAsciiSize(apint)
    MP_ApInt_t mp_apint;
#endif
{
  return mpz_sizeinbase((mpz_ptr) mp_apint, 10) + 2;
}

#endif /* MP_HAVE_GMP_APINT */
