/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    dError.cc
 *  Purpose: implementation for debug error handling
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id$
 *******************************************************************/
#ifndef DERROR_C
#define DERROR_C

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "config.h"

static inline void malloc_free( void * ptr )
{
  free(ptr);
}

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GCC_ABI_DEMANGLE
#include <cxxabi.h>
#endif


#include <reporter/reporter.h>

#ifdef HAVE_CONFIG_H
#include <omalloc/omalloc.h>
#endif

#ifndef MAKE_DISTRIBUTION
// dummy procedure for setting a breakpoint
// within the debugger
void dErrorBreak()
{}
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

int dReportError(const char* fmt, ...)
{
#ifdef HAVE_EXECINFO_H
#define SIZE 50
  void *buffer[SIZE+1]; int i, j, k, ret, status; char **ptr; char *demangledName; char *s; char *ss;
#endif

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

#ifdef HAVE_EXECINFO_H
  ret = backtrace( buffer, SIZE ); // execinfo.h
  fprintf(stderr, "\nExecinfo backtrace (with %zd stack frames): \n", ret);
  
#ifndef HAVE_GCC_ABI_DEMANGLE
  backtrace_symbols_fd(buffer, ret, STDERR_FILENO); // execinfo.h
#else
  ptr = backtrace_symbols( buffer, ret ); // execinfo.h

  for (i = 0; i < ret; i++)
  {
    status = -1;

    s = ptr[i];
//    fprintf (stderr, " #%02d: %s\n", i, s);
    
    ss = index(s, '(');
    ss[0] = 0;
    fprintf (stderr, " #%02d: '%s': ", i, s);
    ss[0] = '('; s = ss + 1;

    ss = index(s, '+');
    
    if ( ss != NULL )
    {
      ss[0] = 0;
      demangledName = abi::__cxa_demangle( s, NULL, NULL, &status ); // cxxabi.h!
      if( status == 0 && demangledName != NULL )
        fprintf (stderr, " '%s'", (demangledName[0] != 0)? demangledName: s);
      else
        fprintf (stderr, " '%s'", s);
        
      malloc_free( demangledName );
      ss[0] = '+';
      s = ss + 1;
    }
    
    ss = index(s, ')');
    if( s != ss)
    {
      ss[0] = 0;
      fprintf (stderr, " + %s", s);
      ss[0] = ')';
    }

    fprintf (stderr, " %s\n", ss + 2);
  }
  malloc_free (ptr);
#endif

#undef SIZE
#endif
  
  dErrorBreak();
#else
  fprintf(stderr, "\n// !!! YOU HAVE FOUND A BUG IN SINGULAR::Spielwiese.");
  fprintf(stderr, "// !!! Please, email the input\n// and the following error message to singular@mathematik.uni-kl.de")
  vfprintf(stderr, fmt, ap);
#endif
  return 0;
}



#ifdef __cplusplus
}
#endif

#endif
  


  
  
  
