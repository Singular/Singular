/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DOTDOT = 258,
     EQUAL_EQUAL = 259,
     GE = 260,
     LE = 261,
     MINUSMINUS = 262,
     NOT = 263,
     NOTEQUAL = 264,
     PLUSPLUS = 265,
     COLONCOLON = 266,
     GRING_CMD = 267,
     BIGINTMAT_CMD = 268,
     INTMAT_CMD = 269,
     PROC_CMD = 270,
     RING_CMD = 271,
     BEGIN_RING = 272,
     IDEAL_CMD = 273,
     MAP_CMD = 274,
     MATRIX_CMD = 275,
     MODUL_CMD = 276,
     NUMBER_CMD = 277,
     POLY_CMD = 278,
     RESOLUTION_CMD = 279,
     VECTOR_CMD = 280,
     BETTI_CMD = 281,
     COEFFS_CMD = 282,
     COEF_CMD = 283,
     CONTRACT_CMD = 284,
     DEGREE_CMD = 285,
     DEG_CMD = 286,
     DIFF_CMD = 287,
     DIM_CMD = 288,
     DIVISION_CMD = 289,
     ELIMINATION_CMD = 290,
     E_CMD = 291,
     FAREY_CMD = 292,
     FETCH_CMD = 293,
     FREEMODULE_CMD = 294,
     KEEPRING_CMD = 295,
     HILBERT_CMD = 296,
     HOMOG_CMD = 297,
     IMAP_CMD = 298,
     INDEPSET_CMD = 299,
     INTERRED_CMD = 300,
     INTERSECT_CMD = 301,
     JACOB_CMD = 302,
     JET_CMD = 303,
     KBASE_CMD = 304,
     KOSZUL_CMD = 305,
     LEADCOEF_CMD = 306,
     LEADEXP_CMD = 307,
     LEAD_CMD = 308,
     LEADMONOM_CMD = 309,
     LIFTSTD_CMD = 310,
     LIFT_CMD = 311,
     MAXID_CMD = 312,
     MINBASE_CMD = 313,
     MINOR_CMD = 314,
     MINRES_CMD = 315,
     MODULO_CMD = 316,
     MONOM_CMD = 317,
     MRES_CMD = 318,
     MULTIPLICITY_CMD = 319,
     ORD_CMD = 320,
     PAR_CMD = 321,
     PARDEG_CMD = 322,
     PREIMAGE_CMD = 323,
     QUOTIENT_CMD = 324,
     QHWEIGHT_CMD = 325,
     REDUCE_CMD = 326,
     REGULARITY_CMD = 327,
     RES_CMD = 328,
     SIMPLIFY_CMD = 329,
     SORTVEC_CMD = 330,
     SRES_CMD = 331,
     STD_CMD = 332,
     SUBST_CMD = 333,
     SYZYGY_CMD = 334,
     VAR_CMD = 335,
     VDIM_CMD = 336,
     WEDGE_CMD = 337,
     WEIGHT_CMD = 338,
     VALTVARS = 339,
     VMAXDEG = 340,
     VMAXMULT = 341,
     VNOETHER = 342,
     VMINPOLY = 343,
     END_RING = 344,
     CMD_1 = 345,
     CMD_2 = 346,
     CMD_3 = 347,
     CMD_12 = 348,
     CMD_13 = 349,
     CMD_23 = 350,
     CMD_123 = 351,
     CMD_M = 352,
     ROOT_DECL = 353,
     ROOT_DECL_LIST = 354,
     RING_DECL = 355,
     RING_DECL_LIST = 356,
     EXAMPLE_CMD = 357,
     EXPORT_CMD = 358,
     HELP_CMD = 359,
     KILL_CMD = 360,
     LIB_CMD = 361,
     LISTVAR_CMD = 362,
     SETRING_CMD = 363,
     TYPE_CMD = 364,
     STRINGTOK = 365,
     BLOCKTOK = 366,
     INT_CONST = 367,
     UNKNOWN_IDENT = 368,
     RINGVAR = 369,
     PROC_DEF = 370,
     BREAK_CMD = 371,
     CONTINUE_CMD = 372,
     ELSE_CMD = 373,
     EVAL = 374,
     QUOTE = 375,
     FOR_CMD = 376,
     IF_CMD = 377,
     SYS_BREAK = 378,
     WHILE_CMD = 379,
     RETURN = 380,
     PARAMETER = 381,
     SYSVAR = 382,
     UMINUS = 383
   };
#endif
#define DOTDOT 258
#define EQUAL_EQUAL 259
#define GE 260
#define LE 261
#define MINUSMINUS 262
#define NOT 263
#define NOTEQUAL 264
#define PLUSPLUS 265
#define COLONCOLON 266
#define GRING_CMD 267
#define BIGINTMAT_CMD 268
#define INTMAT_CMD 269
#define PROC_CMD 270
#define RING_CMD 271
#define BEGIN_RING 272
#define IDEAL_CMD 273
#define MAP_CMD 274
#define MATRIX_CMD 275
#define MODUL_CMD 276
#define NUMBER_CMD 277
#define POLY_CMD 278
#define RESOLUTION_CMD 279
#define VECTOR_CMD 280
#define BETTI_CMD 281
#define COEFFS_CMD 282
#define COEF_CMD 283
#define CONTRACT_CMD 284
#define DEGREE_CMD 285
#define DEG_CMD 286
#define DIFF_CMD 287
#define DIM_CMD 288
#define DIVISION_CMD 289
#define ELIMINATION_CMD 290
#define E_CMD 291
#define FAREY_CMD 292
#define FETCH_CMD 293
#define FREEMODULE_CMD 294
#define KEEPRING_CMD 295
#define HILBERT_CMD 296
#define HOMOG_CMD 297
#define IMAP_CMD 298
#define INDEPSET_CMD 299
#define INTERRED_CMD 300
#define INTERSECT_CMD 301
#define JACOB_CMD 302
#define JET_CMD 303
#define KBASE_CMD 304
#define KOSZUL_CMD 305
#define LEADCOEF_CMD 306
#define LEADEXP_CMD 307
#define LEAD_CMD 308
#define LEADMONOM_CMD 309
#define LIFTSTD_CMD 310
#define LIFT_CMD 311
#define MAXID_CMD 312
#define MINBASE_CMD 313
#define MINOR_CMD 314
#define MINRES_CMD 315
#define MODULO_CMD 316
#define MONOM_CMD 317
#define MRES_CMD 318
#define MULTIPLICITY_CMD 319
#define ORD_CMD 320
#define PAR_CMD 321
#define PARDEG_CMD 322
#define PREIMAGE_CMD 323
#define QUOTIENT_CMD 324
#define QHWEIGHT_CMD 325
#define REDUCE_CMD 326
#define REGULARITY_CMD 327
#define RES_CMD 328
#define SIMPLIFY_CMD 329
#define SORTVEC_CMD 330
#define SRES_CMD 331
#define STD_CMD 332
#define SUBST_CMD 333
#define SYZYGY_CMD 334
#define VAR_CMD 335
#define VDIM_CMD 336
#define WEDGE_CMD 337
#define WEIGHT_CMD 338
#define VALTVARS 339
#define VMAXDEG 340
#define VMAXMULT 341
#define VNOETHER 342
#define VMINPOLY 343
#define END_RING 344
#define CMD_1 345
#define CMD_2 346
#define CMD_3 347
#define CMD_12 348
#define CMD_13 349
#define CMD_23 350
#define CMD_123 351
#define CMD_M 352
#define ROOT_DECL 353
#define ROOT_DECL_LIST 354
#define RING_DECL 355
#define RING_DECL_LIST 356
#define EXAMPLE_CMD 357
#define EXPORT_CMD 358
#define HELP_CMD 359
#define KILL_CMD 360
#define LIB_CMD 361
#define LISTVAR_CMD 362
#define SETRING_CMD 363
#define TYPE_CMD 364
#define STRINGTOK 365
#define BLOCKTOK 366
#define INT_CONST 367
#define UNKNOWN_IDENT 368
#define RINGVAR 369
#define PROC_DEF 370
#define BREAK_CMD 371
#define CONTINUE_CMD 372
#define ELSE_CMD 373
#define EVAL 374
#define QUOTE 375
#define FOR_CMD 376
#define IF_CMD 377
#define SYS_BREAK 378
#define WHILE_CMD 379
#define RETURN 380
#define PARAMETER 381
#define SYSVAR 382
#define UMINUS 383




/* Copy the first part of user declarations.  */
#line 7 "grammar.y"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <kernel/mod2.h>
#include <omalloc/mylimits.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/stype.h>
#include <Singular/ipid.h>
#include <kernel/intvec.h>
#include <kernel/febase.h>
#include <kernel/matpol.h>
#include <kernel/ring.h>
#include <kernel/kstd1.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>
#include <Singular/sdb.h>
#include <kernel/ideals.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/stairc.h>
#include <kernel/timer.h>
#include <Singular/cntrlc.h>
#include <kernel/maps.h>
#include <kernel/syz.h>
#include <Singular/lists.h>
#include <kernel/longrat.h>
#include <Singular/libparse.h>

#if 0
void debug_list(leftv v)
{
  idhdl r=basePackHdl;
  idhdl h;
  BOOLEAN found=FALSE;
  const char *nn=v->name;
  h=IDROOT->get(nn,myynest);
  if (h!=NULL)
  {
     Print("Curr::%s, (%s)\n",nn,Tok2Cmdname((int)IDTYP(h)));
     found=TRUE;
  }
  else         Print("`%s` not found in IDROOT\n",nn);
  while (r!=NULL)
  {
    if ((IDTYP(r)==PACKAGE_CMD)
    || (IDTYP(r)==RING_CMD)
    || (IDTYP(r)==QRING_CMD))
    {
      h=IDPACKAGE(r)->idroot->get(nn,myynest);
      if (h!=NULL)
      {
        Print("%s::%s, (%s)\n",r->id,nn,Tok2Cmdname((int)IDTYP(h)));
        found=TRUE;
      }
      else         Print("%s::%s not found\n",r->id,nn);
    }
    if (r==basePackHdl) r=IDPACKAGE(r)->idroot;
    r=r->next;
   if (r==basePackHdl) break;
  }
  if (!found)
  {
    listall(TRUE);
  }
}
#endif

