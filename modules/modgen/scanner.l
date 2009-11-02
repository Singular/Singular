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
#include "stype.h"
#include "mod_grammar.h"

#define DEBUG 3
 
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

  extern int iseof;

  int offset = 0;
  int yylineno = 1;
  int do_return = 0;

  int state_level = -1;
  int state_max = 0;
  char *yylp_buffer_start;

  extern int sectnum;
  static int oldsectnum = 1;
  extern moddef module_def;
  extern procdef procedure_decl;

  char *sectname[] = { "sect -0", "section 1",
                       "Singular", "procedures", "C-part" 
  };
  
  struct _states {
    char name[32];
    int state;
  } * old_states = NULL;
/*  int *old_states = NULL;*/

  int current_pos(int i);
  int read_string(char **buffer, long *start, long end);
  int libread(FILE* f, char* buf, int max_size);
  void add_action(char *new_text);
 
  void push_state(int state, int new_state, char *name);
  void switch_state(int state, int new_state, char *name);
  void pop_state();
  int return_sect_token(int passed, int old_sect, int new_sect);
  extern int init_modgen(moddef *module_def, char *filename);
 
static char * dupyytext()
{
  if (yyleng>0) yytext[yyleng-1] = '\0';
  return strdup((char *)yytext);
}

#   undef YY_DECL
#   define YY_DECL int yylex(YYSTYPE* lvalp)

#   undef YY_INPUT
#   define YY_INPUT(buf,result,max_size) \
          if ( ((result = libread( (yyin), (char *) buf, max_size )) < 0 ) \
                  && ferror( yyin ) ) \
                YY_FATAL_ERROR( "read in flex scanner failed" );

#   undef yywrap
  extern "C"
  {
  int yywrap();
  }

#define ACTION_ECHO add_action( yytext )

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
begincode      (%+\{)
endcode        (\n+%+[}]+\n)

WS             [[:blank:]]+
NL             \r?\n
WSNL           ([ \t\n\r]+)
EQ             (([{WS}]+=[{WS}]+)|=)

NAME           ([[:alpha:]_][[:alnum:]_-]*)
FILENAME       ([[:alpha:]_][[:alnum:]_-]*\.(c|cc|h))
NUMBER         ({digit}*)
BOOL           (([Yy][Ee][Ss])|([Nn][Oo])|[01])

/* %start START */

%x preamble
%x MODINITIAL
%x PROCCMDBLOCK
%x CODEBLOCK
%x CODEBLOCK2
%x module
%x COMMENT
%x COMMENTB
%x STRING
%x SECT2
%x SECT3
%x SECT4
%x PROCDECL
%x EXAMPLE
%x ANYLINE

%x pdef
%x procopt
%x procdef
%x ctext
%x cstring

%%
       static int brace1 = 0;  /* { } */
       static int brace2 = 0;  /* ( ) */
       static int brace3 = 0;  /* [ ] */
       static int quote  = 0;  /* " */

<COMMENT,COMMENTB,STRING,INITIAL><<EOF>>   {
          printf( "EOF encountered inside an action\n");
	  printf("ERRRRROOOOORRR\n");
                 }

<INITIAL,SECT2,SECT3,SECT4>{
        ^"%Singular".* {
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 2;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT2, "SECT2");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
        "%Singular".* {
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 2;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT2, "SECT2");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
        ^"%procedures".* { 
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 3;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT3, "SECT3");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
        "%procedures".* { 
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 3;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT3, "SECT3");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
        ^"%C".* { 
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 4;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT4, "SECT4");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
        "%C".* { 
                  int passed = (oldsectnum == sectnum) ? 0 : 1;
                  int old_sect = sectnum;

                  oldsectnum = sectnum;
                  sectnum = 4;
                  if(!passed) yyless(0);
                  else push_state(YYSTATE, SECT4, "SECT4-2");
                  return(return_sect_token(passed, old_sect, sectnum));
                }
              }


