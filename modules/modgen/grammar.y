/*
 * $Id: grammar.y,v 1.3 2000-01-17 08:32:25 krueger Exp $
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
extern int checkvar(char *varname, var_type type);
 
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
%token <i>    NUMTOK
%token <i>    BOOLTOK

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
          switch(sectnum) {
              case 1: /* pass 1: */
                if( checkvar($1, VAR_STRING) ) {
                  enter_id(module_def.fmtfp, $1, $3, yylineno,
                           module_def.filename);
                }
                else {
                  myyyerror("Unknown variable '%s' in section %d\n", $1,
                            sectnum);
                }
                
                  // proc_set_var(&procedure_decl, VAR_STRING, $1, $3);
                break;
              case 2: /* pass 2: procedure declaration */
                proc_set_var(&procedure_decl, VAR_FILE, $1, $3);
                break;
              default: break;
                
          }
        }
        | NAME '=' FILENAME
        {
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                proc_set_var(&procedure_decl, VAR_FILE, $1, $3);
                break;
              default: break;
                
          }
        }
        | NAME '=' files
        {
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                proc_set_var(&procedure_decl, VAR_FILES, $1, &$3);
                break;
              default: break;
                
          }
        }
        | NAME '=' NUMTOK
        {
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                proc_set_var(&procedure_decl, VAR_NUM, $1, &$3);
                break;
              default: break;
                
          }
        }
        | NAME '=' BOOLTOK
        {
          printf("BOOL\n");
          
          switch(sectnum) {
              case 1: /* pass 1: */
                if( checkvar($1, VAR_BOOL) ) {
                  
                }
                else {
                  myyyerror("Unknown variable '%s' in section %d\n", $1,
                            sectnum);
                }
                break;
              case 2: /* pass 2: procedure declaration */
                proc_set_var(&procedure_decl, VAR_BOOL, $1, &$3);
                break;
              default: break;
                
          }
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

procdecl: procdecl3 '{' MCODETOK 
        {
          setup_proc(&module_def, &procedure_decl, $3);
        }
        | procdecl3 procdeclhelp '{' MCODETOK
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

procdecl3: procdecl2
        | procdecl2 procdeclflags
        ;

procdeclhelp: MSTRINGTOK
        {
          procedure_decl.help_string = strdup($1);
        }
        ;

procdeclflags: expr ';'
        {
          printf("expr-1\n");
        }
        | procdeclflags expr ';'
        {
          printf("expr-2\n");
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

