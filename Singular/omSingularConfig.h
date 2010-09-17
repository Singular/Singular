/*******************************************************************
 *  File:    omSingularConfig.h
#include <omalloc/omalloc.h>
 *           This file is inlcuded by omDefaultConfig.h, i.e., at the the time
#include <omalloc/omalloc.h>
#include <omalloc/omalloc.h>
 *           rebuilt the library).
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_SINGULAR_CONFIG_H
#define OM_SINGULAR_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>

#define OM_MALLOC_HOOK(size)                OM_SINGULAR_HOOK
#define OM_FREE_HOOK(size)                  OM_SINGULAR_HOOK
#define OM_ALLOC_BINPAGE_HOOK               OM_SINGULAR_HOOK
#define OM_FREE_BINPAGE_HOOK                OM_SINGULAR_HOOK

#ifdef OM_ALLOC_SYSTEM_C
int om_sing_opt_show_mem = 0;
size_t om_sing_last_reported_size = 0;
#else
extern int om_sing_opt_show_mem;
extern size_t om_sing_last_reported_size;
#endif

/* number of bytes for difference to report: every 1 MByte */
#define SING_REPORT_THRESHOLD 1000*1024
#define OM_SINGULAR_HOOK                                                        \
do                                                                              \
{                                                                               \
  if (om_sing_opt_show_mem)                                                     \
  {                                                                             \
    size_t _current_bytes = om_Info.CurrentBytesFromMalloc +                    \
                            (om_Info.UsedPages << LOG_BIT_SIZEOF_SYSTEM_PAGE);  \
    size_t _diff = (_current_bytes > om_sing_last_reported_size ?               \
                   _current_bytes - om_sing_last_reported_size :                \
                   om_sing_last_reported_size - _current_bytes);                \
    if (_diff >= SING_REPORT_THRESHOLD)                                         \
    {                                                                           \
      fprintf(stdout, "[%ldk]", (_current_bytes + 1023)/1024);                   \
      fflush(stdout);                                                           \
      om_sing_last_reported_size = _current_bytes;                              \
    }                                                                           \
  }                                                                             \
}                                                                               \
while (0)

#ifdef __cplusplus
}
#endif

#endif /* OM_SINGULAR_CONFIG_H */
