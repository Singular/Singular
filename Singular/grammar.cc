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
     INTMAT_CMD = 268,
     PROC_CMD = 269,
     RING_CMD = 270,
     BEGIN_RING = 271,
     IDEAL_CMD = 272,
     MAP_CMD = 273,
     MATRIX_CMD = 274,
     MODUL_CMD = 275,
     NUMBER_CMD = 276,
     POLY_CMD = 277,
     RESOLUTION_CMD = 278,
     VECTOR_CMD = 279,
     BETTI_CMD = 280,
     COEFFS_CMD = 281,
     COEF_CMD = 282,
     CONTRACT_CMD = 283,
     DEGREE_CMD = 284,
     DEG_CMD = 285,
     DIFF_CMD = 286,
     DIM_CMD = 287,
     DIVISION_CMD = 288,
     ELIMINATION_CMD = 289,
     E_CMD = 290,
     FAREY_CMD = 291,
     FETCH_CMD = 292,
     FREEMODULE_CMD = 293,
     KEEPRING_CMD = 294,
     HILBERT_CMD = 295,
     HOMOG_CMD = 296,
     IMAP_CMD = 297,
     INDEPSET_CMD = 298,
     INTERRED_CMD = 299,
     INTERSECT_CMD = 300,
     JACOB_CMD = 301,
     JET_CMD = 302,
     KBASE_CMD = 303,
     KOSZUL_CMD = 304,
     LEADCOEF_CMD = 305,
     LEADEXP_CMD = 306,
     LEAD_CMD = 307,
     LEADMONOM_CMD = 308,
     LIFTSTD_CMD = 309,
     LIFT_CMD = 310,
     MAXID_CMD = 311,
     MINBASE_CMD = 312,
     MINOR_CMD = 313,
     MINRES_CMD = 314,
     MODULO_CMD = 315,
     MONOM_CMD = 316,
     MRES_CMD = 317,
     MULTIPLICITY_CMD = 318,
     ORD_CMD = 319,
     PAR_CMD = 320,
     PARDEG_CMD = 321,
     PREIMAGE_CMD = 322,
     QUOTIENT_CMD = 323,
     QHWEIGHT_CMD = 324,
     REDUCE_CMD = 325,
     REGULARITY_CMD = 326,
     RES_CMD = 327,
     SIMPLIFY_CMD = 328,
     SORTVEC_CMD = 329,
     SRES_CMD = 330,
     STD_CMD = 331,
     SUBST_CMD = 332,
     SYZYGY_CMD = 333,
     VAR_CMD = 334,
     VDIM_CMD = 335,
     WEDGE_CMD = 336,
     WEIGHT_CMD = 337,
     VALTVARS = 338,
     VMAXDEG = 339,
     VMAXMULT = 340,
     VNOETHER = 341,
     VMINPOLY = 342,
     END_RING = 343,
     CMD_1 = 344,
     CMD_2 = 345,
     CMD_3 = 346,
     CMD_12 = 347,
     CMD_13 = 348,
     CMD_23 = 349,
     CMD_123 = 350,
     CMD_M = 351,
     ROOT_DECL = 352,
     ROOT_DECL_LIST = 353,
     RING_DECL = 354,
     RING_DECL_LIST = 355,
     EXAMPLE_CMD = 356,
     EXPORT_CMD = 357,
     HELP_CMD = 358,
     KILL_CMD = 359,
     LIB_CMD = 360,
     LISTVAR_CMD = 361,
     SETRING_CMD = 362,
     TYPE_CMD = 363,
     STRINGTOK = 364,
     BLOCKTOK = 365,
     INT_CONST = 366,
     UNKNOWN_IDENT = 367,
     RINGVAR = 368,
     PROC_DEF = 369,
     BREAK_CMD = 370,
     CONTINUE_CMD = 371,
     ELSE_CMD = 372,
     EVAL = 373,
     QUOTE = 374,
     FOR_CMD = 375,
     IF_CMD = 376,
     SYS_BREAK = 377,
     WHILE_CMD = 378,
     RETURN = 379,
     PARAMETER = 380,
     SYSVAR = 381,
     UMINUS = 382
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
#define INTMAT_CMD 268
#define PROC_CMD 269
#define RING_CMD 270
#define BEGIN_RING 271
#define IDEAL_CMD 272
#define MAP_CMD 273
#define MATRIX_CMD 274
#define MODUL_CMD 275
#define NUMBER_CMD 276
#define POLY_CMD 277
#define RESOLUTION_CMD 278
#define VECTOR_CMD 279
#define BETTI_CMD 280
#define COEFFS_CMD 281
#define COEF_CMD 282
#define CONTRACT_CMD 283
#define DEGREE_CMD 284
#define DEG_CMD 285
#define DIFF_CMD 286
#define DIM_CMD 287
#define DIVISION_CMD 288
#define ELIMINATION_CMD 289
#define E_CMD 290
#define FAREY_CMD 291
#define FETCH_CMD 292
#define FREEMODULE_CMD 293
#define KEEPRING_CMD 294
#define HILBERT_CMD 295
#define HOMOG_CMD 296
#define IMAP_CMD 297
#define INDEPSET_CMD 298
#define INTERRED_CMD 299
#define INTERSECT_CMD 300
#define JACOB_CMD 301
#define JET_CMD 302
#define KBASE_CMD 303
#define KOSZUL_CMD 304
#define LEADCOEF_CMD 305
#define LEADEXP_CMD 306
#define LEAD_CMD 307
#define LEADMONOM_CMD 308
#define LIFTSTD_CMD 309
#define LIFT_CMD 310
#define MAXID_CMD 311
#define MINBASE_CMD 312
#define MINOR_CMD 313
#define MINRES_CMD 314
#define MODULO_CMD 315
#define MONOM_CMD 316
#define MRES_CMD 317
#define MULTIPLICITY_CMD 318
#define ORD_CMD 319
#define PAR_CMD 320
#define PARDEG_CMD 321
#define PREIMAGE_CMD 322
#define QUOTIENT_CMD 323
#define QHWEIGHT_CMD 324
#define REDUCE_CMD 325
#define REGULARITY_CMD 326
#define RES_CMD 327
#define SIMPLIFY_CMD 328
#define SORTVEC_CMD 329
#define SRES_CMD 330
#define STD_CMD 331
#define SUBST_CMD 332
#define SYZYGY_CMD 333
#define VAR_CMD 334
#define VDIM_CMD 335
#define WEDGE_CMD 336
#define WEIGHT_CMD 337
#define VALTVARS 338
#define VMAXDEG 339
#define VMAXMULT 340
#define VNOETHER 341
#define VMINPOLY 342
#define END_RING 343
#define CMD_1 344
#define CMD_2 345
#define CMD_3 346
#define CMD_12 347
#define CMD_13 348
#define CMD_23 349
#define CMD_123 350
#define CMD_M 351
#define ROOT_DECL 352
#define ROOT_DECL_LIST 353
#define RING_DECL 354
#define RING_DECL_LIST 355
#define EXAMPLE_CMD 356
#define EXPORT_CMD 357
#define HELP_CMD 358
#define KILL_CMD 359
#define LIB_CMD 360
#define LISTVAR_CMD 361
#define SETRING_CMD 362
#define TYPE_CMD 363
#define STRINGTOK 364
#define BLOCKTOK 365
#define INT_CONST 366
#define UNKNOWN_IDENT 367
#define RINGVAR 368
#define PROC_DEF 369
#define BREAK_CMD 370
#define CONTINUE_CMD 371
#define ELSE_CMD 372
#define EVAL 373
#define QUOTE 374
#define FOR_CMD 375
#define IF_CMD 376
#define SYS_BREAK 377
#define WHILE_CMD 378
#define RETURN 379
#define PARAMETER 380
#define SYSVAR 381
#define UMINUS 382




/* Copy the first part of user declarations.  */
#line 8 "grammar.y"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <kernel/mod2.h>
#include <misc/mylimits.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <misc/options.h>
#include <Singular/stype.h>
#include <Singular/fehelp.h>
#include <Singular/ipid.h>
#include <misc/intvec.h>
#include <kernel/febase.h>
#include <polys/matpol.h>
#include <polys/monomials/ring.h>
#include <kernel/kstd1.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>
#include <Singular/sdb.h>
#include <kernel/ideals.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/stairc.h>
#include <kernel/timer.h>
#include <Singular/cntrlc.h>
#include <polys/monomials/maps.h>
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
#define YYMAXDEPTH MAX_INT_VAL

