
/*  A Bison parser, made from mod_grammar.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	MCOLONCOLON	257
#define	MEQUAL	258
#define	MMODULE_CMD	259
#define	MVERSION_CMD	260
#define	MINFO_CMD	261
#define	MINFOFILE_CMD	262
#define	MHELP_CMD	263
#define	MHELPFILE_CMD	264
#define	MCXXSRC_CMD	265
#define	MPROC_CMD	266
#define	SECTEND	267
#define	SECT2START	268
#define	SECT2END	269
#define	SECT3START	270
#define	SECT3END	271
#define	SECT4START	272
#define	SECT4END	273
#define	PROCDECLTOK	274
#define	EXAMPLETOK	275
#define	STATICTOK	276
#define	VARNAMETOK	277
#define	MSTRINGTOK	278
#define	NAME	279
#define	FILENAME	280
#define	MCCODETOK	281
#define	MCODETOK	282
#define	CODEPART	283
#define	CMTPART	284
#define	PROCCMD	285
#define	ANYTOK	286
#define	VARTYPETOK	287
#define	NUMTOK	288
#define	BOOLTOK	289

#line 5 "mod_grammar.y"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "typmap.h"
#include "stype.h"

VAR int sectnum = 1;
VAR int iseof = 0;
VAR int initdone = 0;
EXTERN_INST_VAR moddef module_def;
EXTERN_VAR int yylineno;
EXTERN_VAR int do_create_makefile;
EXTERN_VAR char *sectname[];

extern int init_modgen(moddef *module_def, char *filename);
extern int write_intro(moddefv module);
extern void write_mod_init(moddefv module, FILE *fp);
extern void enter_id(FILE *fp, char *name, char *value,
                     int lineno, char *file);

INST_VAR procdef procedure_decl;


void yyerror(char * fmt)
  {
    if(!iseof) printf("%s at line %d\n", fmt, yylineno);
  }

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		138
#define	YYFLAG		-32768
#define	YYNTBASE	44

#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 79)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    43,     2,    42,     2,     2,    40,
    41,     2,     2,    38,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    37,     2,
    36,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    39,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     5,     9,    14,    19,    23,    26,    28,    30,    33,
    37,    40,    42,    44,    47,    51,    55,    59,    63,    67,
    71,    75,    77,    80,    82,    86,    88,    91,    93,    96,
   100,   103,   107,   112,   114,   118,   121,   125,   128,   132,
   135,   139,   141,   145,   150,   153,   157,   159,   162,   164,
   167,   170,   173,   178,   180,   182,   185,   189,   195,   202,
   212,   217,   219,   223,   225,   229,   232,   235,   240,   245,
   247,   250,   254,   259,   263,   265
};

static const short yyrhs[] = {    45,
    56,    57,    77,     0,    45,    55,    77,     0,    45,    52,
    55,    77,     0,    45,    55,    52,    77,     0,    46,    47,
    48,     0,    47,    48,     0,    48,     0,    27,     0,    50,
    37,     0,    47,    50,    37,     0,    49,    13,     0,    13,
     0,    29,     0,    49,    29,     0,    25,    36,    24,     0,
    25,    36,    26,     0,    25,    36,    51,     0,    25,    36,
    34,     0,    25,    36,    35,     0,    26,    38,    26,     0,
    14,    53,    54,     0,    58,     0,    53,    58,     0,    15,
     0,    16,    56,    57,     0,    62,     0,    56,    62,     0,
    17,     0,    59,    67,     0,    59,    67,    69,     0,    60,
    39,     0,    60,    66,    39,     0,    61,    40,    75,    41,
     0,    61,     0,    61,    40,    41,     0,    20,    25,     0,
    22,    20,    25,     0,    63,    67,     0,    63,    67,    69,
     0,    64,    39,     0,    64,    66,    39,     0,    65,     0,
    65,    40,    41,     0,    65,    40,    76,    41,     0,    33,
    25,     0,    22,    33,    25,     0,    24,     0,    68,    28,
     0,    29,     0,    68,    29,     0,    68,    30,     0,    68,
    72,     0,    70,    39,    71,    28,     0,    21,     0,    29,
     0,    71,    29,     0,    42,    25,    37,     0,    42,    25,
    40,    41,    37,     0,    42,    25,    40,    25,    41,    37,
     0,    42,    25,    40,    73,    40,    74,    41,    41,    37,
     0,    42,    25,     4,    32,     0,    25,     0,    73,     3,
    25,     0,    25,     0,    74,    38,    25,     0,    33,    25,
     0,    33,    43,     0,    75,    38,    33,    25,     0,    75,
    38,    33,    43,     0,    33,     0,    33,    25,     0,    76,
    38,    33,     0,    76,    38,    33,    25,     0,    18,    78,
    19,     0,    29,     0,    78,    29,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    98,   103,   108,   113,   120,   127,   134,   142,   153,   154,
   158,   164,   171,   181,   187,   225,   244,   262,   279,   312,
   322,   324,   325,   328,   335,   337,   338,   341,   351,   358,
   368,   371,   376,   380,   384,   390,   397,   406,   411,   419,
   423,   429,   430,   431,   433,   439,   448,   457,   463,   468,
   473,   478,   482,   488,   498,   506,   520,   547,   569,   591,
   613,   637,   642,   651,   655,   660,   667,   673,   680,   688,
   693,   701,   706,   716,   720,   726
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","MCOLONCOLON",
"MEQUAL","MMODULE_CMD","MVERSION_CMD","MINFO_CMD","MINFOFILE_CMD","MHELP_CMD",
"MHELPFILE_CMD","MCXXSRC_CMD","MPROC_CMD","SECTEND","SECT2START","SECT2END",
"SECT3START","SECT3END","SECT4START","SECT4END","PROCDECLTOK","EXAMPLETOK","STATICTOK",
"VARNAMETOK","MSTRINGTOK","NAME","FILENAME","MCCODETOK","MCODETOK","CODEPART",
"CMTPART","PROCCMD","ANYTOK","VARTYPETOK","NUMTOK","BOOLTOK","'='","';'","','",
"'{'","'('","')'","'%'","'#'","goal","part1","initmod","sect1","sect1end","codeline2",
"expr","files","sect2full","sect2","sect2end","sect3full","sect3","sect3end",
"procdefsg","procdeclsg","procdeclsg2","procdecl1","procdef","procdecl","procdecl2",
"funcdecl1","procdeclhelp","proccode","proccodeline","procdeclexample","examplestart",
"examplecodeline","proccmd","identifier","arglist","sgtypelist","typelist","sect4",
"codeline", NULL
};
#endif

static const short yyr1[] = {     0,
    44,    44,    44,    44,    45,    45,    45,    46,    47,    47,
    48,    48,    49,    49,    50,    50,    50,    50,    50,    51,
    52,    53,    53,    54,    55,    56,    56,    57,    58,    58,
    59,    59,    60,    60,    60,    61,    61,    62,    62,    63,
    63,    64,    64,    64,    65,    65,    66,    67,    68,    68,
    68,    68,    69,    70,    71,    71,    72,    72,    72,    72,
    72,    73,    73,    74,    74,    75,    75,    75,    75,    76,
    76,    76,    76,    77,    78,    78
};

static const short yyr2[] = {     0,
     4,     3,     4,     4,     3,     2,     1,     1,     2,     3,
     2,     1,     1,     2,     3,     3,     3,     3,     3,     3,
     3,     1,     2,     1,     3,     1,     2,     1,     2,     3,
     2,     3,     4,     1,     3,     2,     3,     2,     3,     2,
     3,     1,     3,     4,     2,     3,     1,     2,     1,     2,
     2,     2,     4,     1,     1,     2,     3,     5,     6,     9,
     4,     1,     3,     1,     3,     2,     2,     4,     4,     1,
     2,     3,     4,     3,     1,     2
};

static const short yydefact[] = {     0,
    12,     0,     8,    13,     0,     0,     0,     7,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    26,     0,
     0,    42,     0,     6,     0,    11,    14,     9,    15,    16,
    18,    19,    17,     0,     0,     0,    22,     0,     0,    34,
     0,     0,    45,     0,     0,     0,     2,    28,     0,    27,
    49,    38,     0,    47,    40,     0,     0,     5,    10,     0,
    36,     0,    24,    21,    23,    29,    31,     0,     0,    25,
    46,     3,    75,     0,     4,     1,    54,    39,     0,    48,
    50,    51,     0,    52,    41,    70,    43,     0,    20,    37,
    30,    32,     0,    35,     0,    74,    76,     0,     0,    71,
     0,    44,    66,    67,     0,    33,    55,     0,     0,    57,
     0,    72,     0,    53,    56,    61,    62,     0,     0,    73,
    68,    69,     0,    58,     0,     0,    59,    63,    64,     0,
     0,     0,    65,     0,    60,     0,     0,     0
};

static const short yydefgoto[] = {   136,
     5,     6,     7,     8,     9,    10,    33,    16,    36,    64,
    17,    18,    49,    37,    38,    39,    40,    19,    20,    21,
    22,    56,    52,    53,    78,    79,   108,    84,   119,   130,
    95,    88,    47,    74
};

static const short yypact[] = {    -9,
-32768,   -15,-32768,-32768,   -11,     8,     1,-32768,     0,     3,
    26,    54,   -14,    20,    33,    29,    -8,    -5,-32768,    34,
    10,    35,     1,-32768,    42,-32768,-32768,-32768,-32768,    43,
-32768,-32768,-32768,    55,    62,    44,-32768,    34,    12,    45,
    -5,    58,-32768,    66,    57,    66,-32768,-32768,    66,-32768,
-32768,    67,    13,-32768,-32768,    48,    21,-32768,-32768,    63,
-32768,    65,-32768,-32768,-32768,    67,-32768,    52,    24,-32768,
-32768,-32768,-32768,    27,-32768,-32768,-32768,-32768,    53,-32768,
-32768,-32768,    68,-32768,-32768,    69,-32768,     6,-32768,-32768,
-32768,-32768,   -18,-32768,    30,-32768,-32768,    70,    -2,-32768,
    64,-32768,-32768,-32768,    71,-32768,-32768,    49,    73,-32768,
   -10,    75,   -16,-32768,-32768,-32768,    60,    59,    -3,-32768,
-32768,-32768,    61,-32768,    77,    78,-32768,-32768,-32768,    32,
    81,    72,-32768,    74,-32768,    95,   107,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   102,    16,-32768,    25,-32768,    92,-32768,-32768,
    94,    99,    76,    79,-32768,-32768,-32768,   -17,-32768,-32768,
-32768,    80,    82,-32768,    50,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    23,-32768
};


#define	YYLAST		120


static const short yytable[] = {   125,
    50,   109,    12,     1,    13,    12,   103,    14,   121,    45,
    14,    48,    26,     1,   117,     2,    14,     3,    15,     4,
    11,    15,    24,    50,   104,     2,   122,    15,    27,     4,
   118,    25,     2,    54,   110,    54,   126,   111,    58,    28,
    80,    81,    82,   101,    13,    96,   102,    25,    55,    29,
    67,    30,    42,    86,    83,    97,    93,    43,    63,    31,
    32,    87,    51,    34,    94,    35,    72,   105,    75,   131,
   106,    76,   132,    34,    57,    35,   114,   115,    59,    61,
    60,    62,    71,    45,    69,    73,    85,    77,    89,    90,
    92,    98,    99,   100,   137,   124,   112,   127,   107,   120,
   123,   128,   129,   113,   116,   133,   138,    23,    46,    44,
   135,    41,   134,     0,    65,    91,    70,     0,    68,    66
};

static const short yycheck[] = {     3,
    18,     4,    14,    13,    16,    14,    25,    22,    25,    18,
    22,    17,    13,    13,    25,    25,    22,    27,    33,    29,
    36,    33,     7,    41,    43,    25,    43,    33,    29,    29,
    41,     7,    25,    24,    37,    24,    40,    40,    23,    37,
    28,    29,    30,    38,    16,    19,    41,    23,    39,    24,
    39,    26,    33,    33,    42,    29,    33,    25,    15,    34,
    35,    41,    29,    20,    41,    22,    44,    38,    46,    38,
    41,    49,    41,    20,    40,    22,    28,    29,    37,    25,
    38,    20,    25,    18,    40,    29,    39,    21,    26,    25,
    39,    39,    25,    25,     0,    37,    33,    37,    29,    25,
    41,    25,    25,    33,    32,    25,     0,     6,    17,    16,
    37,    13,    41,    -1,    36,    66,    41,    -1,    39,    38
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

VAR int	yychar;			/*  the lookahead symbol		*/
VAR YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
VAR YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

