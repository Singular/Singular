#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.3 2000-01-17 08:32:27 krueger Exp $ */
/*
* ABSTRACT: flex/bison interface
*/

#include "modgen.h"

typedef union
{
  int       i;
  char    * name;
  moddefv    lv;
  paramdef  tp;
} MYYSTYPE;
#define YYSTYPE MYYSTYPE
extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);


#endif