extern int   yylineno;
extern FILE* yyin;

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
#ifdef HAVE_LIBFAC
  extern int libfac_interruptflag;
  libfac_interruptflag=0;
#endif // #ifdef HAVE_LIBFAC
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
#define YYLAST   2002

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  146
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  43
/* YYNRULES -- Number of rules. */
#define YYNRULES  167
/* YYNRULES -- Number of states. */
#define YYNSTATES  373

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   382

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   138,     2,
     142,   143,   140,   130,   136,   131,   144,   132,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,   137,
     128,   127,   129,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   133,     2,   134,   135,     2,   145,     2,     2,     2,
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
     125,   126,   141
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
     261,   270,   275,   284,   289,   293,   299,   300,   306,   309,
     311,   314,   317,   321,   325,   329,   333,   337,   341,   345,
     349,   353,   357,   360,   363,   366,   369,   371,   375,   378,
     381,   384,   387,   396,   399,   408,   411,   415,   418,   420,
     422,   428,   430,   432,   437,   439,   443,   445,   449,   451,
     452,   457,   461,   464,   468,   471,   474,   478,   483,   488,
     493,   498,   503,   508,   513,   518,   523,   530,   537,   544,
     551,   558,   565,   572,   579,   583,   585,   594,   597,   600,
     602,   604,   607,   610,   612,   618,   621,   627,   629,   631,
     635,   641,   645,   649,   654,   657,   660,   665
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     147,     0,    -1,    -1,   147,   148,    -1,   149,    -1,   151,
     137,    -1,   162,   137,    -1,   188,    -1,   122,    -1,   137,
      -1,     1,   137,    -1,   183,    -1,   184,    -1,   150,    -1,
     185,    -1,   186,    -1,   170,    -1,   172,    -1,   173,    -1,
     101,   110,    -1,   152,    -1,   174,    -1,   175,    -1,   176,
      -1,   187,    -1,   178,    -1,   179,    -1,   181,    -1,   182,
      -1,   160,   154,    -1,   113,    -1,   161,    -1,   153,    11,
     153,    -1,   153,   144,   153,    -1,   153,   142,   143,    -1,
     153,   142,   154,   143,    -1,   133,   154,   134,    -1,   111,
      -1,   126,    -1,   163,    -1,    14,   142,   155,   143,    -1,
     154,   136,   155,    -1,   155,    -1,   159,    -1,   153,    -1,
     142,   154,   143,    -1,   155,   133,   155,   136,   155,   134,
      -1,   155,   133,   155,   134,    -1,    97,   142,   155,   143,
      -1,    98,   142,   154,   143,    -1,    98,   142,   143,    -1,
      99,   142,   155,   143,    -1,   100,   142,   154,   143,    -1,
     100,   142,   143,    -1,    89,   142,   155,   143,    -1,    90,
     142,   155,   136,   155,   143,    -1,    91,   142,   155,   136,
     155,   136,   155,   143,    -1,    94,   142,   155,   136,   155,
     143,    -1,    94,   142,   155,   136,   155,   136,   155,   143,
      -1,    92,   142,   155,   143,    -1,    93,   142,   155,   143,
      -1,    92,   142,   155,   136,   155,   143,    -1,    95,   142,
     155,   143,    -1,    95,   142,   155,   136,   155,   143,    -1,
      93,   142,   155,   136,   155,   136,   155,   143,    -1,    95,
     142,   155,   136,   155,   136,   155,   143,    -1,    96,   142,
     143,    -1,    96,   142,   154,   143,    -1,    19,   142,   155,
     136,   155,   136,   155,   143,    -1,    19,   142,   155,   143,
      -1,    13,   142,   155,   136,   155,   136,   155,   143,    -1,
      13,   142,   155,   143,    -1,    15,   142,   164,   136,   164,
     136,   168,   143,    -1,    15,   142,   155,   143,    -1,   157,
     155,   158,    -1,   157,   155,   127,   155,   158,    -1,    -1,
     118,   142,   156,   155,   143,    -1,   119,   142,    -1,   143,
      -1,   155,    10,    -1,   155,     7,    -1,   155,   130,   155,
      -1,   155,   131,   155,    -1,   155,   132,   155,    -1,   155,
     135,   155,    -1,   155,   128,   155,    -1,   155,   138,   155,
      -1,   155,     9,   155,    -1,   155,     4,   155,    -1,   155,
       3,   155,    -1,   155,   139,   155,    -1,     8,   155,    -1,
     131,   155,    -1,   162,   169,    -1,   154,   127,    -1,   112,
      -1,   145,   155,   145,    -1,    97,   153,    -1,    98,   153,
      -1,    99,   153,    -1,   100,   153,    -1,    19,   153,   133,
     155,   134,   133,   155,   134,    -1,    19,   153,    -1,    13,
     153,   133,   155,   134,   133,   155,   134,    -1,    13,   153,
      -1,   162,   136,   153,    -1,    14,   153,    -1,   109,    -1,
     155,    -1,   142,   155,   136,   154,   143,    -1,   112,    -1,
     165,    -1,   165,   142,   154,   143,    -1,   166,    -1,   166,
     136,   167,    -1,   166,    -1,   142,   167,   143,    -1,   127,
      -1,    -1,   128,   163,   171,   137,    -1,   103,   109,   137,
      -1,   103,   137,    -1,   101,   109,   137,    -1,   102,   154,
      -1,   104,   153,    -1,   175,   136,   153,    -1,   106,   142,
      97,   143,    -1,   106,   142,    98,   143,    -1,   106,   142,
      99,   143,    -1,   106,   142,   100,   143,    -1,   106,   142,
      15,   143,    -1,   106,   142,    19,   143,    -1,   106,   142,
      13,   143,    -1,   106,   142,    14,   143,    -1,   106,   142,
     153,   143,    -1,   106,   142,   153,   136,    97,   143,    -1,
     106,   142,   153,   136,    98,   143,    -1,   106,   142,   153,
     136,    99,   143,    -1,   106,   142,   153,   136,   100,   143,
      -1,   106,   142,   153,   136,    15,   143,    -1,   106,   142,
     153,   136,    19,   143,    -1,   106,   142,   153,   136,    13,
     143,    -1,   106,   142,   153,   136,    14,   143,    -1,   106,
     142,   143,    -1,    15,    -1,   177,   153,   169,   164,   136,
     164,   136,   168,    -1,   177,   153,    -1,   126,   163,    -1,
     107,    -1,    39,    -1,   180,   155,    -1,   108,   155,    -1,
     154,    -1,   121,   142,   155,   143,   110,    -1,   117,   110,
      -1,   121,   142,   155,   143,   115,    -1,   115,    -1,   116,
      -1,   123,   109,   110,    -1,   120,   109,   109,   109,   110,
      -1,    14,   161,   110,    -1,   114,   109,   110,    -1,   114,
     109,   109,   110,    -1,   125,   162,    -1,   125,   155,    -1,
     124,   142,   154,   143,    -1,   124,   142,   143,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   360,   360,   362,   396,   397,   399,   401,   405,   410,
     412,   463,   464,   465,   466,   467,   468,   469,   470,   474,
     477,   478,   479,   480,   481,   482,   483,   484,   485,   488,
     495,   500,   504,   508,   512,   516,   529,   557,   581,   587,
     593,   600,   611,   617,   622,   623,   624,   628,   632,   636,
     640,   644,   648,   652,   656,   660,   664,   668,   672,   676,
     680,   684,   688,   692,   696,   700,   704,   708,   712,   716,
     720,   724,   728,   732,   736,   740,   758,   757,   775,   783,
     792,   796,   800,   804,   808,   812,   816,   820,   824,   828,
     832,   836,   840,   847,   854,   855,   874,   875,   887,   892,
     897,   901,   905,   921,   925,   941,   955,   976,   984,   988,
     989,  1003,  1011,  1020,  1065,  1066,  1075,  1076,  1082,  1095,
    1094,  1102,  1107,  1114,  1122,  1134,  1150,  1169,  1173,  1177,
    1182,  1186,  1190,  1194,  1198,  1202,  1207,  1213,  1219,  1225,
    1231,  1237,  1243,  1249,  1261,  1268,  1272,  1309,  1319,  1332,
    1332,  1335,  1407,  1411,  1440,  1453,  1470,  1479,  1484,  1492,
    1504,  1523,  1534,  1554,  1578,  1584,  1596,  1603
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "GRING_CMD",
  "INTMAT_CMD", "PROC_CMD", "RING_CMD", "BEGIN_RING", "IDEAL_CMD",
  "MAP_CMD", "MATRIX_CMD", "MODUL_CMD", "NUMBER_CMD", "POLY_CMD",
  "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD", "COEFFS_CMD", "COEF_CMD",
  "CONTRACT_CMD", "DEGREE_CMD", "DEG_CMD", "DIFF_CMD", "DIM_CMD",
  "DIVISION_CMD", "ELIMINATION_CMD", "E_CMD", "FAREY_CMD", "FETCH_CMD",
  "FREEMODULE_CMD", "KEEPRING_CMD", "HILBERT_CMD", "HOMOG_CMD", "IMAP_CMD",
  "INDEPSET_CMD", "INTERRED_CMD", "INTERSECT_CMD", "JACOB_CMD", "JET_CMD",
  "KBASE_CMD", "KOSZUL_CMD", "LEADCOEF_CMD", "LEADEXP_CMD", "LEAD_CMD",
  "LEADMONOM_CMD", "LIFTSTD_CMD", "LIFT_CMD", "MAXID_CMD", "MINBASE_CMD",
  "MINOR_CMD", "MINRES_CMD", "MODULO_CMD", "MONOM_CMD", "MRES_CMD",
  "MULTIPLICITY_CMD", "ORD_CMD", "PAR_CMD", "PARDEG_CMD", "PREIMAGE_CMD",
  "QUOTIENT_CMD", "QHWEIGHT_CMD", "REDUCE_CMD", "REGULARITY_CMD",
  "RES_CMD", "SIMPLIFY_CMD", "SORTVEC_CMD", "SRES_CMD", "STD_CMD",
  "SUBST_CMD", "SYZYGY_CMD", "VAR_CMD", "VDIM_CMD", "WEDGE_CMD",
  "WEIGHT_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT", "VNOETHER", "VMINPOLY",
  "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12", "CMD_13", "CMD_23",
  "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST", "RING_DECL",
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
  "OrderingList", "ordering", "cmdeq", "filecmd", "@2", "helpcmd",
  "examplecmd", "exportcmd", "killcmd", "listcmd", "ringcmd1", "ringcmd",
  "scriptcmd", "setrings", "setringcmd", "typecmd", "ifcmd", "whilecmd",
  "forcmd", "proccmd", "parametercmd", "returncmd", 0
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
     375,   376,   377,   378,   379,   380,   381,    61,    60,    62,
      43,    45,    47,    91,    93,    94,    44,    59,    38,    58,
      42,   382,    40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   146,   147,   147,   148,   148,   148,   148,   148,   148,
     148,   149,   149,   149,   149,   149,   149,   149,   149,   150,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   152,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   154,   154,   155,   155,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   156,   155,   157,   158,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   160,   160,   161,   161,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   163,   164,
     164,   165,   166,   166,   167,   167,   168,   168,   169,   171,
     170,   172,   172,   173,   174,   175,   175,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   177,   178,   178,   179,   180,
     180,   181,   182,   182,   183,   183,   183,   183,   183,   184,
     185,   186,   186,   186,   187,   187,   188,   188
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
       8,     4,     8,     4,     3,     5,     0,     5,     2,     1,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     1,     3,     2,     2,
       2,     2,     8,     2,     8,     2,     3,     2,     1,     1,
       5,     1,     1,     4,     1,     3,     1,     3,     1,     0,
       4,     3,     2,     3,     2,     2,     3,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     6,     6,     6,     6,
       6,     6,     6,     6,     3,     1,     8,     2,     2,     1,
       1,     2,     2,     1,     5,     2,     5,     1,     1,     3,
       5,     3,     3,     4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,   145,     0,   150,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   149,     0,   108,
      37,    96,    30,     0,   157,   158,     0,     0,     0,     0,
       0,     8,     0,     0,     0,    38,     0,     0,     0,     9,
       0,     0,     3,     4,    13,     0,    20,    44,   153,    42,
       0,    43,     0,    31,     0,    39,    16,    17,    18,    21,
      22,    23,     0,    25,    26,     0,    27,    28,    11,    12,
      14,    15,    24,     7,    10,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    92,     0,   105,     0,   107,    31,
       0,     0,   103,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    98,     0,    99,     0,   100,     0,   101,     0,
      19,   124,     0,   122,   125,     0,   152,     0,   155,    76,
      78,     0,     0,     0,     0,     0,   165,   164,   148,   119,
      93,     0,     0,     0,     5,     0,     0,     0,    95,     0,
       0,     0,    81,     0,    80,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,   118,     0,     6,    94,     0,
     147,   151,     0,     0,     0,   161,     0,   109,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    66,     0,
       0,    50,     0,     0,    53,     0,   123,   121,     0,     0,
       0,     0,     0,     0,     0,     0,   144,     0,     0,   162,
       0,     0,     0,   159,   167,     0,     0,    36,    45,    97,
      32,    34,     0,    33,    41,    90,    89,    88,    86,    82,
      83,    84,     0,    85,    87,    91,     0,    79,    74,   106,
     126,     0,     0,    71,     0,    40,    42,    73,     0,     0,
      69,     0,    54,     0,     0,     0,    59,     0,    60,     0,
       0,    62,    67,    48,    49,    51,    52,   133,   134,   131,
     132,   127,   128,   129,   130,     0,   135,   163,     0,     0,
       0,   166,   120,    35,    47,     0,     0,   109,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
     160,   154,   156,     0,    75,     0,     0,     0,     0,     0,
       0,     0,    55,     0,    61,     0,     0,    57,     0,    63,
     142,   143,   140,   141,   136,   137,   138,   139,    46,     0,
       0,     0,   110,   111,     0,   112,   116,     0,     0,     0,
       0,     0,     0,     0,     0,    70,   104,   114,     0,     0,
      72,    68,   102,    56,    64,    58,    65,   146,     0,   117,
       0,   115,   113
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,    52,    53,    54,    55,    56,    57,   142,    59,
     210,    60,   238,    61,    62,    63,    64,    65,   178,   345,
     346,   358,   347,   168,    66,   216,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -339
