/*******************************************************************
 *  File:    omError.h
 *  Purpose: Error handling of omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_ERROR_H
#define OM_ERROR_H

#include <stdio.h>
/*******************************************************************
 *
 *  error codes
 *
 *******************************************************************/
enum omError_e
{
  omError_NoError = 0,
  omError_Unknown,
  omError_InternalBug,
  omError_MemoryCorrupted,
  omError_NullAddr,
  omError_InvalidRangeAddr,
  omError_FalseAddr,
  omError_FalseAddrOrMemoryCorrupted,
  omError_WrongSize,
  omError_FreedAddr,
  omError_FreedAddrOrMemoryCorrupted,
  omError_WrongBin,
  omError_UnknownBin,
  omError_NotBinAddr,
  omError_UnalignedAddr,
  omError_NullSizeAlloc,
  omError_ListCycleError,
  omError_SortedListError,
  omError_KeptAddrListCorrupted,
  omError_FreePattern,
  omError_BackPattern,
  omError_FrontPattern,
  omError_NotString,
  omError_StickyBin,
  omError_MaxError
};
typedef enum omError_e omError_t;

/* global variable holding last omError */
extern omError_t om_ErrorStatus;
/* globale variable holding last low-level omError */
extern omError_t om_InternalErrorStatus;
/* returns description of error */
const char* omError2String(omError_t error);
/* returns error as string */
const char* omError2Serror(omError_t error);
extern omError_t omReportError(omError_t error, omError_t report_error, OM_FLR_DECL,
                               const char* fmt, ...);

/* this is a dummy function and used as default for om_Opts.ErrorHook */
extern void omErrorBreak(void);

#ifndef OM_NDEBUG
extern void omPrintAddrInfo(FILE* fd, void* addr, const char* s);
#else
#define omPrintAddrInfo(fd, addr, s) fprintf(fd, "OM_NDEBUG: no addr info available\n")
#endif

/*BEGINPRIVATE*/

#ifndef OM_NDEBUG
extern int om_CallErrorHook;
#else
#define om_CallErrorHook 1
#endif

/*******************************************************************
 *
 * om_assume(x) -- a handy macro for assumptions
 *
 ******************************************************************/
#ifndef HAVE_OM_ASSUME

#define omAssume(x) do {} while (0)

#else /* ! HAVE_OM_ASSUME */

#define omAssume(x)                                             \
do                                                              \
{                                                               \
  if (! (x))                                                    \
  {                                                             \
    omReportError(omError_InternalBug, omError_InternalBug, OM_FLR, "omAssume violation");   \
  }                                                             \
}                                                               \
while (0)

#endif /* HAVE_OM_ASSUME */
/*ENDPRIVATE*/

#endif /* OM_LOCAL_H */
