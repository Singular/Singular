/*
 * $Id$
 */

%{

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "typmap.h"
#include "stype.h"


int sectnum = 1;
int iseof = 0;
int initdone = 0;
extern moddef module_def;
extern int yylineno;
extern int do_create_makefile;
extern char *sectname[];
 
extern int init_modgen(moddef *module_def, char *filename);
extern int write_intro(moddefv module);
extern void write_mod_init(moddefv module, FILE *fp);
extern void enter_id(FILE *fp, char *name, char *value,
                     int lineno, char *file);
 
procdef procedure_decl;

 
void yyerror(char * fmt)
  {
    if(!iseof) printf("%s at line %d\n", fmt, yylineno);
  }

%}

/* %expect 22 */
%pure_parser

%token MCOLONCOLON
%token MEQUAL

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
/* SECT2: Singular procedure declaration */
%token SECT2START
%token SECT2END
/* SECT3: procedure declaration */
%token SECT3START
%token SECT3END
/* SECT4: C/C++ text */
%token SECT4START
%token <name> SECT4END

/*%token PROCEND*/
%token PROCDECLTOK
%token EXAMPLETOK
%token STATICTOK

/* BISON Declarations */

%token VARNAMETOK
/*%token MSTRINGTOK */
%token <sv>   MSTRINGTOK
%token <name> NAME
%token <name> FILENAME
%token <name> MCCODETOK
%token <name> MCODETOK
%token <name> CODEPART
%token <name> CMTPART
%token <name> PROCCMD
%token <name> ANYTOK
%token  <tp> VARTYPETOK
%token <i>    NUMTOK
%token <i>    BOOLTOK

%type <i>    '='
%type <name> identifier

%nonassoc '='
%%
goal: part1 sect3 sect3end sect4
      {
          if(trace)printf("Finish modules 1\n");
          return 0;
      }
      | part1 sect3full sect4
      {
          if(trace)printf("Finish modules 2\n");
          return 0;
      }
      | part1 sect2full sect3full sect4
      {
          if(trace)printf("Finish modules 3\n");
          return 0;
      }
      | part1 sect3full sect2full sect4
      {
          if(trace)printf("Finish modules 4\n");
          return 0;
      }
;

part1: initmod sect1 sect1end
        {
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        }
        | sect1 sect1end
        {
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        }
        | sect1end
        {
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        };

initmod:
        MCCODETOK
        {
          fprintf(module_def.fmtfp, "%s", $1);
          fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
          free($1);
          write_mod_init(&module_def, module_def.fmtfp);
	  initdone = 1;
        };

sect1: expr ';'
       | sect1 expr ';'
;

        
sect1end:
        codeline2 SECTEND
	{
	  memset(&procedure_decl, 0, sizeof(procdef));
	  if(debug>2)printf("End of section 1 (new=%d)\n", sectnum);
	}  
	| SECTEND
        {
          memset(&procedure_decl, 0, sizeof(procdef));
          if(debug>2)printf("End of section 1 (new=%d)\n", sectnum);
        }
        ;

codeline2: CODEPART
        {
	  if(initdone == 0) {
	    write_mod_init(&module_def, module_def.fmtfp);
	    initdone = 1;
          }
	  fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
	                            yylineno, module_def.filename);
          fprintf(module_def.fmtfp, "%s", $1);
        }
        | codeline2 CODEPART
        {
        fprintf(module_def.fmtfp, "%s", $2);
        }
        ;

expr:   NAME '=' MSTRINGTOK
        {
          var_token vt;
          int rc = 0;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          
          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar($1, VAR_STRING, &write_cmd)) ) {
                  if(write_cmd!=0)
                    write_cmd(&module_def, vt, STRING_CMD, $1, $3.string);
                  if(vt==VAR_VERSION)
                    make_version($3.string, &module_def);
		  if(vt==VAR_MODULE)
		    make_module_name($3.string, &module_def);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar($1, VAR_STRING, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_STRING, vt, $1, $3.string);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              default: break;
                
          }
          free($1);
          free($3.string);
          if(rc)return(rc);
        }
        | NAME '=' FILENAME
        { var_token vt;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                Add2files(&module_def, $3);
                break;
              case 3: /* pass 3: procedure declaration */
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
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 3: /* pass 3: procedure declaration */
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
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar($1, VAR_NUM, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_NUM, vt, $1, &$3);
                }
                break;
              default: break;
                
          }
          free($1);
        }
        | NAME '=' BOOLTOK
        {
          var_token vt;
          int rc = 0;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar($1, VAR_BOOL, &write_cmd)) ) {
                  proc_set_default_var(VAR_BOOL, vt, $1, &$3);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar($1, VAR_BOOL, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_BOOL, vt, $1, &$3);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, $1, sectnum);
                }
                break;
              default: break;
                
          }
          free($1);
          if(rc)return(rc);
        }