VAR int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
VAR int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     VAR char *from;
     VAR unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 99 "mod_grammar.y"
{
          if(trace)printf("Finish modules 1\n");
          return 0;
      ;
    break;}
case 2:
#line 104 "mod_grammar.y"
{
          if(trace)printf("Finish modules 2\n");
          return 0;
      ;
    break;}
case 3:
#line 109 "mod_grammar.y"
{
          if(trace)printf("Finish modules 3\n");
          return 0;
      ;
    break;}
case 4:
#line 114 "mod_grammar.y"
{
          if(trace)printf("Finish modules 4\n");
          return 0;
      ;
    break;}
case 5:
#line 121 "mod_grammar.y"
{
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        ;
    break;}
case 6:
#line 128 "mod_grammar.y"
{
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        ;
    break;}
case 7:
#line 135 "mod_grammar.y"
{
          if(do_create_makefile)mod_create_makefile(&module_def);
          if(write_intro(&module_def)) {
            return(myyyerror("Error while creating files\n"));
          }
        ;
    break;}
case 8:
#line 144 "mod_grammar.y"
{
          fprintf(module_def.fmtfp, "%s", yyvsp[0].name);
          fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
          free(yyvsp[0].name);
          write_mod_init(&module_def, module_def.fmtfp);
	  initdone = 1;
        ;
    break;}
