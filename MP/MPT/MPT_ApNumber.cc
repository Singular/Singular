/******************************************************************
 *
 * File:    MPT_ApNumber.c
 * Purpose: Routines which deal with the handling of abritrary
 *          precision numbers 
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#include "MPT.h"

MPT_Status_t MPT_GetApInt(MP_Link_pt link, MPT_Arg_t *arg)
{
  *arg = NULL;
  mp_return(IMP_GetApInt(link, (MP_ApInt_t *) arg));
}


MPT_Status_t MPT_PutApInt(MP_Link_pt link, MPT_Arg_t arg)
{
  mp_return(IMP_PutApInt(link, (MP_ApInt_t) arg));
}

MPT_Status_t MPT_GetApReal(MP_Link_pt link, MPT_Arg_t *apreal)
{
  *apreal = NULL;
  mp_return(IMP_GetApReal(link, (MP_ApReal_t *) apreal));
}

MPT_Status_t MPT_PutApReal(MP_Link_pt link, MPT_Arg_t arg)
{
  mp_return(IMP_PutApReal(link, (MP_ApReal_t) arg));
}


/******************************************************************
 *
 * Copy and Deletetion
 *     
 ******************************************************************/
#ifdef MP_HAVE_GMP_APINT 
static void MPT_InitCopyGmpApInt(MPT_Arg_t *dest, MPT_Arg_t src);
static void MPT_DeleteGmpApInt(MPT_Arg_t apint);
#endif
#ifdef MP_HAVE_PARI
static void MPT_InitCopyPariApInt(MPT_Arg_t *dest, MPT_Arg_t src);
static void MPT_DeletePariApInt(MPT_Arg_t apint);
#endif
static void MPT_InitCopyDummyApInt(MPT_Arg_t *dest, MPT_Arg_t src);
static void MPT_DeleteDummyApInt(MPT_Arg_t apint);

#if MP_DEFAULT_APINT_FORMAT == MP_PARI

#define MPT_DefaultApIntFormat MP_PARI
#define MPT_InitCopyDefaultApInt MPT_InitCopyPariApInt
#define MPT_DeleteDefaultApInt   MPT_DeletePariApInt
void (*MPT_InitCopyApInt)(MPT_Arg_t *dest, MPT_Arg_t src)
  =  MPT_InitCopyPariApInt;
void (*MPT_DeleteApInt)(MPT_Arg_t apint) 
  = MPT_DeletePariApInt;
static int MPT_ApIntFormat = MP_PARI;

#elif MP_DEFAULT_APINT_FORMAT == MP_GMP

#define MPT_InitCopyDefaultApInt MPT_InitCopyGmpApInt
#define MPT_DeleteDefaultApInt  MPT_DeleteGmpApInt
#define MPT_DefaultApIntFormat MP_GMP
void (*MPT_InitCopyApInt)(MPT_Arg_t *dest, MPT_Arg_t src)
  =  MPT_InitCopyGmpApInt;
void (*MPT_DeleteApInt)(MPT_Arg_t apint) 
  = MPT_DeleteGmpApInt;
static int MPT_ApIntFormat = MP_GMP;

#else

#define MPT_InitCopyDefaultApInt MPT_InitCopyDummyApInt
#define MPT_DeleteDefaultApInt  MPT_DeleteDummyApInt
#define MPT_DefaultApIntFormat MP_DUMMY
void (*MPT_InitCopyApInt)(MPT_Arg_t *dest, MPT_Arg_t src)
  =  MPT_InitCopyDummyApInt;
void (*MPT_DeleteApInt)(MPT_Arg_t apint) 
  = MPT_DeleteDummyApInt;
static int MPT_ApIntFormat = MP_DUMMY;

#endif