;

files:  FILENAME ',' FILENAME
        {
          if(debug>2)printf(">>>>>>>>files '%s' , '%s'\n", $1, $3);
          Add2files(&module_def, $1);
          Add2files(&module_def, $3);
          free($1);
          free($3);
        }
;

sect2full: SECT2START sect2 sect2end;

sect2: procdefsg
       | sect2 procdefsg
       ;

sect2end: SECT2END
{
          if(debug>2)printf("End of section 'Singular' 2 (%d)\n",
                            sectnum);
};


sect3full: SECT3START sect3 sect3end;

sect3:  procdef 
        | sect3 procdef
        ;

sect3end: SECT3END
        {
          write_finish_functions(&module_def, &procedure_decl);
          if(debug>2)printf("End of section 'procedures' 3 (%d)\n",
                            sectnum);
        }
        ;

/*
 */
procdefsg: procdeclsg proccode
        {
          if(debug>2)printf("SG-PROCDEF:\n");
          write_singular_end(&module_def, &procedure_decl, yylineno);
	  setup_proc(&module_def, &procedure_decl);
	  write_helpfile_help(&module_def, &procedure_decl);
        }
        | procdeclsg proccode procdeclexample
        {
          if(debug>2)printf("SG-PROCDEF mit example:\n");
          fflush(module_def.fmtfp);
          write_singular_end(&module_def, &procedure_decl, yylineno);
          setup_proc(&module_def, &procedure_decl);
	  write_helpfile_help(&module_def, &procedure_decl);
        }
;

procdeclsg: procdeclsg2 '{'
        {
        }
        | procdeclsg2 procdeclhelp '{'
        {
        }
        ;

procdeclsg2: procdecl1 '(' sgtypelist ')'
         {
           procedure_decl.lineno_other = yylineno;
         }
        | procdecl1
         {
           procedure_decl.lineno_other = yylineno;
         }
        | procdecl1 '(' ')'
         {
           procedure_decl.lineno_other = yylineno;
         }
        ;

procdecl1: PROCDECLTOK NAME
        {
          init_proc(&procedure_decl, $2, NULL, yylineno, LANG_SINGULAR);
          free($2); 
          if(write_singular_procedures(&module_def, &procedure_decl))
            return(myyyerror("Error while creating bin-file\n"));
        }
        | STATICTOK PROCDECLTOK NAME
        {
          init_proc(&procedure_decl, $3, NULL, yylineno, LANG_SINGULAR);
          procedure_decl.is_static = TRUE;
          free($3);
          if(write_singular_procedures(&module_def, &procedure_decl))
            return(myyyerror("Error while creating bin-file\n"));
        };

procdef: procdecl proccode
        {
          if(debug>2)printf("PROCDEF:\n");
	  write_helpfile_help(&module_def, &procedure_decl);
        }
        | procdecl proccode procdeclexample
        {
          if(debug>2)printf("PROCDEF mit example:\n");
	  write_helpfile_help(&module_def, &procedure_decl);
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

procdecl2: funcdecl1
        | funcdecl1 '(' ')'
        | funcdecl1 '(' typelist ')';

funcdecl1: VARTYPETOK NAME
        {
          if(debug>2)printf("funcdecl1-2\n");
          init_proc(&procedure_decl, $2, &$1, yylineno);
          free($2);
        }
        | STATICTOK VARTYPETOK NAME
        {
          if(debug>2)printf("funcdecl1-4\n");
          init_proc(&procedure_decl, $3, &$2, yylineno);
          free($3);
          procedure_decl.is_static = TRUE;
        };

  
procdeclhelp: MSTRINGTOK
        {
          procedure_decl.help_string = $1.string;
          procedure_decl.lineno_other = $1.lineno;
          if(debug>2)printf("\t\thelp at %d\n", yylineno);
          write_help(&module_def, &procedure_decl);
        }
        ;

proccode: proccodeline MCODETOK
          {
            write_function_errorhandling(&module_def, &procedure_decl);
          };


proccodeline: CODEPART
        {
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, $1);
          write_codeline(&module_def, &procedure_decl, $1, yylineno-1);
        }
        | proccodeline CODEPART
        {
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, $2);
          write_codeline(&module_def, &procedure_decl, $2);
        }
        | proccodeline CMTPART
        {
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, $2);
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
            return(myyyerror("example without proc-declaration at line %d\n",
                     yylineno));
          }
          if(debug>2)printf("Example at %d\n", yylineno);
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
        };