case 11:
#line 160 "mod_grammar.y"
{
	  memset(&procedure_decl, 0, sizeof(procdef));
	  if(debug>2)printf("End of section 1 (new=%d)\n", sectnum);
	;
    break;}
case 12:
#line 165 "mod_grammar.y"
{
          memset(&procedure_decl, 0, sizeof(procdef));
          if(debug>2)printf("End of section 1 (new=%d)\n", sectnum);
        ;
    break;}
case 13:
#line 172 "mod_grammar.y"
{
	  if(initdone == 0) {
	    write_mod_init(&module_def, module_def.fmtfp);
	    initdone = 1;
          }
	  fprintf(module_def.fmtfp, "#line %d \"%s\"\n",
	                            yylineno, module_def.filename);
          fprintf(module_def.fmtfp, "%s", yyvsp[0].name);
        ;
    break;}
case 14:
#line 182 "mod_grammar.y"
{
        fprintf(module_def.fmtfp, "%s", yyvsp[0].name);
        ;
    break;}
case 15:
#line 188 "mod_grammar.y"
{
          var_token vt;
          int rc = 0;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);

          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar(yyvsp[-2].name, VAR_STRING, &write_cmd)) ) {
                  if(write_cmd!=0)
                    write_cmd(&module_def, vt, STRING_CMD, yyvsp[-2].name, yyvsp[0].sv.string);
                  if(vt==VAR_VERSION)
                    make_version(yyvsp[0].sv.string, &module_def);
		  if(vt==VAR_MODULE)
		    make_module_name(yyvsp[0].sv.string, &module_def);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, yyvsp[-2].name, sectnum);
                }
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar(yyvsp[-2].name, VAR_STRING, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_STRING, vt, yyvsp[-2].name, yyvsp[0].sv.string);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, yyvsp[-2].name, sectnum);
                }
                break;
              default: break;

          }
          free(yyvsp[-2].name);
          free(yyvsp[0].sv.string);
          if(rc)return(rc);
        ;
    break;}
