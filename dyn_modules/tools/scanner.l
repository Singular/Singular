%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
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

#define DEBUG 0
 
#  define YYLP_ERR_NONE    0
#  define YYLP_DEF_BR2     1
#  define YYLP_BODY_BR2    2
#  define YYLP_BODY_BR3    3
#  define YYLP_BODY_TMBR2  4
#  define YYLP_BODY_TMBR3  5
#  define YYLP_EX_BR2      6
#  define YYLP_EX_BR3      7
#  define YYLP_BAD_CHAR    8
#  define YYLP_MISSQUOT    9
#  define YYLP_MISS_BR1   10
#  define YYLP_MISS_BR2   11
#  define YYLP_MISS_BR3   12

const char sNoName[]="_";
int brace1 = 0;  /* { } */
int brace2 = 0;  /* ( ) */
int brace3 = 0;  /* [ ] */
int quote  = 0;  /* " */
int offset = 0;

 char       my_yylinebuf[80];
int *old_states = NULL;
int state_level = -1;
int state_max = 0;
int yylineno = 1;
int tok;
int  myynest = -1;
int  traceit = 0;
moddef module_def;
 int yylplineno = 1;
 int yylp_errno = 0;

 long C_start;
 long C_end;
 char c_codetype = 0;
#define C_CODE_NONE 0
#define C_CODE_PROC 1
#define C_CODE_MAIN 2
 
 char string_type = 0;
#define STRING_NONE 0
#define STRING_INFO 1
#define STRING_VERS 2
 long string_start = 0;
 long string_end = 0;
 
char *yylp_buffer_start;

int libread(FILE* f, char* buf, int max_size);
int current_pos(int i);
int read_string(char **buffer, long *start, long end);
 
void push_state(int state, int new_state);
void pop_state();
 
#  undef YY_INPUT
#  define YY_INPUT(buf,result,max_size) \
          if ( ((result = libread( (yyin), (char *) buf, max_size )) < 0 ) \
                  && ferror( yyin ) ) \
                YY_FATAL_ERROR( "read in flex scanner failed" );


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
tnl            ([ \t\n]*)
col            (;|{tos}+;)
eqnl           ([ \t\n]*+=[ \t\n]*)

/* %start START */

%x pdef
%x comment
%x procdef
%x ctext
%x string
%x cstring

