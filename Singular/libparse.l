%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: libparse.l,v 1.3 1998-03-11 12:03:46 Singular Exp $ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef STANDALONE_PARSER
#  include "utils.h"
#  define HAVE_LIBPARSER
//#  define YYLPDEBUG 3
#else
#  include "mod2.h"
#  include "subexpr.h"
#  include "grammar.h"
#  include "ipshell.h"
#  include "ipid.h"
#  include "tok.h"
#  include "febase.h"
#  include "mmemory.h"
#  include "libparse.h"
#endif

#ifdef HAVE_LIBPARSER
#define YY_SKIP_YYWRAP

int libread(FILE* f, char* buf, int max_size);
int current_pos(int i);

int brace1 = 0;  /* { } */
int brace2 = 0;  /* ( ) */
int brace3 = 0;  /* [ ] */
int quote  = 0;  /* " */
int offset = 0;
int p_static = 0;
int old_state = 0;

char libnamebuf[128];

char *yylp_buffer_start;
int yylplineno = 1;

char *yylp_errlist[]= { 
   "",
   "missing close bracket ')' for proc definition in line %d.",  /*  1 */
   "missing close bracket ')' for procbody in line %d.",         /*  2 */
   "missing close bracket ']' for procbody in line %d.",         /*  3 */
   "too many ')' closed brackets in line %d.",                   /*  4 */
   "too many ']' closed brackets in line %d.",                   /*  5 */
   "missing close bracket ')' for example in line %d.",          /*  6 */
   "missing close bracket ']' for example in line %d.",          /*  7 */
   "cannot assign charater in line %d to any group.",            /*  8 */
   "there must be a quote missing somewhere before line %d.",    /*  9 */
   "missing close bracket '}' at end of library in line %d.",    /* 10 */
   "missing close bracket ')' at end of library in line %d.",    /* 11 */
   "missing close bracket ']' at end of library in line %d.",    /* 11 */
   NULL
};
int yylp_errno = 0;

#define YYLP_ERR_NONE    0
#define YYLP_DEF_BR2     1
#define YYLP_BODY_BR2    2
#define YYLP_BODY_BR3    3
#define YYLP_BODY_TMBR2  4
#define YYLP_BODY_TMBR3  5
#define YYLP_EX_BR2      6
#define YYLP_EX_BR3      7
#define YYLP_BAD_CHAR    8
#define YYLP_MISSQUOT    9
#define YYLP_MISS_BR1   10
#define YYLP_MISS_BR2   11
#define YYLP_MISS_BR3   12

print_version(char *p);

#ifdef STANDALONE_PARSER
procinfov pi;
procinfo *iiInitSingularProcinfo(procinfov pi, char *libname,
                                 char *procname, int line, long pos,
				 int pstatic = 0);
printpi(procinfov pi);
pi_clear(procinfov pi);
extern "C" {
  int yylpwrap();
}
#else /* STANDALONE_PARSER */
idhdl h;
#  define pi IDPROC(h)
extern "C"
{
  int yylpwrap();
}
extern libstackv library_stack;
#endif /* STANDALONE_PARSER */

#define SET_DEF_END(pi, p) \
   pi->data.s.def_end = p;
#define SET_HELP_START(pi, p) \
   pi->data.s.help_start = p; \

#define SET_BODY_START(pi, l, p) \
   { pi->data.s.body_lineno = l; \
     pi->data.s.body_start = p; \
   }
#define SET_BODY_END(pi, p) { \
   pi->data.s.body_end = p-1; \
   pi->data.s.proc_end = p-1; \
   }

#define SET_EXAMPLE_START(pi, l, p) \
   { pi->data.s.example_lineno = l; \
     pi->data.s.example_start = p; \
   }
#define SET_PROC_END(pi, p) \
   { pi->data.s.proc_end = p-1; \
     if(pi->data.s.body_end==0) pi->data.s.body_end = p-1; \
   }

  #undef YY_DECL
  #define YY_DECL int yylex(char *newlib, char *libfile)

  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) \
          if ( ((result = libread( (yyin), (char *) buf, max_size )) < 0 ) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "read in flex scanner failed" );

  #define YY_USER_INIT { \
       BEGIN(header); \
       yylplineno = 1; \
       yylp_errno = 0; \
       strcpy(libnamebuf,"(**unknown version**)"); \
     }

#if 0
<pbody>proc[ \t]+{name}  { 
                           printf("MISSING: PROC-cmd found. ERROR!\n"); }
<pbody>example[ \t]*\n   { 
                           yylplineno++;
                           printf("MISSING: EXAMPLE-cmd found. ERROR!\n"); }
#endif
%}

