%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: scanner.l,v 1.3 1999-03-24 13:04:21 krueger Exp $ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "modgen.h"
#include <mod2.h>
#include <febase.h>
#include <grammar.h>
#include <ipid.h>
#include <ipshell.h>
#include <mmemory.h>
#include <structs.h>
#include <subexpr.h>
#include <tok.h>

const char sNoName[]="_";
char       my_yylinebuf[80];
int old_state = 0;
int yylineno = 1;
int tok;
int  myynest = -1;
int  traceit = 0;
moddef module_def;

extern "C"
{
  int yywrap();
}

%}

digit          [0-9]
letter         [@a-zA-Z\']
name           ({letter}({letter}*{digit}*_*)*|_)
fname          ({letter}({letter}*{digit}*_*.)*|_)
letters        ({letter}|{digit}|[_./#%^*:,])
string         ({letters}*)
comment        [\/][\/]
dolar          [$]
symbols        [~!@#$%^&*()_+-={}\\\|\[\];:,<.>/\?\' \n\~\`\r]
aletters       ({letter}|{digit}|{symbols}|{dolar}|{escquote})
strings        ({aletters}*)
quote          [\"]
escquote       (\\\")
taborspace     [ \t]
tos            ({taborspace}*)
eq             (=|{tos}+=|=+{tos}|{tos}+=+{tos})

/* %start START */

%x pdef
%x comment

%%
(\/\/[^\n]*)|(^#![^\n]*)|([ \t]) { }
\/\/*        { old_state = YYSTATE; BEGIN(comment); }

(module+{eq}+\"+{name}\") {
             char *buff = (char *)malloc(yyleng+4);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.name = (char *)malloc(strlen(buff)+1);
             strcpy(module_def.name, buff);
             strcat(buff, ".cc");
             Add2files(&module_def,buff);
             free(buff);
           }

(version+{eq}+\"+{strings}\") {
             char *buff = (char *)malloc(yyleng);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.version = (char *)malloc(strlen(buff)+1);
             strcpy(module_def.version, buff);
             make_version(module_def.version, &module_def);
             free(buff);
           }

(helpfile+{eq}+\"+{fname}\") {
             char *buff = (char *)malloc(yyleng);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.helpfile = (char *)malloc(strlen(buff)+1);
             strcpy(module_def.helpfile, buff);
             free(buff);
           }

(cxxsource+{eq}+{fname}) {
             char *buff = (char *)malloc(yyleng);
             sscanf( yytext, "%*[^=]=%s", buff);
             Add2files(&module_def,buff);
             free(buff);
           }

(info+{eq}+\"+{strings}\") {
             char *buff = (char *)malloc(yyleng);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.info = (char *)malloc(strlen(buff)+1);
             strcpy(module_def.info, buff);
             free(buff);
           }


(proc+{tos}+{name}+{tos}+{name})|({tos}+proc+{tos}+{name}+{tos}+{name}) {
             char proc[256], ret_val[256], n2[32];
             int cmd;
             memset(proc, '\0', 256);
             memset(ret_val, '\0', 256);
             memset(n2, '\0', 32);
             old_state = YYSTATE;
             BEGIN(pdef);
             sscanf( yytext, "%*[^p]proc %s %s", ret_val, proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s %s", ret_val, proc);
             printf("'%s'\n", yytext);
             printf("R: proc '%s' '%s'\n", ret_val, proc);
             cmd = IsCmd(ret_val, tok);
             if(cmd!=0)
                Add2proclist(&module_def, proc, ret_val,
					  decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid return parameter %s.\n",
			 module_def.procs[module_def.proccnt-1].procname,
                         ret_val);
           }

(proc+{tos}+{name})|({tos}+proc+{tos}+{name}) {
             char proc[256];
             old_state = YYSTATE;
             BEGIN(pdef);
             proc[0]='\0';
             sscanf( yytext, "%*[^p]proc %s", proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s", proc);
             Add2proclist(&module_def, proc, "none", "NONE", 0);
           }

<pdef>[ \t]  { }
<pdef>\(     {
             }
<pdef>{name}+, {
             char param[256], n2[32];
             int cmd;
             memset(n2, '\0', 32);
             memset(param, '\0', 256);
             sscanf( yytext, "%[^,],", param);
             cmd = IsCmd(param, tok);
             if(cmd!=0)AddParam(&module_def,
				param, decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid parameter %s.\n",
			 module_def.procs[module_def.proccnt-1].procname,
                         param);
           }
<pdef>{name}+\)+{eq}+{name} {
             char param[256], n2[32],funcname[256];
             int cmd;
             BEGIN(old_state);
             memset(param, '\0', 256);
             memset(n2, '\0', 32);
             memset(funcname, '\0', 256);
             sscanf( yytext, "%[^)])%*[^=]=%s", param, funcname);
             if(strlen(funcname)<=0)
                sscanf( yytext, "%[^)])=%s", param, funcname);
             free(module_def.procs[module_def.proccnt-1].funcname);
             module_def.procs[module_def.proccnt-1].funcname =
                (char *)malloc(strlen(funcname)+1);
             memset(module_def.procs[module_def.proccnt-1].funcname,
		    '\0', strlen(funcname)+1);
             memcpy(module_def.procs[module_def.proccnt-1].funcname,
		    funcname, strlen(funcname));
             cmd = IsCmd(param, tok);
             if(cmd!=0)AddParam(&module_def,
				param, decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid parameter %s.\n",
			 module_def.procs[module_def.proccnt-1].procname, param);
           }
<pdef>\)+{eq}+{name}|{eq}+{tos}+{name} {
             char funcname[256];
             BEGIN(old_state);
             sscanf( yytext, "%*[^=]=%s", funcname);
             if(strlen(funcname)<=0)
                sscanf( yytext, "=%s", funcname);
             free(module_def.procs[module_def.proccnt-1].funcname);
             module_def.procs[module_def.proccnt-1].funcname =
                (char *)malloc(strlen(funcname)+1);
             memset(module_def.procs[module_def.proccnt-1].funcname,
		    '\0', strlen(funcname)+1);
             memcpy(module_def.procs[module_def.proccnt-1].funcname,
		    funcname, strlen(funcname));
           }

<pdef>.      { }

<comment>\*\/            { BEGIN(old_state); }
<comment>\n              { yylineno++; }
<comment>.               { }


\n                       { yylineno++; }
\r                       { }
.                        { }

%%

extern "C" {
  int yywrap() {
    //printf("yywrap()\n");
    return 1;
  }
}

main( int argc, char *argv[] )
{
  ++argv, --argc;  /* skip over program name */
  if ( argc > 0 )
     yyin = fopen( argv[0], "rb" );
  else
     yyin = stdin;
  module_def.name=NULL;
  module_def.version=NULL;
  module_def.info=NULL;
  module_def.helpfile=NULL;
  module_def.procs=NULL;
  module_def.proccnt = 0;
  module_def.files = NULL;
  module_def.filecnt = 0;
  yylex();
  PrintProclist(&module_def);
  generate_mod(&module_def);
  mod_create_makefile(&module_def);

}
