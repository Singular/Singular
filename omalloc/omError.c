/*******************************************************************
 *  File:    omError.c
 *  Purpose: implementation of Error handling routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

#include <stdarg.h>
#include "omalloc.h"

#ifdef HAVE_OMALLOC

omError_t om_ErrorStatus = omError_NoError;
omError_t om_InternalErrorStatus = omError_NoError;

struct omErrorString_s
{
  omError_t error;
  char* s_error;
  char* string;
};

/* strings describing omErrors */
static const struct omErrorString_s om_ErrorStrings[] =
{
  {omError_NoError,                     "omError_NoError",                     "no error"},
  {omError_Unknown,                     "omError_Unknown",                     "unknown error" },
  {omError_MemoryCorrupted,             "omError_MemoryCorrupted",             "memory corrupted"},
  {omError_InternalBug,                 "omError_InternalBug",                 "internal omalloc bug"},
  {omError_NullAddr,                    "omError_NullAddr",                    "addr is NULL"},
  {omError_InvalidRangeAddr,            "omError_InvalidRangeAddr",            "addr not in valid range"},
  {omError_FalseAddr,                   "omError_FalseAddr",                   "addr not as returned by omalloc"},
  {omError_FalseAddrOrMemoryCorrupted,  "omError_FalseAddrOrMemoryCorrupted",  "addr not as returned by omalloc or memory corrupted", },
  {omError_WrongSize,                   "omError_WrongSize",                   "wrong size specification of addr"},
  {omError_FreedAddr,                   "omError_FreedAddr",                   "addr had previosuly been freed"},
  {omError_FreedAddrOrMemoryCorrupted,  "omError_FreedAddrOrMemoryCorrupted",  "addr had previosuly been freed  or memory corrupted"},
  {omError_WrongBin,                    "omError_WrongBin",                    "addr is not from given Bin"},
  {omError_UnknownBin,                  "omError_UnknownBin",                  "given Bin is unknown"},
  {omError_NotBinAddr,                  "omError_NotBinAddr",                  "addr is not a BinAddr"},
  {omError_UnalignedAddr,               "omError_UnalignedAddr",               "addr is unaligned"},
  {omError_NullSizeAlloc,               "omError_NullSizeAlloc",               "alloc of size 0"},
  {omError_ListCycleError,              "omError_ListCycleError",              "list has cycles"},
  {omError_SortedListError,             "omError_SortedListError",             "sorted list is unsorted"},
  {omError_KeptAddrListCorrupted,       "omError_KeptAddrListCorrupted",       "list of kept addresses are corrupted"},
  {omError_FrontPattern,                "omError_FrontPattern",                "written to front of addr"},
  {omError_BackPattern,                 "omError_BackPattern",                 "written after end of addr"},
  {omError_FreePattern,                 "omError_FreePattern",                 "written into freed memory"},
  {omError_NotString,                   "omError_NotString",                   "string not null terminated"},
  {omError_StickyBin,                   "omError_StickyBin",                   "wrong handling of sticky bins"},

  {omError_MaxError, NULL} /* this needs to be the last entry */
};

const char* omError2String(omError_t error)
{
  int i = 0;
  while (! (om_ErrorStrings[i].string == NULL && om_ErrorStrings[i].error == omError_MaxError))
  {
    if (om_ErrorStrings[i].error == error) return om_ErrorStrings[i].string;
    i++;
  }
  return "undocumented error";
}

const char* omError2Serror(omError_t error)
{
  int i = 0;
  while (! (om_ErrorStrings[i].string == NULL && om_ErrorStrings[i].error == omError_MaxError))
  {
    if (om_ErrorStrings[i].error == error) return om_ErrorStrings[i].s_error;
    i++;
  }
  return "omError_UnKnown";
}

#ifndef OM_NDEBUG
int om_CallErrorHook = 1;
#endif

omError_t omReportError(omError_t error, omError_t report_error, OM_FLR_DECL,
                        const char* fmt, ...)
{
  int max_check, max_track;

  if (report_error == omError_MaxError) return error;
  /* reset MaxTrack and MaxCheck to prevent infinite loop, in case
     printf allocates memory */
  max_check = om_Opts.MaxCheck;
  max_track = om_Opts.MaxTrack;
  om_Opts.MaxCheck = 0;
  om_Opts.MaxTrack = 0;

  om_InternalErrorStatus = error;
  om_ErrorStatus = (report_error == omError_NoError ? error : report_error);

  if (om_Opts.HowToReportErrors && om_ErrorStatus != omError_NoError)
  {
    /* to avoid spurious error msg in 64 bit mode*/
    if (om_ErrorStatus == omError_StickyBin) return error;
    fprintf(stderr, "***%s: %s", omError2Serror(om_ErrorStatus), omError2String(om_ErrorStatus));

#ifdef OM_INTERNAL_DEBUG
    if (om_ErrorStatus != error)
      fprintf(stderr, "\n___%s: %s", omError2Serror(error), omError2String(error));
#endif

    if (om_Opts.HowToReportErrors > 2 && fmt != NULL && *fmt != '\0')
    {
      va_list ap;
      va_start(ap, fmt);
      fputs( ": ",stderr);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
    }

    if (om_Opts.HowToReportErrors > 1)
    {
#ifndef OM_NDEBUG
      fputs("\n occurred at: ",stderr);
      if (! _omPrintCurrentBackTrace(stderr, OM_FLR_VAL))
        fputs(" ??",stderr);
#endif
    }
    fputc('\n',stderr);
    fflush(stderr);
  }
  if (om_CallErrorHook)
    om_Opts.ErrorHook();

  om_Opts.MaxCheck = max_check;
  om_Opts.MaxTrack = max_track;
  return error;
}


/* this is a dummy function and used as default for om_Opts.ErrorHook */
extern void omErrorBreak()
{}
#endif