case 16:
#line 226 "mod_grammar.y"
{ var_token vt;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                Add2files(&module_def, yyvsp[0].name);
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar(yyvsp[-2].name, VAR_FILE, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_FILE, vt, yyvsp[-2].name, yyvsp[0].name);
                }
                break;
              default: break;

          }
          free(yyvsp[-2].name);
          free(yyvsp[0].name);
        ;
    break;}
case 17:
#line 245 "mod_grammar.y"
{ var_token vt;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar(yyvsp[-2].name, VAR_FILES, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_FILES, vt, yyvsp[-2].name, &yyvsp[0]);
                }
                break;
              default: break;

          }
          free(yyvsp[-2].name);
          //free($3);
        ;
    break;}
case 18:
#line 263 "mod_grammar.y"
{ var_token vt;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar(yyvsp[-2].name, VAR_NUM, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_NUM, vt, yyvsp[-2].name, &yyvsp[0].i);
                }
                break;
              default: break;

          }
          free(yyvsp[-2].name);
        ;
    break;}
case 19:
#line 280 "mod_grammar.y"
{
          var_token vt;
          int rc = 0;
          void (*write_cmd)(moddefv module, var_token type,
                            idtyp t, void *arg1, void *arg2);
          switch(sectnum) {
              case 1: /* pass 1: */
                if( (vt=checkvar(yyvsp[-2].name, VAR_BOOL, &write_cmd)) ) {
                  proc_set_default_var(VAR_BOOL, vt, yyvsp[-2].name, &yyvsp[0].i);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, yyvsp[-2].name, sectnum);
                }
                break;
              case 3: /* pass 3: procedure declaration */
                if( (vt=checkvar(yyvsp[-2].name, VAR_BOOL, &write_cmd)) ) {
                  proc_set_var(&procedure_decl, VAR_BOOL, vt, yyvsp[-2].name, &yyvsp[0].i);
                }
                else {
                  rc=myyyerror("Line %d: Unknown variable '%s' in section %d\n",
                            yylineno, yyvsp[-2].name, sectnum);
                }
                break;
              default: break;

          }
          free(yyvsp[-2].name);
          if(rc)return(rc);
        ;
    break;}
