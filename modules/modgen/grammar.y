/*
 * $Id: grammar.y,v 1.2 1999-12-21 12:15:40 krueger Exp $
 */

%{

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "modgen.h"
#include "stype.h"

int sectnum = 1;
extern moddef module_def;
extern int yylineno;
 
extern int init_modgen(moddef *module_def, char *filename);
extern void write_intro(moddefv module);
extern void write_mod_init(FILE *fp);
extern void enter_id(FILE *fp, char *name, char *value,
                     int lineno, char *file);
extern int checkvar(char *varname);
 
procdef procedure_decl;

 
void yyerror(char * fmt)
  {
  }

%}

/* %expect 22 */
%pure_parser

/* special symbols */
%token <i> MMODULE_CMD
%token <i> MVERSION_CMD
%token <i> MINFO_CMD
%token <i> MINFOFILE_CMD
%token <i> MHELP_CMD
%token <i> MHELPFILE_CMD
%token <i> MCXXSRC_CMD
%token <i> MPROC_CMD

%token SECTEND
%token SECT2END
%token SECT3END
%token PROCEND
%token PROCDECLTOK

/* BISON Declarations */

%token VARNAMETOK
/*%token MSTRINGTOK */
%token <name> MSTRINGTOK
%token <name> NAME
%token <name> FILENAME
%token <name> MCCODETOK
%token <name> MCODETOK
%token <name> CODEPART
%token  <tp> VARTYPETOK

%type <i>    '='

%nonassoc '='
%%
goal: part1 sect2 sect2end code
        {
          printf("Finish modules\n");
          return 0;
        }
;

part1: initmod sect1 sect1end
        {
          write_intro(&module_def);
        }
        ;

initmod:
        MCCODETOK
        {
          fprintf(module_def.fmtfp, "%s", $1);
          fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
          free($1);
          write_mod_init(module_def.fmtfp);
        }

line:
        expr ';'
        {
          printf("EXPRESSION\n");
        }
        | procdecl
;

sect1: expr ';'
       | sect1 expr ';'
;

        
sect1end: SECTEND
        {
          printf("End of section %d\n", sectnum-1);
        }
        ;

expr:   NAME '=' MSTRINGTOK
        {
          if( checkvar($1) ) {
            enter_id(module_def.fmtfp, $1, $3, yylineno, module_def.filename);
          }
          else printf(">>>>>>>>Test 1 '%s' = '%s'\n", $1, $3);
        }
        | NAME '=' FILENAME
        {
          printf(">>>>>>>>Test 2 '%s' = '%s'\n", $1, $3);
        }
        | NAME '=' files
        {
          printf(">>>>>>>>Test 3 '%s' = '%s'\n", $1, $3);
        }
;

files:  FILENAME ',' FILENAME
        {
          printf(">>>>>>>>files '%s' , '%s'\n", $1, $3);
        }
;

sect2:  procdecl
        | sect2 procdecl
        ;

sect2end: SECT2END
        {
          write_finish_functions(&module_def, &procedure_decl);
          printf("End of section %d\n", sectnum-1);
        }
        ;

procdecl: procdecl2 '{' MCODETOK
        {
          setup_proc(&module_def, &procedure_decl, $3);
        }
        | procdecl2 procdeclhelp '{' MCODETOK
        {
          setup_proc(&module_def, &procedure_decl, $4);
        }
        ;

procdecl1: PROCDECLTOK NAME
        {
          init_proc(&procedure_decl, $2, NULL, yylineno);
        }
        | PROCDECLTOK VARTYPETOK NAME
        {
          init_proc(&procedure_decl, $3, &$2, yylineno);
        }

procdecl2: procdecl1
        | procdecl1 '(' ')'
        | procdecl1 '(' typelist ')'
        ;

procdeclhelp: MSTRINGTOK
        {
          printf("++++++++++Help section '%s'\n", $1);
        }
        ;

procdeclend: PROCEND
        {
          printf("End of procedure declaration\n");
        }
        ;

typelist: VARTYPETOK
        {
          AddParam(&procedure_decl, &$1);
        }
        | typelist ',' VARTYPETOK
        {
          AddParam(&procedure_decl, &$3);
        }
        ;

code:  codeline SECT3END
        {
          printf("C-code\n");
        }
;

codeline: CODEPART
        {
          printf(">%s", $1);
        }
        | codeline CODEPART
        {
          printf(">%s", $1);
        }
        ;


%%