static const short int yypact[] =
{
    -339,   270,  -339,  -127,  1223,  1728,  1777,  -121,  1788,  -339,
    -117,   -88,   -77,   -68,   -66,   -63,   -60,   -52,  1829,  1834,
    1839,  1857,   -40,  1223,  -104,  1634,   -51,  -339,  1223,  -339,
    -339,  -339,  -339,   -94,  -339,  -339,   -73,   -50,   -48,   -11,
     -43,  -339,    -5,   -41,  1280,    -4,    -4,  1223,  1223,  -339,
    1223,  1223,  -339,  -339,  -339,   -31,  -339,    23,  -113,  1679,
    1223,  -339,  1223,  -339,   -92,  -339,  -339,  -339,  -339,  -339,
     -29,  -339,  1634,  -339,  -339,  1223,  -339,  -339,  -339,  -339,
    -339,  -339,  -339,  -339,  -339,   -34,   -32,  -121,   -28,   -24,
     -20,     7,     8,  -339,    -3,  1223,    55,  1223,    23,     1,
    1338,  1223,    78,  1223,  1223,  1223,  1223,  1223,  1223,  1223,
     891,  1223,    23,   993,    23,  1223,    23,  1050,    23,    14,
    -339,   -23,    18,  -339,    23,  1782,  1679,   -37,  -339,  -339,
    -339,    29,  1223,    47,  1108,  1777,  1679,    22,  -339,  -339,
      -3,  -125,  -105,     9,  -339,  1634,  1165,  1634,  -339,  1223,
    1223,  1223,  -339,  1223,  -339,  1223,  1223,  1223,  1223,  1223,
    1223,  1223,  1223,    74,   -23,  -339,  1634,  -339,  -339,  1634,
      -8,  1679,    93,  1223,   456,  -339,  1223,   483,    26,   182,
    1223,   497,  1378,  1393,   288,   319,  1405,   346,  -339,  -100,
     511,  -339,   -87,   538,  -339,   -86,  -339,  -339,    31,   -57,
      40,    41,    51,    52,    60,    65,  -339,    17,    53,  -339,
    1223,    63,   562,  -339,  -339,   -85,    27,  -339,  -339,  -339,
      54,  -339,   -84,  -339,  1679,  1691,   309,   309,   446,   337,
     337,    -3,  1357,    10,  1703,   337,  1223,  -339,  -339,    23,
      23,  1338,  1223,  -339,   922,  -339,  1436,  -339,  1338,  1223,
    -339,  1451,  -339,  1223,  1223,  1223,  -339,  1223,  -339,  1223,
    1223,  -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,
    -339,  -339,  -339,  -339,  -339,   233,  -339,  -339,   593,   100,
     -47,  -339,  -339,  -339,  -339,  1223,   648,  1679,    79,  1463,
      81,  1223,    80,  1494,    85,   675,  1542,   699,  1600,   401,
     425,    76,    84,    90,    91,    92,    94,    95,    96,  -339,
    -339,  -339,  -339,  1631,  -339,  1338,  1223,  1223,   -83,  -110,
    1223,  1223,  -339,  1223,  -339,  1223,  1223,  -339,  1223,  -339,
    -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,   104,
     730,  1643,  -339,  -339,   129,   101,  -339,    99,   744,  1655,
     758,   785,   812,   836,  -110,  -339,  -339,   108,   102,  1223,
    -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,   129,  -339,
     -81,  -339,  -339
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -339,  -339,  -339,  -339,  -339,  -339,  -339,    21,    -1,    20,
    -339,  -339,   -36,  -339,  -339,   243,   207,    42,  -240,  -339,
    -338,  -115,   -99,    87,  -339,  -339,  -339,  -339,  -339,  -339,
    -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,  -339,
    -339,  -339,  -339
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const short int yytable[] =
{
      58,   288,   343,   145,   152,   122,   357,   154,   292,   217,
      84,   149,   150,   151,   148,   127,   152,   152,   153,   154,
     154,   100,   121,   149,    94,   103,    96,    98,   145,   102,
     357,   149,   344,   123,   145,   165,   149,   128,   218,   112,
     114,   116,   118,   262,   166,   167,   124,   141,   126,   149,
     149,   149,   149,   149,   104,   149,   264,   266,   281,   283,
     342,   164,   372,   311,   136,   105,   145,   140,   312,   119,
     120,   143,   208,   209,   106,   339,   107,   150,   151,   108,
     163,   152,   109,   153,   154,    97,   268,   138,   139,   145,
     110,   125,   129,   170,   130,   171,   150,   151,   131,   132,
     152,   134,   153,   154,   133,    29,   144,   169,    95,   189,
      97,   175,   192,   149,   101,   172,   195,   174,   111,   165,
     177,   179,   113,   181,   182,   183,   184,   185,   186,   187,
     159,   190,   160,   215,   146,   193,   147,   155,   211,   156,
     157,   158,   159,   159,   160,   222,   207,   161,   162,   115,
     117,   196,   212,   275,   219,   197,    98,   213,   166,   146,
     276,   147,   248,   277,   282,   146,   220,   147,   223,   224,
     225,   226,   279,   227,   267,   228,   229,   230,   231,   232,
     233,   234,   235,   269,   270,   150,   151,   239,   173,   152,
     240,   153,   154,   244,   271,   272,   246,   146,   147,   147,
     251,   236,   155,   273,   156,   157,   158,   159,   274,   160,
     310,   180,   161,   162,   317,   315,   319,   237,   321,   330,
     146,   155,   147,   156,   157,   158,   159,   331,   160,   242,
     278,   161,   162,   332,   333,   334,   243,   335,   336,   337,
     354,   343,   360,   359,   368,   369,   301,   302,   303,    99,
     314,   137,   304,   371,     0,   367,   286,   241,     0,     0,
       0,   287,   289,     0,     0,     0,     0,     0,   287,   293,
       2,     3,     0,   295,   296,   297,     0,   298,     4,   299,
     300,     0,     0,     5,     6,     7,     0,     0,     0,     8,
     318,   150,   151,     0,     0,   152,     0,   153,   154,     0,
       0,     0,     0,     0,     0,   313,     0,     0,     0,     9,
     155,     0,   156,   157,   158,   159,   152,   160,   249,   154,
     161,   162,   150,   151,     0,   250,   152,     0,   153,   154,
     305,   306,   307,   308,     0,   287,   340,   341,     0,     0,
     348,   349,     0,   350,   152,   351,   352,   154,   353,   150,
     151,     0,     0,   152,     0,   153,   154,     0,   370,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,    26,    27,    28,    29,
       0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,     0,    46,     0,
       0,    47,     0,    48,   150,   151,     0,    49,   152,     0,
     153,   154,    50,     0,     0,    51,   155,     0,   156,   157,
     158,   159,     0,   160,   255,     0,   161,   162,   150,   151,
       0,   256,   152,     0,   153,   154,     0,   155,     0,   156,
     157,   158,   159,     0,   160,     0,     0,   155,   162,   156,
     157,   158,   159,   152,   160,   257,   154,   161,   162,   150,
     151,     0,   258,   152,     0,   153,   154,     0,     0,   158,
     159,     0,   160,     0,   155,     0,   156,   157,   158,   159,
       0,   160,   260,     0,   161,   162,   150,   151,     0,   261,
     152,     0,   153,   154,     0,     0,     0,     0,     0,     0,
     150,   151,     0,     0,   152,     0,   153,   154,     0,     0,
       0,     0,     0,     0,   150,   151,     0,     0,   152,     0,
     153,   154,     0,     0,     0,     0,     0,     0,     0,   155,
       0,   156,   157,   158,   159,     0,   160,   326,     0,   161,
     162,   150,   151,     0,   327,   152,     0,   153,   154,     0,
       0,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,   328,     0,   161,   162,   150,   151,     0,   329,   152,
       0,   153,   154,     0,     0,     0,   156,   157,   158,   159,
       0,   160,     0,     0,   155,   162,   156,   157,   158,   159,
       0,   160,     0,     0,   161,   162,   150,   151,     0,   245,
     152,     0,   153,   154,     0,     0,     0,     0,     0,     0,
       0,   155,     0,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,     0,     0,   155,   247,   156,   157,   158,
     159,     0,   160,     0,     0,   161,   162,     0,     0,   155,
     252,   156,   157,   158,   159,     0,   160,     0,     0,   161,
     162,   150,   151,     0,   263,   152,     0,   153,   154,     0,
       0,     0,     0,     0,     0,     0,   155,     0,   156,   157,
     158,   159,     0,   160,     0,     0,   161,   162,   150,   151,
       0,   265,   152,     0,   153,   154,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,     0,   160,     0,     0,
     161,   162,   150,   151,     0,   280,   152,     0,   153,   154,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   155,     0,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,   150,   151,     0,   309,   152,     0,   153,
     154,     0,     0,     0,     0,     0,     0,   150,   151,     0,
       0,   152,     0,   153,   154,     0,     0,     0,     0,     0,
       0,   150,   151,     0,     0,   152,     0,   153,   154,     0,
       0,     0,     0,     0,     0,     0,   155,     0,   156,   157,
     158,   159,     0,   160,     0,     0,   161,   162,   150,   151,
       0,   237,   152,     0,   153,   154,     0,     0,     0,     0,
       0,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,   150,   151,     0,   322,   152,
       0,   153,   154,     0,     0,     0,     0,   155,     0,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,   150,
     151,     0,   324,   152,     0,   153,   154,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   155,     0,
     156,   157,   158,   159,     0,   160,     0,     0,   161,   162,
       0,     0,   155,   355,   156,   157,   158,   159,     0,   160,
       0,     0,   161,   162,     0,     0,   155,   361,   156,   157,
     158,   159,     0,   160,     0,     0,   161,   162,     0,     4,
       0,   363,     0,     0,    85,    86,    87,     0,     0,     0,
      88,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,   150,   151,     0,   364,   152,
       0,   153,   154,     0,     0,     0,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,     0,   160,     0,     0,
     161,   162,     0,     0,     0,   365,     0,     0,     0,     0,
       0,     0,     0,     0,   155,     0,   156,   157,   158,   159,
       0,   160,     0,     0,   161,   162,     0,     0,     0,   366,
      10,    11,    12,    13,    14,    15,    16,    17,    89,    90,
      91,    92,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     4,    30,    31,    32,     0,    85,    86,    87,    37,
      38,     0,    88,     0,     0,     0,     0,    93,     0,     0,
       0,     0,    47,     0,    48,     0,     0,     0,     0,     0,
       0,     0,     0,    50,   188,     0,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,   290,   160,     4,     0,
     161,   162,     0,    85,    86,    87,     0,     0,     0,    88,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,    30,    31,    32,     0,     0,     0,
       0,    37,    38,     0,     0,     0,     4,     0,     0,    93,
       0,    85,    86,    87,    47,     0,    48,    88,     0,     0,
       0,     0,     0,     0,     0,    50,   191,     0,    51,    10,
      11,    12,    13,    14,    15,    16,    17,    89,    90,    91,
      92,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,    30,    31,    32,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     4,     0,     0,    93,     0,    85,    86,
      87,    47,     0,    48,    88,     0,     0,     0,     0,     0,
       0,     0,    50,   194,     0,    51,     0,    10,    11,    12,
      13,    14,    15,    16,    17,    89,    90,    91,    92,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,    30,
      31,    32,     0,     0,     0,     0,    37,    38,     0,     0,
       0,     4,     0,     0,    93,     0,    85,    86,    87,    47,
       0,    48,    88,     0,     0,     0,     0,     0,     0,     0,
      50,   214,     0,    51,    10,    11,    12,    13,    14,    15,
      16,    17,    89,    90,    91,    92,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,    30,    31,    32,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     4,     0,
       0,    93,     0,     5,   135,    87,    47,     0,    48,     8,
       0,     0,     0,     0,     0,     0,     0,    50,   221,     0,
      51,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,    30,    31,    32,     0,     0,     0,
       0,    37,    38,     0,     0,     0,     4,     0,     0,    93,
       0,    85,    86,    87,    47,     0,    48,    88,     0,     0,
     150,   151,     0,     0,   152,    50,   153,   154,    51,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,   150,   151,     0,     0,   152,     0,   153,   154,    29,
       0,    30,    31,    32,     0,     0,   150,   151,    37,    38,
     152,     0,   153,   154,     0,     0,    93,     0,   150,   151,
       0,    47,   152,    48,   153,   154,     0,     0,     0,     0,
       0,     0,    50,     0,     0,    51,     0,    10,    11,    12,
      13,    14,    15,    16,    17,    89,    90,    91,    92,   150,
     151,     0,     0,   152,     0,   153,   154,    29,     0,    30,
      31,    32,     0,     0,   150,   151,    37,    38,   152,     0,
     153,   154,     0,     0,    93,     0,   150,   151,     0,    47,
     152,    48,   153,   154,     0,     0,     0,     0,     0,     0,
     176,     0,     0,    51,     0,   155,     0,   156,   157,   158,
     159,   284,   160,   285,     0,   161,   162,   150,   151,     0,
       0,   152,     0,   153,   154,     0,   155,     0,   156,   157,
     158,   159,     0,   160,   253,     0,   161,   162,     0,     0,
       0,   155,     0,   156,   157,   158,   159,     0,   160,   254,
       0,   161,   162,   155,     0,   156,   157,   158,   159,     0,
     160,   259,     0,   161,   162,   150,   151,     0,     0,   152,
       0,   153,   154,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   155,     0,   156,   157,   158,   159,
       0,   160,   291,     0,   161,   162,     0,     0,     0,   155,
       0,   156,   157,   158,   159,   294,   160,     0,     0,   161,
     162,   155,     0,   156,   157,   158,   159,     0,   160,   316,
       0,   161,   162,   150,   151,     0,     0,   152,     0,   153,
     154,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   155,     0,   156,   157,   158,   159,     0,   160,
     320,     0,   161,   162,   150,   151,     0,     0,   152,     0,
     153,   154,     0,     0,     0,     0,   150,   151,    86,     0,
     152,     0,   153,   154,     0,     0,     0,     0,   150,   151,
       0,     0,   152,     0,   153,   154,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,     0,   160,   323,     0,
     161,   162,   150,   151,     0,     0,   152,     0,   153,   154,
       0,     0,     0,     0,    -1,   151,     0,     0,   152,     0,
     153,   154,     0,     0,     0,     0,     0,   151,     0,     0,
     152,     0,   153,   154,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   155,     0,
     156,   157,   158,   159,     0,   160,   325,     0,   161,   162,
       0,     0,    86,    29,     0,    30,    31,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   155,
      93,   156,   157,   158,   159,   338,   160,    48,     0,   161,
     162,   155,     0,   156,   157,   158,   159,   356,   160,    51,
       0,   161,   162,   155,     0,   156,   157,   158,   159,   362,
     160,    86,     0,   161,   162,   198,   199,   200,     0,     0,
       0,   201,    86,     0,     0,     0,     0,   155,     0,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,   155,
       0,   156,   157,   158,   159,     0,   160,     0,     0,   161,
     162,   155,     0,   156,   157,   158,   159,    29,   160,    30,
      31,    32,   162,    86,     0,     0,     0,     0,    86,     0,
       0,     0,     0,    86,    93,     0,     0,     0,     0,     0,
       0,    48,     0,     0,     0,     0,     0,     0,     0,     0,
      95,    86,     0,    51,     0,     0,     0,     0,     0,   202,
     203,   204,   205,     0,     0,     0,    29,     0,    30,    31,
      32,    29,     0,    30,    31,    32,     0,    29,     0,    30,
      31,    32,     0,    93,     0,     0,     0,     0,    93,     0,
      48,     0,     0,     0,    93,    48,     0,     0,     0,    97,
       0,    48,    51,     0,     0,   206,     0,    51,     0,     0,
     101,     0,     0,    51,     0,     0,     0,     0,    29,     0,
      30,    31,    32,    29,     0,    30,    31,    32,    29,     0,
      30,    31,    32,     0,     0,    93,     0,     0,     0,     0,
      93,     0,    48,     0,     0,    93,    29,    48,    30,    31,
      32,   111,    48,     0,    51,     0,   113,     0,     0,    51,
       0,   115,     0,    93,    51,     0,     0,     0,     0,     0,
      48,     0,     0,     0,     0,     0,     0,     0,     0,   117,
       0,     0,    51
};

static const short int yycheck[] =
{
       1,   241,   112,    11,     7,   109,   344,    10,   248,   134,
     137,   136,     3,     4,   127,   109,     7,     7,     9,    10,
      10,   142,    23,   136,     4,   142,     5,     6,    11,     8,
     368,   136,   142,   137,    11,   127,   136,   110,   143,    18,
      19,    20,    21,   143,   136,   137,    25,    48,    28,   136,
     136,   136,   136,   136,   142,   136,   143,   143,   143,   143,
     143,    62,   143,   110,    44,   142,    11,    47,   115,   109,
     110,    51,   109,   110,   142,   315,   142,     3,     4,   142,
      60,     7,   142,     9,    10,   142,   143,    45,    46,    11,
     142,   142,   142,    72,   142,    75,     3,     4,   109,   142,
       7,   142,     9,    10,   109,   109,   137,   136,   142,   110,
     142,   110,   113,   136,   142,    95,   117,    97,   142,   127,
     100,   101,   142,   103,   104,   105,   106,   107,   108,   109,
     133,   111,   135,   134,   142,   115,   144,   128,   109,   130,
     131,   132,   133,   133,   135,   146,   125,   138,   139,   142,
     142,   137,   132,   136,   145,   137,   135,   110,   136,   142,
     143,   144,   136,   110,   137,   142,   145,   144,   147,   149,
     150,   151,   109,   153,   143,   155,   156,   157,   158,   159,
     160,   161,   162,   143,   143,     3,     4,   166,   133,     7,
     169,     9,    10,   173,   143,   143,   176,   142,   144,   144,
     180,   127,   128,   143,   130,   131,   132,   133,   143,   135,
     110,   133,   138,   139,   133,   136,   136,   143,   133,   143,
     142,   128,   144,   130,   131,   132,   133,   143,   135,   136,
     210,   138,   139,   143,   143,   143,   143,   143,   143,   143,
     136,   112,   143,   142,   136,   143,    13,    14,    15,     6,
     286,    44,    19,   368,    -1,   354,   236,   170,    -1,    -1,
      -1,   241,   242,    -1,    -1,    -1,    -1,    -1,   248,   249,
       0,     1,    -1,   253,   254,   255,    -1,   257,     8,   259,
     260,    -1,    -1,    13,    14,    15,    -1,    -1,    -1,    19,
     291,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,   285,    -1,    -1,    -1,    39,
     128,    -1,   130,   131,   132,   133,     7,   135,   136,    10,
     138,   139,     3,     4,    -1,   143,     7,    -1,     9,    10,
      97,    98,    99,   100,    -1,   315,   316,   317,    -1,    -1,
     320,   321,    -1,   323,     7,   325,   326,    10,   328,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,   359,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,    -1,   106,   107,   108,   109,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,    -1,   128,    -1,
      -1,   131,    -1,   133,     3,     4,    -1,   137,     7,    -1,
       9,    10,   142,    -1,    -1,   145,   128,    -1,   130,   131,
     132,   133,    -1,   135,   136,    -1,   138,   139,     3,     4,
      -1,   143,     7,    -1,     9,    10,    -1,   128,    -1,   130,
     131,   132,   133,    -1,   135,    -1,    -1,   128,   139,   130,
     131,   132,   133,     7,   135,   136,    10,   138,   139,     3,
       4,    -1,   143,     7,    -1,     9,    10,    -1,    -1,   132,
     133,    -1,   135,    -1,   128,    -1,   130,   131,   132,   133,
      -1,   135,   136,    -1,   138,   139,     3,     4,    -1,   143,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,   130,   131,   132,   133,    -1,   135,   136,    -1,   138,
     139,     3,     4,    -1,   143,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,
     135,   136,    -1,   138,   139,     3,     4,    -1,   143,     7,
      -1,     9,    10,    -1,    -1,    -1,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   128,   139,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   138,   139,     3,     4,    -1,   143,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,   130,   131,   132,   133,    -1,   135,    -1,
      -1,   138,   139,    -1,    -1,   128,   143,   130,   131,   132,
     133,    -1,   135,    -1,    -1,   138,   139,    -1,    -1,   128,
     143,   130,   131,   132,   133,    -1,   135,    -1,    -1,   138,
     139,     3,     4,    -1,   143,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,   130,   131,
     132,   133,    -1,   135,    -1,    -1,   138,   139,     3,     4,
      -1,   143,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,    -1,   135,    -1,    -1,
     138,   139,     3,     4,    -1,   143,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,   130,   131,   132,   133,    -1,   135,    -1,
      -1,   138,   139,     3,     4,    -1,   143,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,   130,   131,
     132,   133,    -1,   135,    -1,    -1,   138,   139,     3,     4,
      -1,   143,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,
     135,    -1,    -1,   138,   139,     3,     4,    -1,   143,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,   128,    -1,   130,
     131,   132,   133,    -1,   135,    -1,    -1,   138,   139,     3,
       4,    -1,   143,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,    -1,   135,    -1,    -1,   138,   139,
      -1,    -1,   128,   143,   130,   131,   132,   133,    -1,   135,
      -1,    -1,   138,   139,    -1,    -1,   128,   143,   130,   131,
     132,   133,    -1,   135,    -1,    -1,   138,   139,    -1,     8,
      -1,   143,    -1,    -1,    13,    14,    15,    -1,    -1,    -1,
      19,    -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,
     135,    -1,    -1,   138,   139,     3,     4,    -1,   143,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,    -1,   135,    -1,    -1,
     138,   139,    -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   138,   139,    -1,    -1,    -1,   143,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,     8,   111,   112,   113,    -1,    13,    14,    15,   118,
     119,    -1,    19,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,    -1,   131,    -1,   133,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   142,   143,    -1,   145,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,   134,   135,     8,    -1,
     138,   139,    -1,    13,    14,    15,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    -1,   111,   112,   113,    -1,    -1,    -1,
      -1,   118,   119,    -1,    -1,    -1,     8,    -1,    -1,   126,
      -1,    13,    14,    15,   131,    -1,   133,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   142,   143,    -1,   145,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
      -1,   111,   112,   113,    -1,    -1,    -1,    -1,   118,   119,
      -1,    -1,    -1,     8,    -1,    -1,   126,    -1,    13,    14,
      15,   131,    -1,   133,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   142,   143,    -1,   145,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,    -1,   111,
     112,   113,    -1,    -1,    -1,    -1,   118,   119,    -1,    -1,
      -1,     8,    -1,    -1,   126,    -1,    13,    14,    15,   131,
      -1,   133,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     142,   143,    -1,   145,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,    -1,   111,   112,   113,    -1,
      -1,    -1,    -1,   118,   119,    -1,    -1,    -1,     8,    -1,
      -1,   126,    -1,    13,    14,    15,   131,    -1,   133,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,    -1,
     145,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    -1,   111,   112,   113,    -1,    -1,    -1,
      -1,   118,   119,    -1,    -1,    -1,     8,    -1,    -1,   126,
      -1,    13,    14,    15,   131,    -1,   133,    19,    -1,    -1,
       3,     4,    -1,    -1,     7,   142,     9,    10,   145,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,     3,     4,    -1,    -1,     7,    -1,     9,    10,   109,
      -1,   111,   112,   113,    -1,    -1,     3,     4,   118,   119,
       7,    -1,     9,    10,    -1,    -1,   126,    -1,     3,     4,
      -1,   131,     7,   133,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,   142,    -1,    -1,   145,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,     3,
       4,    -1,    -1,     7,    -1,     9,    10,   109,    -1,   111,
     112,   113,    -1,    -1,     3,     4,   118,   119,     7,    -1,
       9,    10,    -1,    -1,   126,    -1,     3,     4,    -1,   131,
       7,   133,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
     142,    -1,    -1,   145,    -1,   128,    -1,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   139,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,   128,    -1,   130,   131,
     132,   133,    -1,   135,   136,    -1,   138,   139,    -1,    -1,
      -1,   128,    -1,   130,   131,   132,   133,    -1,   135,   136,
      -1,   138,   139,   128,    -1,   130,   131,   132,   133,    -1,
     135,   136,    -1,   138,   139,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,
      -1,   135,   136,    -1,   138,   139,    -1,    -1,    -1,   128,
      -1,   130,   131,   132,   133,   134,   135,    -1,    -1,   138,
     139,   128,    -1,   130,   131,   132,   133,    -1,   135,   136,
      -1,   138,   139,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,   135,
     136,    -1,   138,   139,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,     3,     4,    14,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,    -1,   135,   136,    -1,
     138,   139,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,    -1,   135,   136,    -1,   138,   139,
      -1,    -1,    14,   109,    -1,   111,   112,   113,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,
     126,   130,   131,   132,   133,   134,   135,   133,    -1,   138,
     139,   128,    -1,   130,   131,   132,   133,   134,   135,   145,
      -1,   138,   139,   128,    -1,   130,   131,   132,   133,   134,
     135,    14,    -1,   138,   139,    13,    14,    15,    -1,    -1,
      -1,    19,    14,    -1,    -1,    -1,    -1,   128,    -1,   130,
     131,   132,   133,    -1,   135,    -1,    -1,   138,   139,   128,
      -1,   130,   131,   132,   133,    -1,   135,    -1,    -1,   138,
     139,   128,    -1,   130,   131,   132,   133,   109,   135,   111,
     112,   113,   139,    14,    -1,    -1,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    14,   126,    -1,    -1,    -1,    -1,    -1,
      -1,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     142,    14,    -1,   145,    -1,    -1,    -1,    -1,    -1,    97,
      98,    99,   100,    -1,    -1,    -1,   109,    -1,   111,   112,
     113,   109,    -1,   111,   112,   113,    -1,   109,    -1,   111,
     112,   113,    -1,   126,    -1,    -1,    -1,    -1,   126,    -1,
     133,    -1,    -1,    -1,   126,   133,    -1,    -1,    -1,   142,
      -1,   133,   145,    -1,    -1,   143,    -1,   145,    -1,    -1,
     142,    -1,    -1,   145,    -1,    -1,    -1,    -1,   109,    -1,
     111,   112,   113,   109,    -1,   111,   112,   113,   109,    -1,
     111,   112,   113,    -1,    -1,   126,    -1,    -1,    -1,    -1,
     126,    -1,   133,    -1,    -1,   126,   109,   133,   111,   112,
     113,   142,   133,    -1,   145,    -1,   142,    -1,    -1,   145,
      -1,   142,    -1,   126,   145,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,
      -1,    -1,   145
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   147,     0,     1,     8,    13,    14,    15,    19,    39,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   106,   107,   108,   109,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   128,   131,   133,   137,
     142,   145,   148,   149,   150,   151,   152,   153,   154,   155,
     157,   159,   160,   161,   162,   163,   170,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   137,    13,    14,    15,    19,    97,
      98,    99,   100,   126,   155,   142,   153,   142,   153,   161,
     142,   142,   153,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   153,   142,   153,   142,   153,   142,   153,   109,
     110,   154,   109,   137,   153,   142,   155,   109,   110,   142,
     142,   109,   142,   109,   142,    14,   155,   162,   163,   163,
     155,   154,   154,   155,   137,    11,   142,   144,   127,   136,
       3,     4,     7,     9,    10,   128,   130,   131,   132,   133,
     135,   138,   139,   155,   154,   127,   136,   137,   169,   136,
     153,   155,   155,   133,   155,   110,   142,   155,   164,   155,
     133,   155,   155,   155,   155,   155,   155,   155,   143,   154,
     155,   143,   154,   155,   143,   154,   137,   137,    13,    14,
      15,    19,    97,    98,    99,   100,   143,   153,   109,   110,
     156,   109,   155,   110,   143,   154,   171,   134,   143,   145,
     153,   143,   154,   153,   155,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   127,   143,   158,   153,
     153,   169,   136,   143,   155,   143,   155,   143,   136,   136,
     143,   155,   143,   136,   136,   136,   143,   136,   143,   136,
     136,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   136,   143,   110,   155,   109,
     143,   143,   137,   143,   134,   136,   155,   155,   164,   155,
     134,   136,   164,   155,   134,   155,   155,   155,   155,   155,
     155,    13,    14,    15,    19,    97,    98,    99,   100,   143,
     110,   110,   115,   155,   158,   136,   136,   133,   154,   136,
     136,   133,   143,   136,   143,   136,   136,   143,   136,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   134,   164,
     155,   155,   143,   112,   142,   165,   166,   168,   155,   155,
     155,   155,   155,   155,   136,   143,   134,   166,   167,   142,
     143,   143,   134,   143,   143,   143,   143,   168,   136,   143,
     154,   167,   143
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
#line 363 "grammar.y"
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
#line 398 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:
#line 400 "grammar.y"
    { yyvsp[-1].lv.CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:
#line 402 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:
#line 406 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:
#line 411 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:
#line 413 "grammar.y"
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
#line 471 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:
#line 474 "grammar.y"
    { omFree((ADDRESS)yyvsp[0].name); ;}
    break;

  case 29:
#line 489 "grammar.y"
    {
            if(iiAssign(&yyvsp[-1].lv,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 30:
#line 496 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&yyval.lv,omStrDup(yyvsp[0].name));
          ;}
    break;

  case 31:
#line 501 "grammar.y"
    {
            syMake(&yyval.lv,yyvsp[0].name);
          ;}
    break;

  case 32:
#line 505 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv, &yyvsp[-2].lv, COLONCOLON, &yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 33:
#line 509 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv, &yyvsp[-2].lv, '.', &yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 34:
#line 513 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-2].lv,'(')) YYERROR;
          ;}
    break;

  case 35:
#line 517 "grammar.y"
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
#line 530 "grammar.y"
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
#line 558 "grammar.y"
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
                n_Read(yyvsp[0].name,&n,coeffs_BIGINT);
                yyval.lv.rtyp=BIGINT_CMD;
                yyval.lv.data = n;
              }
            }
          ;}
    break;

  case 38:
#line 582 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            yyval.lv.rtyp = yyvsp[0].i;
            yyval.lv.data = yyval.lv.Data();
          ;}
    break;

  case 39:
