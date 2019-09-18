#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
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
THREAD_VAR extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);


#endif
