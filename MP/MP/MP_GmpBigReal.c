/*******************************************************************
 *   IMPLEMENTATION FILE:  MP_GmpBigNum.c
 *
 *      The default BigInt and BigReal package is the Gnu Multiple  
 *      Precision package (see root directory for MP for licensing 
 *      details).  Here we have the routines to put/get/print a GMP  
 *      big float.
 *
 *  Change Log:
 *      3/97 obachman Life begins for this file.
 ************************************************************************/

#include "MP.h"

#ifdef MP_HAVE_GMP_APREAL

#ifdef  __GNU_MP_VERSION
#if  __GNU_MP_VERSION > 2
#define _mp_allocate_func      __gmp_allocate_func
#define _mp_reallocate_func    __gmp_reallocate_func
#define _mp_free_func          __gmp_free_func
#endif
#endif

EXTERN void* (*_mp_allocate_func) (size_t size);
EXTERN void (*_mp_free_func) (void* ptr, size_t size);

MP_BigRealOps_t imp_gmp_bigreal_ops = {
    IMP_PutGmpReal,
    IMP_GetGmpReal,
    IMP_GmpRealToStr,
    IMP_GmpRealAsciiSize
};

#if MP_DEFAULT_APREAL_FORMAT == MP_GMP
MP_BigRealOps_t imp_default_bigreal_ops =
{
    IMP_PutGmpReal,
    IMP_GetGmpReal,
    IMP_GmpRealToStr,
    IMP_GmpRealAsciiSize
};
MP_BigNumFormat_t imp_default_bigreal_format = MP_GMP;
#endif


#ifdef __STDC__
MP_Status_t IMP_PutGmpReal(MP_Link_pt link, MP_ApReal_t mp_apreal)
#else
MP_Status_t IMP_PutGmpReal(link, apreal)
    MP_Link_pt link;
     MP_ApReal_t  apreal;
#endif
{
    mpf_ptr apreal = (mpf_ptr) mp_apreal;

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_PutGmpReal: entering\n");
    fflush(stderr);
#endif

    if (!IMP_PutLong(link, (long *)&(apreal->_mp_prec)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (!IMP_PutLong(link, (long *)&(apreal->_mp_size)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (!IMP_PutLong(link, (long *)&(apreal->_mp_exp)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (apreal->_mp_size != 0)
      return IMP_PutUint32Vector(link, apreal->_mp_d,
                                 (apreal->_mp_size < 0 ?
                                  - apreal->_mp_size : apreal->_mp_size));
    else
      return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetGmpReal(MP_Link_pt link, MP_ApReal_t *mp_apreal)
#else
MP_Status_t IMP_GetGmpReal(link, apreal)
    MP_Link_pt link;
    MP_ApReal_t *mp_apreal;
#endif
{
    long int  n;
    mpf_ptr apreal;
    
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetGmpReal: entering\n");
    fflush(stderr);
#endif
    if (*mp_apreal == NULL)
    {
      *mp_apreal = (MP_ApReal_t) IMP_MemAllocFnc(sizeof(*apreal));
      apreal = (mpf_ptr) *mp_apreal;
      mpf_init(apreal);
    }
    else
    {
      apreal = (mpf_ptr) *mp_apreal;
      mpf_clear(apreal);  /* start with a clean structure */
    }

    if (!IMP_GetLong(link, (long *)&(apreal->_mp_prec)))
        return MP_SetError(link, MP_CantGetDataPacket);

    if (!IMP_GetLong(link, (long *)&(apreal->_mp_size)))
        return MP_SetError(link, MP_CantGetDataPacket);

    if (!IMP_GetLong(link, &(apreal->_mp_exp)))
        return MP_SetError(link, MP_CantGetDataPacket);

    n = (apreal->_mp_size < 0 ? (- apreal->_mp_size) : apreal->_mp_size);

    apreal->_mp_d = (mp_ptr)(*_mp_allocate_func)((apreal->_mp_prec + 1) *
                                                 (mp_bits_per_limb/8));
    if (n > 0)
      return IMP_GetUint32Vector(link, &(apreal->_mp_d), n);
    else
      return MP_ClearError(link);
}

#ifdef __STDC__
char * IMP_GmpRealToStr(MP_ApReal_t mp_apreal, char *buffer)
#else
char * IMP_GmpRealToStr(link, apreal, buffer)
    MP_Link_pt link;
    MP_ApReal_t mp_apreal;
    char *buffer;
#endif
{
  mpf_ptr apreal = (mpf_ptr) mp_apreal;
  char     *str = NULL;
  long     len = 0;
  mp_exp_t exp;

#ifdef MP_DEBUG
  fprintf(stderr, "IMP_GmpRealToStr: entering\n");
  fflush(stderr);
#endif

  str = mpf_get_str(NULL, &exp, 10, 0, apreal);
  /* 3.14 is printed as 314 with exp == 1; Hence we need to add one to
     exp for 0.314 */
  sprintf(buffer, "0.%s@%ld", str, exp + 1);
  _mp_free_func(str, strlen(str) + 1);

  return buffer;
}

long IMP_GmpRealAsciiSize(MP_ApReal_t mp_apreal)
{
  mpf_ptr apreal = (mpf_ptr) mp_apreal;
  char     *str = NULL;
  mp_exp_t exp;
  long len;
  
  /* not the most efficient way to find out the length -- but the only
     possible one, provided we use n_digit == 0 */
  str = mpf_get_str(NULL, &exp, 10, 0, apreal);
  len = strlen(str);
  _mp_free_func(str, len + 1);

  /*
   * the 15 is for the "0.", the "@" and the exponent that will
   * appear in the final result.  This allows 12 digits for the
   * exponent - hope this is enough.
   */

  return len + 15;

}


#endif /* MP_HAVE_GMP_APREAL */
