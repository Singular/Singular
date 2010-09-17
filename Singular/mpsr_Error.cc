/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_Error.cc
 * Purpose:    Error handling of mpsr routines
 * Author:     Olaf Bachmann (2/97)
 *
 * Change History (most recent first):
 *
 ***************************************************************/
#include <kernel/mod2.h>

#ifdef HAVE_MPSR

#include <kernel/febase.h>
#include <Singular/mpsr.h>

const char *mpsr_errlist[] =
{
  "mpsr_Failure",
  "mpsr_Success",
  "Low-level MP error",
  "MP (resp. MPT) syntax error",
  "Object of unknown type to put",
  "Object of wrong type to put",
  "Unknown Singular token to put",
  "Unknown dictionary item to get",
  "Unknown operator to get",
  "Unknown MP Node Type",
  "Prototype found where it can not be handled",
  "Wrong Number of Args",
  "Wrong Argument Type",
  "Wrong MP Node Type",
  "Required Annot Skip",
  "Wrong Union Discriminator",
  "Unknown Coefficient Domain Specifications",
};


static mpsr_Status_t mpsr_errno = mpsr_Success;
static MP_Errors mpsr_MP_errno = MP_Failure;

mpsr_Status_t mpsr_SetError(mpsr_Status_t error)
{
  mpsr_errno = error;
  return error;
}

mpsr_Status_t mpsr_SetError(MP_Link_pt link)
{
  mpsr_SetError(mpsr_MP_Failure);
  mpsr_MP_errno = (enum MP_Errors) link->MP_errno;
  return mpsr_MP_Failure;
}

void mpsr_PrintError(mpsr_Status_t error, MP_Link_pt link)
{
  if (error != mpsr_Success)
  {
    if (error == mpsr_MP_Failure)
      Werror("%s : %s", mpsr_errlist[error],
             (link != NULL ? MP_ErrorStr(link) :
              (mpsr_MP_errno < MP_MaxError ? MP_errlist[mpsr_MP_errno] :
               "Unknown MP error")));
    else if (error == mpsr_MPT_Failure)
      Werror("%s : %s", mpsr_errlist[error],
             ((MPT_errno == MPT_MP_Failure && link != NULL) ?
              MP_ErrorStr(link) : MPT_ErrorStr(MPT_errno)));
    else Werror("MP<->Singular interface error : %s",
                (mpsr_errno < mpsr_MaxError ? mpsr_errlist[mpsr_errno] :
                 "Unknown mpsr error"));
  }

}
void mpsr_PrintError(MP_Link_pt link)
{
  mpsr_PrintError(mpsr_errno, link);
}

void mpsr_PrintError(mpsr_Status_t error)
{
  mpsr_PrintError(error, NULL);
}


void mpsr_PrintError()
{
  mpsr_PrintError(mpsr_errno, NULL);
}

mpsr_Status_t mpsr_GetError()
{
  return mpsr_errno;
}

void mpsr_ClearError()
{
  mpsr_errno = mpsr_Success;
}
#endif
