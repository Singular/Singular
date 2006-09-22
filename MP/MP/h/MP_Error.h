/***************************************************************************
 *
 *   HEADER FILE:  MP_Error.h
 *
 *        All the error encodings and the routines the set clear
 *        and print errors.
 *
 *  Change Log:
 *       10/18/95 OB      - put MP_Failure, MP_Success into enum MP_Errors
 *       July 7, 1995  SG - updated MP_Errors to reflect newtypes
 *                          in MP-1.0 and coelesced a few error types.
 *                          Also made the necessary changes to
 *                          MP_Error.h, MP_Put.c, MP_Get.c
 *       July 21, 1995 SG - more additions/corrections.  Checked against
 *                          MP_Error.c for accuracy (enums with corresponding
 *                          error messages)
 *
 *       5/24/96 sgray - deleted a bunch of obsolete error codes (consolidated)
 *
 ***************************************************************************/

#ifndef _MP_Error_h
#define _MP_Error_h

#include "MP.h"
/*
 *  for returning error status from most routines
 */

enum MP_Errors { MP_Failure,
                 MP_Success,
                 MP_UnknownType,
                 MP_UnknownAnnotation,
                 MP_UnknownRequiredAnnotation,
                 MP_NullLink,
                 MP_NullTransport,
                 MP_CantOpenFile,
                 MP_CantConnect,
                 MP_MemAlloc,
                 MP_CantConvert,
                 MP_ValueOutOfRange,
                 MP_CantPutNodeHeader,
                 MP_CantPutNodePacket,
                 MP_CantPutDataPacket,
                 MP_CantPutAnnotationPacket,
                 MP_CantGetNodeHeader,
                 MP_CantGetNodePacket,
                 MP_CantGetDataPacket,
                 MP_CantGetAnnotationPacket,
                 MP_NotRequestedPacketType,
                 MP_CantConvertBadString,
                 MP_CantWriteLink,
                 MP_CantReadLink,
                 MP_NotCommonIdentifier,
		         MP_WrongBasicVectorType,
                 MP_DummyBigNum,
                 MP_CantSkipMsg,
                 MP_CantInitMsg,
                 MP_ExternalError,
                 MP_MaxError };

typedef enum MP_Errors  MP_Status_t;

EXTERN char *MP_errlist[];


#define CHK_ALLOC_MEM(link, ptr, len, ptr_type) {                \
  if (ptr == NULL)                                               \
    if ((ptr = IMP_MemAllocFnc(len * sizeof(ptr_type))) == NULL) \
      return MP_SetError(link, MP_MemAlloc);                     \
}

#define TEST_MEM_ALLOC_ERROR(link, ptr) {    \
   if (ptr == NULL)                          \
      return MP_SetError(link, MP_MemAlloc); \
}

#define RETURN_OK(link)       \
   return MP_ClearError(link)

#define ERR_CHK(fnc_status) {    \
   if (fnc_status != MP_Success) \
      return MP_Failure;         \
}

#endif  /* _MP_Error_h */