#line 588 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            yyval.lv.rtyp  = STRING_CMD;
            yyval.lv.data = yyvsp[0].name;
          ;}
    break;

  case 40:
#line 594 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 41:
#line 601 "grammar.y"
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
#line 612 "grammar.y"
    {
            yyval.lv = yyvsp[0].lv;
          ;}
    break;

  case 43:
#line 618 "grammar.y"
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            yyval.lv = yyvsp[0].lv;
          ;}
    break;

  case 44:
#line 622 "grammar.y"
    { yyval.lv = yyvsp[0].lv; ;}
    break;

  case 45:
#line 623 "grammar.y"
    { yyval.lv = yyvsp[-1].lv; ;}
    break;

  case 46:
#line 625 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,'[',&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 47:
#line 629 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,'[',&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 48:
#line 633 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 49:
#line 637 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 50:
#line 641 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 51:
#line 645 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 52:
#line 649 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 53:
#line 653 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 54:
#line 657 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 55:
#line 661 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 56:
#line 665 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 57:
#line 669 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 58:
#line 673 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 59:
#line 677 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 60:
#line 681 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 61:
#line 685 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 62:
#line 689 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 63:
#line 693 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-3].lv,yyvsp[-5].i,&yyvsp[-1].lv,TRUE)) YYERROR;
          ;}
    break;

  case 64:
