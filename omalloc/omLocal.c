/*******************************************************************
 *  File:    omLocal.c
 *  Purpose: implementation of "local" (not visible to the outside) 
 *           routines for omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omLocal.c,v 1.2 1999-11-22 18:12:59 obachman Exp $
 *******************************************************************/

#include <stdio.h>
#include "omConfig.h"
#include "omPrivate.h"
#include "omLocal.h"
#include "omPage.h"
#include "omTrack.h"

const char* omReportError(const char* msg)
{
  fprintf(stderr, "***Error: %s: occured at \n", msg);
  omPrintCurrentBackTrace(1, 20, stderr);
  return msg;
}


