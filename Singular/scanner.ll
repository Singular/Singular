%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "Singular/tok.h"
#include "Singular/stype.h"
#include "Singular/ipshell.h"
#include "Singular/fevoices.h"
#include "kernel/oswrapper/feread.h"

int feReadLine(char* b, int l);
#define ALLOC(a) omAlloc((a))
#ifndef NEW_FLEX
#endif /* NEW_LEX */
int blocknest = 0;
extern char * yytext;
//extern unsigned char * yytext;
extern int yyleng;
extern int inerror;

#ifndef SING_NDEBUG
// this is to  shadow the malloc/realloc
// used by yy_flex_malloc/yy_flex_realloc
// so that we can mark stuff as static
static void* my_malloc(size_t size)
{
  void* addr = omAlloc(size);
  omMarkAsStaticAddr(addr);
  return addr;
}

static void* my_realloc(void* addr, size_t size)
{
  void* new_addr = omRealloc(addr, size);
  omMarkAsStaticAddr(new_addr);
  return new_addr;
}
#undef malloc
#define malloc my_malloc
#undef realloc
#define realloc my_realloc
#else
#undef malloc
#define malloc omAlloc
#undef realloc
#define realloc omRealloc
#endif
#undef free
#define free omFree
static char * dupyytext()
{
  char* s;
  if (yyleng>0) yytext[yyleng-1] = '\0';
  s = omStrDup((char *)yytext);
  omMarkAsStaticAddr(s);
  return s;
}

static char * dupyytextNL()
{
  int i = yyleng;//strlen((char *)yytext);
  char * rc = (char*)omAlloc( 3 + i );
  omMarkAsStaticAddr(rc);
  if (i>0)
  {
    strncpy( rc, (char *)yytext, i-1 );
  }
  else
  {
    i++;
  }
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
realnum        {digit}*"."{digit}+("e"[+-]{digit}+)?
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
\/\/[^\n]*
^[ \r\t\n]*#![^\n]*
"/*"                     {
                           yy_noeof=noeof_comment;
                           loop
                           {
                             REGISTER int c;
                             while ( (c = yyinput()) != '*' && c != EOF );
                             if ( c == '*' )
                             {
                               while ( (c = yyinput()) == '*' );
                               if ( c == '/' ) break; /* found the end */
                             }
                             else
                             {
                               break;
                             }
                           }
                           yy_noeof=0;
                         }
while                    { prompt_char='.';
                           blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return WHILE_CMD;}
for                      { prompt_char='.';
                           blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return FOR_CMD;}

("help"|"?")[ \t\n]*     { yy_noeof = noeof_asstring;
                           BEGIN(asstring);
                           return HELP_CMD;
                         }

example[ \t\n]*          { yy_noeof = noeof_asstring;
                           BEGIN(asstring);
                           return EXAMPLE_CMD;
                         }

proc[ \t]+{name}[ \t]*\( {
                           char c; char *cp;
                           lvalp->name = omStrDup(iiProcName((char *)yytext,c,cp));
                           yy_noeof = noeof_procname;
                           blocknest = 1;
                           BEGIN(brace);
                           return PROC_DEF;
                         }
<asstring>[^;\n]+        {
                           lvalp->name = omStrDup((char *)yytext);
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
                           lvalp->name = omStrDup((char *)yytext);
                           return STRINGTOK;
                         }
<bracket>\"[^\"]*\"      {
                           lvalp->name = omStrDup((char *)yytext);
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
"&&"                     { lvalp->i='&'; return LOGIC_OP; }
"||"                     { lvalp->i='|'; return LOGIC_OP; }
"<="                     { lvalp->i=LE; return COMP_OP; }
">="                     { lvalp->i=GE; return COMP_OP; }
"!"                      { return NOT; }
"!="                     { return NOTEQUAL; }
"<>"                     { return NOTEQUAL; }
"**"                     { return '^'; }
"->"                     { return ARROW; }
\\                       { return '\\'; }
newline                  {
                           lvalp->name = omStrDup("\n");
                           return STRINGTOK;
                         }
{integer}                {
                           lvalp->name = (char *)yytext;
                           return INT_CONST;
                         }
{integer}\/{integer}     {
                           lvalp->name = (char *)yytext;
                           return RINGVAR;
                         }
\$                        {
                           m2_end(-1);
                         }
(quit|exit)[ \t\n]*;     {
                           #ifdef MM_STAT
                           mmStat(-500);
                           #endif
                           #ifdef OM_TRACK
			   #ifndef SING_NDEBUG
                             omPrintUsedTrackAddrs(stdout, 10);
                           #endif
                           #endif
                           m2_end(0);
                         }

{rgvars}|{realnum}       {
                           lvalp->name = (char *)yytext;
                           return RINGVAR;
                         }
[0-9]+\."e"[+-][0-9]+    {
                           lvalp->name = (char *)yytext;
                           return RINGVAR;
                         }
[0-9]+\./[^\.]           {
                           lvalp->name = (char *)yytext;
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
                           lvalp->name = omStrDup((char *)yytext);
                           return UNKNOWN_IDENT;
                         }

.                       {
                           /*if (*yytext == '\n') REJECT;*/
                           REGISTER char ch= *yytext;
                           lvalp->i = ch;
                           switch(ch)
                           {
                             /* case '&': */
                             case '|':
                               return LOGIC_OP;
                             /* case '/': */
                             case '%':
                             case '*':
                               return MULDIV_OP;
                             /* case '<': */
                             case '>':
                               return COMP_OP;
                             default:
                               break;
                            }
                            return ch;
                         }
%%

void * myynewbuffer()
{
  void * oldb = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
  return oldb;
}

void myyoldbuffer(void * oldb)
{
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer((YY_BUFFER_STATE)oldb);
  //yy_flush_buffer((YY_BUFFER_STATE)oldb);
}

void myychangebuffer()
{
  yy_flush_buffer((YY_BUFFER_STATE)YY_CURRENT_BUFFER);
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
}

void my_yy_flush() { YY_FLUSH_BUFFER;BEGIN(0); }
