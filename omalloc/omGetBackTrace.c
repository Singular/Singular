/*******************************************************************
 *  File:    omGetBackTrace.c
 *  Purpose: routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include "omalloc/omConfig.h"

#ifdef HAVE_OMALLOC
#ifndef OM_NDEBUG

#if __GNUC__ > 1

static void* om_this_main_frame_addr = 0;

void omInitGetBackTrace()
{
  if (__builtin_frame_address(0) != 0 &&
      __builtin_frame_address(1) > __builtin_frame_address(0))
    om_this_main_frame_addr = __builtin_frame_address(1);
}

#define OM_GET_BACK_TRACE(j)                                    \
case j:                                                         \
{                                                               \
  f_addr = __builtin_frame_address(j);                          \
  if (f_addr  > this_frame && f_addr < om_this_main_frame_addr) \
  {                                                             \
    r_addr = __builtin_return_address(j);                       \
    if (r_addr)                                                 \
    {                                                           \
      bt[i] = r_addr;                                           \
      i++;                                                      \
      if (i >= max) break;                                      \
    }                                                           \
    else break;                                                 \
  }                                                             \
  else break;                                                   \
}

int omGetBackTrace(void** bt, int start, int max)
{
  int i = 0;
  void* this_frame = __builtin_frame_address(0);
  void* f_addr;
  void* r_addr;

  start++;

  switch(start)
  {
    OM_GET_BACK_TRACE(1)
    OM_GET_BACK_TRACE(2)
/* the following fails on Mac OsX, but the debugging
 * support it provides is too useful to disable it
 */
#ifdef __linux
#if defined(__x86_64) || defined(__i386)
    OM_GET_BACK_TRACE(3)
    OM_GET_BACK_TRACE(4)
    OM_GET_BACK_TRACE(5)
    OM_GET_BACK_TRACE(6)
    OM_GET_BACK_TRACE(7)
    OM_GET_BACK_TRACE(8)
    OM_GET_BACK_TRACE(9)
    OM_GET_BACK_TRACE(10)
    OM_GET_BACK_TRACE(11)
    OM_GET_BACK_TRACE(12)
    OM_GET_BACK_TRACE(13)
    OM_GET_BACK_TRACE(14)
    OM_GET_BACK_TRACE(15)
    OM_GET_BACK_TRACE(16)
    OM_GET_BACK_TRACE(17)
#endif
#endif
  }
  if (i < max) bt[i] = 0;
  return i;
}

#endif /* __GNUC__ > 1 */

#endif /* ! OM_NDEBUG */
#endif