<INITIAL>{
        ^{WS}      push_state(YYSTATE, CODEBLOCK, "CODEBLOCK");
        ^"/*"      push_state(YYSTATE, COMMENT, "COMMENT"); yymore();
        "//".*{NL} ++yylineno; ACTION_ECHO;
	{BOOL}  { if(yyleng == 1) sscanf(yytext, "%d", &lvalp->i);
	          else {
		    if(strcasecmp(yytext, "yes")==0) lvalp->i = 1;
		    else lvalp->i = 0;
                  }
                  return BOOLTOK; }
	^"%{".*{NL} { 
	        yylineno++;
		push_state(YYSTATE, CODEBLOCK, "CODEBLOCK");
		fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
		}
	^"%modinitial".*{NL} {
	        yylineno++;
		push_state(YYSTATE, MODINITIAL, "MODINITIAL");
		fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
	                               	  yylineno, module_def.filename);
		}
	{WS}            /* discard */
        {NL}    { yylineno++; }

        {eq}    { return '='; }
        ";"     { return ';'; }
        ","     { return ','; }

        "\""    { do_return++; 
                  lvalp->sv.lineno = yylineno;
                  push_state(YYSTATE, STRING, "string");
                }

	^{NAME} {
	          lvalp->name = strdup(yytext);
                  return NAME;
                }
	{FILENAME} {
	          lvalp->name = strdup(yytext);
                  return FILENAME;
                }
	   }

<CODEBLOCK>{
            ^"%}".*{NL} {
                   char * s, *t;

                   yylineno++;
		   pop_state();
		   s = t = lvalp->name = dupyytext();
		   while (*yytext)
		   {
		     if (*yytext == '\\') yytext++;
		     *s++ = *yytext++;
		   }
		   if(s-t>2 && *(s-1)=='}' && *(s-2)=='%') *(s-2)='\0';
		   return MCCODETOK;
                 }
         {NL}    { yylineno++; yymore(); }
         .       { yymore(); }
       }
<MODINITIAL>{
           ^"%endinitial".*{NL} {
	           yylineno++;
		   pop_state();
                 }
           .*{NL}  { yylineno++; 
	           lvalp->name = yytext;
		   return CODEPART; }
       }
