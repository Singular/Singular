%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: scanner.l,v 1.12 1998-04-07 08:30:32 Singular Exp $ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "mod2.h"
#include "tok.h"
#include "stype.h"
#include "ipshell.h"
#include "mmemory.h"
#include "febase.h"

int feReadLine(char* b, int l);
#define ALLOC(a) Alloc((a))
int yylineno  = 0;
static int blocknest = 0;
extern char * yytext;
//extern unsigned char * yytext;
extern int yyleng;
extern BOOLEAN noringvars;
extern int inerror;

static char * dupyytext()
{
  //int i = strlen((char *)yytext);
  //if (i>0) yytext[i-1] = '\0';
  if (yyleng>0) yytext[yyleng-1] = '\0';
  return mstrdup((char *)yytext);
}

static char * dupyytextNL()
{
  int i = yyleng;//strlen((char *)yytext);
  char * rc = (char*)AllocL( 3 + i );
  if (i>0)
  {
    yytext[i-1] = '\0';
    strcpy( rc, (char *)yytext );
  }
  else
    i++;
  rc[i-1] = '\n';
  rc[i] = '\n';
  rc[i+1] = '\0';
  return rc;
}

  #undef YY_DECL
  #define YY_DECL int yylex(YYSTYPE* lvalp)

  #undef yywrap
  extern "C" {
  int yywrap() { return exitVoice(); }
  }

  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) \
          result = feReadLine( (char *) (buf), (max_size) )

  #undef YY_USER_ACTION
  #define YY_USER_ACTION \
          if ((inerror==1)&&(*yytext>=' '))\
          { Print("   skipping text from `%s`",yytext);inerror=2; }

%}

