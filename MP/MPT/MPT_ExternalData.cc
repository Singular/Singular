/******************************************************************
 *
 * File:    MPT_ExternalData.c
 * Purpose: Routines which deal with the handling of MPT_Data of the
 * type ExternalData 
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#include "MPT.h"

/*************************************************************** 
 * 
 * The following routines can (should) be redefined by an application
 * and then dynamically be assigned to the "global" pointer to the
 * respective functions.
 * 
 ***************************************************************/

/* Delete External Data */
void MPT_DefaultDeleteExternalData(MPT_Arg_t edata)
{
  return;
}

/* Copy It */
void MPT_DefaultCpyExternalData(MPT_Arg_t *dest,
                                 MPT_Arg_t src)
{
  *dest = (MPT_Arg_t) NULL;
  return;
}

/* Do not find anything, i.e. always return MPT_NotExternalData */
MPT_Status_t MPT_DefaultGetExternalData(MP_Link_pt link,
                                        MPT_Arg_t *odata,
                                        MPT_Node_pt   node)
{
  return MPT_NotExternalData;
}

MPT_Status_t MPT_DefaultPutExternalData(MP_Link_pt link,
                                        MPT_Arg_t data)
{
  return MPT_Success;
}

/* Set the function pointer to the default values */
void (*MPT_DeleteExternalData)(MPT_Arg_t edata)
  = MPT_DefaultDeleteExternalData;

void (*MPT_CpyExternalData)(MPT_Arg_t *src, MPT_Arg_t dest)
  = MPT_DefaultCpyExternalData;

MPT_Status_t (*MPT_GetExternalData)(MP_Link_pt link,
                                    MPT_Arg_t *data,
                                    MPT_Node_pt node)
  = MPT_DefaultGetExternalData;

MPT_Status_t (*MPT_PutExternalData)(MP_Link_pt link,
                                    MPT_Arg_t data)
  = MPT_DefaultPutExternalData;


