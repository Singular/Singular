#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stype.h,v 1.1 1999-11-23 21:30:23 krueger Exp $ */
/*
* ABSTRACT: flex/bison interface
*/

#include "modgen.h"

typedef union
{
  int       i;
  char    * name;
  moddefv    lv;
} MYYSTYPE;
#define YYSTYPE MYYSTYPE
extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);

enum {
};

#endif