#line 697 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 65:
#line 701 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,yyvsp[-7].i,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 66:
#line 705 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,NULL,yyvsp[-2].i)) YYERROR;
          ;}
    break;

  case 67:
#line 709 "grammar.y"
    {
            if(iiExprArithM(&yyval.lv,&yyvsp[-1].lv,yyvsp[-3].i)) YYERROR;
          ;}
    break;

  case 68:
#line 713 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,MATRIX_CMD,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 69:
#line 717 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,MATRIX_CMD)) YYERROR;
          ;}
    break;

  case 70:
#line 721 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,INTMAT_CMD,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 71:
#line 725 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,INTMAT_CMD)) YYERROR;
          ;}
    break;

  case 72:
#line 729 "grammar.y"
    {
            if(iiExprArith3(&yyval.lv,RING_CMD,&yyvsp[-5].lv,&yyvsp[-3].lv,&yyvsp[-1].lv)) YYERROR;
          ;}
    break;

  case 73:
#line 733 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,RING_CMD)) YYERROR;
          ;}
    break;

  case 74:
#line 737 "grammar.y"
    {
            yyval.lv=yyvsp[-1].lv;
          ;}
    break;

  case 75:
#line 741 "grammar.y"
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

  case 76:
#line 758 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 77:
#line 764 "grammar.y"
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

  case 78:
