%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: scanner.l,v 1.4 1997-04-09 12:20:07 Singular Exp $ */
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

int mmread(FILE* f, char* b, int l);
#define ALLOC(a) Alloc((a))
int yylineno  = 0;
static int noeof = 0;
enum
{
  noeof_brace = 1,
  noeof_asstring,
  noeof_block,
  noeof_string,
  noeof_bracket,
  noeof_procname
};
static int blocknest = 0;
extern char * yytext;
//extern unsigned char * yytext;
extern int yyleng;
extern int inputswitch;
extern int noringvars;
extern inerror;

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
  int yywrap() { return exitVoice(); }

  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) \
          if ( (result = mmread( (yyin), (char *) buf, max_size )) < 0 ) \
              YY_FATAL_ERROR( "read() in flex scanner failed" );

  #undef YY_USER_ACTION
  #define YY_USER_ACTION \
          if (inerror==1) { Print("   skipping text from `%s`",yytext);inerror=2; }

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
while                    { blocknest = 0; noeof = noeof_brace; BEGIN(brace);
                           return WHILE_CMD;}
for                      { blocknest = 0; noeof = noeof_brace; BEGIN(brace);
                           return FOR_CMD;}

("help"|"?")[ \t\n]*     { noeof = noeof_asstring;
                           BEGIN(asstring);
                           return HELP_CMD;
                         }

example[ \t\n]*          { noeof = noeof_asstring;
                           BEGIN(asstring);
                           return EXAMPLE_CMD;
                         }

proc[ \t]+{name}[ \t]*\( {
                           char c; char *cp;
                           lvalp->name = mstrdup(iiProcName((char *)yytext,c,cp));
                           noeof = noeof_procname;
                           blocknest = 1;
                           BEGIN(brace);
                           return PROC_DEF;
                         }
<asstring>[^;\n]+        {
                           lvalp->name = mstrdup((char *)yytext);
                           noeof = 0; BEGIN(INITIAL);
                           return STRINGTOK;
                         }
<asstring>;              {
                           noeof = 0; BEGIN(INITIAL);
                           return *yytext;
                         }

<brace>"\""              {
                           noeof = noeof_string;
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
                             noeof = 0;
                             BEGIN(INITIAL);
                             lvalp->name = dupyytext();
                             return STRINGTOK;
                           }
                           yymore();
                         }
<bracestr>"\""           {
                           noeof = noeof_brace;
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
                           noeof = 0; BEGIN(INITIAL);
                           return ')';
                         }

"{"                      {
                           blocklineno = yylineno;
                           blocknest = 1;
                           noeof = noeof_block;
                           BEGIN(block);
                         }
<block>"\""              {
                           noeof = noeof_string;
                           BEGIN(blockstr);
                           yymore();
                         }
