#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.4 2000-03-29 09:31:43 krueger Exp $ */
/*
* ABSTRACT: flex/bison interface
*/

#include "modgen.h"

typedef union
{
  int       i;
  char    * name;
  moddefv    lv;
  stringdef sv;
  paramdef  tp;
} MYYSTYPE;
#define YYSTYPE MYYSTYPE
extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);


#endif
