/*
 * $Id: grammar.y,v 1.1 1999-11-23 21:30:21 krueger Exp $
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
%token <name> VARTYPETOK

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
        {
          printf("unknown error processing section 2\n" );
        }
        ;

sect2end: SECT2END
        {
          printf("End of section %d\n", sectnum-1);
        }
        ;

procdecl: PROCDECLTOK NAME '{' MCODETOK
        {
          printf("Proc without parameters 2\n");
          printf("========== proc %s begin ===========================\n", $2);
          printf($4);
          printf("========== proc end =============================\n");
        }
        | PROCDECLTOK VARTYPETOK NAME '{' MCODETOK
        {
          printf("Proc without parameters 2\n");
          printf("========== proc %s begin ===========================\n", $2);
          printf($3);
          printf("========== proc end =============================\n");
        }
        | PROCDECLTOK NAME '(' typelist ')' '{' MCCODETOK
        {
          printf("Proc with parameters\n");
        }
        | PROCDECLTOK VARTYPETOK NAME '(' typelist ')' '{' MCCODETOK
        {
          printf("Proc with parameters\n");
        }
        ;

procdeclend: PROCEND
        {
          printf("End of procedure declaration\n");
        }
        ;

typelist: VARTYPETOK
        {
          printf(">>>>>>VarType\n");
        }
        | typelist ',' VARTYPETOK
        {
          printf(">>>>>>VarType\n");
        }
        ;

code:  SECT3END
        {
          printf("C-code\n");
        }
;

%%

