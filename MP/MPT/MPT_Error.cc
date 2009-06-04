/******************************************************************
 *
 * File:    MPT_Error.c
 * Purpose: Routines related to Error handling of MPT
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/

#include "MPT.h"

const char *MPT_errlist[] =
{
  "MPT: MPT_Failure",
  "MPT: MPT_Success",
  "MPT: MPT_MP_Failure",
  "MPT: Unknown Type",
  "MPT: Can't allocate memory",
  "MPT: MPT tree has wrong syntax/structure",
  "MPT: MP node of unknown type",
  "MPT: ProtoD:Pointer without Arg",
  "MPT: Union Discriminator too large (out of permissible range)",
  "MPT: An internal assumption has been violated",
  "MPT: External application did not read data from link",
  "MPT: Wrong ApInt format requested"
};


MPT_Status_t MPT_errno = MPT_Success;

MPT_Status_t MPT_CheckError()
{
  MPT_Status_t status = MPT_errno;
  MPT_ClearError();
  return status;
}

const char*  MPT_ErrorStr(MPT_Status_t error)
{
  if (error < MPT_MaxError && error > 0)
    return MPT_errlist[error];
  else
    return "Unknown MPT error number";
}

const char* MPT_LinkErrorStr(MP_Link_pt link, MPT_Status_t error)
{
  if (error == MPT_MP_Failure &&
      link != NULL && link->MP_errno >= 0 && link->MP_errno != MP_Success)
    return MP_ErrorStr(link);
  
  if (error >= 0 && error < MPT_MaxError)
    return MPT_errlist[error];
  
  return "MPT: Unknown Error";
}

#ifdef MPT_DEBUG
MPT_Status_t MPT_SetError(MPT_Status_t mpt_error)
{
  return MPT_errno = mpt_error;
}

MPT_Status_t MPT_ClearError()
{
  return MPT_errno = MPT_Success;
}

/* when debugging is turned on, this function gets called, whenever
   MPT_Assume is used */
void MPT_Assume(MP_Boolean_t ass)
{
  if (!ass)
  {
    MPT_SetError(MPT_ViolatedAssumption);
  }
}
#endif