case 20:
#line 313 "mod_grammar.y"
{
          if(debug>2)printf(">>>>>>>>files '%s' , '%s'\n", yyvsp[-2].name, yyvsp[0].name);
          Add2files(&module_def, yyvsp[-2].name);
          Add2files(&module_def, yyvsp[0].name);
          free(yyvsp[-2].name);
          free(yyvsp[0].name);
        ;
    break;}
case 24:
#line 329 "mod_grammar.y"
{
          if(debug>2)printf("End of section 'Singular' 2 (%d)\n",
                            sectnum);
;
    break;}
case 28:
#line 342 "mod_grammar.y"
{
          write_finish_functions(&module_def, &procedure_decl);
          if(debug>2)printf("End of section 'procedures' 3 (%d)\n",
                            sectnum);
        ;
    break;}
case 29:
#line 352 "mod_grammar.y"
{
          if(debug>2)printf("SG-PROCDEF:\n");
          write_singular_end(&module_def, &procedure_decl, yylineno);
	  setup_proc(&module_def, &procedure_decl);
	  write_helpfile_help(&module_def, &procedure_decl);
        ;
    break;}
case 30:
#line 359 "mod_grammar.y"
{
          if(debug>2)printf("SG-PROCDEF mit example:\n");
          fflush(module_def.fmtfp);
          write_singular_end(&module_def, &procedure_decl, yylineno);
          setup_proc(&module_def, &procedure_decl);
	  write_helpfile_help(&module_def, &procedure_decl);
        ;
    break;}
