/****************************************************************
 *                                                                  
 * FILE:     MP_PariBigInt.c
 * Authors:  O. Bachmann, T. Metzner, H. Schoenemann, A. Sorgatz
 * Date:     February 97
 *                                                                
 ***************************************************************/


#include "MP.h"

#ifdef MP_HAVE_PARI
/* used to dynamically set the bigint ops to the ones defined here */
MP_BigIntOps_t imp_pari_bigint_ops = {
  IMP_PutPariBigInt,
  IMP_GetPariBigInt,
  IMP_PariBigIntToStr,
  IMP_PariBigIntAsciiSize
};

#if MP_DEFAULT_APINT_FORMAT == MP_PARI
/* used to statically (i.e. at compile time) set the bigint ops -- if
   set here, then these are the one an MP_Env is created with */
MP_BigIntOps_t imp_default_bigint_ops =
{
  IMP_PutPariBigInt,
  IMP_GetPariBigInt,
  IMP_PariBigIntToStr,
  IMP_PariBigIntAsciiSize
};
MP_BigNumFormat_t imp_default_bigint_format = MP_PARI;
#endif

/**************************************************************************
 *
 * Memory management routines 
 *
 *************************************************************************/

/* By default, simply use cgeti, on alloc */
GEN IMP_DefaultAllocCgeti(long l)
{
  return cgeti(l);
}
/* and, do nothing on free */
void IMP_DefaultFreeCgeti(GEN number)
{}
/* However, you might also use IMP_RawMemAlloc */
GEN IMP_RawMemAllocCgeti(long length)
{
  GEN z = (GEN) IMP_RawMemAllocFnc( ((ulong)length)<<TWOPOTBYTES_IN_LONG );
  z[0]=evaltyp(1)+evalpere(1)+evallg(length);
  return( z );
}
void IMP_RawMemFreeCgeti(GEN number)
{
  IMP_RawMemFreeFnc(number);
}

GEN (*IMP_AllocCgeti)(long) = IMP_DefaultAllocCgeti;
void (*IMP_FreeCgeti)(GEN) = IMP_DefaultFreeCgeti;

/**************************************************************************
 *
 * Put/Get
 *
 *************************************************************************/

MP_Status_t IMP_PutPariBigInt(MP_Link_pt link, MP_ApInt_t mp_number) 
{
  GEN number = (GEN) mp_number;
  long length = lgef(number) - 2; /* in "limbs" */
  long sign = signe(number); 

  /* Put the length and sign */
  ERR_CHK(IMP_PutSint32(link, (sign < 0 ? -length : length)));
 
  if (link->link_bigint_format == MP_PARI)
  {
    /* for pari - bigints can use the vector put */
    ERR_CHK(IMP_PutUint32Vector(link, (MP_Uint32_t *) &number[2], length));
  }
  else
  {
    /* we put the numbers in the gmp format */
    /* which is very similar to the pari format, except that in pari,
       limbs are in descending order, and not in ascending, as in gmp */
    GEN ptr = number + length + 1;
    number++;

    for (;ptr > number; ptr--)
      ERR_CHK(IMP_PutLong(link, ptr));
  }

  /* done */
  return MP_ClearError(link);
}

MP_Status_t IMP_GetPariBigInt(MP_Link_pt link, MP_ApInt_t *mp_number)
{
  long length, sign = 1;
  GEN number, ptr;

  /* first, we get the effective length and the sign */
  ERR_CHK(IMP_GetLong(link, &length));

  if (length < 0)
  {
    sign = -1;
    length = -length;
  }
  else if (length == 0)
  {
    sign = 0;
  }

  /* Initialize the number */
  number = IMP_AllocCgeti(length+2);
  setlgef(number, length+2);
  setsigne(number, sign);

  /* Get the actual data */
  if (length > 0)
  {
    if (link->link_bigint_format == MP_PARI)
    {
      ptr = &(number[2]);
      ERR_CHK(IMP_GetUint32Vector(link, (MP_Uint32_t **) &ptr, length));
    }
    else
    {
      number++;
      ptr = number + length;
      for (; ptr > number; ptr--)
        ERR_CHK(IMP_GetLong(link, ptr));
      number--;
    }
  }

  *mp_number = (MP_ApInt_t) number;
  return MP_ClearError(link);
}
     
long IMP_PariBigIntAsciiSize(MP_ApInt_t mp_number)
{
  return gsize((GEN) mp_number) + 2;
}

char * IMP_PariBigIntToStr(MP_ApInt_t mp_number, char *buffer)
{
//  return gitoascii((GEN) mp_number, buffer);
  return "Can not print";
}

/* We do not really need any conversion functions. However, let's
include them for testing purposes */

#ifdef HAVE_GMP_PARI_CONVERSIONS

mpz_ptr _pari_to_gmp(GEN pnum, mpz_ptr *gnum_ptr)
{
  mpz_ptr gnum;
  long length = lgef(pnum) - 2;
  long sign = signe(pnum);
  GEN pptr;
  mp_limb_t *gptr;
  
  if (*gnum_ptr == NULL)
  {
    *gnum_ptr = (mpz_ptr) IMP_MemAllocFnc(sizeof(__mpz_struct));
    gnum = *gnum_ptr;
    mpz_init(gnum);
  }
  else
    gnum = *gnum_ptr;

  gnum->_mp_size = (sign < 0 ? -length : length);

  _mpz_realloc(gnum, length);
  gnum->_mp_alloc = length;

  gptr = gnum->_mp_d;
  pnum++;
  pptr = pnum + length;
  
  for (; pptr > pnum; gptr++, pptr--)
    *gptr = *pptr;

  return gnum;
}

GEN _gmp_to_pari(mpz_ptr gnum)
{
  long length = gnum->_mp_size;
  long sign = (length < 0 ? -1 : (length == 0 ? 0 : 1));
  GEN pnum, pptr;
  mp_limb_t *gptr;

  if (length < 0 ) length = -length;

  pnum = IMP_AllocCgeti(length + 2);
  setlgef(pnum, length + 2);
  setsigne(pnum, sign);

  pnum++;
  pptr = pnum + length;
  gptr = gnum->_mp_d;

  for (; pptr > pnum; gptr++, pptr--)
    *pptr = *gptr;

  pnum--;

  return pnum;
}

#endif

  
  
#endif /* MP_HAVE_PARI */