MPT_Status_t MPT_Init(MP_Env_pt env)
{
  if (env != NULL)
  {
    if (env->bignum.native_bigreal_format == MP_GMP)
    {
#ifdef MP_HAVE_GMP_APINT
      MPT_InitCopyApInt = MPT_InitCopyGmpApInt;
      MPT_DeleteApInt  = MPT_DeleteGmpApInt;
      MPT_ApIntFormat = MP_GMP;
      return MPT_ClearError();
#else 
      return MPT_SetError(MPT_WrongApIntFormat);
#endif
    }
    else if (env->bignum.native_bigreal_format == MP_PARI)
    {
#ifdef MP_HAVE_PARI
      MPT_InitCopyApInt = MPT_InitCopyPariApInt;
      MPT_DeleteApInt  = MPT_DeletePariApInt;
      MPT_ApIntFormat = MP_PARI;
      return MPT_ClearError();
#else 
       return MPT_SetError(MPT_WrongApIntFormat); 
#endif
    }
    else
    {
      if (env->bignum.native_bigreal_format == MP_DUMMY)
      {
        MPT_InitCopyApInt = MPT_InitCopyDummyApInt;
        MPT_DeleteApInt  = MPT_DeleteDummyApInt;
        MPT_ApIntFormat = MP_DUMMY;
        return MPT_ClearError();
      }
      else
        return MPT_SetError(MPT_WrongApIntFormat);
    }
  }
  
  MPT_InitCopyApInt = MPT_InitCopyDefaultApInt;
  MPT_DeleteApInt  = MPT_DeleteDefaultApInt;
  MPT_ApIntFormat = MPT_DefaultApIntFormat;
  return MPT_ClearError();
}

int MPT_GetApIntFormat()
{
  return MPT_ApIntFormat;
}

#ifdef MP_HAVE_GMP_APINT     
#include "mp_gmp.h"

static void MPT_DeleteGmpApInt(MPT_Arg_t arg)
{
  if (arg != NULL)
  {
    mpz_clear((mpz_ptr) arg);
    IMP_MemFreeFnc(arg, sizeof(*((mpz_ptr) arg)));
  }
}

static void MPT_InitCopyGmpApInt(MPT_Arg_t *odest, MPT_Arg_t src)
{
  if (src != NULL)
  {
    mpz_ptr dest = (mpz_ptr) IMP_MemAllocFnc(sizeof(*((mpz_ptr) src)));
    mpz_init_set(dest, (mpz_ptr) src);
    *odest = (MPT_Arg_t) dest;
  }
  else
    *odest = NULL;
}
#endif

#ifdef MP_HAVE_PARI
void MPT_DeletePariApInt(MPT_Arg_t arg)
{
  if (arg != NULL)
    IMP_FreeCgeti((GEN) arg);
}

void MPT_InitCopyPariApInt(MPT_Arg_t *odest, MPT_Arg_t src)
{
  int length;
  GEN number;
  
  if (src != NULL)
  {
    length = lgef((GEN) src);
    number = IMP_AllocCgeti(length);
    memcpy(number, (void*) src, length*sizeof(long));
    *odest = (MPT_Arg_t) number;
  }
  else
    *odest = NULL;
}
#endif

/* Dummy Ints */
void MPT_DeleteDummyApInt(MPT_Arg_t arg) {}
void MPT_InitCopyDummyApInt(MPT_Arg_t *odest, MPT_Arg_t src)
{
  *odest = NULL;
}


#ifdef MP_HAVE_GMP_APREAL
void MPT_InitCopyApReal(MPT_Arg_t *odest, MPT_Arg_t src)
{
  if (src != NULL)
  {
    mpf_ptr dest =  (mpf_ptr) IMP_MemAllocFnc(sizeof(*((mpf_ptr) src)));
    mpf_init_set(dest, (mpf_ptr) src);
    *odest = (MPT_Arg_t) dest;
  }
  else
    *odest = NULL;
}

void MPT_DeleteApReal(MPT_Arg_t arg)
{
  if (arg != NULL)
  {
    mpf_clear((mpf_ptr) arg);
    IMP_MemFreeFnc(arg, sizeof((mpf_ptr) arg));
  }
}
#else
/* Dummy Reals */
void MPT_DeleteApReal(MPT_Arg_t arg) {}
void MPT_InitCopyApReal(MPT_Arg_t *odest, MPT_Arg_t src)
{
  *odest = NULL;
}

#endif /* HAVE_GMP_APREAL */


