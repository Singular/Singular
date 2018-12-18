%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "kernel/mod2.h"
#ifdef STANDALONE_PARSER
  #include "Singular/utils.h"

  #define HAVE_LIBPARSER
  #define YYLPDEBUG 1
  #define myfread fread
#else
  #include "Singular/subexpr.h"
  #include "Singular/grammar.h"
  #include "Singular/ipshell.h"
  #include "Singular/ipid.h"
  #include "Singular/tok.h"
  #include "misc/options.h"
  #include "omalloc/omalloc.h"
#endif
#include "Singular/libparse.h"

#ifdef HAVE_LIBPARSER
#define YY_SKIP_YYWRAP

typedef enum { LP_NONE, LP_INFO, LP_CATEGORY, LP_URL, LP_VERSION} lib_cmds;

int libread(FILE* f, char* buf, int max_size);
int current_pos(int i);
void print_version(lp_modes mode, char *p);
void copy_string(lp_modes mode);
void make_version(char *p, int what);

int brace1 = 0;  /* { } */
int brace2 = 0;  /* ( ) */
int brace3 = 0;  /* [ ] */
int quote  = 0;  /* " */
int offset = 0;
BOOLEAN p_static = FALSE;
int old_state = 0;
lib_cmds last_cmd = LP_NONE;

char libnamebuf[1024];
char *text_buffer=NULL;
long string_start;

char *yylp_buffer_start;
#ifndef NEW_FLEX
int yylplineno = 1;
#endif /* NEW_FLEX */
int lpverbose = 0, check = 0;
int texinfo_out = 0;
int found_info=0,
    found_cat=0,
    found_version=0,
    found_oldhelp = 0,
    found_proc_in_proc = 0;

const char *yylp_errlist[]= {
   "",
   "missing close bracket ')' for proc definition in line %d.",  /*  1 */
   "missing close bracket ')' for procbody in line %d.",         /*  2 */
   "missing close bracket ']' for procbody in line %d.",         /*  3 */
   "too many ')' closed brackets in line %d.",                   /*  4 */
   "too many ']' closed brackets in line %d.",                   /*  5 */
   "missing close bracket ')' for example in line %d.",          /*  6 */
   "missing close bracket ']' for example in line %d.",          /*  7 */
   "cannot assign character '%c' in line %d to any group.",      /*  8 */
   "there must be a quote missing somewhere before line %d.",    /*  9 */
   "missing close bracket '}' at end of library in line %d.",    /* 10 */
   "missing close bracket ')' at end of library in line %d.",    /* 11 */
   "missing close bracket ']' at end of library in line %d.",    /* 12 */
   NULL
};
int yylp_errno = 0;

#ifdef STANDALONE_PARSER
procinfov pi;
int category_out = 0;
void printpi(procinfov pi);
void pi_clear(procinfov pi);
extern "C" {
  int yylpwrap();
}
void main_init(int argc, char *argv[]);
void main_result(char *libname);
#else /* STANDALONE_PARSER */
idhdl h0;
idhdl h_top;
#define pi IDPROC(h0)
extern "C"
{
  int yylpwrap();
}
extern libstackv library_stack;
#endif /* STANDALONE_PARSER */

static unsigned long help_chksum;

#define SET_DEF_END(mode, pi, p) \
  if ( mode == LOAD_LIB) pi->data.s.def_end = p;
#define SET_HELP_START(mode, pi, p) \
  if ( mode == LOAD_LIB) {pi->data.s.help_start = p; help_chksum = 0;}
#define SET_HELP_END(mode, pi, p) \
  if ( mode == LOAD_LIB) {pi->data.s.help_end = p;  \
              pi->data.s.help_chksum = help_chksum;}

#define SET_BODY_START(mode, pi, l, p) \
     if ( mode == LOAD_LIB)            \
     {                                 \
       pi->data.s.body_lineno = l;     \
       pi->data.s.body_start = p;      \
     }
#define SET_BODY_END(mode, pi, p) \
     if ( mode == LOAD_LIB)       \
     {                            \
       pi->data.s.body_end = p-1; \
       pi->data.s.proc_end = p-1; \
     }