/* From the bison docu:

     By defining the macro `YYMAXDEPTH', you can control how deep the
parser stack can become before a stack overflow occurs.  Define the
macro with a value that is an integer.  This value is the maximum number
of tokens that can be shifted (and not reduced) before overflow.  It
must be a constant expression whose value is known at compile time.

   The stack space allowed is not necessarily allocated.  If you
specify a large value for `YYMAXDEPTH', the parser actually allocates a
small stack at first, and then makes it bigger by stages as needed.
This increasing allocation happens automatically and silently.
Therefore, you do not need to make `YYMAXDEPTH' painfully small merely
to save space for ordinary inputs that do not need much stack.

   The default value of `YYMAXDEPTH', if you do not define it, is 10000.
*/
#define YYMAXDEPTH INT_MAX

extern int   yylineno;
extern FILE* yyin;

char       my_yylinebuf[80];
const  char *  currid;
BOOLEAN    yyInRingConstruction=FALSE;
BOOLEAN    expected_parms;
int        cmdtok;
int        inerror = 0;

#define TESTSETINT(a,i)                                \
   if ((a).Typ() != INT_CMD)                           \
   {                                                   \
     WerrorS("no int expression");                     \
     YYERROR;                                          \
   }                                                   \
   (i) = (int)((long)(a).Data());

#define MYYERROR(a) { WerrorS(a); YYERROR; }