case 31:
#line 369 "mod_grammar.y"
{
        ;
    break;}
case 32:
#line 372 "mod_grammar.y"
{
        ;
    break;}
case 33:
#line 377 "mod_grammar.y"
{
           procedure_decl.lineno_other = yylineno;
         ;
    break;}
case 34:
#line 381 "mod_grammar.y"
{
           procedure_decl.lineno_other = yylineno;
         ;
    break;}
case 35:
#line 385 "mod_grammar.y"
{
           procedure_decl.lineno_other = yylineno;
         ;
    break;}
case 36:
#line 391 "mod_grammar.y"
{
          init_proc(&procedure_decl, yyvsp[0].name, NULL, yylineno, LANG_SINGULAR);
          free(yyvsp[0].name);
          if(write_singular_procedures(&module_def, &procedure_decl))
            return(myyyerror("Error while creating bin-file\n"));
        ;
    break;}
case 37:
#line 398 "mod_grammar.y"
{
          init_proc(&procedure_decl, yyvsp[0].name, NULL, yylineno, LANG_SINGULAR);
          procedure_decl.is_static = TRUE;
          free(yyvsp[0].name);
          if(write_singular_procedures(&module_def, &procedure_decl))
            return(myyyerror("Error while creating bin-file\n"));
        ;
    break;}
case 38:
#line 407 "mod_grammar.y"
{
          if(debug>2)printf("PROCDEF:\n");
	  write_helpfile_help(&module_def, &procedure_decl);
        ;
    break;}
case 39:
#line 412 "mod_grammar.y"
{
          if(debug>2)printf("PROCDEF mit example:\n");
	  write_helpfile_help(&module_def, &procedure_decl);
          fflush(module_def.fmtfp);
        ;
    break;}
case 40:
#line 420 "mod_grammar.y"
{
          setup_proc(&module_def, &procedure_decl);
        ;
    break;}
case 41:
#line 424 "mod_grammar.y"
{
          setup_proc(&module_def, &procedure_decl);
        ;
    break;}
case 45:
#line 434 "mod_grammar.y"
{
          if(debug>2)printf("funcdecl1-2\n");
          init_proc(&procedure_decl, yyvsp[0].name, &yyvsp[-1].tp, yylineno);
          free(yyvsp[0].name);
        ;
    break;}
case 46:
#line 440 "mod_grammar.y"
{
          if(debug>2)printf("funcdecl1-4\n");
          init_proc(&procedure_decl, yyvsp[0].name, &yyvsp[-1].tp, yylineno);
          free(yyvsp[0].name);
          procedure_decl.is_static = TRUE;
        ;
    break;}
case 47:
#line 449 "mod_grammar.y"
{
          procedure_decl.help_string = yyvsp[0].sv.string;
          procedure_decl.lineno_other = yyvsp[0].sv.lineno;
          if(debug>2)printf("\t\thelp at %d\n", yylineno);
          write_help(&module_def, &procedure_decl);
        ;
    break;}
case 48:
#line 458 "mod_grammar.y"
{
            write_function_errorhandling(&module_def, &procedure_decl);
          ;
    break;}
case 49:
#line 464 "mod_grammar.y"
{
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, yyvsp[0].name);
          write_codeline(&module_def, &procedure_decl, yyvsp[0].name, yylineno-1);
        ;
    break;}
case 50:
#line 469 "mod_grammar.y"
{
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, yyvsp[0].name);
          write_codeline(&module_def, &procedure_decl, yyvsp[0].name);
        ;
    break;}
case 51:
#line 474 "mod_grammar.y"
{
          printf(">>>>(%d) %s<<<<\n", procedure_decl.flags.start_of_code, yyvsp[0].name);
          write_codeline(&module_def, &procedure_decl, yyvsp[0].name);
        ;
    break;}
