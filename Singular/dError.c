/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    dError.c
 *  Purpose: implementation for debug error handling
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id: dError.c,v 1.1 2000-09-04 13:38:55 obachman Exp $
 *******************************************************************/
#ifndef DERROR_C
#define DERROR_C
#include <stdarg.h>
#include <stdio.h>
#include "mod2.h"
#include "distrib.h"
#ifdef HAVE_CONFIG_H
#include "omalloc.h"
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

void dReportError(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
#ifndef MAKE_DISTRIBUTION
  fprintf(stderr, "// ***dErrror: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\noccured at: ");
#ifdef HAVE_CONFIG_H
  omPrintCurrentBackTrace(stderr);
#endif
#else
  fprintf(stderr, "// !!! YOU HAVE FOUND A BUG IN SINGULAR.");
  fprintf(stderr, "// !!! Please, email the following output to singular@mathematik.uni-kl.de");
  fprintf(stderr, "// !!! Singular Version: " S_UNAME S_VERSION1 " (%lu) %s \n",
          feVersionId, singular_date);
  vfprintf(stderr, fmt, ap);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
  


  
  
  