#define SET_EXAMPLE_START(mode, pi, l, p) \
   if ( mode == LOAD_LIB)                 \
   {                                      \
     pi->data.s.example_lineno = l;       \
     pi->data.s.example_start = p;        \
   }
#define SET_PROC_END(mode, pi, p)   \
     if ( mode == LOAD_LIB)         \
     {                              \
       pi->data.s.proc_end = p-1;   \
       if(pi->data.s.body_end==0)   \
         pi->data.s.body_end = p-1; \
     }

#define ROTATE_RIGHT(c) if ((c) & 01) (c) = ((c) >>1) + 0x8000; else (c) >>= 1;
#define IncrCheckSum(c)                          \
do                                            \
{                                             \
  ROTATE_RIGHT(help_chksum);                  \
  help_chksum += c;                           \
  help_chksum &= 0xffff;                      \
}                                             \
while(0)

#undef YY_DECL
#define YY_DECL int yylex(const char *newlib, const char *libfile, \
                           lib_style_types *lib_style, \
                           idhdl pl, BOOLEAN autoexport, lp_modes mode)
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
          if ( ((result = libread( (yyin), (char *) buf, max_size )) < 0 ) \
                  && ferror( yyin ) ) \
                YY_FATAL_ERROR( "read in flex scanner failed" );

#define YY_USER_INIT { \
       BEGIN(header); \
       yylplineno = 1; \
       yylp_errno = 0; \
       *lib_style = OLD_LIBSTYLE; \
       strcpy(libnamebuf,"(**unknown version**)"); \
     }

#if 0
<pbody>proc[ \t]+{name}  {
                           printf("MISSING: PROC-cmd found. ERROR!\n"); }
<pbody>example[ \t]*\n   {
                           yylplineno++;
                           printf("MISSING: EXAMPLE-cmd found. ERROR!\n"); }
info=+"\"" {
#endif

%}

