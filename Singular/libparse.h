#ifndef LIBPARSE_H
#define LIBPARSE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: libparse.h,v 1.4 1998-04-20 10:05:53 krueger Exp $ */
/*
* ABSTRACT: lib parsing
*/
#ifndef STANDALONE_PARSER
#  include "subexpr.h"
#endif
typedef enum { LOAD_LIB, GET_INFO } lp_modes;
typedef enum { OLD_LIBSTYLE, NEW_LIBSTYLE } lib_style_types;

procinfo *iiInitSingularProcinfo(procinfov pi, char *libname,
                                 char *procname, int line, long pos,
				 BOOLEAN pstatic = FALSE);
int yylplex(char *libname, char *libfile, lib_style_types *lib_style, 
	    lp_modes=LOAD_LIB);
void reinit_yylp();

extern char * text_buffer;

#endif /* LIBPARSE_H */ 


