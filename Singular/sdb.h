#ifndef SDB_H
#define SDB_H
/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: Singular debugger


*/

#define HAVE_SDB

#ifdef HAVE_SDB
#include "kernel/structs.h"

extern int    sdb_lines[];
extern char * sdb_files[];
extern int    sdb_flags;

void sdb_edit(procinfo *pi);
void sdb_show_bp();
BOOLEAN sdb_set_breakpoint(const char *p, int lineno=0);
void sdb(Voice * currentVoice, const char * currLine, int len);
#endif
#endif