digit          [0-9]
letter         [@a-zA-Z\']
name           ({letter}({letter}*{digit}*_*)*|_)
letters        ({letter}|{digit}|[_./#%^*:,])
string         ({letters}*)
comment        [\/][\/]
dolar          [$]

/* %start START */

%x header
%x help
%x libcmd
%x pdef
%x phead
%x pbody
%x pstr
%x pexample
%x pestr
%x string
%x comment

%%
(\/\/[^\n]*)|(^#![^\n]*)|([ \t]) { }
\/\*                     { old_state = YYSTATE; BEGIN(comment); }

static                   { p_static=1; }

(proc[ \t]+{name})|([ \t]proc[ \t]+{name}) {
                           char proc[256];
                           BEGIN(pdef);
#if YYLPDEBUG
                           printf("Newlib:%s\n", newlib);
#endif
#ifdef STANDALONE_PARSER
                           if(pi!=NULL) {
                             printpi(pi);
                             pi_clear(pi);
                           }
                           pi = (procinfo *)malloc(sizeof(procinfo));
                           iiInitSingularProcinfo(pi, newlib, yytext+5,
						      yylplineno,
						      current_pos(0),
						      p_static);
#else STANDALONE_PARSER
                           proc[0]='\0';
                           sscanf( yytext, "%*[^p]proc %s", proc);
                           if(strlen(proc)<1) sscanf( yytext, "proc %s", proc);
                           h = enterid( mstrdup(proc), myynest, PROC_CMD,
					       &idroot, FALSE );
                           if (h!=NULL)
                           {
                             iiInitSingularProcinfo(IDPROC(h), newlib, proc,
					 yylplineno, current_pos(0),p_static);
                             if (BVERBOSE(V_LOAD_PROC))
                                 Warn( "     proc %s loaded", proc );
                           }
#endif STANDALONE_PARSER
                           SET_DEF_END(pi, current_pos(yyleng+1));
#if YYLPDEBUG
                           printf("PROC %d at %d/%d: (%d) %s\n", p_static,
				  yylplineno, current_pos(0), brace1, yytext);
#endif
                           p_static=0;
                         }
example                  {
                           BEGIN(pexample);
                           SET_EXAMPLE_START(pi, yylplineno, current_pos(0));
#if YYLPDEBUG
                           printf("EXAMPLE at %d/%d (%d)\n", yylplineno,
				  current_pos(0), brace1);
#endif
                         }

LIB[ \t]+"\""            { quote++;
                           BEGIN(libcmd);
                         }

<header>({comment}+[ \t]*+{dolar}+Id:+{string}+[^\n]*)|({comment}+[ \t]*+{dolar}+Header:+{string}+[^\n]*) {
                           char ver[10];
                           char date[16];
                           ver[0]='?'; ver[1]='.'; ver[2]='?'; ver[3]='\0';
                           date[0]='?'; date[1]='\0';
                           sscanf(yytext,"// %*s %*s %10s %16s",ver,date);
                           strcpy(libnamebuf,"(");
                           strcat(libnamebuf,ver);
                           strcat(libnamebuf,",");
                           strcat(libnamebuf,date);
                           strcat(libnamebuf,")");
                           //printf("ID=%s; \n", yytext);
                         }
<header>^{comment}+[^\n]* { 
#if YYLPDEBUG > 1
                            printf("+HEAD:%s\n", yytext);
#endif
                          }
<header>(^#![^\n]*)      { 
#if YYLPDEBUG > 1
                           printf("-HEAD:%s\n", yytext);
#endif
                         }
<header>^proc\           { yyless(0);
                           print_version(libfile);
                           BEGIN(INITIAL);
                           yymore();
                         }
<header>^LIB[ \t]+"\""   { quote++;
                           print_version(libfile);
                           BEGIN(libcmd);
                         }
<header>\n               { yylplineno++; }
<header>.                { 
#if YYLPDEBUG > 1
                           printf(" HEAD:%s\n", yytext);
#endif
                           print_version(libfile);
                           BEGIN(help);
                         }
<help>^{comment}+[^\n]*  { 
#if YYLPDEBUG > 1
                           printf(" HELP:%s\n", yytext);
#endif
                           BEGIN(INITIAL); }
<help>^#![^\n]*          { 
#if YYLPDEBUG > 1
                           printf(" HELP:%s\n", yytext);
#endif
                           BEGIN(INITIAL);
                         }
<help>^proc\             { yyless(0);
                           //printf("2) proc found.\n");
                           BEGIN(INITIAL);
                           yymore();
                         }
<help>^LIB[ \t]+"\""     { quote++;
                           BEGIN(libcmd);
                         }

<help>\n                 { yylplineno++; }
<help>.                  { 
#if YYLPDEBUG > 1
                           printf("-HELP:%s\n", yytext);
#endif
                         }


<libcmd>{string}"\""     { quote--;
                           yytext[yyleng-1] = '\0';
#ifndef STANDALONE_PARSER
                           library_stack->push(newlib, yytext);
#endif /* STANDALONE_PARSER */
#if YYLPDEBUG
                           printf("LIB:'%s'\n", yytext);
#endif
                           BEGIN(INITIAL);
                         }

<pdef>[ \t]              { }
<pdef>\(                 { 
                           brace2++;
#if YYLPDEBUG
                           printf("%s", yytext);
#endif
                         }
<pdef>\)                 { 
                           brace2--;
#if YYLPDEBUG
                           printf(">%s<\n", yytext);
                           printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                           if(brace2<=0) {
#if YYLPDEBUG
                             printf("BEGIN(phead){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                             SET_DEF_END(pi, current_pos(yyleng));
                             SET_HELP_START(pi, current_pos(yyleng));
                             BEGIN(phead);
                           }
                         }
<pdef>"{"                { 
                           if(brace2>0) {
#if YYLPDEBUG
                             printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                             yylp_errno = YYLP_DEF_BR2;
                             return(1);
                           } else { 
                             brace1++; BEGIN(pbody);
                             SET_BODY_START(pi, yylplineno, current_pos(0));
                           }
                         }
<pdef>\n                 { yylplineno++;
                           if(brace2<=0) { 
#if YYLPDEBUG
                             printf("BEGIN(phead-2){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                             SET_HELP_START(pi, current_pos(0));
                             BEGIN(phead);
                           }
                         }
<pdef>.                  { 
                           if(brace2<=0) { 
                             SET_HELP_START(pi, current_pos(0));
                             BEGIN(phead);
                           }
                         }

<phead>"\\{"             { }
<phead>"{"               {
                           brace1++; BEGIN(pbody);
#if YYLPDEBUG
                           printf("BEGIN(pbody){=%d, (=%d, [=%d\n", brace1, brace2, brace3);
#endif
                           SET_BODY_START(pi, yylplineno, current_pos(0));
#if YYLPDEBUG
                           printf("BODY at %d/%d", yylplineno, current_pos(0));
#endif
                         }
<phead>\n                { yylplineno++; }
<phead>.                 { }

<pbody>{comment}[^\n]*        { }
<pbody>"\""              { quote++; old_state = YYSTATE;
                           BEGIN(string); /* printf("%s", yytext); */
                         }
<pbody>"{"               { 
                           brace1++;
#if YYLPDEBUG
                           printf("line: %d, (%d)%s\n", yylplineno, brace1, yytext);
#endif
                         }
<pbody>"}"               { 
#if YYLPDEBUG
                           printf("line: %d, (%d)%s\n", yylplineno, brace1, yytext);
#endif
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
                             SET_BODY_END(pi, current_pos(yyleng));
                             SET_PROC_END(pi, current_pos(yyleng));
#if YYLPDEBUG
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

<string>"\""             { quote--;
                           BEGIN(old_state); /* printf("%s", yytext); */
                         }
<string>\\\\             { }
<string>\\\"             { }
<string>\n               { yylplineno++; }
<string>.                { }

<pexample>\/\/[^\n]*     { }
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
                             SET_PROC_END(pi, current_pos(yyleng));
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
;                        { p_static = 0;
#if YYLPDEBUG
                            printf("%s", yytext);
#endif
                         }
.                        { p_static = 0; 
                           yylp_errno = YYLP_BAD_CHAR;
#if YYLPDEBUG
                           printf("%s", yytext);
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
  rc  = fread( buf, 1, max_size, f );
  yylp_buffer_start = buf;
  return rc;
}

extern "C" {
  int yylpwrap() { 
    //printf("======================= YYWRAP ====================\n");
    if(brace1>0) { yylp_errno=YYLP_MISS_BR1; }
    if(brace2>0) { yylp_errno=YYLP_MISS_BR2; }
    if(brace3>0) { yylp_errno=YYLP_MISS_BR3; }
    if(quote>0) { yylp_errno=YYLP_MISSQUOT; }
    /* printf("{=%d, (=%d, [=%d\n", brace1, brace2, brace3);/**/
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
   yy_delete_buffer(yy_current_buffer);
}

void print_init()
{
   printf("Init=%d\n", yy_init);
}

print_version(char *p)
{
#ifdef STANDALONE_PARSER
   //printf("loading %s%s", p, libnamebuf);
#else
   if (BVERBOSE(V_LOAD_LIB))
      Warn( "loading %s%s", p, libnamebuf);
#endif
}

#ifdef STANDALONE_PARSER
main( int argc, char *argv[] )
{
  ++argv, --argc;  /* skip over program name */
  if ( argc > 0 )
     yyin = fopen( argv[0], "r" );
  else
     yyin = stdin;

  printf( "  %-15s  %20s      %s,%s    %s,%s     %s,%s\n", "Library",
	  "function", "line", "start-eod", "line", "body-eop",
	  "line", "example-eoe");
  yylplex(argv[0], argv[0]);
  if(yylp_errno) {
    printf("ERROR accured: ");
    printf(yylp_errlist[yylp_errno], yylplineno);
    printf("\n");
  } 
  else if(pi!=NULL) printpi(pi);
}

#endif /* STANDALONE_PARSER */
#endif /* HAVE_LIBPARSE */
