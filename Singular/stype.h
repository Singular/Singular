#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.3 1997-04-09 12:20:14 Singular Exp $ */
/*
* ABSTRACT: flex/bison interface
*/

#include "structs.h"
#include "subexpr.h"

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
