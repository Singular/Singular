/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    dError.c
 *  Purpose: implementation for debug error handling
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id$
 *******************************************************************/
#ifndef DERROR_C
#define DERROR_C
#include <stdarg.h>
#include <stdio.h>
#include <kernel/mod2.h>
#include <Singular/distrib.h>
#ifdef HAVE_CONFIG_H
#include <omalloc/omalloc.h>
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

int dReportError(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
#ifndef MAKE_DISTRIBUTION
  fprintf(stderr, "\n// ***dError: ");
  vfprintf(stderr, fmt, ap);
#if 0
    if !defined(OM_NDEBUG) && defined(HAVE_CONFIG_H)
#endif
#if  defined(HAVE_CONFIG_H)
  fprintf(stderr, " occured at: \n");
  omPrintCurrentBackTraceMax(stderr, 8);
#endif
  dErrorBreak();
#else
  fprintf(stderr, "\n// !!! YOU HAVE FOUND A BUG IN SINGULAR.");
  fprintf(stderr, "// !!! Please, email the following output to singular@mathematik.uni-kl.de");
  fprintf(stderr, "// !!! Singular Version: " S_UNAME S_VERSION1 "\n");
  vfprintf(stderr, fmt, ap);
#endif
  return 0;
}


#ifndef MAKE_DISTRIBUTION
// dummy procedure for setting a breakpoint
// within the debugger
void dErrorBreak()
{}
#endif

#ifdef __cplusplus
}
#endif

#endif
  


  
  
  
