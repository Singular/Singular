#ifndef SDB_H
#define SDB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.h,v 1.2 1999-04-20 17:02:49 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#include "febase.h"

extern int sdb_lines[];

void sdb(Voice * currentVoice, char * currLine, int len);
#endif