%%
(\/\/[^\n]*)|(^#![^\n]*)|([ \t]) { }
\/\/*        { push_state(YYSTATE, comment); }

(module+{eqnl}+{quote}+{name}+{quote}) {
             char *buff = (char *)malloc(yyleng+4);
             memset(buff, '\0', yyleng+4);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.name = (char *)malloc(strlen(buff)+1);
             memset(module_def.name, '\0', strlen(buff)+1);
             memcpy(module_def.name, buff, strlen(buff));
             strcat(buff, ".cc");
             Add2files(&module_def, buff);
             free(buff);
           }

(version+{eqnl}+{quote}+{strings}+{quote}) {
             yyless(4);
             string_type = STRING_VERS;
             push_state(YYSTATE, cstring);
           }

(helpfile+{eqnl}+{quote}+{fname}+{quote}) {
             char *buff = (char *)malloc(yyleng+1);
             memset(buff, '\0', yyleng+1);
             sscanf( yytext, "%*[^\"]\"%[^\"]\"", buff);
             module_def.helpfile = (char *)malloc(strlen(buff)+1);
             memset(module_def.helpfile, '\0', strlen(buff)+1);
             memcpy(module_def.helpfile, buff, strlen(buff));
             printf("==>HELP:'%s'\n", module_def.helpfile);
             free(buff);
           }

(cxxsource+{eqnl}+{fname}) {
             char *buff = (char *)malloc(yyleng+1);
             memset(buff, '\0', yyleng+1);
             sscanf( yytext, "%*[^=]=%s", buff);
             Add2files(&module_def,buff);
             free(buff);
           }

(info+{eqnl}+{quote}+{strings}+{quote}) {
             yyless(4);
             string_type = STRING_INFO;
             push_state(YYSTATE, cstring);
           }

<cstring>{quote} { quote++; push_state(YYSTATE, string);
                string_start = current_pos(yyleng); }
<cstring>\n { yylplineno++; }
<cstring>{col} {
            switch(string_type) {
                case STRING_INFO:                     
                  read_string(&module_def.info, &string_start, string_end);
                  break;
                  
                case STRING_VERS:                     
                  read_string(&module_def.version, &string_start, string_end);
                  make_version(module_def.version, &module_def);
                  break;
            }
            string_type = STRING_NONE;
            pop_state(); }
<cstring>.  { }



(proc+{tos}+{name})|({tos}+proc+{tos}+{name}) {
             char *proc = (char *)malloc(yyleng+1);
             memset(proc, '\0', yyleng+1);
             push_state(YYSTATE, pdef);
             sscanf( yytext, "%*[^p]proc %s", proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s", proc);
             Add2proclist(&module_def, proc, "none", "NONE", NONE);
             free(proc);
           }

(proc+{tos}+{name}+{tos}+{name})|({tos}+proc+{tos}+{name}+{tos}+{name}) {
             char *proc = (char *)malloc(yyleng+1);
             char *ret_val = (char *)malloc(yyleng+1);
             memset(proc, '\0', yyleng+1);
             memset(ret_val, '\0', yyleng+1);

             char n2[32];
             int cmd;
             memset(n2, '\0', 32);
             push_state(YYSTATE, pdef);
             sscanf( yytext, "%*[^p]proc %s %s", ret_val, proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s %s", ret_val, proc);
             printf("'%s'\n", yytext);
             printf("R: proc '%s' '%s'\n", ret_val, proc);
             cmd = IsCmd(ret_val, tok);
             if(cmd!=0)
                Add2proclist(&module_def, proc, ret_val,
					  decl2str(tok,n2), tok);
             else {
               printf("proc '%s': Invalid return parameter %s.\n",
                         proc, ret_val);
               Add2proclist(&module_def, proc, "none",
					  "NONE", NONE);
             }
             free(proc);
             free(ret_val);
           }

<pdef>[ \t]  { }
<pdef>"("    { }
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
<pdef>{name}+")"+{tnl}+"{" {
             char param[256], n2[32];
             int cmd;
             BEGIN(procdef);
             memset(param, '\0', 256);
             memset(n2, '\0', 32);
             sscanf( yytext, "%[^)])", param);
             cmd = IsCmd(param, tok);
             if(cmd!=0)AddParam(&module_def,
				param, decl2str(tok,n2), tok);
             else printf("proc '%s': Invalid parameter %s.\n",
			 module_def.procs[module_def.proccnt-1].procname,
			 param);
           }
<pdef>")"+{tnl}+"{" {
             BEGIN(procdef);
           }

<pdef>"{" {
             BEGIN(procdef);
           }

<pdef>(")"+{eqnl}+{name})|({eq}+{name}) {
             char funcname[256];
             pop_state();
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

<pdef>({name}+")"+{eqnl}+{name}) {
             char param[256], n2[32],funcname[256];
             int cmd;
             pop_state();
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

<pdef>.      { }

<procdef>"}"  { pop_state(); }
<procdef>RETURN+{eqnl}+{name}+{col}   {
  printf("Return:\n");}
<procdef>function+{eqnl}+{name}+{col} {
       char *funcname = (char *)malloc(yyleng+1);
       memset(funcname, '\0', yyleng+1);
       sscanf( yytext, "%*[^=]=%[^;];", funcname);
       if(strlen(funcname)<=0)
         sscanf( yytext, "=%[^;];", funcname);
       free(module_def.procs[module_def.proccnt-1].funcname);
       module_def.procs[module_def.proccnt-1].funcname =
         (char *)malloc(strlen(funcname)+   1);
       memset(module_def.procs[module_def.proccnt-1].funcname,
              '\0', strlen(funcname)+1);
       memcpy(module_def.procs[module_def.proccnt-1].funcname,
              funcname, strlen(funcname));
     }

<procdef>checkring+{col}            {
  printf("Do checkring\n"); }
<procdef>C+{eqnl}+"{" {
                push_state(YYSTATE, ctext);
                C_start=current_pos(yyleng);
                yyless(yyleng-1);
                c_codetype = C_CODE_PROC;
              }
<procdef>.                          { printf("%s", yytext); }

<comment>\*\/            { pop_state(); }
<comment>\n              { yylineno++; }
<comment>.               { }

<ctext>({comment}[^\n]*) { }
<ctext>\/\/*   { push_state(YYSTATE, comment); }
<ctext>{quote} { quote++; push_state(YYSTATE, string); }
<ctext>"{"     { brace1++; }
<ctext>"}"     {
       brace1--;
       if(brace2>0) {
         yylp_errno = YYLP_BODY_BR2;
         return(1);
       }
       if(brace3>0) {
         yylp_errno = YYLP_BODY_BR3;
         return(1);
       }
       if(brace1<=0) {
         C_end = current_pos(yyleng)-1;
         printf("Ctext: %d-%d\n", C_start, C_end);
         switch(c_codetype) {
             case C_CODE_PROC:
               read_string(&(module_def.procs[module_def.proccnt-1].c_code),
                           &C_start, C_end);
               break;

             case C_CODE_MAIN:
               break;
         }
         c_codetype = C_CODE_NONE;
         pop_state();
       }
     }
<ctext>"("       { brace2++; }
<ctext>")"       { brace2--;
                   if(brace2<0) {
                     yylp_errno = YYLP_BODY_TMBR2;
                     return(1);
                   }
                 }
<ctext>"["       { brace3++; }
<ctext>"]"       { brace3--;
                   if(brace3<0) {
                     yylp_errno = YYLP_BODY_TMBR3;
                     return(1);
                   }
                 }
<ctext>\n  { yylineno++; }
<ctext>.   { }

<string>{quote} { quote--; string_end = current_pos(yyleng)-1; pop_state(); }
<string>(\\\\)|(\\\") { }
<string>\n            { yylplineno++; }
<string>.             { }


C+{eqnl}+"{" { push_state(YYSTATE, ctext);
               C_start=current_pos(yyleng);
               yyless(yyleng-1);
             }
\n           { yylineno++; }
\r           { }
.            { }

%%

extern "C" {
  int yywrap() {
    //printf("yywrap()\n");
    return 1;
  }
}

int libread(FILE* f, char* buf, int max_size)
{ int rc;

  offset = ftell(f);
  rc  = fread( buf, 1, max_size, f );
#if YYLPDEBUG >2
  printf("fread: %d of %d\n", rc, max_size);
#endif
  yylp_buffer_start = buf;
  return rc;
}

int current_pos(int i)
{
  return(i+offset+(int)(yytext-yylp_buffer_start));
}

int read_string(char **p, long *start, long end) 
{
  char *buffer;
  
  if(*start > 0 ) {
    long len = end - *start;
    if(len>=0) {
      long cur = ftell(yyin);
      buffer = (char *)malloc(len+1);
      memset(buffer, '\0', len+1);
      fseek(yyin, *start, SEEK_SET);
      fread(buffer, 1, len, yyin);
      fseek(yyin, cur, SEEK_SET);
      *p = buffer;
    }
    *start = 0;
  }
}
                
void push_state(int state, int new_state)
{
  state_level++;
#if DEBUG
  printf("====>PUSH to new state %d/%d l=%d\n", state, new_state, state_level);
#endif
  if(state_level>=state_max) {
    state_max++;
    if(old_states == NULL)
      old_states = (int *)malloc(sizeof(int));
    else {
      old_states = (int *)realloc(old_states, state_max*sizeof(int));
    }
  }
  old_states[state_level] = state;
  BEGIN(new_state);
}

void pop_state()
{
#if DEBUG
  printf("====>Back to old state %d, l=%d\n", old_states[state_level],
	 state_level);
#endif
  BEGIN(old_states[state_level]);
  state_level--;
  if(state_level<0) state_level = -1;
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
