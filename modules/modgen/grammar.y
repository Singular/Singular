/*
 * $Id: grammar.y,v 1.7 2000-02-18 13:33:47 krueger Exp $
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
extern int write_intro(moddefv module);
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

%token MCOLONCOLON

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
%token STATICTOK

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
          mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            myyyerror("Error while creating files\n");
            return 1;
          }
        };

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
          free($1);
          free($3);
        }
        | NAME '=' FILENAME
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type = VAR_NONE,
                            idtyp t, void *arg1 = NULL, void *arg2 = NULL);
          switch(sectnum) {
              case 1: /* pass 1: */
                Add2files(&module_def, $3);
                break;
              case 2: /* pass 2: procedure declaration */
                if( (vt=checkvar($1, VAR_FILE, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_FILE, vt, $1, $3);
                }
                break;
              default: break;
                
          }
          free($1);
          free($3);
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
          free($1);
          //free($3);
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
          free($1);
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
          free($1);
        }
;

files:  FILENAME ',' FILENAME
        {
          printf(">>>>>>>>files '%s' , '%s'\n", $1, $3);
          Add2files(&module_def, $1);
          Add2files(&module_def, $3);
          free($1);
          free($3);
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

/*
 */
procdef: procdecl proccode
        {
          printf("PROCDEF:\n");
        }
        | procdecl proccode procdeclexample
        {
          printf("PROCDEF mit example:\n");
          fflush(module_def.fmtfp);
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
          init_proc(&procedure_decl, $2, NULL, yylineno, LANG_SINGULAR);
          free($2); 
          if(write_singular_procedures(&module_def, &procedure_decl))
            myyyerror("Error while creating bin-file\n");
        }
        | STATICTOK PROCDECLTOK NAME
        {
          init_proc(&procedure_decl, $3, NULL, yylineno, LANG_SINGULAR);
          procedure_decl.is_static = TRUE;
          free($3);
          if(write_singular_procedures(&module_def, &procedure_decl))
            myyyerror("Error while creating bin-file\n");
        };

procdecl2: procdecl1 '(' sgtypelist ')'
        | funcdecl1
        | funcdecl1 '(' ')'
        | funcdecl1 '(' typelist ')'
        | procdecl1
         {
           write_singular_parameter(&module_def, yylineno, "list", "#");
           procedure_decl.lineno_other = yylineno;
         }
        | procdecl1 '(' ')'
         {
           write_singular_parameter(&module_def, yylineno, "list", "#");
           procedure_decl.lineno_other = yylineno;
         }
        ;

funcdecl1: NAME
        {
          printf("funcdecl1-1\n");
          init_proc(&procedure_decl, $1, NULL, yylineno);
          free($1);
        }
        | VARTYPETOK NAME
        {
          printf("funcdecl1-2\n");
          init_proc(&procedure_decl, $2, &$1, yylineno);
          free($2);
        }
        | STATICTOK NAME
        {
          printf("funcdecl1-3\n");
          init_proc(&procedure_decl, $2, NULL, yylineno);
          free($2);
          procedure_decl.is_static = TRUE;
        }
        | STATICTOK VARTYPETOK NAME
        {
          printf("funcdecl1-4\n");
          init_proc(&procedure_decl, $3, &$2, yylineno);
          free($3);
          procedure_decl.is_static = TRUE;
        };

  
procdeclhelp: MSTRINGTOK
        {
          procedure_decl.help_string = $1;
          printf("\t\thelp at %d\n", yylineno);
          write_help(&module_def, &procedure_decl);
        }
        ;

proccode: proccodeline MCODETOK
          {
            write_function_errorhandling(&module_def, &procedure_decl);
          };


proccodeline: CODEPART
        {
          write_codeline(&module_def, &procedure_decl, $1, yylineno-1);
        }
        | proccodeline CODEPART
        {
          write_codeline(&module_def, &procedure_decl, $2);
        }
        | proccodeline proccmd
        {
        };

procdeclexample: examplestart '{' examplecodeline MCODETOK
        {
          write_example(&module_def, &procedure_decl);
        }
        ;

examplestart: EXAMPLETOK
        {
          if(procedure_decl.procname == NULL) {
            myyyerror("example without proc-declaration at line %d\n",
                     yylineno);
          }
          printf("Example at %d\n", yylineno);
          procedure_decl.lineno_other = yylineno;
        };
  
examplecodeline: CODEPART
        {
          int len = strlen($1);
          procedure_decl.example_len = len;
          procedure_decl.example_string = (char *)malloc(len+1);
          memset(procedure_decl.example_string, 0, len+1);
          memcpy(procedure_decl.example_string, $1, len);
        }
        | examplecodeline CODEPART
        {
          long len = strlen($2);
          long newlen = procedure_decl.example_len + len;
          
          procedure_decl.example_string =
            (char *)realloc((void *)procedure_decl.example_string, newlen+1);
          memset(procedure_decl.example_string+procedure_decl.example_len,
                 0, len+1);
          memcpy(procedure_decl.example_string+procedure_decl.example_len,
                 $2, len);
          procedure_decl.example_len = newlen;
          //strncat(procedure_decl.example_string, $2, strlen($2));
          //procedure_decl.example_string[procedure_decl.example_len] = '\0';
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
        }
        | '%' NAME '(' identifier '(' arglist ')' ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg = NULL);
          
          if( (vt=checkcmd($2, &write_cmd, 1)) ) {
            //write_cmd(&module_def, &procedure_decl, $4);
          }
          else {
            myyyerror("Line %d: Unknown command '%s' in section %d\n",
                      yylineno, $2, sectnum);
          }
          free($2);
        };

identifier: NAME
        {
          printf("### Name %s\n", $1);
        }
        | identifier MCOLONCOLON NAME
        {
          printf("### Name %s\n", $3);
        };

arglist: NAME
        {
          printf("### ARGS %s\n", $1);
        }
        | arglist ',' NAME
        {
          printf("### ARGS %s\n", $3);
        };
        
sgtypelist: VARTYPETOK NAME
        {
          printf("\tsgtypelist %s %s\n", $1.name, $2);
          write_singular_parameter(&module_def, yylineno, $1.name, $2);
          free($1.name);
          free($2);
        }
        | VARTYPETOK '#'
        {
          printf("\tsgtypelist %s %s\n", $1.name, $2);
          write_singular_parameter(&module_def, yylineno, $1.name, "#");
          free($1.name);
        }
        | sgtypelist ',' VARTYPETOK NAME
        {
          printf("\tsgtypelist next  %s %s\n", $3.name, $4);
          write_singular_parameter(&module_def, yylineno, $3.name, $4);
          free($3.name);
          free($4);
        }
        | sgtypelist ',' VARTYPETOK '#'
        {
          printf("\tsgtypelist next  %s %s\n", $3.name, $4);
          write_singular_parameter(&module_def, yylineno, $3.name, "#");
          free($3.name);
        }
        ;

typelist: VARTYPETOK
        {
          AddParam(&procedure_decl, &$1);
          free($1.name);
        }
        | VARTYPETOK NAME
        {
          if(check_reseverd($2)) 
            myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, $2);
          AddParam(&procedure_decl, &$1, $2);
          free($1.name); free($2);
        }
        | typelist ',' VARTYPETOK
        {
          AddParam(&procedure_decl, &$3);
          free($3.name);
        }
        | typelist ',' VARTYPETOK NAME
        {
          if(check_reseverd($4)) 
            myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, $4);
          AddParam(&procedure_decl, &$3, $4);
          free($3.name); free($4);
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

