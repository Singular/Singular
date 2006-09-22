/****************************************************************
 *
 *   HEADER FILE:  MP.h
 *
 *        Everything we need.  Saves doing a zillion includes
 *        in user programs.
 *
 ***************************************************************/

#ifndef _MP_h
#define _MP_h

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */

#undef _ANSI_ARGS_
#undef CONST
#if ((defined(__STDC__) || defined(SABER))  && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#   define _ANSIC_
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#   define CONST const
#   ifdef __cplusplus
#       define VARARGS (...)
#   else
#       define VARARGS ()
#   endif
#else
#   define _ANSI_ARGS_(x)      ()
#   define CONST
#endif

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif


/* standard includes */

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* MP Config include */
#include "MP_Config.h"

/* need select.h where available */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif


/* MP includes */

/*MAKEHEADER*/
#include "MP_Types.h"
/*MAKEHEADER*/
#include "MP_Annotations.h"
/*MAKEHEADER*/
#include "MP_Error.h"
/*MAKEHEADER*/
#include "MP_BuffTypes.h"
/*MAKEHEADER*/
#include "MP_Memory.h"
/*MAKEHEADER*/
#include "MP_BigNum.h"
/*MAKEHEADER*/
#include "MP_Env.h"
/*MAKEHEADER*/
#include "MP_Link.h"
/*MAKEHEADER*/
#include "MP_Buffer.h"
/*MAKEHEADER*/
#include "MP_Util.h"
/*MAKEHEADER*/
#include "MP_Put.h"
/*MAKEHEADER*/
#include "MP_Get.h"
/*MAKEHEADER*/
#include "MP_FileTransp.h"
/*MAKEHEADER*/
#include "MP_TcpTransp.h"
/*MAKEHEADER*/
#include "MP_Vector.h"
/*MAKEHEADER*/
#include "MP_GmpBigInt.h"
/*MAKEHEADER*/
#include "MP_GmpBigReal.h"
/*MAKEHEADER*/
#include "MP_PariBigInt.h"
/*MAKEHEADER*/
#include "MP_PvmTransp.h"
/*MAKEHEADER*/
#include "MP_SacBigInt.h"
/*MAKEHEADER*/
#include "MP_TbTransp.h"

/* MP Dictionary includes */
/*MAKEHEADER*/
#include "MP_BasicDict.h"
/*MAKEHEADER*/
#include "MP_MpDict.h"
/*MAKEHEADER*/
#include "MP_NumberDict.h"
/*MAKEHEADER*/
#include "MP_ProtoDict.h"
/*MAKEHEADER*/
#include "MP_PolyDict.h"
/*MAKEHEADER*/
#include "MP_MatrixDict.h"

#endif /* _MP_h */