digit          [0-9]
letter         [@a-zA-Z\']
integer        {digit}+
monom          {letter}+{digit}*
rgvars         ({digit}+[/])*{digit}+{monom}+
realnum        {digit}+"."{digit}+("e"[+-]{digit}+)?
name           ({letter}({letter}*{digit}*_*)*|_)
parname        #

/* %start START */
%option always-interactive

%x string
%x block
%x blockstr
%x brace
%x bracestr
%x bracket
%x asstring

%%
\/\/[^\n]*               { }
^#![^\n]*                { }
pause[ \t\n]*[\.;]       { fePause(); }
while                    { blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return WHILE_CMD;}
for                      { blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return FOR_CMD;}

("help"|"?")[ \t\n]*     { yy_noeof = noeof_asstring;
                           BEGIN(asstring);
                           return HELP_CMD;
                         }

proc[ \t]+{name}[ \t]*\( {
                           char c; char *cp;
                           lvalp->name = mstrdup(iiProcName((char *)yytext,c,cp));
                           yy_noeof = noeof_procname;
                           blocknest = 1;
                           BEGIN(brace);
                           return PROC_DEF;
                         }
<asstring>[^;\n]+        {
                           lvalp->name = mstrdup((char *)yytext);
                           yy_noeof = 0; BEGIN(INITIAL);
                           return STRINGTOK;
                         }
<asstring>;              {
                           yy_noeof = 0; BEGIN(INITIAL);
                           return *yytext;
                         }

<brace>"\""              {
                           yy_noeof = noeof_string;
                           BEGIN(bracestr);
                           yymore();
                         }
<brace>"("               { if (blocknest++) yymore(); }
<brace>[^;\(\)]          { if (blocknest) yymore(); }
<brace>";"               {
                           if (blocknest)
                           {
                             lvalp->name = dupyytext();
                             return STRINGTOK;
                           }
                         }
<brace>")"               {
                           if (--blocknest <= 0)
                           {
                             yy_noeof = 0;
                             BEGIN(INITIAL);
                             lvalp->name = dupyytext();
                             return STRINGTOK;
                           }
                           yymore();
                         }
<bracestr>"\""           {
                           yy_noeof = noeof_brace;
                           BEGIN(brace);
                           yymore();
                         }
<bracestr>[^\"]          { yymore(); }
<bracket>"("             { return '('; }
<bracket>","             { return ','; }
<bracket>[ \t\n]*        { ; }
<bracket>[^\(\), \t\n]*  {
                           lvalp->name = mstrdup((char *)yytext);
                           return STRINGTOK;
                         }
<bracket>\"[^\"]*\"      {
                           lvalp->name = mstrdup((char *)yytext);
                           return STRINGTOK;
                         }
<bracket>")"             {
                           yy_noeof = 0; BEGIN(INITIAL);
                           return ')';
                         }

"{"                      {
                           yy_blocklineno = yylineno;
                           blocknest = 1;
                           yy_noeof = noeof_block;
                           BEGIN(block);
                         }
<block>"\""              {
                           yy_noeof = noeof_string;
                           BEGIN(blockstr);
                           yymore();
                         }
<blockstr>[^\"]          { yymore(); }
<blockstr>"\\\\"         { yymore(); }
<blockstr>"\\\""         { yymore(); }
<blockstr>"\""           {
                           yy_noeof = noeof_block;
                           BEGIN(block);
                           yymore();
                         }
<block>[^\{\}\"]*        { yymore(); }
<block>\/\/[^\n]*        { yymore(); }
<block>"{"               { blocknest++; yymore(); }
<block>"}"               {
                           if (--blocknest <= 0)
                           {
                             BEGIN(INITIAL);
                             yy_noeof = 0;
                             lvalp->name = dupyytextNL();
                             return BLOCKTOK;
                           }
                           yymore();
                         }
"\""                     { BEGIN(string); yy_noeof = noeof_string;}
~                        { return SYS_BREAK; }
<string>[^\"]            { yymore(); }
<string>"\\\\"           { yymore(); }
<string>"\\\""           { yymore(); }
<string>"\""             {
                           char * s;
                           yy_noeof = 0;
                           BEGIN(INITIAL);
                           s = lvalp->name = dupyytext();
                           while (*yytext)
                           {
                             if (*yytext == '\\') yytext++;
                             *s++ = *yytext++;
                           }
                           *s++ = *yytext++;
                           return STRINGTOK;
                         }

[ \t\r\n]                /* skip whitespace */
".."                     { return DOTDOT; }
"::"                     { return COLONCOLON; }
"--"                     { return MINUSMINUS; }
"++"                     { return PLUSPLUS  ; }
"=="                     { return EQUAL_EQUAL; }
"&&"                     { return '&'; }
"||"                     { return '|'; }
"<="                     { return LE; }
">="                     { return GE; }
"!"                      { return NOT; }
"!="                     { return NOTEQUAL; }
"<>"                     { return NOTEQUAL; }
"**"                     { return '^'; }
\\                       { return '\\'; }
newline                  {
                           lvalp->name = mstrdup("\n");
                           return STRINGTOK;
                         }
{integer}                {
                           lvalp->name = (char *)yytext;
                           if ((currRing!=NULL)
                           && (strlen(lvalp->name)>MAX_INT_LEN))
                           {
                             lvalp->name = mstrdup((char *)yytext);
                             return RINGVAR;
                           }
                           else
                             return INT_CONST;
                         }
{integer}\/{integer}     {
                           lvalp->name = mstrdup((char *)yytext);
                           return RINGVAR;
                         }
\$                       {
                           #ifdef HAVE_TCL
                           if (tclmode)
                             PrintTCL('Q',0,NULL);
                           else
                           #endif
                           { if (BVERBOSE(0)) printf("\n$Bye.\n"); }
                           #ifndef macintosh
                             #ifdef HAVE_FEREAD
                               #ifdef HAVE_ATEXIT
                                 fe_reset_input_mode();
                               #else
                                 fe_reset_input_mode(0,NULL);
                               #endif
			     #else
			       #ifdef HAVE_READLINE
                                 fe_reset_input_mode();
			       #endif
                             #endif
                           #endif
                           #ifdef sun
                           #ifndef __svr4__
                           _cleanup();
                           _exit(0);
                           #endif
                           #endif
                           exit(0);
                         }
(quit|exit)[ \t\n]*[\.;]?  {
                           #ifdef HAVE_TCL
                             if (tclmode)
                               PrintTCL('Q',0,NULL);
                             else
                           #endif
                           #ifdef MM_STAT
                           mmStat(-500);
                           #endif
                           { if (BVERBOSE(0)) printf("\nAuf Wiedersehen.\n"); }
                           #ifndef macintosh
                             #ifdef HAVE_FEREAD
                               #ifdef HAVE_ATEXIT
                                 fe_reset_input_mode();
                               #else
                                 fe_reset_input_mode(0,NULL);
                               #endif
			     #else
			       #ifdef HAVE_READLINE
                                 fe_reset_input_mode();
			       #endif
                             #endif
                           #endif
                           #ifdef MDEBUG
                             #ifdef MLIST
                             mmTestList();
                             #endif
                           #endif
                           #ifdef sun
                           #ifndef __svr4__
                           _cleanup();
                           _exit(0);
                           #endif
                           #endif
                           exit(0);
                         }

{rgvars}|{realnum}       {
                           lvalp->name = mstrdup((char *)yytext);
                           return RINGVAR;
                         }

({parname}|{name})       {
                           /* {name} */
                           int rc=0;
                           if (yytext[strlen((char *)yytext)-1] == '\n')
                           {
                             yytext[strlen((char *)yytext)-1] = '\0';
                           }
                           if (yyleng > 1)
                           {
                             rc = IsCmd((char *)yytext,lvalp->i);
                             if (rc) return rc;
                           }
                           lvalp->name = mstrdup((char *)yytext);
                           return UNKNOWN_IDENT;
                         }

.                       {
                           /*if (*yytext == '\n') REJECT;*/
                           lvalp->i = *yytext;   /* token has own value */
                           return *yytext;
                         }
%%

void * myynewbuffer()
{
  void * oldb = yy_current_buffer;
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
  return oldb;
}

void myyoldbuffer(void * oldb)
{
  yy_delete_buffer(yy_current_buffer);
  yy_switch_to_buffer((YY_BUFFER_STATE)oldb);
  //yy_flush_buffer((YY_BUFFER_STATE)oldb);
}
