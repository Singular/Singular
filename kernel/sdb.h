#ifndef SDB_H
#define SDB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.h,v 1.1.1.1 2003-10-06 12:16:03 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#define HAVE_SDB

#ifdef HAVE_SDB
#include "febase.h"
#include "structs.h"

extern int    sdb_lines[];
extern char * sdb_files[];
extern int    sdb_flags;

void sdb_edit(procinfo *pi);
void sdb_show_bp();
BOOLEAN sdb_set_breakpoint(const char *p, int lineno=0);
void sdb(Voice * currentVoice, const char * currLine, int len);
#endif
#endif

