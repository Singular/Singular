#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.1.1.1 2003-10-06 12:15:55 Singular Exp $ */
/*
* ABSTRACT: flex/bison interface
*/

#include "structs.h"

typedef union
{
  int       i;
  char    * name;
  sleftv    lv;
} MYYSTYPE;
#define YYSTYPE MYYSTYPE
extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);

#endif
