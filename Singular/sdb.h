#ifndef SDB_H
#define SDB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.h,v 1.4 1999-05-06 16:53:25 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#include "febase.h"
#include "structs.h"

extern int    sdb_lines[];
extern char * sdb_files[];
extern int    sdb_flags;

void sdb_edit(procinfo *pi);
void sdb(Voice * currentVoice, const char * currLine, int len);
#endif