void yyerror(const char * fmt)
{

  BOOLEAN old_errorreported=errorreported;
  errorreported = TRUE;
  if (currid!=NULL)
  {
    killid(currid,&IDROOT);
    currid = NULL;
  }
  if(inerror==0)
  {
    {
      if ((strlen(fmt)>1)
      && (strncmp(fmt,"parse",5)!=0)
      && (strncmp(fmt,"syntax",6)!=0))
        WerrorS(fmt);
      Werror( "error occurred in or before %s line %d: `%s`"
             ,VoiceName(), yylineno, my_yylinebuf);
    }
    if (cmdtok!=0)
    {
      const char *s=Tok2Cmdname(cmdtok);
      if (expected_parms)
      {
        Werror("expected %s-expression. type \'help %s;\'",s,s);
      }
      else
      {
        Werror("wrong type declaration. type \'help %s;\'",s);
      }
    }
    if (!old_errorreported && (lastreserved!=NULL))
    {
      Werror("last reserved name was `%s`",lastreserved);
    }
    inerror=1;
  }
  if ((currentVoice!=NULL)
  && (currentVoice->prev!=NULL)
  && (myynest>0)
#ifdef HAVE_SDB
  && ((sdb_flags &1)==0)
#endif
  )
  {
    Werror("leaving %s",VoiceName());
  }
  #ifdef HAVE_FACTORY
  // libfac:
  extern int libfac_interruptflag;
  libfac_interruptflag=0;
  #endif
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 514 "grammar.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1814

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  147
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  44
/* YYNRULES -- Number of rules. */
#define YYNRULES  164
/* YYNRULES -- Number of states. */
#define YYNSTATES  355

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   383

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,     2,
     143,   144,   141,   131,   137,   132,   145,   133,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   140,   138,
     129,   128,   130,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   134,     2,   135,   136,     2,   146,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   142
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    15,    17,    19,
      21,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      43,    45,    47,    49,    51,    53,    55,    57,    59,    61,
      64,    66,    68,    72,    76,    80,    85,    89,    91,    93,
      95,   100,   104,   106,   108,   110,   114,   121,   126,   131,
     136,   140,   145,   150,   154,   159,   166,   175,   182,   191,
     196,   201,   208,   213,   220,   229,   238,   242,   247,   256,
     261,   270,   275,   279,   285,   286,   292,   295,   297,   300,
     303,   307,   311,   315,   319,   323,   327,   331,   335,   339,
     343,   346,   349,   352,   355,   357,   361,   364,   367,   370,
     373,   382,   385,   389,   392,   394,   396,   402,   404,   406,
     411,   413,   417,   419,   423,   425,   427,   429,   431,   432,
     437,   441,   444,   448,   451,   454,   458,   463,   468,   473,
     478,   483,   488,   493,   498,   505,   512,   519,   526,   533,
     540,   547,   551,   553,   562,   565,   568,   570,   572,   575,
     578,   580,   586,   589,   595,   597,   599,   603,   609,   613,
     617,   622,   625,   628,   633
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     148,     0,    -1,    -1,   148,   149,    -1,   150,    -1,   152,
     138,    -1,   163,   138,    -1,   190,    -1,   123,    -1,   138,
      -1,     1,   138,    -1,   185,    -1,   186,    -1,   151,    -1,
     187,    -1,   188,    -1,   172,    -1,   174,    -1,   175,    -1,
     102,   111,    -1,   153,    -1,   176,    -1,   177,    -1,   178,
      -1,   189,    -1,   180,    -1,   181,    -1,   183,    -1,   184,
      -1,   161,   155,    -1,   114,    -1,   162,    -1,   154,    11,
     154,    -1,   154,   145,   154,    -1,   154,   143,   144,    -1,
     154,   143,   155,   144,    -1,   134,   155,   135,    -1,   112,
      -1,   127,    -1,   164,    -1,    15,   143,   156,   144,    -1,
     155,   137,   156,    -1,   156,    -1,   160,    -1,   154,    -1,
     143,   155,   144,    -1,   156,   134,   156,   137,   156,   135,
      -1,   156,   134,   156,   135,    -1,    98,   143,   156,   144,
      -1,    99,   143,   155,   144,    -1,    99,   143,   144,    -1,
     100,   143,   156,   144,    -1,   101,   143,   155,   144,    -1,
     101,   143,   144,    -1,    90,   143,   156,   144,    -1,    91,
     143,   156,   137,   156,   144,    -1,    92,   143,   156,   137,
     156,   137,   156,   144,    -1,    95,   143,   156,   137,   156,
     144,    -1,    95,   143,   156,   137,   156,   137,   156,   144,
      -1,    93,   143,   156,   144,    -1,    94,   143,   156,   144,
      -1,    93,   143,   156,   137,   156,   144,    -1,    96,   143,
     156,   144,    -1,    96,   143,   156,   137,   156,   144,    -1,
      94,   143,   156,   137,   156,   137,   156,   144,    -1,    96,
     143,   156,   137,   156,   137,   156,   144,    -1,    97,   143,
     144,    -1,    97,   143,   155,   144,    -1,   171,   143,   156,
     137,   156,   137,   156,   144,    -1,   171,   143,   156,   144,
      -1,    16,   143,   165,   137,   165,   137,   169,   144,    -1,
      16,   143,   156,   144,    -1,   158,   156,   159,    -1,   158,
     156,   128,   156,   159,    -1,    -1,   119,   143,   157,   156,
     144,    -1,   120,   143,    -1,   144,    -1,   156,    10,    -1,
     156,     7,    -1,   156,   131,   156,    -1,   156,   132,   156,
      -1,   156,   133,   156,    -1,   156,   136,   156,    -1,   156,
     129,   156,    -1,   156,   139,   156,    -1,   156,     9,   156,
      -1,   156,     4,   156,    -1,   156,     3,   156,    -1,   156,
     140,   156,    -1,     8,   156,    -1,   132,   156,    -1,   163,
     170,    -1,   155,   128,    -1,   113,    -1,   146,   156,   146,
      -1,    98,   154,    -1,    99,   154,    -1,   100,   154,    -1,
     101,   154,    -1,   171,   154,   134,   156,   135,   134,   156,
     135,    -1,   171,   154,    -1,   163,   137,   154,    -1,    15,
     154,    -1,   110,    -1,   156,    -1,   143,   156,   137,   155,
     144,    -1,   113,    -1,   166,    -1,   166,   143,   155,   144,
      -1,   167,    -1,   167,   137,   168,    -1,   167,    -1,   143,
     168,   144,    -1,   128,    -1,    20,    -1,    14,    -1,    13,
      -1,    -1,   129,   164,   173,   138,    -1,   104,   110,   138,
      -1,   104,   138,    -1,   102,   110,   138,    -1,   103,   155,
      -1,   105,   154,    -1,   177,   137,   154,    -1,   107,   143,
      98,   144,    -1,   107,   143,    99,   144,    -1,   107,   143,
     100,   144,    -1,   107,   143,   101,   144,    -1,   107,   143,
      16,   144,    -1,   107,   143,   171,   144,    -1,   107,   143,
      15,   144,    -1,   107,   143,   154,   144,    -1,   107,   143,
     154,   137,    98,   144,    -1,   107,   143,   154,   137,    99,
     144,    -1,   107,   143,   154,   137,   100,   144,    -1,   107,
     143,   154,   137,   101,   144,    -1,   107,   143,   154,   137,
      16,   144,    -1,   107,   143,   154,   137,   171,   144,    -1,
     107,   143,   154,   137,    15,   144,    -1,   107,   143,   144,
      -1,    16,    -1,   179,   154,   170,   165,   137,   165,   137,
     169,    -1,   179,   154,    -1,   127,   164,    -1,   108,    -1,
      40,    -1,   182,   156,    -1,   109,   156,    -1,   155,    -1,
     122,   143,   156,   144,   111,    -1,   118,   111,    -1,   122,
     143,   156,   144,   116,    -1,   116,    -1,   117,    -1,   124,
     110,   111,    -1,   121,   110,   110,   110,   111,    -1,    15,
     162,   111,    -1,   115,   110,   111,    -1,   115,   110,   110,
     111,    -1,   126,   163,    -1,   126,   156,    -1,   125,   143,
     155,   144,    -1,   125,   143,   144,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   359,   359,   361,   395,   396,   398,   400,   404,   409,
     411,   462,   463,   464,   465,   466,   467,   468,   469,   473,
     476,   477,   478,   479,   480,   481,   482,   483,   484,   487,
     494,   499,   503,   507,   511,   515,   528,   556,   580,   586,
     592,   599,   610,   616,   621,   622,   623,   627,   631,   635,
     639,   643,   647,   651,   655,   659,   663,   667,   671,   675,
     679,   683,   687,   691,   695,   699,   703,   707,   711,   715,
     719,   723,   727,   731,   749,   748,   766,   774,   783,   787,
     791,   795,   799,   803,   807,   811,   815,   819,   823,   827,
     831,   838,   845,   846,   865,   866,   878,   883,   888,   892,
     896,   929,   953,   974,   982,   986,   987,  1001,  1009,  1018,
    1063,  1064,  1073,  1074,  1080,  1086,  1088,  1090,  1100,  1099,
    1107,  1112,  1119,  1127,  1139,  1155,  1174,  1178,  1182,  1187,
    1191,  1195,  1199,  1203,  1208,  1214,  1220,  1226,  1232,  1238,
    1244,  1256,  1263,  1267,  1304,  1314,  1327,  1327,  1330,  1402,
    1406,  1435,  1448,  1465,  1474,  1479,  1487,  1499,  1518,  1529,
    1549,  1573,  1579,  1591,  1598
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "GRING_CMD",
  "BIGINTMAT_CMD", "INTMAT_CMD", "PROC_CMD", "RING_CMD", "BEGIN_RING",
  "IDEAL_CMD", "MAP_CMD", "MATRIX_CMD", "MODUL_CMD", "NUMBER_CMD",
  "POLY_CMD", "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD", "COEFFS_CMD",
  "COEF_CMD", "CONTRACT_CMD", "DEGREE_CMD", "DEG_CMD", "DIFF_CMD",
  "DIM_CMD", "DIVISION_CMD", "ELIMINATION_CMD", "E_CMD", "FAREY_CMD",
  "FETCH_CMD", "FREEMODULE_CMD", "KEEPRING_CMD", "HILBERT_CMD",
  "HOMOG_CMD", "IMAP_CMD", "INDEPSET_CMD", "INTERRED_CMD", "INTERSECT_CMD",
  "JACOB_CMD", "JET_CMD", "KBASE_CMD", "KOSZUL_CMD", "LEADCOEF_CMD",
  "LEADEXP_CMD", "LEAD_CMD", "LEADMONOM_CMD", "LIFTSTD_CMD", "LIFT_CMD",
  "MAXID_CMD", "MINBASE_CMD", "MINOR_CMD", "MINRES_CMD", "MODULO_CMD",
  "MONOM_CMD", "MRES_CMD", "MULTIPLICITY_CMD", "ORD_CMD", "PAR_CMD",
  "PARDEG_CMD", "PREIMAGE_CMD", "QUOTIENT_CMD", "QHWEIGHT_CMD",
  "REDUCE_CMD", "REGULARITY_CMD", "RES_CMD", "SIMPLIFY_CMD", "SORTVEC_CMD",
  "SRES_CMD", "STD_CMD", "SUBST_CMD", "SYZYGY_CMD", "VAR_CMD", "VDIM_CMD",
  "WEDGE_CMD", "WEIGHT_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT", "VNOETHER",
  "VMINPOLY", "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12", "CMD_13",
  "CMD_23", "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST", "RING_DECL",
  "RING_DECL_LIST", "EXAMPLE_CMD", "EXPORT_CMD", "HELP_CMD", "KILL_CMD",
  "LIB_CMD", "LISTVAR_CMD", "SETRING_CMD", "TYPE_CMD", "STRINGTOK",
  "BLOCKTOK", "INT_CONST", "UNKNOWN_IDENT", "RINGVAR", "PROC_DEF",
  "BREAK_CMD", "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD",
  "IF_CMD", "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR",
  "'='", "'<'", "'>'", "'+'", "'-'", "'/'", "'['", "']'", "'^'", "','",
  "';'", "'&'", "':'", "'*'", "UMINUS", "'('", "')'", "'.'", "'`'",
  "$accept", "lines", "pprompt", "flowctrl", "example_dummy", "command",
  "assign", "elemexpr", "exprlist", "expr", "@1", "quote_start",
  "quote_end", "expr_arithmetic", "left_value", "extendedid",
  "declare_ip_variable", "stringexpr", "rlist", "ordername", "orderelem",
  "OrderingList", "ordering", "cmdeq", "mat_cmd", "filecmd", "@2",
  "helpcmd", "examplecmd", "exportcmd", "killcmd", "listcmd", "ringcmd1",
  "ringcmd", "scriptcmd", "setrings", "setringcmd", "typecmd", "ifcmd",
  "whilecmd", "forcmd", "proccmd", "parametercmd", "returncmd", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,    61,    60,
      62,    43,    45,    47,    91,    93,    94,    44,    59,    38,
      58,    42,   383,    40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   147,   148,   148,   149,   149,   149,   149,   149,   149,
     149,   150,   150,   150,   150,   150,   150,   150,   150,   151,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   153,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   155,   155,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   157,   156,   158,   159,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   161,   161,   162,   162,   163,   163,   163,   163,
     163,   163,   163,   163,   164,   165,   165,   166,   167,   167,
     168,   168,   169,   169,   170,   171,   171,   171,   173,   172,
     174,   174,   175,   176,   177,   177,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   179,   180,   180,   181,   182,   182,   183,   184,
     184,   185,   185,   185,   185,   185,   186,   187,   188,   188,
     188,   189,   189,   190,   190
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       4,     3,     1,     1,     1,     3,     6,     4,     4,     4,
       3,     4,     4,     3,     4,     6,     8,     6,     8,     4,
       4,     6,     4,     6,     8,     8,     3,     4,     8,     4,
       8,     4,     3,     5,     0,     5,     2,     1,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     1,     3,     2,     2,     2,     2,
       8,     2,     3,     2,     1,     1,     5,     1,     1,     4,
       1,     3,     1,     3,     1,     1,     1,     1,     0,     4,
       3,     2,     3,     2,     2,     3,     4,     4,     4,     4,
       4,     4,     4,     4,     6,     6,     6,     6,     6,     6,
       6,     3,     1,     8,     2,     2,     1,     1,     2,     2,
       1,     5,     2,     5,     1,     1,     3,     5,     3,     3,
       4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,   117,   116,     0,   142,   115,
     147,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,     0,
     104,    37,    94,    30,     0,   154,   155,     0,     0,     0,
       0,     0,     8,     0,     0,     0,    38,     0,     0,     0,
       9,     0,     0,     3,     4,    13,     0,    20,    44,   150,
      42,     0,    43,     0,    31,     0,    39,     0,    16,    17,
      18,    21,    22,    23,     0,    25,    26,     0,    27,    28,
      11,    12,    14,    15,    24,     7,    10,     0,     0,     0,
       0,     0,     0,    38,    90,     0,     0,   103,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    96,
       0,    97,     0,    98,     0,    99,     0,    19,   123,     0,
     121,   124,     0,   149,     0,   152,    74,    76,     0,     0,
       0,     0,     0,   162,   161,   145,   118,    91,     0,     0,
       0,     5,     0,     0,     0,    93,     0,     0,     0,    79,
       0,    78,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,   114,     0,     6,    92,     0,   101,     0,   144,
     148,     0,   158,     0,   105,     0,     0,     0,     0,     0,
       0,     0,     0,    66,     0,     0,    50,     0,     0,    53,
       0,   122,   120,     0,     0,     0,     0,     0,     0,   141,
       0,     0,     0,   159,     0,     0,     0,   156,   164,     0,
       0,    36,    45,    95,    32,    34,     0,    33,    41,    88,
      87,    86,    84,    80,    81,    82,     0,    83,    85,    89,
       0,    77,    72,   102,     0,     0,   125,     0,    40,    42,
      71,     0,    54,     0,     0,     0,    59,     0,    60,     0,
       0,    62,    67,    48,    49,    51,    52,   132,   130,   126,
     127,   128,   129,     0,   133,   131,   160,     0,     0,     0,
     163,   119,    35,    47,     0,     0,     0,    69,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,   157,   151,   153,
       0,    73,     0,     0,     0,     0,     0,    55,     0,    61,
       0,     0,    57,     0,    63,   140,   138,   134,   135,   136,
     137,   139,    46,     0,     0,     0,   106,   107,     0,   108,
     112,     0,     0,     0,     0,     0,     0,     0,     0,   110,
       0,     0,    70,    56,    64,    58,    65,    68,   100,   143,
       0,   113,     0,   111,   109
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,    53,    54,    55,    56,    57,    58,   139,    60,
     204,    61,   232,    62,    63,    64,    65,    66,   175,   329,
     330,   340,   331,   165,    95,    68,   210,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -319
static const short int yypact[] =
{
    -319,   253,  -319,  -134,  1202,  -319,  -319,   366,  -129,  -319,
    -319,  -125,  -121,  -105,  -100,   -69,   -63,   -60,   -52,   890,
    1527,  1566,  1650,   -54,  1202,  -107,  1427,   -50,  -319,  1202,
    -319,  -319,  -319,  -319,   -80,  -319,  -319,   -53,   -49,   -46,
     -12,   -43,  -319,    -7,   -39,  1260,    -5,    -5,  1202,  1202,
    -319,  1202,  1202,  -319,  -319,  -319,   -77,  -319,     2,  -113,
    1589,  1202,  -319,  1202,  -319,   -87,  -319,  1660,  -319,  -319,
    -319,  -319,   -30,  -319,  1427,  -319,  -319,  1202,  -319,  -319,
    -319,  -319,  -319,  -319,  -319,  -319,  -319,   -35,  -129,   -31,
     -19,   -18,   -17,  -319,    19,   -15,  1202,     2,    23,  1317,
    1202,  1202,  1202,  1202,  1202,  1202,  1202,   915,  1202,     2,
     972,     2,  1202,     2,  1030,     2,   -28,  -319,    -8,    11,
    -319,     2,  1668,  1589,   -33,  -319,  -319,  -319,    30,  1202,
      46,  1087,   366,  1589,    22,  -319,  -319,    19,   -82,  -117,
      61,  -319,  1427,  1145,  1427,  -319,  1202,  1202,  1202,  -319,
    1202,  -319,  1202,  1202,  1202,  1202,  1202,  1202,  1202,  1202,
      92,    -8,  -319,  1427,  -319,  -319,  1202,    -2,  1427,     5,
    1589,   443,  -319,  1202,   464,    27,   478,  1337,  1359,   176,
     272,  1374,   291,  -319,   -98,   492,  -319,   -95,   523,  -319,
     -90,  -319,  -319,   -58,    16,    31,    32,    34,    40,  -319,
      -6,    45,    55,  -319,  1202,    81,   537,  -319,  -319,   -85,
      49,  -319,  -319,  -319,    51,  -319,   -71,  -319,  1589,  1613,
      77,    77,   403,    18,    18,    19,   827,     1,  1625,    18,
    1202,  -319,  -319,     2,   326,  1202,     2,  1317,  -319,  1386,
    -319,  1317,  -319,  1202,  1202,  1202,  -319,  1202,  -319,  1202,
    1202,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,
    -319,  -319,  -319,   633,  -319,  -319,  -319,   581,    87,   -99,
    -319,  -319,  -319,  -319,  1202,   630,  1202,  -319,  1416,  1589,
      62,  1202,    65,   675,  1431,   689,  1443,   385,   429,    59,
      60,    68,    70,    71,    72,    74,  -319,  -319,  -319,  -319,
    1455,  -319,  1475,    88,  1317,   -65,  -106,  -319,  1202,  -319,
    1202,  1202,  -319,  1202,  -319,  -319,  -319,  -319,  -319,  -319,
    -319,  -319,  -319,  1202,  1202,    90,  -319,  -319,   116,    91,
    -319,    86,   719,   733,   747,   768,   782,  1524,  -106,    96,
      93,  1202,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,
     116,  -319,   -62,  -319,  -319
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -319,  -319,  -319,  -319,  -319,  -319,  -319,    14,    -1,    15,
    -319,  -319,   -70,  -319,  -319,   228,   193,    43,  -235,  -319,
    -318,  -111,   -97,    73,     0,  -319,  -319,  -319,  -319,  -319,
    -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,
    -319,  -319,  -319,  -319
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const short int yytable[] =
{
      59,    67,   280,   119,    86,   142,   282,   327,   149,   142,
     339,   151,   298,   142,    99,   145,   142,   299,   100,    94,
     146,    97,   101,   118,   146,   149,   149,   212,   151,   151,
     124,   120,   339,   109,   111,   113,   115,   328,   102,   146,
     121,   162,   146,   103,   123,    67,   252,   146,   138,   254,
     163,   164,   146,   211,   256,   146,   116,   117,   125,   270,
     133,   141,   161,   137,   147,   148,   146,   140,   149,   325,
     150,   151,   146,   272,   104,   146,   160,   202,   203,   326,
     105,   167,   354,   106,   149,    96,   257,   151,   169,   135,
     136,   107,   170,   122,   126,   147,   148,   127,   128,   149,
     129,   150,   151,   130,   131,    30,   184,   168,    96,   187,
     191,   171,   108,   190,   174,   176,   177,   178,   179,   180,
     181,   182,   201,   185,   110,   112,   114,   188,   166,   146,
     209,   263,   235,   162,   172,   156,   200,   143,   264,   144,
     205,   143,   216,   144,   206,   143,    97,   144,   143,   192,
     144,   155,   156,   156,   157,   157,   214,   207,   217,   163,
     258,   218,   219,   220,   241,   221,   266,   222,   223,   224,
     225,   226,   227,   228,   229,   259,   260,   233,   261,   147,
     148,   234,   236,   149,   262,   150,   151,   271,   239,   265,
     152,   268,   153,   154,   155,   156,   144,   157,   297,   304,
     158,   159,   306,   315,   316,   301,   152,   213,   153,   154,
     155,   156,   317,   157,   318,   319,   320,   159,   321,   267,
     230,   152,   324,   153,   154,   155,   156,   338,   157,   327,
     342,   158,   159,   350,   341,    98,   231,   351,   134,   353,
       0,   349,   237,     0,     0,   275,     0,     0,     0,     0,
     278,     0,   279,     2,     3,     0,   279,     0,   283,   284,
     285,     4,   286,   295,   287,   288,     5,     6,     7,     8,
       0,     0,     0,     9,     0,   147,   148,     0,     0,   149,
     305,   150,   151,     0,     0,     0,     0,     0,     0,   300,
       0,   302,     0,    10,   147,   148,     0,     0,   149,     0,
     150,   151,     0,     0,     0,   152,     0,   153,   154,   155,
     156,     0,   157,   245,     0,   158,   159,     0,     0,   279,
     246,     0,     0,   332,     0,   333,   334,     0,   335,   147,
     148,     0,     0,   149,     0,   150,   151,     0,   336,   337,
     352,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
      27,    28,    29,    30,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    87,    47,     0,     0,    48,     0,    49,   147,   148,
       0,    50,   149,     0,   150,   151,    51,     0,     0,    52,
       0,   152,     0,   153,   154,   155,   156,     0,   157,   247,
     149,   158,   159,   151,     0,     0,   248,     0,     0,     0,
     152,     0,   153,   154,   155,   156,     0,   157,   250,     0,
     158,   159,   147,   148,     0,   251,   149,     0,   150,   151,
       0,     0,     0,     0,     0,     0,   147,   148,     0,     0,
     149,     0,   150,   151,     0,   152,     0,   153,   154,   155,
     156,     0,   157,   276,     0,   158,   159,   147,   148,     0,
     277,   149,     0,   150,   151,     0,    30,     0,    31,    32,
      33,   147,   148,     0,     0,   149,     0,   150,   151,     0,
       0,     0,     0,    93,     0,   147,   148,     0,     0,   149,
      49,   150,   151,     0,     0,     0,     0,     0,     0,    96,
       0,     0,    52,     0,   152,     0,   153,   154,   155,   156,
       0,   157,   311,     0,   158,   159,   147,   148,     0,   312,
     149,     0,   150,   151,   153,   154,   155,   156,     0,   157,
     147,   148,     0,   159,   149,     0,   150,   151,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   152,     0,
     153,   154,   155,   156,     0,   157,   313,     0,   158,   159,
       0,     0,   152,   314,   153,   154,   155,   156,     0,   157,
       0,     0,   158,   159,   147,   148,     0,   238,   149,     0,
     150,   151,     0,   152,     0,   153,   154,   155,   156,     0,
     157,     0,     0,   158,   159,     0,     0,   152,   240,   153,
     154,   155,   156,     0,   157,     0,     0,   158,   159,     0,
       0,   152,   242,   153,   154,   155,   156,     0,   157,     0,
       0,   158,   159,   147,   148,     0,   253,   149,     0,   150,
     151,     0,     0,     0,     0,     0,     5,     6,   289,   290,
       0,     0,   152,     9,   153,   154,   155,   156,     0,   157,
       0,     0,   158,   159,     0,     0,   152,   255,   153,   154,
     155,   156,     0,   157,     0,     0,   158,   159,   147,   148,
       0,   269,   149,     0,   150,   151,     0,     0,     0,     0,
       0,     0,   147,   148,     0,     0,   149,     0,   150,   151,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     152,     0,   153,   154,   155,   156,     0,   157,     0,     0,
     158,   159,   147,   148,     0,   296,   149,     0,   150,   151,
       0,   291,   292,   293,   294,     0,   147,   148,     0,     0,
     149,     0,   150,   151,     0,     0,     0,     0,     0,     0,
     147,   148,     0,     0,   149,     0,   150,   151,     0,   152,
       0,   153,   154,   155,   156,     0,   157,     0,     0,   158,
     159,   147,   148,     0,   231,   149,     0,   150,   151,     0,
       0,     0,     0,     0,     0,   147,   148,     0,     0,   149,
       0,   150,   151,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   152,     0,   153,   154,   155,   156,
       0,   157,     0,     0,   158,   159,     0,     0,   152,   307,
     153,   154,   155,   156,     0,   157,     0,     0,   158,   159,
     147,   148,     0,   309,   149,     0,   150,   151,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   152,     0,
     153,   154,   155,   156,     0,   157,     0,     0,   158,   159,
       0,     0,   152,   343,   153,   154,   155,   156,     0,   157,
       0,     0,   158,   159,     0,     0,   152,   344,   153,   154,
     155,   156,     0,   157,     0,     0,   158,   159,     0,     0,
       0,   345,     0,     0,     0,     0,     0,   152,     0,   153,
     154,   155,   156,     0,   157,    87,     0,   158,   159,     0,
       0,   152,   346,   153,   154,   155,   156,     0,   157,     0,
       0,   158,   159,     4,     0,     0,   347,     0,     5,     6,
      87,    88,     0,     0,     0,     9,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   152,     0,   153,   154,
     155,   156,   273,   157,   274,     0,   158,   159,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     0,     0,     0,     5,     6,    87,    88,     0,
       0,     0,     9,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,    11,    12,    13,    14,    15,
      16,    17,    18,    89,    90,    91,    92,    93,     0,     0,
       0,     0,     0,     0,    49,    30,     0,    31,    32,    33,
       0,     0,     0,   108,    38,    39,    52,     0,     4,     0,
       0,     0,    93,     5,     6,    87,    88,    48,     0,    49,
       9,     0,     0,     0,     0,     0,     0,     0,    51,   183,
       0,    52,    11,    12,    13,    14,    15,    16,    17,    18,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    30,     0,    31,    32,    33,     0,     0,     0,
       0,    38,    39,     0,     0,     4,     0,     0,     0,    93,
       5,     6,    87,    88,    48,     0,    49,     9,     0,     0,
       0,     0,     0,     0,     0,    51,   186,     0,    52,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    89,    90,
      91,    92,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,     0,     0,     0,    38,
      39,     0,     0,     4,     0,     0,     0,    93,     5,     6,
      87,    88,    48,     0,    49,     9,     0,     0,     0,     0,
       0,     0,     0,    51,   189,     0,    52,    11,    12,    13,
      14,    15,    16,    17,    18,    89,    90,    91,    92,     0,
       0,     0,     0,     0,     0,     0,     0,    30,     0,    31,
      32,    33,     0,     0,     0,     0,    38,    39,     0,     0,
       4,     0,     0,     0,    93,     5,     6,    87,    88,    48,
       0,    49,     9,     0,     0,     0,     0,     0,     0,     0,
      51,   208,     0,    52,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    89,    90,    91,    92,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,     0,     0,     0,    38,    39,     0,     0,     4,     0,
       0,     0,    93,     5,     6,   132,    88,    48,     0,    49,
       9,     0,     0,     0,     0,     0,     0,     0,    51,   215,
       0,    52,    11,    12,    13,    14,    15,    16,    17,    18,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    30,     0,    31,    32,    33,     0,     0,     0,
       0,    38,    39,     0,     0,     4,     0,     0,     0,    93,
       5,     6,    87,    88,    48,     0,    49,     9,     0,     0,
     147,   148,     0,     0,   149,    51,   150,   151,    52,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,   147,   148,     0,     0,   149,     0,   150,   151,
      30,     0,    31,    32,    33,     0,     0,   147,   148,    38,
      39,   149,     0,   150,   151,     0,     0,    93,     0,   147,
     148,     0,    48,   149,    49,   150,   151,     0,     0,     0,
       0,     0,     0,    51,     0,     0,    52,    11,    12,    13,
      14,    15,    16,    17,    18,    89,    90,    91,    92,   147,
     148,     0,     0,   149,     0,   150,   151,    30,     0,    31,
      32,    33,     0,     0,   147,   148,    38,    39,   149,     0,
     150,   151,    87,     0,    93,     0,   147,   148,     0,    48,
     149,    49,   150,   151,     0,     0,     0,     0,   147,   148,
     173,     0,   149,    52,   150,   151,   152,     0,   153,   154,
     155,   156,     0,   157,   243,     0,   158,   159,   147,   148,
       0,     0,   149,     0,   150,   151,     0,     0,   152,     0,
     153,   154,   155,   156,     0,   157,   244,     0,   158,   159,
       0,     0,     0,   152,     0,   153,   154,   155,   156,     0,
     157,   249,     0,   158,   159,   152,     0,   153,   154,   155,
     156,     0,   157,   281,     0,   158,   159,   147,   148,     0,
       0,   149,     0,   150,   151,     0,     0,    30,     0,    31,
      32,    33,    87,     0,     0,   152,     0,   153,   154,   155,
     156,   303,   157,     0,    93,   158,   159,     0,     0,     0,
     152,    49,   153,   154,   155,   156,     0,   157,   308,     0,
     158,   159,   152,    52,   153,   154,   155,   156,     0,   157,
     310,    87,   158,   159,   152,     0,   153,   154,   155,   156,
     322,   157,   147,   148,   158,   159,   149,     0,   150,   151,
       0,     0,     0,     0,   152,     0,   153,   154,   155,   156,
       0,   157,   323,     0,   158,   159,    -1,   148,     0,     0,
     149,     0,   150,   151,     0,     0,     0,     0,     0,   148,
       0,     0,   149,     0,   150,   151,     0,    30,     0,    31,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,    93,   153,   154,   155,   156,   348,
     157,    49,     0,   158,   159,    87,     0,     0,     0,     0,
     110,     0,     0,    52,     0,    87,    30,     0,    31,    32,
      33,     5,     6,   193,   194,     0,     0,     0,     9,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
      49,     0,     0,     0,     0,     0,     0,     0,     0,   112,
       0,     0,    52,     0,     0,     0,     0,     0,   152,     0,
     153,   154,   155,   156,     0,   157,     0,     0,   158,   159,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   152,     0,   153,   154,   155,   156,     0,   157,
       0,     0,   158,   159,   152,     0,   153,   154,   155,   156,
      30,   157,    31,    32,    33,   159,   195,   196,   197,   198,
      30,     0,    31,    32,    33,     0,     0,    93,    30,     0,
      31,    32,    33,     0,    49,     0,     0,    93,     0,     0,
       0,     0,     0,   114,    49,    93,    52,     0,     0,     0,
       0,     0,    49,   166,     0,     0,    52,     0,     0,     0,
       0,     0,   199,     0,    52
};

static const short int yycheck[] =
{
       1,     1,   237,   110,   138,    11,   241,   113,     7,    11,
     328,    10,   111,    11,   143,   128,    11,   116,   143,     4,
     137,     7,   143,    24,   137,     7,     7,   144,    10,    10,
     110,   138,   350,    19,    20,    21,    22,   143,   143,   137,
      26,   128,   137,   143,    29,    45,   144,   137,    49,   144,
     137,   138,   137,   135,   144,   137,   110,   111,   111,   144,
      45,   138,    63,    48,     3,     4,   137,    52,     7,   304,
       9,    10,   137,   144,   143,   137,    61,   110,   111,   144,
     143,    67,   144,   143,     7,   143,   144,    10,    74,    46,
      47,   143,    77,   143,   143,     3,     4,   143,   110,     7,
     143,     9,    10,   110,   143,   110,   107,   137,   143,   110,
     138,    96,   143,   114,    99,   100,   101,   102,   103,   104,
     105,   106,   122,   108,   143,   143,   143,   112,   143,   137,
     131,   137,   134,   128,   111,   134,   122,   143,   144,   145,
     110,   143,   143,   145,   129,   143,   132,   145,   143,   138,
     145,   133,   134,   134,   136,   136,   142,   111,   144,   137,
     144,   146,   147,   148,   137,   150,   111,   152,   153,   154,
     155,   156,   157,   158,   159,   144,   144,   163,   144,     3,
       4,   166,   168,     7,   144,     9,    10,   138,   173,   144,
     129,   110,   131,   132,   133,   134,   145,   136,   111,   137,
     139,   140,   137,   144,   144,   275,   129,   146,   131,   132,
     133,   134,   144,   136,   144,   144,   144,   140,   144,   204,
     128,   129,   134,   131,   132,   133,   134,   137,   136,   113,
     144,   139,   140,   137,   143,     7,   144,   144,    45,   350,
      -1,   338,   169,    -1,    -1,   230,    -1,    -1,    -1,    -1,
     235,    -1,   237,     0,     1,    -1,   241,    -1,   243,   244,
     245,     8,   247,   263,   249,   250,    13,    14,    15,    16,
      -1,    -1,    -1,    20,    -1,     3,     4,    -1,    -1,     7,
     281,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,   274,
      -1,   276,    -1,    40,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,   129,    -1,   131,   132,   133,
     134,    -1,   136,   137,    -1,   139,   140,    -1,    -1,   304,
     144,    -1,    -1,   308,    -1,   310,   311,    -1,   313,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,   323,   324,
     341,    -1,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,    15,   129,    -1,    -1,   132,    -1,   134,     3,     4,
      -1,   138,     7,    -1,     9,    10,   143,    -1,    -1,   146,
      -1,   129,    -1,   131,   132,   133,   134,    -1,   136,   137,
       7,   139,   140,    10,    -1,    -1,   144,    -1,    -1,    -1,
     129,    -1,   131,   132,   133,   134,    -1,   136,   137,    -1,
     139,   140,     3,     4,    -1,   144,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,   129,    -1,   131,   132,   133,
     134,    -1,   136,   137,    -1,   139,   140,     3,     4,    -1,
     144,     7,    -1,     9,    10,    -1,   110,    -1,   112,   113,
     114,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,   127,    -1,     3,     4,    -1,    -1,     7,
     134,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,   143,
      -1,    -1,   146,    -1,   129,    -1,   131,   132,   133,   134,
      -1,   136,   137,    -1,   139,   140,     3,     4,    -1,   144,
       7,    -1,     9,    10,   131,   132,   133,   134,    -1,   136,
       3,     4,    -1,   140,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,
     131,   132,   133,   134,    -1,   136,   137,    -1,   139,   140,
      -1,    -1,   129,   144,   131,   132,   133,   134,    -1,   136,
      -1,    -1,   139,   140,     3,     4,    -1,   144,     7,    -1,
       9,    10,    -1,   129,    -1,   131,   132,   133,   134,    -1,
     136,    -1,    -1,   139,   140,    -1,    -1,   129,   144,   131,
     132,   133,   134,    -1,   136,    -1,    -1,   139,   140,    -1,
      -1,   129,   144,   131,   132,   133,   134,    -1,   136,    -1,
      -1,   139,   140,     3,     4,    -1,   144,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    13,    14,    15,    16,
      -1,    -1,   129,    20,   131,   132,   133,   134,    -1,   136,
      -1,    -1,   139,   140,    -1,    -1,   129,   144,   131,   132,
     133,   134,    -1,   136,    -1,    -1,   139,   140,     3,     4,
      -1,   144,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    -1,   131,   132,   133,   134,    -1,   136,    -1,    -1,
     139,   140,     3,     4,    -1,   144,     7,    -1,     9,    10,
      -1,    98,    99,   100,   101,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,   129,
      -1,   131,   132,   133,   134,    -1,   136,    -1,    -1,   139,
     140,     3,     4,    -1,   144,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,   131,   132,   133,   134,
      -1,   136,    -1,    -1,   139,   140,    -1,    -1,   129,   144,
     131,   132,   133,   134,    -1,   136,    -1,    -1,   139,   140,
       3,     4,    -1,   144,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,
     131,   132,   133,   134,    -1,   136,    -1,    -1,   139,   140,
      -1,    -1,   129,   144,   131,   132,   133,   134,    -1,   136,
      -1,    -1,   139,   140,    -1,    -1,   129,   144,   131,   132,
     133,   134,    -1,   136,    -1,    -1,   139,   140,    -1,    -1,
      -1,   144,    -1,    -1,    -1,    -1,    -1,   129,    -1,   131,
     132,   133,   134,    -1,   136,    15,    -1,   139,   140,    -1,
      -1,   129,   144,   131,   132,   133,   134,    -1,   136,    -1,
      -1,   139,   140,     8,    -1,    -1,   144,    -1,    13,    14,
      15,    16,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,   131,   132,
     133,   134,   135,   136,   137,    -1,   139,   140,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       8,    -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,   112,   113,   114,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   127,    -1,    -1,
      -1,    -1,    -1,    -1,   134,   110,    -1,   112,   113,   114,
      -1,    -1,    -1,   143,   119,   120,   146,    -1,     8,    -1,
      -1,    -1,   127,    13,    14,    15,    16,   132,    -1,   134,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,
      -1,   146,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   110,    -1,   112,   113,   114,    -1,    -1,    -1,
      -1,   119,   120,    -1,    -1,     8,    -1,    -1,    -1,   127,
      13,    14,    15,    16,   132,    -1,   134,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   143,   144,    -1,   146,    -1,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,   112,   113,   114,    -1,    -1,    -1,    -1,   119,
     120,    -1,    -1,     8,    -1,    -1,    -1,   127,    13,    14,
      15,    16,   132,    -1,   134,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,   144,    -1,   146,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,
     113,   114,    -1,    -1,    -1,    -1,   119,   120,    -1,    -1,
       8,    -1,    -1,    -1,   127,    13,    14,    15,    16,   132,
      -1,   134,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     143,   144,    -1,   146,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   110,    -1,   112,   113,   114,
      -1,    -1,    -1,    -1,   119,   120,    -1,    -1,     8,    -1,
      -1,    -1,   127,    13,    14,    15,    16,   132,    -1,   134,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,
      -1,   146,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   110,    -1,   112,   113,   114,    -1,    -1,    -1,
      -1,   119,   120,    -1,    -1,     8,    -1,    -1,    -1,   127,
      13,    14,    15,    16,   132,    -1,   134,    20,    -1,    -1,
       3,     4,    -1,    -1,     7,   143,     9,    10,   146,    -1,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,     3,     4,    -1,    -1,     7,    -1,     9,    10,
     110,    -1,   112,   113,   114,    -1,    -1,     3,     4,   119,
     120,     7,    -1,     9,    10,    -1,    -1,   127,    -1,     3,
       4,    -1,   132,     7,   134,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,   143,    -1,    -1,   146,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,     3,
       4,    -1,    -1,     7,    -1,     9,    10,   110,    -1,   112,
     113,   114,    -1,    -1,     3,     4,   119,   120,     7,    -1,
       9,    10,    15,    -1,   127,    -1,     3,     4,    -1,   132,
       7,   134,     9,    10,    -1,    -1,    -1,    -1,     3,     4,
     143,    -1,     7,   146,     9,    10,   129,    -1,   131,   132,
     133,   134,    -1,   136,   137,    -1,   139,   140,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,   129,    -1,
     131,   132,   133,   134,    -1,   136,   137,    -1,   139,   140,
      -1,    -1,    -1,   129,    -1,   131,   132,   133,   134,    -1,
     136,   137,    -1,   139,   140,   129,    -1,   131,   132,   133,
     134,    -1,   136,   137,    -1,   139,   140,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,   110,    -1,   112,
     113,   114,    15,    -1,    -1,   129,    -1,   131,   132,   133,
     134,   135,   136,    -1,   127,   139,   140,    -1,    -1,    -1,
     129,   134,   131,   132,   133,   134,    -1,   136,   137,    -1,
     139,   140,   129,   146,   131,   132,   133,   134,    -1,   136,
     137,    15,   139,   140,   129,    -1,   131,   132,   133,   134,
     135,   136,     3,     4,   139,   140,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,   129,    -1,   131,   132,   133,   134,
      -1,   136,   137,    -1,   139,   140,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,   110,    -1,   112,
     113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   129,   127,   131,   132,   133,   134,   135,
     136,   134,    -1,   139,   140,    15,    -1,    -1,    -1,    -1,
     143,    -1,    -1,   146,    -1,    15,   110,    -1,   112,   113,
     114,    13,    14,    15,    16,    -1,    -1,    -1,    20,    -1,
      -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,
     134,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,
      -1,    -1,   146,    -1,    -1,    -1,    -1,    -1,   129,    -1,
     131,   132,   133,   134,    -1,   136,    -1,    -1,   139,   140,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    -1,   131,   132,   133,   134,    -1,   136,
      -1,    -1,   139,   140,   129,    -1,   131,   132,   133,   134,
     110,   136,   112,   113,   114,   140,    98,    99,   100,   101,
     110,    -1,   112,   113,   114,    -1,    -1,   127,   110,    -1,
     112,   113,   114,    -1,   134,    -1,    -1,   127,    -1,    -1,
      -1,    -1,    -1,   143,   134,   127,   146,    -1,    -1,    -1,
      -1,    -1,   134,   143,    -1,    -1,   146,    -1,    -1,    -1,
      -1,    -1,   144,    -1,   146
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   148,     0,     1,     8,    13,    14,    15,    16,    20,
      40,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   107,   108,   109,
     110,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   129,   132,   134,
     138,   143,   146,   149,   150,   151,   152,   153,   154,   155,
     156,   158,   160,   161,   162,   163,   164,   171,   172,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   138,    15,    16,    98,
      99,   100,   101,   127,   156,   171,   143,   154,   162,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   154,
     143,   154,   143,   154,   143,   154,   110,   111,   155,   110,
     138,   154,   143,   156,   110,   111,   143,   143,   110,   143,
     110,   143,    15,   156,   163,   164,   164,   156,   155,   155,
     156,   138,    11,   143,   145,   128,   137,     3,     4,     7,
       9,    10,   129,   131,   132,   133,   134,   136,   139,   140,
     156,   155,   128,   137,   138,   170,   143,   154,   137,   154,
     156,   156,   111,   143,   156,   165,   156,   156,   156,   156,
     156,   156,   156,   144,   155,   156,   144,   155,   156,   144,
     155,   138,   138,    15,    16,    98,    99,   100,   101,   144,
     154,   171,   110,   111,   157,   110,   156,   111,   144,   155,
     173,   135,   144,   146,   154,   144,   155,   154,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     128,   144,   159,   154,   156,   134,   154,   170,   144,   156,
     144,   137,   144,   137,   137,   137,   144,   137,   144,   137,
     137,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   137,   144,   144,   111,   156,   110,   144,
     144,   138,   144,   135,   137,   156,   137,   144,   156,   156,
     165,   137,   165,   156,   156,   156,   156,   156,   156,    15,
      16,    98,    99,   100,   101,   171,   144,   111,   111,   116,
     156,   159,   156,   135,   137,   155,   137,   144,   137,   144,
     137,   137,   144,   137,   144,   144,   144,   144,   144,   144,
     144,   144,   135,   137,   134,   165,   144,   113,   143,   166,
     167,   169,   156,   156,   156,   156,   156,   156,   137,   167,
     168,   143,   144,   144,   144,   144,   144,   144,   135,   169,
     137,   144,   155,   168,   144
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 362 "grammar.y"
    {
            if (timerv)
            {
              writeTime("used time:");
              startTimer();
            }
            if (rtimerv)
            {
              writeRTime("used real time:");
              startRTimer();
            }
            prompt_char = '>';
#ifdef HAVE_SDB
            if (sdb_flags & 2) { sdb_flags=1; YYERROR; }
#endif
            if(siCntrlc)
            {
              WerrorS("abort...");
              while((currentVoice!=NULL) && (currentVoice->prev!=NULL)) exitVoice();
              if (currentVoice!=NULL) currentVoice->ifsw=0;
            }
            if (errorreported) /* also catches abort... */
            {
              yyerror("");
            }
            if (inerror==2) PrintLn();
            errorreported = inerror = cmdtok = 0;
            lastreserved = currid = NULL;
            expected_parms = siCntrlc = FALSE;
          ;}
    break;

  case 5:
#line 397 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:
#line 399 "grammar.y"
    { yyvsp[-1].lv.CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:
#line 401 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:
#line 405 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:
#line 410 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:
#line 412 "grammar.y"
    {
            #ifdef SIQ
            siq=0;
            #endif
            yyInRingConstruction = FALSE;
            currentVoice->ifsw=0;
            if (inerror)
            {
/*  bison failed here*/
              if ((inerror!=3) && (yyvsp[-1].i<UMINUS) && (yyvsp[-1].i>' '))
              {
                // 1: yyerror called
                // 2: scanner put actual string
                // 3: error rule put token+\n
                inerror=3;
                Print(" error at token `%s`\n",iiTwoOps(yyvsp[-1].i));
              }
/**/

            }
            if (!errorreported) WerrorS("...parse error");
            yyerror("");
            yyerrok;
#ifdef HAVE_SDB
            if ((sdb_flags & 1) && currentVoice->pi!=NULL)
            {
              currentVoice->pi->trace_flag |=1;
            }
            else
#endif
            if (myynest>0)
            {
              feBufferTypes t=currentVoice->Typ();
              //PrintS("leaving yyparse\n");
              exitBuffer(BT_proc);
              if (t==BT_example)
                YYACCEPT;
              else
                YYABORT;
            }
            else if (currentVoice->prev!=NULL)
            {
              exitVoice();
            }
#ifdef HAVE_SDB
            if (sdb_flags &2) sdb_flags=1;
#endif
          ;}
    break;

  case 18:
#line 470 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:
#line 473 "grammar.y"
    { omFree((ADDRESS)yyvsp[0].name); ;}
    break;

  case 29:
#line 488 "grammar.y"
    {
            if(iiAssign(&yyvsp[-1].lv,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 30:
#line 495 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&yyval.lv,omStrDup(yyvsp[0].name));
          ;}
    break;

  case 31:
#line 500 "grammar.y"
    {
            syMake(&yyval.lv,yyvsp[0].name);
          ;}
    break;

  case 32:
#line 504 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv, &yyvsp[-2].lv, COLONCOLON, &yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 33:
#line 508 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv, &yyvsp[-2].lv, '.', &yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 34:
#line 512 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-2].lv,'(')) YYERROR;
          ;}
    break;

  case 35:
#line 516 "grammar.y"
    {
            if (yyvsp[-3].lv.rtyp==UNKNOWN)
            { // for x(i)(j)
              if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,'(',&yyvsp[-1].lv)) YYERROR;
            }
            else
            {
              yyvsp[-3].lv.next=(leftv)omAllocBin(sleftv_bin);
              memcpy(yyvsp[-3].lv.next,&yyvsp[-1].lv,sizeof(sleftv));
              if(iiExprArithM(&yyval.lv,&yyvsp[-3].lv,'(')) YYERROR;
            }
          ;}
    break;

  case 36:
#line 529 "grammar.y"
    {
            if (currRingHdl==NULL) MYYERROR("no ring active");
            int j = 0;
            memset(&yyval.lv,0,sizeof(sleftv));
            yyval.lv.rtyp=VECTOR_CMD;
            leftv v = &yyvsp[-1].lv;
            while (v!=NULL)
            {
              int i,t;
              sleftv tmp;
              memset(&tmp,0,sizeof(tmp));
              i=iiTestConvert((t=v->Typ()),POLY_CMD);
              if((i==0) || (iiConvert(t /*v->Typ()*/,POLY_CMD,i,v,&tmp)))
              {
                pDelete((poly *)&yyval.lv.data);
                yyvsp[-1].lv.CleanUp();
                MYYERROR("expected '[poly,...'");
              }
              poly p = (poly)tmp.CopyD(POLY_CMD);
              pSetCompP(p,++j);
              yyval.lv.data = (void *)pAdd((poly)yyval.lv.data,p);
              v->next=tmp.next;tmp.next=NULL;
              tmp.CleanUp();
              v=v->next;
            }
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 37:
#line 557 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            int i = atoi(yyvsp[0].name);
            /*remember not to omFree($1)
            *because it is a part of the scanner buffer*/
            yyval.lv.rtyp  = INT_CMD;
            yyval.lv.data = (void *)(long)i;

            /* check: out of range input */
            int l = strlen(yyvsp[0].name)+2;
            number n;
            if (l >= MAX_INT_LEN)
            {
              char tmp[MAX_INT_LEN+5];
              sprintf(tmp,"%d",i);
              if (strcmp(tmp,yyvsp[0].name)!=0)
              {
                nlRead(yyvsp[0].name,&n);
                yyval.lv.rtyp=BIGINT_CMD;
                yyval.lv.data = n;
              }
            }
          ;}
    break;

  case 38:
#line 581 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            yyval.lv.rtyp = yyvsp[0].i;
            yyval.lv.data = yyval.lv.Data();
          ;}
    break;

  case 39:
#line 587 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            yyval.lv.rtyp  = STRING_CMD;
            yyval.lv.data = yyvsp[0].name;
          ;}
    break;

  case 40:
#line 593 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 41:
#line 600 "grammar.y"
    {
            leftv v = &yyvsp[-2].lv;
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&(yyvsp[0].lv),sizeof(sleftv));
            yyval.lv = yyvsp[-2].lv;
          ;}
    break;

  case 42:
