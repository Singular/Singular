#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flex/bison interface
*/
/* $Id: stype.h,v 1.2 1997-03-24 14:25:58 Singular Exp $ */

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
