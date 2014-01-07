/*******************************************************************
 *  File:    omGetBackTrace.h
 *  Purpose: routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_GET_BACKTRACE_H
#define OM_GET_BACKTRACE_H

#if defined(OM_GET_BACKTRACE_WORKS) && !defined(OM_NDEBUG) && !defined(__OPTIMIZE__)
void omInitGetBackTrace();
int omGetBackTrace(void** bt, int start, int max);
#else
#define omInitGetBackTrace() do {} while (0)
#define omGetBackTrace(bt, s, max) (0)
#endif

#endif