#line 611 "grammar.y"
    {
            yyval.lv = yyvsp[0].lv;
          ;}
    break;

  case 43:
#line 617 "grammar.y"
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            yyval.lv = yyvsp[0].lv;
          ;}
    break;

  case 44:
#line 621 "grammar.y"
    { yyval.lv = yyvsp[0].lv; ;}
    break;

  case 45:
#line 622 "grammar.y"
    { yyval.lv = yyvsp[-1].lv; ;}
    break;

  case 46:
#line 624 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,'[',&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 47:
#line 628 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,'[',&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 48:
#line 632 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 49:
#line 636 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 50:
#line 640 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 51:
#line 644 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 52:
#line 648 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 53:
#line 652 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 54:
#line 656 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 55:
#line 660 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 56:
#line 664 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 57:
#line 668 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 58:
#line 672 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 59:
#line 676 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 60:
#line 680 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 61:
#line 684 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 62:
#line 688 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 63:
#line 692 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 64:
#line 696 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 65:
#line 700 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 66:
#line 704 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 67:
#line 708 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 68:
#line 712 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 69:
#line 716 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 70:
#line 720 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,RING_CMD,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 71:
#line 724 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,RING_CMD)) YYERROR;
          ;}
    break;

  case 72:
#line 728 "grammar.y"
    {
            yyval.lv=yyvsp[-1].lv;
          ;}
    break;

  case 73:
#line 732 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            if (siq>0)
            { if (iiExprArith2(&yyval.lv,&yyvsp[-3].lv,'=',&yyvsp[-1].lv)) YYERROR; }
            else
            #endif
            {
              memset(&yyval.lv,0,sizeof(yyval.lv));
              yyval.lv.rtyp=NONE;
              if (iiAssign(&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
            }
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 74:
#line 749 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 75:
#line 755 "grammar.y"
    {
            #ifdef SIQ
            if (siq<=0) yyvsp[-1].lv.Eval();
            #endif
            yyval.lv=yyvsp[-1].lv;
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 76:
#line 767 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 77:
#line 775 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 78:
#line 784 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,PLUSPLUS)) YYERROR;
          ;}
    break;

  case 79:
#line 788 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,MINUSMINUS)) YYERROR;
          ;}
    break;

  case 80:
#line 792 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'+',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 81:
#line 796 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'-',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 82:
#line 800 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 83:
#line 804 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'^',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 84:
#line 808 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 85:
#line 812 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 86:
#line 816 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,NOTEQUAL,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 87:
#line 820 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,EQUAL_EQUAL,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 88:
#line 824 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,DOTDOT,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 89:
#line 828 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,':',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 90:
#line 832 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            int i; TESTSETINT(yyvsp[0].lv,i);
            yyval.lv.rtyp  = INT_CMD;
            yyval.lv.data = (void *)(long)(i == 0 ? 1 : 0);
          ;}
    break;

  case 91:
