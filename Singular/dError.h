/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    dError.h
 *  Purpose: declaration for debug error handling
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id: dError.h,v 1.3 2000-09-25 12:26:30 obachman Exp $
 *******************************************************************/
#ifndef DERROR_H
#define DERROR_H

#ifdef __cplusplus
extern "C" 
{
#endif
extern int dReportError(const char* fmt, ...);
#define dReportBug(s) \
  dReportError("Bug reported %s\n occurde at %s,%l\n", s, __FILE__, __LINE__)
#endif

// this is just a dummy procedure which is called after the error
// has been reported. Within the debugger, set a breakpoint on this
// proc.
extern void dErrorBreak();
#ifdef __cplusplus
}
#endif
