#ifndef LIBPARSE_H
#define LIBPARSE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: libparse.h,v 1.1 1998-03-11 12:03:47 Singular Exp $ */
/*
* ABSTRACT: lib parsing
*/
#include "subexpr.h"
procinfo *iiInitSingularProcinfo(procinfov pi, char *libname,
                                 char *procname, int line, long pos,
				 BOOLEAN pstatic = 0);
#endif


