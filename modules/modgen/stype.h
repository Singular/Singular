#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.2 1999-12-21 12:15:43 krueger Exp $ */
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

enum {
};

#endif