case 52:
#line 479 "mod_grammar.y"
{
        ;
    break;}
case 53:
#line 483 "mod_grammar.y"
{
          write_example(&module_def, &procedure_decl);
        ;
    break;}
case 54:
#line 489 "mod_grammar.y"
{
          if(procedure_decl.procname == NULL) {
            return(myyyerror("example without proc-declaration at line %d\n",
                     yylineno));
          }
          if(debug>2)printf("Example at %d\n", yylineno);
          procedure_decl.lineno_other = yylineno;
        ;
    break;}
case 55:
#line 499 "mod_grammar.y"
{
          int len = strlen(yyvsp[0].name);
          procedure_decl.example_len = len;
          procedure_decl.example_string = (char *)malloc(len+1);
          memset(procedure_decl.example_string, 0, len+1);
          memcpy(procedure_decl.example_string, yyvsp[0].name, len);
        ;
    break;}
case 56:
#line 507 "mod_grammar.y"
{
          long len = strlen(yyvsp[0].name);
          long newlen = procedure_decl.example_len + len;

          procedure_decl.example_string =
            (char *)realloc((void *)procedure_decl.example_string, newlen+1);
          memset(procedure_decl.example_string+procedure_decl.example_len,
                 0, len+1);
          memcpy(procedure_decl.example_string+procedure_decl.example_len,
                 yyvsp[0].name, len);
          procedure_decl.example_len = newlen;
        ;
    break;}
case 57:
#line 521 "mod_grammar.y"
{ cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);

          switch(vt=checkcmd(yyvsp[-1].name, &write_cmd, CMDT_SINGLE, 0)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, yyvsp[-1].name, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, yyvsp[-1].name, sectnum));
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
          free(yyvsp[-1].name);
        ;
    break;}
case 58:
#line 548 "mod_grammar.y"
{
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);

          switch(vt=checkcmd(yyvsp[-3].name, &write_cmd, CMDT_0, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, yyvsp[-3].name, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, yyvsp[-3].name, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl,NULL);
          }
          free(yyvsp[-3].name);
        ;
    break;}
case 59:
#line 570 "mod_grammar.y"
{
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);

          switch(vt=checkcmd(yyvsp[-4].name, &write_cmd, CMDT_ANY, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, yyvsp[-4].name, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, yyvsp[-4].name, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, yyvsp[-2].name);
          }
          free(yyvsp[-4].name); free(yyvsp[-2].name);
        ;
    break;}
case 60:
#line 592 "mod_grammar.y"
{
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);

          switch(vt=checkcmd(yyvsp[-7].name, &write_cmd, CMDT_ANY, 1)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, yyvsp[-7].name, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, yyvsp[-7].name, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, yyvsp[-5].name);
          }
          free(yyvsp[-7].name);
        ;
    break;}
case 61:
#line 614 "mod_grammar.y"
{
          cmd_token vt;
          void (*write_cmd)(moddefv module, procdefv pi, void *arg);

          switch(vt=checkcmd(yyvsp[-2].name, &write_cmd, CMDT_EQ, 0)) {
              case CMD_NONE:
                return(myyyerror("Line %d: Unknown command '%s' in section %d\n",
                          yylineno, yyvsp[-2].name, sectnum));
                break;
              case CMD_BADSYNTAX:
                return(myyyerror("Line %d: bad syntax of command '%s' in section %d\n",
                          yylineno, yyvsp[-2].name, sectnum));
                break;
              default:
                write_function_header(&module_def, &procedure_decl);
              case CMD_DECL:
              case CMD_CHECK:
                write_cmd(&module_def, &procedure_decl, yyvsp[0].name);
          }
          free(yyvsp[-2].name);
        ;
    break;}
case 62:
#line 638 "mod_grammar.y"
{
          if(debug>2)printf("### ID ### Name %s\n", yyvsp[0].name);
          yyval.name = yyvsp[0].name;
        ;
    break;}
