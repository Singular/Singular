/*******************************************************************
 *  File:    omLocal.c
 *  Purpose: implementation of "local" (not visible to the outside) 
 *           routines for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omLocal.c,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/

#include <stdio.h>
#include "omConfig.h"
#include "omPrivate.h"
#include "omLocal.h"
#include "omPage.h"

const char* omReportError(const char* msg, const char* file, const int line)
{
  fprintf(stderr, "***Error: %s in %s:%d\n", msg, file, line);
  return msg;
}