#line 839 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[0].lv,'-')) YYERROR;
          ;}
    break;

  case 92:
#line 845 "grammar.y"
    { yyval.lv = yyvsp[-1].lv; ;}
    break;

  case 93:
#line 847 "grammar.y"
    {
            if (yyvsp[-1].lv.rtyp==0)
            {
              Werror("`%s` is undefined",yyvsp[-1].lv.Fullname());
              YYERROR;
            }
            else if ((yyvsp[-1].lv.rtyp==MODUL_CMD)
            // matrix m; m[2]=...
            && (yyvsp[-1].lv.e!=NULL) && (yyvsp[-1].lv.e->next==NULL))
            {
              MYYERROR("matrix must have 2 indices");
            }
            yyval.lv = yyvsp[-1].lv;
          ;}
    break;

  case 95:
#line 867 "grammar.y"
    {
            if (yyvsp[-1].lv.Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            yyval.name = (char *)yyvsp[-1].lv.CopyD(STRING_CMD);
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 96:
#line 879 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 97:
#line 884 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 98:
#line 889 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 99:
#line 893 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 100:
#line 897 "grammar.y"
    {
            int r; TESTSETINT(yyvsp[-4].lv,r);
            int c; TESTSETINT(yyvsp[-1].lv,c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            leftv v;
            idhdl h;
            if (yyvsp[-7].i == MATRIX_CMD)
            {
              if (iiDeclCommand(&yyval.lv,&yyvsp[-6].lv,myynest,yyvsp[-7].i,&(currRing->idroot), TRUE)) YYERROR;
              v=&yyval.lv;
              h=(idhdl)v->data;
              idDelete(&IDIDEAL(h));
              IDMATRIX(h) = mpNew(r,c);
              if (IDMATRIX(h)==NULL) YYERROR;
            }
            else if (yyvsp[-7].i == INTMAT_CMD)
            {
              if (iiDeclCommand(&yyval.lv,&yyvsp[-6].lv,myynest,yyvsp[-7].i,&(yyvsp[-6].lv.req_packhdl->idroot)))
                YYERROR;
              v=&yyval.lv;
              h=(idhdl)v->data;
              delete IDINTVEC(h);
              IDINTVEC(h) = new intvec(r,c,0);
              if (IDINTVEC(h)==NULL) YYERROR;
            }
            else /* BIGINTMAT_CMD */
            {
            }
          ;}
    break;

  case 101:
#line 930 "grammar.y"
    {
            if (yyvsp[-1].i == MATRIX_CMD)
            {
              if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
            }
            else if (yyvsp[-1].i == INTMAT_CMD)
            {
              if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
                YYERROR;
              leftv v=&yyval.lv;
              idhdl h;
              do
              {
                 h=(idhdl)v->data;
                 delete IDINTVEC(h);
                 IDINTVEC(h) = new intvec(1,1,0);
                 v=v->next;
              } while (v!=NULL);
            }
            else /* BIGINTMAT_CMD */
            {
            }
          ;}
    break;

  case 102:
#line 954 "grammar.y"
    {
            int t=yyvsp[-2].lv.Typ();
            sleftv r;
            memset(&r,0,sizeof(sleftv));
            if ((BEGIN_RING<t) && (t<END_RING))
            {
              if (iiDeclCommand(&r,&yyvsp[0].lv,myynest,t,&(currRing->idroot), TRUE))
                YYERROR;
            }
            else
            {
              if (iiDeclCommand(&r,&yyvsp[0].lv,myynest,t,&(yyvsp[0].lv.req_packhdl->idroot)))
                YYERROR;
            }
            leftv v=&yyvsp[-2].lv;
            while (v->next!=NULL) v=v->next;
            v->next=(leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&r,sizeof(sleftv));
            yyval.lv=yyvsp[-2].lv;
          ;}
    break;

  case 103:
#line 975 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 106:
#line 988 "grammar.y"
    {
            leftv v = &yyvsp[-3].lv;
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&(yyvsp[-1].lv),sizeof(sleftv));
            yyval.lv = yyvsp[-3].lv;
          ;}
    break;

  case 107:
#line 1002 "grammar.y"
    {
          // let rInit take care of any errors
          yyval.i=rOrderName(yyvsp[0].name);
        ;}
    break;

  case 108:
#line 1010 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = yyvsp[0].i;
            yyval.lv.rtyp = INTVEC_CMD;
            yyval.lv.data = (void *)iv;
          ;}
    break;

  case 109:
#line 1019 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            leftv sl = &yyvsp[-1].lv;
            int slLength;
            {
              slLength =  exprlist_length(sl);
              int l = 2 +  slLength;
              intvec *iv = new intvec(l);
              (*iv)[0] = slLength;
              (*iv)[1] = yyvsp[-3].i;

              int i = 2;
              while ((i<l) && (sl!=NULL))
              {
                if (sl->Typ() == INT_CMD)
                {
                  (*iv)[i++] = (int)((long)(sl->Data()));
                }
                else if ((sl->Typ() == INTVEC_CMD)
                ||(sl->Typ() == INTMAT_CMD))
                {
                  intvec *ivv = (intvec *)(sl->Data());
                  int ll = 0,l = ivv->length();
                  for (; l>0; l--)
                  {
                    (*iv)[i++] = (*ivv)[ll++];
                  }
                }
                else
                {
                  delete iv;
                  yyvsp[-1].lv.CleanUp();
                  MYYERROR("wrong type in ordering");
                }
                sl = sl->next;
              }
              yyval.lv.rtyp = INTVEC_CMD;
              yyval.lv.data = (void *)iv;
            }
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 111:
#line 1065 "grammar.y"
    {
            yyval.lv = yyvsp[-2].lv;
            yyval.lv.next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy(yyval.lv.next,&yyvsp[0].lv,sizeof(sleftv));
          ;}
    break;

  case 113:
#line 1075 "grammar.y"
    {
            yyval.lv = yyvsp[-1].lv;
          ;}
    break;

  case 114:
#line 1081 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 115:
#line 1087 "grammar.y"
    { yyval.i = yyvsp[0].i; ;}
    break;

  case 116:
#line 1089 "grammar.y"
    { yyval.i = yyvsp[0].i; ;}
    break;

  case 117:
#line 1091 "grammar.y"
    { yyval.i = yyvsp[0].i; ;}
    break;

  case 118:
#line 1100 "grammar.y"
    { if (yyvsp[-1].i != '<') YYERROR;
            if((feFilePending=feFopen(yyvsp[0].name,"r",NULL,TRUE))==NULL) YYERROR; ;}
    break;

  case 119:
#line 1103 "grammar.y"
    { newFile(yyvsp[-2].name,feFilePending); ;}
    break;

  case 120:
#line 1108 "grammar.y"
    {
            feHelp(yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[-1].name);
          ;}
    break;

  case 121:
#line 1113 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 122:
#line 1120 "grammar.y"
    {
            singular_example(yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[-1].name);
          ;}
    break;

  case 123:
#line 1128 "grammar.y"
    {
          if (basePack!=yyvsp[0].lv.req_packhdl)
          {
            if(iiExport(&yyvsp[0].lv,0,currPackHdl)) YYERROR;
          }
          else
            if (iiExport(&yyvsp[0].lv,0)) YYERROR;
        ;}
    break;

  case 124:
#line 1140 "grammar.y"
    {
          leftv v=&yyvsp[0].lv;
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 125:
#line 1156 "grammar.y"
    {
          leftv v=&yyvsp[0].lv;
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 126:
#line 1175 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 127:
#line 1179 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 128:
#line 1183 "grammar.y"
    {
            if (yyvsp[-1].i==QRING_CMD) yyvsp[-1].i=RING_CMD;
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 129:
#line 1188 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 130:
#line 1192 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 131:
#line 1196 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
           ;}
    break;

  case 132:
#line 1200 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 133:
#line 1204 "grammar.y"
    {
            list_cmd(0,yyvsp[-1].lv.Fullname(),"// ",TRUE);
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 134:
#line 1209 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 135:
#line 1215 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 136:
#line 1221 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 137:
#line 1227 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 138:
#line 1233 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 139:
#line 1239 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 140:
#line 1245 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 141:
#line 1257 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 142:
#line 1263 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 143:
#line 1272 "grammar.y"
    {
            const char *ring_name = yyvsp[-6].lv.name;
            ring b=
            rInit(&yyvsp[-4].lv,            /* characteristik and list of parameters*/
                  &yyvsp[-2].lv,            /* names of ringvariables */
                  &yyvsp[0].lv);            /* ordering */
            idhdl newRingHdl=NULL;

            if (b!=NULL)
            {
              newRingHdl=enterid(ring_name, myynest, RING_CMD,
                                   &(yyvsp[-6].lv.req_packhdl->idroot),FALSE);
              yyvsp[-6].lv.CleanUp();
              if (newRingHdl!=NULL)
              {
                IDRING(newRingHdl)=b;
              }
              else
              {
                rKill(b);
              }
            }
            yyInRingConstruction = FALSE;
            if (newRingHdl==NULL)
            {
              MYYERROR("cannot make ring");
            }
            else
            {
              rSetHdl(newRingHdl);
            }
          ;}
    break;

  case 144:
#line 1305 "grammar.y"
    {
            const char *ring_name = yyvsp[0].lv.name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            yyvsp[0].lv.CleanUp();
          ;}
    break;

  case 145:
#line 1315 "grammar.y"
    {
            if ((yyvsp[-1].i!=LIB_CMD)||(iiLibCmd(yyvsp[0].name,TRUE,TRUE,TRUE)))
            //if ($1==LIB_CMD)
            //{
            //  sleftv tmp;
            //  if(iiExprArith1(&tmp,&$2,LIB_CMD)) YYERROR;
            //}
            //else
                YYERROR;
          ;}
    break;

  case 148:
#line 1331 "grammar.y"
    {
            if ((yyvsp[-1].i==KEEPRING_CMD) && (myynest==0))
               MYYERROR("only inside a proc allowed");
            const char * n=yyvsp[0].lv.Name();
            if (((yyvsp[0].lv.Typ()==RING_CMD)||(yyvsp[0].lv.Typ()==QRING_CMD))
            && (yyvsp[0].lv.rtyp==IDHDL))
            {
              idhdl h=(idhdl)yyvsp[0].lv.data;
              if (yyvsp[0].lv.e!=NULL) h=rFindHdl((ring)yyvsp[0].lv.Data(),NULL, NULL);
              //Print("setring %s lev %d (ptr:%x)\n",IDID(h),IDLEV(h),IDRING(h));
              if (yyvsp[-1].i==KEEPRING_CMD)
              {
                if (h!=NULL)
                {
                  if (IDLEV(h)!=0)
                  {
                    if (iiExport(&yyvsp[0].lv,myynest-1)) YYERROR;
#if 1
                    idhdl p=IDRING(h)->idroot;
                    idhdl root=p;
                    int prevlev=myynest-1;
                    while (p!=NULL)
                    {
                      if (IDLEV(p)==myynest)
                      {
                        idhdl old=root->get(IDID(p),prevlev);
                        if (old!=NULL)
                        {
                          if (BVERBOSE(V_REDEFINE))
                            Warn("redefining %s",IDID(p));
                          killhdl2(old,&root,IDRING(h));
                          IDRING(h)->idroot=root;
                        }
                        IDLEV(p)=prevlev;
                      }
                      p=IDNEXT(p);
                    }
#endif
                  }
#ifdef USE_IILOCALRING
                  iiLocalRing[myynest-1]=IDRING(h);
#endif
                  procstack->cRing=IDRING(h);
                  procstack->cRingHdl=h;
                }
                else
                {
                  Werror("%s is no identifier",n);
                  yyvsp[0].lv.CleanUp();
                  YYERROR;
                }
              }
              if (h!=NULL) rSetHdl(h);
              else
              {
                Werror("cannot find the name of the basering %s",n);
                yyvsp[0].lv.CleanUp();
                YYERROR;
              }
              yyvsp[0].lv.CleanUp();
            }
            else
            {
              Werror("%s is no name of a ring/qring",n);
              yyvsp[0].lv.CleanUp();
              YYERROR;
            }
          ;}
    break;

  case 149:
#line 1403 "grammar.y"
    {
            type_cmd(&(yyvsp[0].lv));
          ;}
    break;

  case 150:
#line 1407 "grammar.y"
    {
            //Print("typ is %d, rtyp:%d\n",$1.Typ(),$1.rtyp);
            #ifdef SIQ
            if (yyvsp[0].lv.rtyp!=COMMAND)
            {
            #endif
              if (yyvsp[0].lv.Typ()==UNKNOWN)
              {
                if (yyvsp[0].lv.name!=NULL)
                {
                  Werror("`%s` is undefined",yyvsp[0].lv.name);
                  omFree((ADDRESS)yyvsp[0].lv.name);
                }
                YYERROR;
              }
            #ifdef SIQ
            }
            #endif
            yyvsp[0].lv.Print(&sLastPrinted);
            yyvsp[0].lv.CleanUp(currRing);
            if (errorreported) YYERROR;
          ;}
    break;

  case 151:
#line 1436 "grammar.y"
    {
            int i; TESTSETINT(yyvsp[-2].lv,i);
            if (i!=0)
            {
              newBuffer( yyvsp[0].name, BT_if);
            }
            else
            {
              omFree((ADDRESS)yyvsp[0].name);
              currentVoice->ifsw=1;
            }
          ;}
    break;

  case 152:
#line 1449 "grammar.y"
    {
            if (currentVoice->ifsw==1)
            {
              currentVoice->ifsw=0;
              newBuffer( yyvsp[0].name, BT_else);
            }
            else
            {
              if (currentVoice->ifsw!=2)
              {
                Warn("`else` without `if` in level %d",myynest);
              }
              omFree((ADDRESS)yyvsp[0].name);
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 153:
#line 1466 "grammar.y"
    {
            int i; TESTSETINT(yyvsp[-2].lv,i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 154:
#line 1475 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 155:
#line 1480 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 156:
#line 1488 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen(yyvsp[-1].name) + strlen(yyvsp[0].name) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,yyvsp[-1].name,yyvsp[0].name);
            newBuffer(s,BT_break);
            omFree((ADDRESS)yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[0].name);
          ;}
    break;

  case 157:
#line 1500 "grammar.y"
    {
            /* $2 */
            /* if (!$3) break; $5; $4; continue; */
            char * s = (char *)omAlloc( strlen(yyvsp[-2].name)+strlen(yyvsp[-1].name)+strlen(yyvsp[0].name)+36);
            sprintf(s,"forif (!(%s)) break;\n%s%s;\ncontinue;\n "
                   ,yyvsp[-2].name,yyvsp[0].name,yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[-2].name);
            omFree((ADDRESS)yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[0].name);
            newBuffer(s,BT_break);
            s = (char *)omAlloc( strlen(yyvsp[-3].name) + 3);
            sprintf(s,"%s;\n",yyvsp[-3].name);
            omFree((ADDRESS)yyvsp[-3].name);
            newBuffer(s,BT_if);
          ;}
    break;

  case 158:
#line 1519 "grammar.y"
    {
            procinfov pi;
            idhdl h = enterid(yyvsp[-1].name,myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL) {omFree((ADDRESS)yyvsp[-1].name);omFree((ADDRESS)yyvsp[0].name); YYERROR;}
            iiInitSingularProcinfo(IDPROC(h),"", yyvsp[-1].name, 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen(yyvsp[0].name)+31);;
            sprintf(IDPROC(h)->data.s.body,"parameter list #;\n%s;return();\n\n",yyvsp[0].name);
            omFree((ADDRESS)yyvsp[0].name);
            omFree((ADDRESS)yyvsp[-1].name);
          ;}
    break;

  case 159:
#line 1530 "grammar.y"
    {
            idhdl h = enterid(yyvsp[-2].name,myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)yyvsp[-2].name);
              omFree((ADDRESS)yyvsp[-1].name);
              omFree((ADDRESS)yyvsp[0].name);
              YYERROR;
            }
            char *args=iiProcArgs(yyvsp[-1].name,FALSE);
            omFree((ADDRESS)yyvsp[-1].name);
            procinfov pi;
            iiInitSingularProcinfo(IDPROC(h),"", yyvsp[-2].name, 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen(yyvsp[0].name)+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,yyvsp[0].name);
            omFree((ADDRESS)args);
            omFree((ADDRESS)yyvsp[0].name);
            omFree((ADDRESS)yyvsp[-2].name);
          ;}
    break;

  case 160:
#line 1550 "grammar.y"
    {
            omFree((ADDRESS)yyvsp[-1].name);
            idhdl h = enterid(yyvsp[-3].name,myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)yyvsp[-3].name);
              omFree((ADDRESS)yyvsp[-2].name);
              omFree((ADDRESS)yyvsp[0].name);
              YYERROR;
            }
            char *args=iiProcArgs(yyvsp[-2].name,FALSE);
            omFree((ADDRESS)yyvsp[-2].name);
            procinfov pi;
            iiInitSingularProcinfo(IDPROC(h),"", yyvsp[-3].name, 0, 0);
            omFree((ADDRESS)yyvsp[-3].name);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen(yyvsp[0].name)+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,yyvsp[0].name);
            omFree((ADDRESS)args);
            omFree((ADDRESS)yyvsp[0].name);
          ;}
    break;

  case 161:
#line 1574 "grammar.y"
    {
            // decl. of type proc p(int i)
            if (yyvsp[-1].i==PARAMETER)  { if (iiParameter(&yyvsp[0].lv)) YYERROR; }
            else                { if (iiAlias(&yyvsp[0].lv)) YYERROR; }
          ;}
    break;

  case 162:
#line 1580 "grammar.y"
    {
            // decl. of type proc p(i)
            sleftv tmp_expr;
            if (yyvsp[-1].i==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&yyvsp[0].lv,myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 163:
#line 1592 "grammar.y"
    {
            if(iiRETURNEXPR==NULL) YYERROR;
            iiRETURNEXPR[myynest].Copy(&yyvsp[-1].lv);
            yyvsp[-1].lv.CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 164:
#line 1599 "grammar.y"
    {
            if (yyvsp[-2].i==RETURN)
            {
              if(iiRETURNEXPR==NULL) YYERROR;
              iiRETURNEXPR[myynest].Init();
              iiRETURNEXPR[myynest].rtyp=NONE;
              if (exitBuffer(BT_proc)) YYERROR;
            }
          ;}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 3560 "grammar.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}



