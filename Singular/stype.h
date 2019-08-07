#ifndef STYPE_H
#define STYPE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flex/bison interface
*/

#include "kernel/structs.h"
#include "Singular/subexpr.h"

typedef union
{
  int       i;
  char    * name;
  sleftv    lv;
} MYYSTYPE;
#define YYSTYPE MYYSTYPE
THREAD_VAR extern YYSTYPE  yylval;

int yylex(MYYSTYPE *l);

#endif
