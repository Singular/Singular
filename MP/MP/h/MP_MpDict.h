/***************************************************************************
 *
 *   HEADER FILE:  MP_MpDict.h
 *
 *      The MP dictionary for (definitions and declarations)
 *
 *
 *   Change Log:
 *        3/26/97  sgray - Life begins for this file.
 *
 ***************************************************************************/
#ifndef _MP_MpDict_h
#define _MP_MpDict_h

/* Let's give the MP Dictionary the number 2 */

#define MP_MpDict 2

/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopMpEndSession               1

#define MP_CopMpByteOrderRequest        10
#define MP_CopMpFpFormatRequest         11
#define MP_CopMpBigIntFormatRequest     12

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotMpLabel                 1
#define MP_AnnotMpRef                   2
#define MP_AnnotMpStore                 3
#define MP_AnnotMpStored                4
#define MP_AnnotMpRetrieve              5
#define MP_AnnotMpSource                6
#define MP_AnnotMpComment               7
#define MP_AnnotMpTiming                8
#define MP_AnnotMpType                  9
#define MP_AnnotMpUnits                10

#endif /* _MP_MpDict_h  */