case 63:
#line 643 "mod_grammar.y"
{
          int len = strlen(yyval.name) + strlen(yyvsp[0].name) + 2;
          if(debug>2)printf("### ID ### Name %s\n", yyvsp[0].name);
          yyval.name = (char *)realloc(yyval.name, len);
          strcat(yyval.name, "::");
          strcat(yyval.name, yyvsp[0].name);
        ;
    break;}
case 64:
#line 652 "mod_grammar.y"
{
          if(debug>2)printf("### ARGS %s\n", yyvsp[0].name);
        ;
    break;}
case 65:
#line 656 "mod_grammar.y"
{
          if(debug>2)printf("### ARGS %s\n", yyvsp[0].name);
        ;
    break;}
case 66:
#line 661 "mod_grammar.y"
{
          if(debug>2)printf("\tsgtypelist %s %s\n", yyvsp[-1].tp.name, yyvsp[0].name);
          write_singular_parameter(&module_def, yylineno, yyvsp[-1].tp.name, yyvsp[0].name);
          free(yyvsp[-1].tp.name);
          free(yyvsp[0].name);
        ;
    break;}
case 67:
#line 668 "mod_grammar.y"
{
          if(debug>2)printf("\tsgtypelist %s %s\n", yyvsp[-1].tp.name, yyvsp[0]);
          write_singular_parameter(&module_def, yylineno, yyvsp[-1].tp.name, "#");
          free(yyvsp[-1].tp.name);
        ;
    break;}
case 68:
#line 674 "mod_grammar.y"
{
          if(debug>2)printf("\tsgtypelist next  %s %s\n", yyvsp[-1].tp.name, yyvsp[0].name);
          write_singular_parameter(&module_def, yylineno, yyvsp[-1].tp.name, yyvsp[0].name);
          free(yyvsp[-1].tp.name);
          free(yyvsp[0].name);
        ;
    break;}
case 69:
#line 681 "mod_grammar.y"
{
          if(debug>2)printf("\tsgtypelist next  %s %s\n", yyvsp[-1].tp.name, yyvsp[0]);
          write_singular_parameter(&module_def, yylineno, yyvsp[-1].tp.name, "#");
          free(yyvsp[-1].tp.name);
        ;
    break;}
case 70:
#line 689 "mod_grammar.y"
{
          AddParam(&procedure_decl, &yyvsp[0].tp);
          free(yyvsp[0].tp.name);
        ;
    break;}
case 71:
#line 694 "mod_grammar.y"
{
          if(check_reseverd(yyvsp[0].name))
            return(myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, yyvsp[0].name));
          AddParam(&procedure_decl, &yyvsp[-1].tp, yyvsp[0].name);
          free(yyvsp[-1].tp.name); free(yyvsp[0].name);
        ;
    break;}
case 72:
#line 702 "mod_grammar.y"
{
          AddParam(&procedure_decl, &yyvsp[0].tp);
          free(yyvsp[0].tp.name);
        ;
    break;}
case 73:
#line 707 "mod_grammar.y"
{
          if(check_reseverd(yyvsp[0].name))
            return(myyyerror("Line %d: variablename '%s' is reserved\n",
                            yylineno, yyvsp[0].name));
          AddParam(&procedure_decl, &yyvsp[-1].tp, yyvsp[0].name);
          free(yyvsp[-1].tp.name); free(yyvsp[0].name);
        ;
    break;}
case 74:
#line 717 "mod_grammar.y"
{
        ;
    break;}
case 75:
#line 721 "mod_grammar.y"
{
          fprintf(module_def.modfp, "#line %d \"%s\"\n",
					  yylineno, module_def.filename);
          fprintf(module_def.modfp, "%s", yyvsp[0].name);
        ;
    break;}
case 76:
#line 727 "mod_grammar.y"
{
	  fprintf(module_def.modfp, "%s", yyvsp[0].name);
        ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 733 "mod_grammar.y"