digit          [0-9]
letter         [@a-zA-Z\']
name           ({letter}({letter}*{digit}*_*)*|_)
varname        ({letter}({letter}*{digit}*_*\(\))*|_|#)
letters        ({letter}|{digit}|[_./#%^*:,])
string         ({letters}*)
comment        [\/][\/]
dolar          [$]
nls            [\n\r]
symbols        [~!@#$%^&*()_+-=\\\|\[\];:,<.>/\?\' \t\~\`]
asymbols       ({symbols}|[{}])
bsymbols       ({symbols}|{escbrack}|[}])
aletters       ({letter}|{digit}|{asymbols}|{dolar}|{escquote}|{nls})
bletters       ({letter}|{digit}|{bsymbols}|{dolar}|{quote}|{nls})
cletters       ({letter}|{digit}|{asymbols}|{dolar}|{quote})
strings        ({aletters}*)
escstrings     ({bletters}*)
fstring        ({cletters}*)
param          ({name}+{tos}+{varname})
parameters     ({param}(,{param})*)
paramlist      ("("{parameters}")")
quote          [\"]
escquote       (\\\")
escbrack       (\\\{)
tnl            ([ \t\n\r]*)
eos            ({quote}+{tnl}+"{")
tos            ([ \t]*)
eq             ([ \t]*+=[ \t]*)
eqnl           ([ \t\n\r]*+=[ \t\n\r]*)

/* %start START */

%x header
%x help
%x libcmd
%x libcmd2
%x pdef
%x phead
%x poldhelp
%x phelp
%x pbody
%x pstr
%x pexample
%x pestr
%x string
%x comment
%x info
%x category
%x url
%x version

%%
(\/\/[^\n]*)|(^#![^\n]*)|([ \t]) { }
\/\/*      { old_state = YYSTATE; BEGIN(comment); }

(info+{eqnl}+{quote}+{strings}+{quote}) {
         yyless(4); old_state = YYSTATE; BEGIN(info);
       }
(category+{eqnl}+{quote}+{strings}+{quote}) {
         yyless(8); old_state = YYSTATE; BEGIN(category);
       }
(url+{eqnl}+{quote}+{strings}+{quote}) {
             if ( mode != GET_INFO )
             {
               #ifdef STANDALONE_PARSER
               if (texinfo_out)
               {
                 char *c = yytext;
                 printf("$url = \"");
                 while ((*c != '\0') && (*c != '"')) c++;
                 c++;
                 while ((*c != '\0') && (*c != '"'))
                 {
                    if (*c != '\r') putchar(*c);
                    c++;
                 }
                 printf("\";\n");
               }
               #endif
             }
       }

(version+{eqnl}+{quote}+{strings}+{quote}) {
             found_version++;
             if ( mode != GET_INFO )
             {
               make_version(yytext,1);
               #ifdef STANDALONE_PARSER
               if (texinfo_out)
               {
                 char *c = libnamebuf;
                 printf("$version = \"");
                 while (*c != '\0')
                 {
                    if (*c  == '$' || *c  == '@') putchar('\\');
                    if (*c != '\r') putchar(*c);
                    if (*c  == '\\')
                    {
                      c++;
                      if (*c != '"') putchar('\\');
                    }
                    else
                      c++;
                 }
                 printf("\";\n");
               }
               else if (!category_out)
                 printf("Version:%s;\n", libnamebuf);
               #else
               if (text_buffer!=NULL) omFree((ADDRESS)text_buffer);
               text_buffer = omStrDup(libnamebuf);
               omMarkAsStaticAddr(text_buffer);
               #endif
             }
           }

static     { p_static=TRUE; }

(proc[ \t]+{name})|([ \t]proc[ \t]+{name}) {
             char proc[256];
             BEGIN(pdef);
             found_proc_in_proc = 0;
             proc[0]='\0';
             sscanf( yytext, "%*[^p]proc %s", proc);
             if(strlen(proc)<1) sscanf( yytext, "proc %s", proc);
             #if YYLPDEBUG > 1
             printf("Newlib:%s\n", newlib);
             #endif
             #ifdef STANDALONE_PARSER
             if ( pi != NULL )
             {
               printpi(pi);
               pi_clear(pi);
             }
             pi = (procinfo *)malloc(sizeof(procinfo));
             iiInitSingularProcinfo(pi, newlib, proc, yylplineno,
                                    current_pos(0), p_static);
             #else /*STANDALONE_PARSER*/
             if( mode == LOAD_LIB)
             {
               h0 = enterid( proc, 0 /*myynest*/, PROC_CMD,
                                 &(IDPACKAGE(pl)->idroot), TRUE, !p_static);
               if (h0==NULL) return(1);
               if((!p_static) && autoexport)
               {
                  package save=currPack;
                  currPack=basePack;
                  h_top = enterid( proc, 0 /*myynest*/, PROC_CMD,
                                 &(basePack->idroot), FALSE );
                  currPack=save;
                  if (h_top==NULL) return(1);
               }
               /* omCheckAddr(IDID(h0)); */
               if (h0!=NULL)
               {
                 iiInitSingularProcinfo(IDPROC(h0), newlib, proc,
                                yylplineno, current_pos(0),p_static);
                 if ((!p_static) && (h_top != NULL) && autoexport)
                 {
                   if(IDPROC(h_top)!=NULL) piKill((procinfo *)IDPROC(h_top));
                   IDPROC(h_top)=IDPROC(h0);
                   IDPROC(h_top)->ref++;
                 }
                 IDPROC(h0)->pack=IDPACKAGE(pl);
                 if (BVERBOSE(V_LOAD_PROC))
                   Warn( "     proc '%s' registered", proc );
               }
               #endif /*STANDALONE_PARSER*/
               SET_DEF_END(mode, pi, current_pos(yyleng+1));
               #if YYLPDEBUG
               if(lpverbose)
               {
                  printf("// PROCEDURE '%s' status: %s, ", proc,
                      p_static ? "local" : "global");
                  printf("starting at line %d,%d: definition end: %d (%d).\n",
                      yylplineno, current_pos(0), (int)pi->data.s.def_end, brace1);
               }
               #endif
               p_static=FALSE;
             #ifndef STANDALONE_PARSER
             }
             #endif /*STANDALONE_PARSER*/
           }
example    {
             BEGIN(pexample);
             SET_EXAMPLE_START(mode, pi, yylplineno, current_pos(0));
             #if YYLPDEBUG
             if(lpverbose)
             {
                printf("//     EXAMPLE at line %d,%d (%d)\n", yylplineno,
                    current_pos(0), brace1);
             }
             #endif
           }

LIB[ \t]+"\"" { quote++;
             BEGIN(libcmd);
           }

LIB[ \t]*"(\"" { quote++; brace2++;
             BEGIN(libcmd2);
           }

<header>({comment}+{tos}+{dolar}+Id:+{string}+[^\n]*)|({comment}+{tos}+{dolar}+Header:+{string}+[^\n]*) {
             make_version(yytext, 0);
             #if YYLPDEBUG > 1
             printf("+(id)HEAD:%s\n", yytext);
             #endif
           }
<header>(^{comment}+[^\n]*) {
             #if YYLPDEBUG
             printf("+(cmt)HEAD:%s\n", yytext);
             #endif
           }
<header>(^#![^\n]*) {
             #if YYLPDEBUG > 1
             printf("-HEAD:%s\n", yytext);
             #endif
           }
<header>^proc\  { yyless(0);
             BEGIN(INITIAL);
             yymore();
           }
<header>(info+{eqnl}+{quote})|(version+{eqnl}+{quote})|(category+{eqnl}+{quote})|(url+{eqnl}+{quote}) {
             yyless(0);
             *lib_style = NEW_LIBSTYLE;
             BEGIN(INITIAL);
             yymore();
           }

<header>^LIB[ \t]+"\""   { quote++;
             BEGIN(libcmd);
           }
<header>^LIB[ \t]+"(\""   { quote++; brace2++;
             BEGIN(libcmd2);
           }
<header>\n { yylplineno++; }
<header>.  {
             #if YYLPDEBUG > 1
             printf(" HEAD:%s\n", yytext);
             #endif
             yyless(0);
             BEGIN(help);
           }
<help>(^{comment}+[^\n]*)  {
             #if YYLPDEBUG > 1
             printf(" HELP:%s\n", yytext);
             #endif
             BEGIN(INITIAL); }
<help>(^#![^\n]*) {
             #if YYLPDEBUG > 1
             printf(" HELP:%s\n", yytext);
             #endif
             BEGIN(INITIAL);
           }
<help>(info+{eqnl}+{quote})|(version+{eqnl}+{quote})|(category+{eqnl}+{quote})|(url+{eqnl}+{quote}) {
             yyless(0);
             *lib_style = NEW_LIBSTYLE;
             BEGIN(INITIAL);
             yymore();
           }
<help>^proc\  {
             yyless(0);
             //printf("2) proc found.\n");
             BEGIN(INITIAL);
             yymore();
           }
<help>^LIB[ \t]+"\""     { quote++;
             BEGIN(libcmd);
           }
<help>^LIB[ \t]+"(\""     { quote++; brace2++;
             BEGIN(libcmd2);
           }

<help>\n { yylplineno++; }
<help>({tos}|{comment}+{fstring}) {
             #if YYLPDEBUG
             if(lpverbose>2) printf("--->%s<---\n", yytext);
             #endif
           }
<help>.    {
             found_oldhelp=1;
             #if YYLPDEBUG > 1
             printf("-HELP:%s\n", yytext);
             #endif
           }


<libcmd>{string}"\""     { quote--;
             yytext[yyleng-1] = '\0';
             #ifndef STANDALONE_PARSER
             if ( mode == LOAD_LIB )
             {
               library_stack->push(newlib, yytext);
             }
             #endif /* STANDALONE_PARSER */
             #if YYLPDEBUG
             if(lpverbose>1) printf("LIB:'%s'\n", yytext);
             #endif
             BEGIN(INITIAL);
           }
<libcmd2>{string}"\")"     { quote--; brace2--;
             yytext[yyleng-1] = '\0';
             #ifndef STANDALONE_PARSER
             if ( mode == LOAD_LIB )
             {
               library_stack->push(newlib, yytext);
             }
             #endif /* STANDALONE_PARSER */
             #if YYLPDEBUG
             if(lpverbose>1) printf("LIB:'%s'\n", yytext);
             #endif
             BEGIN(INITIAL);
           }

<pdef>[ \t] { }
<pdef>\(   {
             brace2++;
             #if YYLPDEBUG > 1
             printf("%s", yytext);
             #endif
           }
<pdef>\)   {
             brace2--;
             #if YYLPDEBUG > 1
             printf(">%s<\n", yytext);
             printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);
             #endif
             if(brace2<=0)
             {
               #if YYLPDEBUG > 1
               printf("BEGIN(phead){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
               #endif
               SET_DEF_END(mode, pi, current_pos(yyleng));
               BEGIN(phead);
             }
           }
<pdef>"{"  {
             if(brace2>0)
             {
               #if YYLPDEBUG > 1
               printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);
               #endif
               yylp_errno = YYLP_DEF_BR2;
               return(1);
             }
             else
             {
               brace1++; BEGIN(pbody);
               if(lpverbose)
                  printf("//     BODY at line %d,%d (%d)\n", yylplineno,
                      current_pos(0), brace1);
               SET_BODY_START(mode, pi, yylplineno, current_pos(0));
             }
           }
<pdef>\n { yylplineno++;
              if(brace2<=0)
              {
#if YYLPDEBUG > 1
                printf("BEGIN(phead-2){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                BEGIN(phead);
              }
            }
<pdef>({comment}[^\n]*)  { }
<pdef>\/\/*      { old_state = YYSTATE; BEGIN(comment); }
<pdef>.    {
             if(brace2<=0)
             {
               BEGIN(phead);
               yyless(0);
             }
           }

<phead>({tnl}+{quote}+{strings}+{escquote}+{tnl}+"{") {
              #if YYLPDEBUG
              if(lpverbose>2)printf("0-Len=%d;\n", yyleng);
              #endif
              if(check)
              {
                printf("Procedure %s has OLD-STYLE-HELP!\n",
                       pi->procname);
              }
              SET_HELP_START(mode, pi, current_pos(0));
              BEGIN(poldhelp);
              yyless(0);
           }
<phead>({tnl}+{quote}+{strings}+{eos}) {
              #if YYLPDEBUG
              if(lpverbose>2)printf("1-Len=%d;\n", yyleng);
              #endif
              BEGIN(phelp);
              yyless(0);
           }
<phead>{escstrings}+"{" {
              if(check && yyleng>2)
              {
                printf("Procedure %s has OLD-STYLE-HELP!\n",
                       pi->procname);
              }
              #if YYLPDEBUG
              if(lpverbose>2 && yyleng>2)
                 printf("2-Len=%d, %s;\n", yyleng, pi->procname);
              #endif
              SET_HELP_START(mode, pi, current_pos(0));
              BEGIN(poldhelp);
              yyless(0);
           }
<phead>.   { printf("[%s]", yytext); }

<poldhelp>{escbrack} { }
<poldhelp>"{" {
                SET_HELP_END(mode, pi, current_pos(0));
                brace1++; BEGIN(pbody);
                if(lpverbose)
                {
                   printf("//     HELP from %d to %d\n",
                       (int)pi->data.s.help_start, (int)pi->data.s.help_end);
                   printf("//     BODY at line %d,%d (%d)\n", yylplineno,
                       current_pos(0), brace1);
                }
#if YYLPDEBUG > 1
                printf("BEGIN(pbody){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                SET_BODY_START(mode, pi, yylplineno, current_pos(0));
#if YYLPDEBUG > 1
                printf("BODY at %d/%d", yylplineno, current_pos(0));
#endif
              }
<poldhelp>\n  { yylplineno++; }
<poldhelp>.   { }

<phelp>{quote} {
             old_state = YYSTATE;
             BEGIN(string);
             SET_HELP_START(mode, pi, current_pos(1));
           }
<phelp>{tos} {}
<phelp>"{" {
             brace1++; BEGIN(pbody);
             if(lpverbose)
             {
                printf("//     HELP from %d to %d\n",
                    (int)pi->data.s.help_start, (int)pi->data.s.help_end);
                printf("//     BODY at line %d,%d (%d)\n", yylplineno,
                    current_pos(0), brace1);
             }
             #if YYLPDEBUG > 1
             printf("BEGIN(pbody){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
             #endif
             SET_BODY_START(mode, pi, yylplineno, current_pos(0));
             #if YYLPDEBUG > 1
             printf("BODY at %d/%d", yylplineno, current_pos(0));
             #endif
           }
<phelp>\n  { yylplineno++;}

<pbody>({comment}[^\n]*) { }
<pbody>{quote} { quote++; old_state = YYSTATE;
                 BEGIN(string); /* printf("%s", yytext); */
               }

<pbody>proc+{tos}+{name}+{tnl}+{paramlist}+{tnl}+"{" {
             if(check) printf("*** found 2 proc whithin procedure '%s'.\n",
                          pi->procname);
             yyless(yyleng-1);
           }
<pbody>proc+{tos}+{name}+{tnl}+"{" {
             if(check) printf("*** found 1 proc whithin procedure '%s'.\n",
                          pi->procname);
             yyless(yyleng-1);
           }
<pbody>"{"     {
                 brace1++;
                 #if YYLPDEBUG > 1
                 printf("line: %d, (%d)%s\n", yylplineno, brace1, yytext);
                 #endif
                }
<pbody>"}"               {
                           #if YYLPDEBUG > 1
                           printf("line: %d, (%d)%s\n",
                             yylplineno, brace1, yytext);
                           #endif
                           brace1--;
                           if(brace2>0)
                           {
                             yylp_errno = YYLP_BODY_BR2;
                             return(1);
                           }
                           if(brace3>0)
                           {
                             yylp_errno = YYLP_BODY_BR3;
                             return(1);
                           }
                           if(brace1<=0)
                           {
                             SET_BODY_END(mode, pi, current_pos(yyleng));
                             SET_PROC_END(mode, pi, current_pos(yyleng));
                             #if YYLPDEBUG > 1
                               printf("-%d\n", current_pos(0));
                             #endif
                             BEGIN(INITIAL);
                           }
                         }
<pbody>"("               {
                           brace2++; /* printf("%s", yytext); */
                         }
<pbody>")"               {
                           brace2--; /* printf("%s", yytext); */
                           if(brace2<0) {
                             yylp_errno = YYLP_BODY_TMBR2;
                             return(1);
                           }
                         }
<pbody>"["               {
                           brace3++; /* printf("%s", yytext); */
                         }
<pbody>"]"               {
                           brace3--; /* printf("%s", yytext); */
                           if(brace3<0) {
                             yylp_errno = YYLP_BODY_TMBR3;
                             return(1);
                           }
                         }
<pbody>\n                { yylplineno++; }
<pbody>.                 { }

<info>{quote} {
             quote++; BEGIN(string);
             found_info++;
             string_start = current_pos(yyleng);
             *lib_style = NEW_LIBSTYLE;
             last_cmd = LP_INFO;
       }
<info>\n { yylplineno++; }
<info>.  { }

<category>{quote} {
             quote++; BEGIN(string);
             found_cat++;
             string_start = current_pos(yyleng);
             *lib_style = NEW_LIBSTYLE;
             last_cmd = LP_CATEGORY;
       }
<category>\n { yylplineno++; }
<category>.  { }


<string>"\""             { quote--;
                           copy_string(mode);
                           last_cmd = LP_NONE;
                           if(old_state==phelp)
                           {
                              SET_HELP_END(mode, pi, current_pos(0));
                           }
                           BEGIN(old_state); /* printf("%s", yytext); */
                         }
<string>(\\\\)|(\\\")    { if (old_state == phelp) IncrCheckSum(*yytext);}
<string>\n               { yylplineno++; if (old_state == phelp) IncrCheckSum('\n');}
<string>.                { if (old_state == phelp) IncrCheckSum(*yytext);}

<pexample>(\/\/[^\n]*)  { }
<pexample>"\""           { quote++; old_state = YYSTATE;
                           BEGIN(string); /* printf("%s", yytext); */
                         }
<pexample>"{"            {
                           brace1++; /* printf("(%d)%s", brace1, yytext); */
                         }
<pexample>"}"            {
                           brace1--; /* printf("(%d)%s", brace1, yytext); */
                           if(brace1<=0) {
                             if(brace2>0) { yylp_errno=YYLP_EX_BR2; return(1); }
                             if(brace3>0) { yylp_errno=YYLP_EX_BR3; return(1); }
                             BEGIN(INITIAL);
                             SET_PROC_END(mode, pi, current_pos(yyleng));
                           }
                         }
<pexample>"("            {
                           brace2++; /* printf("%s", yytext); */
                         }
<pexample>")"            {
                           brace2--; /* printf("%s", yytext); */
                         }
<pexample>"["            {
                           brace3++; /* printf("%s", yytext); */
                         }
<pexample>"]"            {
                           brace3--; /* printf("%s", yytext); */
                         }
<pexample>\n             { yylplineno++; }
<pexample>.              { }

<pestr>"\""              { quote--;
                           BEGIN(pexample); /* printf("%s", yytext); */
                         }
<pestr>\\\\              { }
<pestr>\\\"              { }
<pestr>\n                { yylplineno++; }
<pestr>.                 { }

<comment>\*\/            { BEGIN(old_state); }
<comment>\n              { yylplineno++; }
<comment>.               { }

\n                       { yylplineno++; }
\r                       { }
;                        { p_static = FALSE;
                            #if YYLPDEBUG > 1
                            printf("%s", yytext);
                            #endif
                         }
.                        { p_static = FALSE;
                           yylp_errno = YYLP_BAD_CHAR;
                           #ifdef STANDALONE_PARSER
                           printf("[%d]", *yytext);
                           #else
                           if (text_buffer!=NULL) omFree((ADDRESS)text_buffer);
                           text_buffer = omStrDup(yytext);
                           omMarkAsStaticAddr(text_buffer);
                           #endif
                           #if YYLPDEBUG > 1
                             printf("[%s]", yytext);
                           #endif
                           return(1);
                         }

%%

int current_pos(int i)
{
  return(i+offset+(int)(yytext-yylp_buffer_start));
}

int libread(FILE* f, char* buf, int max_size)
{ int rc;

  offset = ftell(f);
  rc  = myfread( buf, 1, max_size, f );
  #if YYLPDEBUG >2
    printf("fread: %d of %d\n", rc, max_size);
  #endif
  yylp_buffer_start = buf;
  return rc;
}

extern "C" {
  int yylpwrap()
  {
    //printf("======================= YYWRAP ====================\n");
    if(brace1>0) { yylp_errno=YYLP_MISS_BR1; }
    if(brace2>0) { yylp_errno=YYLP_MISS_BR2; }
    if(brace3>0) { yylp_errno=YYLP_MISS_BR3; }
    if(quote>0) { yylp_errno=YYLP_MISSQUOT; }
    //printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);
    if(feof(yyin)) return 1; else return 0;
  }
}

void reinit_yylp()
{
   brace1 = 0;
   brace2 = 0;
   brace3 = 0;
   quote  = 0;
   yy_init=1;
   yy_delete_buffer(YY_CURRENT_BUFFER);
}

void make_version(char *p,int what)
{
  char ver[11];
  char date[17];
  ver[0]='?'; ver[1]='.'; ver[2]='?'; ver[3]='\0';
  date[0]='?'; date[1]='\0';
  if(what) sscanf(p,"%*[^=]= %*s %*s %10s %16s",ver,date);
  else sscanf(p,"// %*s %*s %10s %16s",ver,date);
  strcpy(libnamebuf,"(");
  strcat(libnamebuf,ver);
  strcat(libnamebuf,",");
  strcat(libnamebuf,date);
  strcat(libnamebuf,")");
  if(what && strcmp(libnamebuf, "(?.?,?)")==0)
  {
    sscanf(p,"%*[^\"]\"%[^\"]\"",libnamebuf);
  }
  //printf("ID=(%d)%s; \n", what, p);
}

void copy_string(lp_modes mode)
{
#ifdef STANDALONE_PARSER
  if ((texinfo_out
     && (last_cmd == LP_INFO || last_cmd == LP_CATEGORY || last_cmd == LP_URL))
  || (category_out && last_cmd == LP_CATEGORY)
)
  {
    long current_location = ftell(yylpin), i = string_start, quote = 0;
    char c;
    if (texinfo_out)
    {
     if (last_cmd == LP_INFO)
     {
       printf("$info = <<EOT;\n");
     }
     else if (last_cmd == LP_URL)
     {
       printf("$url = <<EOT;\n");
     }
     else
     {
       printf("$category = <<EOT;\n");
     }
    }
    fseek (yylpin, i, SEEK_SET);
    while (i< current_location)
    {
      c = fgetc(yylpin);
      if (c == '\\')
      {
        quote = (! quote);
      }
      else if (c == '"')
      {
        if (! quote) break;
      }
      else
        quote = 0;
      if (c == '@' || c == '$') putchar('\\');
      if (c != '\r') putchar(c);
      i++;
    }
    if (category_out) exit(0);
    fseek (yylpin, current_location, SEEK_SET);
    printf("\nEOT\n");
  }
#else
  if((last_cmd == LP_INFO)&&(mode == GET_INFO))
  {
    int i, offset=0;
    long current_location = ftell(yylpin);
    int len = (int)(current_pos(0) - string_start);
    fseek(yylpin, string_start, SEEK_SET);
    if (text_buffer!=NULL) omFree((ADDRESS)text_buffer);
    text_buffer = (char *)omAlloc(len+2);
    omMarkAsStaticAddr(text_buffer);
    myfread(text_buffer, len, 1, yylpin);
    fseek(yylpin, current_location, SEEK_SET);
    text_buffer[len]='\0';
    offset=0;
    for(i=0;i<=len; i++)
    {
      if(text_buffer[i]=='\\' &&
         (text_buffer[i+1]=='\"' || text_buffer[i+1]=='{' ||
          text_buffer[i+1]=='}' || text_buffer[i+1]=='\\'))
      {
        i++;
        offset++;
      }
      if(offset>0) text_buffer[i-offset] = text_buffer[i];
    }
  }
#endif /* STANDALONE_PARSER */
}

void print_init()
{
   printf("Init=%d\n", yy_init);
}

void print_version(lp_modes mode, char *p)
{
#ifdef STANDALONE_PARSER
  //printf("loading %s%s", p, libnamebuf);
#else
  if ( mode == LOAD_LIB )
  {
    if (BVERBOSE(V_LOAD_LIB) && p!=NULL ) Print(" %s...", p);
       //Warn( "loading %s%s", p, libnamebuf);
  }
#endif
}

#ifdef STANDALONE_PARSER
int main( int argc, char *argv[] )
{
  lib_style_types lib_style;
  main_init(argc, argv);
  if(yyin == NULL)
  {
    fprintf(stderr, "No library found to parse.\n");
    return 1;
  }
  if (! (texinfo_out || category_out))
  {
    if(lpverbose)printf("Verbose level=%d\n", lpverbose);
    if(check)printf("Reporting most possible annomalies.\n");
    if(lpverbose||check)printf("\n");

    printf( "  %-15s  %20s      %s,%s    %s,%s     %s,%s\n", "Library",
            "function", "line", "start-eod", "line", "body-eob",
            "line", "example-eoe");
  }
  yylplex(argv[0], argv[0], &lib_style,NULL);
  if(yylp_errno)
  {
    printf("ERROR occurred: [%d] ", yylp_errno);
    printf(yylp_errlist[yylp_errno], yylplineno);
    printf("\n");
  }
  else if(pi!=NULL) printpi(pi);
  if (texinfo_out)
    printf("1;");
  return 0;
}

#endif /* STANDALONE_PARSER */
#endif /* HAVE_LIBPARSE */