proccmd: '%' NAME ';'
        { cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);
          
          switch(vt=checkcmd($2, &write_cmd, CMDT_SINGLE, 0)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;

              case CMD_DECL:
              case CMD_CHECK:
                procedure_decl.flags.auto_header = 0;
              case CMD_NODECL:
                write_cmd(&module_def, &procedure_decl,NULL);
                break;

              default:
                write_function_header(&module_def, &procedure_decl);
                write_cmd(&module_def, &procedure_decl,NULL);
          }
          free($2);
        }
        | '%' NAME '(' ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);
          
          switch(vt=checkcmd($2, &write_cmd, CMDT_0, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl,NULL);
          }
          free($2);
        }
        | '%' NAME '(' NAME ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);
          
          switch(vt=checkcmd($2, &write_cmd, CMDT_ANY, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, $4);
          }
          free($2); free($4);
        }
        | '%' NAME '(' identifier '(' arglist ')' ')' ';'
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);
          
          switch(vt=checkcmd($2, &write_cmd, CMDT_ANY, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, $4);
          }
          free($2);
        }
        | '%' NAME MEQUAL ANYTOK
        {
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);
          
          switch(vt=checkcmd($2, &write_cmd, CMDT_EQ, 0)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, $2, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, $4);
          }
          free($2);
        };


identifier: NAME
        {
          if(debug>2)printf("### ID ### Name %s\n", $1);
          $$ = $1;
        }
        | identifier MCOLONCOLON NAME
        {
          int len = strlen($$) + strlen($3) + 2;
          if(debug>2)printf("### ID ### Name %s\n", $3);
          $$ = (char *)realloc($$, len);
          strcat($$, "::");
          strcat($$, $3);
        };

arglist: NAME
        {
          if(debug>2)printf("### ARGS %s\n", $1);
        }
        | arglist ',' NAME
        {
          if(debug>2)printf("### ARGS %s\n", $3);
        };
        
sgtypelist: VARTYPETOK NAME
        {
          if(debug>2)printf("\tsgtypelist %s %s\n", $1.name, $2);
          write_singular_parameter(&module_def, yylineno, $1.name, $2);
          free($1.name);
          free($2);
        }
        | VARTYPETOK '#'
        {
          if(debug>2)printf("\tsgtypelist %s %s\n", $1.name, $2);
          write_singular_parameter(&module_def, yylineno, $1.name, "#");
          free($1.name);
        }
        | sgtypelist ',' VARTYPETOK NAME
        {
          if(debug>2)printf("\tsgtypelist next  %s %s\n", $3.name, $4);
          write_singular_parameter(&module_def, yylineno, $3.name, $4);
          free($3.name);
          free($4);
        }
        | sgtypelist ',' VARTYPETOK '#'
        {
          if(debug>2)printf("\tsgtypelist next  %s %s\n", $3.name, $4);
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
            return(myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, $2));
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
            return(myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, $4));
          AddParam(&procedure_decl, &$3, $4);
          free($3.name); free($4);
        }
        ;

sect4:  SECT4START codeline SECT4END
        {
        };

codeline: CODEPART
        {
          fprintf(module_def.modfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
          fprintf(module_def.modfp, "%s", $1);
        }
        | codeline CODEPART
        {
	  fprintf(module_def.modfp, "%s", $2);
        }
        ;


%%

