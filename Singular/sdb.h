#ifndef SDB_H
#define SDB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.h,v 1.3 1999-04-29 16:57:18 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#include "febase.h"

extern int sdb_lines[];

void sdb(Voice * currentVoice, char * currLine, int len, char * buffer);
#endif

