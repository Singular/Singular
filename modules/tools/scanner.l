%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: scanner.l,v 1.2 1999-03-17 12:41:31 krueger Exp $ */
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
procdefv proclist = NULL;
moddef module_def;
cfilesv c_filelist = NULL;

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
             c_filelist = Add2files(c_filelist,buff);
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
             c_filelist = Add2files(c_filelist,buff);
             free(buff);
           }

(info+{eq}+\"+{strings}\") {
             char *buff = (char *)malloc(yyleng);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.info = (char *)malloc(strlen(buff)+1);
             strcpy(module_def.info, buff);
             free(buff);
           }


(proc+{tos}+{name})|({tos}+proc+{tos}+{name}) {
             char proc[256];
             old_state = YYSTATE;
             BEGIN(pdef);
             proc[0]='\0';
             sscanf( yytext, "%*[^p]proc %s", proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s", proc);
             proclist = Add2proclist(proclist, proc);
             //printf("proc: %s\n", proc);
           }

<pdef>[ \t]  { }
<pdef>\(     {
             }
<pdef>{name}+, {
             char param[256], n2[32];
             int cmd;
             param[0]='\0';
             sscanf( yytext, "%[^,],", param);
             cmd = IsCmd(param, tok);
             if(cmd!=0)AddParam(proclist, param, decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid parameter %s.\n",
			 proclist->procname, param);
           }
<pdef>{name}+\)+{eq}+{name} {
             char param[256], n2[32],funcname[256];
             int cmd;
             BEGIN(old_state);
             memset(funcname, '\0', 256);
             param[0]='\0';
             sscanf( yytext, "%[^)])%*[^=]=%s", param, funcname);
             if(strlen(funcname)<=0)
                sscanf( yytext, "%[^)])=%s", param, funcname);
             proclist->funcname = (char *)malloc(strlen(funcname)+1);
             strcpy(proclist->funcname, funcname);
             cmd = IsCmd(param, tok);
             if(cmd!=0)AddParam(proclist, param, decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid parameter %s.\n",
			 proclist->procname, param);
           }
<pdef>\)+{eq}+{name}|{eq}+{tos}+{name} {
             char funcname[256];
             BEGIN(old_state);
             sscanf( yytext, "%*[^=]=%s", funcname);
             if(strlen(funcname)<=0)
                sscanf( yytext, "=%s", funcname);
             proclist->funcname = (char *)malloc(strlen(funcname)+1);
             strcpy(proclist->funcname, funcname);
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
  yylex();
  PrintProclist(proclist);
  generate_mod(proclist, &module_def, c_filelist);
  mod_create_makefile(&module_def, c_filelist);

}