#line 776 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 79:
#line 784 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 80:
#line 793 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,PLUSPLUS)) YYERROR;
          ;}
    break;

  case 81:
#line 797 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[-1].lv,MINUSMINUS)) YYERROR;
          ;}
    break;

  case 82:
#line 801 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'+',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 83:
#line 805 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'-',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 84:
#line 809 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 85:
#line 813 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,'^',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 86:
#line 817 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 87:
#line 821 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,yyvsp[-1].i,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 88:
#line 825 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,NOTEQUAL,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 89:
#line 829 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,EQUAL_EQUAL,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 90:
#line 833 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,DOTDOT,&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 91:
#line 837 "grammar.y"
    {
            if(iiExprArith2(&yyval.lv,&yyvsp[-2].lv,':',&yyvsp[0].lv)) YYERROR;
          ;}
    break;

  case 92:
#line 841 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            int i; TESTSETINT(yyvsp[0].lv,i);
            yyval.lv.rtyp  = INT_CMD;
            yyval.lv.data = (void *)(long)(i == 0 ? 1 : 0);
          ;}
    break;

  case 93:
#line 848 "grammar.y"
    {
            if(iiExprArith1(&yyval.lv,&yyvsp[0].lv,'-')) YYERROR;
          ;}
    break;

  case 94:
#line 854 "grammar.y"
    { yyval.lv = yyvsp[-1].lv; ;}
    break;

  case 95:
#line 856 "grammar.y"
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

  case 97:
#line 876 "grammar.y"
    {
            if (yyvsp[-1].lv.Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            yyval.name = (char *)yyvsp[-1].lv.CopyD(STRING_CMD);
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 98:
#line 888 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 99:
#line 893 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 100:
#line 898 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 101:
#line 902 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 102:
#line 906 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[-6].lv,myynest,yyvsp[-7].i,&(currRing->idroot), TRUE)) YYERROR;
            int r; TESTSETINT(yyvsp[-4].lv,r);
            int c; TESTSETINT(yyvsp[-1].lv,c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            leftv v=&yyval.lv;
            //while (v->next!=NULL) { v=v->next; }
            idhdl h=(idhdl)v->data;
            idDelete(&IDIDEAL(h));
            IDMATRIX(h) = mpNew(r,c);
            if (IDMATRIX(h)==NULL) YYERROR;
          ;}
    break;

  case 103:
#line 922 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 104:
#line 926 "grammar.y"
    {
            int r; TESTSETINT(yyvsp[-4].lv,r);
            int c; TESTSETINT(yyvsp[-1].lv,c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            if (iiDeclCommand(&yyval.lv,&yyvsp[-6].lv,myynest,yyvsp[-7].i,&(yyvsp[-6].lv.req_packhdl->idroot)))
              YYERROR;
            leftv v=&yyval.lv;
            idhdl h=(idhdl)v->data;
            delete IDINTVEC(h);
            IDINTVEC(h) = new intvec(r,c,0);
            if (IDINTVEC(h)==NULL) YYERROR;
          ;}
    break;

  case 105:
#line 942 "grammar.y"
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
          ;}
    break;

  case 106:
#line 956 "grammar.y"
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

  case 107:
#line 977 "grammar.y"
    {
            if (iiDeclCommand(&yyval.lv,&yyvsp[0].lv,myynest,yyvsp[-1].i,&(yyvsp[0].lv.req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 110:
#line 990 "grammar.y"
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

  case 111:
#line 1004 "grammar.y"
    {
          // let rInit take care of any errors
          yyval.i=rOrderName(yyvsp[0].name);
        ;}
    break;

  case 112:
#line 1012 "grammar.y"
    {
            memset(&yyval.lv,0,sizeof(yyval.lv));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = yyvsp[0].i;
            yyval.lv.rtyp = INTVEC_CMD;
            yyval.lv.data = (void *)iv;
          ;}
    break;

  case 113:
#line 1021 "grammar.y"
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

  case 115:
#line 1067 "grammar.y"
    {
            yyval.lv = yyvsp[-2].lv;
            yyval.lv.next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy(yyval.lv.next,&yyvsp[0].lv,sizeof(sleftv));
          ;}
    break;

  case 117:
#line 1077 "grammar.y"
    {
            yyval.lv = yyvsp[-1].lv;
          ;}
    break;

  case 118:
#line 1083 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 119:
#line 1095 "grammar.y"
    { if (yyvsp[-1].i != '<') YYERROR;
            if((feFilePending=feFopen(yyvsp[0].name,"r",NULL,TRUE))==NULL) YYERROR; ;}
    break;

  case 120:
#line 1098 "grammar.y"
    { newFile(yyvsp[-2].name,feFilePending); ;}
    break;

  case 121:
#line 1103 "grammar.y"
    {
            feHelp(yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[-1].name);
          ;}
    break;

  case 122:
#line 1108 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 123:
#line 1115 "grammar.y"
    {
            singular_example(yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[-1].name);
          ;}
    break;

  case 124:
#line 1123 "grammar.y"
    {
          if (basePack!=yyvsp[0].lv.req_packhdl)
          {
            if(iiExport(&yyvsp[0].lv,0,currPackHdl)) YYERROR;
          }
          else
            if (iiExport(&yyvsp[0].lv,0)) YYERROR;
        ;}
    break;

  case 125:
#line 1135 "grammar.y"
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
#line 1151 "grammar.y"
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

  case 127:
#line 1170 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 128:
#line 1174 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 129:
#line 1178 "grammar.y"
    {
            if (yyvsp[-1].i==QRING_CMD) yyvsp[-1].i=RING_CMD;
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 130:
#line 1183 "grammar.y"
    {
            list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
          ;}
    break;

  case 131:
#line 1187 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 132:
#line 1191 "grammar.y"
    {
            list_cmd(MATRIX_CMD,NULL,"// ",TRUE);
           ;}
    break;

  case 133:
#line 1195 "grammar.y"
    {
            list_cmd(INTMAT_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 134:
#line 1199 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 135:
#line 1203 "grammar.y"
    {
            list_cmd(0,yyvsp[-1].lv.Fullname(),"// ",TRUE);
            yyvsp[-1].lv.CleanUp();
          ;}
    break;

  case 136:
#line 1208 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 137:
#line 1214 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 138:
#line 1220 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 139:
#line 1226 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 140:
#line 1232 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 141:
#line 1238 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 142:
#line 1244 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 143:
#line 1250 "grammar.y"
    {
            if(yyvsp[-3].lv.Typ() == PACKAGE_CMD)
              list_cmd(yyvsp[-1].i,NULL,"// ",TRUE);
            yyvsp[-3].lv.CleanUp();
          ;}
    break;

  case 144:
#line 1262 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 145:
#line 1268 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 146:
#line 1277 "grammar.y"
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

  case 147:
#line 1310 "grammar.y"
    {
            const char *ring_name = yyvsp[0].lv.name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            yyvsp[0].lv.CleanUp();
          ;}
    break;

  case 148:
#line 1320 "grammar.y"
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

  case 151:
#line 1336 "grammar.y"
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

  case 152:
#line 1408 "grammar.y"
    {
            type_cmd(&(yyvsp[0].lv));
          ;}
    break;

  case 153:
#line 1412 "grammar.y"
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

  case 154:
#line 1441 "grammar.y"
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

  case 155:
#line 1454 "grammar.y"
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

  case 156:
#line 1471 "grammar.y"
    {
            int i; TESTSETINT(yyvsp[-2].lv,i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 157:
#line 1480 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 158:
#line 1485 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 159:
#line 1493 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen(yyvsp[-1].name) + strlen(yyvsp[0].name) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,yyvsp[-1].name,yyvsp[0].name);
            newBuffer(s,BT_break);
            omFree((ADDRESS)yyvsp[-1].name);
            omFree((ADDRESS)yyvsp[0].name);
          ;}
    break;

  case 160:
#line 1505 "grammar.y"
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

  case 161:
#line 1524 "grammar.y"
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

  case 162:
#line 1535 "grammar.y"
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

  case 163:
#line 1555 "grammar.y"
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

  case 164:
#line 1579 "grammar.y"
    {
	    // decl. of type proc p(int i)
            if (yyvsp[-1].i==PARAMETER)  { if (iiParameter(&yyvsp[0].lv)) YYERROR; }
	    else                { if (iiAlias(&yyvsp[0].lv)) YYERROR; } 
          ;}
    break;

  case 165:
#line 1585 "grammar.y"
    {
	    // decl. of type proc p(i)
            sleftv tmp_expr;
	    if (yyvsp[-1].i==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&yyvsp[0].lv,myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 166:
#line 1597 "grammar.y"
    {
            if(iiRETURNEXPR==NULL) YYERROR;
            iiRETURNEXPR[myynest].Copy(&yyvsp[-1].lv);
            yyvsp[-1].lv.CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 167:
#line 1604 "grammar.y"
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
#line 3621 "grammar.cc"

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



