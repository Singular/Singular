/*******************************************************************
 *  File:    omGetBackTrace.h
 *  Purpose: routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omGetBackTrace.h,v 1.3 2000-08-14 12:26:45 obachman Exp $
 *******************************************************************/
#ifndef OM_GET_BACKTRACE_H
#define OM_GET_BACKTRACE_H

#if defined(OM_GET_BACKTRACE_WORKS) && !defined(OM_NDEBUG)
void omInitGetBackTrace();
int omGetBackTrace(void** bt, int start, int max);
#else
#define omInitGetBackTrace() ((void)0)
#define omGetBackTrace(bt, s, max) ((void)0)
#endif

#endif 