<blockstr>[^\"]          { yymore(); }
<blockstr>"\\\\"         { yymore(); }
<blockstr>"\\\""         { yymore(); }
<blockstr>"\""           {
                           noeof = noeof_block;
                           BEGIN(block);
                           yymore();
                         }
<block>[^\{\}\"]*        { yymore(); }
<block>"{"               { blocknest++; yymore(); }
<block>"}"               {
                           if (--blocknest <= 0)
                           {
                             BEGIN(INITIAL);
                             noeof = 0;
                             lvalp->name = dupyytextNL();
                             return BLOCKTOK;
                           }
                           yymore();
                         }
"\""                     { BEGIN(string); noeof = noeof_string;}
~                        { return SYS_BREAK; }
<string>[^\"]            { yymore(); }
<string>"\\\\"           { yymore(); }
<string>"\\\""           { yymore(); }
<string>"\""             {
                           char * s;
                           noeof = 0;
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
                           && (strlen(lvalp->name)>=MAX_INT_LEN-1))
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
\$[\.;]?                 {
                           #ifdef HAVE_TCL
                           if (tclmode)
                             PrintTCL('Q',0,NULL);
                           else
                           #endif
                           { if (BVERBOSE(0)) printf("\n$Bye.\n"); }
                           #ifdef sun
                           #ifndef __svr4__
			   #ifdef HAVE_FEREAD
                           fe_reset_input_mode(0,NULL);
			   #endif
                           _cleanup();
                           _exit(0);
                           #endif
                           #endif
                           exit(0);
                         }
(quit|exitall)[ \t\n]*[\.;]?       {
                           #ifdef HAVE_TCL
                             if (tclmode)
                               PrintTCL('Q',0,NULL);
                             else
                           #endif
                           #ifdef MM_STAT
                           mmStat(-500);
                           #endif
                           { if (BVERBOSE(0)) printf("\nAuf Wiedersehen.\n"); }
                           #ifdef MDEBUG
                             #ifndef macintosh
			       #ifdef HAVE_FEREAD
                                 #ifdef HAVE_ATEXIT
                                   fe_reset_input_mode();
                                 #else
                                   fe_reset_input_mode(0,NULL);
                                 #endif
                               #endif
                             #endif
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

.                        {
                           /*if (*yytext == '\n') REJECT;*/
                           lvalp->i = *yytext;   /* token has own value */
                           return *yytext;
                         }
%%


int mmread(FILE* f, char* b, int l)
{
  int rc;
  b[0] = '\0';
  if (inputswitch <=0)
  {
    char* s;
    int offset=0;
    showInput();
NewRead:
    if (f==stdin)
      s = fe_fgets_stdin(b+offset,l-offset);
    else
      s = fgets(b+offset,l-offset,f);
    if (s==NULL)
    {
      if (noeof)
      {
        switch (noeof)
        {
          case noeof_brace:
          case noeof_block:
            WerrorS("premature end of file while reading {...}");
            break;
          case noeof_asstring:
            WerrorS("premature end of file while reading till `.`");
            break;
          case noeof_string:
            WerrorS("premature end of file while reading string");
            break;
          case noeof_bracket:
            WerrorS("premature end of file while reading (...)");
            break;
          case noeof_procname:
            WerrorS("premature end of file while reading proc");
            break;
        }
        exit(1);
      }
      return 0;
    }
    /*else*/
    {
      rc=strlen(s)-2;
      if (s[rc]=='\\')
      {
        s[rc]='\0';
        offset+=rc;
        if (offset<l) goto NewRead;
      }
    }
    if (feProt&PROT_I)
    {
      fputs(s,feProtFile);
    }
    rc=strlen(b);
  }
  else
  {
    rc=readbuf(b,l);
  }
  yylineno++;
  //if ((rc != strlen(b)) printf("++funny error found! (scanner:rc=%d, len=%d)\n",
  //  rc,(int)strlen(b));
  if (rc>1)
  {
    strncpy(my_yylinebuf,b,rc-1);
    if (my_yylinebuf[rc-2] == '\n') my_yylinebuf[rc-2] = '\0';
    my_yylinebuf[rc-1] = '\0';
    if (((si_echo>voice) && (strncmp(b,";RETURN();",10)!=0))
    || (traceit&TRACE_SHOW_LINE)
    || (traceit&TRACE_SHOW_LINE1))
    {
      printf("%s",b);
      mflush();
      if (traceit&TRACE_SHOW_LINE)
      {
        while(fgetc(stdin)!='\n');
      }
    }
    else if (traceit&TRACE_SHOW_LINENO)
    {
      printf("{%d}",yylineno);
      mflush();
    }
  }
  else
    my_yylinebuf[0] = '\0';
  return rc;
}

void * myynewbuffer()
{
  void * oldb = yy_current_buffer;
  yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
  return oldb;
}

void myyoldbuffer(void * oldb)
{
  yy_delete_buffer(yy_current_buffer);
  yy_switch_to_buffer((YY_BUFFER_STATE)oldb);
}