<STRING>{
          {NL}    { yylineno++; yymore(); }
          [^\"]   { yymore(); }
          "\\\\"  { yymore(); }
          "\\\""  { yymore(); }
          ("\"")  {
                    char * s, *t;
                    pop_state();
	            if(do_return) {
                      s = t = lvalp->sv.string = dupyytext();
                      while (*yytext)
                      {
                        if (*yytext == '\\') yytext++;
                        *s++ = *yytext++;
                      }
		      *s++ = *yytext++;
		      do_return = 0;
		      return MSTRINGTOK;
                    } else {
		      do_return = 0;
                      yymore();
                    }
		  }
	}

<COMMENT>{
        "*/"            ACTION_ECHO; pop_state(); yymore();
        "*"           yymore();
        {NL}          { yylineno++; yymore(); }
        .             { yymore(); }
	  }

<SECT2>{
        {NL}    { yylineno++; }
        {WS}    /* ignore */
        ;       /* ignore */
        "/*"      push_state(YYSTATE, COMMENT, "COMMENT"); yymore();
        "//".*{NL}  ++yylineno; ACTION_ECHO; 
        proc+{WS} {
                  brace1 = 0; /* { */
                  brace2 = 0; /* ( */
                  brace3 = 0; /* [ */
	          push_state(YYSTATE, PROCDECL, "PROCDECL");
		  return PROCDECLTOK;
                }
        example {
#if DEBUG > 1
	          printf(">>>EXAMPLE\n");
#endif
                  brace1 = 0; /* { */
                  brace2 = 0; /* ( */
                  brace3 = 0; /* [ */
	            push_state(YYSTATE, EXAMPLE, "EXAMPLE");
		        return EXAMPLETOK;
                }
 	    static  { return STATICTOK; }
        .       { printf("SG(%d) <<<'%s' ", yylineno, yytext); }
       }

<SECT3>{
        {NL}    { yylineno++; }
        {WS}    /* ignore */
        ;       /* ignore */
        "/*"      push_state(YYSTATE, COMMENT, "COMMENT"); yymore();
        "//".*{NL}  ++yylineno; ACTION_ECHO; 
        example {
#if DEBUG > 1
	          printf(">>>EXAMPLE\n");
#endif
                  brace1 = 0; /* { */
                  brace2 = 0; /* ( */
                  brace3 = 0; /* [ */
	          push_state(YYSTATE, EXAMPLE, "EXAMPLE");
		  return EXAMPLETOK;
                }
	static  { return STATICTOK; }
	{NAME}  { 
	           int i,tok;
#if DEBUG > 1
	           printf("(%d) VAR: %s\n", yylineno, yytext);
#endif
		   i = IsCmd(yytext, tok);
#if DEBUG > 1
		   printf("Res=%d, %d => %s\n", i, tok,
			  i ? "VARTYPETOK" : "NAME");
#endif
		   if(i) {
		     lvalp->tp.name = strdup(yytext);
		     lvalp->tp.typ = tok;
		     push_state(YYSTATE, PROCDECL, "PROCDECL");
		     return VARTYPETOK;
		   }
		   else {
		     //do_return++;
		     lvalp->name = strdup(yytext);
		     push_state(YYSTATE, PROCDECL, "PROCDECL");
		     return NAME;
		   }
                 }
        .       { printf("PR(%d) <<<'%s' ", yylineno, yytext); }
       }

<EXAMPLE>{
        {NL}    { yylineno++; }
        {WS}    /* ignore */
        "{"     { brace1++;
		  switch_state(YYSTATE, CODEBLOCK2, "CODEBLOCK2");
		  return '{';
		}
        .       { printf("ERROR <<<'%s' ", yytext); }
}

<SECT4>{
        {NL}    { yylineno++; }
        .*(\n?) { lvalp->name = yytext; 
	          return CODEPART; }

        <<EOF>>  { sectnum = 0;
	           iseof=1;
		   lvalp->name = yytext;
	           return SECT4END;
                   /* yyterminate(); */
                 }
       }

<PROCDECL>{
         "{"     { brace1++;
		   switch_state(YYSTATE, CODEBLOCK2, "CODEBLOCK2");
		   return '{';
		 }
         "/*"       push_state(YYSTATE, COMMENT, "COMMENT");yymore();
         "//".*{NL}  ++yylineno; ACTION_ECHO; 
         "\""    { do_return++; 
	           lvalp->sv.lineno = yylineno;
                   push_state(YYSTATE, STRING, "string"); 
                 }
	 {BOOL}  { if(yyleng == 1) sscanf(yytext, "%d", &lvalp->i);
	           else {
		     if(strcasecmp(yytext, "yes")==0) lvalp->i = 1;
		     else lvalp->i = 0;
                   }
                   return BOOLTOK; }
	 {NUMBER} { sscanf(yytext, "%d", &lvalp->i); return NUMTOK; }
	 {NAME}  { 
	           int i,tok;
#if DEBUG > 1
	           printf("(%d) VAR: %s\n", yylineno, yytext);
#endif
		   i = IsCmd(yytext, tok);
#if DEBUG > 1
		   printf("Res=%d, %d => %s\n", i, tok,
			  i ? "VARTYPETOK" : "NAME");
#endif
		   if(i) {
		     lvalp->tp.name = strdup(yytext);
		     lvalp->tp.typ = tok;
		     return VARTYPETOK;
		   }
		   else {
		     //do_return++;
		     lvalp->name = strdup(yytext);
		     return NAME;
		   }
                 }
         {NL}    { yylineno++; }
	 "("     { return '('; }
	 ")"     { return ')'; }
	 ","     { return ','; }
	 "#"     { return '#'; }
	 "="     { return '='; }
	 ";"     { return ';'; }
         .       { }

       }

<COMMENTB>{
        "*/"          { ACTION_ECHO;
                        pop_state();
                        lvalp->name = yytext;
                        return CMTPART;
                	yymore();
                      }
        "*"           yymore();
        {NL}          { yylineno++; yymore(); }
        .             { yymore(); }
	  }

<CODEBLOCK2>{
         "/*"      push_state(YYSTATE, COMMENT, "COMMENT"); yymore();
         "//".*{NL} { yylineno++; lvalp->name = yytext; return CODEPART; }
         "\""    { 
	           lvalp->sv.lineno = yylineno;
                   push_state(YYSTATE, STRING, "string"); yymore();}
         "{"     { brace1++; yymore();}
         "}"     { brace1--;
                   if(brace1<=0) {
                     char * s, *t;

		     pop_state();
#if 0
		     s = t = lvalp->name = dupyytext();
		     while (*yytext)
		     {
		       if (*yytext == '\\') yytext++;
		       *s++ = *yytext++;
		     }
		     if(s-t>2 && *(s-1)=='}' && *(s-2)=='%') *(s-2)='\0';
#else
                     lvalp->name = yytext;
#endif
#if DEBUG > 1
                     printf("2 BRACE DOWN=%d\n", brace1);
#endif
		     return MCODETOK;
                   } else {
                     yymore();
		   }
                 }
         {NL}    { yylineno++; lvalp->name = yytext; return CODEPART; }
	 "%" { push_state(YYSTATE, PROCCMDBLOCK, "PROCCMDBLOCK");
               procedure_decl.flags.start_of_code = 1;
               return '%'; }
         " " { yymore(); }
         .   { yymore(); procedure_decl.flags.start_of_code = 1; }

       }

<PROCCMDBLOCK>{
       {col}  { pop_state(); return ';'; }
	"("   { return '('; }
	")"   { return ')'; }
	","   { return ','; }
       {WS}   /* ignore */
       {NAME} { lvalp->name = strdup(yytext); return NAME; }
       {NL}   { yylineno++; }
       "::"   { return MCOLONCOLON; }
       "="    { switch_state(YYSTATE, ANYLINE, "ANYLINE"); return MEQUAL; }
       .      { printf("PCB: '%s'\n", yytext); }
       }

<ANYLINE>{
       {col}  { lvalp->name = yytext; 
                pop_state();
                return ANYTOK; }
       {NL}   { yylineno++; }
       .       { yymore(); }
}

%%
/*
        [^*\n]+         ++yylineno; yymore(); /*ACTION_ECHO;* /
        [^*\n]*{NL}     ++yylineno; yymore(); /*ACTION_ECHO;* /
         .*(\n?) { yylineno++; lvalp->name = yytext; return CODEPART; }
*/

extern "C" {
  int yywrap() {
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

void switch_state(int state, int new_state, char *name)
{
#if DEBUG > 2
  printf("====>SWITCH to new state %d/%d l=%d, [%s] at %d\n",
         state, new_state, state_level, name, yylineno);
#endif
  strncpy(old_states[state_level].name, name,
          sizeof(old_states[state_level].name));
  BEGIN(new_state);
}

void push_state(int state, int new_state, char *name)
{
  state_level++;
#if DEBUG > 2
  printf("====>PUSH to new state %d/%d l=%d, [%s] at %d\n",
         state, new_state, state_level, name, yylineno);
#endif
  if(state_level>=state_max) {
    state_max++;
    if(old_states == NULL)
      old_states = (struct _states *)malloc(sizeof(struct _states));
    else {
      old_states = (struct _states *)realloc(old_states,
                                             state_max*sizeof(struct _states));
    }
  }
  old_states[state_level].state = state;
  strncpy(old_states[state_level].name, name,
          sizeof(old_states[state_level].name));
  BEGIN(new_state);
}

void pop_state()
{
#if DEBUG > 2
  printf("====>Back to old state %d, l=%d [%s] at %d\n",
         old_states[state_level].state,
         state_level,
         (state_level>0) ? old_states[state_level-1].name : "INITIAL",
         yylineno);
#endif
  if(state_level<0) return;
  BEGIN(old_states[state_level].state);
  state_level--;
  if(state_level<0) state_level = -1;
}

int return_sect_token(
  int passed,
  int old_sect,
  int new_sect
  )
{
  if(passed) {
#if DEBUG
    printf("Go to section '%s' (%d)\n", sectname[new_sect], new_sect);
#endif
    switch(new_sect) {
        case 2: return SECT2START;
        case 3: return SECT3START;
        case 4: return SECT4START;
        default: return SECTEND;
    }
  } else {
#if DEBUG
    printf("End of section '%s' (%d)\n", sectname[old_sect], old_sect);
#endif
    switch(old_sect) {
        case 2: return SECT2END;
        case 3: return SECT3END;
        case 4: return SECT4END;
        default: return SECTEND;
    }
  }
}

void add_action(char *new_text)
{
  return;
  switch(sectnum) {
    case 1:
      printf("1>%s<#\n", new_text);
      break;
    case 2:
      printf("2>%s<#\n", new_text);
      break;
    case 3:
      printf("3>%s<#\n", new_text);
      break;
    case 4:
      printf("4>%s<#\n", new_text);
      break;
  }
}
