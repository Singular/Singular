/*
 * $Id: grammar.y,v 1.5 2000-01-27 12:40:54 krueger Exp $
 */

%{

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include <mod2.h>
#include <tok.h>

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
/*%token PROCEND*/
%token PROCDECLTOK
%token EXAMPLETOK

/* BISON Declarations */

%token VARNAMETOK
/*%token MSTRINGTOK */
%token <name> MSTRINGTOK
%token <name> NAME
%token <name> FILENAME
%token <name> MCCODETOK
%token <name> MCODETOK
%token <name> CODEPART
%token <name> PROCCMD
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

sect1: expr ';'
       | sect1 expr ';'
;

        
sect1end: SECTEND
        {
          memset(&procedure_decl, 0, sizeof(procdef));
          printf("End of section %d\n", sectnum-1);
        }
        ;

expr:   NAME '=' MSTRINGTOK
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          
          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar($1, VAR_STRING, &write_cmd)) ) {
                  if(write_cmd!=0)
                    write_cmd(&module_def, vt, STRING_CMD, $1, $3);
                }
                else {
                  myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_STRING, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_STRING, vt, $1, $3);
                }
                else {
                  myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              default: break;
                
          }
        }
        | NAME '=' FILENAME
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_FILE, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_FILE, vt, $1, $3);
                }
                break;
              default: break;
                
          }
        }
        | NAME '=' files
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_FILES, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_FILES, vt, $1, &$3);
                }
                break;
              default: break;
                
          }
        }
        | NAME '=' NUMTOK
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_NUM, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_NUM, vt, $1, &$3);
                }
                break;
              default: break;
                
          }
        }
        | NAME '=' BOOLTOK
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar($1, VAR_BOOL, &write_cmd)) ) {
                  proc_set_default_var(VAR_BOOL, vt, $1, &$3);
                }
                else {
                  myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_BOOL, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_BOOL, vt, $1, &$3);
                }
                else {
                  myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
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

sect2:  procdef 
        | sect2 procdef
        ;

sect2end: SECT2END
        {
          write_finish_functions(&module_def, &procedure_decl);
          printf("End of section %d\n", sectnum-1);
        }
        ;

procdef: procdecl proccode
        {
          printf("PROCDEF:\n");
        }
        | procdecl proccode procdeclexample
        {
          printf("PROCDEF mit example:\n");
        }
        ;

procdecl: procdecl2 '{'
        {
          setup_proc(&module_def, &procedure_decl);
        }
        | procdecl2 procdeclhelp '{'
        {
          setup_proc(&module_def, &procedure_decl);
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
          procedure_decl.help_string = strdup($1);
        }
        ;

proccode: proccodeline MCODETOK
          {
            write_function_errorhandling(&procedure_decl, module_def.fmtfp);
          };


proccodeline: CODEPART
        {
          fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
					  yylineno-1, module_def.filename);
          fprintf(module_def.fmtfp, "%s", $1);
        }
        | proccodeline CODEPART
        {
          fprintf(module_def.fmtfp, "%s", $2);
        }
        | proccodeline proccmd
        {
        };

procdeclexample: EXAMPLETOK examplecodeline MCODETOK
        {
          printf("Example\n");
          if(procedure_decl.procname != NULL) {
            //write_example(&module_def, &procedure_decl);
          } else {
            myyyerror("example without proc-declaration at line %d\n",
                     yylineno);
          }
        }
        ;

examplecodeline: CODEPART
        {
          printf(">>1\n");
          procedure_decl.example_len = strlen($1);
          procedure_decl.example_string = malloc(strlen($1));
          strncpy(procedure_decl.example_string, $1, strlen($1));
        }
        | examplecodeline CODEPART
        {
          printf(">>2\n");
          procedure_decl.example_len += strlen($2);
          procedure_decl.example_string =
            realloc(procedure_decl.example_string, procedure_decl.example_len);
          strcat(procedure_decl.example_string, $2);
          printf(">>2b\n");
        };

proccmd: '%' NAME ';'
        { cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg = NULL);
          
          if( (vt=checkcmd($2, &write_cmd, 0)) ) {
            write_cmd(&module_def, &procedure_decl);
          }
          else {
            myyyerror("Line %d: Unknown command '%s' in section %d\n",
                      yylineno, $2, sectnum);
          }
          free($2);
        }
        | '%' NAME '(' ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg = NULL);
          
          if( (vt=checkcmd($2, &write_cmd, 1)) ) {
            write_cmd(&module_def, &procedure_decl, procedure_decl.procname);
          }
          else {
            myyyerror("Line %d: Unknown command '%s' in section %d\n",
                      yylineno, $2, sectnum);
          }
          free($2);
        }
        | '%' NAME '(' NAME ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg = NULL);
          
          if( (vt=checkcmd($2, &write_cmd, 1)) ) {
            write_cmd(&module_def, &procedure_decl, $4);
          }
          else {
            myyyerror("Line %d: Unknown command '%s' in section %d\n",
                      yylineno, $2, sectnum);
          }
          free($2); free($4);
        };

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
          fprintf(module_def.modfp, "%s", $1);
        }
;

codeline: CODEPART
        {
          fprintf(module_def.modfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
        }
        | codeline CODEPART
        {
        }
        ;


%%

